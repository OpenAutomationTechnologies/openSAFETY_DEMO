/**
********************************************************************************
\file   libpsi/ssdo.h

\brief  Application interface SSDO module header

This header provides all public functions or types of the ssdo module. It
is the user interface to the ssdo module.

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

#ifndef _INC_libpsi_ssdo_H_
#define _INC_libpsi_ssdo_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/

#include <libpsi/apglobal.h>

#include <libpsicommon/ssdo.h>

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

/**
 * \brief State of the transmit channel
 */
typedef enum {
    kSsdoTxStatusError       = 0x00,   /**< Error while posting to the transmit channel */
    kSsdoTxStatusSuccessful  = 0x01,   /**< Post to transmit channel successful */
    kSsdoTxStatusBusy        = 0x02,   /**< Transmit channel is currently busy */
} tSsdoTxStatus;

typedef struct eSsdoInstance *tSsdoInstance;

/**
 * \brief SSDO module frame receive handler
 */
typedef BOOL (*tSsdoRxHandler) (UINT8* pPayload_p, UINT16 size_p);

/**
 * \brief  SSDO module initialization structure
 */
typedef struct {
    tTbufNumLayout         buffIdTx_m;      /**< Id of the transmit buffer */

    tTbufNumLayout         buffIdRx_m;      /**< Id of the receive buffer */
    tSsdoRxHandler         pfnRxHandler_m;  /**< SSDO module receive handler */
} tSsdoInitParam;


/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/
DLLEXPORT tSsdoInstance ssdo_create(tSsdoChanNum chanId_m, tSsdoInitParam* pInitParam_p);
DLLEXPORT void ssdo_destroy(tSsdoInstance  pInstance_p);

DLLEXPORT BOOL ssdo_process(tSsdoInstance pInstance_p);

DLLEXPORT BOOL ssdo_getCurrentTxBuffer(tSsdoInstance pInstance_p, UINT8 ** ppPayload_p, UINT16 * pPaylLen_p);
DLLEXPORT tSsdoTxStatus ssdo_postPayload(tSsdoInstance pInstance_p, UINT8* pPayload_p,
                                           UINT16 paylSize_p);
DLLEXPORT void ssdo_receiveMsgFinished(tSsdoInstance pInstance_p);

#endif /* _INC_libpsi_ssdo_H_ */

