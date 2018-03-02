/**
********************************************************************************
\file   demo-sn-gpio/shnf/xcom.c

\defgroup module_sn_shnf_xcom Cross communication module
\{

\brief  uP-Master <-> uP-Slave cross communication

This module implements the functions of the cross communication which are
needed on the uP-Master and uP-Slave processor.

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
#include <shnf/xcom.h>
#include <shnf/xcomint.h>

#include <shnf/constime.h>

#include <sn/upserial.h>
#include <common/benchmark.h>

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
#define MAX_TIMEBASE_THRESHOLD    30      /**< Maximum time in us the local time can deviate between the two processors */


#define SAMPLE_VALS     100

/*----------------------------------------------------------------------------*/
/* local types                                                                */
/*----------------------------------------------------------------------------*/

typedef struct
{
    tXComTransParams transParams_m;     /**< The cross communication transmit parameters*/
    volatile BOOLEAN fWaitForRcv_m;     /**< This flag ensures that cross communication occurs in each cycle */
} tXComInstance;

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/
static tXComInstance xcomInstance_l SAFE_INIT_SEKTOR;

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static void transferFinished(void);
static void frameReceived(void);
static void transferError(void);

static BOOLEAN serialEnableReceive(void);

static BOOLEAN verifyTime(void);
static BOOLEAN verifyFlowCount(void);
static BOOLEAN verifySpdoCrcs(void);
static BOOLEAN verifySsdoSnmtCrcs(void);

static BOOLEAN isSpdoSet(void);
static BOOLEAN isSsdoSnmtSet(void);

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the xcom module

Initialize the cross communication module for both processors.

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
BOOLEAN xcom_init(void)
{
    BOOLEAN fReturn = FALSE;
    tXComTransParams transParam;

    MEMSET(&xcomInstance_l, 0, sizeof(tXComInstance));
    MEMSET(&transParam, 0, sizeof(tXComTransParams));

    /* Register all upserial callback functions */
    upserial_registerCb(transferFinished, frameReceived, transferError);

    /* Initialize the xcom internal module */
    if(xcomint_init(&transParam))
    {
        /* Store the transmit parameters from internal module */
        MEMCOPY(&xcomInstance_l.transParams_m, &transParam, sizeof(tXComTransParams));

        /* Initially enable the receiver of the serial device */
        if(serialEnableReceive())
        {
            fReturn = TRUE;
        }   /* no else: Error is reported in the called function */
    }   /* no else: Error is reported in the called function */

    return fReturn;
}


/*----------------------------------------------------------------------------*/
/**
\brief    Close the cross communication module
*/
/*----------------------------------------------------------------------------*/
void xcom_exit(void)
{
    /* Close the xcom internal module */
    xcomint_exit();

    MEMSET(&xcomInstance_l, 0, sizeof(tXComInstance));
}


