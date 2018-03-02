/**
********************************************************************************
\file   psi/logbook.h

\brief  Header file of the logger module

This file defines the interface to the logger module.

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

#ifndef _INC_psi_logger_H_
#define _INC_psi_logger_H_

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <psi/pcpglobal.h>

#include <libpsicommon/logbook.h>

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------

/**
 * \brief Logger channel initialization parameters
 */
typedef struct {
    tLogChanNum          chanId_m;          ///< Id of the logger channel

    UINT8                tbufTxId_m;        ///< Id of the transmit triple buffer
    tTbufLogStructure*   pTbufTxBase_m;     ///< Base address of the transmit triple buffer
    UINT32               tbufTxSize_m;      ///< Size of the transmit triple buffer
    UINT8*               pConsAckBase_m;    ///< Consumer acknowledge register base
} tLogInitStruct;

typedef struct eLogInstance *tLogInstance;

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
void log_init(UINT8 nodeId_p, UINT16 idxLogStub_p, tPsiCritSec pfnCritSec_p);
tLogInstance log_create(tLogInitStruct* pInitParam_p);
void log_destroy(tLogInstance pInstance_p);
tPsiStatus log_process(tLogInstance pInstance_p);
tPsiStatus log_setNettime(tNetTime * pNetTime_p);

tPsiStatus log_closeSdoChannel(tLogInstance pInstance_p);
tPsiStatus log_consTxTransferFinished(tLogInstance pInstance_p);
tPsiStatus log_handleIncoming(tLogInstance pInstance_p);

#endif /* _INC_psi_logger_H_ */


