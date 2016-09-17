/**
********************************************************************************
\file   psi.c

\brief  Application interface managing module

This module acts as an interface between the slim interface modules
and the openPOWERLINK stack modules. It initializes all psi modules and calls
the synchronous and asynchronous tasks.

\ingroup module_psi
*******************************************************************************/

/*------------------------------------------------------------------------------
* License Agreement
*
* Copyright 2014 BERNECKER + RAINER, AUSTRIA, 5142 EGGELSBERG, B&R STRASSE 1
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

#include <psi/psi.h>

#include <psi/tbuf.h>
#include <psi/status.h>
#include <psi/cc.h>
#include <psi/icc.h>
#include <psi/occ.h>
#include <psi/pdo.h>
#include <psi/rpdo.h>
#include <psi/tpdo.h>
#include <psi/rssdo.h>
#include <psi/tssdo.h>
#include <psi/logbook.h>
#include <psi/fifo.h>
#include <libpsicommon/ccobject.h>
#include <debug.h>

#include <config/ccobjectlist.h>
#include <config/triplebuffer.h>


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


//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

typedef struct {
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)
    tRssdoInstance   instRssdoChan_m[kNumSsdoInstCount];    ///< Instance of the SSDO receive channels
    tTssdoInstance   instTssdoChan_m[kNumSsdoInstCount];    ///< Instance of the SSDO transmit channels
#endif
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_LOGBOOK)) != 0)
    tLogInstance     instLogChan_m[kNumLogInstCount];       ///< Instance of the logger channels
#endif
    UINT8            nodeId_m;                              ///< The node Id of the CN
} tPsiInstance;

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static tPsiInstance psiInstance_l;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0  && \
    ((PSI_MODULE_INTEGRATION) & (PSI_MODULE_LOGBOOK)) != 0 )
static tPsiStatus forwardInstanceHandle(UINT32* pInstHdl_p);
#endif

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Application interface initialization function

Initializes the slim interface and all its submodules.

\param[in] pfnCritSec_p           Critical section entry point function

\return tPsiStatus
\retval kPsiSuccessful          successful initialization
\retval other                     Other initialization error code

\ingroup module_psi
*/
//------------------------------------------------------------------------------
tPsiStatus psi_init(UINT8 nodeId_p, tPsiCritSec pfnCritSec_p)
{
    tPsiStatus         ret = kPsiSuccessful;
    tStatusInitStruct    statusInitParam;
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_CC)) != 0)
    tOccInitStruct       occInitParam;
    tIccInitStruct       iccInitParam;
#else
    UNUSED_PARAMETER(pfnCritSec_p);
#endif
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)
    UINT8                i;
    tRssdoInitStruct     rssdoInitParam;
    tTssdoInitStruct     tssdoInitParam;
#endif
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_PDO)) != 0)
    tRpdoInitStruct      rpdoInitParam;
    tTpdoInitStruct      tpdoInitParam;
#endif
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_LOGBOOK)) != 0)
    UINT8                j;
    tLogInitStruct       logInitParam;
#endif

    tTbufDescriptor      tbufDescList[kTbufCount] = TBUF_INIT_VEC;
    UINT8*               prodAckBase = (UINT8 *)(TBUF_BASE_ADDRESS +
                                       tbufDescList[kTbufAckRegisterProd].buffOffset_m);
    UINT8*               consAckBase = (UINT8 *)(TBUF_BASE_ADDRESS +
                                       tbufDescList[kTbufAckRegisterCons].buffOffset_m);

    // Reset psi instance structure
    PSI_MEMSET(&psiInstance_l, 0 , sizeof(tPsiInstance));

    // Make node id global
    psiInstance_l.nodeId_m = nodeId_p;

#if _DEBUG
    if(TRIPLE_BUFFER_COUNT != kTbufCount)
    {
        ret = kPsiInitError;
        DEBUG_TRACE(DEBUG_LVL_ERROR, "ERROR: Triple buffer count mismatch! -> Correct ipcore settings!\n");
        goto Exit;
    }

    if(UNALIGNED32((UINT8 *) TBUF_BASE_ADDRESS))
    {
        ret = kPsiInitError;
        DEBUG_TRACE(DEBUG_LVL_ERROR, "ERROR: Triple buffer base address is not aligned!\n");
        goto Exit;
    }

    if( UNALIGNED32(prodAckBase) || UNALIGNED32(consAckBase) )
    {
        ret = kPsiInitError;
        DEBUG_TRACE(DEBUG_LVL_ERROR, "ERROR: Producer or consumer acknowledge "
                "base address is not aligned!\n");
        goto Exit;
    }
