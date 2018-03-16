/**
********************************************************************************
\file   demo-sn-gpio/target/stm32f401rb/upserial.c

\defgroup module_sn_stm32f401_upserial Cross communication serial module
\{

\brief  Implements the driver for the uP interconnect serial device

Defines the platform specific functions for the serial to interconnect the
uP-Master with the uP-Slave. (Target is the stm32f401re board)

\ingroup group_app_sn_targ_stm32f401
*******************************************************************************/

/*------------------------------------------------------------------------------
* License Agreement
*
* Copyright (c) 2018, B&R Industrial Automation GmbH
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
#include <sn/upserial.h>

#include <common/platform.h>

#include <stm32f4xx_hal.h>

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
#define USARTx                           USART1
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
#define DMAx_CLK_ENABLE()                __HAL_RCC_DMA2_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_CLK_DISABLE()             __HAL_RCC_USART1_CLK_DISABLE()

/* Definition for USARTx Pins */
#define USARTx_RX_PIN                    GPIO_PIN_10
#define USARTx_TX_PIN                    GPIO_PIN_9

#define USARTx_GPIO_PORT                 GPIOA
#define USARTx_ALT_FUNC                  GPIO_AF7_USART1

/* Definition for USARTx's DMA */
#define USARTx_TX_DMA_CHANNEL            DMA_CHANNEL_4
#define USARTx_TX_DMA_STREAM             DMA2_Stream7
#define USARTx_RX_DMA_CHANNEL            DMA_CHANNEL_4
#define USARTx_RX_DMA_STREAM             DMA2_Stream2

/* Definition for USARTx's NVIC */
#define USARTx_DMA_TX_IRQn               DMA2_Stream7_IRQn
#define USARTx_DMA_RX_IRQn               DMA2_Stream2_IRQn
#define USARTx_DMA_TX_IRQHandler         DMA2_Stream7_IRQHandler
#define USARTx_DMA_RX_IRQHandler         DMA2_Stream2_IRQHandler

#define USARTx_IRQ_Handler               USART1_IRQHandler
#define USARTx_IRQn                      USART1_IRQn

/*----------------------------------------------------------------------------*/
/* local types                                                                */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/
static tUpSerialTransferFin pfnTransfFin_l = NULL;      /**< This callback is called when a transfer is finished */
static tUpSerialReceiveFin pfnReceiveFin_l = NULL;      /**< This callback is called on reception of a new frame */
static tUpSerialTransferError pfnTransfError_l = NULL;  /**< This callback is called on serial error */

static UART_HandleTypeDef UsartHandle_l;     /**< USART handle structure */
static DMA_HandleTypeDef DmaRxHandle_l;      /**< DMA receive handle structure */
static DMA_HandleTypeDef DmaTxHandle_l;      /**< DMA transmit handle structure */

static BOOLEAN fTxFinished_l = FALSE;       /**< Transfer finished flag for blocking mode */
static BOOLEAN fRxFinished_l = FALSE;       /**< Receive finished flag for blocking mode */

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static void initGpio(void);
static BOOLEAN initUsart(void);
static BOOLEAN initDma(UART_HandleTypeDef* pUsartHandler_p);
static void initNvic(void);

static BOOLEAN waitForTransferFinished(BOOLEAN * pTransFin_p, UINT32 timeoutMs_p);
static void disableFifoErrorIr(void);

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief  Initialize the up serial device

This function initializes the serial device connecting the uP-Master with
the uP-Slave.

\retval TRUE    On success
\retval FALSE   USART initialization failed
*/
/*----------------------------------------------------------------------------*/
BOOLEAN upserial_init(void)
{
    UINT8 fReturn = FALSE;

    if(initUsart())
    {
        initNvic();

        fReturn = TRUE;
    }

    return fReturn;
}


/*----------------------------------------------------------------------------*/
/**
\brief  Close the USART serial
*/
/*----------------------------------------------------------------------------*/
void upserial_exit(void)
{
    pfnTransfFin_l =  NULL;
    pfnReceiveFin_l = NULL;
    pfnTransfError_l = NULL;

    /* Close the USART peripherial */
    HAL_UART_DeInit(&UsartHandle_l);
}

