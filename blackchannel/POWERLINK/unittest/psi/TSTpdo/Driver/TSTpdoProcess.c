/**
********************************************************************************
\file   TSTpdoProcess.c

\brief  Test pdo module init and processing with correct stream init

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

#include <Driver/TSTpdoConfig.h>
#include <Stubs/STBdescList.h>
#include <Stubs/STBinitStream.h>
#include <Stubs/STBdummyHandler.h>

#include <libpsi/internal/pdo.h>
#include <libpsi/internal/stream.h>

#if (((PSI_MODULE_INTEGRATION) & (PSI_MODULE_PDO)) != 0)

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
\brief    Init stream with valid parameters

\return int
\retval 0       Init successful
\retval other   Init failed

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
int TST_validInit(void)
{
    BOOL fReturn;

    stb_initBuffers();

    // Call init successfully
    fReturn = stb_initStreamModule();

    return (fReturn != FALSE) ? 0 : 1;
}

//------------------------------------------------------------------------------
/**
\brief Test pdo module initialization function

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_pdoInit(void)
{
    BOOL fReturn;
    tPdoInitParam InitParam;

    PSI_MEMSET(&InitParam, 0 , sizeof(tPdoInitParam));

    // Init with no parameters
    fReturn = pdo_init(NULL, NULL);

    CU_ASSERT_FALSE( fReturn );

    // Init with no init parameter structure
    fReturn = pdo_init(stb_dummyPdoCbSuccess, NULL);

    CU_ASSERT_FALSE( fReturn );

    // Init with unset init parameter structure (rpdoid = tpdoid)
    fReturn = pdo_init(stb_dummyPdoCbSuccess, &InitParam);

    CU_ASSERT_FALSE( fReturn );

    // Init with rpdo and tpdo invalid
    InitParam.buffIdRpdo_m = kTbufCount;
    InitParam.buffIdTpdo_m = kTbufCount;

    fReturn = pdo_init(stb_dummyPdoCbSuccess, &InitParam);

    CU_ASSERT_FALSE( fReturn );

    // Call dummy exit function
    pdo_exit();
}

//------------------------------------------------------------------------------
/**
\brief Test pdo module initialization function

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_pdoProcessBoth(void)
{
    BOOL fReturn;
    tPdoInitParam InitParam;

    // Init pdo module successfully
    InitParam.buffIdRpdo_m = kTbufNumRpdoImage;
    InitParam.buffIdTpdo_m = kTbufNumTpdoImage;

    fReturn = pdo_init(stb_dummyPdoCbSuccess, &InitParam);

    CU_ASSERT_TRUE( fReturn );

    fReturn = stream_processSync();

    CU_ASSERT_TRUE( fReturn );

    // Init PDO module with failing PDO callback
    fReturn = pdo_init(stb_dummyPdoCbFail, &InitParam);

    CU_ASSERT_TRUE( fReturn );

    fReturn = stream_processPostActions();

    CU_ASSERT_FALSE( fReturn );
}

//------------------------------------------------------------------------------
/**
\brief Test pdo module initialization with RPDO only

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_pdoProcessRpdoOnly(void)
{
    BOOL fReturn;
    tPdoInitParam InitParam;

    // Init with rpdo id init parameter set
    InitParam.buffIdRpdo_m = kTbufNumRpdoImage;
    InitParam.buffIdTpdo_m = kTbufCount;

    fReturn = pdo_init(stb_dummyPdoCbSuccess, &InitParam);

    CU_ASSERT_TRUE( fReturn );

    fReturn = stream_processSync();

    CU_ASSERT_TRUE( fReturn );

    // Init PDO module with failing PDO callback
    fReturn = pdo_init(stb_dummyPdoCbFail, &InitParam);

    CU_ASSERT_TRUE( fReturn );

    fReturn = stream_processPostActions();

    CU_ASSERT_FALSE( fReturn );
}

//------------------------------------------------------------------------------
/**
\brief Test pdo module initialization with TPDO only

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_pdoProcessTpdoOnly(void)
{
    BOOL fReturn;
    tPdoInitParam InitParam;

    // Init with tpdo id init parameter set
    InitParam.buffIdRpdo_m = kTbufCount;
    InitParam.buffIdTpdo_m = kTbufNumTpdoImage;

    fReturn = pdo_init(stb_dummyPdoCbSuccess, &InitParam);

    CU_ASSERT_TRUE( fReturn );

    fReturn = stream_processSync();

    CU_ASSERT_TRUE( fReturn );

    // Init PDO module with failing PDO callback
    fReturn = pdo_init(stb_dummyPdoCbFail, &InitParam);

    CU_ASSERT_TRUE( fReturn );

    fReturn = stream_processPostActions();

    CU_ASSERT_FALSE( fReturn );
}


//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

/// \}

#endif // #if (((PSI_MODULE_INTEGRATION) & (PSI_MODULE_PDO)) != 0)
