/**
********************************************************************************
\file   libpsicommon/timeout.h

\brief  Module header file timeout generation

This module generates a timeout for asynchronous transmissions by simply counting
the synchronous interrupts. If a limit is reached the timeout is generated.

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

#ifndef _INC_psicommon_timeout_H_
#define _INC_psicommon_timeout_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/

#include <libpsicommon/global.h>

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

/**
 * \brief Timer module instance type
 */
typedef struct eTimeoutInstance *tTimeoutInstance;

/**
 * \brief Current state of the timer
 */
typedef enum {
    kTimerStateInvalid   = 0x00,    /**< Invalid timer state */
    kTimerStateRunning   = 0x01,    /**< Timer is currently running */
    kTimerStateExpired   = 0x02,    /**< Timer is expired */
    kTimerStateStopped   = 0x03,    /**< Timer is currently stopped */
} tTimerStatus;

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/
DLLEXPORT void timeout_init(void);
DLLEXPORT tTimeoutInstance timeout_create(UINT16 cycleLimit_p);
DLLEXPORT void timeout_destroy(tTimeoutInstance pInstance_p);

DLLEXPORT tTimerStatus timeout_checkExpire(tTimeoutInstance pInstance_p);
DLLEXPORT void timeout_incrementCounter(tTimeoutInstance pInstance_p);
DLLEXPORT void timeout_startTimer(tTimeoutInstance pInstance_p);
DLLEXPORT void timeout_stopTimer(tTimeoutInstance pInstance_p);
DLLEXPORT tTimerStatus timeout_isRunning(tTimeoutInstance pInstance_p);

#endif /* _INC_psicommon_timeout_H_ */


