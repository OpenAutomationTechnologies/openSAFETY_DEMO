/**
********************************************************************************
\file   demo-sn-gpio/sapl/sapl.c

\defgroup module_sn_sapl_sapl Main module
\{

\brief  Safe application managing module

This module manages the application near firmware parts. It's main purpose is
to trigger all SAPL submodules in a way to be as less time consuming as possible.
Therefore the function sapl_processSync implements a task scheduler which enables
to process any of the needed subtask of the SAPL. All subtasks are written in
a way so that they are carried out in small pieces where the openSAFETY stack
get's time to do some calculation between each piece.

\ingroup group_app_sn_sapl
*******************************************************************************/

/*------------------------------------------------------------------------------
* License Agreement
*
* Copyright (c) 2014, B&R Industrial Automation GmbH
* All rights reserved.
*
* Redistribution and use in source and binary forms,
* with or without modification,
* are permitted provided that the following conditions are met:
*
*   * Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer
*     in the documentation and/or other materials provided with the
*     distribution.
*   * Neither the name of the B&R nor the names of its contributors
*     may be used to endorse or promote products derived from this software
*     without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/
#include <sapl/sapl.h>

#include <shnf/shnf.h>

#include <sn/statehandler.h>
#include <sn/gpio.h>

#include <sapl/parameterset.h>
#include <sapl/parametercrc.h>
#include <sapl/sodstore.h>
#include <sapl/app.h>

#include <SODapi.h>
#include <SERRapi.h>
#include <SNMTSapi.h>

#include <oschecksum/crc.h>

/*============================================================================*/
/*            G L O B A L   D E F I N I T I O N S                             */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* module global vars                                                         */
/*----------------------------------------------------------------------------*/

/**
 * @var SHNF_aaulConnValidBit
 * @brief This variable contains the connection valid bit of the SPDOs.
 *
 * It is a bit field which contains one bit each RxSPDO. It is to be hosted by the SHNF.
 */
extern UINT32 SHNF_aaulConnValidBit[EPLS_cfg_MAX_INSTANCES][(SPDO_cfg_MAX_NO_RX_SPDO + 31) / 32];

/*----------------------------------------------------------------------------*/
/* global function prototypes                                                 */
/*----------------------------------------------------------------------------*/


/*============================================================================*/
/*            P R I V A T E   D E F I N I T I O N S                           */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/
#define SAPL_TASK_PROCESS_PARAM_SET_PARSE_BIT         0x00
#define SAPL_TASK_PROCESS_PARAM_SET_PARSE_MASK        0x01

#define SAPL_TASK_PROCESS_PARAM_SET_CRC_BIT           0x01
#define SAPL_TASK_PROCESS_PARAM_SET_CRC_MASK          0x02

#define SAPL_TASK_PROCESS_PARAM_SET_STORE_BIT         0x02
#define SAPL_TASK_PROCESS_PARAM_SET_STORE_MASK        0x04


/*----------------------------------------------------------------------------*/
/* local types                                                                */
/*----------------------------------------------------------------------------*/

/**
 * \brief Details of the incoming parameter set
 */
typedef struct
{
    void * pData_m;         /**< Pointer to the start of the parameter set */
    UINT32 length_m;        /**< Actual length of the parameter set */
} tParamSetAttr;

/**
 * \brief SAPL instance structure
 */
typedef struct
{
    UINT32 activeTasks_m;             /**< Stores which task of the SAPL is currently active */
    UINT32 lastTask_m;                /**< Stores the last active task in the SAPL (Debug only!) */
    BOOLEAN fParamCrcValid_m;         /**< TRUE if the parameter CRC is valid */
    tParamSetAttr paramSetAttr_m;     /**< Attributes of the SOD parameter set */
} tSaplInstance;

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/
static tSaplInstance saplInstance_l SAFE_INIT_SEKTOR;

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static BOOLEAN getConnValidInst0(UINT16 spdoId_p);
static void checkConnectionValid(void);

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize safe application main modules

