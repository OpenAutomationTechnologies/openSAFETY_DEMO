/**
********************************************************************************
\file   demo-sn-gpio/shnf/constime.c

\defgroup module_sn_shnf_constime Consecutive timebase module
\{

\brief  This module provides the consecutive timebase

This module provides the consecutive timebase. It acts as an interface to the
target specific module of the hardware timer.

\ingroup group_app_sn_shnf
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
#include <shnf/constime.h>

#include <sapl/sapl.h>
#include <sn/timer.h>

#include <SODapi.h>

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

/*============================================================================*/
/*            P R I V A T E   D E F I N I T I O N S                           */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/
#define CONSTIME_BASE_1US       (UINT32)1       /**< The consecutive timebase is 1us */
#define CONSTIME_BASE_10US      (UINT32)10      /**< The consecutive timebase is 10us */
#define CONSTIME_BASE_100US     (UINT32)100     /**< The consecutive timebase is 100us */
#define CONSTIME_BASE_1MS       (UINT32)1000    /**< The consecutive timebase is 1ms */

/*----------------------------------------------------------------------------*/
/* local types                                                                */
/*----------------------------------------------------------------------------*/

/**
 * \brief Consecutive time module instance type
 */
typedef struct
{
    UINT64 usTimeBase_m;        /**< Microsecond timebase of the system. (Derived from a 16bit hardware counter) */
    UINT32 currConsTime_m;      /**< The current value of the consecutive time */
    UINT32 consTimeFactor_m;    /**< The consecutive timebase division factor */
} tConsTimeInstance;

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/
static tConsTimeInstance consTimeInstance_l;

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the consecutive time

\retval TRUE    CT initialization successful
\retval FALSE   Error during CT initialization
*/
/*----------------------------------------------------------------------------*/
BOOLEAN constime_init(void)
{
    BOOLEAN fReturn = FALSE;

    MEMSET(&consTimeInstance_l, 0, sizeof(tConsTimeInstance));

    if(timer_init())
    {
        /* Initialize the consecutive timebase division factor */
        consTimeInstance_l.consTimeFactor_m = CONSTIME_BASE_100US;

        fReturn = TRUE;
    }
    else
    {
        errh_postFatalError(kErrSourceShnf, kErrorInitConsTimeFailed, 0);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Disable the consecutive time
*/
/*----------------------------------------------------------------------------*/
void constime_exit(void)
{
    MEMSET(&consTimeInstance_l, 0, sizeof(tConsTimeInstance));

    timer_close();
}

/*----------------------------------------------------------------------------*/
/**
\brief    Get the current consecutive time

\return The current consecutive time
*/
/*----------------------------------------------------------------------------*/
UINT32 constime_getTime(void)
{
    return consTimeInstance_l.currConsTime_m;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Get the us timebase of the consecutive time

The system generates a microsecond timebase as a basis value for the consecutive
time.

\return The current timebase of the system
*/
/*----------------------------------------------------------------------------*/
UINT64 constime_getTimeBase(void)
{
    UINT64 currentTime = 0;
    UINT64 newTime = 0;

    currentTime = consTimeInstance_l.usTimeBase_m;
    newTime = currentTime + (((UINT64)(timer_getTickCount()) - currentTime) & 0xffff);

    return newTime;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Update the local time to a new value

\note Only call this function once on system bootup

\param[in] newTime_p    The new value of the time
*/
/*----------------------------------------------------------------------------*/
void constime_setTimebase(UINT64 newTime_p)
{
    consTimeInstance_l.usTimeBase_m = newTime_p;
    timer_setTickCount((UINT16)newTime_p);

}

/*----------------------------------------------------------------------------*/
/**
\brief    Synchronize the local time with the remote time

\param[in] pLocTime_p    Pointer to the local time
\param[in] pRcvTime_p    Pointer to the received time

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
BOOLEAN constime_syncConsTime(UINT64 * pLocTime_p, UINT64 * pRcvTime_p)
{
    BOOLEAN fReturn = FALSE;
    UINT64 avgVal = 0;

    if(pLocTime_p != NULL && pRcvTime_p != NULL)
    {
        /* Calculate the average value of both times */
        avgVal = (((*pLocTime_p) / 2) + ((*pRcvTime_p) / 2) + (((*pLocTime_p) & (*pRcvTime_p)) & 1));

        consTimeInstance_l.currConsTime_m = (UINT32)(avgVal / consTimeInstance_l.consTimeFactor_m);
        fReturn = TRUE;
    }
    else
    {
        errh_postFatalError(kErrSourceShnf, kErrorInvalidParameter, 0);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Set the consecutive time to a new value

\param[in] pNewTime_p    Pointer to the new timebase

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
BOOLEAN constime_setConsTime(UINT64 * pNewTime_p)
{
    BOOLEAN fReturn = FALSE;

    if(pNewTime_p != NULL)
    {
        consTimeInstance_l.currConsTime_m = (UINT32)(*pNewTime_p / consTimeInstance_l.consTimeFactor_m);

        fReturn = TRUE;
    }
    else
    {
        errh_postFatalError(kErrSourceShnf, kErrorInvalidParameter, 0);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process the system microsecond timer

This function needs to be called at least each 65ms in order to update the
local microsecond counter of the system.

\return Pointer to the new current time
*/
/*----------------------------------------------------------------------------*/
UINT64 * constime_process(void)
{
    UINT64 usTimeBase = 0;

    usTimeBase = consTimeInstance_l.usTimeBase_m;
    consTimeInstance_l.usTimeBase_m = usTimeBase + (((UINT64)(timer_getTickCount())
                                      - consTimeInstance_l.usTimeBase_m) & 0xffff);

    return &consTimeInstance_l.usTimeBase_m;
}


/*
 * This function is called after the write access to object
 * 0x1200, subindex 0x3. It is used to configure the hardware
 * timer with the new timebase.
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
BOOLEAN SHNF_SOD_ConsTimeBase_CLBK(BYTE_B_INSTNUM_ SOD_t_SERVICE e_srvc,
                                   const SOD_t_OBJECT *ps_obj,
                                   const void *pv_data,
                                   UINT32 dw_offset, UINT32 dw_size,
                                   SOD_t_ABORT_CODES *pe_abortCode)
{
    BOOLEAN fReturn = FALSE;
    UINT32 timeBase = 0;

    /* The parameter set is handled in the process function in the background.
     * Therefore these function parameters are not used!
     */
    UNUSED_PARAMETER(e_srvc);
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
        /* Store parameter set details in global structure */
        timeBase = (UINT32)(*(INT8*)ps_obj->pv_objData);
        switch(timeBase)
        {
            case 0:
                consTimeInstance_l.consTimeFactor_m = CONSTIME_BASE_1US;
                fReturn = TRUE;
            break;
            case 1:
                consTimeInstance_l.consTimeFactor_m = CONSTIME_BASE_10US;
                fReturn = TRUE;
            break;
            case 2:
                consTimeInstance_l.consTimeFactor_m = CONSTIME_BASE_100US;
                fReturn = TRUE;
            break;
            case 3:
                consTimeInstance_l.consTimeFactor_m = CONSTIME_BASE_1MS;
                fReturn = TRUE;
            break;
            default:
            {
                *pe_abortCode   = SOD_ABT_GENERAL_ERROR;
                fReturn = TRUE;
                break;
            }
        }
    }

    if(fReturn == TRUE)
        *pe_abortCode   = SOD_ABT_NO_ERROR;

    return fReturn;
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/**
 * \}
 * \}
 */
