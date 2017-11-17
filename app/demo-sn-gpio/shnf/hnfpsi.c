/**
********************************************************************************
\file   demo-sn-gpio/shnf/hnfpsi.c

\defgroup module_sn_shnf_hnfpsi Hardware near firmware module (PSI implementation)
\{

\brief  Implements the hardware near firmware for the psi interface

Provides the interface from the SHNF to the slim interface library.

\see group_libpsi
\see group_libpsicommon

\ingroup group_app_sn_shnf
*******************************************************************************/

/*------------------------------------------------------------------------------
* License Agreement
*
* Copyright 2017 BERNECKER + RAINER, AUSTRIA, 5142 EGGELSBERG, B&R STRASSE 1
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

#include <shnf/hnf.h>

#include <libpsi/psi.h>

#include <common/pcpserial.h>       /* Platform specific functions for the serial */
#include <common/syncir.h>       /* Platform specific functions for the synchronous interrupt */
#include <common/benchmark.h>    /* Debug header for performance measurements */

#include <common/tbufparams.h>

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
 * \brief Describes a buffer
 */
typedef struct
{
    UINT8 * pBuffer_m;      /**< Pointer to the buffer */
    UINT16 buffSize_m;      /**< Size of the buffer */
} tBuffer;

/**
 * \brief The instance structure of the hnf for the PSI interface
 */
typedef struct {
    volatile UINT8 tbufMemLayout_m[TBUF_IMAGE_SIZE];        /**< Local copy of the triple buffer memory */
    UINT8 fCcWriteObjTestEnable_m;                          /**< Enable periodic writing of a cc object */
    tSsdoInstance apSsdoInstance_m[kNumSsdoInstCount];      /**< SSDO channel instance handler array */
    tSsdoRxHandler apfnSsdoRxHandler[kNumSsdoInstCount];    /**< Array of SSDO channel receive callbacks */
    tLogInstance apLogInstance_m[kNumLogInstCount];         /**< Logbook instance handler array */
    tAsyncRxHandler pfnSsdoSnmtRcvHandler_m;                /**< SSDO/SNMT receive handler */
    UINT8 ssdoSnmtRcvBuffer_m[SSDO_STUB_DATA_DOM_SIZE];     /**< Temporary SSDO/SNMT reveive buffer */
    tSyncRxHandler pfnSpdoRxHandler_m;                      /**< SPDO receive handler */
    tSyncTxCreate pfnSpdoTxCreate_m;                        /**< Triggers the building of a transmit spdo frame */
    tBuffer spdo0TxBuffer_m;                                /**< Describes the current transmit buffer of the spdo0 channel */
    tProcSync pfnProcSync_m;                                /**< Pointer to the process sync callback function */
    tSyncCycle pfnSyncronize_m;                             /**< Pointer to the synchronization callback function */
} tHnfPsiInstance;

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/

static tHnfPsiInstance hnfPsiInstance_l SAFE_INIT_SEKTOR;            /**< Instance of the main module */

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static BOOL initPsi(void);
static BOOL initModules(void);
static void exitModules(void);
static BOOL processSync(tPsiTimeStamp* pTimeStamp_p );
static BOOL processApp(UINT32 rpdoRelTimeLow_p,
        tRpdoMappedObj* pRpdoImage_p,
        tTpdoMappedObj* pTpdoImage_p );

static void syncHandler(void* pArg_p);
static void serialTransferFinished(BOOL fError_p);

static void errorHandler(tPsiErrorInfo* pErrorInfo_p);

static BOOL processRxAsyncChannel0(UINT8 * pPayload_p, UINT16 size_p);

static BOOL reformatErrorMessage(tErrorDesc * pErrDesc_p, tLogFormat * pLogData);
static tLogLevel reformatErrorInfo(tErrLevel errLevel_p);

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the SHNF slim interface driver

The hardware near firmware driver for the slim interface. This driver
provides an interface to POWERLINK by using the slim interface library.

\param pHnfInit_p    Module initialization parameters

