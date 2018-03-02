/**
********************************************************************************
\file   demo-sn-gpio/shnf/shnftx-single.c

\defgroup module_sn_shnf_txsingle SHNF transmit module (Single channel)
\{

\brief  SHNF transmit functions for single channeled demos

This module implements the transmit parts of the SHNF for single channeled
demos. It prepares the frames for transmission to the underlying fieldbus.

\ingroup group_app_sn_shnf
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
#include <shnf/shnftx.h>

#include <shnf/hnf.h>

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
static BOOLEAN swapSubFrames(UINT8 * pTargBuffer_p, UINT16 targBuffLen_p,
                             UINT8 * pSrcBuffer_p, UINT16 srcBuffLen_p,
                             BOOLEAN fIsSlim_p);

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Post the SPDO transmit frame to the HNF

\param[in] pSrcBase_p   Pointer to the source buffer
\param[in] srcLen_p     The length of the source buffer
\param[in] pDstBase_p   Pointer to the destination buffer
\param[in] dstLen_p     The length of the destination buffer

\retval TRUE    Successfully processed the transmit frame
\retval FALSE   Error on posting the frame
*/
/*----------------------------------------------------------------------------*/
BOOLEAN shnftx_postSpdoFrame(UINT8 * pSrcBase_p, UINT32 srcLen_p,
                             UINT8 * pDstBase_p, UINT32 dstLen_p)
{
    BOOLEAN fReturn = FALSE;

    if(pSrcBase_p != NULL && srcLen_p > 0 &&
       pDstBase_p != NULL && dstLen_p > 0  )
    {
        /* Prepare synchronous target buffer and swap frame */
        if(swapSubFrames(pDstBase_p, dstLen_p, pSrcBase_p, srcLen_p, FALSE))
        {
            /* Forward filled buffer to HNF */
            if(hnf_postSyncTx(pDstBase_p, dstLen_p))
            {
                fReturn = TRUE;
            }
            else
            {
                errh_postFatalError(kErrSourceShnf, kErrorSyncFramePostingFailed, 0);
            }
        }
        else
        {
            errh_postFatalError(kErrSourceShnf, kErrorSyncFrameCopyFailed, 0);
        }
    }
    else
    {
        errh_postFatalError(kErrSourceShnf, kErrorInvalidParameter, 0);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Post the SSDO/SNMT transmit frame to the HNF

\param[in] pSrcBase_p   Pointer to the source buffer
\param[in] srcLen_p     The length of the source buffer
\param[in] pDstBase_p   Pointer to the destination buffer
\param[in] dstLen_p     The length of the destination buffer
\param[in] isSlim_p     TRUE if the frame is a slim frame

\retval TRUE    Successfully processed the transmit frame
\retval FALSE   Error on posting the frame
*/
/*----------------------------------------------------------------------------*/
BOOLEAN shnftx_postSsdoSnmtFrame(UINT8 * pSrcBase_p, UINT32 srcLen_p,
                                 UINT8 * pDstBase_p, UINT32 dstLen_p,
                                 BOOLEAN isSlim_p)
{
    BOOLEAN fReturn = FALSE;

    if(pSrcBase_p != NULL && srcLen_p > 0 &&
       pDstBase_p != NULL && dstLen_p > 0  )
    {
        /* Prepare asynchronous target buffer and swap frame */
        if(swapSubFrames(pDstBase_p, dstLen_p, pSrcBase_p, srcLen_p, isSlim_p))
        {
            /* Post transmit frame to hnf */
            if(hnf_postAsyncTxChannel0(pDstBase_p, srcLen_p))
            {
                fReturn = TRUE;
            }
            else
            {
                errh_postFatalError(kErrSourceShnf, kErrorAsyncFramePostingFailed, 0);
            }
        }
        else
        {
            errh_postFatalError(kErrSourceShnf, kErrorAsyncFrameCopyFailed, 0);
        }
    }
    else
    {
        errh_postFatalError(kErrSourceShnf, kErrorInvalidParameter, 0);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process the SHNF transmit to trigger an image exchange

\retval TRUE    Successfully processed the transmit frame
\retval FALSE   Error on posting the frame
*/
/*----------------------------------------------------------------------------*/
BOOLEAN shnftx_process(void)
{
    /* Sending is done without interruption (Processing not needed) */

    return TRUE;
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */


/*----------------------------------------------------------------------------*/
/**
\brief    Swap sub1 and sub2 for transmission

This function swaps subframe1 and subframe2 and provides the result in the
target buffer.

\param pTargBuffer_p   Pointer to the target buffer
\param targBuffLen_p   Length of the target buffer
\param pSrcBuffer_p    Pointer to the source frame
\param srcBuffLen_p    Length of the frame
\param fIsSlim_p       TRUE if the frame is an SSDO slim frame

\return TRUE if the target buffer is valid; FALSE otherwise
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN swapSubFrames(UINT8 * pTargBuffer_p, UINT16 targBuffLen_p,
                             UINT8 * pSrcBuffer_p, UINT16 srcBuffLen_p,
                             BOOLEAN fIsSlim_p)
{
    BOOLEAN fReturn = FALSE;
    UINT16 subFrame1Pos = 0;
    UINT16 subFrame2Targ = 0;
    UINT16 subFrame1Len = 0;
    UINT16 subFrame2Len = 0;

    /* Check if frame fits into target buffer */
    if(srcBuffLen_p <= targBuffLen_p)
    {
        if(fIsSlim_p)
        {
            /* Calculate frame positions and lengths for slim frames */
            if(srcBuffLen_p <= SLIM_FRAME_MAX_CRC8_LEN)
                subFrame1Pos = SLIM_FRAME_SUB1_POS_CRC8;
            else
                subFrame1Pos = SLIM_FRAME_SUB1_POS_CRC16;

            subFrame2Targ = srcBuffLen_p - subFrame1Pos;
        }
        else
        {
            /* Calculate frame positions and lengths for normal frames */
            subFrame1Pos = (srcBuffLen_p>>1) + 1;
            subFrame2Targ = subFrame1Pos - 1;
        }

        /* Get length of sub frames */
        subFrame1Len = srcBuffLen_p - subFrame1Pos;
        subFrame2Len = srcBuffLen_p - subFrame1Len;

        /* Copy subframe1 to start of target buffer */
        MEMCOPY(pTargBuffer_p, &pSrcBuffer_p[subFrame1Pos], subFrame1Len);
        /* Add subframe2 to end of subframe1 */
        MEMCOPY(&pTargBuffer_p[subFrame2Targ], pSrcBuffer_p, subFrame2Len);

        fReturn = TRUE;
    }

    return fReturn;
}

/**
 * \}
 * \}
 */
