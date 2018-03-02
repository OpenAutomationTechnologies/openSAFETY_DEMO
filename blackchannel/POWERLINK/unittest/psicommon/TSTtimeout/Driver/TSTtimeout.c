/**
********************************************************************************
\file   TSTtimeout.c

\brief  Test drivers for the timeout module

This driver tests the whole timeout module of the slim interface.

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

#include <Driver/TSTtimeoutConfig.h>

#include <libpsicommon/internal/timeout.h>

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
#define DUMMY_CYCLE_LIMIT       1000    ///< Timeout module cycle limit

#define TEST_INSTANCE_0         0       ///< The first instance to test

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static tTimeoutInstance  pInstances_l[TIMEOUT_MAX_INSTANCES];

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Initialize timeout module test

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_timeoutInit(void)
{
    UINT8 i;
    tTimeoutInstance pInstance;

    // Init module
    timeout_init();

    // Create all valid instances
    for(i=0; i < TIMEOUT_MAX_INSTANCES; i++)
    {
        pInstances_l[i] = timeout_create(DUMMY_CYCLE_LIMIT);

        CU_ASSERT_NOT_EQUAL(pInstances_l, NULL);
    }

    // Create additional failing instance
    pInstance = timeout_create(DUMMY_CYCLE_LIMIT);
    CU_ASSERT_EQUAL(pInstance, NULL);
}

//------------------------------------------------------------------------------
/**
\brief    Test the timeout module API when the timer is not running

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_timeoutNotRunning(void)
{
    tTimerStatus timerState;

    // Test is running function when timer is stopped
    timerState = timeout_isRunning(pInstances_l[TEST_INSTANCE_0]);

    CU_ASSERT_EQUAL(timerState, kTimerStateStopped);

    // Test check expire function when timer is stopped
    timerState = timeout_checkExpire(pInstances_l[TEST_INSTANCE_0]);

    CU_ASSERT_EQUAL(timerState, kTimerStateStopped);

    // Test increment counter function when timer is stopped
    timeout_incrementCounter(pInstances_l[TEST_INSTANCE_0]);
}

//------------------------------------------------------------------------------
/**
\brief    Test the timeout API when the timer is started

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_timeoutStartTimer(void)
{
    tTimerStatus timerState;

    timeout_startTimer(pInstances_l[TEST_INSTANCE_0]);

    // Test timer is running function when timer is started
    timerState = timeout_isRunning(pInstances_l[TEST_INSTANCE_0]);

    CU_ASSERT_EQUAL(timerState, kTimerStateRunning);

    timerState = timeout_checkExpire(pInstances_l[TEST_INSTANCE_0]);

    CU_ASSERT_EQUAL(timerState, kTimerStateRunning);
}

//------------------------------------------------------------------------------
/**
\brief    Increment the timer counter and check it's state

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_timeoutIncrement(void)
{
    UINT16 i;
    tTimerStatus timerState;

    for(i=0; i < DUMMY_CYCLE_LIMIT; i++)
    {
        timeout_incrementCounter(pInstances_l[TEST_INSTANCE_0]);
        timerState = timeout_checkExpire(pInstances_l[TEST_INSTANCE_0]);

        CU_ASSERT_EQUAL(timerState, kTimerStateRunning);
    }

    // Check if timer is expired
    timeout_incrementCounter(pInstances_l[TEST_INSTANCE_0]);
    timerState = timeout_checkExpire(pInstances_l[TEST_INSTANCE_0]);

    CU_ASSERT_EQUAL(timerState, kTimerStateExpired);
}

//------------------------------------------------------------------------------
/**
\brief    Stop the timer under test

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_timeoutStopTimer(void)
{
    timeout_stopTimer(pInstances_l[TEST_INSTANCE_0]);
}

//------------------------------------------------------------------------------
/**
\brief    Destroy all allocated timer instances

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_timeoutDestroy(void)
{
    UINT8 i;

    // Destroy all timeout module instances
    for(i=0; i < TIMEOUT_MAX_INSTANCES; i++)
    {
        timeout_destroy(pInstances_l[i]);
    }

    // Test destroy with invalid parameter
    timeout_destroy(NULL);
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

/// \}