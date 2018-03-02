/**
********************************************************************************
\file   demo-sn-gpio/target/stm32f103rb/timer.c

\defgroup module_sn_stm32f103_timer Timer module
\{

\brief  Target specific functions of the system timer

This module implements the hardware near target specific functions of the
system timer for stm32f103rb (Cortex-M3).

\ingroup group_app_sn_targ_stm32f103
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
#include <sn/timer.h>

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
#define TIMER_PRESCALE_1US         (UINT16)71   /**< Prescaler for 1us resolution */

/* Defines for the TIMx peripheral */
#define TIMx                TIM2

#define TIMx_CLK_ENABLE()     __HAL_RCC_TIM2_CLK_ENABLE()

/*----------------------------------------------------------------------------*/
/* local types                                                                */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/
static TIM_HandleTypeDef TimerHandle_l;

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static BOOLEAN initTimer(void);

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the timer module

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
BOOLEAN timer_init(void)
{
    BOOLEAN fReturn = FALSE;

    if(initTimer())
    {
        if(HAL_TIM_Base_Start(&TimerHandle_l) == HAL_OK)
        {
            fReturn = TRUE;
        }
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Close the timer module
*/
/*----------------------------------------------------------------------------*/
void timer_close(void)
{
    /* Close TIMx */
    HAL_TIM_Base_DeInit(&TimerHandle_l);
}

/*----------------------------------------------------------------------------*/
/**
\brief    Get current system tick

This function returns the current value of the internal 16bit timer.

\return Returns the system tick in milliseconds
*/
/*----------------------------------------------------------------------------*/
UINT16 timer_getTickCount(void)
{
    UINT64 usTime = 0;

    usTime = (UINT32)__HAL_TIM_GetCounter(&TimerHandle_l);

    return usTime;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Set the current system tick to a desired value

\param[in] newVal_p     The new value for the timer
*/
/*----------------------------------------------------------------------------*/
void timer_setTickCount(UINT16 newVal_p)
{
    __HAL_TIM_SetCounter(&TimerHandle_l, newVal_p);
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/*----------------------------------------------------------------------------*/
/**
\brief  Initialize the timer core
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN initTimer(void)
{
    BOOLEAN fReturn = FALSE;

    TIMx_CLK_ENABLE();

    TimerHandle_l.Instance = TIMx;
    TimerHandle_l.Init.Period = 0xFFFF;
    TimerHandle_l.Init.Prescaler = TIMER_PRESCALE_1US;
    TimerHandle_l.Init.ClockDivision = 0;
    TimerHandle_l.Init.CounterMode = TIM_COUNTERMODE_UP;
    if(HAL_TIM_Base_Init(&TimerHandle_l) == HAL_OK)
    {
        fReturn = TRUE;
    }

    return fReturn;
}

/**
 * \}
 * \}
 */
