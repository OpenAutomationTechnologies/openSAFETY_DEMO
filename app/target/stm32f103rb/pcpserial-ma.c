/**
********************************************************************************
\file   target/stm32f103rb/pcpserial-ma.c

\defgroup module_targ_stm32f103_serial_ma PCP serial module (Master Mode)
\{

\brief  Implements the driver for the serial device in master mode

Defines the platform specific functions for the serial to interconnect the app
with the POWERLINK processor. (Target is the stm32f103rb board)

\ingroup group_app_targ_stm32f103
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
#include <common/pcpserial.h>

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


#define SPIx                             SPI1
#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI1_CLK_ENABLE()
#define DMAx_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_NSS_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPIx_CLK_DISABLE()               __HAL_RCC_SPI1_CLK_DISABLE()

/* Definition for SPIx Pins */
#define SPIx_SCK_PIN                     GPIO_PIN_5
#define SPIx_MISO_PIN                    GPIO_PIN_6
#define SPIx_MOSI_PIN                    GPIO_PIN_7

#define SPIx_GPIO_PORT                   GPIOA

#define SPIx_SSN_PIN                     GPIO_PIN_6
#define SPIx_SSN_GPIO_PORT               GPIOB

/* Definition for SPIx's DMA */
#define SPIx_TX_DMA_CHANNEL              DMA1_Channel3
#define SPIx_RX_DMA_CHANNEL              DMA1_Channel2

/* Definition for SPIx's NVIC */
#define SPIx_DMA_TX_IRQn                 DMA1_Channel3_IRQn
#define SPIx_DMA_RX_IRQn                 DMA1_Channel2_IRQn
#define SPIx_DMA_TX_IRQHandler           DMA1_Channel3_IRQHandler
#define SPIx_DMA_RX_IRQHandler           DMA1_Channel2_IRQHandler

/*----------------------------------------------------------------------------*/
/* local types                                                                */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/
static tPcpSerialTransferFin pfnTransfFin_l = NULL;

