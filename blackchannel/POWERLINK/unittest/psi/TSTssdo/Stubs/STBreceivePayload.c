/**
********************************************************************************
\file   STBreceivePayload.c

\brief  Stub for receiving payload data over the SSDO module

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

#include <Stubs/STBreceivePayload.h>
#include <Stubs/STBdescList.h>

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
#define RX_PAYLOAD_LENGTH       20      ///< Size of the dummy rx data

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static BOOL enRxPayload_l = FALSE;
static tSeqNrValue currSeqNrValue_l = kSeqNrValueInvalid;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Enable receiving valid data

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void stb_enableReceivePayload(void)
{
    enRxPayload_l = TRUE;
}

//------------------------------------------------------------------------------
/**
\brief    Disable receiving valid data

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void stb_disableReceivePayload(void)
{
    enRxPayload_l = FALSE;
}

//------------------------------------------------------------------------------
/**
\brief    Set sequence number to value

\param seqNr_p      New value of the sequence number

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void stb_setSequenceNumber(tSeqNrValue seqNr_p)
{
    currSeqNrValue_l = seqNr_p;
}

//------------------------------------------------------------------------------
/**
\brief Receive valid payload over the stream handler

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void stb_receivePayload(void)
{
    tBuffDescriptor* pBuffDesc;
    tTbufSsdoRxStructure* pSsdoRxStruct;
    UINT8 rxPayload[RX_PAYLOAD_LENGTH];

    PSI_MEMSET(&rxPayload, 0xBB, sizeof(rxPayload));

    if(enRxPayload_l)
    {
        // Get SSDO receive buffer descriptor
        pBuffDesc = stb_getDescElement(kTbufNumSsdoReceive0);

        pSsdoRxStruct = (tTbufSsdoRxStructure*)pBuffDesc->pBuffBase_m;

        pSsdoRxStruct->seqNr_m = currSeqNrValue_l;
        PSI_MEMCPY(pSsdoRxStruct->ssdoStubDataDom_m, &rxPayload[0], sizeof(pSsdoRxStruct->ssdoStubDataDom_m));
        pSsdoRxStruct->paylSize_m = sizeof(rxPayload);
    }
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

/// \}

#endif // #if (((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)
