/**
********************************************************************************
\file   psi/tbuf.h

\brief  Header file for the triple buffer interface module

This file contains definitions for the triple buffer module.

*******************************************************************************/

/*------------------------------------------------------------------------------
Copyright (c) 2013, B&R Industrial Automation GmbH
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holders nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------------------*/

#ifndef _INC_psi_tbuf_H_
#define _INC_psi_tbuf_H_

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <psi/pcpglobal.h>

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------

/**
 * \brief Initialization parameters of the tbuf module
 */
typedef struct {
    UINT8                 id_m;             ///< Id of the triple buffer
    UINT8*                pBase_m;          ///< Base address of the triple buffer
    UINT8*                pAckBase_m;       ///< Producer acknowledge register base
    UINT32                size_m;           ///< Size of the triple buffer
} tTbufInitStruct;

typedef struct eTbufInstance    *tTbufInstance;

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
tPsiStatus tbuf_init(void);
tTbufInstance tbuf_create(tTbufInitStruct* pInitParam_p);
void tbuf_destroy(tTbufInstance pInstance_p);

// Function for buffer acknowledging
tPsiStatus tbuf_setAck(tTbufInstance pInstance_p);

// Functions for read and write to the buffers
tPsiStatus tbuf_writeByte(tTbufInstance pInstance_p, UINT32 targetOffset_p,
        UINT8 data_p);
tPsiStatus tbuf_readByte(tTbufInstance pInstance_p, UINT32 targetOffset_p,
        UINT8* pData_p);
tPsiStatus tbuf_writeWord(tTbufInstance pInstance_p, UINT32 targetOffset_p,
        UINT16 data_p);
tPsiStatus tbuf_readWord(tTbufInstance pInstance_p, UINT32 targetOffset_p,
        UINT16* pData_p);
tPsiStatus tbuf_writeDword(tTbufInstance pInstance_p, UINT32 targetOffset_p,
        UINT32 data_p);
tPsiStatus tbuf_readDword(tTbufInstance pInstance_p, UINT32 targetOffset_p,
        UINT32* pData_p);
tPsiStatus tbuf_writeStream(tTbufInstance pInstance_p, UINT32 targetOffset_p,
        const void* pWriteData_p, UINT32 length_p);
tPsiStatus tbuf_readStream(tTbufInstance pInstance_p, UINT32 targetOffset_p,
        void* pReadData_p, UINT32 length_p);

// Get pointer to data buffer
tPsiStatus tbuf_getDataPtr(tTbufInstance pInstance_p, UINT32 targetOffset_p,
        UINT8** ppDataPtr_p );

#endif /* _INC_psi_tbuf_H_ */
