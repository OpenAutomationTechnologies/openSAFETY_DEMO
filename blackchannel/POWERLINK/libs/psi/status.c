/**
********************************************************************************
\file   status.c

\defgroup module_psi_status Status module
\{

\brief  Status module for synchronization information forwarding

This module forwards the time information to the user application.
It also provides the SSDO channel status information.

\ingroup group_libpsi
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

#include <libpsi/internal/status.h>

#include <libpsi/internal/stream.h>

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

/*----------------------------------------------------------------------------*/
/* local types                                                                */
/*----------------------------------------------------------------------------*/

/**
\brief status user instance type

The status instance holds the status information of this module
*/
typedef struct
{
    tTbufStatusOutStructure*  pStatusOutLayout_m;   /**< Local copy of the status output triple buffer */
    tTbufNumLayout            buffOutId_m;          /**< Id of the output status register buffer */
    UINT8                     iccStatus_m;          /**< Icc status register */
    UINT16                    ssdoTxStatus_m;       /**< Status of the SSDO transmit channel */
    UINT8                     logTxStatus_m;        /**< Status of the logbook transmit channel */

    tTbufStatusInStructure*   pStatusInLayout_m;    /**< Local copy of the status incoming triple buffer */
    tTbufNumLayout            buffInId_m;           /**< Id of the incoming status register buffer */
    UINT16                    ssdoRxStatus_m;       /**< Status of the SSDO receive channel */

    tPsiAppCbSync           pfnProcSyncCb_m;       /**< Synchronous callback function */
} tStatusInstance;

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/

static tStatusInstance          statusInstance_l;

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static BOOL status_initOutBuffer(tTbufNumLayout statOutId_p);
static BOOL status_initInBuffer(tTbufNumLayout statInId_p);
static BOOL status_processSync(UINT8* pBuffer_p, UINT16 bufSize_p,
        void * pUserArg_p);
static BOOL status_updateOutStatusReg(UINT8* pBuffer_p, UINT16 bufSize_p,
        void * pUserArg_p);
static BOOL status_updateInStatusReg(UINT8* pBuffer_p, UINT16 bufSize_p,
        void * pUserArg_p);

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the status module

\param[in]  pInitParam_p     Initialization structure of the status module

