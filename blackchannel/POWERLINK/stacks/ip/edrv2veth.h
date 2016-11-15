/**
********************************************************************************
\file   edrv2veth.h

\brief  IP stack to Ethernet driver wrapper

This module converts the driver interface of the IP stack to the interface
provided from the Virtual Ethernet driver NoOs.

*******************************************************************************/

/*------------------------------------------------------------------------------
Copyright (c) 2013, Bernecker+Rainer Industrie-Elektronik Ges.m.b.H. (B&R)
Copyright (c) 2016, Kalycito Infotech Private Ltd
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

#ifndef _INC_edrv2veth_H_
#define _INC_edrv2veth_H_

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <ip.h>

//---------------------------------------------------------------------------
// const defines
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// typedef
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// function prototypes
//---------------------------------------------------------------------------

tOplkError edrv2veth_init(eth_addr* pEthMac);
void       edrv2veth_exit(void);
void       edrv2veth_changeAddress(UINT32 ipAddr_p, UINT32 subNetMask_p, UINT16 mtu_p);
void       edrv2veth_changeGateway(UINT32 defGateway_p);
void       edrv2veth_setNmtState(tNmtState nmtState_p);
tOplkError edrv2veth_receiveHandler(UINT8* pFrame_p, UINT32 frameSize_p);
tOplkError edrv2veth_process(void);


#endif /* _INC_edrv2veth_H_ */
