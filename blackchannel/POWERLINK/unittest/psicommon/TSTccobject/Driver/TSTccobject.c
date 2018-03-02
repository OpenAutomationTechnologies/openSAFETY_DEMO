/**
********************************************************************************
\file   TSTccobject.c

\brief  Test drivers for the ccobject module read and write functions

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

#define DUMMY_DATA_LOW          0xAABBCCDD
#define DUMMY_DATA_HIGH         0x11223344

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
\brief    Initialize ccobject module test

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_ccobjectInit(void)
{
    BOOL fReturn;

    fReturn = ccobject_init(NULL);

    CU_ASSERT_FALSE( fReturn );

    fReturn = ccobject_init(stb_dummyCriticalSection);

    CU_ASSERT_TRUE( fReturn );
}

//------------------------------------------------------------------------------
/**
\brief    Init objects initialization tests

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_initObject(void)
{
    BOOL fReturn;
    tConfChanObject dummyObject;

    // Init NULL object
    fReturn = ccobject_initObject(0, NULL );

    CU_ASSERT_FALSE( fReturn );

    // Init all available object
    stb_initAllObjects(DUMMY_OBJECT_IDX, DUMMY_OBJECT_SIZE, 0x0);

    // Init one more -> fail!
    fReturn = ccobject_initObject( CONF_CHAN_NUM_OBJECTS, &dummyObject);

    CU_ASSERT_FALSE( fReturn );

    // Call dummy exit
    ccobject_exit();
}

//------------------------------------------------------------------------------
/**
\brief    Tests for the write objects function

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_writeObject(void)
{
    BOOL fReturn;
    tConfChanObject dummyObject;

    // Write valid object
    dummyObject.objIdx_m = DUMMY_OBJECT_IDX;
    dummyObject.objSubIdx_m = DUMMY_OBJECT_SUBIDX;
    dummyObject.objSize_m = DUMMY_OBJECT_SIZE;
    dummyObject.objPayloadLow_m = DUMMY_DATA_LOW;
    dummyObject.objPayloadHigh_m = DUMMY_DATA_HIGH;

    fReturn = ccobject_writeObject(&dummyObject);

    CU_ASSERT_TRUE( fReturn );

    // Write object with wrong size
    dummyObject.objSize_m = DUMMY_INVALID_OBJECT_SIZE;
    fReturn = ccobject_writeObject(&dummyObject);

    CU_ASSERT_FALSE( fReturn );

    // Write object with not existing index
    dummyObject.objIdx_m = DUMMY_INVALID_OBJECT_IDX;
    dummyObject.objSize_m = DUMMY_OBJECT_SIZE;
    fReturn = ccobject_writeObject(&dummyObject);

    CU_ASSERT_FALSE( fReturn );

    // Write object with not existing sub index
    dummyObject.objIdx_m = DUMMY_OBJECT_IDX;
    dummyObject.objSubIdx_m = DUMMY_INVALID_OBJECT_SUBIDX;
    fReturn = ccobject_writeObject(&dummyObject);

    CU_ASSERT_FALSE( fReturn );
}

//------------------------------------------------------------------------------
/**
\brief    Tests for the read objects function

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_readObject(void)
{
    tConfChanObject* obj;

    // Read object with invalid subindex
    obj = ccobject_readObject(DUMMY_OBJECT_IDX, DUMMY_INVALID_OBJECT_SUBIDX);

    CU_ASSERT_EQUAL( obj, NULL);

    // Read object with invalid index
    obj = ccobject_readObject(DUMMY_INVALID_OBJECT_IDX, DUMMY_OBJECT_SUBIDX);

    CU_ASSERT_EQUAL( obj, NULL);

    // Read valid object
    obj = ccobject_readObject(DUMMY_OBJECT_IDX, DUMMY_OBJECT_SUBIDX);

    CU_ASSERT_NOT_EQUAL( obj, NULL);
}

//------------------------------------------------------------------------------
/**
\brief    Tests for the get objects size function

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_getObjectSize(void)
{
    BOOL fReturn;
    UINT8 size = 0xFF;

    // Successfully get object size
    fReturn = ccobject_getObjectSize(DUMMY_OBJECT_IDX, DUMMY_OBJECT_SUBIDX, &size);

    CU_ASSERT_TRUE( fReturn );
    CU_ASSERT_EQUAL( size, DUMMY_OBJECT_SIZE );

    // Get size with invalid index
    size = 0xFF;
    fReturn = ccobject_getObjectSize(DUMMY_INVALID_OBJECT_IDX, DUMMY_OBJECT_SUBIDX, &size);

    CU_ASSERT_FALSE( fReturn );
    CU_ASSERT_NOT_EQUAL( size, DUMMY_OBJECT_SIZE );

    // Get size with invalid sub index
    size = 0xFF;
    fReturn = ccobject_getObjectSize(DUMMY_OBJECT_IDX, DUMMY_INVALID_OBJECT_SUBIDX, &size);

    CU_ASSERT_FALSE( fReturn );
    CU_ASSERT_NOT_EQUAL( size, DUMMY_OBJECT_SIZE );
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

/// \}