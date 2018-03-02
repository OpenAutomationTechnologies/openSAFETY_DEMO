/**
********************************************************************************
\file   demo-sn-gpio/shnf/include/shnf/xcomint.h

\brief  Internal header of the cross communication module

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

#ifndef _INC_shnf_xcomint_H_
#define _INC_shnf_xcomint_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/
#include <sn/global.h>

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/
#define MSG_FORMAT_SPDO_SET     0       /**< Id of the SPDO bit set */
#define MSG_FORMAT_SSDO_SET     1       /**< Id of the SSDO/SNMT bit set */

#define ID_VAL_MASL_MSG         (UINT8)0xAA    /**< ID value of the up-Master -> uP-Slave image */
#define ID_VAL_SLMA_MSG         (UINT8)0x55    /**< ID value of the up-Slave -> uP-Master image */

/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

/**
 * \brief Determines the type of payload to return
 */
typedef enum
{
    kPaylTypeInvalid    = 0x0,      /**< Invalid type */
    kPaylTypeReceive    = 0x1,      /**< Get the received payload */
    kPaylTypeTransmit   = 0x2,      /**< Get the transmit payload */
} tPaylType;

/**
 * \brief Provides the structure of the uP-Master -> uP-Slave transmit image
 */
typedef struct
{
    UINT8 id_m;             /**< The unique id of this message (0xAA) */
    UINT8 msgFormat_m;      /**< The format of this message */
    UINT32 flowCnt_m;       /**< The value of the flow counter */
    UINT64 currTime_m;      /**< The current timebase of the uP-Master in us */
    UINT16 spdoSub1Crc_m;   /**< The CRC value of TSPDO sub1 */
    UINT16 spdoSub2Crc_m;   /**< The CRC value of TSPDO sub2 */
    UINT16 ssdoSub1Crc_m;   /**< The CRC value of TSSDO/SNMT sub1 */
    UINT16 ssdoSub2Crc_m;   /**< The CRC value of TSSDO/SNMT sub2 */
} tXComMaSlImage;

/**
 * \brief Provides the structure of the uP-Slave -> uP-Master transmit image
 */
typedef struct
{
    UINT8 id_m;                                 /**< The unique id of this message (0x55) */
    UINT8 msgFormat_m;                          /**< The format of this message */
    UINT32 flowCnt_m;                           /**< The value of the flow counter */
    UINT64 currTime_m;                          /**< The current timebase of the uP-Slave in us */
    UINT16 spdoSub1Crc_m;                       /**< The CRC value of TSPDO sub1 */
    UINT16 spdoSub2Crc_m;                       /**< The CRC value of TSPDO sub2 */
    UINT8 spdoSub2Payl_m[TSPDO_SUB2_LEN];       /**< The payload of TSPDO sub2 */
    UINT16 ssdoSub1Crc_m;                       /**< The CRC value of TSSDO/TSNMT sub1 */
    UINT16 ssdoSub2Crc_m;                       /**< The CRC value of TSSDO/TSNMT sub2 */
    UINT8 ssdoSub2Payl_m[TSSDO_SNMT_SUB2_LEN];  /**< The payload of TSSDO/TSNMT sub2 */
} tXComSlMaImage;

/**
 * \brief The xcom module initialization parameters
 */
typedef struct
{
    UINT8 * pTxImg_m;       /**< Pointer to the transmit image base address*/
    UINT16 txImgSize_m;     /**< The size of the transmit image */
    UINT8 * pRxImg_m;       /**< Pointer to the receive image base address*/
    UINT16 rxImgSize_m;     /**< The size of the receive image */
} tXComTransParams;


/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
    extern "C" {
#endif

BOOLEAN xcomint_init(tXComTransParams * pTransParam_p);
void xcomint_exit(void);

BOOLEAN xcomint_verifyIdValue(void);

BOOLEAN xcomint_getMsgFormat(tPaylType paylType_p, UINT8 * pMsgForm_p);
BOOLEAN xcomint_setMsgFormat(tPaylType paylType_p, UINT8 msgForm_p);

void xcomint_setFlowCount(UINT32 flowCount_p);
BOOLEAN xcomint_getFlowCnt(tPaylType paylType_p, UINT32 * pFlowCount_p);

BOOLEAN xcomint_getTimeBase(tPaylType paylType_p, UINT64 * pRetTime);
void xcomint_setTransTimebase(UINT64 * pCurrTime_p);

void xcomint_setSsdoSnmtCrc(UINT16 crcSub1_p, UINT16 crcSub2_p);
BOOLEAN xcomint_getSsdoSnmtCrc(tPaylType paylType_p, UINT16 * pSub1Crc_p,
                                                     UINT16 * pSub2Crc_p);
BOOLEAN xcomint_postSsdoSnmtFrame(tSubFrameParams * pSub1Params_p,
                                  tSubFrameParams * pSub2Params_p,
                                  UINT8 * pTargBuff_p, UINT32 targLen_p);
BOOLEAN xcomint_handleSsdoSnmtPayload(void);

void xcomint_setSpdoCrc(UINT16 crcSub1_p, UINT16 crcSub2_p);
BOOLEAN xcomint_getSpdoCrc(tPaylType paylType_p, UINT16 * pSub1Crc_p,
                                                 UINT16 * pSub2Crc_p);
BOOLEAN xcomint_postSpdoFrame(tSubFrameParams * pSub1Params_p,
                              tSubFrameParams * pSub2Params_p,
                              UINT8 * pTargBuff_p, UINT32 targLen_p);
BOOLEAN xcomint_handleSpdoPayload(void);

#ifdef __cplusplus
    }
#endif


#endif /* _INC_shnf_xcomint_H_ */
