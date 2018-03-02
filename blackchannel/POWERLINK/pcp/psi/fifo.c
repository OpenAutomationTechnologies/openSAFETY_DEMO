/**
********************************************************************************
\file   fifo.c

\brief  Simple generic FIFO implementation

Implements a FIFO to store data which is forward it to the reading task.
This FIFO is implemented thread safe.

\ingroup module_fifo
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
#include <psi/internal/fifo.h>
#include <psi/fifo.h>

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
#define FIFO_MAX_INSTANCES      4           ///< Maximum number of FIFO instances

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

/**
 * \brief Buffer header of one element
 */
typedef struct {
    UINT32  size_m;     ///< Size of one element
} tElemHeader;

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static struct eFifoInstance fifoInstance_l[FIFO_MAX_INSTANCES];

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------


//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Initialize the FIFO module

\ingroup module_fifo
*/
//------------------------------------------------------------------------------
tPsiStatus fifo_init(void)
{
    tPsiStatus ret = kPsiSuccessful;

    PSI_MEMSET(&fifoInstance_l, 0, sizeof(struct eFifoInstance) * FIFO_MAX_INSTANCES);

    // Check alignment of element header
    if((sizeof(tElemHeader) & 3) != 0)
    {
        ret = kPsiFifoAlignError;
    }

    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Create a FIFO instance

\param[in] elemSize_p       Size of one element
\param[in] elemCount_p      Maximum number of elements in the FIFO

\return tFifoInstance
\retval Pointer        Pointer to the FIFO instance
\retval NULL           Unable to allocate FIFO instance

\ingroup module_fifo
*/
//------------------------------------------------------------------------------
tFifoInstance fifo_create(UINT32 elemSize_p, UINT8 elemCount_p)
{
    tFifoInstance     pInstance = NULL;
    UINT32            elemSizeAlign = (elemSize_p + 3U) & 0xFFFFFFFCU;
    UINT32            lastElemOffset;

    UINT8  id = 0xFF, i;

    // Search for free instance
    for(i=0; i < FIFO_MAX_INSTANCES; i++)
    {
        if(fifoInstance_l[i].fInstUsed_m == FALSE)
        {
            // Free instance found!
            id = i;
            break;
        }
    }

    if(id == 0xFF)
    {
        // No free instance found
        goto Exit;
    }

    lastElemOffset = ((elemSizeAlign + sizeof(tElemHeader)) * (elemCount_p - 1));

    // Allocate FIFO buffer
    fifoInstance_l[id].pFifoBuffer_m = PSI_MALLOC((elemSizeAlign + sizeof(tElemHeader)) * elemCount_p);

    // Init read and write position pointer
    fifoInstance_l[id].pReadPos_m = fifoInstance_l[id].pFifoBuffer_m;
    fifoInstance_l[id].pWritePos_m = fifoInstance_l[id].pFifoBuffer_m;

    // Remember element size and count
    fifoInstance_l[id].elemSize_m = elemSizeAlign + sizeof(tElemHeader);
    fifoInstance_l[id].maxElem_m = elemCount_p;

    // Set pointer to fifo tail
    fifoInstance_l[id].pFifoTail_m = (UINT8*)((UINT32)fifoInstance_l[id].pFifoBuffer_m + lastElemOffset);

    // Create pointer to instance
    fifoInstance_l[id].fInstUsed_m = TRUE;
    pInstance = &fifoInstance_l[id];

Exit:
    return pInstance;
}

//------------------------------------------------------------------------------
/**
\brief    Destroy a FIFO instance

\param[in] pInstance       Pointer to FIFO instance

\ingroup module_fifo
*/
//------------------------------------------------------------------------------
void fifo_destroy(tFifoInstance pInstance)
{
    // Free FIFO buffer
    PSI_FREE(pInstance->pFifoBuffer_m);

    // Reset instance structure
    PSI_MEMSET(pInstance, 0, sizeof(struct eFifoInstance));
}

//------------------------------------------------------------------------------
/**
\brief    Insert an element to the FIFO

\param[in] pInstance       Pointer to FIFO instance
\param[in] pElement_p      Pointer to the element to post
\param[in] elemSize_p      Size of the element to post

\return tPsiStatus
\retval kPsiSuccessful    On success
\retval kPsiFifoFull      FIFO is full

\ingroup module_fifo
*/
//------------------------------------------------------------------------------
tPsiStatus fifo_insertElement(tFifoInstance pInstance, tFifoElement pElement_p,
        UINT32 elemSize_p)
{
    tPsiStatus ret = kPsiSuccessful;
    UINT8        numElement;
    tElemHeader  elementHeader;
    UINT8*       pWriteHeader;

    if(elemSize_p > (pInstance->elemSize_m - sizeof(tElemHeader)))
    {
        ret = kPsiFifoElementSizeOverflow;
        goto Exit;
    }

    numElement = pInstance->numWriteElem_m - pInstance->numReadElem_m;

    // Check if FIFO is full
    if(numElement > pInstance->maxElem_m)
    {
        ret = kPsiFifoFull;
        goto Exit;
    }

    elementHeader.size_m = elemSize_p;

    // Write header to FIFO
    PSI_MEMCPY(pInstance->pWritePos_m, &elementHeader, sizeof(tElemHeader));

    // Move write position header
    pWriteHeader = pInstance->pWritePos_m + sizeof(tElemHeader);

    // Post element to FIFO
    PSI_MEMCPY(pWriteHeader, pElement_p, pInstance->elemSize_m - sizeof(tElemHeader));

    // Increment write position
    pInstance->numWriteElem_m++;

    // Check overflow of write pointer
    if( pInstance->pWritePos_m >= pInstance->pFifoTail_m)
    {
        pInstance->pWritePos_m = pInstance->pFifoBuffer_m;
    }
    else
    {
        pInstance->pWritePos_m = (UINT8 *)((UINT32)pInstance->pWritePos_m +
                    pInstance->elemSize_m);
    }

Exit:
    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Get an element from the FIFO

\param[in]  pInstance       Pointer to FIFO instance
\param[in]  pElement_p      Pointer to the element to post
\param[out] pElemSize_p     Size of the returned element

\return tPsiStatus
\retval kPsiSuccessful    On success
\retval kPsiFifoEmpty     FIFO is empty

\ingroup module_fifo
*/
//------------------------------------------------------------------------------
tPsiStatus fifo_getElement(tFifoInstance pInstance, tFifoElement pElement_p,
        UINT32* pElemSize_p)
{
    tPsiStatus ret = kPsiSuccessful;
    UINT8        numElement;
    UINT8*       pReadHeader;

    numElement = pInstance->numWriteElem_m - pInstance->numReadElem_m;

    if(pElement_p == NULL)
    {
        ret = kPsiFifoInvalidParam;
        goto Exit;
    }

    if(numElement == 0)
    {
        ret = kPsiFifoEmpty;
        goto Exit;
    }

    // Get size of element
    PSI_MEMCPY(pElemSize_p, pInstance->pReadPos_m, sizeof(tElemHeader));

    // Move read position header
    pReadHeader = pInstance->pReadPos_m + sizeof(tElemHeader);

    // Get element from FIFO
    PSI_MEMCPY(pElement_p, pReadHeader, pInstance->elemSize_m - sizeof(tElemHeader));

    // Increment read position
    pInstance->numReadElem_m++;

    // Check overflow of read pointer
    if(pInstance->pReadPos_m >= pInstance->pFifoTail_m)
    {
        pInstance->pReadPos_m = pInstance->pFifoBuffer_m;
    }
    else
    {
        pInstance->pReadPos_m = (UINT8 *)((UINT32)pInstance->pReadPos_m +
                pInstance->elemSize_m);
    }

Exit:
    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Flush the FIFO

\param[in]  pInstance       Pointer to FIFO instance

\return tPsiStatus
\retval kPsiSuccessful    On success

\ingroup module_fifo
*/
//------------------------------------------------------------------------------
tPsiStatus fifo_flush(tFifoInstance pInstance)
{
    tPsiStatus ret = kPsiSuccessful;

    // Reset FIFO buffer
    pInstance->numReadElem_m = 0;
    pInstance->numWriteElem_m = 0;
    pInstance->pReadPos_m = pInstance->pFifoBuffer_m;
    pInstance->pWritePos_m = pInstance->pFifoBuffer_m;

    return ret;
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{


/// \}