/*----------------------------------------------------------------------------*/
/**
\brief    Set the current time to the output data stream
*/
/*----------------------------------------------------------------------------*/
BOOLEAN xcom_setCurrentTimebase(UINT64 * p_currTime)
{
    BOOLEAN fReturn = FALSE;

    if(p_currTime != NULL)
    {
        /* Set the timebase to the internal module */
        xcomint_setTransTimebase(p_currTime);

        fReturn = TRUE;
    }
    else
    {
        errh_postFatalError(kErrSourceXCom, kErrorInvalidParameter, 0);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Enable the cross communication receive check

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
BOOLEAN xcom_enableReceiveCheck(void)
{
    BOOLEAN fReturn = FALSE;

    if(xcomInstance_l.fWaitForRcv_m == FALSE)
    {
        xcomInstance_l.fWaitForRcv_m = TRUE;
        fReturn = TRUE;
    }
    else
    {
        errh_postFatalError(kErrSourceXCom, kXComMissingCrossCommunication, 0);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Set the SSDO/SNMT CRC to transmit image

\param[in] crcSub1_p   The new CRC value of subframe one
\param[in] crcSub2_p   The new CRC value of subframe two
*/
/*----------------------------------------------------------------------------*/
void xcom_setSsdoSnmtCrc(UINT16 crcSub1_p, UINT16 crcSub2_p)
{
    xcomint_setSsdoSnmtCrc(crcSub1_p, crcSub2_p);
}

/*----------------------------------------------------------------------------*/
/**
\brief    Set the SSDO/SNMT parameters of the transmit image

\param[in] pSub1Params_p   Subframe1 transfer details
\param[in] pSub2Params_p   Subframe2 transfer details
\param[in] pTargBuff_p     The base address of the target buffer
\param[in] targLen_p       The length of the target buffer

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
BOOLEAN xcom_postSsdoSnmtFrame(tSubFrameParams * pSub1Params_p,
                               tSubFrameParams * pSub2Params_p,
                               UINT8 * pTargBuff_p, UINT32 targLen_p)
{
    BOOLEAN fReturn = FALSE;
    UINT8 msgFormat = 0;

    if(pSub1Params_p != NULL && pSub2Params_p != NULL &&
       pTargBuff_p != NULL && targLen_p > 0            )
    {
        if(xcomint_postSsdoSnmtFrame(pSub1Params_p,
                                     pSub2Params_p,
                                     pTargBuff_p, targLen_p))
        {
            if(xcomint_getMsgFormat(kPaylTypeTransmit, &msgFormat))
            {
                msgFormat |= (1<<MSG_FORMAT_SSDO_SET);

                if(xcomint_setMsgFormat(kPaylTypeTransmit, msgFormat))
                {
                    fReturn = TRUE;
                }
                else
                {
                    errh_postFatalError(kErrSourceXCom, kErrorInvalidMsgFormatValue, 0);
                }
            }
            else
            {
                errh_postFatalError(kErrSourceXCom, kErrorInvalidMsgFormatValue, 0);
            }
        }
        else
        {
            errh_postFatalError(kErrSourceXCom, kErrorAsyncFramePostingFailed, 0);
        }
    }
    else
    {
        errh_postFatalError(kErrSourceXCom, kErrorInvalidParameter, 0);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Set the SPDO crc to transmit image

\param[in] crcSub1_p   The new CRC value of subframe one
\param[in] crcSub2_p   The new CRC value of subframe two
*/
/*----------------------------------------------------------------------------*/
void xcom_setSpdoCrc(UINT16 crcSub1_p, UINT16 crcSub2_p)
{
    xcomint_setSpdoCrc(crcSub1_p, crcSub2_p);
}

/*----------------------------------------------------------------------------*/
/**
\brief    Post the SPDO frame to the transmit image

\param[in] pSub1Params_p   Subframe1 transfer details
\param[in] pSub2Params_p   Subframe2 transfer details
\param[in] pTargBuff_p   The base address of the target buffer
\param[in] targLen_p     The length of the target buffer

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
BOOLEAN xcom_postSpdoFrame(tSubFrameParams * pSub1Params_p,
                           tSubFrameParams * pSub2Params_p,
                           UINT8 * pTargBuff_p, UINT32 targLen_p)
{
    BOOLEAN fReturn = FALSE;
    UINT8 msgFormat = 0;

    if(pSub1Params_p != NULL && pSub2Params_p != NULL &&
       pTargBuff_p != NULL && targLen_p > 0            )
    {
        if(xcomint_postSpdoFrame(pSub1Params_p,
                                 pSub2Params_p,
                                 pTargBuff_p, targLen_p))
        {
            if(xcomint_getMsgFormat(kPaylTypeTransmit, &msgFormat))
            {
                /* Set SPDO enable bit */
                msgFormat |= (1<<MSG_FORMAT_SPDO_SET);

                if(xcomint_setMsgFormat(kPaylTypeTransmit, msgFormat))
                {
                    fReturn = TRUE;
                }
                else
                {
                    errh_postFatalError(kErrSourceXCom, kErrorInvalidMsgFormatValue, 0);
                }
            }
            else
            {
                errh_postFatalError(kErrSourceXCom, kErrorInvalidMsgFormatValue, 0);
            }
        }
        else
        {
            errh_postFatalError(kErrSourceXCom, kErrorSyncFramePostingFailed, 0);
        }
    }
    else
    {
        errh_postFatalError(kErrSourceXCom, kErrorInvalidParameter, 0);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Send the current data stream to the other processor

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
BOOLEAN xcom_transmit(UINT32 flowCount_p)
{
    BOOLEAN fReturn = FALSE;

    /* Set current flow count value to transmit image */
    xcomint_setFlowCount(flowCount_p);

    /* Forward frame to other processor */
    if(upserial_transmit(xcomInstance_l.transParams_m.pTxImg_m,
                         xcomInstance_l.transParams_m.txImgSize_m))
    {
        fReturn = TRUE;
    }
    else
    {
        errh_postFatalError(kErrSourceXCom, kErrorSerialTransferFailed, 0);
    }

    return fReturn;
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */


/*----------------------------------------------------------------------------*/
/**
\brief    This function is called if the serial transfer is finished
*/
/*----------------------------------------------------------------------------*/
static void transferFinished(void)
{

}

/*----------------------------------------------------------------------------*/
/**
\brief    This function is called if a new frame is received
*/
/*----------------------------------------------------------------------------*/
static void frameReceived(void)
{
    BENCHMARK_MOD_01_SET(0);

    /* Reset wait for receive flag */
    xcomInstance_l.fWaitForRcv_m = FALSE;

    /* Verify incoming message id header */
    if(xcomint_verifyIdValue())
    {
        /* Verify the local timebase to the received timbase */
        if(verifyTime())
        {
            /* Verify the local value of the flow count with the received one */
            if(verifyFlowCount())
            {
                /* Verify the CRC of the local frame with the received frame */
                if(verifySpdoCrcs())
                {
                    /* Verify the CRC of the local frame with the received frame */
                    if(verifySsdoSnmtCrcs())
                    {
                        /* Handle the SPDO payload if data is received */
                        if(xcomint_handleSpdoPayload())
                        {
                            /* Handle the SSDO/SNMT payload if data is received */
                            if(xcomint_handleSsdoSnmtPayload())
                            {
                                /* Reset the message format field */
                                if(xcomint_setMsgFormat(kPaylTypeTransmit, 0))
                                {
                                    /* Re-enable the receiver to get the next message */
                                    (void)serialEnableReceive();
                                }
                                else
                                {
                                    errh_postFatalError(kErrSourceXCom, kErrorInvalidMsgFormatValue, 0);
                                }
                            }   /* no else: Error handled in called function */
                        }   /* no else: Error handled in called function */
                    }   /* no else: Error handled in called function */
                }   /* no else: Error handled in called function */
            }   /* no else: Error handled in called function */
        }   /* no else: Error handled in called function */
    }
    else
    {
        errh_postFatalError(kErrSourceXCom, kXComIdValueInvalid, 0);
    }

    BENCHMARK_MOD_01_RESET(0);
}

/*----------------------------------------------------------------------------*/
/**
\brief    This function is called if the serial device encounters an error
*/
/*----------------------------------------------------------------------------*/
static void transferError(void)
{
    /* Signal error to error handler */
    errh_postFatalError(kErrSourceXCom, kErrorSerialTransferFailed, 0);
}

/*----------------------------------------------------------------------------*/
/**
\brief    Enable the serial receive task

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN serialEnableReceive(void)
{
    BOOLEAN fReturn = FALSE;

    /* Enable the upserial receiver */
    if(upserial_enableReceive(xcomInstance_l.transParams_m.pRxImg_m,
                              xcomInstance_l.transParams_m.rxImgSize_m))
    {
        fReturn = TRUE;
    }
    else
    {
        errh_postFatalError(kErrSourceXCom, kErrorSerialReceiveFailed, 0);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Verify the local time with the received time

\return TRUE on success; FALSE otherwise
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN verifyTime(void)
{
    BOOLEAN fReturn = FALSE;
    UINT64 rcvTime = 0;
    UINT64 transTime = 0;

    if(xcomint_getTimeBase(kPaylTypeReceive, &rcvTime))
    {
        if(xcomint_getTimeBase(kPaylTypeTransmit, &transTime))
        {
            /* Synchronize the local time to the time of the second processor */
            if(constime_syncConsTime(&transTime, &rcvTime))
            {
                fReturn = TRUE;
            }   /* no else: Error is handled in the called function */
        }
        else
        {
            errh_postFatalError(kErrSourceXCom, kErrorInvalidTimeBase, 0);
        }
    }
    else
    {
        errh_postFatalError(kErrSourceXCom, kErrorInvalidTimeBase, 0);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Verify the flow local flow count value with the received value

\return TRUE if flow count matches; FALSE otherwise
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN verifyFlowCount(void)
{
    BOOLEAN fReturn = FALSE;
    UINT32 rcvFlowCnt = 0;
    UINT32 transFlowCnt = 0;
    UINT32 flowCntDiff = 0;

    if(xcomint_getFlowCnt(kPaylTypeReceive, &rcvFlowCnt))
    {
        if(xcomint_getFlowCnt(kPaylTypeTransmit, &transFlowCnt))
        {
            /* Verify the local flow control counter with the one from the second processor */
            if(rcvFlowCnt == transFlowCnt)
            {
                fReturn = TRUE;
            }
            else
            {
                flowCntDiff = rcvFlowCnt < transFlowCnt ? transFlowCnt - rcvFlowCnt : rcvFlowCnt - transFlowCnt;
                errh_postFatalError(kErrSourceXCom, kXComFlowCountMissmatch, flowCntDiff);
            }
        }
        else
        {
            errh_postFatalError(kErrSourceXCom, kErrorInvalidFlowCount, 0);
        }
    }
    else
    {
        errh_postFatalError(kErrSourceXCom, kErrorInvalidFlowCount, 0);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Verify the SPDO CRC with the received value

\return TRUE if CRC matches; FALSE otherwise
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN verifySpdoCrcs(void)
{
    BOOLEAN fReturn = FALSE;
    UINT16 rxSpdoCrcSub1 = 0;
    UINT16 rxSpdoCrcSub2 = 0;
    UINT16 txSpdoCrcSub1 = 0;
    UINT16 txSpdoCrcSub2 = 0;

    /* Check if the payload consists of an SPDO message */
    if(isSpdoSet())
    {
        if(xcomint_getSpdoCrc(kPaylTypeReceive, &rxSpdoCrcSub1, &rxSpdoCrcSub2))
        {
            if(xcomint_getSpdoCrc(kPaylTypeTransmit, &txSpdoCrcSub1, &txSpdoCrcSub2))
            {
                /* Compare local CRC with the received CRC */
                if(rxSpdoCrcSub1 == txSpdoCrcSub1)
                {
                    if(rxSpdoCrcSub2 == txSpdoCrcSub2)
                    {
                        fReturn = TRUE;
                    }
                    else
                    {
                        errh_postFatalError(kErrSourceXCom, kXComSpdoCrcMissmatch, (rxSpdoCrcSub2 << 16 | txSpdoCrcSub2));
                    }
                }
                else
                {
                    errh_postFatalError(kErrSourceXCom, kXComSpdoCrcMissmatch, (rxSpdoCrcSub1 << 16 | txSpdoCrcSub1));
                }
            }
            else
            {
                errh_postFatalError(kErrSourceXCom, kErrorInvalidCrcValue, 0);
            }
        }
        else
        {
            errh_postFatalError(kErrSourceXCom, kErrorInvalidCrcValue, 0);
        }
    }
    else
    {
        /* No SPDO payload available in this cycle -> Success! */
        fReturn = TRUE;
    }

    return fReturn;
}


/*----------------------------------------------------------------------------*/
/**
\brief    Verify the SSDO/SNMT CRC with the received value

\return TRUE if CRC matches; FALSE otherwise
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN verifySsdoSnmtCrcs(void)
{
    BOOLEAN fReturn = FALSE;
    UINT16 rxSsdoCrcSub1 = 0;
    UINT16 rxSsdoCrcSub2 = 0;
    UINT16 txSsdoCrcSub1 = 0;
    UINT16 txSsdoCrcSub2 = 0;

    /* Check if the payload consists of an SSDO/SNMT message */
    if(isSsdoSnmtSet())
    {
        if(xcomint_getSsdoSnmtCrc(kPaylTypeReceive, &rxSsdoCrcSub1, &rxSsdoCrcSub2))
        {
            if(xcomint_getSsdoSnmtCrc(kPaylTypeTransmit, &txSsdoCrcSub1, &txSsdoCrcSub2))
            {
                /* Compare local CRC with the received CRC */
                if(rxSsdoCrcSub1 == txSsdoCrcSub1)
                {
                    if(rxSsdoCrcSub2 == txSsdoCrcSub2)
                    {
                        fReturn = TRUE;
                    }
                    else
                    {
                        errh_postFatalError(kErrSourceXCom, kXComSsdoSnmtCrcMissmatch, (rxSsdoCrcSub2 << 16 | txSsdoCrcSub2));
                    }
                }
                else
                {
                    errh_postFatalError(kErrSourceXCom, kXComSsdoSnmtCrcMissmatch, (rxSsdoCrcSub1 << 16 | txSsdoCrcSub1));
                }
            }
            else
            {
                errh_postFatalError(kErrSourceXCom, kErrorInvalidCrcValue, 0);
            }
        }
        else
        {
            errh_postFatalError(kErrSourceXCom, kErrorInvalidCrcValue, 0);
        }
    }
    else
    {
        /* No SSDO/SNMT payload available in this cycle -> Success! */
        fReturn = TRUE;
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Check if the SPDO flag is set in the message format field

\return TRUE if flag is set; FALSE otherwise
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN isSpdoSet(void)
{
    BOOLEAN fSpdoSet = FALSE;
    UINT8 rxMsgFormat = 0;
    UINT8 txMsgFormat = 0;

    if(xcomint_getMsgFormat(kPaylTypeTransmit, &txMsgFormat))
    {
        if(xcomint_getMsgFormat(kPaylTypeReceive, &rxMsgFormat))
        {
            if((txMsgFormat & (1<<MSG_FORMAT_SPDO_SET)) > 0 &&
               (rxMsgFormat & (1<<MSG_FORMAT_SPDO_SET)) > 0  )
            {
                fSpdoSet = TRUE;
            }
        }
        else
        {
            errh_postFatalError(kErrSourceXCom, kErrorInvalidMsgFormatValue, 0);
        }
    }
    else
    {
        errh_postFatalError(kErrSourceXCom, kErrorInvalidMsgFormatValue, 0);
    }

    return fSpdoSet;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Check if the SSDO/SNMT flag is set in the message format field

\return TRUE if flag is set; FALSE otherwise
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN isSsdoSnmtSet(void)
{
    BOOLEAN fSsdoSet = FALSE;
    UINT8 rxMsgFormat = 0;
    UINT8 txMsgFormat = 0;

    if(xcomint_getMsgFormat(kPaylTypeTransmit, &txMsgFormat))
    {
        if(xcomint_getMsgFormat(kPaylTypeReceive, &rxMsgFormat))
        {
            if((txMsgFormat & (1<<MSG_FORMAT_SSDO_SET)) > 0 &&
               (rxMsgFormat & (1<<MSG_FORMAT_SSDO_SET)) > 0  )
            {
                fSsdoSet = TRUE;
            }
        }
        else
        {
            errh_postFatalError(kErrSourceXCom, kErrorInvalidMsgFormatValue, 0);
        }
    }
    else
    {
        errh_postFatalError(kErrSourceXCom, kErrorInvalidMsgFormatValue, 0);
    }

    return fSsdoSet;
}

/**
 * \}
 * \}
 */