\retval  TRUE      Successfully initialized the status module
\retval  FALSE     Error during initialization
*/
/*----------------------------------------------------------------------------*/
BOOL status_init(tStatusInitParam* pInitParam_p)
{
    BOOL fReturn = FALSE;

    PSI_MEMSET(&statusInstance_l, 0 , sizeof(tStatusInstance));

    if(pInitParam_p == NULL)
    {
        error_setError(kPsiModuleStatus, kPsiStatusInitError);
    }
    else
    {
        if(pInitParam_p->pfnProcSyncCb_m == NULL    ||
           pInitParam_p->buffOutId_m >= kTbufCount ||
           pInitParam_p->buffInId_m >= kTbufCount   )
        {
            error_setError(kPsiModuleStatus, kPsiStatusInitError);
        }
        else
        {
            /* Register status outgoing triple buffer */
            if(status_initOutBuffer(pInitParam_p->buffOutId_m) != FALSE)
            {
                /* Register status incoming triple buffer */
                if(status_initInBuffer(pInitParam_p->buffInId_m) != FALSE)
                {
                    /* Remember id of the buffer */
                    statusInstance_l.buffOutId_m = pInitParam_p->buffOutId_m;

                    /* Remember synchronous callback function */
                    statusInstance_l.pfnProcSyncCb_m = pInitParam_p->pfnProcSyncCb_m;

                    fReturn = TRUE;
                }
                else
                {
                    error_setError(kPsiModuleStatus, kPsiStatusInitError);
                }
            }
            else
            {
                error_setError(kPsiModuleStatus, kPsiStatusInitError);
            }
        }
    }


    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Cleanup status module
*/
/*----------------------------------------------------------------------------*/
void status_exit(void)
{
    /* Free module internals */
}

/*----------------------------------------------------------------------------*/
/**
\brief    Get Icc status register

\param[out]  pSeqNr_p        The current status of the channel
*/
/*----------------------------------------------------------------------------*/
void status_getIccStatus(tSeqNrValue* pSeqNr_p)
{
    /* Reformat to sequence number type */
    if(CHECK_BIT(statusInstance_l.iccStatus_m, STATUS_ICC_BUSY_FLAG_POS))
    {
        *pSeqNr_p = kSeqNrValueSecond;
    }
    else
    {
        *pSeqNr_p = kSeqNrValueFirst;
    }

}

/*----------------------------------------------------------------------------*/
/**
\brief    Set the SSDO receive channel to next element

\param[in] chanNum_p     Id of the channel to mark as busy
\param[in] seqNr_p       The value of the sequence number
*/
/*----------------------------------------------------------------------------*/
void status_setSsdoRxChanFlag(UINT8 chanNum_p, tSeqNrValue seqNr_p)
{
    if(seqNr_p == kSeqNrValueFirst)
    {
        statusInstance_l.ssdoRxStatus_m &= ~(1<<chanNum_p);
    }
    else
    {
        statusInstance_l.ssdoRxStatus_m |= (1<<chanNum_p);
    }

}

/*----------------------------------------------------------------------------*/
/**
\brief    Get the SSDO transmit channel status

\param[in]  chanNum_p     Id of the channel to mark as busy
\param[out] pSeqNr_p      The value of the sequence number
*/
/*----------------------------------------------------------------------------*/
void status_getSsdoTxChanFlag(UINT8 chanNum_p, tSeqNrValue* pSeqNr_p)
{
    /* Reformat to sequence number type */
    if(CHECK_BIT(statusInstance_l.ssdoTxStatus_m,chanNum_p))
    {
        *pSeqNr_p = kSeqNrValueSecond;
    }
    else
    {
        *pSeqNr_p = kSeqNrValueFirst;
    }
}

/*----------------------------------------------------------------------------*/
/**
\brief    Get the logbook transmit channel status

\param[in]  chanNum_p     Id of the channel to mark as busy
\param[out] pSeqNr_p      The value of the sequence number
*/
/*----------------------------------------------------------------------------*/
void status_getLogTxChanFlag(UINT8 chanNum_p, tSeqNrValue* pSeqNr_p)
{
    /* Reformat to sequence number type */
    if(CHECK_BIT(statusInstance_l.logTxStatus_m, chanNum_p))
    {
        *pSeqNr_p = kSeqNrValueSecond;
    }
    else
    {
        *pSeqNr_p = kSeqNrValueFirst;
    }
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the outgoing status buffer

\param[in] statOutId_p         Id of the buffer to initialize

\retval TRUE        Successfully initialized the output buffer
\retval FALSE       Unable to initialize the output buffer
*/
/*----------------------------------------------------------------------------*/
static BOOL status_initOutBuffer( tTbufNumLayout statOutId_p)
{
    BOOL fReturn = FALSE;
    tBuffDescriptor* pDescStatOut;

    pDescStatOut = stream_getBufferParam(statOutId_p);
    if(pDescStatOut->pBuffBase_m != NULL)
    {
        if(pDescStatOut->buffSize_m == sizeof(tTbufStatusOutStructure))
        {
            /* Remember buffer address for later usage */
            statusInstance_l.pStatusOutLayout_m = (tTbufStatusOutStructure *)pDescStatOut->pBuffBase_m;

            /* Register status module pre action for sync processing */
            if(stream_registerAction(kStreamActionPre, statOutId_p,
                    status_processSync, NULL) != FALSE)
            {
                /* Register outgoing status module post action for status register update */
                if(stream_registerAction(kStreamActionPost, statOutId_p,
                        status_updateOutStatusReg, NULL) != FALSE)
                {
                    fReturn = TRUE;
                }
            }
        }
        else
        {   /* Invalid size of output buffer */
            error_setError(kPsiModuleStatus, kPsiStatusBufferSizeMismatch);
        }
    }
    else
    {   /* Invalid base address of output buffer */
        error_setError(kPsiModuleStatus, kPsiStreamInvalidBuffer);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the incoming status buffer

\param[in] statInId_p         Id of the buffer to initialize

\retval TRUE        Successfully initialized the input buffer
\retval FALSE       Unable to initialize the input buffer
*/
/*----------------------------------------------------------------------------*/
static BOOL status_initInBuffer(tTbufNumLayout statInId_p)
{
    BOOL fReturn = FALSE;
    tBuffDescriptor* pDescStatIn;

    pDescStatIn = stream_getBufferParam(statInId_p);
    if(pDescStatIn->pBuffBase_m != NULL)
    {
        if(pDescStatIn->buffSize_m == sizeof(tTbufStatusInStructure))
        {
            /* Remember buffer address for later usage */
            statusInstance_l.pStatusInLayout_m = (tTbufStatusInStructure *)pDescStatIn->pBuffBase_m;

            /* Register incoming status module post action for status register update */
            if(stream_registerAction(kStreamActionPost, statInId_p,
                    status_updateInStatusReg, NULL) != FALSE)
            {
                fReturn = TRUE;
            }
            else
            {   /* Unable to register in buffer user action */
                error_setError(kPsiModuleStatus, kPsiStreamInitError);
            }

        }
        else
        {
            /* Invalid size of input buffer */
            error_setError(kPsiModuleStatus, kPsiStatusBufferSizeMismatch);
        }
    }
    else
    {
        /* Invalid base address of input buffer */
        error_setError(kPsiModuleStatus, kPsiStreamInvalidBuffer);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process time synchronization task

\param[in] pBuffer_p        Pointer to the base address of the buffer
\param[in] bufSize_p        Size of the buffer
\param[in] pUserArg_p       User defined argument

\retval TRUE        Successfully processed synchronous task
\retval FALSE       Error while processing sync task
*/
/*----------------------------------------------------------------------------*/
static BOOL status_processSync(UINT8* pBuffer_p, UINT16 bufSize_p,
        void * pUserArg_p)
{
    BOOL fReturn = FALSE;
    tPsiTimeStamp        timeStamp;
    tTbufStatusOutStructure*  pStatusBuff;

    UNUSED_PARAMETER(bufSize_p);
    UNUSED_PARAMETER(pUserArg_p);

    /* Convert to status buffer structure */
    pStatusBuff = (tTbufStatusOutStructure*) pBuffer_p;

    /* Call synchronous callback function */
    timeStamp.relTimeLow_m = ami_getUint32Le((UINT8 *)&pStatusBuff->relTimeLow_m);
    timeStamp.relTimeHigh_m = ami_getUint32Le((UINT8 *)&pStatusBuff->relTimeHigh_m);

    if(statusInstance_l.pfnProcSyncCb_m(&timeStamp) != FALSE)
    {
        fReturn = TRUE;
    }
    else
    {
        error_setError(kPsiModuleStatus, kPsiStatusProcessSyncFailed);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process outgoing status register fields

\param[in] pBuffer_p        Pointer to the base address of the buffer
\param[in] bufSize_p        Size of the buffer
\param[in] pUserArg_p       User defined argument

\retval TRUE        Successfully updated the status register
\retval FALSE       Error while updating the status register
*/
/*----------------------------------------------------------------------------*/
static BOOL status_updateOutStatusReg(UINT8* pBuffer_p, UINT16 bufSize_p,
        void * pUserArg_p)
{
    tTbufStatusOutStructure*  pStatusBuff;

    UNUSED_PARAMETER(bufSize_p);
    UNUSED_PARAMETER(pUserArg_p);

    /* Convert to status buffer structure */
    pStatusBuff = (tTbufStatusOutStructure*) pBuffer_p;

    /* Get CC status register */
    statusInstance_l.iccStatus_m = ami_getUint8Le((UINT8 *)&pStatusBuff->iccStatus_m);

    /* Update ssdo tx status register */
    statusInstance_l.ssdoTxStatus_m = ami_getUint16Le((UINT8 *)&pStatusBuff->ssdoConsStatus_m);

    /* Update logbook tx status register */
    statusInstance_l.logTxStatus_m = ami_getUint8Le((UINT8 *)&pStatusBuff->logConsStatus_m);

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process incoming status register fields

\param[in] pBuffer_p        Pointer to the base address of the buffer
\param[in] bufSize_p        Size of the buffer
\param[in] pUserArg_p       User defined argument

\retval TRUE        Successfully updated the status register
\retval FALSE       Error while updating the status register
*/
/*----------------------------------------------------------------------------*/
static BOOL status_updateInStatusReg(UINT8* pBuffer_p, UINT16 bufSize_p,
        void * pUserArg_p)
{
    tTbufStatusInStructure*  pStatusBuff;

    UNUSED_PARAMETER(bufSize_p);
    UNUSED_PARAMETER(pUserArg_p);

    /* Convert to status buffer structure */
    pStatusBuff = (tTbufStatusInStructure*) pBuffer_p;

    /* Write rx status register */
    ami_setUint16Le((UINT8 *)&pStatusBuff->ssdoProdStatus_m, statusInstance_l.ssdoRxStatus_m);

    return TRUE;
}

/**
 * \}
 * \}
 */


