/**
********************************************************************************
\file   pdo.c

\brief  Module for the synchronous receive PDO exchange.

This module consists of the isochronous functions which is used by the receive
and transmit channels.

\ingroup module_pdo
*******************************************************************************/

/*------------------------------------------------------------------------------
* License Agreement
*
* Copyright (c) 2014, B&R Industrial Automation GmbH
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

#include <psi/pdo.h>

#include <oplk/oplk.h>

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
\brief    Link an object to the desired variable

\param[in] objIdx_p             Index of the object to link
\param[in] objSubIdx_p          Subindex of the object to link
\param[in] pTargBase_p          Base address of the target variable
\param[in] targAddrOff_p        Offset of the target variable
\param[in] objSize_p            Size of the object to link

\return tPsiStatus
\retval kPsiSuccessful                On success
\retval kPsiMainInitObjLinkFailed     Unable to link object to variable


\ingroup module_pdo
*/
//------------------------------------------------------------------------------
tPsiStatus psi_linkPdo(UINT16 objIdx_p, UINT8 objSubIdx_p, UINT8* pTargBase_p,
        UINT32 targAddrOff_p, UINT16 objSize_p)
{
    tPsiStatus ret = kPsiSuccessful;
    tOplkError   oplkret = kErrorOk;
    UINT32       objSize = objSize_p;
    UINT32       pVarEntries = 1;
    UINT8*       pTargetAddr;

    // Assemble target offset
    pTargetAddr = (UINT8 *)((UINT32)pTargBase_p + targAddrOff_p);

    if(objSize_p == 0)
    {
        // Read object size from local obdict
        oplkret = oplk_readLocalObject(objIdx_p, objSubIdx_p,
                pTargetAddr, &objSize);
        if(oplkret != kErrorOk)
        {
            ret = kPsiConfChanObjLinkFailed;
            goto Exit;
        }
    }

    oplkret = oplk_linkObject(objIdx_p, pTargetAddr, &pVarEntries, &objSize, objSubIdx_p);
    if(oplkret != kErrorOk)
    {
        ret = kPsiConfChanObjLinkFailed;
        goto Exit;
    }

Exit:
    return ret;
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

/// \}


