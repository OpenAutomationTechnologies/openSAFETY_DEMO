/**
********************************************************************************
\file   TSTami.c

\brief  Drivers for the ami module tests

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

#include <Driver/TSTamiConfig.h>

#include <libpsicommon/ami.h>

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

#if !defined(PLATFORM_LE) && !defined(PLATFORM_BE)
    #error "Please define either PLATFORM_LE or PLATFORM_BE"
#endif

#define UINT16_IN_DATA                  (UINT16)0xAABB
#ifdef PLATFORM_BE
  #define UINT16_OUT_DATA_SET_BE        UINT16_IN_DATA
  #define UINT16_OUT_DATA_SET_LE        (UINT16)0xBBAA
  #define UINT16_OUT_DATA_GET_FROM_BE   UINT16_IN_DATA
  #define UINT16_OUT_DATA_GET_FROM_LE   (UINT16)0xBBAA
#else
  #define UINT16_OUT_DATA_SET_BE        (UINT16)0xBBAA
  #define UINT16_OUT_DATA_SET_LE        UINT16_IN_DATA
  #define UINT16_OUT_DATA_GET_FROM_BE   (UINT16)0xBBAA
  #define UINT16_OUT_DATA_GET_FROM_LE   UINT16_IN_DATA
#endif

#define UINT32_IN_DATA                  (UINT32)0xAABBCCDD
#ifdef PLATFORM_BE
  #define UINT32_OUT_DATA_SET_BE        UINT32_IN_DATA
  #define UINT32_OUT_DATA_SET_LE        (UINT32)0xDDCCBBAA
  #define UINT32_OUT_DATA_GET_FROM_BE   UINT32_IN_DATA
  #define UINT32_OUT_DATA_GET_FROM_LE   (UINT32)0xDDCCBBAA
#else
  #define UINT32_OUT_DATA_SET_BE        (UINT32)0xDDCCBBAA
  #define UINT32_OUT_DATA_SET_LE        UINT32_IN_DATA
  #define UINT32_OUT_DATA_GET_FROM_BE   (UINT32)0xDDCCBBAA
  #define UINT32_OUT_DATA_GET_FROM_LE   UINT32_IN_DATA
#endif

#define UINT64_IN_DATA                  (UINT64)0x1122334455667788
#ifdef PLATFORM_BE
  #define UINT64_OUT_DATA_SET_BE        UINT64_IN_DATA
  #define UINT64_OUT_DATA_SET_LE        (UINT64)0x8877665544332211
  #define UINT64_OUT_DATA_GET_FROM_BE   UINT64_IN_DATA
  #define UINT64_OUT_DATA_GET_FROM_LE   (UINT64)0x8877665544332211
#else
  #define UINT64_OUT_DATA_SET_BE        (UINT64)0x8877665544332211
  #define UINT64_OUT_DATA_SET_LE        UINT64_IN_DATA
  #define UINT64_OUT_DATA_GET_FROM_BE   (UINT64)0x8877665544332211
  #define UINT64_OUT_DATA_GET_FROM_LE   UINT64_IN_DATA
#endif


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
\brief    Test ami Uint16 conversion functions

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_amiUint16(void)
{
    UINT16 inData = UINT16_IN_DATA;
    UINT16 outData;

    // Test set uint16 to big endian
    ami_setUint16Be(&outData, inData);

    CU_ASSERT_EQUAL(outData, UINT16_OUT_DATA_SET_BE);

    // Test set uint16 to little endian
    ami_setUint16Le(&outData, inData);

    CU_ASSERT_EQUAL(outData, UINT16_OUT_DATA_SET_LE);

    // Test get uint16 from big endian
    outData = ami_getUint16Be(&inData);

    CU_ASSERT_EQUAL(outData, UINT16_OUT_DATA_GET_FROM_BE);

    // Test get uint16 from little endian
    outData = ami_getUint16Le(&inData);

    CU_ASSERT_EQUAL(outData, UINT16_OUT_DATA_GET_FROM_LE);
}

//------------------------------------------------------------------------------
/**
\brief    Test ami Uint32 conversion functions

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_amiUint32(void)
{
    UINT32 inData = UINT32_IN_DATA;
    UINT32 outData;

    // Test set uint32 to big endian
    ami_setUint32Be(&outData, inData);

    CU_ASSERT_EQUAL(outData, UINT32_OUT_DATA_SET_BE);

    // Test set uint32 to little endian
    ami_setUint32Le(&outData, inData);

    CU_ASSERT_EQUAL(outData, UINT32_OUT_DATA_SET_LE);

    // Test get uint32 from big endian
    outData = ami_getUint32Be(&inData);

    CU_ASSERT_EQUAL(outData, UINT32_OUT_DATA_GET_FROM_BE);

    // Test get uint32 from little endian
    outData = ami_getUint32Le(&inData);

    CU_ASSERT_EQUAL(outData, UINT32_OUT_DATA_GET_FROM_LE);
}

//------------------------------------------------------------------------------
/**
\brief    Test ami Uint64 conversion functions

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_amiUint64(void)
{
    UINT64 inData = UINT64_IN_DATA;
    UINT64 outData;

    // Test set uint64 to big endian
    ami_setUint64Be(&outData, inData);
    
    CU_ASSERT_EQUAL(outData, UINT64_OUT_DATA_SET_BE);

    // Test set uint64 to little endian
    ami_setUint64Le(&outData, inData);

    CU_ASSERT_EQUAL(outData, UINT64_OUT_DATA_SET_LE);

    // Test get uint64 from big endian
    outData = ami_getUint64Be(&inData);

    CU_ASSERT_EQUAL(outData, UINT64_OUT_DATA_GET_FROM_BE);

    // Test get uint64 from little endian
    outData = ami_getUint64Le(&inData);

    CU_ASSERT_EQUAL(outData, UINT64_OUT_DATA_GET_FROM_LE);
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

/// \}
