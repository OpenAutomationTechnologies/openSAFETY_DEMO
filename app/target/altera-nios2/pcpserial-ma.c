/**
********************************************************************************
\file   target/altera-nios2/pcpserial-ma.c

\defgroup module_targ_nios2_serial_ma PCP serial module (Master Mode)
\{

\brief  Implements the driver for the serial device in master mode

Defines the platform specific functions for the serial to interconnect the app
with the POWERLINK processor. (Target is Altera Nios2)

\ingroup group_app_targ_nios2
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

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/
#include <common/pcpserial.h>

#include <system.h>
#include <string.h>
#include <stdio.h>

#include <alt_types.h>
#include <altera_avalon_pio_regs.h>

/* Include custom SPI driver */
#include <avalon_spi/avalon_spi.h>

/*============================================================================*/
/*            G L O B A L   D E F I N I T I O N S                             */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

/* SPI master */
#ifdef SPI_MASTER_BASE
  #define SPI_MASTER_BASE_ADDRESS SPI_MASTER_BASE
#endif

/*----------------------------------------------------------------------------*/
/* module global vars                                                         */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* global function prototypes                                                 */
/*----------------------------------------------------------------------------*/


/*============================================================================*/
/*            P R I V A T E   D E F I N I T I O N S                           */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* local types                                                                */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/
static tPcpSerialTransferFin pfnTransfFin_l = NULL;

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief  Initialize the PCP serial

This function init's the serial device which connects the application processor
with the POWERLINK processor. The transfer can either be carried out in
slave or master mode.

\param pTransParam_p    The transfer parameters (rx/tx base and size)
\param pfnTransfFin_p   Pointer to the transfer finished interrupt

\retval TRUE    On success
\retval FALSE   Error during initialization
*/
/*----------------------------------------------------------------------------*/
BOOL pcpserial_init(tHandlerParam * pTransParam_p, tPcpSerialTransferFin pfnTransfFin_p)
{
    UNUSED_PARAMETER(pTransParam_p);

    /* No initialization needed for Nios2 (Done in ipcore configuration!) */
    pfnTransfFin_l = pfnTransfFin_p;

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/**
\brief  Close the serial device
*/
/*----------------------------------------------------------------------------*/
void pcpserial_exit(void)
{
    pfnTransfFin_l = NULL;
}

/*----------------------------------------------------------------------------*/
/**
\brief  Start an serial transfer

pcpserial_transfer() starts an serial transfer to exchange the process
image with the PCP.

\param[in] pHandlParam_p       The parameters of the serial transfer handler

\retval TRUE        On success
\retval FALSE       Error on sending or receiving
*/
/*----------------------------------------------------------------------------*/
BOOL pcpserial_transfer(tHandlerParam* pHandlParam_p)
{
    BOOL fReturn = FALSE;
    int spiRet;
    tDescriptor   initDesc = { NULL, 4, NULL, 4 };

    /* Perform four byte initial SPI access */
    spiRet = avalon_spi_tf(
            SPI_MASTER_BASE_ADDRESS,
            0,
            &initDesc,
            AVALON_SPI_HOLD_SS
            );
    if(spiRet == 0)
    {
        /* Transfer the actual input and output image */
        spiRet = avalon_spi_tf(
                SPI_MASTER_BASE_ADDRESS,
                0,
                (tDescriptor *)&pHandlParam_p->consDesc_m,
                0
                );
        if(spiRet == 0)
        {
            fReturn = TRUE;
        }
    }

    if(fReturn != FALSE)
    {
        /* Call the transfer finished callback function */
        if(pfnTransfFin_l != NULL)
            pfnTransfFin_l(FALSE);
    }

    return fReturn;
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions  */
/** \{ */


/**
 * \}
 * \}
 */

