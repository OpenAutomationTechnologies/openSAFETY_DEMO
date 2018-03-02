/**
********************************************************************************
\file   TSTstatusAsyncField.c

\brief  Test status module asyncronous channel fields

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

#include <Driver/TSTstatusConfig.h>
#include <Stubs/STBdescList.h>
#include <Stubs/STBdummyHandler.h>

#include <libpsi/internal/status.h>
#include <libpsi/internal/stream.h>


//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define ASYNC_CHANNEL_UUT       0   ///< Async channel under test

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

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static BOOL streamHandler(tHandlerParam* pHandlParam_p);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Initialize stream and status module

\return int
\retval 0       Init successful
\retval other   Init failed

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
int TST_asyncInitStatus(void)
{
    BOOL fReturn;
    tStreamInitParam StreamInitParam;
    tStatusInitParam StatusInitParam;

    // Initialize image of the transfer buffers
    stb_initBuffers();

    // Init Stream module
    StreamInitParam.pfnStreamHandler_m = streamHandler;
    StreamInitParam.pBuffDescList_m = stb_getDescList();
    StreamInitParam.idConsAck_m = (tTbufNumLayout)0;
    StreamInitParam.idFirstProdBuffer_m = (tTbufNumLayout)(TBUF_NUM_CON + 1);

    fReturn = stream_init(&StreamInitParam);

    if(fReturn != FALSE)
    {
        // Init status module
        StatusInitParam.pfnProcSyncCb_m = stb_dummySyncHandlerSuccess;
        StatusInitParam.buffInId_m = kTbufNumStatusIn;
        StatusInitParam.buffOutId_m = kTbufNumStatusOut;

        fReturn = status_init(&StatusInitParam);
    }

    return (fReturn != FALSE) ? 0 : 1;
}

//------------------------------------------------------------------------------
/**
\brief    Test for changing the asynchronous module status flags

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_statusChangeAsyncStatus(void)
{
    BOOL fReturn;
    tSeqNrValue seqNrIcc, seqNrSsdoTx;

    // Run tests with initial settings
    status_getIccStatus(&seqNrIcc);

    CU_ASSERT_EQUAL( seqNrIcc, kSeqNrValueFirst );

    status_getSsdoTxChanFlag(ASYNC_CHANNEL_UUT, &seqNrSsdoTx);

    CU_ASSERT_EQUAL( seqNrSsdoTx, kSeqNrValueFirst );

    status_setSsdoRxChanFlag(ASYNC_CHANNEL_UUT, kSeqNrValueFirst);

    // Call process function (calls stream handler)
    fReturn = stream_processSync();
    CU_ASSERT_TRUE_FATAL( fReturn );

    fReturn = stream_processPostActions();
    CU_ASSERT_TRUE_FATAL( fReturn );

    // Run tests with changed settings
    status_getIccStatus(&seqNrIcc);

    CU_ASSERT_EQUAL( seqNrIcc, kSeqNrValueSecond );

    status_getSsdoTxChanFlag(ASYNC_CHANNEL_UUT, &seqNrSsdoTx);

    CU_ASSERT_EQUAL( seqNrSsdoTx, kSeqNrValueSecond );

    status_setSsdoRxChanFlag(ASYNC_CHANNEL_UUT, kSeqNrValueSecond);
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

//------------------------------------------------------------------------------
/**
\brief    Stream handler for changing the state of the status flags

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
static BOOL streamHandler(tHandlerParam* pHandlParam_p)
{
    tBuffDescriptor* pBuffDesc;
    tTbufStatusOutStructure* pStatOutStruct;

    UNUSED_PARAMETER(pHandlParam_p);

    // Set icc status busy flag
    pBuffDesc = stb_getDescElement(kTbufNumStatusOut);

    pStatOutStruct = (tTbufStatusOutStructure*)pBuffDesc->pBuffBase_m;

    pStatOutStruct->iccStatus_m |= (1 << STATUS_ICC_BUSY_FLAG_POS);

    // Set ssdo tx status busy flag
    pStatOutStruct->ssdoConsStatus_m |= (1 << ASYNC_CHANNEL_UUT);

    return TRUE;
}

/// \}