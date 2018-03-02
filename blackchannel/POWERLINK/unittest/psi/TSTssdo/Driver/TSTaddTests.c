/**
********************************************************************************
\file   TSTaddTests.c

\brief  Create a test suite and add tests to it

Create a suite and add ssdo module tests to it.

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

#include <assert.h>
#include <stdlib.h>

#include <cunit/CUnit.h>

#include <Driver/TSTssdoConfig.h>

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

#if (((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)

/* Empty cleanup function for the tests */
static int TST_defaultClean(void)
{
    return 0;
}

static CU_TestInfo ssdoProcessSuite[] = {
    { "Simple initialization tests", TST_ssdoInit },
    { "Module process test", TST_ssdoProcess },
    CU_TEST_INFO_NULL,
};

static CU_TestInfo ssdoInitInvalidSuite[] = {
    { "Test with invalid stream module initialization", TST_ssdoInitFail },
    CU_TEST_INFO_NULL,
};

static CU_TestInfo ssdoReadWriteSuite[] = {
    { "Test write payload API functions", TST_ssdoWritePayload },
    { "Receive payload tests", TST_receivePayload },
    { "Receive payload with invalid user handler", TST_receivePayloadInvalidHandler },
    CU_TEST_INFO_NULL,
};

static CU_TestInfo ssdoSuite[] = {
    { "Test ssdo process function of internal module", TST_internalProcess },
    { "Test ssdo process function of failing receive handler", TST_internalProcessRxHandlerFail },
    CU_TEST_INFO_NULL,
};

static CU_SuiteInfo suites[] = {
    { "Process suite", TST_streamInit, TST_defaultClean, ssdoProcessSuite },
    { "Buffer rx address invalid", TST_initSsdoRxAddrInvalid, TST_defaultClean, ssdoInitInvalidSuite },
    { "Buffer tx address invalid", TST_initSsdoTxAddrInvalid, TST_defaultClean, ssdoInitInvalidSuite },
    { "Buffer rx size invalid", TST_initSsdoRxSizeInvalid, TST_defaultClean, ssdoInitInvalidSuite },
    { "Buffer tx size invalid", TST_initSsdoTxSizeInvalid, TST_defaultClean, ssdoInitInvalidSuite },
    { "Buffer rx post action list full", TST_initRxPostActionListFull, TST_defaultClean, ssdoInitInvalidSuite },
    { "Buffer tx with no timeout instance available", TST_initTxTimeoutInitFails, TST_defaultClean, ssdoInitInvalidSuite },
    { "Test read write API functions", TST_initFull, TST_defaultClean, ssdoReadWriteSuite },
    { "Ssdo module suite", TST_initInternal, TST_defaultClean, ssdoSuite },
    CU_SUITE_INFO_NULL,
};
#else
  // Pass empty suite to cunit
  static CU_SuiteInfo suites[] = { { NULL, NULL, NULL, NULL } };
#endif // #if (((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Add tests to the suites

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_AddTests(void)
{
    assert(NULL != CU_get_registry());
    assert(!CU_is_test_running());

    /* Register suites. */
    if (CU_register_suites(suites) != CUE_SUCCESS) {
            fprintf(stderr, "suite registration failed - %s\n",
                    CU_get_error_msg());
            exit(EXIT_FAILURE);
    }
} /*TST_AddTests()*/
