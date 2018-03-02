/**
********************************************************************************
\file   demo-sn-gpio/shnf/include/shnf/hnf.h

\brief  Hardware near firmware interface to the grey channel

*******************************************************************************/

/*------------------------------------------------------------------------------
Copyright (c) 2014, B&R Industrial Automation GmbH
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

#ifndef _INC_shnf_hnf_H_
#define _INC_shnf_hnf_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/
#include <sn/global.h>

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

typedef BOOLEAN (*tAsyncRxHandler)(UINT8 * pPayload_p, UINT16 paylLen_p);
typedef void (*tSyncRxHandler)(UINT8 * pPayload_p, UINT16 paylLen_p);
typedef void (*tSyncTxCreate)(void);

/**
 * \brief Type of the synchronous process function
 */
typedef BOOLEAN (*tProcSync)(void);

/**
 * \brief Module initialization parameters
 */
typedef struct
{
    tAsyncRxHandler asyncRcvChan0Handler_m;       /**< Asynchronous receive handler */
    tSyncRxHandler syncRcvHandler_m;              /**< Synchronous receive handler */
    tSyncTxCreate syncTxBuild_m;                  /**< Trigger the build of synchronous transmit frames */
    tProcSync pfnProcSync_m;                      /**< Process the synchronous task at the end of the cycle */
    tSyncCycle pfnSyncronize_m;                   /**< Callback to synchronize to the current cycle (Beginning of the cycle) */
} tHnfInit;

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
    extern "C" {
#endif

BOOLEAN hnf_init(tHnfInit * pHnfInit_p);
void hnf_exit(void);

BOOLEAN hnf_processAsync(void);

/* Access functions of the asynchronous channels */
BOOLEAN hnf_postAsyncTxChannel0(const UINT8 * pPayload_p, UINT16 paylLen_p);
BOOLEAN hnf_getAsyncTxBufferChannel0(UINT8 ** ppTxBuffer_p, UINT16 * pBuffLen_p);
void hnf_finishedAsyncRxChannel0(void);

/* Access functions of the logbook channels */
BOOLEAN hnf_postLogChannel0(tErrorDesc * pErrDesc_p);

/* Access functions of the synchronous channels */
BOOLEAN hnf_postSyncTx(const UINT8 * pPayload_p, UINT16 paylLen_p);
BOOLEAN hnf_getSyncTxBuffer(UINT8 ** ppTxBuffer_p, UINT16 * pBuffLen_p);

void hnf_enableSyncIr(void);

#ifdef __cplusplus
    }
#endif


#endif /* _INC_shnf_shnf_H_ */
