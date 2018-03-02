/**
********************************************************************************
\file   libpsicommon/status.h

\brief  Header defines the layout of the status triple buffer

This header gives the basic structure of the status triple buffers.

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

#ifndef _INC_psicommon_status_H_
#define _INC_psicommon_status_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/

#include <libpsicommon/global.h>

/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

/**
 * \brief Status channel outgoing buffer layout
 */
typedef struct {
    UINT32 relTimeLow_m;
    UINT32 relTimeHigh_m;
    UINT8  iccStatus_m;
    UINT8  logConsStatus_m;
    UINT16 ssdoConsStatus_m;
} PACK_STRUCT tTbufStatusOutStructure;

/**
 * \brief Status channel incoming buffer layout
 */
typedef struct {
    UINT16 reserved_m;
    UINT16 ssdoProdStatus_m;
} PACK_STRUCT tTbufStatusInStructure;

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

#define TBUF_RELTIME_LOW_OFF            offsetof(tTbufStatusOutStructure, relTimeLow_m)
#define TBUF_RELTIME_HIGH_OFF           offsetof(tTbufStatusOutStructure, relTimeHigh_m)
#define TBUF_ICC_STATUS_OFF             offsetof(tTbufStatusOutStructure, iccStatus_m)
#define TBUF_LOG_CONS_STATUS_OFF        offsetof(tTbufStatusOutStructure, logConsStatus_m)
#define TBUF_SSDO_CONS_STATUS_OFF       offsetof(tTbufStatusOutStructure, ssdoConsStatus_m)

#define TBUF_SSDO_PROD_STATUS_OFF       offsetof(tTbufStatusInStructure, ssdoProdStatus_m)


#define STATUS_ICC_BUSY_FLAG_POS        0       /**< Position of the ICC busy flag */
#define STATUS_ICC_ERROR_FLAG_POS       1       /**< Position of the ICC error flag (TODO) */

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/

#endif /* _INC_psicommon_status_H_ */

