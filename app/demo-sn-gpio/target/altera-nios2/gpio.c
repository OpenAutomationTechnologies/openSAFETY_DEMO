/**
********************************************************************************
\file   demo-sn-gpio/target/altera-nios2/gpio.c

\defgroup module_sn_nios2_gpio GPIO pin's module
\{

\brief  Target specific functions of the system gpio pins

This module implements the hardware near target specific functions of the
system gpio pins for Altera Nios2 on TERASIC INK.

\ingroup group_app_sn_targ_nios2
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
#include <sn/gpio.h>

#include "altera_avalon_pio_regs.h"
#include <system.h>

/*============================================================================*/
/*            G L O B A L   D E F I N I T I O N S                             */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/
#define STATUS_BASE_ADDRESS   STATUS_PIO_BASE   /**< Base address of the gpio module */
#define STATUS_LED_BIT        1                 /**< Id of the first led of the target */

#define TARGET_MAX_LEDS       2                 /**< Maximum of LEDS on this target */

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

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the gpio module

\return 0 on success; 1 on error
*/
/*----------------------------------------------------------------------------*/
void gpio_init(void)
{
    /* No initialization needed to write to gpio pins */
}

/*----------------------------------------------------------------------------*/
/**
\brief    Close the gpio module
*/
/*----------------------------------------------------------------------------*/
void gpio_close(void)
{
    /* Nothing to close for this target */
}

/*----------------------------------------------------------------------------*/
/**
\brief    Set the gpio pins according the the state of the connection valid bit

\param[in] spdoId_p       The id of the SPDO the connection valid should be set
\param[in] isSet_p        TRUE if the connection valid is set
*/
/*----------------------------------------------------------------------------*/
void gpio_changeConValid(UINT16 spdoId_p, UINT8 isSet_p)
{
    if(spdoId_p < TARGET_MAX_LEDS)
    {
        if(isSet_p != 0)
            IOWR_ALTERA_AVALON_PIO_SET_BITS(STATUS_BASE_ADDRESS, STATUS_LED_BIT + spdoId_p);
        else
            IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(STATUS_BASE_ADDRESS, STATUS_LED_BIT + spdoId_p);
    }
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/**
 * \}
 * \}
 */
