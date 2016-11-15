/**
********************************************************************************
\file   rssdo.c

\brief  Creates a channel for receiving SSDO frames

This module handles the receive SSDO channels between the PCP and the
application.
This receive channel forwards the SSDO payload from the POWERLINK network
to the application.

\ingroup module_ssdo
*******************************************************************************/

/*------------------------------------------------------------------------------
* License Agreement
*
* Copyright 2014 BERNECKER + RAINER, AUSTRIA, 5142 EGGELSBERG, B&R STRASSE 1
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

#include <psi/internal/rssdo.h>
#include <psi/rssdo.h>

#include <psi/status.h>

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

extern tOplkError rssdo_obdAccessCb(tObdAlConHdl* pParam_p);

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define SSDO_RECEIVE_FIFO_ELEM_COUNT       5      ///< Number of elements in the receive FIFO
#define SSDO_RX_TIMEOUT_CYCLE_COUNT        400    ///< Number of cycles after a transmit has a timeout

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static struct  eRssdoInstance       rssdoInstance_l[kNumSsdoInstCount];

static UINT8   nodeId_l = 0;            ///< Node id of the CN
static UINT16  idxSsdoStub_l = 0;       ///< Index of the SSDO stub object
static UINT16  idxSsdoStubData_l = 0;   ///< Index of the SSDO stub data object

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static tPsiStatus processReceiveSm(tRssdoInstance pInstance_p);
static tRssdoInstance getInstance(tSsdoChanNum  chanNum_p );
static tPsiStatus handleReceiveFrame(tRssdoInstance pInstance_p,
        UINT8* pRxBuffer_p, UINT32 buffSize_p);
static tPsiStatus postToBuffer(tRssdoInstance pInstance_p,
        UINT8* pMsgBuffer_p, UINT16 buffSize_p);
static void changeLocalSeqNr(tSeqNrValue* pSeqNr_p);
static tPsiStatus checkChannelStatus(tRssdoInstance pInstance_p);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Initialize the SSDO channel module

\param[in] nodeId_p             Node id of the Cn
\param[in] idxSsdoStub_p        Object index of the SSDO stub
\param[in] idxSsdoStubData_p    Object index of the SSDO stub address

\ingroup module_ssdo
*/
//------------------------------------------------------------------------------
void rssdo_init(UINT8 nodeId_p, UINT16 idxSsdoStub_p, UINT16 idxSsdoStubData_p)
{
    // Remember stub object addresses
    nodeId_l = nodeId_p;
    idxSsdoStub_l = idxSsdoStub_p;
    idxSsdoStubData_l = idxSsdoStubData_p;

    PSI_MEMSET(&rssdoInstance_l, 0 , sizeof(struct eRssdoInstance) * kNumSsdoInstCount);
}

//------------------------------------------------------------------------------
/**
\brief    Create a SSDO receive channel instance

Instantiate a new SSDO receive channel which grabs the payload from the
obdict and forwards it to the triple buffers.

\param[in]  pInitParam_p     SSDO initialization structure

\retval Address              Pointer to the instance of the channel
\retval NULL                 Unable to allocate instance

\ingroup module_ssdo
*/
//------------------------------------------------------------------------------
tRssdoInstance rssdo_create(tRssdoInitStruct* pInitParam_p)
{
    tRssdoInstance     pInstance = NULL;
    tTbufInitStruct    tbufInitParam;

#if _DEBUG
    if(pInitParam_p->tbufRxSize_m != sizeof(tTbufSsdoRxStructure))
    {
        goto Exit;
    }

    if(pInitParam_p->chanId_m > (kNumSsdoInstCount - 1))
    {
        goto Exit;
    }
#endif

    // init the receive triple buffer module
    tbufInitParam.id_m = pInitParam_p->tbufRxId_m;
    tbufInitParam.pAckBase_m = pInitParam_p->pProdAckBase_m;
    tbufInitParam.pBase_m = (UINT8 *)pInitParam_p->pTbufRxBase_m;
    tbufInitParam.size_m = pInitParam_p->tbufRxSize_m;

    rssdoInstance_l[pInitParam_p->chanId_m].pTbufProdRxInst_m = tbuf_create(&tbufInitParam);
    if(rssdoInstance_l[pInitParam_p->chanId_m].pTbufProdRxInst_m == NULL)
    {
        goto Exit;
    }

    // Initialize the frame receive FIFO
    rssdoInstance_l[pInitParam_p->chanId_m].pRxFifoInst_m = fifo_create(
            sizeof(rssdoInstance_l[pInitParam_p->chanId_m].prodRecvBuff_m.ssdoStubDataBuff_m),
            SSDO_RECEIVE_FIFO_ELEM_COUNT);
    if(rssdoInstance_l[pInitParam_p->chanId_m].pRxFifoInst_m == NULL)
    {
        goto Exit;
    }

    // Create timeout module for local ssdo receive channel
    rssdoInstance_l[pInitParam_p->chanId_m].pTimeoutInst_m = timeout_create(
            SSDO_RX_TIMEOUT_CYCLE_COUNT);
    if(rssdoInstance_l[pInitParam_p->chanId_m].pTimeoutInst_m == NULL)
    {
        goto Exit;
    }

    // Remember channel id
    rssdoInstance_l[pInitParam_p->chanId_m].instId_m = pInitParam_p->chanId_m;

    // Set initial receive state
    rssdoInstance_l[pInitParam_p->chanId_m].prodRxState_m = kProdRxStateWaitForFrame;

    // Set sequence number init value
    rssdoInstance_l[pInitParam_p->chanId_m].currProdSeq_m = kSeqNrValueFirst;

    // Set valid instance id
    pInstance = &rssdoInstance_l[pInitParam_p->chanId_m];

Exit:
    return pInstance;
}

