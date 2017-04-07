/**
********************************************************************************
\file   demo-sn-gpio/config/sn/sodutil.h

\brief  Header of the SN SAPL safe object dictionary (SOD) utility

This file provides additional structures for the SOD which are application
dependent. The user application needs to include this file.

*******************************************************************************/

/*------------------------------------------------------------------------------
Copyright (c) 2014, Bernecker+Rainer Industrie-Elektronik Ges.m.b.H. (B&R)
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

#ifndef _INC_sodutil_H_
#define _INC_sodutil_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/
#include <sn/global.h>

#include <SODapi.h>
#include <SPDOapi.h>

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

#define USEDCHANNELSIZE     8       /**< The total count of SPDO channels */

/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

/**
* \brief Type for storing information about used channels
*/
typedef BOOLEAN tUsedChannels[USEDCHANNELSIZE];

/**
* \brief Structure for managing the used channels information
*/
typedef struct
{
    UINT16 entries;
    tUsedChannels channel;
} tUsedChannelsInfo;

/**
* \brief Structure for the SettingGroup "GenericParameters"
*/
typedef struct
{
    UINT32 DefaultSetting01;
    UINT16 DefaultSetting02;
    UINT16 DefaultSetting03;
} tGenericParameters;

/**
 * \brief Structure of the SPDOTransport channel of SafeIN
 */
typedef struct
{
    UINT8 SafeInput01;      /**< SafeInput object data 0x6000/0x1 */
    UINT8 SafeInput02;      /**< SafeInput object data 0x6000/0x2 */
    UINT8 SafeInput03;      /**< SafeInput object data 0x6000/0x3 */
    UINT8 SafeInput04;      /**< SafeInput object data 0x6000/0x4 */
} tSPDOTransportSafeIN;

/**
 * \brief Structure of the SPDOTransport channel of SafeOUT
 */
typedef struct
{
    UINT8 SafeOutput01;     /**< SafeOutput object data 0x6200/0x1 */
    UINT8 SafeOutput02;     /**< SafeOutput object data 0x6200/0x2 */
    UINT8 SafeOutput03;     /**< SafeOutput object data 0x6200/0x3 */
    UINT8 SafeOutput04;     /**< SafeOutput object data 0x6200/0x4 */
} tSPDOTransportSafeOUT;

/*----------------------------------------------------------------------------*/
/* external variables                                                         */
/*----------------------------------------------------------------------------*/

extern tSPDOTransportSafeIN traspSafeIN_g SAFE_NO_INIT_SEKTOR;      /**< The SafeIn SPDOTransport channels */
extern tSPDOTransportSafeOUT traspSafeOUT_g SAFE_NO_INIT_SEKTOR;    /**< The SafeOUT SPDOTransport channels */

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
    extern "C" {
#endif


#ifdef __cplusplus
    }
#endif


#endif /* _INC_sodutil_H_ */
