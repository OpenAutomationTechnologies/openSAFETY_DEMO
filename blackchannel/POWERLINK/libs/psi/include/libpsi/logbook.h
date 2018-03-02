/**
********************************************************************************
\file   libpsi/logbook.h

\brief  Application interface logbook module header

This header provides the user API for the logbook module. It provides all
public functions are types needed by the user application.

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

#ifndef _INC_libpsi_logbook_H_
#define _INC_libpsi_logbook_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/

#include <libpsi/apglobal.h>

#include <libpsicommon/logbook.h>

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
    kLogTxStatusError       = 0x00,   /**< Error while posting to the transmit channel */
    kLogTxStatusSuccessful  = 0x01,   /**< Post to transmit channel successful */
    kLogTxStatusBusy        = 0x02,   /**< Transmit channel is currently busy */
} tLogTxStatus;

typedef struct eLogInstance *tLogInstance;

/**
 * \brief  Logbook module initialization structure
 */
typedef struct {
    tTbufNumLayout         buffIdTx_m;      /**< Id of the transmit buffer */
} tLogInitParam;


/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/
DLLEXPORT tLogInstance log_create(tLogChanNum chanId_m, tLogInitParam* pInitParam_p);
DLLEXPORT void log_destroy(tLogInstance  pInstance_p);

DLLEXPORT BOOL log_process(tLogInstance pInstance_p);

DLLEXPORT BOOL log_getCurrentLogBuffer(tLogInstance pInstance_p, tLogFormat ** ppLogData_p);
DLLEXPORT tLogTxStatus log_postLogEntry(tLogInstance pInstance_p, tLogFormat* pLogData_p);

#endif /* _INC_libpsi_logbook_H_ */

