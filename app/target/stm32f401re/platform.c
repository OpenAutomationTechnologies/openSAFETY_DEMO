/**
********************************************************************************
\file   target/stm32f401re/platform.c

\defgroup module_targ_stm32f401_platform Platform module
\{

\brief  Application interface target handling

Defines the platform specific functions of the slim interface example
implementation.

\ingroup group_app_targ_stm32f401
*******************************************************************************/

/*------------------------------------------------------------------------------
* License Agreement
*
* Copyright (c) 2017, Bernecker+Rainer Industrie-Elektronik Ges.m.b.H. (B&R)
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
#include <common/platform.h>

#include <apptarget/benchmark.h>

#include <stm32f4xx_hal.h>

#include <stdio.h>

/*============================================================================*/
/*            G L O B A L   D E F I N I T I O N S                             */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/
/* Definition for USARTx clock resources */
#define USARTx                           USART2
#define USARTx_CLK_ENABLE()              __USART2_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __USART2_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __USART2_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_2
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_AF                     GPIO_AF7_USART2
#define USARTx_RX_PIN                    GPIO_PIN_3
#define USARTx_RX_GPIO_PORT              GPIOA
#define USARTx_RX_AF                     GPIO_AF7_USART2

/*----------------------------------------------------------------------------*/
/* module global vars                                                         */
/*----------------------------------------------------------------------------*/
static UART_HandleTypeDef UartHandle_l;

/*----------------------------------------------------------------------------*/
/* global function prototypes                                                 */
/*----------------------------------------------------------------------------*/
static BOOL systemClockInit(void);
static BOOL uart2init(void);
static void initBenchmark(void);

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
\brief  Initialize the peripherals of the target

This function init's the peripherals of the AP like cache and the interrupt
controller.

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
BOOL platform_init(void)
{
    BOOL retVal = FALSE;

    (void)HAL_DeInit();

    if(HAL_Init() == HAL_OK)
    {
        if(systemClockInit())
        {
            /* By default stdin/stdout are on usart2 */
            if(uart2init())
            {
                /* turn off buffers, so IO occurs immediately */
                setvbuf(stdin, NULL, _IONBF, 0);
                setvbuf(stdout, NULL, _IONBF, 0);
                setvbuf(stderr, NULL, _IONBF, 0);

                /* Initialize the benchmark pins */
                initBenchmark();

                retVal = TRUE;
            }
        }
    }

    return retVal;
}

/*----------------------------------------------------------------------------*/
/**
\brief  Close all peripherals of the target
*/
/*----------------------------------------------------------------------------*/
void platform_exit(void)
{
    /* Disable USART */
    USARTx_FORCE_RESET();
    USARTx_RELEASE_RESET();

    HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
    HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);

    /* Disable benchmark */
    HAL_GPIO_DeInit(PINx_BENCHMARK_PORT, PINx_BENCHMARK_PIN0);
    HAL_GPIO_DeInit(PINx_BENCHMARK_PORT, PINx_BENCHMARK_PIN1);

    /* Close HAL library */
    HAL_DeInit();
}

/*----------------------------------------------------------------------------*/
/**
\brief  Sleep for an amount of milliseconds

\param[in] msec_p       The milliseconds to sleep
*/
/*----------------------------------------------------------------------------*/
void platform_msleep(UINT32 msec_p)
{
    HAL_Delay(msec_p);
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/*----------------------------------------------------------------------------*/
/**
  \brief  System Clock Configuration

      The system Clock is configured as follow :
         System Clock source            = PLL (HSE)
         SYSCLK(Hz)                     = 84000000
         HCLK(Hz)                       = 84000000
         AHB Prescaler                  = 1
         APB1 Prescaler                 = 2
         APB2 Prescaler                 = 1
         HSI Frequency(Hz)              = 8000000
         PLL_M                          = 4
         PLL_N                          = 168
         PLL_P                          = 4
         PLL_Q                          = 7
         VDD(V)                         = 3.3
         Main regulator output voltage  = Scale2 mode
         Flash Latency(WS)              = 2

 \return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
static BOOL systemClockInit(void)
{
    BOOL retVal = FALSE;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    /* Enable Power Control clock */
    __PWR_CLK_ENABLE();

    /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    /* Enable HSI Oscillator and activate PLL with HSI as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 168;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) == HAL_OK)
    {
        /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
        RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
        RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
        if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) == HAL_OK)
        {
            retVal = TRUE;
        }
    }

    return retVal;
}

/*----------------------------------------------------------------------------*/
/**
\brief  Initialize the USART2 to enable prints to the terminal

The USART2 is used to forward characters from printf via the stdlib to the
host PC terminal.

\return TRUE on success; FALSE on error;
*/
/*----------------------------------------------------------------------------*/
static BOOL uart2init(void)
{
    BOOL retVal = FALSE;
    GPIO_InitTypeDef  GPIO_InitStruct;

    memset(&UartHandle_l, 0, sizeof(UART_HandleTypeDef));

    /* Enable GPIO TX/RX clock */
    __GPIOA_CLK_ENABLE();

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitTypeDef));

    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin = USARTx_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = USARTx_TX_AF;
    HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitTypeDef));

    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin = USARTx_RX_PIN;
    GPIO_InitStruct.Alternate = USARTx_RX_AF;
    HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);

    /* Enable USART2 clock */
    USARTx_CLK_ENABLE();

    /* Configure USART */
    UartHandle_l.Instance = USARTx;
    UartHandle_l.Init.BaudRate = 115200;
    UartHandle_l.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle_l.Init.StopBits = UART_STOPBITS_1;
    UartHandle_l.Init.Parity = UART_PARITY_NONE;
    UartHandle_l.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UartHandle_l.Init.Mode = UART_MODE_TX_RX;
    if(HAL_UART_Init(&UartHandle_l) == HAL_OK)
    {
        retVal = TRUE;
    }

    return retVal;
}

/*----------------------------------------------------------------------------*/
/**
\brief  Initialize the benchmark pins used by benchmark.h
*/
/*----------------------------------------------------------------------------*/
static void initBenchmark(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    memset(&GPIO_InitStructure, 0, sizeof(GPIO_InitTypeDef));

    /* Enable benchmark GPIO clock */
    PINx_BENCHMARK_CLK_ENABLE();

    /* Init benchmark pins */
    GPIO_InitStructure.Pin = PINx_BENCHMARK_PIN0 | PINx_BENCHMARK_PIN1 | PINx_BENCHMARK_PIN2;
    GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(PINx_BENCHMARK_PORT, &GPIO_InitStructure);
}

/*----------------------------------------------------------------------------*/
/**
\brief  Retargets the C library printf function to the USART.

\param char_p   The char to print

\return The printed char on success; 0 otherweise
*/
/*----------------------------------------------------------------------------*/
int __io_putchar(int char_p)
{
    int ret = 0;

    if(HAL_UART_Transmit(&UartHandle_l, (UINT8 *)&char_p, 1, 0xFFFF) == HAL_OK)
    {
        ret = char_p;
    }

    return ret;
}

/*----------------------------------------------------------------------------*/
/**
\brief  Retargets the C library printf function to the USART.

\return The read data from the UART
*/
/*----------------------------------------------------------------------------*/
int __io_getchar(void)
{
    int ret = 0;
    UINT8 data = 0;

    if(HAL_UART_Receive(&UartHandle_l, &data, sizeof(data), 0xFFFF) == HAL_OK)
    {
        ret = data;
    }

    return ret;
}

/**
 * \}
 * \}
 */
