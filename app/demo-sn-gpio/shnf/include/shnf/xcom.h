/**
********************************************************************************
\file   demo-sn-gpio/shnf/include/shnf/xcom.h

\brief  This header provides the interface to the cross communication module

*******************************************************************************/

/*------------------------------------------------------------------------------
Copyright (c) 2014, B&R Industrial Automation GmbH
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

#ifndef _INC_shnf_xcom_H_
#define _INC_shnf_xcom_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/
#include <sn/global.h>

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/
#define TSPDO_SUB2_LEN          0x10       /**< Length of the SPDO transmit frame (TODO: Exchange this define with a global header) */
#define TSSDO_SNMT_SUB2_LEN     0x10       /**< Length of the SSDO/SNMT transmit frame (TODO: Exchange this define with a global header) */

#define MSG_FORMAT_SPDO_SET     0       /**< Id of the SPDO bit set */
#define MSG_FORMAT_SSDO_SET     1       /**< Id of the SSDO/SNMT bit set */

/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

/**
 * \brief Defines the sub frame parameters
 */
typedef struct
{
    UINT8 * pSubBase_m;     /**< Pointer to the base address of the subframe */
    UINT32 subLen_m;        /**< The length of the subframe */
} tSubFrameParams;

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
    extern "C" {
#endif

BOOLEAN xcom_init(void);
void xcom_exit(void);

BOOLEAN xcom_setCurrentTimebase(UINT64 * p_currTime);
BOOLEAN xcom_enableReceiveCheck(void);

void xcom_setSsdoSnmtCrc(UINT16 crcSub1_p, UINT16 crcSub2_p);
BOOLEAN xcom_postSsdoSnmtFrame(tSubFrameParams * pSub1Params_p,
                               tSubFrameParams * pSub2Params_p,
                               UINT8 * pTargBuff_p, UINT32 targLen_p);

void xcom_setSpdoCrc(UINT16 crcSub1_p, UINT16 crcSub2_p);
BOOLEAN xcom_postSpdoFrame(tSubFrameParams * pSub1Params_p,
                           tSubFrameParams * pSub2Params_p,
                           UINT8 * pTargBuff_p, UINT32 targLen_p);

BOOLEAN xcom_transmit(UINT32 flowCount_p);

#ifdef __cplusplus
    }
#endif


#endif /* _INC_shnf_xcom_H_ */
