/**
********************************************************************************
\file   target/stm32f103rb/app-gpio.c

\defgroup module_targ_stm32f103_app GPIO application module
\{

\brief  Implements a GPIO application for target stm32f103 (Cortex-M3)

This application simply reads inputs and outputs from common GPIO pins and
forwards it to the user application.

\ingroup group_app_targ_stm32f103
*******************************************************************************/

/*------------------------------------------------------------------------------
* License Agreement
*
* Copyright (c) 2017, B&R Industrial Automation GmbH
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
#include <stm32f1xx_hal.h>
/*============================================================================*/
/*            G L O B A L   D E F I N I T I O N S                             */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

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
/**
 * \name STM32F103RB specific defines
 * These defines are purely target specific and are closely related
 * to the functions in which they are used, so it is considered better to
 * place them inside the app-gpio.c source file than in an additional header
 * file.
 * They describe port, pin and control registers of the STM32F103RB
 * microcontroller.
 * For information on the pinout see \ref sect_gs_nucleoboard_setup,
 * http://www.st.com/web/catalog/tools/FM116/SC959/SS1532/LN1847/PF259875
 * the STM32 NUCLEO-F103RB webpage
 * or the STM32F103RB webpage
 * http://www.st.com/web/catalog/mmc/FM141/SC1169/SS1031/LN1565/PF164487
 */
/* @{*/
#define GPIO_PORT           GPIOC                       /**<  Port C */
#define GPIO_INPUT_PINS     (GPIO_PIN_6 | GPIO_PIN_8)   /**< MorphoConnector CN10: Pin 2 (PC8), Pin 4 (PC6) */
#define GPIO_OUTPUT_PINS    (GPIO_PIN_2 | GPIO_PIN_3)   /**< MorphoConnector CN7: Pin 35 (PC2), Pin 37 (PC3) */
#define GPIO_CLK_ENABLE()   __HAL_RCC_GPIOC_CLK_ENABLE()  /**< HAL driver macro to enable clock */
#define GPIO_CLK_DISABLE()  __HAL_RCC_GPIOC_CLK_DISABLE() /**< HAL driver macro to disable clock */
#define GPIO_CLK_IS_SET     __HAL_RCC_GPIOC_IS_CLK_ENABLED()    /**< HAL driver macro to check for enabled clock */
/* @}*/
/*----------------------------------------------------------------------------*/
/* local types                                                                */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/
static BOOL gpioClockWasActivated_l = 0;
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
    GPIO_InitTypeDef inputInitSettings;
    GPIO_InitTypeDef outputInitSettings;

    /* enable clock for GPIO port, if it isn't activated */
    if (!GPIO_CLK_IS_SET)
    {
        GPIO_CLK_ENABLE();
    }
    else
    {
        gpioClockWasActivated_l = 1;
    }

    inputInitSettings.Pin = GPIO_INPUT_PINS;
    inputInitSettings.Mode = GPIO_MODE_INPUT;
    inputInitSettings.Pull = GPIO_PULLDOWN;
    inputInitSettings.Speed = GPIO_SPEED_LOW;   /* Ignored when pin is configured as input */

    outputInitSettings.Pin = GPIO_OUTPUT_PINS;
    outputInitSettings.Mode = GPIO_MODE_OUTPUT_PP;
    outputInitSettings.Pull = GPIO_NOPULL;
    outputInitSettings.Speed = GPIO_SPEED_LOW;

    HAL_GPIO_Init (GPIO_PORT, & inputInitSettings);
    HAL_GPIO_Init (GPIO_PORT, & outputInitSettings);

    return 0;
}

/*----------------------------------------------------------------------------*/
/**
\brief  Cleanup the GPIO application
*/
/*----------------------------------------------------------------------------*/
void appgpio_exit(void)
{
    HAL_GPIO_DeInit(GPIO_PORT, GPIO_INPUT_PINS | GPIO_OUTPUT_PINS);

    /* disable clock for port, if it got activated in appgpio_init() */
    if (gpioClockWasActivated_l == 0)
    {
        GPIO_CLK_DISABLE();
    }
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
    UINT16 outputPin = 0x01;
    GPIO_PinState pinState = GPIO_PIN_RESET;

    /* process value for output ports */
    if (GPIO_OUTPUT_PINS != 0)
    {
        while (outputPin)
        {
            if (outputPin & GPIO_OUTPUT_PINS)
            {
                pinState = (value_p & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET;
                HAL_GPIO_WritePin(GPIO_PORT, outputPin, pinState);
                value_p = value_p >> 1;
            }

            outputPin = (outputPin << 1)  & GPIO_PIN_MASK;
        }
    }
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
    UINT8 valPosition = 0x01;
    UINT16 inputPin = 0x01;
    GPIO_PinState pinState = GPIO_PIN_RESET;

    /* process value from intput ports */
    if (GPIO_INPUT_PINS != 0)
    {
        while (inputPin)
        {
            if (inputPin & GPIO_INPUT_PINS)
            {
                pinState = HAL_GPIO_ReadPin(GPIO_PORT, inputPin);

                if (pinState == GPIO_PIN_SET)
                {
                    val = val | valPosition;
                }

                valPosition = valPosition << 1;
            }
            inputPin = (inputPin << 1)  & GPIO_PIN_MASK;
        }
    }

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