#endif

    ret = tbuf_init();
    if(ret != kPsiSuccessful)
    {
        DEBUG_TRACE(DEBUG_LVL_ERROR, "ERROR: tbuf_init() failed with 0x%x\n",ret);
        goto Exit;
    }

    ret = fifo_init();
    if(ret != kPsiSuccessful)
    {
        DEBUG_TRACE(DEBUG_LVL_ERROR, "ERROR: fifo_init() failed with 0x%x\n",ret);
        goto Exit;
    }

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)
    rssdo_init(psiInstance_l.nodeId_m, SSDO_STUB_OBJECT_INDEX, SSDO_STUB_DATA_OBJECT_INDEX);
    tssdo_init(psiInstance_l.nodeId_m, SSDO_STUB_OBJECT_INDEX, SSDO_STUB_DATA_OBJECT_INDEX);
#endif

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)
    log_init(psiInstance_l.nodeId_m, LOG_STUB_OBJECT_INDEX, pfnCritSec_p);
#endif

    // Initialize the status module
    statusInitParam.outId_m = kTbufNumStatusOut;
    statusInitParam.pOutTbufBase_m = (tTbufStatusOutStructure *)(TBUF_BASE_ADDRESS +
            tbufDescList[kTbufNumStatusOut].buffOffset_m);
    statusInitParam.pProdAckBase_m = prodAckBase;
    statusInitParam.outTbufSize_m = tbufDescList[kTbufNumStatusOut].buffSize_m;

    statusInitParam.inId_m = kTbufNumStatusIn;
    statusInitParam.pInTbufBase_m = (tTbufStatusInStructure *)(TBUF_BASE_ADDRESS +
            tbufDescList[kTbufNumStatusIn].buffOffset_m);
    statusInitParam.pConsAckBase_m = consAckBase;
    statusInitParam.inTbufSize_m = tbufDescList[kTbufNumStatusIn].buffSize_m;

    ret = status_init(&statusInitParam);
    if(ret != kPsiSuccessful)
    {
        DEBUG_TRACE(DEBUG_LVL_ERROR, "ERROR: status_init() failed with 0x%x\n",ret);
        goto Exit;
    }

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_CC)) != 0)
    // Initialize the configuration channel module
    if(ccobject_init(pfnCritSec_p) == FALSE)
    {
        ret = kPsiConfChanInitError;
        DEBUG_TRACE(DEBUG_LVL_ERROR, "ERROR: ccobject_init() failed with 0x%x\n",ret);
        goto Exit;
    }

    // Initialize the output configuration channel
    occInitParam.id_m = kTbufNumOutputConfChan;
    occInitParam.pTbufBase_m = (tTbufCcStructure *)(TBUF_BASE_ADDRESS +
            tbufDescList[kTbufNumOutputConfChan].buffOffset_m);
    occInitParam.pProdAckBase_m = prodAckBase;
    occInitParam.tbufSize_m = tbufDescList[kTbufNumOutputConfChan].buffSize_m;

    ret = occ_init(&occInitParam);
    if(ret != kPsiSuccessful)
    {
        DEBUG_TRACE(DEBUG_LVL_ERROR, "ERROR: occ_init() failed with 0x%x\n",ret);
        goto Exit;
    }

    // Initialize the input configuration channel
    iccInitParam.id_m = kTbufNumInputConfChan;
    iccInitParam.pTbufBase_m = (tTbufCcStructure *)(TBUF_BASE_ADDRESS +
            tbufDescList[kTbufNumInputConfChan].buffOffset_m);
    iccInitParam.pConsAckBase_m = consAckBase;
    iccInitParam.tbufSize_m = tbufDescList[kTbufNumInputConfChan].buffSize_m;

    ret = icc_init(&iccInitParam);
    if(ret != kPsiSuccessful)
    {
        DEBUG_TRACE(DEBUG_LVL_ERROR, "ERROR: icc_init() failed with 0x%x\n",ret);
        goto Exit;
    }
