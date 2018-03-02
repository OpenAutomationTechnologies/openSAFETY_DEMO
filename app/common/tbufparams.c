/**
********************************************************************************
\file   common/tbufparams.c

\defgroup module_com_tbufp Triple buffer settings generator module
\{

\brief  Implements helper functions for tbuf parameter handling

This helper functions generate buffer lists which can be used in the libpsi
or by the serial interface.

\ingroup group_app_common

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

#include <common/tbufparams.h>

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

/*----------------------------------------------------------------------------*/
/* local types                                                                */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static UINT32 getInitOffset(INT8 isProducer_p, BOOL isFirstAck_p);

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/


/*----------------------------------------------------------------------------*/
/**
\brief    Generate a list of buffer descriptors for the useage inside the library

\param[in]  pTbufBase_m      Base address of the triple buffer image
\param[in]  tbufCount_m      Count of buffers in the image
\param[out] pBuffDescList_p     Buffer descriptor list in library format

\return TRUE on success; FALSE on error*/
/*----------------------------------------------------------------------------*/
BOOL tbufp_genDescList(UINT8 * pTbufBase_m, UINT16 tbufCount_m, tBuffDescriptor* pBuffDescList_p)
{
    UINT8 i;
    BOOL retVal = FALSE;
    tBuffDescriptor* pBuffDec = pBuffDescList_p;
    tTbufDescriptor tbufDescList[] = TBUF_INIT_VEC;
    UINT32 offset = 0;
    BOOL isFirstAck = TRUE;

    if(pTbufBase_m != NULL && pBuffDescList_p != NULL)
    {
        /* Generate a descriptor list which can be used in the library */
        for(i=0; i < tbufCount_m; i++, pBuffDec++)
        {
            offset = getInitOffset(tbufDescList[i].isProducer_m, isFirstAck);

            isFirstAck = FALSE;

            pBuffDec->pBuffBase_m = (UINT8 *)((UINT32)pTbufBase_m + (UINT32)tbufDescList[i].buffOffset_m + offset);
            pBuffDec->buffSize_m = tbufDescList[i].buffSize_m;
        }

        retVal = TRUE;
    }

    return retVal;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Generate the transfer parameters with initialization offsets

\param[in]  pTbufBase_m      Base address of the triple buffer image
\param[out] p_transParam     Pointer to the transfer parameters

\return TRUE on success; FALSE on error*/
/*----------------------------------------------------------------------------*/
BOOL tbufp_genTransferParams(UINT8 * pTbufBase_m, tHandlerParam * p_transParam)
{
    tTbufDescriptor tbufDescList[] = TBUF_INIT_VEC;
    BOOL retVal = FALSE;

    if(pTbufBase_m != NULL && p_transParam != NULL)
    {
        /* Setup consumer parameters */
        p_transParam->consDesc_m.pBuffBase_m = pTbufBase_m;
        p_transParam->consDesc_m.buffSize_m = tbufDescList[TBUF_NUM_CON].buffOffset_m +
                                              tbufDescList[TBUF_NUM_CON].buffSize_m +
                                              (UINT32)TBUF_INIT_SIZE;

        /* Setup producer parameters */
        p_transParam->prodDesc_m.pBuffBase_m = (UINT8 *)((UINT32)pTbufBase_m +
                                                         (UINT32)tbufDescList[TBUF_NUM_CON+1].buffOffset_m +
                                                         (UINT32)TBUF_INIT_SIZE);
        p_transParam->prodDesc_m.buffSize_m = (UINT32)TBUF_IMAGE_SIZE - p_transParam->consDesc_m.buffSize_m;

        retVal = TRUE;
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
\brief    Determine the SPI initialization offset

The SPI protocol always needs 4 byte initialization data. This function
details if this buffer needs 4 byte offset (app consuming buffers) or
8 byte offset. (app producing)

\param[in] isProducer_p    Is this buffer a producing buffer
\param[in] isFirstAck_p    Is this buffer the first ACK register

\return The offset of this buffer*/
/*----------------------------------------------------------------------------*/
static UINT32 getInitOffset(INT8 isProducer_p, BOOL isFirstAck_p)
{
    UINT32 offset = 0;

    if(isProducer_p  == (INT8)-1)
    {
        /* This buffer is an acknowledge register */
        if(isFirstAck_p)
        {
            offset = (UINT32)TBUF_INIT_SIZE;
        }
        else
        {
            offset = (UINT32)TBUF_INIT_SIZE * 2;
        }
    }
    else if(isProducer_p == 1)
    {
        offset = (UINT32)TBUF_INIT_SIZE * 2;
    }
    else if(isProducer_p == 0)
    {
        offset = (UINT32)TBUF_INIT_SIZE;
    }

    return offset;
}

/**
 * \}
 * \}
 */