\retval TRUE    Init of the slim interface successfully
\retval FALSE   Error on initialization
*/
/*----------------------------------------------------------------------------*/
BOOLEAN hnf_init(tHnfInit * pHnfInit_p)
{
    BOOL fReturn = FALSE;

    PSI_MEMSET(&hnfPsiInstance_l, 0, sizeof(hnfPsiInstance_l));

    if(pHnfInit_p != NULL)
    {
        if(pHnfInit_p->asyncRcvChan0Handler_m != NULL  &&
           pHnfInit_p->syncRcvHandler_m != NULL      &&
           pHnfInit_p->syncTxBuild_m != NULL          )
        {
            /* Set callback functions */
            hnfPsiInstance_l.pfnSsdoSnmtRcvHandler_m = pHnfInit_p->asyncRcvChan0Handler_m;
            hnfPsiInstance_l.pfnSpdoRxHandler_m = pHnfInit_p->syncRcvHandler_m;
            hnfPsiInstance_l.pfnSpdoTxCreate_m = pHnfInit_p->syncTxBuild_m;
            hnfPsiInstance_l.pfnProcSync_m = pHnfInit_p->pfnProcSync_m;
            hnfPsiInstance_l.pfnSyncronize_m = pHnfInit_p->pfnSyncronize_m;

            /* Init array of SSDO channel receive handler */
            hnfPsiInstance_l.apfnSsdoRxHandler[kNumSsdoChan0] = processRxAsyncChannel0;

            /* Initialize the slim interface internals */
            if(initPsi())
            {
                fReturn = TRUE;
            }
        }
        else
        {
            errh_postFatalError(kErrSourceHnf, kErrorInvalidParameter, 0);
        }
    }
    else
    {
        errh_postFatalError(kErrSourceHnf, kErrorInvalidParameter, (UINT32)pHnfInit_p);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Close the HNF slim interface driver
*/
/*----------------------------------------------------------------------------*/
void hnf_exit(void)
{
    /* Cleanup target specific functions */
    syncir_exit();
    pcpserial_exit();

    /* Cleanup libpsi */
    exitModules();
    psi_exit();

    MEMSET(&hnfPsiInstance_l, 0, sizeof(tHnfPsiInstance));
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process the asynchronous task

\retval TRUE    Processing of the asynchronous task failed
\retval FALSE   Error during processing
*/
/*----------------------------------------------------------------------------*/
BOOLEAN hnf_processAsync(void)
{
    BOOLEAN fReturn = FALSE;
    BOOLEAN fError = FALSE;
    UINT8 i;

    if(psi_processAsync())
    {
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)
        for(i=0; i < kNumSsdoInstCount; i++)
        {
            if(ssdo_process(hnfPsiInstance_l.apSsdoInstance_m[i]) == FALSE)
            {
                errh_postFatalError(kErrSourceHnf, kErrorAsyncProcessFailed, 0);
                fError = TRUE;
                break;
            }
        }
#endif

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_LOGBOOK)) != 0)
        if(fError == FALSE)
        {
            for(i=0; i < kNumLogInstCount; i++)
            {
                if(log_process(hnfPsiInstance_l.apLogInstance_m[i]) == FALSE)
                {
                    errh_postFatalError(kErrSourceHnf, kErrorAsyncProcessFailed, 0);
                    fError = TRUE;
                    break;
                }
            }
        }
#endif
    }
    else
    {
        errh_postFatalError(kErrSourceHnf, kErrorAsyncProcessFailed, 0);
    }

    if(fError == FALSE)
        fReturn = TRUE;

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Transmit a frame over the asynchronous channel 0

This function forwards an asynchronous frame over the asynchronous
channel 0 to the network.

\param pPayload_p     Pointer to the payload to transmit
\param paylLen_p      Length of the payload

