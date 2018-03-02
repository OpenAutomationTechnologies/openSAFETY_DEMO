/**
********************************************************************************
\file   demo-sn-gpio/config/tbuf/include/config/tpdo.h

\brief  This header defines the configurable parameters of the tpdo module

This header gives the basic structure of the transmit pdo triple buffers.

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

#ifndef _INC_config_tpdo_H_
#define _INC_config_tpdo_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/

#include <libpsicommon/global.h>

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

#define TPDO_NUM_OBJECTS    1       /**< Number of mapped TPDO objects */

#define TX_SPDO_SIZE       32       /**< Size of the spdo0 transmit container */

/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/
/**
 * \brief List of all mappable objects
 */
typedef struct {
    UINT8          spdo0[TX_SPDO_SIZE];       /**< SPDO transmit container */
} PACK_STRUCT tTpdoMappedObj;

/*----------------------------------------------------------------------------*/
/* offsetof defines                                                           */
/*----------------------------------------------------------------------------*/
#define TBUF_TPDO_SPDO0_OFF         offsetof(tTpdoMappedObj, spdo0)

/*----------------------------------------------------------------------------*/
/* object linking parameters                                                  */
/*----------------------------------------------------------------------------*/

/* Link between object and buffer address: objIdx | objSubIdx | addressOffset            | objSize */
#define TPDO_LINKING_LIST_INIT_VECTOR   { {0x4000 , 0x01      , TBUF_TPDO_SPDO0_OFF      , TX_SPDO_SIZE  }  \
                                        }

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/

#endif /* _INC_config_tpdo_H_ */

