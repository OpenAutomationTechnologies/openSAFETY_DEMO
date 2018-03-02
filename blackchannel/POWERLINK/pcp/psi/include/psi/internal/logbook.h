/**
********************************************************************************
\file   psi/internal/logbook.h

\brief  Internal header file of the SSDO module

This file contains internal definitions for the SSDO module.

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

#ifndef _INC_psi_int_logger_H_
#define _INC_psi_int_logger_H_

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <psi/pcpglobal.h>

#include <psi/tbuf.h>

#include <libpsicommon/timeout.h>
#include <config/logbook.h>

#include <oplk/oplk.h>

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------

/**
 * \brief Defines the format of a BuR logbock entry
 */
typedef struct
{
    UINT8 formatId_m;           ///< 2 for this format
    UINT32 entryNumber_m;       ///< counting up one on each new entry
    UINT64 timeStamp_m;         ///< absolute time stamp in ms since 1.1.1970 00:00
    UINT16 errCode_m;           ///< unique error code display as 0x10000 + errorCode
    UINT16 errInfo1_m;          ///< additional info
    UINT32 errInfo2_m;          ///< additional info
    UINT8 level_m;              ///< optional level
} PACK_STRUCT tBuRLogEntry;

/**
 * \brief State machine type for the consuming transmit buffer
 */
typedef enum {
    kConsTxStateInvalid                     = 0x00,
    kConsTxStateWaitForFrame                = 0x01,
    kConsTxStateProcessFrame                = 0x02,
    kConsTxStateWaitForTxFinished           = 0x03,
    kConsTxStateWaitForNextArpRetry         = 0x04,
    kConsTxStateRetransmitCurrentMessage    = 0x05,
    kConsTxStateTxFinished                  = 0x05,
} tConsTxState;

/**
\brief Logger channel user instance

The logger instance holds configuration information of each logger channel.
*/
struct eLogInstance
{
    tLogChanNum       instId_m;             ///< Id of the logger instance
    tTbufInstance     pTbufConsTxInst_m;    ///< Instance pointer to the consuming transmit triple buffer
    tSeqNrValue       currConsSeq_m;        ///< Consuming buffer sequence number
    tConsTxState      consTxState_m;        ///< State of the consuming transmit buffer
    tSdoComConHdl     sdoComConHdl_m;       ///< SDO connection handler
    tTimeoutInstance  pArpTimeoutInst_m;    ///< Timer for ARP request retry
    tBuRLogEntry      burLogEntry_m;        ///< The logbook entry converted into the B&R format
    UINT32            entryCount_m;         ///< The current logbook entry count
};

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

#endif /* _INC_psi_int_logger_H_ */


