/**
********************************************************************************
\file   demo-sn-gpio/config/tbuf/include/config/ssdo.h

\brief  This header provides the configuration parameters of the ssdo module

This header gives the basic structure of the SSDO receive and
transmit buffers.

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

#ifndef _INC_config_ssdo_H_
#define _INC_config_ssdo_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/

#include <libpsicommon/global.h>

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/
#define SSDO_STUB_OBJECT_INDEX           0x2110     /**< Object index of the SSDO stub (Container needs to be defined in xdd) */
#define SSDO_STUB_DATA_OBJECT_INDEX      0x2130     /**< Object index of the SSDO stub data (Container needs to be defined in xdd) */


#define SSDO_STUB_DATA_DOM_SIZE     0x20      /**< Size of the SSDO stub data object */
#define TSSDO_TRANSMIT_DATA_SIZE    0x20      /**< Size of the SSDO channel transmit data */

/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

/**
 * \brief Number of SSDO channel instances
 */
typedef enum {
    kNumSsdoChan0     = 0x00,
    kNumSsdoInstCount = 0x01,
} tSsdoChanNum;

/*----------------------------------------------------------------------------*/
/* offsetof defines                                                           */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/

#endif /* _INC_config_ssdo_H_ */

