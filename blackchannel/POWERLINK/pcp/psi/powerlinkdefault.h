/**
********************************************************************************
\file   powerlinkdefault.h

\brief  This header defines the default configuration of the POWERLINK stack

All parameters which are set inside this header can be overridden by using
the demo specific header in config/powerlink.h!

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

#ifndef _INC_config_powerlinkdefault_H_
#define _INC_config_powerlinkdefault_H_

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// POWERLINK CN default settings (Use powerlink.h to overload these settings)
//------------------------------------------------------------------------------
#ifndef CONFIG_ISOCHR_TX_MAX_PAYLOAD
#define CONFIG_ISOCHR_TX_MAX_PAYLOAD    max(36, sizeof(tTpdoMappedObj))
#endif

#ifndef CONFIG_ISOCHR_RX_MAX_PAYLOAD
#define CONFIG_ISOCHR_RX_MAX_PAYLOAD    1490
#endif

#ifndef CONFIG_IDENT_DEVICE_TYPE
#define CONFIG_IDENT_DEVICE_TYPE        -1              ///< Device type of the node
#endif

#ifndef CONFIG_IDENT_VENDOR_ID
#define CONFIG_IDENT_VENDOR_ID          0x00000000      ///< Vendor ID of the node
#endif

#ifndef CONFIG_IDENT_PRODUCT_CODE
#define CONFIG_IDENT_PRODUCT_CODE       0x00            ///< Product code of the node
#endif

#ifndef CONFIG_IDENT_REVISION
#define CONFIG_IDENT_REVISION           0x00010020      ///< Revision number of the node
#endif

#ifndef CONFIG_IDENT_SERIAL_NUMBER
#define CONFIG_IDENT_SERIAL_NUMBER      0x00000000      ///< Serial number of the node
#endif

#ifndef MAC_ADDR
#define MAC_ADDR    {0x00,0x12,0x34,0x56,0x78,0x9A}  ///< MAC address of the CN
#endif

#ifndef IP_ADDR
#define IP_ADDR     0xc0a86401                          ///< IP-Address 192.168.100.1 (Object: 0x1E40/0x02) (don't care the last byte!)
#endif

#ifndef SUBNET_MASK
#define SUBNET_MASK 0xFFFFFF00                          ///< Subnet mask 255.255.255.0 (Object: 0x1E40/0x03)
#endif

#ifndef DEF_GATEWAY
#define DEF_GATEWAY 0xc0a864f0                          ///< Default gateway: 192.168.100.254 (Object: 0x1E40/0x05)
#endif

//------------------------------------------------------------------------------
// Application interface interrupt settings
//------------------------------------------------------------------------------
#ifndef SYNC_INT_CYCLE_NUM
#define SYNC_INT_CYCLE_NUM          1          ///< Execute the sync interrupt in every cycle
#endif

#ifndef SYNC_INT_PULSE_WIDTH_NS
#define SYNC_INT_PULSE_WIDTH_NS     2000       ///< Pulse width of the synchronous interrupt pulse [ns]
#endif

//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

#endif /* _INC_config_powerlinkdefault_H_ */
