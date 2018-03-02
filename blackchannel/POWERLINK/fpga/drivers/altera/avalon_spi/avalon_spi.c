/**
********************************************************************************
\file   avalon_spi.c

\brief  Avalon Spi driver

This Spi driver supports duplex write/read transfers.
*******************************************************************************/

/*------------------------------------------------------------------------------
* License Agreement
*
* Copyright © 2013 B&R Industrial Automation GmbH
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
#include <avalon_spi/avalon_spi.h>

#include <apptarget/target.h>

#include <stdlib.h>
#include <alt_types.h>
#include <altera_avalon_spi_regs.h>

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
\brief  This function starts a full duplex transfer by transmitting data from
        the provided write buffer and forwarding the received data to the read
        buffer. In case of different write and read length the maximum length
        is taken for the transfer length - unneeded read data is dropped or
        write data is set to zeros.
        Note: * This function is not thread-save!
              * This function is blocking (waiting for transfer complete)!
              * This function is intended for 8 bit Spi register size!

\param  base_p          Avalon Spi base address
\param  slave_p         Spi Slave to be selected
\param  pDesc_p         Descriptor of the read and write data of the stream
\param  flag_p          Flags

\return int
\retval 0               Transfer was successfully completed
\retval 1               Parameters incorrect
\retval -1              Transfer error

*/
//------------------------------------------------------------------------------
int avalon_spi_tf(alt_u32 base_p, alt_u32 slave_p, tDescriptor* pDesc_p,
        alt_u32 flag_p)
{
    int     ret = 0;
    alt_u16 i;
    alt_u16 tfLength;
    alt_u16 writeCnt;
    alt_u16 readCnt;
    alt_u32 status;

    if(pDesc_p->pReadBase_m == NULL && pDesc_p->pWriteBase_m == NULL)
    {
        // No buffer provided at all, get maximum
        tfLength = (pDesc_p->writeSize_m > pDesc_p->readSize_m) ?
                pDesc_p->writeSize_m : pDesc_p->readSize_m;

        // And set write and read to zero
        pDesc_p->writeSize_m = pDesc_p->readSize_m = 0;
    }
    else if(pDesc_p->pWriteBase_m == NULL)
    {
        // No write buffer
        pDesc_p->writeSize_m = 0;
        tfLength = pDesc_p->readSize_m;
    }
    else if (pDesc_p->pReadBase_m == NULL)
    {
        // No read buffer
        pDesc_p->readSize_m = 0;
        tfLength = pDesc_p->writeSize_m;
    }
    else
    {
        // Get transfer length by taking maximum
        tfLength = (pDesc_p->writeSize_m > pDesc_p->readSize_m) ?
                pDesc_p->writeSize_m : pDesc_p->readSize_m;
    }

    // Read from rxdata (if wasn't done before)
    IORD_ALTERA_AVALON_SPI_RXDATA(base_p);

    // Select slave
    IOWR_ALTERA_AVALON_SPI_SLAVE_SEL(base_p, 1 << slave_p);

    // Assert ss during whole transfer
    IOWR_ALTERA_AVALON_SPI_CONTROL(base_p, ALTERA_AVALON_SPI_CONTROL_SSO_MSK);

    writeCnt = readCnt = 0;

    for(i=0; i<tfLength; i++)
    {
        alt_u32 buffer;

        // Select data to be written depending on write length
        if(writeCnt < pDesc_p->writeSize_m)
        {
            buffer = (alt_u8)pDesc_p->pWriteBase_m[writeCnt];
            writeCnt++;
        }
        else
        {
            // No more data to be written, transmit zero
            buffer = 0;
        }

        // Check if ready for tx
        do
        {
            status = IORD_ALTERA_AVALON_SPI_STATUS(base_p);
        }
        while((status & ALTERA_AVALON_SPI_STATUS_TRDY_MSK) == 0);

        // Write data to txdata register
        IOWR_ALTERA_AVALON_SPI_TXDATA(base_p, buffer);

        // Check if rxdata is ready
        do
        {
            status = IORD_ALTERA_AVALON_SPI_STATUS(base_p);
        }
        while((status & ALTERA_AVALON_SPI_STATUS_RRDY_MSK) == 0);

        // Get read data from rxdata register
        buffer = IORD_ALTERA_AVALON_SPI_RXDATA(base_p);

        if(readCnt < pDesc_p->readSize_m)
        {
            pDesc_p->pReadBase_m[readCnt] = (alt_u8)(buffer & 0xFF);
            readCnt++;
        }
    }

    // Check if caller wants ss signal stay asserted
    if((flag_p & AVALON_SPI_HOLD_SS) == 0)
    {
        // Clear control register
        IOWR_ALTERA_AVALON_SPI_CONTROL(base_p, 0);
    }

    return ret;
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{


/// \}
