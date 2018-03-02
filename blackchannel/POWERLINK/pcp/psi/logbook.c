/**
********************************************************************************
\file   logbook.c

\brief  Creates a logbook channel for transmitting a log entry

This module handles the logbook channels between the PCP and the application.
It receives logbook entries from the application, reformats the message
and forwards the data to the target in the LOGStub.

\ingroup module_log
*******************************************************************************/

/*------------------------------------------------------------------------------
* License Agreement
*
* Copyright (c) 2014, B&R Industrial Automation GmbH
* Copyright (c) 2016, Kalycito Infotech Private Ltd
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

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <psi/internal/logbook.h>
#include <psi/logbook.h>

#include <psi/status.h>
#include <libpsicommon/ami.h>

#include <limits.h>


//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define LOG_ARP_TIMEOUT_CYCLE_COUNT       50     ///< Number of cycles after an ARP request is retried

#define BUR_ERROR_LEVEL_INFO        0x1     ///< Info error level by BuR
#define BUR_ERROR_LEVEL_WARNING     0x2     ///< WARNING error level by BuR
#define BUR_ERROR_LEVEL_ERROR       0x3     ///< ERROR error level by BuR

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static struct  eLogInstance       logInstance_l[kNumLogInstCount];

static UINT8   nodeId_l = 0;                ///< Node id of the CN
static UINT16  idxLogStub_l = 0;            ///< Index of the LOGstub object
static tNetTime * pNetTime_l;               ///< The current POWERLINK nettime
static tPsiCritSec pfnCritSec_l = NULL;   ///< Function pointer to the critical section entry

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static tPsiStatus processTransmitSm(tLogInstance pInstance_p);
static tPsiStatus getTargetNode(tLogInstance pInstance_p,
        UINT8* pTargNode_p, UINT16* pTargIdx_p, UINT8* pTargSubIdx_p);
static tPsiStatus sendToDestTarget(tLogInstance pInstance_p,
        UINT8* pTargNode_p, UINT16* pTargIdx_p, UINT8* pTargSubIdx_p,
        UINT8* pMsgBuffer_p, UINT16 buffSize_p);
static tPsiStatus verifyTargetInfo(UINT8 targNode_p, UINT16 targIdx_p,
        UINT8 targSubIdx_p);
static tPsiStatus reformatLogEntry(tBuRLogEntry * pBurLog_p,
                                     tLogFormat * pLogEntry_p,
                                     UINT32 * pEntryCnt_p);
static UINT64 convertNetTime(tNetTime * pNetTime_p);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Initialize the logbook channel module

\param[in] nodeId_p             Node id of the Cn
\param[in] idxLogStub_p         Object index of the log stub
\param[in] pfnCritSec_p         Pointer to the critical section

\ingroup module_log
*/
//------------------------------------------------------------------------------
void log_init(UINT8 nodeId_p, UINT16 idxLogStub_p, tPsiCritSec pfnCritSec_p)
{
    // Remember stub object addresses
    nodeId_l = nodeId_p;
    idxLogStub_l = idxLogStub_p;
    pfnCritSec_l = pfnCritSec_p;

    PSI_MEMSET(&logInstance_l, 0, sizeof(struct eLogInstance) * kNumLogInstCount);
}

//------------------------------------------------------------------------------
/**
\brief    Create a logger channel instance

Instantiate a new logger channel which a transmit channel for logger
information.

\param[in]  pInitParam_p          Logbook initialization structure

\return tLogInstance
\retval Address              Pointer to the instance of the channel
\retval Null                 Unable to allocate instance

\ingroup module_log
*/
//------------------------------------------------------------------------------
tLogInstance log_create(tLogInitStruct* pInitParam_p)
{
    tLogInstance     pInstance = NULL;
    tTbufInitStruct  tbufInitParam;

#if _DEBUG
    if (pInitParam_p->tbufTxSize_m != sizeof(tTbufLogStructure))
    {
        goto Exit;
    }

    if (pInitParam_p->chanId_m > (kNumLogInstCount - 1))
    {
        goto Exit;
    }
#endif

    // init the transmit triple buffer module
    tbufInitParam.id_m = pInitParam_p->tbufTxId_m;
    tbufInitParam.pAckBase_m = pInitParam_p->pConsAckBase_m;
    tbufInitParam.pBase_m = (UINT8 *)pInitParam_p->pTbufTxBase_m;
    tbufInitParam.size_m = pInitParam_p->tbufTxSize_m;

    logInstance_l[pInitParam_p->chanId_m].pTbufConsTxInst_m = tbuf_create(&tbufInitParam);
    if (logInstance_l[pInitParam_p->chanId_m].pTbufConsTxInst_m == NULL)
    {
        goto Exit;
    }

    // Create timeout module for ARP retry count
    logInstance_l[pInitParam_p->chanId_m].pArpTimeoutInst_m = timeout_create(
            LOG_ARP_TIMEOUT_CYCLE_COUNT);
    if (logInstance_l[pInitParam_p->chanId_m].pArpTimeoutInst_m == NULL)
    {
        goto Exit;
    }

    // Remember channel id
    logInstance_l[pInitParam_p->chanId_m].instId_m = pInitParam_p->chanId_m;

    // Set initial transmit state
    logInstance_l[pInitParam_p->chanId_m].consTxState_m = kConsTxStateWaitForFrame;

    // Set invalid SDO instance
    logInstance_l[pInitParam_p->chanId_m].sdoComConHdl_m = UINT_MAX;

    // Set valid instance id
    pInstance = &logInstance_l[pInitParam_p->chanId_m];

Exit:
    return pInstance;
}

