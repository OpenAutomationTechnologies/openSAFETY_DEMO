/**
********************************************************************************
\file   TSTccobjectCurrent.c

\brief  Test drivers for the ccobject module current objects

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

#include <Driver/TSTccobjectConfig.h>
#include <Stubs/STBinitObjects.h>
#include <Stubs/STBcritSec.h>

#include <libpsicommon/ccobject.h>

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
#define DUMMY_OBJECT_IDX              0x6000
#define DUMMY_OBJECT_SUBIDX             0x01
#define DUMMY_OBJECT_SIZE                  1

#define DUMMY_INVALID_OBJECT_IDX      0x6001
#define DUMMY_INVALID_OBJECT_SUBIDX     0xFF
#define DUMMY_INVALID_OBJECT_SIZE          2

#define DUMMY_DATA          (UINT64) 0x11223344AABBCCDD

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void testCurrentFunctions(UINT8 size_p);


//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Test ccobjects current functions with int8

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_ccobjectCurrInt8(void)
{
    // Init module
    ccobject_init(stb_dummyCriticalSection);

    testCurrentFunctions(sizeof(UINT8));
}

//------------------------------------------------------------------------------
/**
\brief    Test ccobjects current functions with int16

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_ccobjectCurrInt16(void)
{
    // Init module
    ccobject_init(stb_dummyCriticalSection);

    testCurrentFunctions(sizeof(UINT16));
}

//------------------------------------------------------------------------------
/**
\brief    Test ccobjects current functions with int32

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_ccobjectCurrInt32(void)
{
    // Init module
    ccobject_init(stb_dummyCriticalSection);

    testCurrentFunctions(sizeof(UINT32));
}

//------------------------------------------------------------------------------
/**
\brief    Test ccobjects current functions with int64

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_ccobjectCurrInt64(void)
{
    // Init module
    ccobject_init(stb_dummyCriticalSection);

    testCurrentFunctions(sizeof(UINT64));
}

//------------------------------------------------------------------------------
/**
\brief    Test cc object current write function error conditions

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_writeCurrObjectError(void)
{
    UINT64 dummyData = DUMMY_DATA;
    UINT8* pData = (UINT8*)&dummyData;
    tCcWriteState writeState;

    // Write object with invalid subindex
    writeState = ccobject_writeCurrObject(DUMMY_OBJECT_IDX, CONF_CHAN_NUM_OBJECTS + 1, pData);

    CU_ASSERT_EQUAL( writeState, kCcWriteStateOutOfSync );

    // Write object with invalid index
    writeState = ccobject_writeCurrObject(DUMMY_INVALID_OBJECT_IDX, DUMMY_OBJECT_SUBIDX, pData);

    CU_ASSERT_EQUAL( writeState, kCcWriteStateOutOfSync );
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

//------------------------------------------------------------------------------
/**
\brief    Test current functions read and write

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
static void testCurrentFunctions(UINT8 size_p)
{
    BOOL fReturn;
    UINT8 i;
    tConfChanObject* pReadObject;
    UINT64 dummyData;
    UINT64 resData;
    UINT8* pData;

    switch(size_p)
    {
        case sizeof(UINT8):
        {
            dummyData = (UINT8)DUMMY_DATA;
            break;
        }
        case sizeof(UINT16):
        {
            dummyData = (UINT16)DUMMY_DATA;
            break;
        }
        case sizeof(UINT32):
        {
            dummyData = (UINT32)DUMMY_DATA;
            break;
        }
        default:
        {
            dummyData = (UINT64)DUMMY_DATA;
            break;
        }
    }

    pData = (UINT8*)&dummyData;

    fReturn = stb_initAllObjects(DUMMY_OBJECT_IDX, size_p, 0x0);

    CU_ASSERT_TRUE( fReturn );

    // Write all objects via current objects
    for(i=0; i < CONF_CHAN_NUM_OBJECTS; i++)
    {
        fReturn = ccobject_writeCurrObject(DUMMY_OBJECT_IDX, i, pData);

        CU_ASSERT_TRUE( fReturn );

        ccobject_incObjWritePointer();
    }

    // Read back objects via current objects
    for(i=0; i < CONF_CHAN_NUM_OBJECTS; i++)
    {
        pReadObject = ccobject_readCurrObject();

        CU_ASSERT_NOT_EQUAL( pReadObject, NULL );

        resData = (((UINT64)pReadObject->objPayloadHigh_m << 32) | pReadObject->objPayloadLow_m);

        CU_ASSERT_EQUAL( resData, dummyData );

        ccobject_incObjReadPointer();
    }
}

/// \}