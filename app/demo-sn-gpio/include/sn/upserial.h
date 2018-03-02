/**
********************************************************************************
\file   demo-sn-gpio/include/sn/upserial.h

\brief  Interface to the target specific serial handler

Implements the interface to the serial which provides the connection between
the uP-Master and uP-Slave.

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

#ifndef _INC_sn_upserial_H_
#define _INC_sn_upserial_H_

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
 * \brief Transfer finished callback type
 */
typedef void (*tUpSerialTransferFin)(void);

/**
 * \brief Frame received callback type
 */
typedef void (*tUpSerialReceiveFin)(void);

/**
 * \brief Serial transfer error callback
 */
typedef void (*tUpSerialTransferError)(void);

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/
BOOLEAN upserial_init(void);
void upserial_exit(void);

void upserial_registerCb(tUpSerialTransferFin pfnTransfFin_p,
                         tUpSerialReceiveFin pfnReceivefFin_p,
                         tUpSerialTransferError pfnTransfError_p);
void upserial_deRegisterCb(void);

BOOLEAN upserial_receiveBlock(volatile UINT8 * pData_p, UINT32 size_p, UINT32 timeoutMs_p);
BOOLEAN upserial_transmitBlock(volatile UINT8 * pData_p, UINT32 size_p);

BOOLEAN upserial_enableReceive(volatile UINT8 * pData_p, UINT32 size_p);
BOOLEAN upserial_transmit(volatile UINT8 * pData_p, UINT32 size_p);

#endif /* _INC_sn_upserial_H_ */