\retval TRUE    Transmit of the asynchronous frame successfully
\retval FALSE   Transmit failed or channel is busy
*/
/*----------------------------------------------------------------------------*/
BOOLEAN hnf_postAsyncTxChannel0(const UINT8 * pPayload_p, UINT16 paylLen_p)
{
    BOOL fReturn = FALSE;
    tSsdoTxStatus transState;

    transState = ssdo_postPayload(hnfPsiInstance_l.apSsdoInstance_m[kNumSsdoChan0],
                                  (UINT8 *)pPayload_p, paylLen_p);
    if(transState == kSsdoTxStatusSuccessful)
    {
        fReturn = TRUE;
    } /* no else: Error is posted to error handler in shnf module */

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Get the current transmit buffer from channel0

\param[out] ppTxBuffer_p     Pointer to the current transmit buffer
\param[out] pBuffLen_p       Pointer to the length of the current transmit buffer

\retval TRUE    Transmit buffer valid
\retval FALSE   Error on getting the transmit buffer
*/
/*----------------------------------------------------------------------------*/
BOOLEAN hnf_getAsyncTxBufferChannel0(UINT8 ** ppTxBuffer_p, UINT16 * pBuffLen_p)
{
    BOOLEAN fReturn = FALSE;

    if(ssdo_getCurrentTxBuffer(hnfPsiInstance_l.apSsdoInstance_m[kNumSsdoChan0],
                               ppTxBuffer_p, pBuffLen_p))
    {
        fReturn = TRUE;
    } /* no else: Error is posted to error handler in shnf module */

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Call this function to free the receive message of async channel0
*/
/*----------------------------------------------------------------------------*/
void hnf_finishedAsyncRxChannel0(void)
{
    ssdo_receiveMsgFinished(hnfPsiInstance_l.apSsdoInstance_m[kNumSsdoChan0]);
}


/*----------------------------------------------------------------------------*/
/**
\brief    Forward a logbook entry over logbook channel 0

\param pErrDesc_p       Pointer to the error descriptor

\retval TRUE    Post of the entry successful
\retval FALSE   Error on posting
*/
/*----------------------------------------------------------------------------*/
BOOLEAN hnf_postLogChannel0(tErrorDesc * pErrDesc_p)
{
    BOOL fReturn = FALSE;
    tLogTxStatus transState;
    tLogFormat * pLogData = NULL;

    /* Acquire buffer for new log message */
    if(log_getCurrentLogBuffer(hnfPsiInstance_l.apLogInstance_m[kNumLogChan0],
                               &pLogData))
    {
        /* Adapt error message format to special interface type */
        if(reformatErrorMessage(pErrDesc_p, pLogData))
        {
            /* Post the logging data to the logger module */
            transState = log_postLogEntry(hnfPsiInstance_l.apLogInstance_m[kNumLogChan0],
                                          pLogData);
            if(transState == kLogTxStatusSuccessful)
            {
                fReturn = TRUE;
            }   /* no else: Error will be indicated by the caller of this function*/
        }
    }   /* no else: Error will be indicated by the caller of this function */

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Transmit a isochronous frame over the synchronous channel

This function forwards an asynchronous frame over the asynchronous
channel 0 to the network.

\param pPayload_p     Pointer to the payload to transmit
\param paylLen_p      Length of the payload

\retval TRUE    Transmit of the synchronous frame successful
\retval FALSE   Error on transmitting
*/
/*----------------------------------------------------------------------------*/
BOOLEAN hnf_postSyncTx(const UINT8 * pPayload_p, UINT16 paylLen_p)
{
    UNUSED_PARAMETER(pPayload_p);
    UNUSED_PARAMETER(paylLen_p);

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Get the current synchronous transmit buffer

\param[out] ppTxBuffer_p     Pointer to the current transmit buffer
\param[out] pBuffLen_p       Pointer to the length of the current transmit buffer

\retval TRUE    Transmit buffer valid
\retval FALSE   Error on getting the tx buffer
*/
/*----------------------------------------------------------------------------*/
BOOLEAN hnf_getSyncTxBuffer(UINT8 ** ppTxBuffer_p, UINT16 * pBuffLen_p)
{
    BOOL fReturn = FALSE;

    if(hnfPsiInstance_l.spdo0TxBuffer_m.pBuffer_m != NULL &&
       hnfPsiInstance_l.spdo0TxBuffer_m.buffSize_m > 0     )
    {
        *ppTxBuffer_p = hnfPsiInstance_l.spdo0TxBuffer_m.pBuffer_m;
        *pBuffLen_p = hnfPsiInstance_l.spdo0TxBuffer_m.buffSize_m;
        fReturn = TRUE;
    } /* no else: Error is posted to error handler in shnf module */

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Enable the syncronous interrupt
*/
/*----------------------------------------------------------------------------*/
void hnf_enableSyncIr(void)
{
    /* Now enable the synchronous interrupt */
    syncir_enable();
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the slim interface

\retval TRUE    Transmit of the asynchronous frame successfully
\retval FALSE   Transmit failed
*/
/*----------------------------------------------------------------------------*/
static BOOL initPsi(void)
{
    BOOL fReturn = FALSE;
    tPsiInitParam initParam;
    tBuffDescriptor buffDescList[kTbufCount];
    tHandlerParam transferParam;
    UINT8 * pTbufMemBase = (UINT8 *)(&hnfPsiInstance_l.tbufMemLayout_m[0]);

    PSI_MEMSET(&transferParam, 0, sizeof(tHandlerParam));
    PSI_MEMSET(&buffDescList, 0, sizeof(buffDescList));

    /* Generate buffer descriptor list */
    if(tbufp_genDescList(pTbufMemBase, kTbufCount, &buffDescList[0]))
    {
        /* Enable test of configuration channel */
        hnfPsiInstance_l.fCcWriteObjTestEnable_m = TRUE;

        /* initialize and start the slim interface internals */
        DEBUG_TRACE(DEBUG_LVL_ALWAYS,"\nInitialize psi internals -> ");

        initParam.pBuffDescList_m = &buffDescList[0];
        initParam.pfnStreamHandler_m = pcpserial_transfer;
        initParam.pfnErrorHandler_m = errorHandler;
        initParam.idConsAck_m = kTbufAckRegisterCons;
        initParam.idProdAck_m = kTbufAckRegisterProd;
        initParam.idFirstProdBuffer_m = TBUF_NUM_CON + 1;   /* Add one buffer for the consumer ACK register */

        if(psi_init(&initParam))
        {
            DEBUG_TRACE(DEBUG_LVL_ALWAYS, "SUCCESS!\n");

            /* initialize and start the slim interface modules */
            DEBUG_TRACE(DEBUG_LVL_ALWAYS,"\nInitialize psi modules -> ");
            if(initModules())
            {
                DEBUG_TRACE(DEBUG_LVL_ALWAYS, "SUCCESS!\n");

                /* Set the base address of the PDO image */
                hnfPsiInstance_l.spdo0TxBuffer_m.pBuffer_m = (UINT8 *)pdo_getTpdoImage();
                hnfPsiInstance_l.spdo0TxBuffer_m.buffSize_m = TX_SPDO_SIZE;

                /* Setup consumer/producer transfer parameters with initialization fields */
                if(tbufp_genTransferParams(pTbufMemBase, &transferParam))
                {
                    /* initialize serial interface*/
                    DEBUG_TRACE(DEBUG_LVL_ALWAYS,"\nInitialize serial device -> ");
                    if(pcpserial_init(&transferParam, serialTransferFinished))
                    {
                        DEBUG_TRACE(DEBUG_LVL_ALWAYS, "SUCCESS!\n");

                        /* initialize PCP interrupt handler*/
                        DEBUG_TRACE(DEBUG_LVL_ALWAYS,"\nInitialize sync interrupt -> ");
                        if(syncir_init(syncHandler))
                        {
                            DEBUG_TRACE(DEBUG_LVL_ALWAYS, "SUCCESS!\n");

                            fReturn = TRUE;
                        }    /* Error on libpsi module initialization! Error reported via errorHandler() */
                    }    /* platform_initSyncInterrupt() failed! */
                }
                else
                {
                    errh_postFatalError(kErrSourceHnf, kErrorUnableToGenerateStreamParams, 0);
                }
            }    /* Error on libpsi module initialization! Error reported via errorHandler() */
        }    /* Error on libpsi initialization! Error reported via errorHandler() */
    }
    else
    {
        errh_postFatalError(kErrSourceHnf, kErrorUnableToGenerateStreamParams, 0);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize slim interface modules

\retval TRUE    Library module initialization successful
\retval FALSE   Error during library module initialization
*/
/*----------------------------------------------------------------------------*/
static BOOL initModules(void)
{
    BOOL fReturn = FALSE;
    UINT8 i;
    tStatusInitParam   statusInitParam;
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_PDO)) != 0)
    tPdoInitParam      pdoInitParam;
#endif
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)
    tSsdoInitParam     ssdoInitParam;
#endif
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_LOGBOOK)) != 0)
    tLogInitParam      logInitParam;
#endif

    /* Initialize the status module */
    statusInitParam.pfnProcSyncCb_m = processSync;
    statusInitParam.buffOutId_m = kTbufNumStatusOut;
    statusInitParam.buffInId_m = kTbufNumStatusIn;

    if(status_init(&statusInitParam) == FALSE)
    {
        errh_postFatalError(kErrSourceHnf, kErrorStatusModuleInitFailed, 0);
        return fReturn;
    }

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_PDO)) != 0)
    /* Initialize the PDO module */
    pdoInitParam.buffIdRpdo_m = kTbufNumRpdoImage;
    pdoInitParam.buffIdTpdo_m = kTbufNumTpdoImage;

    if(pdo_init(processApp, &pdoInitParam) == FALSE)
    {
        errh_postFatalError(kErrSourceHnf, kErrorPdoModuleInitFailed, 0);
        return fReturn;
    }
