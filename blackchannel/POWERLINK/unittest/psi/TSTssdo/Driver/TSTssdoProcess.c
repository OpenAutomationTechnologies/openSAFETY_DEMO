/**
********************************************************************************
\file   TSTssdoProcess.c

\brief  Test SSDO module init and processing with correct stream init

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
#include <Stubs/STBinitStream.h>
#include <Stubs/STBdummyHandler.h>

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

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Init with valid parameters

\return int
\retval 0       Init successful
\retval other   Init failed

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
int TST_streamInit(void)
{
    BOOL fReturn;

    stb_initBuffers();

    // Call init successfully
    fReturn = stb_initStreamModule();

    return (fReturn != FALSE) ? 0 : 1;
}

//------------------------------------------------------------------------------
/**
\brief Test ssdo module initialization function

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_ssdoInit(void)
{
    tSsdoInitParam InitParam;

    PSI_MEMSET(&InitParam, 0, sizeof(tSsdoInitParam));

    ssdo_init();

    // Test init with no parameter and no channel
    pSsdoInst_l = ssdo_create(kNumSsdoInstCount, NULL);

    CU_ASSERT_EQUAL( pSsdoInst_l, NULL );

    // Test init correct channel but no parameters
    pSsdoInst_l = ssdo_create(kNumSsdoChan0, NULL);

    CU_ASSERT_EQUAL( pSsdoInst_l, NULL );

    // Test init with empty parameters and no channel
    pSsdoInst_l = ssdo_create(kNumSsdoInstCount, &InitParam);

    CU_ASSERT_EQUAL( pSsdoInst_l, NULL );

    // Test init with empty parameters and valid channel
    pSsdoInst_l = ssdo_create(kNumSsdoChan0, &InitParam);

    CU_ASSERT_EQUAL( pSsdoInst_l, NULL );

    // Test init with wrong Ssdo receive buffer id
    InitParam.buffIdRx_m = kTbufCount;
    InitParam.buffIdTx_m = kTbufNumSsdoTransmit0;

    pSsdoInst_l = ssdo_create(kNumSsdoChan0, &InitParam);

    CU_ASSERT_EQUAL( pSsdoInst_l, NULL );

    // Test init with wrong Ssdo transmit buffer id
    InitParam.buffIdRx_m = kTbufNumSsdoReceive0;
    InitParam.buffIdTx_m = kTbufCount;

    pSsdoInst_l = ssdo_create(kNumSsdoChan0, &InitParam);

    CU_ASSERT_EQUAL( pSsdoInst_l, NULL );

    // Test init with parameters and correct channel
    InitParam.buffIdRx_m = kTbufNumSsdoReceive0;
    InitParam.buffIdTx_m = kTbufNumSsdoTransmit0;
    InitParam.pfnRxHandler_m = stb_dummySsdoReceiveHandlerSuccess;

    pSsdoInst_l = ssdo_create(kNumSsdoChan0, &InitParam);

    CU_ASSERT_NOT_EQUAL( pSsdoInst_l, NULL );
}

//------------------------------------------------------------------------------
/**
\brief Process synchronous task of SSDO channel

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_ssdoProcess(void)
{
    BOOL fReturn;

    // Process SSDO sync
    fReturn = stream_processSync();

    CU_ASSERT_TRUE( fReturn );

    // Process SSDO async
    fReturn = ssdo_process(pSsdoInst_l);

    CU_ASSERT_TRUE( fReturn );

    ssdo_destroy(pSsdoInst_l);
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

/// \}

#endif // #if (((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)