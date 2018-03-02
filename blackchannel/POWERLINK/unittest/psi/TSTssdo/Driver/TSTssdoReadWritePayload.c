/**
********************************************************************************
\file   TSTssdoReadWritePayload.c

\brief  Test SSDO module read and write payload API functions

\ingroup module_unittests
*******************************************************************************/

/*------------------------------------------------------------------------------
* License Agreement
*
* Copyright (c) 2013, B&R Industrial Automation GmbH
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
#include <cunit/CUnit.h>

#include <Driver/TSTssdoConfig.h>
#include <Stubs/STBdescList.h>
#include <Stubs/STBdummyHandler.h>
#include <Stubs/STBreceivePayload.h>

#include <libpsi/internal/ssdo.h>
#include <libpsi/internal/stream.h>

#if (((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)

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

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static tSsdoInstance pSsdoInst_l = NULL;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static BOOL streamHandlerRxPayload(tHandlerParam* pHandlParam_p);
static BOOL processSyncAsync(void);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Full init with valid parameters

\return int
\retval 0       Init successful
\retval other   Init failed

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
int TST_initFull(void)
{
    BOOL fReturn;
    tStreamInitParam streamInitParam;
    tSsdoInitParam ssdoInitParam;
    tStatusInitParam statusInitParam;

    stb_initBuffers();

    // Call init of stream module
    streamInitParam.pfnStreamHandler_m = streamHandlerRxPayload;
    streamInitParam.pBuffDescList_m = stb_getDescList();
    streamInitParam.idConsAck_m = (tTbufNumLayout)0;
    streamInitParam.idFirstProdBuffer_m = (tTbufNumLayout)(TBUF_NUM_CON + 1);

    fReturn = stream_init(&streamInitParam);

    if(fReturn != FALSE)
    {
        statusInitParam.buffInId_m = kTbufNumStatusIn;
        statusInitParam.buffOutId_m = kTbufNumStatusOut;
        statusInitParam.pfnProcSyncCb_m = stb_dummySyncHandlerSuccess;

        fReturn = status_init(&statusInitParam);

        if(fReturn != FALSE)
        {
            // Call init of SSDO module
            ssdoInitParam.buffIdRx_m = kTbufNumSsdoReceive0;
            ssdoInitParam.buffIdTx_m = kTbufNumSsdoTransmit0;
            ssdoInitParam.pfnRxHandler_m = stb_dummySsdoReceiveHandlerSuccess;

            pSsdoInst_l = ssdo_create(kNumSsdoChan0, &ssdoInitParam);
        }
    }

    return (fReturn != FALSE) ? 0 : 1;
}

//------------------------------------------------------------------------------
/**
\brief Test SSDO write payload API function

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_ssdoWritePayload(void)
{
    BOOL fReturn;
    UINT8 asyncPayload[20];
    UINT16 i;
    tSsdoTxStatus txState;

    PSI_MEMSET(&asyncPayload, 0xAA, sizeof(asyncPayload));

    // Perform post with no parameters
    txState = ssdo_postPayload(NULL, NULL, 0);

    CU_ASSERT_EQUAL( txState, kSsdoTxStatusError );

    // Perform write with no payload
    txState = ssdo_postPayload(pSsdoInst_l, NULL, 0);

    CU_ASSERT_EQUAL( txState, kSsdoTxStatusError );

    // Perform write with size too low
    txState = ssdo_postPayload(pSsdoInst_l, &asyncPayload[0], 0);

    CU_ASSERT_EQUAL( txState, kCcWriteStatusError );

    // Perform write with size too high
    txState = ssdo_postPayload(pSsdoInst_l, &asyncPayload[0], 
        sizeof(((tTbufSsdoTxStructure *)0)->tssdoTransmitData_m) + 1);

    CU_ASSERT_EQUAL( txState, kCcWriteStatusError );

    // Perform valid posting of data
    txState = ssdo_postPayload(pSsdoInst_l, &asyncPayload[0], sizeof(asyncPayload));

    CU_ASSERT_EQUAL( txState, kSsdoTxStatusSuccessful );

    fReturn = ssdo_process(pSsdoInst_l);

    CU_ASSERT_TRUE( fReturn );

    // Perform second write to signal channel busy
    txState = ssdo_postPayload(pSsdoInst_l, &asyncPayload[0], sizeof(asyncPayload));

    CU_ASSERT_EQUAL( txState, kSsdoTxStatusBusy );

    // Process sync until timeout occures
    for(i=0; i < SSDO_TX_TIMEOUT_CYCLE_COUNT + 1; i++)
    {
        fReturn = stream_processSync();

        CU_ASSERT_TRUE( fReturn );
    }

    // Process async for timeout
    fReturn = ssdo_process(pSsdoInst_l);

    CU_ASSERT_TRUE( fReturn );

    // Write again to post an other frame to free channel
    txState = ssdo_postPayload(pSsdoInst_l, &asyncPayload[0], sizeof(asyncPayload));

    CU_ASSERT_EQUAL( txState, kSsdoTxStatusSuccessful );

    // Process new frame
    fReturn = ssdo_process(pSsdoInst_l);

    CU_ASSERT_TRUE( fReturn );
}

//------------------------------------------------------------------------------
/**
\brief Test SSDO receive payload handler

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_receivePayload(void)
{
    BOOL fReturn;

    // Process sync with enabled rx receive
    stb_enableReceivePayload();

    // Set sequence number to new value
    stb_setSequenceNumber(kSeqNrValueSecond);

    // Receive frame with process sync -> Process frame with async
    fReturn = processSyncAsync();

    CU_ASSERT_TRUE( fReturn );

    // Carry out process again to receive frame with same sequence number twice
    // -> Received frame will be ignored!
    fReturn = processSyncAsync();

    CU_ASSERT_TRUE( fReturn );

    stb_disableReceivePayload();

    // destroy instance
    ssdo_destroy(pSsdoInst_l);
}

//------------------------------------------------------------------------------
/**
\brief Test SSDO receive payload handler with failing user callback

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_receivePayloadInvalidHandler(void)
{
    BOOL fReturn;
    tSsdoInitParam ssdoInitParam;

    // Call init of SSDO module with failing user handler
    ssdoInitParam.buffIdRx_m = kTbufNumSsdoReceive0;
    ssdoInitParam.buffIdTx_m = kTbufNumSsdoTransmit0;
    ssdoInitParam.pfnRxHandler_m = stb_dummySsdoReceiveHandlerFail;

    pSsdoInst_l = ssdo_create(kNumSsdoChan0, &ssdoInitParam);

    // toggle sequence number
    stb_setSequenceNumber(kSeqNrValueFirst);

    stb_enableReceivePayload();

    // Receive frame with process sync -> Process frame with async
    fReturn = processSyncAsync();

    CU_ASSERT_FALSE( fReturn );

    stb_disableReceivePayload();

    // destroy instance
    ssdo_destroy(pSsdoInst_l);
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

//------------------------------------------------------------------------------
/**
\brief Stream handler which simulates arriving frames

\param pHandlParam_p        Stream handler parameter

\return BOOL
\retval TRUE        Handler processing successful

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
static BOOL streamHandlerRxPayload(tHandlerParam* pHandlParam_p)
{
    stb_receivePayload();

    return TRUE;
}

//------------------------------------------------------------------------------
/**
\brief Process both sync and async tasks

\return BOOL
\retval TRUE    Processing success
\retval FALSE   Error on processing

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
static BOOL processSyncAsync(void)
{
    BOOL fReturn;

    // Process sync task to detect frame
    fReturn = stream_processSync();

    if(fReturn != FALSE)
    {
        // Process async task to inform user
        fReturn = ssdo_process(pSsdoInst_l);
    }

    return fReturn;
}

/// \}

#endif // #if (((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)