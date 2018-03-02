/**
********************************************************************************
\file   TSTstreamInit.c

\brief  Test the stream module initialization and exit function

Test the basic functions of the stream module like init, exit and the buffer
parameter return function.

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
\brief    Initialize stream module test

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_init(void)
{
    BOOL fReturn;
    tStreamInitParam InitParam;

    // Call init with initializer set to null
    fReturn = stream_init(NULL);

    CU_ASSERT_FALSE ( fReturn );

    // Create image of the transfer buffers
    stb_initBuffers();

    PSI_MEMSET(&InitParam, 0, sizeof(tStreamInitParam));

    fReturn = stream_init(&InitParam);

    CU_ASSERT_FALSE ( fReturn );

    // Call init with only stream handler set
    InitParam.pfnStreamHandler_m = stb_streamHandlerSuccess;

    fReturn = stream_init(&InitParam);

    CU_ASSERT_FALSE ( fReturn );

    // Call init with only descriptor list set
    InitParam.pfnStreamHandler_m = NULL;
    InitParam.pBuffDescList_m = stb_getDescList();

    fReturn = stream_init(&InitParam);

    CU_ASSERT_FALSE ( fReturn );

    // Call init successfully
    InitParam.pfnStreamHandler_m = stb_streamHandlerSuccess;
    InitParam.pBuffDescList_m = stb_getDescList();
    InitParam.idConsAck_m = (tTbufNumLayout)0;
    InitParam.idFirstProdBuffer_m = (tTbufNumLayout)(TBUF_NUM_CON + 1);

    fReturn = stream_init(&InitParam);

    CU_ASSERT_FALSE ( !fReturn );

    // Call exit function
    stream_exit();
}

//------------------------------------------------------------------------------
/**
\brief    Get buffer parameter test

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_getBufferParam(void)
{
    UINT8 i;
    tBuffDescriptor *pDesc;

    for(i=0; i < kTbufCount; i++)
    {
        pDesc = stream_getBufferParam((tTbufNumLayout)i);

        CU_ASSERT_NOT_EQUAL(pDesc, NULL);
    }

    // Call get buffer for not existing buffer
    pDesc = stream_getBufferParam((tTbufNumLayout)i);

    CU_ASSERT_EQUAL(pDesc, NULL);
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

/// \}