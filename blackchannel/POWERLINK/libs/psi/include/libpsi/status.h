/**
********************************************************************************
\file   libpsi/status.h

\brief  Application interface status module header

This header provides all public functions or types of the status module. It
is the user interface to the status module.

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

#ifndef _INC_libpsi_status_H_
#define _INC_libpsi_status_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/

#include <libpsi/apglobal.h>

#include <libpsicommon/status.h>

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

/**
 * \brief  Timestamp of the synchronous interrupt
 */
typedef struct
{
    UINT32             relTimeLow_m;
    UINT32             relTimeHigh_m;
} tPsiTimeStamp;


typedef BOOL (* tPsiAppCbSync) ( tPsiTimeStamp* pTimeStamp_p );

/**
 * \brief  Status module initialization structure
 */
typedef struct {
    tPsiAppCbSync        pfnProcSyncCb_m;    /**< Synchronization callback function */

    tTbufNumLayout       buffOutId_m;        /**< Id of the output status buffer */
    tTbufNumLayout       buffInId_m;         /**< Id of the input status buffer */
} tStatusInitParam;

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/

DLLEXPORT BOOL status_init(tStatusInitParam* pInitParam_p);
DLLEXPORT void status_exit(void);

#endif /* _INC_libpsi_status_H_ */
