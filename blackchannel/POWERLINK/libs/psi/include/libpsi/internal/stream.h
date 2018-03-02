/**
********************************************************************************
\file   libpsi/internal/stream.h

\brief  Streaming module internal header for streamed transfer of input/output data

This module handles the transfer of the input of output buffers via the steam
handler. It enables to insert pre- and post actions before and after transfer.

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

#ifndef _INC_libpsi_intenal_stream_H_
#define _INC_libpsi_intenal_stream_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/

#include <libpsi/apglobal.h>
#include <libpsi/stream.h>

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/
#define PDO_CHANNEL_DEACTIVATED     0xFF    /**< Id of an deactivated PDO channel */

/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

/**
 * \brief  Buffer action types -> Before or after filling!
 */
typedef enum {
    kStreamActionInvalid  = 0x00,
    kStreamActionPre      = 0x01,
    kStreamActionPost     = 0x02,
} tActionType;

/**
 * \brief Callback for stream buffer action. Called to inform module about new data.
 */
typedef BOOL (*tBuffAction) (UINT8* pBuffer_p, UINT16 bufSize_p,
        void * pUserArg_p);

/**
 * \brief Synchronous user callback function. Informs user about new data.
 */
typedef BOOL (*tBuffSyncCb)(void);


/**
 * \brief  Stream module initialization structure
 */
typedef struct {
    tBuffDescriptor* pBuffDescList_m;      /**< Triple buffer descriptor list */
    tTbufNumLayout   idConsAck_m;          /**< Id of the consumer ack register */
    tTbufNumLayout   idFirstProdBuffer_m;  /**< Id of the first producing buffer */
    tStreamHandler   pfnStreamHandler_m;   /**< Stream receive and transmit handler */
} tStreamInitParam;

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/
BOOL stream_init(tStreamInitParam* pInitParam_p);
void stream_exit(void);

tBuffDescriptor* stream_getBufferParam(tTbufNumLayout buffId_p);
BOOL stream_registerAction(tActionType actType_p, UINT8 buffId_p,
        tBuffAction pfnBuffAct_p, void * pUserArg_p);
void stream_registerSyncCb(tBuffSyncCb pfnSyncCb_p);
BOOL stream_processSync(void);
BOOL stream_processPostActions(void);

#endif /* _INC_libpsi_intenal_stream_H_ */
