/**
********************************************************************************
\file   TSTstatusInitInvalid.c

\brief  Status module test cases for invalid initialization

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
#include <Stubs/STBinitStream.h>
#include <Stubs/STBdummyHandler.h>

#include <libpsi/internal/status.h>
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
\brief    Init with invalid input buffer address

\return int
\retval 0       Init successful
\retval other   Init failed

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
int TST_initBufferInAddrInvalid(void)
{
    BOOL fReturn;

    stb_initDescriptorsChangeSelected(kTbufNumStatusIn, NULL, sizeof(tTbufStatusInStructure));

    fReturn = stb_initStreamModule();

    return (fReturn != FALSE) ? 0 : 1;
}

//------------------------------------------------------------------------------
/**
\brief    Init with invalid output buffer address

\return int
\retval 0       Init successful
\retval other   Init failed

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
int TST_initBufferOutAddrInvalid(void)
{
    BOOL fReturn;

    stb_initDescriptorsChangeSelected(kTbufNumStatusOut, NULL, sizeof(tTbufStatusOutStructure));

    fReturn = stb_initStreamModule();

    return (fReturn != FALSE) ? 0 : 1;
}

//------------------------------------------------------------------------------
/**
\brief    Init with invalid input buffer size

\return int
\retval 0       Init successful
\retval other   Init failed

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
int TST_initBufferInSizeInvalid(void)
{
    BOOL fReturn;

    tTbufStatusInStructure buffInStruct;

    stb_initDescriptorsChangeSelected(kTbufNumStatusIn, (UINT8*)&buffInStruct, 0);

    fReturn = stb_initStreamModule();

    return (fReturn != FALSE) ? 0 : 1;
}

//------------------------------------------------------------------------------
/**
\brief    Init with invalid output buffer size

\return int
\retval 0       Init successful
\retval other   Init failed

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
int TST_initBufferOutSizeInvalid(void)
{
    BOOL fReturn;

    tTbufStatusOutStructure buffOutStruct;

    stb_initDescriptorsChangeSelected(kTbufNumStatusOut, (UINT8*)&buffOutStruct, 0);

    fReturn = stb_initStreamModule();

    return (fReturn != FALSE) ? 0 : 1;
}

//------------------------------------------------------------------------------
/**
\brief    Init with input buffer full post action list

\return int
\retval 0       Init successful
\retval other   Init failed

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
int TST_initInBufferPostActionListFull(void)
{
    BOOL fReturn;
    tTbufStatusInStructure buffStruct;
    UINT8 numActions;

    stb_initDescriptorsChangeSelected(kTbufNumStatusIn, (UINT8*)&buffStruct, sizeof(buffStruct));

    fReturn = stb_initStreamModule();

    if(fReturn != FALSE)
    {
        numActions = kTbufCount - 1;    // Leave one action for the out buffer (Is always initialized first)
        fReturn = stb_initAllActions(kTbufNumStatusIn, kStreamActionPost, numActions);
    }

    return (fReturn != FALSE) ? 0 : 1;
}

//------------------------------------------------------------------------------
/**
\brief    Init with output buffer full pre action list

\return int
\retval 0       Init successful
\retval other   Init failed

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
int TST_initOutBufferPreActionListFull(void)
{
    BOOL fReturn;
    tTbufStatusOutStructure buffStruct;

    stb_initDescriptorsChangeSelected(kTbufNumStatusOut, (UINT8*)&buffStruct, sizeof(buffStruct));

    fReturn = stb_initStreamModule();

    if(fReturn != FALSE)
    {
        fReturn = stb_initAllActions(kTbufNumStatusOut, kStreamActionPre, kTbufCount);
    }

    return (fReturn != FALSE) ? 0 : 1;
}

//------------------------------------------------------------------------------
/**
\brief    Init with output buffer full post action list

\return int
\retval 0       Init successful
\retval other   Init failed

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
int TST_initOutBufferPostActionListFull(void)
{
    BOOL fReturn;
    tTbufStatusOutStructure buffStruct;

    stb_initDescriptorsChangeSelected(kTbufNumStatusOut, (UINT8*)&buffStruct, sizeof(buffStruct));

    fReturn = stb_initStreamModule();

    if(fReturn != FALSE)
    {
        fReturn = stb_initAllActions(kTbufNumStatusOut, kStreamActionPost, kTbufCount);
    }

    return (fReturn != FALSE) ? 0 : 1;
}

//------------------------------------------------------------------------------
/**
\brief Test status module initialization with invalid initialized stream module

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_statusInitFail(void)
{
    BOOL fReturn;
    tStatusInitParam InitParam;

    PSI_MEMSET(&InitParam, 0, sizeof(tStatusInitParam));

    // Test initialization successful
    InitParam.pfnProcSyncCb_m = stb_dummySyncHandlerSuccess;
    InitParam.buffInId_m = kTbufNumStatusIn;
    InitParam.buffOutId_m = kTbufNumStatusOut;

    fReturn = status_init(&InitParam);

    CU_ASSERT_FALSE( fReturn );
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

/// \}