//------------------------------------------------------------------------------
/**
\brief    Destroy a receive SSDO channel

\param[in] pInstance_p           Pointer to the instance

\ingroup module_ssdo
*/
//------------------------------------------------------------------------------
void rssdo_destroy(tRssdoInstance pInstance_p)
{
    if(pInstance_p != NULL)
    {
        tbuf_destroy(pInstance_p->pTbufProdRxInst_m);

        // Destroy receive FIFO instance
        fifo_destroy(pInstance_p->pRxFifoInst_m);

        // Destroy the timeout module for the ssdo rx channel
        timeout_destroy(pInstance_p->pTimeoutInst_m);
    }
}

//------------------------------------------------------------------------------
/**
\brief    Process incoming SDO access

Background task of the incoming SDO access. Read the data from the triple buffer
and forward it to the POWERLINK stack.

\param[in] pInstance_p           Pointer to the instance

\retval kPsiSuccessful                    On success
\retval kPsiSsdoProcessingFailed         Unable to process SDO access
\retval kPsiConfChanWriteToObDictFailed   Unable to forward object to stack

\ingroup module_ssdo
*/
//------------------------------------------------------------------------------
tPsiStatus rssdo_process(tRssdoInstance pInstance_p)
{
    tPsiStatus ret = kPsiSuccessful;
    tPsiStatus chanRet = kPsiSuccessful;
    tTimerStatus   timerState;

    if(pInstance_p == NULL)
    {
        ret = kPsiSsdoProcessingFailed;
        goto Exit;
    }

    // Process outgoing frames
    ret = processReceiveSm(pInstance_p);
    if(ret != kPsiSuccessful)
    {
        goto Exit;
    }

    // Check if timeout counter is expired
    timerState = timeout_checkExpire(pInstance_p->pTimeoutInst_m);
    if(timerState == kTimerStateExpired)
    {
        // Timeout occurred -> Increment local sequence number!
        changeLocalSeqNr(&pInstance_p->currProdSeq_m);

        ret = kPsiSuccessful;
        goto Exit;
    }

    // Check if message is acknowledged -> Then reset timer!
    timerState = timeout_isRunning(pInstance_p->pTimeoutInst_m);
    if(timerState == kTimerStateRunning)
    {
        chanRet = checkChannelStatus(pInstance_p);
        if(chanRet == kPsiSuccessful)
        {
            timeout_stopTimer(pInstance_p->pTimeoutInst_m);
        }
    }

Exit:
    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Process the synchronous task of the receive SSDO channel

\param[in] pInstance_p           Pointer to the instance

\retval kPsiSuccessful               On success
\retval kPsiSsdoInvalidParameter     On an invalid instance

\ingroup module_ssdo
*/
//------------------------------------------------------------------------------
tPsiStatus rssdo_processSync(tRssdoInstance pInstance_p)
{
    tPsiStatus ret = kPsiSuccessful;

    if(pInstance_p == NULL)
    {
        ret = kPsiSsdoInvalidParameter;
        goto Exit;
    }

    // Increment cycle count transmit timer
    timeout_incrementCounter(pInstance_p->pTimeoutInst_m);

Exit:
    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    SSDO module object access occurred

In case of an access to the local obdict.h of objects which are SSDO
channel objects the payload needs to be forwarded to the triple buffer.

\param[in] pParam_p               Object access parameter

\retval  kErrorOk                   On success
\retval  kErrorApiInvalidParam      Invalid parameter passed to function
\retval  kErrorObdIndexNotExist     The index of the object is not existing
\retval  kErrorObdSubindexNotExist  The subindex of the object is not existing
\retval  kErrorObdValueLengthError  The size of the object is invalid
\retval  kErrorObdAccessViolation   Error on accessing the object

\ingroup module_ssdo
*/
//------------------------------------------------------------------------------
tOplkError rssdo_obdAccessCb(tObdAlConHdl* pParam_p)
{
    tPsiStatus ret = kPsiSuccessful;
    tOplkError oplkret = kErrorOk;
    tRssdoInstance  pInstance;

    if(pParam_p == NULL)
    {
        oplkret = kErrorApiInvalidParam;
        goto Exit;
    }

    // Check if callback is on the right object
    if(pParam_p->index != idxSsdoStubData_l)
    {
        oplkret = kErrorObdIndexNotExist;
        goto Exit;
    }

    // Find instance handle
    pInstance = getInstance(pParam_p->subIndex - 1);
    if(pInstance == NULL)
    {
        oplkret = kErrorObdSubindexNotExist;
        goto Exit;
    }

    if(pParam_p->totalPendSize > SSDO_STUB_DATA_DOM_SIZE)
    {
        return kErrorObdValueLengthError;
    }
    else
    {
        // Save object size for later use
        pInstance->objSize_m = pParam_p->totalPendSize;
    }

    // Post frame to receive FIFO
    ret = fifo_insertElement(pInstance->pRxFifoInst_m,
                             (tFifoElement) pParam_p->pSrcData,
                             pInstance->objSize_m);
    if(ret != kPsiSuccessful)
    {
        oplkret = kErrorObdAccessViolation;
    }
    else
    {
        // Start timeout timer for this frame!
        timeout_startTimer(pInstance->pTimeoutInst_m);
    }

Exit:
    return oplkret;
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

//------------------------------------------------------------------------------
/**
\brief    Process the frame receive state machine

Implements the SSDO receive state machine. Reads the frame from the
FIFO and forwards it to the triple buffer.

\param[in] pInstance_p               Pointer to the local instance

\retval  kPsiSuccessful            On success
\retval  kPsiSsdoInvalidState     Invalid state machine state

\ingroup module_ssdo
*/
//------------------------------------------------------------------------------
static tPsiStatus processReceiveSm(tRssdoInstance pInstance_p)
{
    tPsiStatus ret = kPsiSuccessful;

    switch(pInstance_p->prodRxState_m)
    {
        case kProdRxStateWaitForFrame:
        {
            // Get frame from receive FIFO
            ret = fifo_getElement(pInstance_p->pRxFifoInst_m,
                    (tFifoElement)&pInstance_p->prodRecvBuff_m.ssdoStubDataBuff_m,
                    &pInstance_p->prodRecvBuff_m.ssdoStubSize_m);
            switch(ret)
            {
                case kPsiFifoEmpty:
                {
                    // Nothing to do -> Check again later!
                    ret = kPsiSuccessful;
                    goto Exit;
                }
                case kPsiSuccessful:
                {
                    ret = handleReceiveFrame(pInstance_p,
                            pInstance_p->prodRecvBuff_m.ssdoStubDataBuff_m,
                            pInstance_p->prodRecvBuff_m.ssdoStubSize_m);
                    if(ret == kPsiSuccessful)
                    {
                        // Frame posted successfully -> Get next frame!
                        pInstance_p->prodRxState_m = kProdRxStateWaitForFrame;
                    }
                    else if(kPsiSsdoNoFreeBuffer)
                    {
                        // No buffer is not a problem -> Retry until buffer gets ACK from AP!
                        ret = kPsiSuccessful;
                        pInstance_p->prodRxState_m = kProdRxStateRepostFrame;
                    }

                    break;
                }
                default:
                {
                    // Internal FIFO error occurred
                    goto Exit;
                }
            }
            break;
        }
        case kProdRxStateRepostFrame:
        {
            // Try to post frame to local triple buffer
            ret = handleReceiveFrame(pInstance_p,
                    pInstance_p->prodRecvBuff_m.ssdoStubDataBuff_m,
                    pInstance_p->prodRecvBuff_m.ssdoStubSize_m);
            if(ret == kPsiSuccessful)
            {
                // Frame posted successfully -> Activate blocking mechanism
                pInstance_p->prodRxState_m = kProdRxStateWaitForFrame;
            }
            else if(kPsiSsdoNoFreeBuffer)
            {
                // No buffer is not a problem -> Retry until buffer gets ACK from AP!
                ret = kPsiSuccessful;
            }

            break;
        }
        default:
        {
            ret = kPsiSsdoInvalidState;
            break;
        }
    }

Exit:
    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Get instance handler from channel number


\param[in] chanNum_p             Channel number of the instance

\retval  Address        Pointer to the instance
\retval  NULL           On invalid channel number

\ingroup module_ssdo
*/
//------------------------------------------------------------------------------
static tRssdoInstance getInstance(tSsdoChanNum  chanNum_p )
{
    tRssdoInstance  pInstance = NULL;

    if(chanNum_p < kNumSsdoInstCount )
    {
        // Set pointer to instance
        pInstance = &rssdoInstance_l[chanNum_p];
    }

    return pInstance;
}

//------------------------------------------------------------------------------
/**
\brief    Forward receive frame to buffer

\param[in] pInstance_p             Pointer to the local instance
\param[in] pRxBuffer_p             Pointer to the receive buffer
\param[in] buffSize_p              Size of the receive buffer


\retval  kPsiSuccessful              On success
\retval  kPsiTbuffWriteError         Unable to write data to buffer

\ingroup module_ssdo
*/
//------------------------------------------------------------------------------
static tPsiStatus handleReceiveFrame(tRssdoInstance pInstance_p,
        UINT8* pRxBuffer_p, UINT32 buffSize_p)
{
    tPsiStatus ret = kPsiSuccessful;

    // Check if channel is ready for transmission
    ret = checkChannelStatus(pInstance_p);
    if(ret == kPsiSsdoChannelBusy)
    {
        ret = kPsiSuccessful;
        goto Exit;
    }

    // Forward payload to buffer
    ret = postToBuffer(pInstance_p, pRxBuffer_p, (UINT16)buffSize_p);
    if(ret != kPsiSuccessful)
    {
        goto Exit;
    }

    // Increment local sequence number
    changeLocalSeqNr(&pInstance_p->currProdSeq_m);

    // Set sequence number
    ret = tbuf_writeByte(pInstance_p->pTbufProdRxInst_m,
            TBUF_SSDORX_SEQNR_OFF, pInstance_p->currProdSeq_m);
    if(ret != kPsiSuccessful)
    {
        goto Exit;
    }

    // Acknowledge ssdo producing receive triple buffer
    ret = tbuf_setAck(pInstance_p->pTbufProdRxInst_m);
    if(ret != kPsiSuccessful)
    {
        goto Exit;
    }

Exit:
    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Post received frame to buffer

\param[in] pInstance_p             Pointer to the local instance
\param[in] pMsgBuffer_p            Pointer to the payload
\param[in] buffSize_p              Size of the payload

\retval  kPsiSuccessful       On success

\ingroup module_ssdo
*/
//------------------------------------------------------------------------------
static tPsiStatus postToBuffer(tRssdoInstance pInstance_p,
        UINT8* pMsgBuffer_p, UINT16 buffSize_p)
{
    tPsiStatus ret = kPsiSuccessful;

    // Write payload field
    ret = tbuf_writeWord(pInstance_p->pTbufProdRxInst_m,
            TBUF_SSDORX_PAYLSIZE_OFF, buffSize_p);
    if(ret != kPsiSuccessful)
    {
        goto Exit;
    }

    // Post frame to buffer
    ret = tbuf_writeStream(pInstance_p->pTbufProdRxInst_m,
            TBUF_SSDORX_SSDO_STUB_DATA_DOM_OFF, pMsgBuffer_p, buffSize_p);
    if(ret != kPsiSuccessful)
    {
        goto Exit;
    }

Exit:
    return ret;

}

//------------------------------------------------------------------------------
/**
\brief    Change local sequence number

\param[out] pSeqNr_p        Sequence number to change

\ingroup module_ssdo
*/
//------------------------------------------------------------------------------
static void changeLocalSeqNr(tSeqNrValue* pSeqNr_p)
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

//------------------------------------------------------------------------------
/**
\brief    Check if channel is ready for transmission

\param[in]  pInstance_p             Pointer to the local instance

\retval kPsiSuccessful        Channel is free for transmission
\retval kPsiSsdoChannelBusy  Channel is currently transmitting

\ingroup module_ssdo
*/
//------------------------------------------------------------------------------
static tPsiStatus checkChannelStatus(tRssdoInstance pInstance_p)
{
    tPsiStatus ret = kPsiSuccessful;
    tSeqNrValue  seqNr = kSeqNrValueInvalid;

    // Get status of transmit channel
    status_getSsdoProdChanFlag(pInstance_p->instId_m, &seqNr);

    // Check if old transmission is already finished!
    if(seqNr != pInstance_p->currProdSeq_m)
    {
        // Message in progress -> retry later!
        ret = kPsiSsdoChannelBusy;
    }

    return ret;
}

/// \}