static SPI_HandleTypeDef SpiHandle_l;        /**< SPI handle structure */
static DMA_HandleTypeDef DmaRxHandle_l;      /**< DMA receive handle structure */
static DMA_HandleTypeDef DmaTxHandle_l;      /**< DMA transmit handle structure */
/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static void initGpio(void);
static BOOL initSpi(void);
static BOOL initDma(SPI_HandleTypeDef* pSpiHandler_p);
static void initNvic(void);

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
    UINT8 fReturn = FALSE;
    GPIO_InitTypeDef GPIO_InitStruct;

    UNUSED_PARAMETER(pTransParam_p);

    if(initSpi())
    {
        initNvic();

        SPIx_NSS_GPIO_CLK_ENABLE();

        /* SPI_NSS_Soft pin configuration */
        GPIO_InitStruct.Pin = SPIx_SSN_PIN;
        GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(SPIx_SSN_GPIO_PORT, &GPIO_InitStruct);

        /* Set NSS high (not active) */
        HAL_GPIO_WritePin(SPIx_SSN_GPIO_PORT, SPIx_SSN_PIN, GPIO_PIN_SET);

        /* Assign transfer finished interrupt callback function */
        pfnTransfFin_l = pfnTransfFin_p;

        fReturn = TRUE;
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief  Close the serial device
*/
/*----------------------------------------------------------------------------*/
void pcpserial_exit(void)
{
    pfnTransfFin_l =  NULL;

    pfnTransfFin_l =  NULL;

    HAL_GPIO_DeInit(SPIx_SSN_GPIO_PORT, SPIx_SSN_PIN);

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
    BOOL retVal = FALSE;
    UINT8* pConsWithInit = NULL;
    UINT8* pProdWithInit = NULL;
    UINT32 transLen = 0;

    if(pHandlParam_p != NULL)
    {
        if(pHandlParam_p->consDesc_m.buffSize_m == pHandlParam_p->prodDesc_m.buffSize_m)
        {
            pConsWithInit = pHandlParam_p->consDesc_m.pBuffBase_m - 4;
            pProdWithInit = pHandlParam_p->prodDesc_m.pBuffBase_m - 4;
            transLen = pHandlParam_p->consDesc_m.buffSize_m + 4;

            /* Wait until SPI is really ready for this transfer */
            while(HAL_SPI_GetState(&SpiHandle_l) == HAL_SPI_STATE_BUSY_TX_RX)
            {
                /* Busy wait here until transfer is finished! */
            }

            /* Set NSS low (active) */
            HAL_GPIO_WritePin(SPIx_SSN_GPIO_PORT, SPIx_SSN_PIN, GPIO_PIN_RESET);

            /* Enable the DMA rx channel */
            if(HAL_SPI_TransmitReceive_DMA(&SpiHandle_l, pProdWithInit, pConsWithInit, transLen) == HAL_OK)
            {
                retVal = TRUE;
            }
        }
    }

    return retVal;
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/*----------------------------------------------------------------------------*/
/**
\brief  Initialize the peripherals needed by the SPI core

This function is called in the call of HAL_SPI_Init and initializes all
peripherals needed to carry out the transfer with DMA.

\param pSpiHandler_p    Pointer to the SPI handler
*/
/*----------------------------------------------------------------------------*/
void HAL_SPI_MspInit(SPI_HandleTypeDef* pSpiHandler_p)
{
    if(pSpiHandler_p->Instance == SPIx)
    {
        SPIx_CLK_ENABLE();

        /* Initialize all needed peripherals */
        initGpio();
        initDma(pSpiHandler_p);
    }
}

/*----------------------------------------------------------------------------*/
/**
\brief  Close the peripherals needed by the SPI core

This function is called in the call of HAL_SPI_DeInit and closes all
peripherals needed by the SPI core.

\param pSpiHandler_p    Pointer to the SPI handler
*/
/*----------------------------------------------------------------------------*/
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* pSpiHandler_p)
{
    if(pSpiHandler_p->Instance==SPIx)
    {
        SPIx_CLK_DISABLE();

        /* Disable the DMA Streams */
        HAL_DMA_DeInit(pSpiHandler_p->hdmarx);
        HAL_DMA_DeInit(pSpiHandler_p->hdmatx);

        /* Disable peripherals and GPIO Clocks */
        HAL_GPIO_DeInit(SPIx_GPIO_PORT, SPIx_SCK_PIN);
        HAL_GPIO_DeInit(SPIx_GPIO_PORT, SPIx_MISO_PIN);
        HAL_GPIO_DeInit(SPIx_GPIO_PORT, SPIx_MOSI_PIN);
    }
}

/*----------------------------------------------------------------------------*/
/**
\brief  Initialize the SPI GPIO pins

Enable SCK, MOSI and NSS as outputs and MISO as input.
*/
/*----------------------------------------------------------------------------*/
static void initGpio(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = SPIx_SCK_PIN | SPIx_MISO_PIN | SPIx_MOSI_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(SPIx_GPIO_PORT, &GPIO_InitStruct);
}

/*----------------------------------------------------------------------------*/
/**
\brief  Initialize the SPI core as master

Setup SPI core as full duplex 8bit data with CPOL, CPHA = (0,0)
*/
/*----------------------------------------------------------------------------*/
static BOOL initSpi(void)
{
    BOOL retVal = FALSE;

    memset(&SpiHandle_l, 0, sizeof(SPI_HandleTypeDef));

    /* SPI configuration */
    SpiHandle_l.Instance = SPIx;
    SpiHandle_l.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    SpiHandle_l.Init.Direction = SPI_DIRECTION_2LINES;
    SpiHandle_l.Init.CLKPhase = SPI_PHASE_1EDGE;
    SpiHandle_l.Init.CLKPolarity = SPI_POLARITY_LOW;
    SpiHandle_l.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    SpiHandle_l.Init.CRCPolynomial = 7;
    SpiHandle_l.Init.DataSize = SPI_DATASIZE_8BIT;
    SpiHandle_l.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SpiHandle_l.Init.NSS = SPI_NSS_SOFT;
    SpiHandle_l.Init.TIMode = SPI_TIMODE_DISABLE;
    SpiHandle_l.Init.Mode = SPI_MODE_MASTER;

    if(HAL_SPI_Init(&SpiHandle_l) == HAL_OK)
    {
        retVal = TRUE;
    }

    return retVal;
}

/*----------------------------------------------------------------------------*/
/**
\brief  Initialize DMA channel 2 and 3

Setup the DMA for receive from SPI to RAM and back!

\param pSpiHandler_p    Pointer to the SPI handler
*/
/*----------------------------------------------------------------------------*/
static BOOL initDma(SPI_HandleTypeDef* pSpiHandler_p)
{

    BOOL retVal = FALSE;

    memset(&DmaRxHandle_l, 0, sizeof(DMA_HandleTypeDef));
    memset(&DmaTxHandle_l, 0, sizeof(DMA_HandleTypeDef));


    /* Enable DMAx clock */
    DMAx_CLK_ENABLE();

    /* Configure DMAx - Receive channel (SPI -> memory) */
    DmaRxHandle_l.Instance = SPIx_RX_DMA_CHANNEL;
    DmaRxHandle_l.Init.Direction = DMA_PERIPH_TO_MEMORY;          /* P2M transfer mode                 */
    DmaRxHandle_l.Init.PeriphInc = DMA_PINC_DISABLE;              /* Peripheral increment mode Disable */
    DmaRxHandle_l.Init.MemInc = DMA_MINC_ENABLE;                  /* Memory increment mode Enable      */
    DmaRxHandle_l.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; /* Peripheral data alignment : Byte  */
    DmaRxHandle_l.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;    /* memory data alignment : Byte      */
    DmaRxHandle_l.Init.Mode = DMA_NORMAL;                         /* Normal DMA mode                   */
    DmaRxHandle_l.Init.Priority = DMA_PRIORITY_HIGH;              /* priority level : high             */

    if(HAL_DMA_Init(&DmaRxHandle_l) == HAL_OK)
    {
        __HAL_LINKDMA(pSpiHandler_p, hdmarx, DmaRxHandle_l);

        /* Configure DMAx - Transmit channel (memory -> SPI) */
        DmaTxHandle_l.Instance = SPIx_TX_DMA_CHANNEL;
        DmaTxHandle_l.Init.Direction = DMA_MEMORY_TO_PERIPH;          /* M2P transfer mode                 */
        DmaTxHandle_l.Init.PeriphInc = DMA_PINC_DISABLE;              /* Peripheral increment mode Disable */
        DmaTxHandle_l.Init.MemInc = DMA_MINC_ENABLE;                  /* Memory increment mode Enable      */
        DmaTxHandle_l.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; /* Peripheral data alignment : Byte  */
        DmaTxHandle_l.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;    /* memory data alignment : Byte      */
        DmaTxHandle_l.Init.Mode = DMA_NORMAL;                         /* Normal DMA mode                   */
        DmaTxHandle_l.Init.Priority = DMA_PRIORITY_LOW;               /* priority level : low              */
        if(HAL_DMA_Init(&DmaTxHandle_l) == HAL_OK)
        {
            __HAL_LINKDMA(pSpiHandler_p, hdmatx, DmaTxHandle_l);

            retVal = TRUE;
        }
    }

    return retVal;

}

/*----------------------------------------------------------------------------*/
/**
\brief  Initialize the DMA interrupts in NVIC

Setupt transfer finished and transfer error interrupts for both DMA
channels.
*/
/*----------------------------------------------------------------------------*/
static void initNvic(void)
{
    /* NVIC configuration for DMA transfer complete interrupt (SPIx_TX) */
    HAL_NVIC_SetPriority(SPIx_DMA_TX_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(SPIx_DMA_TX_IRQn);

    /* NVIC configuration for DMA transfer complete interrupt (SPIx_RX) */
    HAL_NVIC_SetPriority(SPIx_DMA_RX_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(SPIx_DMA_RX_IRQn);
}


/*----------------------------------------------------------------------------*/
/**
\brief  TxRx transfer completed callback.

\param pSpiHandle_p     Pointer to the SPI handle
*/
/*----------------------------------------------------------------------------*/
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *pSpiHandle_p)
{
    if(pSpiHandle_p == &SpiHandle_l)
    {
        while(HAL_SPI_GetState(pSpiHandle_p) == HAL_SPI_STATE_BUSY_TX_RX)
        {
            /* Busy wait here until transfer is finished! */
        }

        /* Reset SPI NSS pin */
        HAL_GPIO_WritePin(SPIx_SSN_GPIO_PORT, SPIx_SSN_PIN, GPIO_PIN_SET);

        /* Call transfer finished callback function */
        if(pfnTransfFin_l != NULL)
            pfnTransfFin_l(FALSE);
    }
}


/*----------------------------------------------------------------------------*/
/**
\brief  DMA error callback

\param pSpiHandle_p     Pointer to the SPI handle
*/
/*----------------------------------------------------------------------------*/
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *pSpiHandle_p)
{
    if(pSpiHandle_p == &SpiHandle_l)
    {
         /* Call transfer finished callback function */
         if(pfnTransfFin_l != NULL)
             pfnTransfFin_l(TRUE);
    }
}

/*----------------------------------------------------------------------------*/
/**
\brief  This function handles DMA Rx interrupt request
*/
/*----------------------------------------------------------------------------*/
void SPIx_DMA_RX_IRQHandler(void)
{
    HAL_NVIC_ClearPendingIRQ(SPIx_DMA_RX_IRQn);
    HAL_DMA_IRQHandler(SpiHandle_l.hdmarx);
}



/*----------------------------------------------------------------------------*/
/**
\brief  This function handles DMA Tx interrupt request.
*/
/*----------------------------------------------------------------------------*/
void SPIx_DMA_TX_IRQHandler(void)
{
    HAL_NVIC_ClearPendingIRQ(SPIx_DMA_TX_IRQn);
    HAL_DMA_IRQHandler(SpiHandle_l.hdmatx);
}

/**
 * \}
 * \}
 */
