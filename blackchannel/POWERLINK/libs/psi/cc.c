/**
********************************************************************************
\file   cc.c

\defgroup module_psi_cc Configuration channel module
\{

\brief  Handles the configuration channel for incoming and outgoing objects

This modules forwards incoming object data from the occ to the local object list
and forwards objects from the user application to the PCP.

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

#include <libpsi/internal/cc.h>

#include <libpsi/internal/stream.h>
#include <libpsi/internal/status.h>

#include <libpsicommon/ccobject.h>
#include <libpsicommon/timeout.h>

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_CC)) != 0)

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
 * \brief Status of the asynchronous channel
 */
typedef enum {
    kChanStatusInvalid   = 0x00,    /**< Invalid channel status */
    kChanStatusBusy      = 0x01,    /**< Channel is currently busy */
    kChanStatusFree      = 0x02,    /**< Channel is free for transmission */
} tCcChanStatus;

typedef struct {
    UINT8                   isLocked_m;        /**< Is buffer free for filling */
    tTbufCcStructure*       pIccTxPayl_m;      /**< Pointer to the Icc transmit buffer */
} tCcTxBuffer;

/**
 * \brief Parameter type of the transmit buffer
 */
typedef struct {
    tTbufNumLayout     idIccTx_m;          /**< Input transmit buffer id */
    tCcTxBuffer        iccTxBuffer_m;      /**< Parameters of the Icc buffer */
    tSeqNrValue        currTxSeqNr_m;      /**< Current transmit sequence number */
    tTimeoutInstance   pTimeoutInst_m;     /**< Timer instance for asynchronous transmissions */
} tCcTxChannel;

/**
 * \brief Parameter type of the receive buffer
 */
typedef struct {
    tTbufNumLayout     idOccRx_m;           /**< Output receive buffer id */
    tTbufCcStructure*  pOccLayout_m;        /**< Pointer to the Occ transmit buffer */
} tCcRxChannel;

/**
\brief CC user instance type
*/
typedef struct {
    tCcTxChannel       txChannel_m;         /**< Application outgoing channel */
    tCcRxChannel       rxChannel_m;         /**< Application incoming channel */
} tCcInstance;

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/

static tCcInstance          ccInstance_l;

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static BOOL cc_initIccTxBuffer(tTbufNumLayout iccId_p);
static BOOL cc_initOccRxBuffer(tTbufNumLayout occId_p);
static void cc_initCcObjects(void);
static void cc_processTxObject(void);
static void cc_changeLocalSeqNr(tSeqNrValue* pSeqNr_p);
static tCcChanStatus cc_checkIccStatus(void);
static BOOL cc_handleOccRxObjects(UINT8* pBuffer_p, UINT16 bufSize_p,
        void* pUserArg_p);

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the cc module

Initialize the input and output configuration channel

\param[in]  pCcInitParam_p         Configuration channel initialization structure

