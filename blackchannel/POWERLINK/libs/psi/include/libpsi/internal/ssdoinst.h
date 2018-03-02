/**
********************************************************************************
\file   libpsi/internal/ssdoinst.h

\brief  Application interface SSDO module internal instance header

Internal header for the SSDO channel which holds the instance
declaration of one channel.

*******************************************************************************/

/*------------------------------------------------------------------------------
Copyright (c) 2013, B&R Industrial Automation GmbH
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holders nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------------------*/

#ifndef _INC_libpsi_internal_ssdoinst_H_
#define _INC_libpsi_internal_ssdoinst_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/

#include <libpsicommon/ssdo.h>
#include <libpsicommon/timeout.h>

#include <libpsi/ssdo.h>

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

typedef struct {
    UINT8                   isLocked_m;        /**< Is buffer free for filling */
    tTbufSsdoTxStructure*   pSsdoTxPayl_m;    /**< Pointer to transmit buffer */
} tTbufSsdoTxBuffer;

/**
 * \brief Parameter type of the transmit buffer
 */
typedef struct {
    tTbufNumLayout        idTxBuff_m;           /**< Id of the transmit buffer */
    tTbufSsdoTxBuffer     ssdoTxBuffer_m;       /**< SSDO transmit buffer copy */
    tSeqNrValue           currTxSeqNr_m;        /**< Current transmit sequence number */
    UINT8                 currTxBuffer_m;       /**< Current active transmit buffer */
    tTimeoutInstance      pTimeoutInst_m;       /**< Timer instance for SSDO transmissions */
} tSsdoTxChannel;

/**
 * \brief Parameter type of the receive buffer
 */
typedef struct {
    tTbufNumLayout         idRxBuff_m;          /**< Id of the receive buffer */
    tSsdoRxHandler         pfnRxHandler_m;      /**< SSDO module receive handler */
    tTbufSsdoRxStructure*  pSsdoRxBuffer_m;     /**< Pointer to receive buffer */
    tSeqNrValue            currRxSeqNr_m;       /**< Current receive sequence number */
    UINT8                  fRxFrameIncoming_m;  /**< Receive buffer incoming flag */
} tSsdoRxChannel;

/**
\brief SSDO channel user instance

The SSDO instance holds configuration information of each SSDO channel.
*/
struct eSsdoInstance
{
    tSsdoChanNum       chanId_m;           /**< Id of the SSDO channel */
    tSsdoTxChannel     txBuffParam_m;      /**< Parameters of the transmit channel */
    tSsdoRxChannel     rxBuffParam_m;      /**< Parameters of the receive channel */
};

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/

#endif /* _INC_libpsi_internal_ssdoinst_H_ */