/*----------------------------------------------------------------------------*/
/**
\brief  Register serial callback functions

\param pfnTransfFin_p     Pointer to the transfer finished callback
\param pfnReceivefFin_p   Pointer to the receive finished callback
\param pfnTransfError_p   Pointer to the transfer error callback
*/
/*----------------------------------------------------------------------------*/
void upserial_registerCb(tUpSerialTransferFin pfnTransfFin_p,
                         tUpSerialReceiveFin pfnReceivefFin_p,
                         tUpSerialTransferError pfnTransfError_p)
{
    pfnTransfFin_l = pfnTransfFin_p;
    pfnReceiveFin_l = pfnReceivefFin_p;
    pfnTransfError_l = pfnTransfError_p;
}

/*----------------------------------------------------------------------------*/
/**
\brief  Deregister serial callback functions
*/
/*----------------------------------------------------------------------------*/
void upserial_deRegisterCb(void)
{
    pfnTransfFin_l = NULL;
    pfnReceiveFin_l = NULL;
    pfnTransfError_l = NULL;
}

/*----------------------------------------------------------------------------*/
/**
\brief  Perform an USART transfer

This function starts an USART transfer to send data to the second processor.

\note This function is blocking

\param[in] pData_p       Pointer to the transmit data
\param[in] size_p        Size of the transmit data

\retval TRUE        On success
\retval FALSE       USART send failed
*/
/*----------------------------------------------------------------------------*/
BOOLEAN upserial_transmitBlock(volatile UINT8 * pData_p, UINT32 size_p)
{
    BOOLEAN fReturn = FALSE;

    if(pData_p != NULL && size_p > 0)
    {
        /* Perform a blocking UART transfer */
        if(HAL_UART_Transmit_DMA(&UsartHandle_l, (UINT8*)pData_p, size_p) == HAL_OK)
        {
            /* Wait until DMA transfer is finished */
            if(waitForTransferFinished(&fTxFinished_l, 0xFFFF))
            {
                /* Frame successfully transmitted */
                fReturn = TRUE;
            }
            else
            {
                /* Timeout occurred */
                (void)HAL_UART_DMAStop(&UsartHandle_l);
            }

            fTxFinished_l = FALSE;
        }
    }

    return fReturn;
}