\retval kPsiSuccessful           On success
\retval kPsiConfChanInitError    Unable to initialize the configuration channel
*/
/*----------------------------------------------------------------------------*/
BOOL cc_init(tCcInitParam* pCcInitParam_p)
{
    BOOL fReturn = FALSE;

    PSI_MEMSET(&ccInstance_l, 0 , sizeof(tCcInstance));

    if(pCcInitParam_p == NULL)
    {
        error_setError(kPsiModuleCc, kPsiConfChanInitError);
    }
    else
    {
        if(pCcInitParam_p->iccId_m >= kTbufCount             ||
           pCcInitParam_p->occId_m >= kTbufCount             ||
           pCcInitParam_p->iccId_m == pCcInitParam_p->occId_m )
        {
            error_setError(kPsiModuleCc, kPsiConfChanInitError);
        }
        else
        {
            /* Register icc tx buffer to stream module */
            if(cc_initIccTxBuffer(pCcInitParam_p->iccId_m) != FALSE)
            {
                /* Register occ rx buffer to stream module */
                if(cc_initOccRxBuffer(pCcInitParam_p->occId_m) != FALSE)
                {
                    /* Initialize the configuration channel objects module */
                    if(ccobject_init(pCcInitParam_p->pfnCritSec_p) != FALSE)
                    {
                        /* Initialize the cc object internal list */
                        cc_initCcObjects();

                        /* Fill local instance structure */
                        ccInstance_l.txChannel_m.idIccTx_m = pCcInitParam_p->iccId_m;
                        ccInstance_l.rxChannel_m.idOccRx_m = pCcInitParam_p->occId_m;

                        /* Set sequence number init value */
                        ccInstance_l.txChannel_m.currTxSeqNr_m = kSeqNrValueSecond;

                        fReturn = TRUE;
                    }
                    else
                    {
                        error_setError(kPsiModuleCc, kPsiConfChanInitError);
                    }
                }
            }
        }


    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Close the configuration channel
*/
/*----------------------------------------------------------------------------*/
void cc_exit(void)
{
    /* Free cc object list module */
    ccobject_exit();

    /* Destroy timeout module */
    timeout_destroy(ccInstance_l.txChannel_m.pTimeoutInst_m);
}

/*----------------------------------------------------------------------------*/
/**
\brief    Read a PCP object via the configuration channel

\param[in]  objIdx_p           Index of the to read object
\param[in]  objSubIdx_p        Subindex of the to read object

\retval Address     Success while reading object
\retval Null        Unable to read object
*/
/*----------------------------------------------------------------------------*/
tConfChanObject* cc_readObject(UINT16 objIdx_p, UINT8 objSubIdx_p)
{
    tConfChanObject* pObject;

    /* Read object from local object list */
    pObject = ccobject_readObject(objIdx_p, objSubIdx_p);
    if(pObject == NULL)
    {
        error_setError(kPsiModuleCc, kPsiConfChanObjectNotFound);
    }

    return pObject;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Write a PCP object via the configuration channel

\param[in] pObject_p         The object to write

\retval kCcWriteStatusSuccessfull   Writing to the Cc object successful
\retval kCcWriteStatusError         Error while writing the Cc object
\retval kCcWriteStatusBusy          Unable to write the Cc object! Channel is busy!
*/
/*----------------------------------------------------------------------------*/
tCcWriteStatus cc_writeObject(tConfChanObject* pObject_p)
{
    tCcWriteStatus stateWrite = kCcWriteStatusError;

    if(pObject_p == NULL)
    {
        error_setError(kPsiModuleCc, kPsiConfChanInvalidParameter);
    }
    else
    {
        if(pObject_p->objSize_m > sizeof(UINT64) ||
           pObject_p->objSize_m == 0              )
        {
            error_setError(kPsiModuleCc, kPsiConfChanInvalidSizeOfObj);
        }
        else
        {
            /* Check if buffer is free for filling */
            if(ccInstance_l.txChannel_m.iccTxBuffer_m.isLocked_m == FALSE)
            {
                /* Write object data to local object list */
                if(ccobject_writeObject(pObject_p) != FALSE)
                {
                    /* Fill transmit buffer */
                    PSI_MEMCPY(&ccInstance_l.txChannel_m.iccTxBuffer_m.pIccTxPayl_m->objPayloadLow_m,
                            &pObject_p->objPayloadLow_m, pObject_p->objSize_m);

                    /* Set object index and subindex */
                    ami_setUint16Le((UINT8*)&ccInstance_l.txChannel_m.iccTxBuffer_m.pIccTxPayl_m->objIdx_m,
                            pObject_p->objIdx_m);
                    ami_setUint8Le((UINT8*)&ccInstance_l.txChannel_m.iccTxBuffer_m.pIccTxPayl_m->objSubIdx_m,
                            pObject_p->objSubIdx_m);

                    /* Set sequence number in tx buffer */
                    ami_setUint8Le((UINT8*)&ccInstance_l.txChannel_m.iccTxBuffer_m.pIccTxPayl_m->seqNr_m,
                            ccInstance_l.txChannel_m.currTxSeqNr_m);

                    /* Lock buffer for transmission */
                    ccInstance_l.txChannel_m.iccTxBuffer_m.isLocked_m = TRUE;

                    /* Enable transmit timer */
                    timeout_startTimer(ccInstance_l.txChannel_m.pTimeoutInst_m);

                    stateWrite = kCcWriteStatusSuccessful;
                }
            }
            else
            {
                stateWrite = kCcWriteStatusBusy;
            }
        }
    }

    return stateWrite;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process configuration channel module
*/
/*----------------------------------------------------------------------------*/
void cc_process(void)
{
    /* Process transmit channel objects */
    cc_processTxObject();
}


/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/*----------------------------------------------------------------------------*/
/**
\brief    Process the transmit objects

Forward the object to the ICC buffer to update the object dictionary of the
PCP.
*/
/*----------------------------------------------------------------------------*/
static void cc_processTxObject(void)
{
    tCcChanStatus chanState;
    tTimerStatus  timerState;

    if(ccInstance_l.txChannel_m.iccTxBuffer_m.isLocked_m != FALSE)
    {
        /* Check if channel is ready for transmission */
        chanState = cc_checkIccStatus();
        if(chanState == kChanStatusFree)
        {
            /* Ongoing message is acknowledged */
            ccInstance_l.txChannel_m.iccTxBuffer_m.isLocked_m = FALSE;

            /* Increment local sequence number */
            cc_changeLocalSeqNr(&ccInstance_l.txChannel_m.currTxSeqNr_m);

            /* Stop the timer for this message */
            timeout_stopTimer(ccInstance_l.txChannel_m.pTimeoutInst_m);
        }
        else
        {
            /* Channel is currently busy! */
            /* Check if timeout counter is expired */
            timerState = timeout_checkExpire(ccInstance_l.txChannel_m.pTimeoutInst_m);
            if(timerState == kTimerStateExpired)
            {
                /* Timeout occurred -> Increment local sequence number! */
                cc_changeLocalSeqNr(&ccInstance_l.txChannel_m.currTxSeqNr_m);

                /* Unlock channel anyway! */
                ccInstance_l.txChannel_m.iccTxBuffer_m.isLocked_m = FALSE;
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the Icc transmit buffer

\param[in] iccId_p               Id of the Icc transmit buffer

\retval TRUE        Successfully initialized the input configuration channel
\retval FALSE       Error on initialization
*/
/*----------------------------------------------------------------------------*/
static BOOL cc_initIccTxBuffer(tTbufNumLayout iccId_p)
{
    BOOL fReturn = FALSE;
    tBuffDescriptor* pDescIccRcv;

    pDescIccRcv = stream_getBufferParam(iccId_p);
    if(pDescIccRcv->pBuffBase_m != NULL)
    {
        if(pDescIccRcv->buffSize_m == sizeof(tTbufCcStructure))
        {
            /* Remember buffer address for later usage */
            ccInstance_l.txChannel_m.iccTxBuffer_m.pIccTxPayl_m =
                    (tTbufCcStructure *)pDescIccRcv->pBuffBase_m;

            /* Create timeout instance for transmit channel */
            ccInstance_l.txChannel_m.pTimeoutInst_m = timeout_create(
                    CC_TX_TIMEOUT_CYCLE_COUNT);
            if(ccInstance_l.txChannel_m.pTimeoutInst_m != NULL)
            {
                fReturn = TRUE;
            }
            else
            {
                error_setError(kPsiModuleCc, kPsiConfChanInitError);
            }
        }
        else
        {
            error_setError(kPsiModuleCc, kPsiConfChanBufferSizeMismatch);
        }
    }
    else
    {
        error_setError(kPsiModuleCc, kPsiConfChanInvalidBuffer);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the Occ receive buffer

\param[in] occId_p               Id of the Icc receive buffer

\retval TRUE        Successfully initialized the output configuration channel
\retval FALSE       Error on initialization
*/
/*----------------------------------------------------------------------------*/
static BOOL cc_initOccRxBuffer(tTbufNumLayout occId_p)
{
    BOOL fReturn = FALSE;
    tBuffDescriptor* pDescOccTrans;

    pDescOccTrans = stream_getBufferParam(occId_p);
    if(pDescOccTrans->pBuffBase_m != NULL)
    {
        if(pDescOccTrans->buffSize_m == sizeof(tTbufCcStructure))
        {
            /* Remember buffer address for later usage */
            ccInstance_l.rxChannel_m.pOccLayout_m =
                    (tTbufCcStructure *)pDescOccTrans->pBuffBase_m;

            /* Register object receive post action */
            if(stream_registerAction(kStreamActionPost, occId_p,
                    cc_handleOccRxObjects, NULL) != FALSE)
            {
                fReturn = TRUE;
            }
        }
        else
        {
            error_setError(kPsiModuleCc, kPsiConfChanBufferSizeMismatch);
        }
    }
    else
    {
        error_setError(kPsiModuleCc, kPsiConfChanInvalidBuffer);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the list of configuration channel objects
*/
/*----------------------------------------------------------------------------*/
static void cc_initCcObjects(void)
{
    tConfChanObject  object;
    UINT8            i;
    UINT64           paylDest = 0;
    tCcObject        initObjList[CONF_CHAN_NUM_OBJECTS] = CCOBJECT_LIST_INIT_VECTOR;

    PSI_MEMSET(&object, 0, sizeof(tConfChanObject));

    for(i=0; i < CONF_CHAN_NUM_OBJECTS; i++)
    {
        /* Assemble object for list */
        object.objIdx_m = initObjList[i].objIdx;
        object.objSubIdx_m = initObjList[i].objSubIdx;
        object.objSize_m = initObjList[i].objSize;
        PSI_MEMCPY(&object.objPayloadLow_m, &paylDest, object.objSize_m);

        /* Now initialize the object in the local list */
        ccobject_initObject(i, &object);
    }
}

/*----------------------------------------------------------------------------*/
/**
\brief    Change local sequence number

\param[out] pSeqNr_p        Changed sequence number
*/
/*----------------------------------------------------------------------------*/
static void cc_changeLocalSeqNr(tSeqNrValue* pSeqNr_p)
{
    if(*pSeqNr_p == kSeqNrValueFirst)
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
\brief    Check if channel is ready for transmission

\return tCcChanStatus
\retval kChanStatusFree    Channel is free for transmission
\retval kChanStatusBusy    Channel is currently transmitting
*/
/*----------------------------------------------------------------------------*/
static tCcChanStatus cc_checkIccStatus(void)
{
    tCcChanStatus chanState = kChanStatusInvalid;
    tSeqNrValue  seqNr = kSeqNrValueInvalid;

    /* Get status of transmit channel */
    status_getIccStatus(&seqNr);

    /* Check if old transmission is already finished! */
    if(seqNr != ccInstance_l.txChannel_m.currTxSeqNr_m)
    {
        /* Message in progress -> retry later! */
        chanState = kChanStatusBusy;
    }
    else
    {
        chanState = kChanStatusFree;
    }

    return chanState;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Handle incoming objects from the occ buffer

\param[in] pBuffer_p        Pointer to the base address of the buffer
\param[in] bufSize_p        Size of the buffer
\param[in] pUserArg_p       The user argument

\retval TRUE        Successfully written to object list
\retval FALSE       Error while writing to object list
*/
/*----------------------------------------------------------------------------*/
static BOOL cc_handleOccRxObjects(UINT8* pBuffer_p, UINT16 bufSize_p,
        void* pUserArg_p)
{
    BOOL fReturn = FALSE;
    tCcWriteState  writeState;
    tTbufCcStructure*  pOccBuff;

    UNUSED_PARAMETER(bufSize_p);
    UNUSED_PARAMETER(pUserArg_p);

    /* Convert to configuration channel buffer structure */
    pOccBuff = (tTbufCcStructure*) pBuffer_p;

    /* Increment transmit timer cycle count */
    timeout_incrementCounter(ccInstance_l.txChannel_m.pTimeoutInst_m);

    /* Forward receive objects to local list */
    writeState = ccobject_writeCurrObject(pOccBuff->objIdx_m, pOccBuff->objSubIdx_m,
                 (UINT8*)&pOccBuff->objPayloadLow_m);
    if(writeState == kCcWriteStateSuccessful)
    {
        /* Increment local object write pointer */
        ccobject_incObjWritePointer();

        fReturn = TRUE;
    }
    else
    {
        /* Don't update object and wait for sync again */
        fReturn = TRUE;
    }

    return fReturn;
}

/**
 * \}
 */

#endif /* #if (((PSI_MODULE_INTEGRATION) & (PSI_MODULE_CC)) != 0) */

/**
 * \}
 */
