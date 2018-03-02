/**
********************************************************************************
\file   demo-sn-gpio/config/tbuf/include/config/triplebuffer.h

\brief  Global header file for the triple buffers layout

This file configures the layout of the triple buffers. It assigns a meaning
to each instantiated memory.

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

#ifndef _INC_config_triplebuffer_H_
#define _INC_config_triplebuffer_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/
#include <libpsicommon/status.h>
#include <libpsicommon/cc.h>
#include <libpsicommon/ssdo.h>
#include <libpsicommon/rpdo.h>
#include <libpsicommon/tpdo.h>
#include <libpsicommon/logbook.h>

/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

typedef UINT32 tTbufAckRegister;    /**< Acknowledge register size type */

/**
 * \brief This type assigns a number to each triple buffer
 */
typedef enum {
    kTbufAckRegisterCons     = 0x00,     /**< ID of the consumer acknowledge register */
    kTbufNumStatusOut        = 0x01,     /**< ID of the status output triple buffer */
    kTbufNumRpdoImage        = 0x02,     /**< ID of the RPDO triple buffer image */
    kTbufNumSsdoReceive0     = 0x03,     /**< ID of the Ssdo0 receive buffer */
    kTbufNumStatusIn         = 0x04,     /**< ID of the status input triple buffer */
    kTbufNumTpdoImage        = 0x05,     /**< ID of the TPDO triple buffer image */
    kTbufNumSsdoTransmit0    = 0x06,     /**< ID of the Ssdo0 transmit buffer */
    kTbufNumLogbook0         = 0x07,     /**< ID of the Logger0 buffer */
    kTbufAckRegisterProd     = 0x08,     /**< ID of the producer acknowledge register */
    kTbufCount               = 0x09,     /**< Total count of triple buffers */
} tTbufNumLayout;

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

/**
 * \brief Application interface module list
 */
#define PSI_MODULE_INTEGRATION  (0 \
                                | PSI_MODULE_STATUS \
                                | PSI_MODULE_SSDO \
                                | PSI_MODULE_LOGBOOK \
                                | PSI_MODULE_PDO \
                                )

/* Detect configuration errors */
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_STATUS)) == 0)
#error "Status module is not active! This module is mandatory for the slim interface"
#endif

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/

#endif /* _INC_config_triplebuffer_H_ */