/*----------------------------------------------------------------------------*/
/**
\brief  Perform an USART reception

This function blocks until the desired amount of bytes is received from the uart
serial.

\note This function is blocking

\param[in] pData_p       Pointer to the received data
\param[in] size_p        Size of the received data
\param[in] timeoutMs_p   The timeout in ms

\retval TRUE        On success
\retval FALSE       USART receive failed
*/
/*----------------------------------------------------------------------------*/
BOOLEAN upserial_receiveBlock(volatile UINT8 * pData_p, UINT32 size_p, UINT32 timeoutMs_p)
{
    BOOLEAN fReturn = FALSE;

    if(pData_p != NULL && size_p > 0)
    {
        /* Perform a blocking UART receive */
        if(HAL_UART_Receive_DMA(&UsartHandle_l, (UINT8*)pData_p, size_p) == HAL_OK)
        {
            /* Wait until DMA transfer is finished */
            if(waitForTransferFinished(&fRxFinished_l, timeoutMs_p))
            {
                /* Frame successfully received */
                fReturn = TRUE;
            }
            else
            {
                /* Timeout occurred */
                (void)HAL_UART_DMAStop(&UsartHandle_l);
            }

            fRxFinished_l = FALSE;
        }
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief  Enable a DMA reception

This function enables a DMA reception via the local uP-Master <-> uP-Slave
serial device.

\param[in] pData_p       Pointer to the received data
\param[in] size_p        Size of the received data

\retval TRUE        On success
\retval FALSE       USART receive failed
*/
/*----------------------------------------------------------------------------*/
BOOLEAN upserial_enableReceive(volatile UINT8 * pData_p, UINT32 size_p)
{
    BOOLEAN fReturn = FALSE;

    if(pData_p != NULL && size_p > 0)
    {
        /* Perform a DMA transfer */
        if(HAL_UART_Receive_DMA(&UsartHandle_l, (UINT8*)pData_p, size_p) == HAL_OK)
        {
            disableFifoErrorIr();

            fReturn = TRUE;
        }
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief  Start an USART transfer

This function starts an USART DMA transfer to send data to the second
processor.

\param[in] pData_p       Pointer to the transmit data
\param[in] size_p        Size of the transmit data

\retval TRUE        On success
\retval FALSE       USART send failed
*/
/*----------------------------------------------------------------------------*/
BOOLEAN upserial_transmit(volatile UINT8 * pData_p, UINT32 size_p)
{
    BOOLEAN fReturn = FALSE;

    if(pData_p != NULL && size_p > 0)
    {
        /* Perform a DMA transfer */
        if(HAL_UART_Transmit_DMA(&UsartHandle_l, (UINT8*)pData_p, size_p) == HAL_OK)
        {
            disableFifoErrorIr();

            fReturn = TRUE;
        }
    }

    return fReturn;
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/*----------------------------------------------------------------------------*/
/**
\brief  Initialize the peripherals needed by the USART core

This function is called in the call of HAL_UART_Init and initializes all
peripherals needed to carry out the transfer with DMA.

\note
The function call __HAL_UART_ENABLE_IT() has been added for HAL 1.4.0
(STM32Cube_FW_F4_V1.4.0) support.

\param pUsartHandler_p    Pointer to the USART handler
*/
/*----------------------------------------------------------------------------*/
void HAL_UART_MspInit(UART_HandleTypeDef* pUsartHandler_p)
{
    if(pUsartHandler_p->Instance == USARTx)
    {
        USARTx_CLK_ENABLE();

        /* Initialize all needed peripherals */
        initGpio();
        initDma(pUsartHandler_p);
        __HAL_UART_ENABLE_IT(pUsartHandler_p, UART_IT_TC);
    }
}

/*----------------------------------------------------------------------------*/
/**
\brief  Close the peripherals needed by the USART core

This function is called in the call of HAL_UART_DeInit and closes all
peripherals needed by the USART core.

\param pUsartHandler_p    Pointer to the USART handler
*/
/*----------------------------------------------------------------------------*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* pUsartHandler_p)
{
    if(pUsartHandler_p->Instance == USARTx)
    {
        USARTx_CLK_DISABLE();

        /* Disable the DMA Streams */
        HAL_DMA_DeInit(pUsartHandler_p->hdmarx);
        HAL_DMA_DeInit(pUsartHandler_p->hdmatx);

        /* Disable peripherals and GPIO Clocks */
        HAL_GPIO_DeInit(USARTx_GPIO_PORT, USARTx_RX_PIN);
    }
}

/*----------------------------------------------------------------------------*/
/**
\brief  Initialize the SPI GPIO pins

Enable SCK and MOSI as outputs and MISO as input.
*/
/*----------------------------------------------------------------------------*/
static void initGpio(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = USARTx_RX_PIN | USARTx_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = USARTx_ALT_FUNC;
    HAL_GPIO_Init(USARTx_GPIO_PORT, &GPIO_InitStruct);
}

/*----------------------------------------------------------------------------*/
/**
\brief  Initialize the USART core

Setup USART core for receive and transmit

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN initUsart(void)
{
    BOOLEAN fReturn = FALSE;

    memset(&UsartHandle_l, 0, sizeof(UART_HandleTypeDef));

    /* Configure the USART core */
    UsartHandle_l.Instance = USARTx;
    UsartHandle_l.Init.BaudRate = 10500000;
    UsartHandle_l.Init.WordLength = UART_WORDLENGTH_8B;
    UsartHandle_l.Init.StopBits = UART_STOPBITS_1;
    UsartHandle_l.Init.Parity = UART_PARITY_NONE;
    UsartHandle_l.Init.Mode = UART_MODE_TX_RX;
    UsartHandle_l.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UsartHandle_l.Init.OverSampling = UART_OVERSAMPLING_8;
    if(HAL_UART_Init(&UsartHandle_l) == HAL_OK)
    {
        fReturn = TRUE;
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief  Initialize USART DMA for receive and transmit

Setup the DMA for receive from USART to RAM and back!

\param pUsartHandler_p    Pointer to the USART handler
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN initDma(UART_HandleTypeDef* pUsartHandler_p)
{
    BOOLEAN fReturn = FALSE;

    memset(&DmaRxHandle_l, 0, sizeof(DMA_HandleTypeDef));
    memset(&DmaTxHandle_l, 0, sizeof(DMA_HandleTypeDef));

    DMAx_CLK_ENABLE();

    /* Configure DMAx - Receive channel (USART -> memory) */
    DmaRxHandle_l.Init.Channel = USARTx_RX_DMA_CHANNEL;
    DmaRxHandle_l.Init.Direction = DMA_PERIPH_TO_MEMORY;          /* P2M transfer mode                 */
    DmaRxHandle_l.Init.PeriphInc = DMA_PINC_DISABLE;              /* Peripheral increment mode Disable */
    DmaRxHandle_l.Init.MemInc = DMA_MINC_ENABLE;                  /* Memory increment mode Enable      */
    DmaRxHandle_l.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; /* Peripheral data alignment : Byte  */
    DmaRxHandle_l.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;    /* memory data alignment : Byte      */
    DmaRxHandle_l.Init.Mode = DMA_NORMAL;                         /* Normal DMA mode                   */
    DmaRxHandle_l.Init.Priority = DMA_PRIORITY_HIGH;              /* priority level : high             */
    DmaRxHandle_l.Init.FIFOMode = DMA_FIFOMODE_DISABLE;           /* FIFO mode disabled                */
    DmaRxHandle_l.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    DmaRxHandle_l.Init.MemBurst = DMA_MBURST_SINGLE;              /* Memory burst                      */
    DmaRxHandle_l.Init.PeriphBurst = DMA_PBURST_SINGLE;           /* Peripheral burst                  */
    DmaRxHandle_l.Instance = USARTx_RX_DMA_STREAM;
    if(HAL_DMA_Init(&DmaRxHandle_l) == HAL_OK)
    {
        __HAL_LINKDMA(pUsartHandler_p, hdmarx, DmaRxHandle_l);

        /* Configure DMAx - Transmit channel (memory -> USART) */
        DmaTxHandle_l.Init.Channel = USARTx_TX_DMA_CHANNEL;
        DmaTxHandle_l.Init.Direction = DMA_MEMORY_TO_PERIPH;          /* M2P transfer mode                 */
        DmaTxHandle_l.Init.PeriphInc = DMA_PINC_DISABLE;              /* Peripheral increment mode Disable */
        DmaTxHandle_l.Init.MemInc = DMA_MINC_ENABLE;                  /* Memory increment mode Enable      */
        DmaTxHandle_l.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; /* Peripheral data alignment : Byte  */
        DmaTxHandle_l.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;    /* memory data alignment : Byte      */
        DmaTxHandle_l.Init.Mode = DMA_NORMAL;                         /* Normal DMA mode                   */
        DmaTxHandle_l.Init.Priority = DMA_PRIORITY_LOW;               /* priority level : low              */
        DmaTxHandle_l.Init.FIFOMode = DMA_FIFOMODE_DISABLE;           /* FIFO mode disabled                */
        DmaTxHandle_l.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
        DmaTxHandle_l.Init.MemBurst = DMA_MBURST_SINGLE;              /* Memory burst                      */
        DmaTxHandle_l.Init.PeriphBurst = DMA_PBURST_SINGLE;           /* Peripheral burst                  */
        DmaTxHandle_l.Instance = USARTx_TX_DMA_STREAM;
        if(HAL_DMA_Init(&DmaTxHandle_l) == HAL_OK)
        {
            __HAL_LINKDMA(pUsartHandler_p, hdmatx, DmaTxHandle_l);

            fReturn = TRUE;
        }
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief  Initialize the DMA interrupts in NVIC

Setupt transfer finished and transfer error interrupts for both DMA
channels.

\note
The function calls under 'NVIC configuration for UART transfer complete
interrupt' have been added for HAL 1.4.0 (STM32Cube_FW_F4_V1.4.0) support.

*/
/*----------------------------------------------------------------------------*/
static void initNvic(void)
{
    /* NVIC configuration for DMA transfer complete interrupt (USARTx_TX) */
    HAL_NVIC_SetPriority(USARTx_DMA_TX_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USARTx_DMA_TX_IRQn);

    /* NVIC configuration for DMA transfer complete interrupt (USARTx_RX) */
    HAL_NVIC_SetPriority(USARTx_DMA_RX_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(USARTx_DMA_RX_IRQn);

    /* NVIC configuration for UART transfer complete interrupt */
    HAL_NVIC_SetPriority(USARTx_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USARTx_IRQn);
}

/*----------------------------------------------------------------------------*/
/**
\brief  USART transfer completed callback.

\param pUsartHandle_p     Pointer to the USART handle
*/
/*----------------------------------------------------------------------------*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef * pUsartHandle_p)
{
    if(pUsartHandle_p == &UsartHandle_l)
    {
        fTxFinished_l = TRUE;

        /* Call transfer finished callback function */
        if(pfnTransfFin_l != NULL)
            pfnTransfFin_l();
    }
}

/*----------------------------------------------------------------------------*/
/**
\brief  USART reception completed callback.

\param pUsartHandle_p     Pointer to the USART handle
*/
/*----------------------------------------------------------------------------*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef * pUsartHandle_p)
{
    if(pUsartHandle_p == &UsartHandle_l)
    {
        fRxFinished_l = TRUE;

        /* Call receive finished callback function */
        if(pfnReceiveFin_l != NULL)
            pfnReceiveFin_l();
    }
}

/*----------------------------------------------------------------------------*/
/**
\brief  DMA error callback

\param pUsartHandle_p     Pointer to the USART handle
*/
/*----------------------------------------------------------------------------*/
void HAL_UART_ErrorCallback(UART_HandleTypeDef * pUsartHandle_p)
{
    if(pUsartHandle_p == &UsartHandle_l)
    {
         /* Call transfer error callback function */
         if(pfnTransfError_l != NULL)
             pfnTransfError_l();
    }
}

/*----------------------------------------------------------------------------*/
/**
\brief  This function handles DMA Rx interrupt request
*/
/*----------------------------------------------------------------------------*/
void USARTx_DMA_RX_IRQHandler(void)
{
    HAL_NVIC_ClearPendingIRQ(USARTx_DMA_RX_IRQn);
    HAL_DMA_IRQHandler(UsartHandle_l.hdmarx);
}

/*----------------------------------------------------------------------------*/
/**
\brief  This function handles DMA Tx interrupt request.
*/
/*----------------------------------------------------------------------------*/
void USARTx_DMA_TX_IRQHandler(void)
{
    HAL_NVIC_ClearPendingIRQ(USARTx_DMA_TX_IRQn);
    HAL_DMA_IRQHandler(UsartHandle_l.hdmatx);
}

/*----------------------------------------------------------------------------*/
/**
\brief  This function handles UART TC interrupt request.

\note
This function has been added for HAL 1.4.0 (STM32Cube_FW_F4_V1.4.0) support.

*/
/*----------------------------------------------------------------------------*/
void USARTx_IRQ_Handler(void)
{
    HAL_NVIC_ClearPendingIRQ(USARTx_IRQn);
    HAL_UART_IRQHandler(&UsartHandle_l);
}

/*----------------------------------------------------------------------------*/
/**
\brief  Wait until the transfer is finished

\param[in] pTransFin_p      Pointer to the transfer finished flag
\param[in] timeoutMs_p      Time in ms until timeout
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN waitForTransferFinished(BOOLEAN * pTransFin_p, UINT32 timeoutMs_p)
{
    BOOLEAN fTransFin = FALSE;
    UINT32 currTime = HAL_GetTick();
    UINT32 timeoutTime = currTime + timeoutMs_p;

    /* Wait until timeout is reached or transfer is finished */
    while(*pTransFin_p == FALSE && currTime < timeoutTime)
    {
        currTime = HAL_GetTick();
    }

    fTransFin = *pTransFin_p;

    return fTransFin;
}

/*----------------------------------------------------------------------------*/
/**
\brief  Disable the FIFO error IR

The HAL library sometimes enables the FIFO error interrupt. As the FIFO is
disabled and not used this error has no meaning. Therefore the FE interrupt
needs to be always disabled!
*/
/*----------------------------------------------------------------------------*/
static void disableFifoErrorIr(void)
{
    /* Disable FIFO error IR */
    __HAL_DMA_DISABLE_IT(&DmaTxHandle_l, DMA_IT_FE);
    __HAL_DMA_DISABLE_IT(&DmaRxHandle_l, DMA_IT_FE);
}

/**
 * \}
 * \}
 */
