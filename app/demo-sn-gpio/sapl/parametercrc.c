/**
********************************************************************************
\file   demo-sn-gpio/sapl/parametercrc.c

\defgroup module_sn_sapl_paramcrc Parameter CRC module
\{

\brief  This module calculates the parameter CRC over the SOD

This module iterates over the whole SOD and calculates the checksum for all
CRC relevant objects.

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

#include <sapl/parametercrc.h>

#include <sod.h>

#include <SODapi.h>
#include <SNMTSapi.h>
#include <SERRapi.h>
#include <SCFMapi.h>

/*============================================================================*/
/*            G L O B A L   D E F I N I T I O N S                             */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* module global vars                                                         */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* global function prototypes                                                 */
/*----------------------------------------------------------------------------*/
extern UINT32 HNFiff_Crc32CalcSwp(UINT32 w_initCrc, INT32 l_length,
                                  const void *pv_data);


/*============================================================================*/
/*            P R I V A T E   D E F I N I T I O N S                           */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/
#define MAX_CRC32_DATA_LENGTH       0x200               /**< For every 512 bytes of data we have one CRC32!  */
#define CRC32_ENTRY_COUNT           EPS_NO_SOD_CRCS     /**< Stores the count of CRC32 values in 0x1018/0x6 */

#define DEVICE_VENDOR_OBJ_IDX            0x1018
#define PARAMETER_CHECKSUM_OBJ_SUBIDX       0x6

/*----------------------------------------------------------------------------*/
/* local types                                                                */
/*----------------------------------------------------------------------------*/

/**
 * \brief This type represents all states of the CRC calulator
 */
typedef enum
{
    kParamCrcStateInvalid        = 0x0,       /**< Invalid state of the CRC calculator */
    kParamCrcStateInit           = 0x1,       /**< Start of the CRC calculation */
    kParamCrcStateProcess        = 0x2,       /**< Iterate over each SOD object and calculate the CRC */
    kParamCrcStateFinished       = 0x3,       /**< CRC calculation finished */
} tParamCrcState;

/**
 * \brief SOD CRC calculation module instance structure
 */
typedef struct
{
    tParamCrcState paramCrcState_m;             /**< State of the CRC calculator */
    BOOLEAN fStartProc_m;                       /**< TRUE of the processing state is entered */
    UINT16 currDataLen_m;                       /**< The current length of data already in the CRC32 */
    UINT32 crc32Entries_m[CRC32_ENTRY_COUNT];   /**< Stores all CRC32 value */
    UINT8 activeCrc_m;                          /**< Value of the currently active CRC32 value */
} tParamCrcInstance;

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/
static tParamCrcInstance paramCrcInstance_l SAFE_INIT_SEKTOR;

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static BOOLEAN calculateObjectCrc(SOD_t_OBJECT * pSodObject_p,
                                  EPLS_t_DATATYPE objType,
                                  UINT32 objLength_p);
static UINT32 * getCurrentActiveCrc32(UINT32 currObjLen_p, UINT32 * pRemObjLen_p);
static BOOLEAN verifyCrc32Values(void);

static void resetParamCrcModule(void);

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the parameter CRC module

