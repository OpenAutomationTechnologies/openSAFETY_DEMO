/**
********************************************************************************
\file   sn/statehandler.h

\brief  This module handles the current SN state

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

#ifndef _INC_sn_statehandler_H_
#define _INC_sn_statehandler_H_

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

/**
 * \brief Represents the current state of the SN
 */
typedef enum
{
    kSnStateBooting        = 0x0,       /**< Default the SN is in state booting */
    kSnStateInitializing   = 0x1,       /**< SN is currently initializing */
    kSnStatePreOperational = 0x2,       /**< SN is in pre-operational state */
    kSnStateOperational    = 0x3,       /**< SN is operational */
    kSnStateCount          = 0x4,       /**< The count of SN states */
} tSnState;

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
    extern "C" {
#endif

BOOLEAN stateh_init(tSnState snInitState_p);
void stateh_exit(void);

void stateh_setSnState(tSnState newState_p);
tSnState stateh_getSnState(void);

void stateh_setEnterOpFlag(BOOLEAN newVal_p);
BOOLEAN stateh_getEnterOpFlag(void);

void stateh_setEnterPreOpFlag(BOOLEAN newVal_p);
BOOLEAN stateh_getEnterPreOpFlag(void);

void stateh_setShutdownFlag(BOOLEAN newVal_p);
BOOLEAN stateh_getShutdownFlag(void);

void stateh_printSNState(void);

BOOLEAN stateh_handleStateChange(void);
BOOLEAN stateh_enterPreOperational(void);


#ifdef __cplusplus
    }
#endif


#endif /* _INC_sn_statehandler_H_ */