#endif

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_PDO)) != 0)
    // Initialize the output configuration channel
    rpdoInitParam.id_m = kTbufNumRpdoImage;
    rpdoInitParam.pTbufBase_m = (tTbufRpdoImage *)(TBUF_BASE_ADDRESS +
            tbufDescList[kTbufNumRpdoImage].buffOffset_m);
    rpdoInitParam.pProdAckBase_m = prodAckBase;
    rpdoInitParam.tbufSize_m = tbufDescList[kTbufNumRpdoImage].buffSize_m;

    ret = rpdo_init(&rpdoInitParam);
    if(ret != kPsiSuccessful)
    {
        DEBUG_TRACE(DEBUG_LVL_ERROR, "ERROR: rpdo_init() failed with 0x%x\n",ret);
        goto Exit;
    }

    // Initialize the input configuration channel
    tpdoInitParam.id_m = kTbufNumTpdoImage;
    tpdoInitParam.pTbufBase_m = (tTbufTpdoImage *)(TBUF_BASE_ADDRESS +
            tbufDescList[kTbufNumTpdoImage].buffOffset_m);
    tpdoInitParam.pConsAckBase_m = consAckBase;
    tpdoInitParam.tbufSize_m = tbufDescList[kTbufNumTpdoImage].buffSize_m;

    ret = tpdo_init(&tpdoInitParam);
    if(ret != kPsiSuccessful)
    {
        DEBUG_TRACE(DEBUG_LVL_ERROR, "ERROR: tpdo_init() failed with 0x%x\n",ret);
        goto Exit;
    }
#endif

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)
    // Initialize all SSDO channels
    for(i=0; i < kNumSsdoInstCount; i++)
    {
        rssdoInitParam.chanId_m = i;

        rssdoInitParam.tbufRxId_m = kTbufNumSsdoReceive0 + i;
        rssdoInitParam.pTbufRxBase_m = (tTbufSsdoRxStructure *)(TBUF_BASE_ADDRESS +
                tbufDescList[kTbufNumSsdoReceive0 + i].buffOffset_m);
        rssdoInitParam.pProdAckBase_m = prodAckBase;
        rssdoInitParam.tbufRxSize_m = tbufDescList[kTbufNumSsdoReceive0 + i].buffSize_m;

        psiInstance_l.instRssdoChan_m[i] = rssdo_create(&rssdoInitParam);
        if(psiInstance_l.instRssdoChan_m[i] == NULL)
        {
            ret = kPsiSsdoInitError;
            DEBUG_TRACE(DEBUG_LVL_ERROR,"ERROR: rssdo_create() failed for instance "
                    "number %d!\n", i );
            goto Exit;
        }

        tssdoInitParam.chanId_m = i;

        tssdoInitParam.tbufTxId_m = kTbufNumSsdoTransmit0 + i;
        tssdoInitParam.pTbufTxBase_m = (tTbufSsdoTxStructure *)(TBUF_BASE_ADDRESS +
                tbufDescList[kTbufNumSsdoTransmit0 + i].buffOffset_m);
        tssdoInitParam.pConsAckBase_m = consAckBase;
        tssdoInitParam.tbufTxSize_m = tbufDescList[kTbufNumSsdoTransmit0 + i].buffSize_m;

        psiInstance_l.instTssdoChan_m[i] = tssdo_create(&tssdoInitParam);
        if(psiInstance_l.instTssdoChan_m[i] == NULL)
        {
            ret = kPsiSsdoInitError;
            DEBUG_TRACE(DEBUG_LVL_ERROR,"ERROR: tssdo_create() failed for instance "
                    "number %d!\n", i );
            goto Exit;
        }
    }
#endif

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_LOGBOOK)) != 0)
    // Initialize all logbook channels
    for(j=0; j < kNumLogInstCount; j++)
    {
        logInitParam.chanId_m = j;

        logInitParam.tbufTxId_m = kTbufNumLogbook0 + j;
        logInitParam.pTbufTxBase_m = (tTbufLogStructure *)(TBUF_BASE_ADDRESS +
                tbufDescList[kTbufNumLogbook0 + j].buffOffset_m);
        logInitParam.pConsAckBase_m = consAckBase;
        logInitParam.tbufTxSize_m = tbufDescList[kTbufNumLogbook0 + j].buffSize_m;

        psiInstance_l.instLogChan_m[j] = log_create(&logInitParam);
        if(psiInstance_l.instLogChan_m[j] == NULL)
        {
            ret = kPsiLogInitError;
            DEBUG_TRACE(DEBUG_LVL_ERROR,"ERROR: log_create() failed for instance "
                    "number %d!\n", j );
            goto Exit;
        }
    }
#endif

Exit:
    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Application interface exit function

Close and destroy the slim interface and all its submodules.

\ingroup module_psi
*/
//------------------------------------------------------------------------------
void psi_exit(void)
{
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)
    UINT8 i;
#endif
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_LOGBOOK)) != 0)
    UINT8 j;
#endif

    // Destroy slim interface modules
    status_exit();

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_CC)) != 0)
    ccobject_exit();
    icc_exit();
    occ_exit();
#endif

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_PDO)) != 0)
    rpdo_exit();
    tpdo_exit();
