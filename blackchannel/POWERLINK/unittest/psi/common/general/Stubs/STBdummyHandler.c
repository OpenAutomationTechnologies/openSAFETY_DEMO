/**
********************************************************************************
\file   STBdummyHandler.c

\brief  Dummy handler functions of the stream tests

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
#include <Stubs/STBdummyHandler.h>

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
\brief    Dummy successful sync callback

\return Always success

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
BOOL stb_dummySyncCbSuccess(void)
{
    return TRUE;
}

//------------------------------------------------------------------------------
/**
\brief    Dummy failing sync callback

\return Always fail

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
BOOL stb_dummySyncCbFail(void)
{
    return FALSE;
}

//------------------------------------------------------------------------------
/**
\brief    Dummy sync handler callback success

\return Always success

\ingroup module_unittests
*/
//------------------------------------------------------------------------------

BOOL stb_dummySyncHandlerSuccess(tPsiTimeStamp* pTimeStamp_p)
{
    UNUSED_PARAMETER(pTimeStamp_p);

    return TRUE;
}

//------------------------------------------------------------------------------
/**
\brief    Dummy sync handler callback fail

\return Always fail

\ingroup module_unittests
*/
//------------------------------------------------------------------------------

BOOL stb_dummySyncHandlerFail(tPsiTimeStamp* pTimeStamp_p)
{
    UNUSED_PARAMETER(pTimeStamp_p);

    return FALSE;
}

//------------------------------------------------------------------------------
/**
\brief    Dummy stream handler function

\param pHandlParam_p    Stream handler parameter

\return Always success

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
BOOL stb_streamHandlerSuccess(tHandlerParam* pHandlParam_p)
{
    UNUSED_PARAMETER(pHandlParam_p);

    return TRUE;
}

//------------------------------------------------------------------------------
/**
\brief    Failing dummy stream handler function

\param pHandlParam_p    Stream handler parameter

\return Always fail

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
BOOL stb_streamHandlerFail(tHandlerParam* pHandlParam_p)
{
    UNUSED_PARAMETER(pHandlParam_p);

    return FALSE;
}

//------------------------------------------------------------------------------
/**
\brief    Dummy buffer action success

\return Always succeed

\param pBuffer_p        Address of the buffer
\param bufSize_p        Size of the buffer
\param pUserArg_p       User argument

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
BOOL stb_dummyActionSuccess (UINT8* pBuffer_p, UINT16 bufSize_p, void * pUserArg_p)
{
    UNUSED_PARAMETER(pBuffer_p);
    UNUSED_PARAMETER(bufSize_p);
    UNUSED_PARAMETER(pUserArg_p);

    return TRUE;
}

//------------------------------------------------------------------------------
/**
\brief    Dummy buffer action failing

\return Always fail

\param pBuffer_p        Address of the buffer
\param bufSize_p        Size of the buffer
\param pUserArg_p       User argument

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
BOOL stb_dummyActionFail (UINT8* pBuffer_p, UINT16 bufSize_p, void * pUserArg_p)
{
    UNUSED_PARAMETER(pBuffer_p);
    UNUSED_PARAMETER(bufSize_p);
    UNUSED_PARAMETER(pUserArg_p);

    return FALSE;
}

//------------------------------------------------------------------------------
/**
\brief    Dummy successful PDO processing callback

\param rpdoRelTimeLow_p    Relative time low dword
\param pRpdoImage_p        Pointer to the RPDO image
\param pTpdoImage_p        Pointer to the TPDO image

\return Always success

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
BOOL stb_dummyPdoCbSuccess ( UINT32 rpdoRelTimeLow_p, tRpdoMappedObj* pRpdoImage_p,
                             tTpdoMappedObj* pTpdoImage_p )
{
    UNUSED_PARAMETER(rpdoRelTimeLow_p);
    UNUSED_PARAMETER(pRpdoImage_p);
    UNUSED_PARAMETER(pTpdoImage_p);

    return TRUE;
}

//------------------------------------------------------------------------------
/**
\brief    Dummy failing PDO processing callback

\param rpdoRelTimeLow_p    Relative time low dword
\param pRpdoImage_p        Pointer to the RPDO image
\param pTpdoImage_p        Pointer to the TPDO image

\return Always fail

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
BOOL stb_dummyPdoCbFail ( UINT32 rpdoRelTimeLow_p, tRpdoMappedObj* pRpdoImage_p,
                          tTpdoMappedObj* pTpdoImage_p )
{
    UNUSED_PARAMETER(rpdoRelTimeLow_p);
    UNUSED_PARAMETER(pRpdoImage_p);
    UNUSED_PARAMETER(pTpdoImage_p);

    return FALSE;
}

//------------------------------------------------------------------------------
/**
\brief    Enter critical section dummy function

\param fEnable_p        Unused parameter

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void stb_dummyCriticalSection(UINT8 fEnable_p)
{
    UNUSED_PARAMETER(fEnable_p);
}

//------------------------------------------------------------------------------
/**
\brief    Dummy Ssdo receive handler success

\param pPayload_p        Pointer to the received payload
\param size_p            Size of the received payload

\return BOOL
\retval TRUE     Always success

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
BOOL stb_dummySsdoReceiveHandlerSuccess (UINT8* pPayload_p, UINT16 size_p)
{
    UNUSED_PARAMETER(pPayload_p);
    UNUSED_PARAMETER(size_p);

    return TRUE;
}

//------------------------------------------------------------------------------
/**
\brief    Dummy Ssdo receive handler

\param pPayload_p        Pointer to the received payload
\param size_p            Size of the received payload

\return BOOL
\retval FALSE    Always fail

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
BOOL stb_dummySsdoReceiveHandlerFail (UINT8* pPayload_p, UINT16 size_p)
{
    UNUSED_PARAMETER(pPayload_p);
    UNUSED_PARAMETER(size_p);

    return FALSE;
}