//------------------------------------------------------------------------------
/**
\brief    Destroy a logbook channel

\param[in] pInstance_p           Pointer to the instance

\ingroup module_log
*/
//------------------------------------------------------------------------------
void log_destroy(tLogInstance pInstance_p)
{
    if (pInstance_p != NULL)
    {
        tbuf_destroy(pInstance_p->pTbufConsTxInst_m);

        // Destroy the timeout module for the arp retry counter
        timeout_destroy(pInstance_p->pArpTimeoutInst_m);
    }
}

//------------------------------------------------------------------------------
/**
\brief    Process incoming SDO access

Background task of the incoming SDO access. Read the data from the triple buffer
and forward it to the POWERLINK stack.

\param[in] pInstance_p           Pointer to the instance

\return tPsiStatus
\retval kPsiSuccessful                On success
\retval kPsiLogProcessingFailed       Unable to process SDO access
\retval kPsiLogWriteToObDictFailed    Unable to forward object to stack

\ingroup module_log
*/
//------------------------------------------------------------------------------
tPsiStatus log_process(tLogInstance pInstance_p)
{
    tPsiStatus ret = kPsiSuccessful;

    if (pInstance_p == NULL)
    {
        ret = kPsiLogProcessingFailed;
        goto Exit;
    }

    // Process incoming frames
    ret = processTransmitSm(pInstance_p);
    if (ret != kPsiSuccessful)
    {
        goto Exit;
    }

Exit:
    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Update the current nettime

\param[in] pNetTime_p           Pointer to the current time information

\retval kPsiSuccessful            On success
\retval kPsiLogNettimeInvalid     Nettime not set

\ingroup module_log
*/
//------------------------------------------------------------------------------
tPsiStatus log_setNettime(tNetTime * pNetTime_p)
{
    tPsiStatus ret = kPsiSuccessful;

    if (pNetTime_p != NULL)
        pNetTime_l = pNetTime_p;
    else
        ret = kPsiLogNettimeInvalid;

    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Frees the SDO Channel

\param[in] pInstance_p          Pointer to the instance

\return tPsiStatus
\retval kPsiSuccessful        On successful
\retval kPsiInvalidHandle     Passed handle invalid

\ingroup module_ssdo
*/
//------------------------------------------------------------------------------
tPsiStatus log_closeSdoChannel(tLogInstance pInstance_p)
{
    tPsiStatus ret = kPsiSuccessful;
    tOplkError oplkret = kErrorOk;

    // Nothing to free, if not used before
    if (pInstance_p->sdoComConHdl_m == UINT_MAX)
        return ret;

    // Abort SDO and free the channel
    oplk_abortSdo(pInstance_p->sdoComConHdl_m, SDO_AC_TIME_OUT);
    oplkret = oplk_freeSdoChannel(pInstance_p->sdoComConHdl_m);

    // Wrap oplk return value to tPsiStatus return value
    switch (oplkret)
    {
        case kErrorOk:
        {
            pInstance_p->sdoComConHdl_m = UINT_MAX;
            break;
        }

        case kErrorSdoSeqInvalidHdl:
        case kErrorSdoComInvalidHandle:
        {
           ret = kPsiInvalidHandle;
           break;
        }
        default:
        {
            ret = kPsiLogCloseSdoConError;
            break;
        }
    }
    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Handle finished SDO transfer

\param[in] pInstance_p           Pointer to the instance

\return kPsiSuccessful

\ingroup module_log
*/
//------------------------------------------------------------------------------
tPsiStatus log_consTxTransferFinished(tLogInstance pInstance_p)
{
    tPsiStatus ret = kPsiSuccessful;

    // set logbook status register flag to current sequence flag
    status_setLogConsChanFlag(pInstance_p->instId_m, pInstance_p->currConsSeq_m);

    // Set state machine to wait for next frame
    pInstance_p->consTxState_m = kConsTxStateWaitForFrame;

    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Handle incoming logbook payload

Handle incoming data from the triple buffers by checking the sequence number.
If data has arrived forward set the channel blocking and forward it to the
background task.
(This function is called in interrupt context)

\param[in] pInstance_p           Pointer to the instance

\return tPsiStatus
\retval kPsiSuccessful               On success
\retval kPsiLogInvalidParameter     On an invalid instance
\retval other error

\ingroup module_log
*/
//------------------------------------------------------------------------------
tPsiStatus log_handleIncoming(tLogInstance pInstance_p)
{
    tPsiStatus ret = kPsiSuccessful;
    tSeqNrValue  currSeqNr = kSeqNrValueInvalid;

    if (pInstance_p == NULL)
    {
        ret = kPsiLogInvalidParameter;
        goto Exit;
    }

    // Increment cycle counter for ARP retry timer
    timeout_incrementCounter(pInstance_p->pArpTimeoutInst_m);

    if (pInstance_p->consTxState_m != kConsTxStateWaitForFrame)
    {
        // Object access is currently in progress -> do nothing here!
        goto Exit;
    }

    // Acknowledge consuming buffer
    ret = tbuf_setAck(pInstance_p->pTbufConsTxInst_m);
    if (ret != kPsiSuccessful)
    {
        goto Exit;
    }

    // Get current sequence number
    ret = tbuf_readByte(pInstance_p->pTbufConsTxInst_m,
                        TBUF_LOG_SEQNR_OFF, (UINT8 *)&currSeqNr);
    if (ret != kPsiSuccessful)
    {
        goto Exit;
    }

    // Check sequence number sanity
    if (currSeqNr != kSeqNrValueFirst && currSeqNr != kSeqNrValueSecond)
    {
        goto Exit;
    }

    if (currSeqNr != pInstance_p->currConsSeq_m)
    {
        // Switch to state process frame
        pInstance_p->consTxState_m = kConsTxStateProcessFrame;

        // Increment local sequence number
        pInstance_p->currConsSeq_m = currSeqNr;
    }

Exit:
    return ret;
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

//------------------------------------------------------------------------------
/**
\brief    Process the frame transmit state machine

Implements the logbook transmit state machine. Reads the frame from the
triple buffer and forwards it to the target node.

\param[in] pInstance_p               Pointer to the local instance

\return  tPsiStatus
\retval  kPsiSuccessful          On success
\retval  kPsiLogInvalidState     Invalid state machine state

\ingroup module_log
*/
//------------------------------------------------------------------------------
static tPsiStatus processTransmitSm(tLogInstance pInstance_p)
{
    tPsiStatus ret = kPsiSuccessful;
    tTimerStatus timerState;
    static UINT8        targNode;
    static UINT16       targIdx;
    static UINT8        targSubIdx;
    tLogFormat*  pLogData;

    // Process logbook channel
    switch (pInstance_p->consTxState_m)
    {
        case kConsTxStateWaitForFrame:
        {
            // Wait for new frame -> do nothing here!
            break;
        }
        case kConsTxStateProcessFrame:
        {
            // Get data pointer to local buffer
            ret = tbuf_getDataPtr(pInstance_p->pTbufConsTxInst_m,
                                  TBUF_LOG_DATA_OFF,
                                  (UINT8**)&pLogData);
            if (ret != kPsiSuccessful)
            {
                goto Exit;
            }

            // Get target node for incoming message!
            ret = getTargetNode(pInstance_p, &targNode, &targIdx, &targSubIdx);
            if (ret != kPsiSuccessful)
            {
                goto Exit;
            }

            // Verify target node
            ret = verifyTargetInfo(targNode, targIdx, targSubIdx);
            if (ret != kPsiSuccessful)
            {
                // TODO Signal error back to application
                ret = kPsiSuccessful;
                goto Exit;
            }

            // Adapt logging message to fit to BuR style
            ret = reformatLogEntry(&pInstance_p->burLogEntry_m, pLogData, &pInstance_p->entryCount_m);
            if (ret != kPsiSuccessful)
            {
                ret = kPsiLogEntryReformatFailed;
                goto Exit;
            }

            // Forward object access to target node
            ret = sendToDestTarget(pInstance_p, &targNode, &targIdx, &targSubIdx,
                    (UINT8*)&pInstance_p->burLogEntry_m, sizeof(tBuRLogEntry));
            if (ret != kPsiSuccessful)
            {
                goto Exit;
            }

            break;
        }
        case kConsTxStateWaitForTxFinished:
        {
            // Wait until transfer is finished -> Do nothing here!
            break;
        }
        case kConsTxStateWaitForNextArpRetry:
        {
            // Check if the timer is expired
            timerState = timeout_checkExpire(pInstance_p->pArpTimeoutInst_m);
            if (timerState == kTimerStateExpired)
            {
                pInstance_p->consTxState_m = kConsTxStateProcessFrame;
                timeout_stopTimer(pInstance_p->pArpTimeoutInst_m);
            }

            break;
        }
        case kConsTxStateRetransmitCurrentMessage:
        {
            // Forward object access to target node
            ret = sendToDestTarget(pInstance_p, &targNode, &targIdx, &targSubIdx,
                    (UINT8*)&pInstance_p->burLogEntry_m, sizeof(tBuRLogEntry));
            if (ret != kPsiSuccessful)
            {
                goto Exit;
            }

            break;
        }
        default:
        {
            ret = kPsiLogInvalidState;
            break;
        }
    }

Exit:
    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Read target information from local OD

Read nodeId, index and subindex from the local object dictionary.

\param[in]  pInstance_p             Pointer to the local instance
\param[out] pTargNode_p             The target node
\param[out] pTargIdx_p              The target object index
\param[out] pTargSubIdx_p           The target object subindex

\return  tPsiStatus
\retval  kPsiSuccessful              On success
\retval  kPsiLogDestinationUnknown   Unable to find target in object dictionary

\ingroup module_log
*/
//------------------------------------------------------------------------------
static tPsiStatus getTargetNode(tLogInstance pInstance_p,
        UINT8* pTargNode_p, UINT16* pTargIdx_p, UINT8* pTargSubIdx_p)
{
    tOplkError oplkret = kErrorOk;
    tPsiStatus ret = kPsiSuccessful;
    UINT32   accTargNode;
    UINT32   accTargSize = sizeof(accTargNode);

    // Read LOG-Stub object to get target node, idx and subindex
    oplkret = oplk_readLocalObject(idxLogStub_l, pInstance_p->instId_m + 1,
            &accTargNode, &accTargSize);
    if (oplkret != kErrorOk)
    {
        ret = kPsiLogDestinationUnknown;
        goto Exit;
    }

    // Acquire target node parameters
    *pTargIdx_p = (UINT16)(accTargNode & 0xFFFF);
    *pTargSubIdx_p = (UINT8)((accTargNode >> 16) & 0xFF);
    *pTargNode_p = (UINT8)((accTargNode >> 24) & 0xFF);

Exit:
    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Forward object access to target

This function calls oplk_writeObject which transfers the object access to
the destination target.

\param[in] pInstance_p             Pointer to the local instance
\param[in] pTargNode_p             The target node
\param[in] pTargIdx_p              The target object index
\param[in] pTargSubIdx_p           The target object subindex
\param[in] pMsgBuffer_p            Pointer to the message to transfer
\param[in] buffSize_p              Size of the message to transfer

\return  tPsiStatus
\retval  kPsiSuccessful                  On success
\retval  kPsiLogWriteToObDictFailed      Unable to send data to target

\ingroup module_log
*/
//------------------------------------------------------------------------------
static tPsiStatus sendToDestTarget(tLogInstance pInstance_p,
        UINT8* pTargNode_p, UINT16* pTargIdx_p, UINT8* pTargSubIdx_p,
        UINT8* pMsgBuffer_p, UINT16 buffSize_p)
{
    tPsiStatus ret = kPsiSuccessful;
    tOplkError   oplkret = kErrorOk;

    // Write incoming message to target node
    oplkret = oplk_writeObject(&pInstance_p->sdoComConHdl_m,
            *pTargNode_p, *pTargIdx_p, *pTargSubIdx_p, // Target node, index and subindex
            pMsgBuffer_p,                              // Object data to transfer
            buffSize_p,                                // Object size to transfer
            kSdoTypeUdp,                               // Type of SDO carrier (Always use UDP!)
            (void *)pInstance_p);                      // User argument is the instance pointer

    switch (oplkret)
    {
        case kErrorApiTaskDeferred:
        {
            pInstance_p->consTxState_m = kConsTxStateWaitForTxFinished;
            break;
        }
        case kErrorOk:
        {
            // Transfer is finished -> Free channel!
            ret = log_consTxTransferFinished(pInstance_p);
            break;
        }

        case kErrorSdoUdpArpInProgress:
        {
            // ARP table is still not updated -> Retry to transmit the frame later!
            timeout_startTimer(pInstance_p->pArpTimeoutInst_m);

            pInstance_p->consTxState_m = kConsTxStateWaitForNextArpRetry;

            break;
        }
        case  kErrorSdoComHandleBusy:
        {
            // Handle is busy -> try to retransmit later!
            pInstance_p->consTxState_m = kConsTxStateRetransmitCurrentMessage;
            break;
        }
        default:
        {
            // Other error happened
            ret = kPsiLogWriteToObDictFailed;
            break;
        }
    }

    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Check target node information

\param[in]  targNode_p             Node id of the target
\param[in]  targIdx_p              Object index of the target
\param[in]  targSubIdx_p           Object subindex of the target

\return tPsiStatus
\retval kPsiSuccessful              Target information valid
\retval kPsiLogInvalidTargetInfo  Invalid target information

\ingroup module_log
*/
//------------------------------------------------------------------------------
static tPsiStatus verifyTargetInfo(UINT8 targNode_p, UINT16 targIdx_p,
        UINT8 targSubIdx_p)
{
    tPsiStatus ret = kPsiSuccessful;

    if (targNode_p == 0 || targNode_p == nodeId_l)
    {
        ret = kPsiLogInvalidTargetInfo;
        goto Exit;
    }

    if (targIdx_p == 0 || targSubIdx_p == 0)
    {
        ret = kPsiLogInvalidTargetInfo;
        goto Exit;
    }

Exit:
    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Reformat log entry to B&R internal style

\param[in]  pInstance_p             Log instance handle
\param[in]  pBurLog_p               The logbook entry in the B&R format
\param[in]  pLogEntry_p             The logbook entry in the internal format

\retval kPsiSuccessful            Reformat of message successful

\ingroup module_log
*/
//------------------------------------------------------------------------------
static tPsiStatus reformatLogEntry(tBuRLogEntry * pBurLog_p,
                                     tLogFormat * pLogEntry_p,
                                     UINT32 * pEntryCount_p)
{
    tPsiStatus ret = kPsiSuccessful;
    UINT64 netTime = 0;

    (*pEntryCount_p)++;

    netTime = convertNetTime(pNetTime_l);

    ami_setUint8Le((UINT8*)&pBurLog_p->formatId_m, 2);
    ami_setUint32Le((UINT8*)&pBurLog_p->entryNumber_m, *pEntryCount_p);
    ami_setUint64Le((UINT8*)&pBurLog_p->timeStamp_m, netTime);
    ami_setUint16Le((UINT8*)&pBurLog_p->errCode_m, pLogEntry_p->code_m);
    ami_setUint16Le((UINT8*)&pBurLog_p->errInfo1_m, pLogEntry_p->source_m);
    ami_setUint32Le((UINT8*)&pBurLog_p->errInfo2_m, pLogEntry_p->addInfo_m);

    /* Convert error level to BuR type */
    switch ((tLogLevel)pLogEntry_p->level_m)
    {
        case kLogLevelInfo:
            ami_setUint8Le((UINT8*)&pBurLog_p->level_m, BUR_ERROR_LEVEL_INFO);
            break;
        case kLogLevelMinor:
            ami_setUint8Le((UINT8*)&pBurLog_p->level_m, BUR_ERROR_LEVEL_WARNING);
            break;
        case kLogLevelFatal:
            ami_setUint8Le((UINT8*)&pBurLog_p->level_m, BUR_ERROR_LEVEL_ERROR);
            break;
        default:
        {
            // Error
            ret = kPsiLogInvalidErrorLevel;
            break;
        }
    }

    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Convert the Nettime format to milliseconds

\param[in]  pNetTime_p             The nettime in IEEE1588 format

\return The nettime in ms

\ingroup module_log
*/
//------------------------------------------------------------------------------
static UINT64 convertNetTime(tNetTime * pNetTime_p)
{
    UINT64 netTimeMs = 0;

    if (pfnCritSec_l != NULL)
    {
        pfnCritSec_l(TRUE);
        /* Convert the nettime to strange ms format */
        netTimeMs = (pNetTime_p->nsec & 0x7FFFFFFF)/1000000 + (UINT64)pNetTime_p->sec*1000;
        pfnCritSec_l(FALSE);
    }

    return netTimeMs;
}

/// \}
