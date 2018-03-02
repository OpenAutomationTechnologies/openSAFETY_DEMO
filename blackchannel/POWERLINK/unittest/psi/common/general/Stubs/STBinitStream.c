/**
********************************************************************************
\file   STBinitStream.c

\brief  Stub that successfully initializes the stream module and its actions

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
#include <Stubs/STBinitStream.h>
#include <Stubs/STBdummyHandler.h>
#include <Stubs/STBdescList.h>

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
\brief    Initialize the stream module with the descriptor list

\return BOOL
\retval TRUE        Success on init of stream module
\retval FALSE       Failed to init stream module

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
BOOL stb_initStreamModule(void)
{
    BOOL fReturn;
    tStreamInitParam InitParam;

        // Setup stream module init parameters
    InitParam.pfnStreamHandler_m = stb_streamHandlerSuccess;
    InitParam.pBuffDescList_m = stb_getDescList();
    InitParam.idConsAck_m = (tTbufNumLayout)0;
    InitParam.idFirstProdBuffer_m = (tTbufNumLayout)(TBUF_NUM_CON + 1);

    fReturn = stream_init(&InitParam);

    return fReturn;
}

//------------------------------------------------------------------------------
/**
\brief    Initialize a defined count of buffer actions

\param[in] buffId_p          Id of the buffer to register the action
\param[in] actionType_p      Type of the action to register
\param[in] actCount_p        Number of actions to register

\return BOOL
\retval TRUE        On success
\retval FALSE       Unable to register all actions

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
BOOL stb_initAllActions(tTbufNumLayout buffId_p, tActionType actionType_p, UINT8 actCount_p)
{
    BOOL fReturn = FALSE;
    UINT8 i;

    for(i=0; i < actCount_p; i++)
    {
        fReturn = stream_registerAction(actionType_p, buffId_p, stb_dummyActionSuccess, NULL);
        if(fReturn == FALSE)
            break;
    }

    return fReturn;
}
