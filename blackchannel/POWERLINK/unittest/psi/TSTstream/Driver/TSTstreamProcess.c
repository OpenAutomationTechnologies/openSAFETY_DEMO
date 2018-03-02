/**
********************************************************************************
\file   TSTstreamProcess.c

\brief  Test the stream module process function

Tests all successful process function conditions. The failing condigitions
are tested by an other test.

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

#include <Driver/TSTstreamConfig.h>
#include <Stubs/STBdescList.h>
#include <Stubs/STBdummyHandler.h>

#include <libpsi/internal/stream.h>

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

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Initialize the process tests

\return int
\retval 0       On success
\retval other   Init failed

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
int TST_processInit(void)
{
    BOOL fReturn;
    tStreamInitParam InitParam;

    // Create image of the transfer buffers
    stb_initBuffers();

    // Init module
    InitParam.pfnStreamHandler_m = stb_streamHandlerSuccess;
    InitParam.pBuffDescList_m = stb_getDescList();
    InitParam.idConsAck_m = (tTbufNumLayout)0;
    InitParam.idFirstProdBuffer_m = (tTbufNumLayout)(TBUF_NUM_CON + 1);

    fReturn = stream_init(&InitParam);

    return (fReturn != FALSE ? 0 : 1);
}

//------------------------------------------------------------------------------
/**
\brief    Add action to the stream module test

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_addAction(void)
{
    BOOL fReturn;
    UINT8 i;

    // Register action invalid parameter
    fReturn = stream_registerAction(kStreamActionPre, 0, NULL, NULL);

    CU_ASSERT_FALSE ( fReturn );

    // Another invalid parameter
    fReturn = stream_registerAction(kStreamActionInvalid, 0,
        stb_dummyActionSuccess, NULL);

    CU_ASSERT_FALSE ( fReturn );

    // Register all pre actions
    for(i=0; i < kTbufCount; i++)
    {
        fReturn = stream_registerAction(kStreamActionPre, 0,
            stb_dummyActionSuccess, NULL);
    }

    CU_ASSERT_TRUE ( fReturn );

    // Register all post actions
    for(i=0; i < kTbufCount; i++)
    {
        fReturn = stream_registerAction(kStreamActionPost, 0,
            stb_dummyActionSuccess, NULL);
    }

    CU_ASSERT_TRUE ( fReturn );

    // Register one more pre action
    fReturn = stream_registerAction(kStreamActionPre, 0,
            stb_dummyActionSuccess, NULL);

    CU_ASSERT_FALSE ( fReturn );

    // Register one more post action
    fReturn = stream_registerAction(kStreamActionPost, 0,
            stb_dummyActionSuccess, NULL);

    CU_ASSERT_FALSE ( fReturn );
}

//------------------------------------------------------------------------------
/**
\brief    Process stream module test

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_process(void)
{
    BOOL fReturn;

    fReturn = stream_processPostActions();

    CU_ASSERT_TRUE ( fReturn );

    stream_registerSyncCb(stb_dummySyncCbSuccess);

    fReturn = stream_processPostActions();
    CU_ASSERT_TRUE ( fReturn );

    stream_registerSyncCb(stb_dummySyncCbFail);

    fReturn = stream_processPostActions();
    CU_ASSERT_FALSE ( fReturn );
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{


/// \}