/**
********************************************************************************
\file   TSTssdoInternal.c

\brief  Test Ssdo module specific parts of the internal module

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
#include <Stubs/STBdummyHandler.h>
#include <Stubs/STBreceivePayload.h>

#include <libpsi/psi.h>
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
static BOOL streamHandlerRxPayload(tHandlerParam* pHandlParam_p);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief Initialize system with internal module

\return int
\retval 0       Init successful
\retval other   Init failed

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
int TST_initInternal(void)
{
    BOOL fReturn;
    tPsiInitParam psiInit;

    PSI_MEMSET(&psiInit, 0, sizeof(tPsiInitParam));

    // Initialize internal module successfully
    stb_initBuffers();

    // Call init of internal module
    psiInit.idConsAck_m = kTbufAckRegisterCons;
    psiInit.idProdAck_m = kTbufAckRegisterProd;
    psiInit.pBuffDescList_m = stb_getDescList();
    psiInit.pfnStreamHandler_m = streamHandlerRxPayload;
    psiInit.idFirstProdBuffer_m = (tTbufNumLayout)(TBUF_NUM_CON + 1);

    fReturn = psi_init(&psiInit);

    return (fReturn != FALSE) ? 0 : 1;
}

//------------------------------------------------------------------------------
/**
\brief Test SSDO process function of the internal module

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_internalProcess(void)
{
    BOOL fReturn;
    tSsdoInitParam ssdoInitParam;
    tSsdoInstance pSsdoInst;

    // Call create of SSDO module
    ssdoInitParam.buffIdRx_m = kTbufNumSsdoReceive0;
    ssdoInitParam.buffIdTx_m = kTbufNumSsdoTransmit0;
    ssdoInitParam.pfnRxHandler_m = stb_dummySsdoReceiveHandlerSuccess;

    pSsdoInst = ssdo_create(kNumSsdoChan0, &ssdoInitParam);

    fReturn = psi_processAsync();

    CU_ASSERT_TRUE( fReturn );

    // Destroy created async instance
    ssdo_destroy(pSsdoInst);
}

//------------------------------------------------------------------------------
/**
\brief Test SSDO process function with failing receive handler

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_internalProcessRxHandlerFail(void)
{
    BOOL fReturn;
    tSsdoInitParam ssdoInitParam;
    tSsdoInstance pSsdoInst;

    // Call create of SSDO module
    ssdoInitParam.buffIdRx_m = kTbufNumSsdoReceive0;
    ssdoInitParam.buffIdTx_m = kTbufNumSsdoTransmit0;
    ssdoInitParam.pfnRxHandler_m = stb_dummySsdoReceiveHandlerFail;

    pSsdoInst = ssdo_create(kNumSsdoChan0, &ssdoInitParam);

    stb_enableReceivePayload();
    stb_setSequenceNumber(kSeqNrValueSecond);

    fReturn = stream_processSync();

    CU_ASSERT_TRUE_FATAL( fReturn );

    fReturn = psi_processAsync();

    CU_ASSERT_FALSE( fReturn );
}



//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

static BOOL streamHandlerRxPayload(tHandlerParam* pHandlParam_p)
{
    stb_receivePayload();

    return TRUE;
}

/// \}

#endif // #if (((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)