\retval TRUE    Initialization was successful
\retval FALSE   Error during initialization
*/
/*----------------------------------------------------------------------------*/
BOOLEAN sapl_init(void)
{
    BOOLEAN fReturn = FALSE;

    MEMSET(&saplInstance_l, 0, sizeof(tSaplInstance));

    /* Initialize the parameter set module */
    if(paramset_init())
    {
        /* Initialize the SOD CRC calculation module */
        if(paramcrc_init())
        {
            /* Initialize the SOD storage module */
            if(sodstore_init())
            {
                /* Initialize target specific gpio pins */
                gpio_init();

                /* Initialize the SN application */
                if(app_init(getConnValidInst0))
                {
                    fReturn = TRUE;
                }   /* no else: Error is handled in the called function */
            }   /* no else: Error is handled in the called function */
        }   /* no else: Error is handled in the called function */
    }   /* no else: Error is handled in the called function */

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Shutdown the SAPL module
*/
/*----------------------------------------------------------------------------*/
void sapl_exit(void)
{
    paramset_exit();
    paramcrc_exit();

    sodstore_close();

    gpio_close();
    app_exit();
}

/*----------------------------------------------------------------------------*/
/**
\brief    Reset all SAPL internals

\note This function is called on a cycle time violation
*/
/*----------------------------------------------------------------------------*/
void sapl_reset(void)
{
    app_reset();
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process the SAPL background task

Process all tasks which are independent of the openSAFETY stack in the
background loop. (Shall not increment the flow counter)

\retval TRUE    Processing of the SAPL successful
\retval FALSE   Error during processing
*/
/*----------------------------------------------------------------------------*/
BOOLEAN sapl_process(void)
{
    BOOLEAN fReturn = FALSE;
    tProcStoreRet sodStoreRet;

    if((saplInstance_l.activeTasks_m & SAPL_TASK_PROCESS_PARAM_SET_PARSE_MASK) != 0)
    {
        if(saplInstance_l.lastTask_m != saplInstance_l.activeTasks_m)
        {
            DEBUG_TRACE(DEBUG_LVL_ALWAYS, "\nParse ParameterSet ...\n");
            saplInstance_l.lastTask_m = saplInstance_l.activeTasks_m;
        }

        fReturn = TRUE;
    }
    else if((saplInstance_l.activeTasks_m & SAPL_TASK_PROCESS_PARAM_SET_CRC_MASK) != 0)
    {
        if(saplInstance_l.lastTask_m != saplInstance_l.activeTasks_m)
        {
            DEBUG_TRACE(DEBUG_LVL_ALWAYS, "\nCalc CRC ...\n");
            saplInstance_l.lastTask_m = saplInstance_l.activeTasks_m;
        }

        fReturn = TRUE;
    }
    else if((saplInstance_l.activeTasks_m & SAPL_TASK_PROCESS_PARAM_SET_STORE_MASK) != 0)
    {
        if(saplInstance_l.lastTask_m != saplInstance_l.activeTasks_m)
        {
            DEBUG_TRACE(DEBUG_LVL_ALWAYS, "\nStore SOD -> ");
            saplInstance_l.lastTask_m = saplInstance_l.activeTasks_m;
        }

        /* Process the SOD store state machine */
        sodStoreRet = sodstore_process((UINT8*)saplInstance_l.paramSetAttr_m.pData_m,
                                       saplInstance_l.paramSetAttr_m.length_m);
        if(sodStoreRet == kSodStoreProcFinished)
        {
            /* Reset the flag of the parameter store task */
            saplInstance_l.activeTasks_m &= ~(1<<SAPL_TASK_PROCESS_PARAM_SET_STORE_BIT);

            DEBUG_TRACE(DEBUG_LVL_ALWAYS, "SUCCESS!\n");

            fReturn = TRUE;
        }
        else if(sodStoreRet == kSodStoreProcBusy)
        {
            /* Continue processing on next call of SAPL process */
            fReturn = TRUE;
        }
        else if(sodStoreRet == kSodStoreProcNotApplicable)
        {
            /* Reset the flag of the parameter store task */
            saplInstance_l.activeTasks_m &= ~(1<<SAPL_TASK_PROCESS_PARAM_SET_STORE_BIT);

            /* Unable to restore the SOD (NVM is not empty!) */
            DEBUG_TRACE(DEBUG_LVL_ALWAYS, "FAILED!\n");

            fReturn = TRUE;
        }
    }
    else
    {
        /* Nothing to process -> Success! */
        fReturn = TRUE;
    }

    /* Check if the connection valid bit is set */
    checkConnectionValid();

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process the SAPL synchronous task

All tasks of the SAPL which call stack functions need to be processed
in the synchronous IR. (Stack functions increment the flow counter)

\retval TRUE    Processing of the SAPL successful
\retval FALSE   Error during processing
*/
/*----------------------------------------------------------------------------*/
BOOLEAN sapl_processSync(void)
{
    BOOLEAN fReturn = FALSE;
    tProcParamRet paramProcRet;
    tProcCrcRet paramCrcRet;

    if((saplInstance_l.activeTasks_m & SAPL_TASK_PROCESS_PARAM_SET_PARSE_MASK) != 0)
    {
        /* Process the parameter parser state machine */
        paramProcRet = paramset_process((UINT8*)saplInstance_l.paramSetAttr_m.pData_m,
                                        saplInstance_l.paramSetAttr_m.length_m);
        if(paramProcRet == kParamProcFinished)
        {
            /* Reset the flag of the parameter parser */
            saplInstance_l.activeTasks_m &= ~(1<<SAPL_TASK_PROCESS_PARAM_SET_PARSE_BIT);
            /* Store the currently processed parameter set to the NVS */
            saplInstance_l.activeTasks_m |= (1<<SAPL_TASK_PROCESS_PARAM_SET_STORE_BIT);

            fReturn = TRUE;
        }
        else if(paramProcRet == kParamProcBusy)
        {
            /* Continue processing on next call of SAPL process */
            fReturn = TRUE;
        }
    }
    else if((saplInstance_l.activeTasks_m & SAPL_TASK_PROCESS_PARAM_SET_CRC_MASK) != 0)
    {
        /* Process calculation of the SOD checksum */
        paramCrcRet = paramcrc_process();
        if(paramCrcRet == kParamCrcProcCrcValid)
        {
            /* Tell stack if the received checksum matches the calculated checksum */
            SNMTS_PassParamChkSumValid(B_INSTNUM_ TRUE);
            fReturn = TRUE;
            saplInstance_l.fParamCrcValid_m = TRUE;

            /* Reset the flag of the CRC calculator */
            saplInstance_l.activeTasks_m &= ~(1<<SAPL_TASK_PROCESS_PARAM_SET_CRC_BIT);

            saplInstance_l.lastTask_m = 0;
        }
        else if(paramCrcRet == kParamCrcProcCrcInvalid)
        {
            /* Report invalid checksum to stack */
            SNMTS_PassParamChkSumValid(B_INSTNUM_ FALSE);
            fReturn = TRUE;

            /* Reset the flag of the CRC calculator */
            saplInstance_l.activeTasks_m &= ~(1<<SAPL_TASK_PROCESS_PARAM_SET_CRC_BIT);

            saplInstance_l.lastTask_m = 0;
        }
        else if(paramCrcRet == kParamCrcProcBusy)
        {
            /* Continue processing on next call of SAPL process */
            fReturn = TRUE;
        }
    }
    else
    {
        /* Nothing to process -> Success! */
        fReturn = TRUE;
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process the SAPL application task

This function processes the SAPL application task if the connection valid
bit of the SPDO is set.

\retval TRUE    Processing of the SAPL application successful
\retval FALSE   Error during processing
*/
/*----------------------------------------------------------------------------*/
BOOLEAN sapl_processApp(void)
{
    BOOLEAN fReturn = FALSE;

    /* Call the application process function */
    if(app_process())
    {
        fReturn = TRUE;
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Check if there is an SOD image present in the NVM

\retval TRUE    There is a valid SOD image in the storage
\retval FALSE   No image available in the storage
*/
/*----------------------------------------------------------------------------*/
BOOLEAN sapl_checkSodStorage(void)
{
    UINT8* pParamSetBase = NULL;
    UINT32 paramSetLen = 0;
    BOOLEAN fSodPresent = FALSE;

    if(sodstore_getSodImage(&pParamSetBase, &paramSetLen))
    {
        fSodPresent = TRUE;
    }

    return fSodPresent;
}


/*----------------------------------------------------------------------------*/
/**
\brief    Check if there is an SOD image in the NVS and restore it if so

\param[in] fRestoreSod_p    TRUE if the SOD needs to be restored

\retval TRUE    Restore successful or nothing to restore
\retval FALSE   Error during restore of the image
*/
/*----------------------------------------------------------------------------*/
BOOLEAN sapl_restoreSod(BOOLEAN fRestoreSod_p)
{
    BOOLEAN fReturn = FALSE;
    UINT8* pParamSetBase = NULL;
    UINT32 paramSetLen = 0;
    tProcParamRet procRet;

    if(sodstore_getSodImage(&pParamSetBase, &paramSetLen) &&
       fRestoreSod_p == TRUE                               )
    {
        DEBUG_TRACE(DEBUG_LVL_ALWAYS, "\nRestore SOD -> ");

        /* A valid SOD image is in the NVS -> Restore it with the parameter parser */
        do
        {
            procRet = paramset_process(pParamSetBase, paramSetLen);

        } while(procRet != kParamProcFinished || procRet == kParamProcError);

        if(procRet == kParamProcFinished)
        {
            DEBUG_TRACE(DEBUG_LVL_ALWAYS, "SUCCESS!\n");

            fReturn = TRUE;
        }
    }
    else
    {
        DEBUG_TRACE(DEBUG_LVL_ALWAYS, "\nUnable to restore SOD from NVM ...\n");

        /* No image available -> Continue normal bootup */
        if(sodstore_prepareStorage())
        {
            fReturn = TRUE;
        }
    }

    return fReturn;
}

/**
 *
 * The function is called by the openSAFETY Stack (unit SNMTS) to indicate that a SN state transition from
 * PRE-OPERATIONAL to OPERATIONAL was requested by the SCM. To be able to respond the requested state
 * transition the openSAFETY Application has to admit the transition  by calling the function SNMTS_EnterOpState().
 *
 * @attention To inform the SNMTS about the API confirmation of switching into state OPERATIONAL application
 * must call the API function SNMTS_EnterOpState(). This API function MUST NOT be called within this callback function.
 *
 * @param        b_instNum        instance number, valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 */
void SAPL_SNMTS_SwitchToOpReqClbk(BYTE_B_INSTNUM)
{
#if (EPLS_cfg_MAX_INSTANCES > 1)
    b_instNum = b_instNum; /* To avoid warnings */
#endif

    /*
     * Verify if the parameter checksum is indeed valid
     * and all SAPL tasks are finished!
     */
    if(saplInstance_l.fParamCrcValid_m == TRUE  &&
       saplInstance_l.activeTasks_m == 0         )
    {
        saplInstance_l.fParamCrcValid_m = FALSE;

        stateh_setEnterOpFlag(TRUE);
    }
}

/**
 *
 * The function is called by the openSAFETY Stack (unit SNMTS) in case of reception of SNMT Service "SN set to OPERATIONAL".
 * This callback function requests the application of the SN to calculate the checksum of the current SOD.
 *
 * @attention To pass the calculated parameter checksum to the SNMTS the application must call the API function
 * SNMTS_PassParamChkSumValid(). This API function MUST NOT be called within this callback function.
 *
 * @param       b_instNum         instance number, valid range:  0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 */
void SAPL_SNMTS_CalcParamChkSumClbk(BYTE_B_INSTNUM)
{
#if (EPLS_cfg_MAX_INSTANCES > 1)
    b_instNum = b_instNum; /* To avoid warnings */
#endif

    /* Start the CRC calculation in the background */
    paramcrc_startProcessing();

    /* Enable corresponding task in SAPL process */
    saplInstance_l.activeTasks_m |= (1<<SAPL_TASK_PROCESS_PARAM_SET_CRC_BIT);
}

/**
 * @brief This function is a callback function which is provided by the openSAFETY Application. The function is called by the
 * openSAFETY Stack (unit SNMTS) to check whether the "parameter set" downloaded by the SCM is already processed.
 *
 * @param        b_instNum        instance number, valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return
 * - TRUE           - "parameter set" is already processed
 * - FALSE          - "parameter set" is not processed
 */
BOOLEAN SAPL_SNMTS_ParameterSetProcessed(BYTE_B_INSTNUM)
{
    BOOLEAN procFin = FALSE;

#if (EPLS_cfg_MAX_INSTANCES > 1)
    UNUSED_PARAMETER(b_instNum);    /* to avoid warnings */
#endif

    if((saplInstance_l.activeTasks_m & SAPL_TASK_PROCESS_PARAM_SET_PARSE_MASK) == 0)
    {
        /* Indicate processing finished */
        procFin = TRUE;
    }

    return procFin;
}


/*
 * This function is called after the write access of the parameter
 * set object.Parameter set object has SOD index 0x101A
 * sub-index 0x00.
 *
 * \param B_INSTNUM    instance number
 * \param e_srvc       type of service, see {SOD_t_SERVICE}
 *                     (not checked, only called with enum
 *                     value in CallBeforeReadClbk() or
 *                     CallBeforeWriteClbk() or
 *                     CallAfterWriteClbk())
 *                     valid range: SOD_t_SERVICE
 * \param ps_obj       pointer to a SOD entry, see
 *                     {SOD_t_OBJECT} (pointer not checked,
 *                     only called with reference to struct
 *                     in SOD_Read() or SOD_Write())
 *                     valid range: pointer to a SOD_t_OBJECT
 * \param pv_data      pointer to data to be written, in case
 *                     of SOD_k_SRV_BEFORE_WRITE, otherwise
 *                     NULL (pointer not checked)
 *                     valid range: pointer to data to be
 *                     written, in case of
 *                     SOD_k_SRV_BEFORE_WRITE, otherwise NULL
 * \param dw_offset    start offset in bytes of the segment
 *                     within the data block (not used)
 * \param dw_size      size in bytes of the segment (not used)
 * \param pe_abortCode abort code has to be set for the SSDO if
 *                     the return value is FALSE.
 *                     (pointer not checked, only called with
 *                     reference to variable)
 *                     valid range: pointer to the
 *                     SOD_t_ABORT_CODES
 *
 * \return - TRUE  - success
 *         - FALSE - failure
 */
BOOLEAN SAPL_SOD_ParameterSet_CLBK(BYTE_B_INSTNUM_ SOD_t_SERVICE e_srvc,
                                   const SOD_t_OBJECT *ps_obj,
                                   const void *pv_data,
                                   UINT32 dw_offset, UINT32 dw_size,
                                   SOD_t_ABORT_CODES *pe_abortCode)
{
    BOOLEAN fReturn = FALSE;

    /* The parameter set is handled in the process function in the background.
     * Therefore these function parameters are not used!
     */
    UNUSED_PARAMETER(e_srvc);
    UNUSED_PARAMETER(ps_obj);
    UNUSED_PARAMETER(dw_offset);
    UNUSED_PARAMETER(pv_data);
    UNUSED_PARAMETER(dw_size);

    /* Check used parameters sanity */
    if( ps_obj == NULL              ||
        ps_obj->pv_objData == NULL  ||
        pe_abortCode == NULL         )
    {
        errh_postFatalError(kErrSourceSapl, kErrorInvalidParameter, 0);

        if(pe_abortCode != NULL)
        {
            *pe_abortCode = SOD_ABT_GENERAL_ERROR;
        }
    }
    else
    {
        /* Store parameter set attributes */
        saplInstance_l.paramSetAttr_m.pData_m = ((SOD_t_ACT_LEN_PTR_DATA*)(ps_obj->pv_objData))->pv_objData;
        saplInstance_l.paramSetAttr_m.length_m = ((SOD_t_ACT_LEN_PTR_DATA*)(ps_obj->pv_objData))->dw_actLen;

        saplInstance_l.activeTasks_m |= (1<<SAPL_TASK_PROCESS_PARAM_SET_PARSE_BIT);

        *pe_abortCode   = SOD_ABT_NO_ERROR;

        fReturn = TRUE;
    }

    return fReturn;
}

/**
 * HNFiff_Crc32CalcSwp
 * calculates the checksymbols of a buffer (\a buf) of length \a len bytes,
 * using a table (\a crctab) which contains the checksymbols for the values 0
 * up to 255. It is possible to calculate the crc of a huge buffer in several
 * chuncks. For this reason the parameter \a initcrc is used.\n
 * To init a chunckwise calculation \a buf is set to the beginning of the
 * buffer, \a len holds the chuncksize, and \a initcrc is set zero.
 * To continue a chunckwise calculation the buffer pointer \a (buf) must be
 * updated and \a initcrc is loaded with the result of the previous function
 * call.\n
 * If \a initcrc is nonzero when the first call of a calculation (entire
 * buffer or partial) is done, the result is a coset code.
 * \retval  crc value
 */
UINT32 HNFiff_Crc32CalcSwp(UINT32 w_initCrc, INT32 l_length,
                           const void *pv_data)
{
    return crc32Checksum(l_length, (UINT8*)pv_data, w_initCrc);
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */


/*----------------------------------------------------------------------------*/
/**
\brief    Check if the connection valid bit is set for this RxSpdo

\param[in] spdoId_p       Id if the RxSpdo

\retval TRUE    Connection valid bit is set
\retval FALSE   Connection is not valid
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN getConnValidInst0(UINT16 spdoId_p)
{
    BOOLEAN conValid = FALSE;

    if(spdoId_p < SPDO_cfg_MAX_NO_RX_SPDO)
    {
        if((SHNF_aaulConnValidBit[0][spdoId_p] & (UINT32)0x00000001) != FALSE)
            conValid = TRUE;
    }

    return conValid;
}


/*----------------------------------------------------------------------------*/
/**
\brief    Poll the connection valid bit and forward the value to the hardware

This function needs to be called periodically. It forwards the value of the
connection valid bit to an external hardware peripheral.
*/
/*----------------------------------------------------------------------------*/
static void checkConnectionValid(void)
{
    UINT16 i;
    BOOLEAN conValidBit = FALSE;

    /* Iterate over all SPDOs */
    for(i=0; i<SPDO_cfg_MAX_NO_RX_SPDO; i++)
    {
        /* Get connection valid bit of current SPDO */
        conValidBit = getConnValidInst0(i);

        /* Set gpio according to the new value */
        gpio_changeConValid(i, conValidBit);
    }
}


/**
 * \}
 * \}
 */

