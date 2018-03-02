/**
********************************************************************************
\file   STBinitTimeout.c

\brief  Stub that initializes the timeout module

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
#include <Stubs/STBinitTimeout.h>

#include <libpsicommon/internal/timeout.h>

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
#define DEFAULT_CYCLE_TIME  100     ///< Defailt timeout time

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static tTimeoutInstance timeoutInstance_l[TIMEOUT_MAX_INSTANCES];

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Initialize all available timeout instances

\return BOOL
\retval TRUE        All timeout instances successfully initialized
\retval FALSE       Error while initializing the timeout module

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
BOOL stb_initAllTimeouts(void)
{
    BOOL fError = FALSE;
    UINT8 i;

    timeout_init();

    for(i=0; i < TIMEOUT_MAX_INSTANCES; i++)
    {
        timeoutInstance_l[i] = timeout_create(DEFAULT_CYCLE_TIME);
        if(timeoutInstance_l[i] == NULL)
        {
            fError = TRUE;
            break;
        }
    }

    return ((fError != FALSE) ? FALSE : TRUE);
}

//------------------------------------------------------------------------------
/**
\brief    Destroy all previous allocated timeout instances

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void stb_destroyAllTimeouts(void)
{
    UINT8 i;

    for(i=0; i < TIMEOUT_MAX_INSTANCES; i++)
    {
        timeout_destroy(timeoutInstance_l[i]);
    }
}