\retval TRUE        Init of the module successful
\retval FALSE       Error on initialization
*/
/*----------------------------------------------------------------------------*/
BOOLEAN paramcrc_init(void)
{
    BOOLEAN fReturn = FALSE;

    MEMSET(&paramCrcInstance_l, 0, sizeof(tParamCrcInstance));

    fReturn = TRUE;

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Close the parameter CRC module
*/
/*----------------------------------------------------------------------------*/
void paramcrc_exit(void)
{
    /* Nothing to free */
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process the parameter CRC state machine

\retval kParamCrcProcError      Error on processing of the parameter CRC
\retval kParamCrcProcBusy       Calculation of the parameter CRC is busy
\retval kParamCrcProcFinished   Finished to calculate the parameter CRC
*/
/*----------------------------------------------------------------------------*/
tProcCrcRet paramcrc_process(void)
{
    tProcCrcRet paramCrcRet = kParamCrcProcError;
    SOD_t_ATTR * pCurrSodAttr = (SOD_t_ATTR *)NULL;
    UINT32 sodHdl = 0;
    SOD_t_ERROR_RESULT sodErrRes;
    SOD_t_OBJECT * pSodObject = (SOD_t_OBJECT *)NULL;

    switch(paramCrcInstance_l.paramCrcState_m)
    {
        case kParamCrcStateInit:
        {
            DEBUG_TRACE(DEBUG_LVL_SAPL, "\nStart SOD CRC calculation...\n");

            resetParamCrcModule();

            paramCrcInstance_l.fStartProc_m = TRUE;
            paramCrcInstance_l.paramCrcState_m = kParamCrcStateProcess;
            paramCrcRet = kParamCrcProcBusy;
            break;
        }
        case kParamCrcStateProcess:
        {
            /* Get the attribute of the next object */
            pCurrSodAttr = (SOD_t_ATTR *)SOD_AttrGetNext(B_INSTNUM_
                                                         paramCrcInstance_l.fStartProc_m,
                                                         &sodHdl,
                                                         &sodErrRes);
            if(pCurrSodAttr != NULL)
            {
                /* Reset the start flag */
                paramCrcInstance_l.fStartProc_m = FALSE;

                /* Check if the current object is CRC relevant */
                if((pCurrSodAttr->w_attr & SOD_k_ATTR_CRC) != 0 )
                {
                    /* Convert the handle to a pointer to the object */
                    pSodObject = (SOD_t_OBJECT*)sodHdl;
                    if(calculateObjectCrc(pSodObject, pCurrSodAttr->e_dataType, pCurrSodAttr->dw_objLen))
                    {
                        paramCrcRet = kParamCrcProcBusy;
                    }
                }
                else
                {
                    /* The current object is not relevant for CRC calculation */
                    paramCrcRet = kParamCrcProcBusy;
                }
            }
            else
            {
                /* End of SOD reached */
                paramCrcInstance_l.paramCrcState_m = kParamCrcStateFinished;
                paramCrcRet = kParamCrcProcBusy;
            }
            break;
        }
        case kParamCrcStateFinished:
        {
            /* Verify calculated checksum with the one in the SOD 1018/6 */
            if(verifyCrc32Values())
            {
                /* Parameter CRC is valid */
                paramCrcInstance_l.paramCrcState_m = kParamCrcStateInvalid;
                paramCrcRet = kParamCrcProcCrcValid;
            }
            else
            {
                /* Parameter CRC is not valid */
                paramCrcInstance_l.paramCrcState_m = kParamCrcStateInvalid;
                paramCrcRet = kParamCrcProcCrcInvalid;
            }
            break;
        }
        default:
        {
            /* Invalid state reached! */
            errh_postFatalError(kErrorInvalidState, kErrorInvalidState, 0);

            break;
        }
    }

    return paramCrcRet;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Start the processing of the CRC calculator
*/
/*----------------------------------------------------------------------------*/
void paramcrc_startProcessing(void)
{
    paramCrcInstance_l.paramCrcState_m = kParamCrcStateInit;
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/*----------------------------------------------------------------------------*/
/**
\brief    Calculate the CRC value of an object

\param[in] pSodObject_p     Pointer to the object in the SOD
\param[in] objType          Type of the object
\param[in] objLength_p      Length of the object

\retval TRUE    CRC calculation for this object successful
\retval FALSE   Failed to calculate the CRC for this object
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN calculateObjectCrc(SOD_t_OBJECT * pSodObject_p,
                                  EPLS_t_DATATYPE objType,
                                  UINT32 objLength_p)
{
    BOOLEAN fReturn = FALSE;
    BOOLEAN fObjProc = TRUE;
    void * pObjData = NULL;
    UINT32 objLen = 0;
    UINT32 * pActCrc32 = &paramCrcInstance_l.crc32Entries_m[paramCrcInstance_l.activeCrc_m];
    UINT32 remObjLen = 0;

    /* Get length and data pointer from the current object */
    if ((objType == EPLS_k_VISIBLE_STRING) ||
        (objType == EPLS_k_DOMAIN)         ||
        (objType == EPLS_k_OCTET_STRING)    )
    {
        pObjData = ((SOD_t_ACT_LEN_PTR_DATA*)pSodObject_p->pv_objData)->pv_objData;
        objLen = (UINT32)((SOD_t_ACT_LEN_PTR_DATA*)pSodObject_p->pv_objData)->dw_actLen;
    }
    else
    {
        /* Object has a normal datatype e.g: UINT32 */
        pObjData = pSodObject_p->pv_objData;
        objLen = (UINT32)objLength_p;
    }

    /* Calculate the CRC32 over the object if object available */
    if(pObjData != NULL)
    {
        /* Iterate over all segments of the object */
        while(objLen > 0)
        {
            pActCrc32 = getCurrentActiveCrc32(objLen, &remObjLen);
            if(pActCrc32 == NULL)
            {
                fObjProc = FALSE;
                break;
            }
            else
            {
                /* Calculate the CRC for this object and add to checksum */
                *pActCrc32 = HNFiff_Crc32CalcSwp(*pActCrc32, remObjLen, pObjData);
                paramCrcInstance_l.currDataLen_m += remObjLen;
                objLen -= remObjLen;
            }
        }

        if(fObjProc)
        {
            fReturn = TRUE;
        }
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Get the pointer to the current active CRC32 value

\param[in]  currObjLen_p Length of the current object
\param[out] pRemObjLen_p The remaining length of the object which fits into this CRC32

\retval Address     The address of the current CRC32 field
\retval NULL        On error
*/
/*----------------------------------------------------------------------------*/
static UINT32 * getCurrentActiveCrc32(UINT32 currObjLen_p, UINT32 * pRemObjLen_p)
{
    UINT32 * pActCrc32 = (UINT32 *)NULL;

    /* Check if there is space left in the current CRC32 */
    if((paramCrcInstance_l.currDataLen_m + currObjLen_p) > MAX_CRC32_DATA_LENGTH)
    {
        /* Set remaining length which fits into the current active CRC32 */
        *pRemObjLen_p = MAX_CRC32_DATA_LENGTH - paramCrcInstance_l.currDataLen_m;
        pActCrc32 = &paramCrcInstance_l.crc32Entries_m[paramCrcInstance_l.activeCrc_m];
    }
    else if(paramCrcInstance_l.currDataLen_m == MAX_CRC32_DATA_LENGTH)
    {
        if(paramCrcInstance_l.activeCrc_m < CRC32_ENTRY_COUNT)
        {
            /* Length of the CRC reached the limit -> Start a new CRC32 */
            paramCrcInstance_l.currDataLen_m = 0;
            paramCrcInstance_l.activeCrc_m++;

            *pRemObjLen_p = currObjLen_p;

            pActCrc32 = &paramCrcInstance_l.crc32Entries_m[paramCrcInstance_l.activeCrc_m];
        }
        else
        {
            /* There is no CRC32 field allocated -> Return FALSE! */
            errh_postFatalError(kErrSourceShnf, kErrorNoCrcFieldAllocated, 0);
        }
    }
    else
    {
        /* Data can be added to the current active CRC32 */
        *pRemObjLen_p = currObjLen_p;

        pActCrc32 = &paramCrcInstance_l.crc32Entries_m[paramCrcInstance_l.activeCrc_m];
    }

    return pActCrc32;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Verify the calculated CRC32 values with the one from the SOD

Read the CRC32 values from 0x1018/0x6 and compare them with the calcualted
CRCs.

\retval TRUE     Calculated CRCs match the SOD
\retval FALSE    CRCs are invalid
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN verifyCrc32Values(void)
{
    BOOLEAN fCrcValid = FALSE;
    BOOLEAN fSubCrcValid = TRUE;
    BOOLEAN fIsAppObj = FALSE;
    UINT8 i = 0;
    UINT32 paramCrcHdl = 0;
    SOD_t_ERROR_RESULT errRes;
    SOD_t_ATTR * pSodAttr = (SOD_t_ATTR *)NULL;
    tParamChksum * pChkSumObj = NULL;

    MEMSET(&errRes, 0, sizeof(SOD_t_ERROR_RESULT));

    /* Get attribute of the Parameter Checksum object */
    pSodAttr = (SOD_t_ATTR *)SOD_AttrGet(DEVICE_VENDOR_OBJ_IDX, PARAMETER_CHECKSUM_OBJ_SUBIDX,
                                         &paramCrcHdl, &fIsAppObj, &errRes);
    if(pSodAttr != NULL)
    {
        /* Read the data of the parameter checksum object */
        pChkSumObj = (tParamChksum *)SOD_Read(paramCrcHdl, fIsAppObj, 0, pSodAttr->dw_objLen, &errRes);
        if(pChkSumObj != NULL)
        {
            /* Iterate over all parameter checksum values */
            for(i=0; i < CRC32_ENTRY_COUNT; i++)
            {
                /* Compare the checksum value with the one from the SOD */
                if(pChkSumObj->aulSodCrc[i] != paramCrcInstance_l.crc32Entries_m[i])
                {
                    fSubCrcValid = FALSE;
                    break;
                }
                else
                {
                    DEBUG_TRACE(DEBUG_LVL_SAPL, "CRC32: 0x%x verified!\n", paramCrcInstance_l.crc32Entries_m[i]);
                }
            }

            if(fSubCrcValid)
            {
                fCrcValid = TRUE;
            }
        }
        else
        {
            errh_postFatalError(kErrSourceShnf, kErrorUnableToReadFromSod, 0);
        }
    }
    else
    {
        errh_postFatalError(kErrSourceShnf, kErrorUnableToObjAttr, 0);
    }

    return fCrcValid;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Reset parameter CRC instance to initial state
*/
/*----------------------------------------------------------------------------*/
static void resetParamCrcModule(void)
{
    UINT8 i;

    paramCrcInstance_l.currDataLen_m = 0;
    paramCrcInstance_l.activeCrc_m = 0;

    paramCrcInstance_l.fStartProc_m = FALSE;
    paramCrcInstance_l.paramCrcState_m = kParamCrcStateInvalid;

    for(i=0; i<CRC32_ENTRY_COUNT; i++)
        paramCrcInstance_l.crc32Entries_m[i] = 0x0;
}

/**
 * \}
 * \}
 */

