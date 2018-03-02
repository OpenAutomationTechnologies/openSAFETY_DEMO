/**
********************************************************************************
\file   STBdescList.c

\brief  Stub that generates a list of descriptors need by the library

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
#define TBUF_IMAGE_SIZE     ( TBUF_OFFSET_PROACK + TBUF_SIZE_PROACK )   ///< Size of the triple buffer image

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

typedef struct {
    UINT8              tbufMemLayout[TBUF_IMAGE_SIZE];
    tBuffDescriptor    buffDescList[kTbufCount];
} tDescList;


//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static tDescList descListInstance_l;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Initialize an image of the transfer buffers

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void stb_initBuffers(void)
{
    UINT8 i;
    tBuffDescriptor* pBuffDec = &descListInstance_l.buffDescList[0];
    tTbufDescriptor tbufDescList[kTbufCount] = TBUF_INIT_VEC;

    // Set image of the buffers to zero
    PSI_MEMSET( &descListInstance_l, 0, sizeof(tDescList) );

    // Generate a descriptor list which can be used in the library
    for(i=0; i < kTbufCount; i++, pBuffDec++)
    {
        pBuffDec->pBuffBase_m = (UINT8 *)((UINT32)&descListInstance_l.tbufMemLayout
            + (UINT32)tbufDescList[i].buffOffset_m);
        pBuffDec->buffSize_m = tbufDescList[i].buffSize_m;
    }
}

//------------------------------------------------------------------------------
/**
\brief    Get a list of buffer descriptors for the usage inside the library

\return tBuffDescriptor*
\retval Address             Address of the buffer descriptor list

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
tBuffDescriptor* stb_getDescList(void)
{
    tBuffDescriptor* pBuffDec = &descListInstance_l.buffDescList[0];

    return pBuffDec;
}

//------------------------------------------------------------------------------
/**
\brief    Initialize all buffers and change one buffer to the given parameters.

If the id of the buffer to modify is invalid no buffer will be changed and a valid
initialization is done.

\param[in] id_p             Id of the buffer to init
\param[in] pBaseAddr_p      Base address of the buffer
\param[in] size_p           Size of the buffer

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void stb_initDescriptorsChangeSelected(tTbufNumLayout id_p, UINT8* pBaseAddr_p, UINT16 size_p)
{
    tBuffDescriptor* pBuffDescElement;

    stb_initBuffers();

    if(id_p < kTbufCount)
    {
        pBuffDescElement = stb_getDescElement(id_p);

        pBuffDescElement->pBuffBase_m = pBaseAddr_p;
        pBuffDescElement->buffSize_m = size_p;
    }
}

//------------------------------------------------------------------------------
/**
\brief    Get one element of the descriptor list

\param id_p     Id of the element to get

\return tBuffDescriptor*
\retval Address             Address of the buffer descriptor list

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
tBuffDescriptor* stb_getDescElement(tTbufNumLayout id_p)
{
    tBuffDescriptor* pBuffDec = &descListInstance_l.buffDescList[id_p];

    return pBuffDec;
}


//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

/// \}
