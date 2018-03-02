/**
********************************************************************************
\file   cunit_main.c

\brief  Unit tests main file

This file defines the framework for unittests performed in the PSI interface

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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <getopt.h>

#include <cunit/Automated.h>
#include <cunit/Basic.h>

#include <cunit_main.h>

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
extern void TST_AddTests(void);

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
static bool runDebug = FALSE;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------


//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Check if debug is enabled

\return bool
\retval TRUE     Debug enabled
\retval FALSE    Debug disabled

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
bool CU_AllowDebug()
{
    return runDebug;
}

//------------------------------------------------------------------------------
/**
\brief    Test program entry point.

Calls CUnit initialization routines, adds all tests to the CUnit registry and
executes them.

\param argc     Count of arguments
\param argv     The program arguments

\return The number of failed tests

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
int main(int argc, char **argv)
{
    unsigned int nrOfFailures = 0;

    bool printNrFailures = FALSE;
    bool runVerbose = TRUE;

    int c;

    while ( EOF != ( c = getopt ( argc, argv, "fdvq" ) ) )
    {
        switch( c )
        {
            case 'd':
                runDebug = TRUE;
                break;
            case 'f':
                printNrFailures = TRUE;
                break;
            case 'v':
                runVerbose = TRUE;
                break;
            case 'q':
                runVerbose = FALSE;
                break;
            default:
                break;
        }
    }

    // set test mode
    CU_basic_set_mode(runVerbose ? CU_BRM_VERBOSE : CU_BRM_SILENT);
    // exit on errors in the framework
    CU_set_error_action(CUEA_ABORT);

    if(CU_initialize_registry() == CUE_SUCCESS)
    {
        // Add all tests
        TST_AddTests();

#if CUNIT_AUTOMATED
        CU_automated_run_tests();
        CU_list_tests_to_file();
#else
        // execute tests
        if(CU_basic_run_tests() != CUE_SUCCESS)
            printf("\n Test run failed");

        nrOfFailures = CU_get_number_of_tests_failed();
#endif
        // cleanup CUnit registry
        CU_cleanup_registry();
    }

    if ( printNrFailures )
        printf ( "Nr of Failures: %d\n", nrOfFailures );

    return nrOfFailures;
}

//------------------------------------------------------------------------------
/**
\brief    Exit the Unit tests

Called by the function 'CU_set_error' if an internal CUnit error occurred. Exits
the test run and shows abort condition on the stdout.

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_exit(void)
{
    // print out error on stdout
    printf("\nTest stopped by CUnit with internal error #%d: %s\n",
                CU_get_error(),
                CU_get_error_msg());
    // exit test program
    exit(CU_get_error());
}
