/**
********************************************************************************
\file   libpsi/internal/logbookinst.h

\brief  Application interface logbook module internal instance header

Internal header for the logbook channel which holds the instance
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

#ifndef _INC_libpsi_internal_logbookinst_H_
#define _INC_libpsi_internal_logbookinst_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/

#include <libpsicommon/logbook.h>
#include <libpsicommon/timeout.h>

#include <libpsi/logbook.h>

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

typedef struct {
    UINT8                   isLocked_m;        /**< Is buffer free for filling */
    tTbufLogStructure*      pLogTxPayl_m;      /**< Pointer to transmit buffer */
} tTbufLogBuffer;

/**
\brief Logbook channel user instance

The logbook instance holds configuration information of each logbook channel.
*/
struct eLogInstance
{
    tLogChanNum           chanId_m;           /**< Id of the logbook channel */

    tTbufNumLayout        idTxBuff_m;           /**< Id of the transmit buffer */
    tTbufLogBuffer        logTxBuffer_m;        /**< Logbook transmit buffer copy */
    tSeqNrValue           currTxSeqNr_m;        /**< Current transmit sequence number */
    UINT8                 currTxBuffer_m;       /**< Current active transmit buffer */
    tTimeoutInstance      pTimeoutInst_m;       /**< Timer instance for a logbook transmissions */
};

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/

#endif /* _INC_libpsi_internal_logbookinst_H_ */