#endif

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)
    // Destroy all SSDO channels
    for(i=0; i < kNumSsdoInstCount; i++)
    {
        rssdo_destroy(psiInstance_l.instRssdoChan_m[i]);
        tssdo_destroy(psiInstance_l.instTssdoChan_m[i]);
    }
#endif

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_LOGBOOK)) != 0)
    // Destroy all logbook channels
    for(j=0; j < kNumLogInstCount; j++)
    {
        log_destroy(psiInstance_l.instLogChan_m[j]);
    }
#endif
}

//------------------------------------------------------------------------------
/**
\brief    Perform the module configuration steps after the POWERLINK stack init

\return tPsiStatus
\retval kPsiSuccessful        On success
\retval Other                   Other initialization error

\ingroup module_psi
*/
//------------------------------------------------------------------------------
tPsiStatus psi_configureModules(void)
{
    tPsiStatus ret = kPsiSuccessful;

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_PDO)) != 0)
    ret = rpdo_linkRpdos();
    if(ret != kPsiSuccessful)
    {
        DEBUG_TRACE(DEBUG_LVL_ERROR, "ERROR: Unable to link RPDO to variable! Reason: 0x%x\n", ret);
        goto Exit;
    }

    ret = tpdo_linkTpdos();
    if(ret != kPsiSuccessful)
    {
        DEBUG_TRACE(DEBUG_LVL_ERROR, "ERROR: Unable to link TPDO to variable! Reason: 0x%x\n", ret);
        goto Exit;
    }
#endif

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_CC)) != 0)
    ret = psi_initCcObjects();
    if(ret != kPsiSuccessful)
    {
        DEBUG_TRACE(DEBUG_LVL_ERROR, "ERROR: Unable to init CC object list! Reason: 0x%x\n", ret);
        goto Exit;
    }
#endif

Exit:
    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Process psi background function

Call modules where data needs to be forwarded in the background.

\ingroup module_psi
*/
//------------------------------------------------------------------------------
tPsiStatus psi_handleAsync(void)
{
    tPsiStatus ret = kPsiSuccessful;
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)
    UINT8 i;
#endif
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_LOGBOOK)) != 0)
    UINT8 j;
#endif

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_CC)) != 0)
    ret = icc_process();
    if(ret != kPsiSuccessful)
    {
        DEBUG_TRACE(DEBUG_LVL_ERROR, "ERROR: icc_process() failed with: 0x%x!\n", ret);
        goto Exit;
    }
#endif

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)
    // Process all instantiated asynchronous channels
    for(i=0; i < kNumSsdoInstCount; i++)
    {
        ret = rssdo_process(psiInstance_l.instRssdoChan_m[i]);
        if(ret != kPsiSuccessful)
        {
            DEBUG_TRACE(DEBUG_LVL_ERROR, "ERROR: rssdo_process() failed for "
                    "instance %d with: 0x%x!\n", i, ret);
            goto Exit;
        }

        ret = tssdo_process(psiInstance_l.instTssdoChan_m[i]);
        if(ret != kPsiSuccessful)
        {
            DEBUG_TRACE(DEBUG_LVL_ERROR, "ERROR: tssdo_process() failed for "
                    "instance %d with: 0x%x!\n", i, ret);
            goto Exit;
        }
    }
#endif

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_LOGBOOK)) != 0)
    // Process all instantiated logger channels
    for(j=0; j < kNumLogInstCount; j++)
    {
        ret = log_process(psiInstance_l.instLogChan_m[j]);
        if(ret != kPsiSuccessful)
        {
            DEBUG_TRACE(DEBUG_LVL_ERROR, "ERROR: log_process() failed for "
                    "instance %d with: 0x%x!\n", j, ret);
            goto Exit;
        }
    }
#endif

Exit:
    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Process psi synchronous functions

\param[in] pNetTime_p       Pointer to the current nettime

Call modules where data needs to be forwarded in the synchronous interrupt.

\ingroup module_psi
*/
//------------------------------------------------------------------------------
tPsiStatus psi_handleSync(tNetTime * pNetTime_p)
{
    tPsiStatus ret = kPsiSuccessful;
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)
    UINT8 i;
#endif
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_LOGBOOK)) != 0)
    UINT8 j;
#else
    UNUSED_PARAMETER(pNetTime_p);
#endif

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_CC)) != 0)
    // Handle configuration channel module
    ret = occ_handleOutgoing();
    if(ret != kPsiSuccessful)
    {
        goto Exit;
    }

    ret = icc_handleIncoming();
    if(ret != kPsiSuccessful)
    {
        goto Exit;
    }
