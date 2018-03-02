/**
********************************************************************************
\file   demo-sn-gpio/shnf/shnftx-dual.c

\defgroup module_sn_shnf_txdual SHNF transmit module (Dual channel)
\{

\brief  SHNF transmit functions for dual channeled demos

This module implements the transmit parts of the SHNF for dual channeled
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

#include <shnf/xcom.h>

#include <SCFMapi.h>
#include <SSCapi.h>

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
#define GET_POS_CRC8_SUB_1(len)         (len - 1)
#define GET_POS_CRC8_SUB_2(len)         (len >> 1)

#define GET_POS_CRC16_SUB_1(len)         (len - 2)
#define GET_POS_CRC16_SUB_2(len)         ((len >> 1) - 1)

/*----------------------------------------------------------------------------*/
/* local types                                                                */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static BOOLEAN getFrameDetails(UINT8 * pPayBase_p, UINT32 payLen_p, BOOLEAN isSlim_p,
                               UINT16 *pCrcSub1_p, UINT16 *pCrcSub2_p,
                               tSubFrameParams * pSub1Param_p, tSubFrameParams * pSub2Param_p);

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
    UINT16 crcSub1 = 0;
    UINT16 crcSub2 = 0;
    tSubFrameParams sub1Params;
    tSubFrameParams sub2Params;

    MEMSET(&sub1Params, 0, sizeof(tSubFrameParams));
    MEMSET(&sub2Params, 0, sizeof(tSubFrameParams));

    if(pSrcBase_p != NULL && srcLen_p > 0 &&
       pDstBase_p != NULL && dstLen_p > 0  )
    {
        /* Get the CRC values from the frame */
        if(getFrameDetails(pSrcBase_p, srcLen_p, FALSE,
                           &crcSub1, &crcSub2,
                           &sub1Params, &sub2Params))
        {
            /* Forward the CRC values to the xcom module */
            xcom_setSpdoCrc(crcSub1, crcSub2);

            /* Forward the frame to the xcom module */
            if(xcom_postSpdoFrame(&sub1Params, &sub2Params,
                                  pDstBase_p, dstLen_p))
            {
                fReturn = TRUE;
            }
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
    UINT16 crcSub1 = 0;
    UINT16 crcSub2 = 0;
    tSubFrameParams sub1Params;
    tSubFrameParams sub2Params;

    UNUSED_PARAMETER(dstLen_p);

    MEMSET(&sub1Params, 0, sizeof(tSubFrameParams));
    MEMSET(&sub2Params, 0, sizeof(tSubFrameParams));

    if(pSrcBase_p != NULL && srcLen_p > 0 &&
       pDstBase_p != NULL && dstLen_p > 0  )
    {
        /* Get the CRC values from the frame */
        if(getFrameDetails(pSrcBase_p, srcLen_p, isSlim_p,
                           &crcSub1, &crcSub2,
                           &sub1Params, &sub2Params))
        {
            /* Forward the CRC values to the xcom module */
            xcom_setSsdoSnmtCrc(crcSub1, crcSub2);

            /* Forward the frame to the xcom module */
            if(xcom_postSsdoSnmtFrame(&sub1Params, &sub2Params,
                                      pDstBase_p, srcLen_p))
            {
                fReturn = TRUE;
            }
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
    BOOLEAN fReturn = FALSE;

    if(xcom_transmit(SCFM_GetResetPath()))
    {
        fReturn = TRUE;
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
\brief    Get the CRC values of subframe one and two from the payload

\param[in] pPayBase_p       The base address of the frame
\param[in] payLen_p         The length of the frame
\param[in] isSlim_p         Is the frame slim
\param[out] pCrcSub1_p      The result CRC value of subframe one
\param[out] pCrcSub2_p      The result CRC value of subframe two
\param[out] pSub1Param_p    Pointer to the subframe one parameters
\param[out] pSub2Param_p    Pointer to the subframe two parameters

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN getFrameDetails(UINT8 * pPayBase_p, UINT32 payLen_p, BOOLEAN isSlim_p,
                               UINT16 *pCrcSub1_p, UINT16 *pCrcSub2_p,
                               tSubFrameParams * pSub1Param_p, tSubFrameParams * pSub2Param_p)
{
    BOOLEAN fReturn = FALSE;

    if(pCrcSub1_p != NULL && pCrcSub2_p != NULL    &&
       pSub1Param_p != NULL && pSub2Param_p != NULL )
    {
        if(isSlim_p)
        {
            /* Payload is a slim frame */
            if(payLen_p <= SLIM_FRAME_MAX_CRC8_LEN)
            {
                /* Frame has a CRC8 */
                *pCrcSub1_p = (UINT8)pPayBase_p[GET_POS_CRC8_SUB_1(payLen_p)];
                *pCrcSub2_p = (UINT8)pPayBase_p[SLIM_FRAME_SUB1_POS_CRC8 - 1];

                pSub1Param_p->pSubBase_m = &pPayBase_p[SLIM_FRAME_SUB1_POS_CRC8];
                pSub1Param_p->subLen_m = payLen_p - SLIM_FRAME_SUB1_POS_CRC8;

                pSub2Param_p->pSubBase_m = pPayBase_p;
                pSub2Param_p->subLen_m = SLIM_FRAME_SUB1_POS_CRC8;
            }
            else
            {
                /* Frame has a CRC16 */
                *pCrcSub1_p = (UINT16)pPayBase_p[GET_POS_CRC16_SUB_1(payLen_p)];
                *pCrcSub2_p = (UINT16)pPayBase_p[SLIM_FRAME_SUB1_POS_CRC16 - 1];

                pSub1Param_p->pSubBase_m = &pPayBase_p[SLIM_FRAME_SUB1_POS_CRC16];
                pSub1Param_p->subLen_m = payLen_p - SLIM_FRAME_SUB1_POS_CRC16;

                pSub2Param_p->pSubBase_m = pPayBase_p;
                pSub2Param_p->subLen_m = SLIM_FRAME_SUB1_POS_CRC16;
            }
        }
        else
        {
            /* Payload is a normal safety frame */
            if(payLen_p <= SSC_k_MAX_TEL_LEN_SHORT)
            {
                /* Frame has a CRC8 */
                *pCrcSub1_p = (UINT8)pPayBase_p[GET_POS_CRC8_SUB_1(payLen_p)];
                *pCrcSub2_p = (UINT8)pPayBase_p[GET_POS_CRC8_SUB_2(payLen_p)];
            }
            else
            {
                /* Frame has a CRC16 */
                *pCrcSub1_p = (UINT16)pPayBase_p[GET_POS_CRC16_SUB_1(payLen_p)];
                *pCrcSub2_p = (UINT16)pPayBase_p[GET_POS_CRC16_SUB_2(payLen_p)];
            }

            pSub1Param_p->pSubBase_m = &pPayBase_p[(payLen_p >> 1) + 1];
            pSub1Param_p->subLen_m = payLen_p >> 1;

            pSub2Param_p->pSubBase_m = pPayBase_p;
            pSub2Param_p->subLen_m = (payLen_p >> 1) + 1;
        }

        fReturn = TRUE;
    }
    else
    {
        errh_postFatalError(kErrSourceShnf, kErrorInvalidParameter, 0);
    }

    return fReturn;
}

/**
 * \}
 * \}
 */
