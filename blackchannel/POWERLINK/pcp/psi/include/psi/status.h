/**
********************************************************************************
\file   psi/status.h

\brief  Header file for status module

This file contains definitions for the status module.

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

#ifndef _INC_psi_status_H_
#define _INC_psi_status_H_

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <psi/pcpglobal.h>

#include <libpsicommon/status.h>

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------

/**
 * \brief Time information structure for processing function
 */
typedef struct {
    UINT32  relativeTimeLow_m;      ///< Relative time low value
    UINT32  relativeTimeHigh_m;     ///< Relative time high value
    UINT8   fTimeValid_m;           ///< Relative time valid
    UINT8   fCnIsOperational_m;     ///< Cn is already operational
} tTimeInfo;

/**
 * \brief Initialization parameters of the status module
 */
typedef struct {
    UINT8                      outId_m;             ///< Id of the outgoing triple buffer
    tTbufStatusOutStructure*   pOutTbufBase_m;      ///< Base address of the outgoing triple buffer
    UINT8*                     pProdAckBase_m;      ///< Producer acknowledge register base
    UINT32                     outTbufSize_m;       ///< Size of the outgoing triple buffer

    UINT8                      inId_m;             ///< Id of the incoming triple buffer
    tTbufStatusInStructure*    pInTbufBase_m;      ///< Base address of the incoming triple buffer
    UINT8*                     pConsAckBase_m;     ///< Consumer acknowledge register base
    UINT32                     inTbufSize_m;       ///< Size of the incoming triple buffer
} tStatusInitStruct;

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
tPsiStatus status_init(tStatusInitStruct* pInitParam_p);
void status_exit(void);
void status_setCycleTime(UINT32 cycleTime_p);
tPsiStatus status_resetRelTime(void);
void status_getRelativeTimeLow(UINT32* pRelTimeLow_p);
tPsiStatus status_process(tTimeInfo* pTime_p);

// Interrupt configuration functions
void status_enableSyncInt(void);
void status_disableSyncInt(void);

// Configuration channel status flag
void status_setIccStatus(tSeqNrValue seqNr_p);

// SSDO channel status flag
void status_setSsdoConsChanFlag(UINT8 chanNum_p, tSeqNrValue seqNr_p);
void status_getSsdoProdChanFlag(UINT8 chanNum_p, tSeqNrValue* pSeqNr_p);

// Logbook channel status flag
void status_setLogConsChanFlag(UINT8 chanNum_p, tSeqNrValue seqNr_p);

#endif /* _INC_psi_status_H_ */


