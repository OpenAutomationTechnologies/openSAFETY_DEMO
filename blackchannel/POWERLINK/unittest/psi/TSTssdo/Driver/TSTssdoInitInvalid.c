/**
********************************************************************************
\file   TSTssdoInitInvalid.c

\brief  SSDO module test cases for invalid initialization

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
#include <Stubs/STBinitTimeout.h>
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

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Init with invalid receive buffer address

\return int
\retval 0       Init successful
\retval other   Init failed

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
int TST_initSsdoRxAddrInvalid(void)
{
    BOOL fReturn;

    stb_initDescriptorsChangeSelected(kTbufNumSsdoReceive0, NULL, sizeof(tTbufSsdoRxStructure));

    fReturn = stb_initStreamModule();

    return (fReturn != FALSE) ? 0 : 1;
}

//------------------------------------------------------------------------------
/**
\brief    Init with invalid transmit buffer address

\return int
\retval 0       Init successful
\retval other   Init failed

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
int TST_initSsdoTxAddrInvalid(void)
{
    BOOL fReturn;

    stb_initDescriptorsChangeSelected(kTbufNumSsdoTransmit0, NULL, sizeof(tTbufSsdoTxStructure));

    fReturn = stb_initStreamModule();

    return (fReturn != FALSE) ? 0 : 1;
}

//------------------------------------------------------------------------------
/**
\brief    Init with invalid receive buffer size

\return int
\retval 0       Init successful
\retval other   Init failed

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
int TST_initSsdoRxSizeInvalid(void)
{
    BOOL fReturn;

    tTbufSsdoRxStructure buffInStruct;

    stb_initDescriptorsChangeSelected(kTbufNumSsdoReceive0, (UINT8*)&buffInStruct, 0);

    fReturn = stb_initStreamModule();

    return (fReturn != FALSE) ? 0 : 1;
}

//------------------------------------------------------------------------------
/**
\brief    Init with invalid transmit buffer size

\return int
\retval 0       Init successful
\retval other   Init failed

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
int TST_initSsdoTxSizeInvalid(void)
{
    BOOL fReturn;

    tTbufSsdoTxStructure buffOutStruct;

    stb_initDescriptorsChangeSelected(kTbufNumSsdoTransmit0, (UINT8*)&buffOutStruct, 0);

    fReturn = stb_initStreamModule();

    return (fReturn != FALSE) ? 0 : 1;
}

//------------------------------------------------------------------------------
/**
\brief    Init with receove buffer full post action list

\return int
\retval 0       Init successful
\retval other   Init failed

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
int TST_initRxPostActionListFull(void)
{
    BOOL fReturn;
    tTbufSsdoRxStructure buffStruct;

    stb_initDescriptorsChangeSelected(kTbufNumSsdoReceive0, (UINT8*)&buffStruct, sizeof(buffStruct));

    fReturn = stb_initStreamModule();

    if(fReturn != FALSE)
    {
        fReturn = stb_initAllActions(kTbufNumSsdoReceive0, kStreamActionPost, kTbufCount);
    }

    return (fReturn != FALSE) ? 0 : 1;
}


//------------------------------------------------------------------------------
/**
\brief    Init transmit buffer with no timeout instance available

\return int
\retval 0       Init successful
\retval other   Init failed

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
int TST_initTxTimeoutInitFails(void)
{
    BOOL fReturn;
    tTbufSsdoTxStructure buffStruct;

    stb_initDescriptorsChangeSelected(kTbufNumSsdoTransmit0, (UINT8*)&buffStruct, sizeof(buffStruct));

    fReturn = stb_initStreamModule();

    if(fReturn != FALSE)
    {
        fReturn = stb_initAllTimeouts();
    }

    return (fReturn != FALSE) ? 0 : 1;
}

//------------------------------------------------------------------------------
/**
\brief Test SSDO module initialization with invalid initialized stream module

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_ssdoInitFail(void)
{
    tSsdoInitParam InitParam;
    tSsdoInstance pSsdoInst;

    PSI_MEMSET(&InitParam, 0, sizeof(tSsdoInitParam));

    ssdo_init();

    // Test initialization successful
    InitParam.buffIdRx_m = kTbufNumSsdoReceive0;
    InitParam.buffIdTx_m = kTbufNumSsdoTransmit0;
    InitParam.pfnRxHandler_m = stb_dummySsdoReceiveHandlerSuccess;

    pSsdoInst = ssdo_create(kNumSsdoChan0, &InitParam);

    CU_ASSERT_EQUAL( pSsdoInst, NULL );

    ssdo_destroy(pSsdoInst);

    // If initialized kill all timeout instances
    stb_destroyAllTimeouts();

    ssdo_destroy(pSsdoInst);
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

/// \}

#endif // #if (((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)
