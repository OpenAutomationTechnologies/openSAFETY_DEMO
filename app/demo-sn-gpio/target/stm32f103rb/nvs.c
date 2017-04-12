/**
********************************************************************************
\file   demo-sn-gpio/target/stm32f103rb/nvs.c

\defgroup module_sn_stm32f103_nvs Non volatile storage (NVS) module
\{

\brief  Target specific functions to access the non volatile storage.

This module implements the hardware near target specific functions of the
flash memory for target stm32f103rb (Cortex-M3).

\ingroup group_app_sn_targ_stm32f103
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
#include <sn/nvs.h>

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
#define FLASH_IMAGE_OFFSET        0x1E400UL       /**< Offset of the stored SOD in the NVS (Page: 120) */

/*----------------------------------------------------------------------------*/
/* local types                                                                */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/
static UINT32 imageBaseAddr_l = 0;

static FLASH_EraseInitTypeDef EraseInitHandle_l;

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static BOOLEAN progUint32(UINT32 address_p, UINT32 * pData_p);
static BOOLEAN progUint16(UINT32 address_p, UINT16 * pData_p);
static BOOLEAN progUint8(UINT32 address_p, UINT8 * pData_p);

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the non volatile storage

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
BOOLEAN nvs_init(void)
{
    /* Set base address of flash image */
    imageBaseAddr_l = (UINT32)(FLASH_BASE + FLASH_IMAGE_OFFSET);

    /* Clear All pending flags */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_BSY | FLASH_FLAG_EOP|FLASH_FLAG_PGERR |FLASH_FLAG_WRPERR);

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Close the non volatile storage
*/
/*----------------------------------------------------------------------------*/
void nvs_close(void)
{
    imageBaseAddr_l = 0;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Write data to the non volatile storage

\param offset_p  The offset of the data in the storage
\param pData_p   Pointer to the data to write
\param length_p  The length of the data to write

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
BOOLEAN nvs_write(UINT32 offset_p, UINT8 * pData_p, UINT32 length_p)
{
    BOOLEAN retVal = FALSE, error = FALSE;
    UINT32 address = imageBaseAddr_l + offset_p;

    if(length_p > 0 && pData_p != NULL)
    {
        HAL_FLASH_Unlock();

        while(length_p > 0)
        {
            /* Program all UINT32 fields first */
            if(length_p > 4 || length_p == 4)
            {
                if(progUint32(address, (UINT32*)pData_p) == FALSE)
                {
                    error = TRUE;
                    break;
                }
                length_p -= 4;
                address += 4;
                pData_p += 4;
            }
            else if(length_p == 2 || length_p == 3)
            {
                if(progUint16(address, (UINT16*)pData_p) == FALSE)
                {
                    error = TRUE;
                    break;
                }
                length_p -= 2;
                address += 2;
                pData_p += 2;
            }
            else if(length_p == 1)
            {
                if(progUint8(address, pData_p) == FALSE)
                {
                    error = TRUE;
                    break;
                }
                length_p--;
                address++;
                pData_p++;
            }

            else
            {
                error = TRUE;
                break;
            }
        }

        HAL_FLASH_Lock();
    }

    if(error == FALSE)
        retVal = TRUE;

    return retVal;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Read an Uint32 from the non volatile storage

\param offset_p       The offset of the data in the storage
\param ppReadData_p   Pointer to the resulting read data

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
BOOLEAN nvs_readUint32(UINT32 offset_p, UINT32 ** ppReadData_p)
{
    BOOLEAN retVal = FALSE;

    if(ppReadData_p != NULL)
    {
        *ppReadData_p = (UINT32 *)((UINT32)imageBaseAddr_l + offset_p);
        retVal = TRUE;
    }

    return retVal;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Erase the sector behind the offset

\param offset_p  The offset of the sector to erase

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
BOOLEAN nvs_erase(UINT32 offset_p)
{
    BOOLEAN retVal = FALSE;
    uint32_t pageError = 0;
    UINT32 address = imageBaseAddr_l + offset_p;

    HAL_FLASH_Unlock();

    EraseInitHandle_l.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitHandle_l.PageAddress = address;
    EraseInitHandle_l.NbPages     = 1;

    if(HAL_FLASHEx_Erase(&EraseInitHandle_l, &pageError) == HAL_OK)
    {
        retVal = TRUE;
    }

    HAL_FLASH_Lock();

    return retVal;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Get the base address of the NVS memory offset

This functions enables to bypass the nvs_read function and to access the data
directly. This can be done on parallel flashes where no command is needed
to read data from the flash.

\return Pointer to the offset address
*/
/*----------------------------------------------------------------------------*/
UINT8* nvs_getAddress(UINT32 offset_p)
{
    return (UINT8*)(imageBaseAddr_l + offset_p);
}


/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/*----------------------------------------------------------------------------*/
/**
\brief    Program a word to flash

\param address_p    The address to write to
\param pData_p      Pointer to the data to write

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN progUint32(UINT32 address_p, UINT32 * pData_p)
{
    BOOLEAN retVal = FALSE;

    if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address_p, (UINT64)(*pData_p)) == HAL_OK)
    {
        retVal = TRUE;
    }

    return retVal;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Program a half word to flash

\param address_p    The address to write to
\param pData_p      Pointer to the data to write

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN progUint16(UINT32 address_p, UINT16 * pData_p)
{
    BOOLEAN retVal = FALSE;


    if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address_p, (UINT64)(*pData_p)) == HAL_OK)
    {
        retVal = TRUE;
    }

    return retVal;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Program a byte to flash

This function converts the byte to a half word and programs this value to flash.
Therefore this function can only be used at the end of the stream for the last
byte. Access to the flash which is not aligned results in a hard fault of
the CPU.

\param address_p    The address to write to
\param pData_p      Pointer to the data to write

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN progUint8(UINT32 address_p, UINT8 * pData_p)
{
    BOOLEAN retVal = FALSE;
    UINT16 tempData = 0;

    if(pData_p != NULL)
    {
        /* Convert Uint8 to 16bit wide variable (Only half word accesses allowed) */
        tempData = (UINT16)*pData_p;

        /* Perform half word write access */
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address_p, (UINT64)(tempData)) == HAL_OK)
        {
            retVal = TRUE;
        }
    }

    return retVal;
}


/**
 * \}
 * \}
 */
