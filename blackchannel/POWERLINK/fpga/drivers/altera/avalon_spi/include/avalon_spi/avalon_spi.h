/**
********************************************************************************
\file   avalon_spi.h

\brief  Header Avalon Spi Driver

TODO: do it!

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

#ifndef _INC_AVALON_SPI_H_
#define _INC_AVALON_SPI_H_

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <alt_types.h>

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define AVALON_SPI_HOLD_SS      1

//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
/**
 * \brief SPI input and output descriptor
 */
typedef struct {
    alt_u8*     pReadBase_m;      ///< Base address of the buffer for the data to be read
    alt_u16     readSize_m;       ///< Size of the read data
    alt_u8*     pWriteBase_m;     ///< Base address of the buffer for the outgoing write data
    alt_u16     writeSize_m;      ///< Size of the write data
} tDescriptor;

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
int avalon_spi_tf(alt_u32 base_p, alt_u32 slave_p, tDescriptor* pDesc_p,
        alt_u32 flag_p);

#endif /* _INC_AVALON_SPI_H_ */

