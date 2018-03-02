/**
********************************************************************************
\file   tbuf.c

\brief  Interface module to the triple buffers

This module enables easy access to the triple buffers in the tripleBuffer ipcore.
It is instantiated for every triple buffer allocated.

\ingroup module_tbuff
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
#include <psi/internal/tbuf.h>
#include <psi/tbuf.h>
#include <libpsicommon/ami.h>

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

static struct  eTbufInstance       tbufInstance_l[TRIPLE_BUFFER_COUNT];

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------


//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Initialize the triple buffer module

\return tPsiStatus
\retval kPsiSuccessful    On success

\ingroup module_tbuff
*/
//------------------------------------------------------------------------------
tPsiStatus tbuf_init(void)
{
    tPsiStatus ret = kPsiSuccessful;

    PSI_MEMSET(&tbufInstance_l, 0 , sizeof(struct eTbufInstance) * TRIPLE_BUFFER_COUNT);

    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Create a triple buffer instance

Instantiate a new triple buffer instance which provides basic access to the
triple buffer functions.

\param[in]  pInitParam_p          Triple buffer initialization structure

\return tTbuffInstance
\retval address              Pointer to the instance of the tbuf buffer
\retval Null                 Unable to allocate instance

\ingroup module_tbuff
*/
//------------------------------------------------------------------------------
tTbufInstance tbuf_create(tTbufInitStruct* pInitParam_p)
{
    tTbufInstance     pInstance = NULL;

#if _DEBUG
    if(pInitParam_p->id_m > (TRIPLE_BUFFER_COUNT - 1))
    {
        goto Exit;
    }

    if(pInitParam_p->pBase_m == NULL)
    {
        goto Exit;
    }

    // Check base and size alignment
    if(UNALIGNED32(pInitParam_p->pBase_m) && UNALIGNED32(pInitParam_p->size_m))
    {
        goto Exit;
    }
#endif

    // Rember triple buffer id
    tbufInstance_l[pInitParam_p->id_m].id_m = pInitParam_p->id_m;

    // set base address to instance
    tbufInstance_l[pInitParam_p->id_m].pBaseAddr_m = pInitParam_p->pBase_m;

    // set size of buffer
    tbufInstance_l[pInitParam_p->id_m].size_m = pInitParam_p->size_m;

    // set acknowledge register address
    tbufInstance_l[pInitParam_p->id_m].pAckBaseAddr_m = pInitParam_p->pAckBase_m;

    // Set valid instance id
    pInstance = &tbufInstance_l[pInitParam_p->id_m];

#if _DEBUG
Exit:
#endif
    return pInstance;
}

//------------------------------------------------------------------------------
/**
\brief    Destroy a triple buffer instance

Destroy a new triple buffer instance which provides basic access to the
triple buffer functions.

\param  pInstance_p           Pointer to the instance

\ingroup module_tbuff
*/
//------------------------------------------------------------------------------
void tbuf_destroy(tTbufInstance pInstance_p)
{
    PSI_MEMSET(pInstance_p, 0 , sizeof(struct  eTbufInstance));

    // free allocated memory (if needed)!

}

//------------------------------------------------------------------------------
/**
\brief    Write the acknowledge register

\param[in] pInstance_p           Pointer to the instance

\return tPsiStatus
\retval kPsiSuccessful      On success of writing
\retval kPsiTbuffWriteError Error on writing

\ingroup module_tbuff
*/
//------------------------------------------------------------------------------
tPsiStatus tbuf_setAck(tTbufInstance pInstance_p)
{
    tPsiStatus ret = kPsiSuccessful;
    UINT32       ackData = (1 << (pInstance_p->id_m - 1));

    ami_setUint32Le((UINT8* )pInstance_p->pAckBaseAddr_m, ackData);

    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Write a byte to the buffer

\param[in] pInstance_p           Pointer to the instance
\param[in] targetOffset_p        Offset of the target to be written to
\param[in] data_p                The data to be written to the register

\return tPsiStatus
\retval kPsiSuccessful      On success of writing
\retval kPsiTbuffWriteError Error on writing

\ingroup module_tbuff
*/
//------------------------------------------------------------------------------
tPsiStatus tbuf_writeByte(tTbufInstance pInstance_p, UINT32 targetOffset_p,
        UINT8 data_p)
{
    tPsiStatus ret = kPsiSuccessful;

    ami_setUint8Le((UINT8* )((UINT32)pInstance_p->pBaseAddr_m + targetOffset_p),
                data_p);

    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Read a byte from the buffer

\param[in]  pInstance_p           Pointer to the instance
\param[in]  targetOffset_p        Offset of the target to be read from
\param[out] pData_p               The read byte

\return tPsiStatus
\retval kPsiSuccessful      On success on reading
\retval kPsiTbuffReadError  Error on reading

\ingroup module_tbuff
*/
//------------------------------------------------------------------------------
tPsiStatus tbuf_readByte(tTbufInstance pInstance_p, UINT32 targetOffset_p,
        UINT8* pData_p)
{
    tPsiStatus ret = kPsiSuccessful;

    if(pData_p == NULL)
    {
        ret = kPsiTbuffReadError;
    }
    else
    {
        *pData_p = ami_getUint8Le((UINT8 *)((UINT32)pInstance_p->pBaseAddr_m +
                targetOffset_p));
    }

    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Write a word to the buffer

\param[in]  pInstance_p           Pointer to the instance
\param[in]  targetOffset_p        Offset of the target to be written to
\param[in]  data_p                The data to be written to the register

\return tPsiStatus
\retval kPsiSuccessful      On success of writing
\retval kPsiTbuffWriteError Error on writing

\ingroup module_tbuff
*/
//------------------------------------------------------------------------------
tPsiStatus tbuf_writeWord(tTbufInstance pInstance_p, UINT32 targetOffset_p,
        UINT16 data_p)
{
    tPsiStatus ret = kPsiSuccessful;

    ami_setUint16Le((UINT8* )((UINT32)pInstance_p->pBaseAddr_m + targetOffset_p),
                data_p);

    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Read a word from the buffer

\param[in]  pInstance_p           Pointer to the instance
\param[in]  targetOffset_p        Offset of the target to be read from
\param[out] pData_p               The read data

\return tPsiStatus
\retval kPsiSuccessful      On success on reading
\retval kPsiTbuffReadError  Error on reading

\ingroup module_tbuff
*/
//------------------------------------------------------------------------------
tPsiStatus tbuf_readWord(tTbufInstance pInstance_p, UINT32 targetOffset_p,
        UINT16* pData_p)
{
    tPsiStatus ret = kPsiSuccessful;

    if(pData_p == NULL)
    {
        ret = kPsiTbuffReadError;
    }
    else
    {
        *pData_p = ami_getUint16Le((UINT8 *)((UINT32)pInstance_p->pBaseAddr_m +
                targetOffset_p));
    }

    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Write a double word to the buffer

\param[in]  pInstance_p           Pointer to the instance
\param[in]  targetOffset_p        Offset of the target to be written to
\param[in]  data_p                The data to be written to the register

\return tPsiStatus
\retval kPsiSuccessful      On success of writing
\retval kPsiTbuffWriteError Error on writing

\ingroup module_tbuff
*/
//------------------------------------------------------------------------------
tPsiStatus tbuf_writeDword(tTbufInstance pInstance_p, UINT32 targetOffset_p,
        UINT32 data_p)
{
    tPsiStatus ret = kPsiSuccessful;

    ami_setUint32Le((UINT8* )((UINT32)pInstance_p->pBaseAddr_m + targetOffset_p),
                data_p);

    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Read a double word from the buffer

\param[in]  pInstance_p           Pointer to the instance
\param[in]  targetOffset_p        Offset of the target to be read from
\param[out]  pData_p               The read data

\return tPsiStatus
\retval kPsiSuccessful      On success on reading
\retval kPsiTbuffReadError  Error on reading

\ingroup module_tbuff
*/
//------------------------------------------------------------------------------
tPsiStatus tbuf_readDword(tTbufInstance pInstance_p, UINT32 targetOffset_p,
        UINT32* pData_p)
{
    tPsiStatus ret = kPsiSuccessful;

    if(pData_p == NULL)
    {
        ret = kPsiTbuffReadError;
    }
    else
    {
        *pData_p = ami_getUint32Le((UINT8 *)((UINT32)pInstance_p->pBaseAddr_m +
                targetOffset_p));
    }

    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Write a stream to the buffer

\param[in] pInstance_p           Pointer to the instance
\param[in] targetOffset_p        Offset of the target to be written to
\param[in] pWriteData_p          Base address of the stream to write
\param[in] length_p              Length of the stream

\return tPsiStatus
\retval kPsiSuccessful      On success on reading
\retval kPsiTbuffWriteError Error on writing

\ingroup module_tbuff
*/
//------------------------------------------------------------------------------
tPsiStatus tbuf_writeStream(tTbufInstance pInstance_p, UINT32 targetOffset_p,
        const void* pWriteData_p, UINT32 length_p)
{
    tPsiStatus ret = kPsiSuccessful;

    if(pWriteData_p == NULL || length_p == 0)
    {
        ret = kPsiTbuffWriteError;
    }
    else
    {
        PSI_MEMCPY((UINT8 *)((UINT32)pInstance_p->pBaseAddr_m + targetOffset_p),
                pWriteData_p, length_p);
    }

    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Read a stream from the buffer

\param[in]  pInstance_p           Pointer to the instance
\param[in]  targetOffset_p        Offset of the target to be read from
\param[out] pReadData_p           Base address of the stream to read
\param[in]  length_p              Length of the stream

\return tPsiStatus
\retval kPsiSuccessful      On success on reading
\retval kPsiTbuffReadError  Error on reading

\ingroup module_tbuff
*/
//------------------------------------------------------------------------------
tPsiStatus tbuf_readStream(tTbufInstance pInstance_p, UINT32 targetOffset_p,
        void* pReadData_p, UINT32 length_p)
{
    tPsiStatus ret = kPsiSuccessful;

    if(pReadData_p == NULL || length_p == 0)
    {
        ret = kPsiTbuffReadError;
    }
    else
    {
        PSI_MEMCPY(pReadData_p, (UINT8 *)((UINT32)pInstance_p->pBaseAddr_m + targetOffset_p),
                length_p);
    }

    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Read a stream from the buffer

\param[in]  pInstance_p           Pointer to the instance
\param[in]  targetOffset_p        Offset of the target to be read from
\param[out] ppDataPtr_p           Base address of the stream to read

\return kPsiSuccessful

\ingroup module_tbuff
*/
//------------------------------------------------------------------------------
tPsiStatus tbuf_getDataPtr(tTbufInstance pInstance_p, UINT32 targetOffset_p,
        UINT8** ppDataPtr_p )
{
    tPsiStatus ret = kPsiSuccessful;

    *ppDataPtr_p = (UINT8 *)((UINT32)pInstance_p->pBaseAddr_m + targetOffset_p);

    return ret;
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{


/// \}


