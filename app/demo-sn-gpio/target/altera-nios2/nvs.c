/**
********************************************************************************
\file   demo-sn-gpio/target/altera-nios2/nvs.c

\defgroup module_sn_nios2_nvs Non volatile storage (NVS) module
\{

\brief  Target specific functions to access the non volatile storage.

This module implements the hardware near target specific functions of the
flash memory for Altera Nios2.

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
#include <sn/nvs.h>

#include <system.h>
#include "sys/alt_flash.h"

/*============================================================================*/
/*            G L O B A L   D E F I N I T I O N S                             */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/
#define FLASH_IMAGE_OFFSET        0x200000UL       /**< Offset of the stored SOD in the NVS */

#define FLASH_DEV_NAME            CFI_FLASH_NAME   /**< Name of the flash controller */
#define FLASH_BASE                CFI_FLASH_BASE   /**< Base address of the flash controller */

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
static UINT32 imageBaseAddr_l = 0;
static alt_flash_fd* pFlashDesc_l = (alt_flash_fd*)0;

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static INT16 getBlockByOffset(UINT32 offset_p);

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
    BOOLEAN retVal = FALSE;

    /* Set base address of flash image */
    imageBaseAddr_l = (UINT32)(FLASH_BASE + FLASH_IMAGE_OFFSET);

    pFlashDesc_l = alt_flash_open_dev(FLASH_DEV_NAME);
    if(pFlashDesc_l != (alt_flash_fd*)0)
    {
        retVal = TRUE;
    }

    return retVal;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Close the non volatile storage
*/
/*----------------------------------------------------------------------------*/
void nvs_close(void)
{
    /* Close the open flash device */
    alt_flash_close_dev(pFlashDesc_l);
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
    BOOLEAN retVal = FALSE;
    int blockOffset;
    UINT32 address = FLASH_IMAGE_OFFSET + offset_p;

    if(pData_p != (UINT8*)0 && length_p > 0)
    {
        blockOffset = getBlockByOffset(address);
        if(blockOffset >= 0)
        {
            if(alt_write_flash_block(pFlashDesc_l, blockOffset, address, pData_p, length_p) == 0)
            {
                retVal = TRUE;
            }
        }
    }

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
        *ppReadData_p = (UINT32*)(imageBaseAddr_l + offset_p);

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
    BOOLEAN retVal = TRUE;
    UINT32 address = FLASH_IMAGE_OFFSET + offset_p;

    if(alt_erase_flash_block(pFlashDesc_l, address, 0) == 0)
    {
        retVal = TRUE;
    }

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
\brief    Get the flash block index by offset

\param offset_p  The global flash offset

\return The id of the block the offset is in
*/
/*----------------------------------------------------------------------------*/
static INT16 getBlockByOffset(UINT32 offset_p)
{
    UINT16 i, j;
    flash_region* pFirstReg = (flash_region*)0;
    flash_region* pCurrReg = (flash_region*)0;
    int regionCount = 0;
    INT16 isInBlock = -1;

    if(alt_get_flash_info(pFlashDesc_l, &pFirstReg, &regionCount) == 0)
    {
        /* Iterate over all regions */
        for(i=0; i<(UINT16)regionCount; i++)
        {
            pCurrReg = &pFirstReg[i];

            if(offset_p > (UINT32)(pCurrReg->offset + pCurrReg->region_size))
                continue;

            /* Iterate over all blocks */
            for(j=0; j<(UINT16)pCurrReg->number_of_blocks; j++)
            {
                /* Check if the provided block is in the region */
                if(offset_p <= ((UINT32)pCurrReg->offset + (pCurrReg->block_size * (j+1))))
                {
                    isInBlock = j;
                    break;
                }
            }

            /* If already found -> return */
            if(isInBlock != -1)
                break;

        }
    }

    return isInBlock;
}

/**
 * \}
 * \}
 */