#endif

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)
    /* Initialize all needed SSDO channels */
    for(i=0; i<kNumSsdoInstCount; i++)
    {
        ssdoInitParam.buffIdRx_m = kTbufNumSsdoReceive0 + i;
        ssdoInitParam.buffIdTx_m = kTbufNumSsdoTransmit0 + i;

        ssdoInitParam.pfnRxHandler_m = hnfPsiInstance_l.apfnSsdoRxHandler[i];

        hnfPsiInstance_l.apSsdoInstance_m[i] = ssdo_create(kNumSsdoChan0 + i, &ssdoInitParam);
        if(hnfPsiInstance_l.apSsdoInstance_m[i] == NULL)
        {
            errh_postFatalError(kErrSourceHnf, kErrorSsdoModuleInitFailed, 0);
            return fReturn;
        }
    }
#endif

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_LOGBOOK)) != 0)
    /* Initialize all needed logbook channels */
    for(i=0; i<kNumLogInstCount; i++)
    {
        logInitParam.buffIdTx_m = kTbufNumLogbook0 + i;

        hnfPsiInstance_l.apLogInstance_m[i] = log_create(kNumLogChan0 + i, &logInitParam);
        if(hnfPsiInstance_l.apLogInstance_m[i] == NULL)
        {
            errh_postFatalError(kErrSourceHnf, kErrorLoggerModuleInitFailed, 0);
            return fReturn;
        }
    }
