/**
********************************************************************************
\file   demo-sn-gpio/config/pcp/powerlink.h

\brief  This header defines some configurable parameters of the POWERLINk stack

These defines override the standard configuration of the POWERLINK CN on the
PCP and enable to pass a different configuration to the POWERLINK processor.

*******************************************************************************/

/*------------------------------------------------------------------------------
Copyright (c) 2017, Bernecker+Rainer Industrie-Elektronik Ges.m.b.H. (B&R)
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

#ifndef _INC_config_powerlink_H_
#define _INC_config_powerlink_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

#define CONFIG_IDENT_DEVICE_TYPE        0x00000000      /**< Device type of the node */
#define CONFIG_IDENT_VENDOR_ID          0x00000000      /**< Vendor ID of the node */
#define CONFIG_IDENT_PRODUCT_CODE       0x00000001      /**< Product code of the node */
#define CONFIG_IDENT_REVISION           0x00010020      /**< Revision number of the node */
#define CONFIG_IDENT_SERIAL_NUMBER      0x00000000      /**< Serial number of the node */


#define MAC_VENDOR_ID   0x12,0x34,0x56                      /**< Vendor specific part of the MAC address */
#define MAC_ADDR        {MAC_VENDOR_ID,0x56,0x78,0x9A}      /**< MAC address of the CN */
#define IP_ADDR         0xc0a86401                          /**< IP-Address 192.168.100.1 (Object: 0x1E40/0x02) (don't care the last byte!) */
#define SUBNET_MASK     0xFFFFFF00                          /**< Subnet mask 255.255.255.0 (Object: 0x1E40/0x03) */
#define DEF_GATEWAY     0xc0a864f0                          /**< Default gateway: 192.168.100.254 (Object: 0x1E40/0x05) */

/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/

#endif /* _INC_config_powerlink_H_ */