#endif

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)
    // Process all instantiated asynchronous channels
    for(i=0; i < kNumSsdoInstCount; i++)
    {
        ret = tssdo_handleIncoming(psiInstance_l.instTssdoChan_m[i]);
        if(ret != kPsiSuccessful)
        {
            DEBUG_TRACE(DEBUG_LVL_ERROR, "ERROR: tssdo_handleIncoming() failed for "
                    "instance %d with: 0x%x!\n", i, ret);
            goto Exit;
        }

        ret = rssdo_processSync(psiInstance_l.instRssdoChan_m[i]);
        if(ret != kPsiSuccessful)
        {
            DEBUG_TRACE(DEBUG_LVL_ERROR, "ERROR: rssdo_processSync() failed for "
                    "instance %d with: 0x%x!\n", i, ret);
            goto Exit;
        }
    }
#endif

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_LOGBOOK)) != 0)
    // Set logbook nettime
    ret = log_setNettime(pNetTime_p);
    if(ret != kPsiSuccessful)
    {
        goto Exit;
    }

    // Process all instantiated logger channels
    for(j=0; j < kNumLogInstCount; j++)
    {
        ret = log_handleIncoming(psiInstance_l.instLogChan_m[j]);
        if(ret != kPsiSuccessful)
        {
            DEBUG_TRACE(DEBUG_LVL_ERROR, "ERROR: log_handleIncoming() failed for "
                    "instance %d with: 0x%x!\n", j, ret);
            goto Exit;
        }
    }
#endif

Exit:
    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Processing of a pdo is finished

\param[in] pdoDir_p       The direction of the PDO

\ingroup module_psi
*/
//------------------------------------------------------------------------------
void psi_pdoProcFinished(tPsiPdoDir pdoDir_p)
{
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_PDO)) != 0)
    switch(pdoDir_p)
    {
        case tPdoDirRpdo:
        {
            rpdo_procFinished();
            break;
        }
        case tPdoDirTpdo:
        {
            tpdo_procFinished();
            break;
        }
        default:
        {
            // Should not happen
            break;
        }
    }
#endif
}


//------------------------------------------------------------------------------
/**
\brief    Handle finished SDO transfers

\param[in] pSdoComFinHdl_p             Handle of the finished SDO transfer

\return tPsiStatus
\retval kPsiSuccessful                On success


\ingroup module_psi
*/
//------------------------------------------------------------------------------
tPsiStatus psi_sdoAccFinished(tSdoComFinished* pSdoComFinHdl_p )
{
    tPsiStatus ret = kPsiSuccessful;

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0  && \
    ((PSI_MODULE_INTEGRATION) & (PSI_MODULE_LOGBOOK)) != 0 )
    /* The SSDO and the logger module are active */
    ret = forwardInstanceHandle((UINT32*)pSdoComFinHdl_p->pUserArg);
#elif(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)
    /* Only the SSDO module is active */
    ret = tssdo_consTxTransferFinished((tTssdoInstance)pSdoComFinHdl_p->pUserArg);
#elif(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_LOGBOOK)) != 0)
    /* Only the logbook module is active */
    ret = log_consTxTransferFinished((tLogInstance)pSdoComFinHdl_p->pUserArg);
#else
    UNUSED_PARAMETER(pSdoComFinHdl_p);
#endif

    return ret;
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0  && \
    ((PSI_MODULE_INTEGRATION) & (PSI_MODULE_LOGBOOK)) != 0 )

//------------------------------------------------------------------------------
/**
\brief    Search for the right instance handle and forward to module

\param[in] pInstHdl_p        The instance handle which needs to be forwarded

\retval kPsiSuccessful          On success
\retval kPsiInvalidHandle       No handle found for this argument

\ingroup module_psi
*/
//------------------------------------------------------------------------------
static tPsiStatus forwardInstanceHandle(UINT32* pInstHdl_p)
{
    tPsiStatus ret = kPsiInvalidHandle;
    UINT8 i;

    /* Iterate over all SSDO channels */
    for(i=0; i < kNumSsdoInstCount; i++)
    {
        if(pInstHdl_p == (UINT32*)psiInstance_l.instTssdoChan_m[i])
        {
            ret = tssdo_consTxTransferFinished((tTssdoInstance)pInstHdl_p);
            goto Exit;
        }
    }

    /* Iterate over all logbook channels */
    for(i=0; i < kNumLogInstCount; i++)
    {
        if(pInstHdl_p == (UINT32*)psiInstance_l.instLogChan_m[i])
        {
            ret = log_consTxTransferFinished((tLogInstance)pInstHdl_p);
            goto Exit;
        }
    }

Exit:
    return ret;
}
#endif

/// \}