#endif

    /* All modules successfully initialized! -> Set return to success! */
    fReturn = TRUE;

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Destroy slim interface modules
*/
/*----------------------------------------------------------------------------*/
static void exitModules(void)
{
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)
    UINT8 i;
#endif
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_LOGBOOK)) != 0)
    UINT8 j;
#endif

    /* Destroy all library modules */
    status_exit();

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_PDO)) != 0)
    pdo_exit();
#endif

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)
    for(i=0; i<kNumSsdoInstCount; i++)
        ssdo_destroy(hnfPsiInstance_l.apSsdoInstance_m[i]);
#endif

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_LOGBOOK)) != 0)
    for(j=0; j<kNumLogInstCount; j++)
        log_destroy(hnfPsiInstance_l.apLogInstance_m[j]);
#endif
}

/*----------------------------------------------------------------------------*/
/**
\brief    Synchronization to the POWERLINK cycle

 \param[in] pTimeStamp_p             Time information of the last interrupt.

 \return This function always succeeds
*/
/*----------------------------------------------------------------------------*/
static BOOL processSync(tPsiTimeStamp* pTimeStamp_p )
{
    BOOL fReturn = FALSE;

    UNUSED_PARAMETER(pTimeStamp_p);

    /* Note: This is your place to do local synchronization. Synchronize */
    /*       your local clock here by using the provided timestamp. */

    if(hnfPsiInstance_l.pfnSyncronize_m != NULL)
    {
        /* Call the synchronization callback function */
        if(hnfPsiInstance_l.pfnSyncronize_m())
        {
            fReturn = TRUE;
        }
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process synchronous data

This function is used to process the synchronous part of the application.
Basically it calls all functions of the openSAFETY stack which need to be
finished in one cycle. (This function is called at the end of the PSI tasks)

\param[in] rpdoRelTimeLow_p     Relative time low value
\param[in] pRpdoImage_p         Pointer to the RPDO objects
\param[in] pTpdoImage_p         Pointer to the TPDO objects

 \return This function always succeeds
*/
/*----------------------------------------------------------------------------*/
static BOOL processApp(UINT32 rpdoRelTimeLow_p,
        tRpdoMappedObj* pRpdoImage_p,
        tTpdoMappedObj* pTpdoImage_p )
{
    BOOL fReturn = FALSE;

    UNUSED_PARAMETER(rpdoRelTimeLow_p);
    UNUSED_PARAMETER(pTpdoImage_p);

    if(hnfPsiInstance_l.pfnSpdoRxHandler_m != NULL)
    {
        if (pRpdoImage_p != NULL)
        {
            /* Call RSPDO receive handler () */
            hnfPsiInstance_l.pfnSpdoRxHandler_m(&pRpdoImage_p->spdo0[0], RX_SPDO0_SIZE);

            if(hnfPsiInstance_l.pfnSpdoTxCreate_m != NULL)
            {
                /* Call TSPDO transmit create */
                hnfPsiInstance_l.pfnSpdoTxCreate_m();

                if(hnfPsiInstance_l.pfnProcSync_m != NULL)
                {
                    /* Call the process sync callback function (Handles all actions of the end of the cycle) */
                    if(hnfPsiInstance_l.pfnProcSync_m())
                    {
                        fReturn = TRUE;
                    }
                }
                else
                {
                    errh_postFatalError(kErrSourceHnf, kErrorCallbackNotInitialized, 0);
                }
            }
            else
            {
                errh_postFatalError(kErrSourceHnf, kErrorCallbackNotInitialized, 0);
            }
        }
        else
        {
            errh_postFatalError(kErrSourceHnf, kErrorInvalidParameter, 0);
        }
    }
    else
    {
        errh_postFatalError(kErrSourceHnf, kErrorCallbackNotInitialized, 0);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Synchronous interrupt handler

\param[in] pArg_p       Interrupt handler arguments

 psi_syncIntH() implements the synchronous data interrupt. The PCP asserts
 the interrupt when periodic data is ready to transfer.
*/
/*----------------------------------------------------------------------------*/
static void syncHandler(void* pArg_p)
{
    UNUSED_PARAMETER(pArg_p);

    BENCHMARK_MOD_01_SET(0);

    /* Call internal synchronous process function */
    if(psi_processSync() == FALSE)
    {
        errh_postFatalError(kErrSourceHnf, kErrorSyncProcessFailed, 0);
    }

    syncir_acknowledge();

    BENCHMARK_MOD_01_RESET(0);
}

/*----------------------------------------------------------------------------*/
/**
\brief    Serial transfer finished callback function

This function is called after a serial transfer from the PCP to the application.

\param[in] fError_p       True if the transfer had an error
*/
/*----------------------------------------------------------------------------*/
static void serialTransferFinished(BOOL fError_p)
{
    if(fError_p == FALSE)
    {
        BENCHMARK_MOD_01_SET(0);

        /* Transfer finished -> Call all post action tasks */
        if(psi_processPostTransferActions() == FALSE)
        {
            errh_postFatalError(kErrSourceHnf, kErrorSyncProcessFailed, 0);
        }

        BENCHMARK_MOD_01_RESET(0);
    }
    else
    {
        /* There was an error during serial transfer */
        errh_postFatalError(kErrSourceHnf, kErrorSerialTransmitFailed, 0);
    }
}

/*----------------------------------------------------------------------------*/
/**
\brief    Error handler callback function

\param pErrorInfo_p     Provides details about the occurred error
*/
/*----------------------------------------------------------------------------*/
static void errorHandler(tPsiErrorInfo* pErrorInfo_p)
{
    tErrorDesc errInfo;

    MEMSET(&errInfo, 0, sizeof(tErrorDesc));

    errInfo.source_m = kErrSourceHnf;       /* Source is always the HNF in this case */
    errInfo.fFailSafe_m = TRUE;
    errInfo.class_m = kErrLevelFatal;       /* All errors in the HNF are critical */

    if(pErrorInfo_p != NULL)
    {
        errInfo.unit_m = (UINT8)pErrorInfo_p->srcModule_m;
        errInfo.code_m = (UINT8)pErrorInfo_p->errCode_m;
    }
    else
    {
        /* Error during posting of the error itself */
        errInfo.unit_m = kPsiModuleExternal;
        errInfo.code_m = kPsiGeneralError;
    }

    /* Forward error to SN error handler */
    errh_postError(&errInfo);
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process the received frame from asynchronous channel 0

\param pPayload_p    Pointer to the received payload
\param size_p        The length of the received payload

\retval TRUE        Processing of receive frame successful
\retval FALSE       Error during processing
*/
/*----------------------------------------------------------------------------*/
static BOOL processRxAsyncChannel0(UINT8 * pPayload_p, UINT16 size_p)
{
    BOOL fReturn = FALSE;

    if(hnfPsiInstance_l.pfnSsdoSnmtRcvHandler_m != NULL)
    {
        if(size_p <= SSDO_STUB_DATA_DOM_SIZE)
        {
            /* The openSAFETY stack modifies the second subframe in place
             * This modification is overwritten by the SPI stream which results in
             * a CRC error. Therefore we need to save the asynchronous payload
             * in this temporary buffer to prevent any manipulation of the
             * stream.
             */
            MEMCOPY(&hnfPsiInstance_l.ssdoSnmtRcvBuffer_m, pPayload_p, size_p);

            /* Forward frame to SHNF main module */
            if(hnfPsiInstance_l.pfnSsdoSnmtRcvHandler_m((UINT8 *)&hnfPsiInstance_l.ssdoSnmtRcvBuffer_m, size_p))
            {
                fReturn = TRUE;
            }
        }
        else
        {
            errh_postFatalError(kErrSourceHnf, kErrorAsyncFrameTooLarge, size_p);
        }
    }
    else
    {
        errh_postFatalError(kErrSourceHnf, kErrorCallbackNotInitialized, 0);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Reformat the error message to special interface type

\param pErrDesc_p    The error descriptor from the SAPL
\param pLogData      The error message in interface form

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
static BOOL reformatErrorMessage(tErrorDesc * pErrDesc_p, tLogFormat * pLogData)
{
    BOOL fReturn = FALSE;
    tLogLevel errLevel;
    UINT16 errSource;

    /* Reformat the error info type */
    errLevel = reformatErrorInfo(pErrDesc_p->class_m);
    if(errLevel != kLogLevelInvalid)
    {
        ami_setUint8Le((UINT8 *)&pLogData->level_m, (UINT8)errLevel);

        errSource = (UINT16)(((UINT16)pErrDesc_p->unit_m << 8) | ((UINT8)pErrDesc_p->source_m));
        ami_setUint16Le((UINT8 *)&pLogData->source_m, errSource);

        ami_setUint32Le((UINT8 *)&pLogData->code_m, (UINT32)pErrDesc_p->code_m);
        ami_setUint32Le((UINT8 *)&pLogData->addInfo_m, pErrDesc_p->addInfo_m);

        fReturn = TRUE;
    }   /* no else: Error handled by caller */

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Reformat the error level to special interface  type

\param errLevel_p    The stack error level type

\return The interface error level
*/
/*----------------------------------------------------------------------------*/
static tLogLevel reformatErrorInfo(tErrLevel errLevel_p)
{
    tLogLevel levelRet = kLogLevelInvalid;

    /* Reformat error descriptor to internal logger format */
    switch(errLevel_p)
    {
        case kErrLevelInfo:
            levelRet = kLogLevelInfo;
            break;
        case kErrLevelMinor:
            levelRet = kLogLevelMinor;
            break;
        case kErrLevelFatal:
            levelRet = kLogLevelFatal;
            break;
        default:
            /* Error occurred: is handled by the calling function */
            break;
    }

    return levelRet;
}

/**
 * \}
 * \}
 */
