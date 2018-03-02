/**
********************************************************************************
\file   target/altera-nios2/app-gpio.c

\defgroup module_targ_nios2_app GPIO application module
\{

\brief  Implements a GPIO application for target Altera Nios2

This application simply reads inputs and outputs from common GPIO pins and
forwards it to the user application.

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
#include <common/app-gpio.h>

#include "altera_avalon_pio_regs.h"
#include <system.h>

/*============================================================================*/
/*            G L O B A L   D E F I N I T I O N S                             */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/
#ifdef LEDR_PIO_BASE
    #define OUTPORT_AP_BASE_ADDRESS LEDR_PIO_BASE
#endif

#ifdef KEY_PIO_BASE
    #define INPORT_AP_BASE_ADDRESS KEY_PIO_BASE
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

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief  Initialize the GPIO application

\retval 0       Init successful
\retval 1       Error on init
*/
/*----------------------------------------------------------------------------*/
UINT8 appgpio_init(void)
{
    return 0;
}

/*----------------------------------------------------------------------------*/
/**
\brief  Cleanup the GPIO application
*/
/*----------------------------------------------------------------------------*/
void appgpio_exit(void)
{
    /* Nothing to cleanup */
}


/*----------------------------------------------------------------------------*/
/**
\brief  Write a value to the output port

This function writes a value to the output port of the AP

\param[in] value_p       the value to write
*/
/*----------------------------------------------------------------------------*/
void appgpio_writeOutputPort(UINT32 value_p)
{
#ifdef OUTPORT_AP_BASE_ADDRESS
    IOWR_ALTERA_AVALON_PIO_DATA(OUTPORT_AP_BASE_ADDRESS, (UINT16)value_p);
#endif
}

/*----------------------------------------------------------------------------*/
/**
\brief  Read a value from the input port

This function reads a value from the input port of the AP

\return  UINT32
\retval  value              the value of the input port
*/
/*----------------------------------------------------------------------------*/
UINT8 appgpio_readInputPort(void)
{
    UINT8 val = 0;

#ifdef INPORT_AP_BASE_ADDRESS
    val = IORD_ALTERA_AVALON_PIO_DATA(INPORT_AP_BASE_ADDRESS);
#endif

    return val;
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

