/**
********************************************************************************
\file   demo-sn-gpio/config/sn/sod.c

\defgroup module_sn_sapl_sod Safe object dictionary (SOD) module
\{

\brief  Safe object dictionary (SOD)

This file implements the safe object dictionary SOD. In addition it provides
all memory structures and the initialization data of each object.

\ingroup group_app_sn_sapl
*******************************************************************************/

/*------------------------------------------------------------------------------
* License Agreement
*
* Copyright (c) 2014, B&R Industrial Automation GmbH
* All rights reserved.
*
* Redistribution and use in source and binary forms,
* with or without modification,
* are permitted provided that the following conditions are met:
*
*   * Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer
*     in the documentation and/or other materials provided with the
*     distribution.
*   * Neither the name of the B&R nor the names of its contributors
*     may be used to endorse or promote products derived from this software
*     without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/
#include <sod.h>

#include <SODapi.h>
#include <SPDOapi.h>
#include <SERRapi.h>

#if (EPLS_cfg_SCM == EPLS_k_ENABLE)
  #include <SCMapi.h>
#endif


/*----------------------------------------------------------------------------*/
/* global variables                                                           */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* static constants, types, macros, variables                                 */
/*----------------------------------------------------------------------------*/

/**
 * \brief This symbol is used to map the SOD into RAM.
 */
#define k_RAM

/**
 * \brief This symbol is used to map the SOD into ROM.
 */
#define k_ROM

/**
 * \brief This symbol defines that the SOD is mapped into RAM or ROM
 */
#define SOD_cfg_TO_RAM_ROM k_RAM

/**
 * \brief Type definition to set range for the unsigned 8-bit objects
 */
typedef struct
{
  UINT8 b_low;  /* low limit */
  UINT8 b_high; /* high limit */
} t_U8_RANGE;

/**
 * \brief Type definition to set range for the unsigned 16-bit objects
 */
typedef struct
{
  UINT16 w_low;  /* low limit */
  UINT16 w_high; /* high limit */
} t_U16_RANGE;


/**
 * \brief Type definition to set range for the unsigned 32-bit objects
 */
typedef struct
{
  UINT32 dw_low;  /* low limit */
  UINT32 dw_high; /* high limit */
} t_U32_RANGE;

/* Defines to increase the readability of the SOD */
#define CONS SOD_k_ATTR_RO_CONST     /**< constant object */
#define RO SOD_k_ATTR_RO             /**< read only object */
#define WO SOD_k_ATTR_WO             /**< write only object */
#define RW SOD_k_ATTR_RW             /**< readable and writable object */
#define CRC SOD_k_ATTR_CRC           /**< CRC object */
#define PDO SOD_k_ATTR_PDO_MAP       /**< mappable object */
#define NLEN SOD_k_ATTR_NO_LEN_CHK   /**< the length checking at the SPDO mapping is deactivated */

#if (EPLS_cfg_MAX_INSTANCES > 1)
#define SHR SOD_k_ATTR_SHARED        /**< shared object */
#else /* (EPLS_cfg_MAX_INSTANCES > 1) */
#define SHR
#endif /* (EPLS_cfg_MAX_INSTANCES > 1) */

/* Defines to increase the readability of the SOD */
#define BEF_RD SOD_k_ATTR_BEF_RD      /**< Before read callback function is called for the object */
#define BEF_WR SOD_k_ATTR_BEF_WR      /**< Before write callback function is called for the object */
#define AFT_WR SOD_k_ATTR_AFT_WR      /**< After write callback function is called for the object */

/* Defines to increase the readability of the SOD */
#define U8 EPLS_k_UINT8         /**< unsigned 8-bit object */
#define U16 EPLS_k_UINT16       /**< unsigned 16-bit object */
#define U32 EPLS_k_UINT32       /**< unsigned 32-bit object */
#define I8 EPLS_k_INT8          /**< signed 8-bit object */
#define I16 EPLS_k_INT16        /**< signed 16-bit object */
#define I32 EPLS_k_INT32        /**< signed 32-bit object */
#define OCT EPLS_k_OCTET_STRING /**< octet string object */
#define DOM EPLS_k_DOMAIN       /**< domain object */

/* Variable definition for the number of entries in the object dictionary.
   _noE_x : number of entries = x */
static UINT8 b_noE_2 SAFE_INIT_SEKTOR = 2;      /**< number of entries = 2 */
static UINT8 b_noE_3 SAFE_INIT_SEKTOR = 3;      /**< number of entries = 3 */
static UINT8 b_noE_4 SAFE_INIT_SEKTOR = 4;      /**< number of entries = 4 */
static UINT8 b_noE_7 SAFE_INIT_SEKTOR = 7;      /**< number of entries = 7 */
static UINT8 b_noE_12 SAFE_INIT_SEKTOR = 12;    /**< number of entries = 12 */
static UINT8 b_noE_13 SAFE_INIT_SEKTOR = 13;    /**< number of entries = 13 */

/* Variable definition to set the range in the object dictionary.
   _rg_type_min_max  : range checking structure (range : min <= data <= max) */
static t_U8_RANGE  s_rg_b_1_63 = {1,63};
static t_U8_RANGE  s_rg_b_1_255 = {1,255};
static t_U16_RANGE s_rg_w_0_1023 = {0,1023};
static t_U16_RANGE s_rg_w_1_1023 = {1,1023};
static t_U16_RANGE s_rg_w_1_32767 = {1,32767};
static t_U16_RANGE s_rg_w_0_65535 = {0,65535};
static t_U16_RANGE s_rg_w_1_65535 = {1,65535};
static t_U32_RANGE s_rg_dw_1_65535 = {1,65535};
#define PROP_DELAY_RANGE s_rg_w_1_65535
static t_U8_RANGE s_rg_b_0_3 = {0, 3};
static t_U8_RANGE s_rg_b_0_4 = {0, 4};

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Default value and actual value variable definition for the object          */
/* dictionary of the instance 0                                               */
/*                                                                            */
/* _0_    : instance 0                                                        */
/* _def_  : default value                                                     */
/* _xxxx_ : xxxx [16 bit] index                                               */
/* _xx_   : xx [8 bit] sub-index                                              */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/* This structure stores the actual values of instance 0 */
static tSodObjectData s_0_act_general SAFE_NO_INIT_SEKTOR;

/* 0x100C Life Guarding */
static UINT32 dw_0_def_100C_01  SAFE_INIT_SEKTOR = 10000000;
static UINT8 b_0_def_100C_02   SAFE_INIT_SEKTOR = 2;

/* 0x100D Pre-Operational signal */
static UINT32 dw_0_def_100D_00  SAFE_INIT_SEKTOR = 100000;
static UINT8 b_0_def_100E_00   SAFE_INIT_SEKTOR = 5;

/* 0x1018 Device Vendor Information */
static UINT32 dw_0_def_1018_01 SAFE_INIT_SEKTOR = CFG_SAPL_SN_VENDORID;        /**< Vendor ID */
static UINT32 dw_0_def_1018_02 SAFE_INIT_SEKTOR = CFG_SAPL_SN_PRODUCT_CODE;    /**< Product Code */
static UINT32 dw_0_def_1018_03 SAFE_INIT_SEKTOR = CFG_SAPL_SN_REVISION_NR;     /**< Revision Number */
static UINT32 dw_0_def_1018_04 SAFE_INIT_SEKTOR = CFG_SAPL_SN_SERIAL_NR;       /**< Serial Number */
static UINT32 dw_0_def_1018_05 SAFE_INIT_SEKTOR = CFG_SAPL_SN_FW_CHKSUM;       /**< Firmware Checksum */

/* Begin of the parameter checksum definition */
/* length of the parameter checksum domain */
#define k_LEN_PARAM_CHKSUM_DOM sizeof(tParamChksum)

static SOD_t_ACT_LEN_PTR_DATA s_0_act_1018_06 = { k_LEN_PARAM_CHKSUM_DOM,
                                                  &s_0_act_general.devVendInfo.aParamCrcs };
static tParamChksum s_0_def_1018_06 SAFE_INIT_SEKTOR = { 0UL,
                                                         {0UL} };
/* End of the parameter checksum definition */

static UINT32 dw_0_def_1018_07 SAFE_INIT_SEKTOR = 0x0;     /**< Parameter Timestamp */

/* 0x1019 Unique Device ID */
static SOD_t_ACT_LEN_PTR_DATA s_0_act_1019_00 SAFE_INIT_SEKTOR = { EPLS_k_UDID_LEN,
                                                                   &s_0_act_general.devVendInfo.aucUdid };
static UINT8 ab_0_def_1019_00[EPLS_k_UDID_LEN] SAFE_INIT_SEKTOR = CFG_SAPL_SN_UDID;

/* 0x101A Parameter download */
static UINT8 ab_0_act_101A_00[SAPL_k_MAX_PARAM_SET_LEN] SAFE_NO_INIT_SEKTOR;
static SOD_t_ACT_LEN_PTR_DATA s_0_act_101A_00 SAFE_INIT_SEKTOR = { 0U,                       /* actual length */
                                                                   &ab_0_act_101A_00[0] };   /* pointer to the object data */

/* 0x1200 Common Communication Parameter */
static UINT16 w_0_def_1200_01 SAFE_INIT_SEKTOR = 1;
static UINT16 w_0_def_1200_02 SAFE_INIT_SEKTOR = 0x01;
static INT8 c_0_def_1200_03 SAFE_INIT_SEKTOR = 2;                   /**< Consecutive time base */
                                                                    /* 0 : 1us */
                                                                    /* 1 : 10us */
                                                                    /* 2 : 100us */
                                                                    /* 3 : 1000us */

static UINT8 ab_0_def_1200_04[EPLS_k_UDID_LEN] SAFE_INIT_SEKTOR = CFG_SAPL_SN_UDID;
static SOD_t_ACT_LEN_PTR_DATA s_0_act_1200_04 SAFE_INIT_SEKTOR = { EPLS_k_UDID_LEN,
                                                                   &s_0_act_general.commonComParam.aucUdidScm[0]};

/* 0x1C00-0x1FFE RxSPDO Communication Parameter */
static UINT16 w_0_def_1400_01 SAFE_INIT_SEKTOR = 0x0000;
static UINT32 dw_0_def_1400_02 SAFE_INIT_SEKTOR = 0x00000001UL;
static UINT8 b_0_def_1400_03 SAFE_INIT_SEKTOR = 0x01;
static UINT32 dw_0_def_1400_04 SAFE_INIT_SEKTOR = 0x00000000UL;
static UINT32 dw_0_def_1400_05 SAFE_INIT_SEKTOR = 0x00000001UL;
static UINT16 w_0_def_1400_06 SAFE_INIT_SEKTOR = 0x0001;
static UINT16 w_0_def_1400_07 SAFE_INIT_SEKTOR = 0x0001;
static UINT16 w_0_def_1400_08 SAFE_INIT_SEKTOR = 0x0001;
static UINT16 w_0_def_1400_09 SAFE_INIT_SEKTOR = 0x0001;
static UINT32 w_0_def_1400_0A SAFE_INIT_SEKTOR = 0x0000;
static UINT32 dw_0_def_1400_0B SAFE_INIT_SEKTOR = 0x00000001UL;
static UINT16 w_0_def_1400_0C SAFE_INIT_SEKTOR = 0x01;

/* 0x1800-0x1BFE RxSPDO Mapping Parameter */
static UINT8 b_0_def_1800_00 SAFE_INIT_SEKTOR = 0x00;
static UINT32 dw_0_def_1800_01 SAFE_INIT_SEKTOR = 0x00000000UL;

/* 0x1C00-0x1FFE TxSPDO Communication Parameter */
static UINT16 w_0_def_1C00_01 SAFE_INIT_SEKTOR = 0x0000;
static UINT16 w_0_def_1C00_02 SAFE_INIT_SEKTOR = 1;
static UINT8 b_0_def_1C00_03 SAFE_INIT_SEKTOR = 0x00;

/* 0x2001 Parameters */
static tUsedChannelsInfo usedChannels = {0, { 0 }};
static SOD_t_ACT_LEN_PTR_DATA SOD_UsedChannels SAFE_INIT_SEKTOR = { sizeof(tUsedChannels), &usedChannels.channel[0] };

/* 0x4000 Settings */
static tGenericParameters manSettings = {0,0,0};
static UINT32 dw_0_def_4000_01 SAFE_INIT_SEKTOR = 0x0000;
static UINT16 w_0_def_4000_02 SAFE_INIT_SEKTOR = 0x0000;
static UINT16 w_0_def_4000_03 SAFE_INIT_SEKTOR = 0x0000;

/* 0x6200 SPDO transmit Parameters */
static UINT8 b_0_def_6200_01 = 0x00;
static UINT8 b_0_def_6200_02 = 0x00;
static UINT8 b_0_def_6200_03 = 0x00;
static UINT8 b_0_def_6200_04 = 0x00;

/* 0xC000-0xC3FE TxSPDO Mapping Parameter */
static UINT8 b_0_def_C000_00  SAFE_INIT_SEKTOR = 0x00;
static UINT32 dw_0_def_C000_01 SAFE_INIT_SEKTOR = 0x00000000UL;

/*----------------------------------------------------------------------------*/
/* MFW SOD variables                                                          */
/*----------------------------------------------------------------------------*/

/**
    Object dictionary for intance 0. E.g. for one object entry:
    see {SOD_t_OBJECT}
    1   : Object index
    2   : Object sub-index
    3-5 : see {SOD_t_ATTR}
      3 : attributes, see {SOD-Attributes}
      4 : datatype of the object
      5 : maximum length of the object
      6 : reference to the default value
    7   : reference to object data
    8   : reference to min-/max values
    9   : reference to a callback function
   |  1   |  2  |  3  |  4  |   5   |  6   |  7  |  8  |  9  |
   {0x0000,0x00,{ 0x00, INT8, 0x00UL, NULL}, NULL, NULL, NULL},
*/
SOD_cfg_TO_RAM_ROM SOD_t_OBJECT SAPL_s_OD_INST_0[]=
{
    /* Error Register */
    {0x1001, 0x00, {RO , U8 ,    0x1UL,    NULL }, &s_0_act_general.errRegister.ucErrorRegister , NULL , SOD_k_NO_CALLBACK},

    /* Telegram error statistic counter */
    {0x1004,    0x00,   {CONS , U8 ,  0x1UL,  &b_noE_13 },  &b_noE_13                                     , NULL, SOD_k_NO_CALLBACK},
    {0x1004,    0x01,   {RO   , U32,  0x4UL,  NULL      },  &SERR_aadwCommonEvtCtr[0][SERR_k_SFS_LENGTH]  , NULL, SOD_k_NO_CALLBACK},
    {0x1004,    0x02,   {RO   , U32,  0x4UL,  NULL      },  &SERR_aadwCommonEvtCtr[0][SERR_k_SFS_TOO_LONG], NULL, SOD_k_NO_CALLBACK},
    {0x1004,    0x03,   {RO   , U32,  0x4UL,  NULL      },  &SERR_aadwCommonEvtCtr[0][SERR_k_SFS_FRM_ID]  , NULL, SOD_k_NO_CALLBACK},
    {0x1004,    0x04,   {RO   , U32,  0x4UL,  NULL      },  &SERR_aadwCommonEvtCtr[0][SERR_k_SFS_SADR_INV], NULL, SOD_k_NO_CALLBACK},
    {0x1004,    0x05,   {RO   , U32,  0x4UL,  NULL      },  &SERR_aadwCommonEvtCtr[0][SERR_k_SFS_SDN_INV] , NULL, SOD_k_NO_CALLBACK},
    {0x1004,    0x06,   {RO   , U32,  0x4UL,  NULL      },  &SERR_aadwCommonEvtCtr[0][SERR_k_SFS_TADR_INV], NULL, SOD_k_NO_CALLBACK},
    {0x1004,    0x07,   {RO   , U32,  0x4UL,  NULL      },  &SERR_aadwCommonEvtCtr[0][SERR_k_SFS_CRC1]    , NULL, SOD_k_NO_CALLBACK},
    {0x1004,    0x08,   {RO   , U32,  0x4UL,  NULL      },  &SERR_aadwCommonEvtCtr[0][SERR_k_SFS_CRC2]    , NULL, SOD_k_NO_CALLBACK},
    {0x1004,    0x09,   {RO   , U32,  0x4UL,  NULL      },  &SERR_aadwCommonEvtCtr[0][SERR_k_SFS_DATA]    , NULL, SOD_k_NO_CALLBACK},
    {0x1004,    0x0A,   {RO   , U32,  0x4UL,  NULL      },  &SERR_aadwCommonEvtCtr[0][SERR_k_CYC_REJECT]  , NULL, SOD_k_NO_CALLBACK},
    {0x1004,    0x0B,   {RO   , U32,  0x4UL,  NULL      },  &SERR_aadwCommonEvtCtr[0][SERR_k_CYC_ERROR]   , NULL, SOD_k_NO_CALLBACK},
    {0x1004,    0x0C,   {RO   , U32,  0x4UL,  NULL      },  &SERR_aadwCommonEvtCtr[0][SERR_k_ACYC_REJECT] , NULL, SOD_k_NO_CALLBACK},
    {0x1004,    0x0D,   {RO   , U32,  0x4UL,  NULL      },  &SERR_aadwCommonEvtCtr[0][SERR_k_ACYC_RETRY]  , NULL, SOD_k_NO_CALLBACK},

    /* Life Guarding */
    {0x100C, 0x00, {CONS , U8 , 0x1UL, &b_noE_2}, &b_noE_2 , NULL , SOD_k_NO_CALLBACK},
    {0x100C, 0x01, {RW | CRC , U32, 0x4UL, &dw_0_def_100C_01 }, &s_0_act_general.lifeGuard.ulGuardTime , NULL , SOD_k_NO_CALLBACK},
    {0x100C, 0x02, {RW | CRC , U8 , 0x1UL, &b_0_def_100C_02 }, &s_0_act_general.lifeGuard.ucLifeTimeFactor , &s_rg_b_1_255 , SOD_k_NO_CALLBACK},

    /* Pre-Operational signal */
    {0x100D, 0x00, {RW , U32, 0x4UL, &dw_0_def_100D_00 }, &s_0_act_general.numRetriesRG.ulRefreshPreOp , NULL , SOD_k_NO_CALLBACK},
    {0x100E, 0x00, {RW , U8 , 0x1UL, &b_0_def_100E_00 }, &s_0_act_general.refreshIntRG.ucNoRetries , NULL , SOD_k_NO_CALLBACK},

    /* Device Vendor Information */
    {0x1018, 0x00, {CONS , U8 , 0x1UL, &b_noE_7 },    &b_noE_7 , NULL, SOD_k_NO_CALLBACK},
    {0x1018, 0x01, {RO , U32, 0x4UL, &dw_0_def_1018_01 }, &s_0_act_general.devVendInfo.ulVendorId, NULL , SOD_k_NO_CALLBACK},
    {0x1018, 0x02, {RO , U32, 0x4UL, &dw_0_def_1018_02 }, &s_0_act_general.devVendInfo.ulProductCode, NULL , SOD_k_NO_CALLBACK},
    {0x1018, 0x03, {RO , U32, 0x4UL, &dw_0_def_1018_03 }, &s_0_act_general.devVendInfo.ulRevisionNumber, NULL , SOD_k_NO_CALLBACK},
    {0x1018, 0x04, {RO , U32, 0x4UL, &dw_0_def_1018_04 }, &s_0_act_general.devVendInfo.ulSerialNumber, NULL , SOD_k_NO_CALLBACK},
    {0x1018, 0x05, {RO , U32, 0x4UL, &dw_0_def_1018_05 }, &s_0_act_general.devVendInfo.ulFirmwareCrc, NULL , SOD_k_NO_CALLBACK},
    {0x1018, 0x06, {RW , DOM, k_LEN_PARAM_CHKSUM_DOM, &s_0_def_1018_06 }, &s_0_act_1018_06, NULL , SOD_k_NO_CALLBACK},
    {0x1018, 0x07, {RO , U32, 0x4UL, &dw_0_def_1018_07 }, &s_0_act_general.devVendInfo.aulParamTimestamp[0], NULL , SOD_k_NO_CALLBACK},

    /* Unique Device ID */
    {0x1019, 0x00, {CONS , OCT, EPLS_k_UDID_LEN, ab_0_def_1019_00 }, &s_0_act_1019_00, NULL, SOD_k_NO_CALLBACK},

    /* Parameter download */
    {0x101A, 0x00, {WO | AFT_WR , DOM, SAPL_k_MAX_PARAM_SET_LEN, NULL }, &s_0_act_101A_00 , NULL, SAPL_SOD_ParameterSet_CLBK},

    /* Common Communication Parameter */
    {0x1200, 0x00, {CONS , U8 , 0x1UL, &b_noE_4 }, &b_noE_4 , NULL , SOD_k_NO_CALLBACK},
    {0x1200, 0x01, {RO , U16, 0x2UL, &w_0_def_1200_01 }, &s_0_act_general.commonComParam.usSdn, &s_rg_w_0_1023, SOD_k_NO_CALLBACK},
    {0x1200, 0x02, {RO , U16, 0x2UL, &w_0_def_1200_02 }, &s_0_act_general.commonComParam.usSadrOfScm, &s_rg_w_0_1023, SOD_k_NO_CALLBACK},
    {0x1200, 0x03, {RW | CRC | AFT_WR , I8 , 0x1UL, &c_0_def_1200_03 }, &s_0_act_general.commonComParam.cCtb, &s_rg_b_0_3, SHNF_SOD_ConsTimeBase_CLBK},
    {0x1200, 0x04, {RW , OCT , EPLS_k_UDID_LEN, ab_0_def_1200_04}, &s_0_act_1200_04, NULL, SOD_k_NO_CALLBACK},

    /* RxSPDO communication parameters */
    {0x1400, 0x00, {CONS , U8 , 0x1UL, &b_noE_12 }, &b_noE_12 , NULL , NULL},
    {0x1400, 0x01, {RW | CRC , U16, 0x2UL, &w_0_def_1400_01 }, &s_0_act_general.aRxComParam[0].usSadr, &s_rg_w_0_1023, NULL},
    {0x1400, 0x02, {RW | CRC , U32, 0x4UL, &dw_0_def_1400_02 }, &s_0_act_general.aRxComParam[0].ulSct, &s_rg_dw_1_65535, NULL},
    {0x1400, 0x03, {RW | CRC , U8 , 0x1UL, &b_0_def_1400_03 }, &s_0_act_general.aRxComParam[0].ucNoConsecTReq, &s_rg_b_1_63, NULL},
    {0x1400, 0x04, {RW | CRC , U32, 0x4UL, &dw_0_def_1400_04 }, &s_0_act_general.aRxComParam[0].ulTimeDelayTReq, NULL, NULL},
    {0x1400, 0x05, {RW | CRC , U32, 0x4UL, &dw_0_def_1400_05 }, &s_0_act_general.aRxComParam[0].ulTimeDelaySync, NULL, NULL},
    {0x1400, 0x06, {RW | CRC , U16, 0x2UL, &w_0_def_1400_06 }, &s_0_act_general.aRxComParam[0].usMinTSyncPropDelay, &PROP_DELAY_RANGE, NULL},
    {0x1400, 0x07, {RW | CRC , U16, 0x2UL, &w_0_def_1400_07 }, &s_0_act_general.aRxComParam[0].usMaxTSyncPropDelay, &s_rg_w_1_65535, NULL},
    {0x1400, 0x08, {RW | CRC , U16, 0x2UL, &w_0_def_1400_08 }, &s_0_act_general.aRxComParam[0].usMinSpdoPropDelay, &PROP_DELAY_RANGE, NULL},
    {0x1400, 0x09, {RW | CRC , U16, 0x2UL, &w_0_def_1400_09 }, &s_0_act_general.aRxComParam[0].usMaxSpdoPropDelay, &s_rg_w_1_65535, NULL},
    {0x1400, 0x0A, {RW | CRC , U16, 0x2UL, &w_0_def_1400_0A }, &s_0_act_general.aRxComParam[0].usBestCaseTresDelay, &s_rg_w_0_65535, NULL},
    {0x1400, 0x0B, {RW | CRC , U32, 0x4UL, &dw_0_def_1400_0B }, &s_0_act_general.aRxComParam[0].ulTReqCycle, NULL, NULL},
    {0x1400, 0x0C, {RW | CRC , U16, 0x2UL, &w_0_def_1400_0C }, &s_0_act_general.aRxComParam[0].usTxSpdoNo, &s_rg_w_1_1023, NULL},


    /* RxSPDO mapping Parameter */
    {0x1800, 0x00, {RW | CRC | BEF_WR, U8, 0x1UL, &b_0_def_1800_00}, &s_0_act_general.aRxMapParam[0].ucNoMappingEntries, &s_rg_b_0_4, SPDO_SOD_RxMappPara_CLBK},
    {0x1800, 0x01, {RW | CRC | BEF_WR, U32, 0x4UL, &dw_0_def_1800_01}, &s_0_act_general.aRxMapParam[0].aulMappingEntry[0], NULL, SPDO_SOD_RxMappPara_CLBK},
    {0x1800, 0x02, {RW | CRC | BEF_WR, U32, 0x4UL, &dw_0_def_1800_01}, &s_0_act_general.aRxMapParam[0].aulMappingEntry[1], NULL, SPDO_SOD_RxMappPara_CLBK},
    {0x1800, 0x03, {RW | CRC | BEF_WR, U32, 0x4UL, &dw_0_def_1800_01}, &s_0_act_general.aRxMapParam[0].aulMappingEntry[2], NULL, SPDO_SOD_RxMappPara_CLBK},
    {0x1800, 0x04, {RW | CRC | BEF_WR, U32, 0x4UL, &dw_0_def_1800_01}, &s_0_act_general.aRxMapParam[0].aulMappingEntry[3], NULL, SPDO_SOD_RxMappPara_CLBK},

    /* TxSPDO communication parameters */
    {0x1C00, 0x00, {CONS , U8 , 0x1UL, &b_noE_3 }, &b_noE_3 , NULL , SOD_k_NO_CALLBACK},
    {0x1C00, 0x01, {RW | CRC , U16, 0x2UL, &w_0_def_1C00_01 }, &s_0_act_general.aTxComParam[0].usSadr , &s_rg_w_0_1023 , SOD_k_NO_CALLBACK},
    {0x1C00, 0x02, {RW | CRC , U16, 0x2UL, &w_0_def_1C00_02 }, &s_0_act_general.aTxComParam[0].usRefreshPrescale , &s_rg_w_1_32767 , SOD_k_NO_CALLBACK},
    {0x1C00, 0x03, {RW | CRC , U8 , 0x1UL, &b_0_def_1C00_03 }, &s_0_act_general.aTxComParam[0].ucNoTRes , NULL , SOD_k_NO_CALLBACK},

    /* vendor/module specific data */
    {0x2001, 0x00, {RW | CRC , DOM, sizeof(tUsedChannels), NULL }, &SOD_UsedChannels, NULL, SOD_k_NO_CALLBACK},

    {0x4000, 0x00, {CONS, U8, 0x1UL, &b_noE_3}, &b_noE_3, NULL, SOD_k_NO_CALLBACK},
    {0x4000, 0x01, {RW | CRC , U32, 0x4UL, &dw_0_def_4000_01}, &manSettings.DefaultSetting01, NULL, SOD_k_NO_CALLBACK},
    {0x4000, 0x02, {RW | CRC , U16, 0x2UL, &w_0_def_4000_02}, &manSettings.DefaultSetting02, NULL, SOD_k_NO_CALLBACK},
    {0x4000, 0x03, {RW | CRC , U16, 0x2UL, &w_0_def_4000_03}, &manSettings.DefaultSetting03, NULL, SOD_k_NO_CALLBACK},


    /* Input data */
    {0x6000, 0x00, {CONS, U8, 0x1UL, &b_noE_4}, &b_noE_4, NULL, SOD_k_NO_CALLBACK},
    {0x6000, 0x01, {RO    | PDO, U8, 0x1UL, NULL}, &traspSafeIN_g.SafeInput01, NULL, SOD_k_NO_CALLBACK},
    {0x6000, 0x02, {RO    | PDO, U8, 0x1UL, NULL}, &traspSafeIN_g.SafeInput02, NULL, SOD_k_NO_CALLBACK},
    {0x6000, 0x03, {RO    | PDO, U8, 0x1UL, NULL}, &traspSafeIN_g.SafeInput03, NULL, SOD_k_NO_CALLBACK},
    {0x6000, 0x04, {RO    | PDO, U8, 0x1UL, NULL}, &traspSafeIN_g.SafeInput04, NULL, SOD_k_NO_CALLBACK},

    /* Output data */
    {0x6200, 0x00, {CONS, U8, 0x1UL, &b_noE_4}, &b_noE_4, NULL, SOD_k_NO_CALLBACK},
    {0x6200, 0x01, {RW    | PDO, U8, 0x1UL, &b_0_def_6200_01}, &traspSafeOUT_g.SafeOutput01, NULL, SOD_k_NO_CALLBACK},
    {0x6200, 0x02, {RW    | PDO, U8, 0x1UL, &b_0_def_6200_02}, &traspSafeOUT_g.SafeOutput02, NULL, SOD_k_NO_CALLBACK},
    {0x6200, 0x03, {RW    | PDO, U8, 0x1UL, &b_0_def_6200_03}, &traspSafeOUT_g.SafeOutput03, NULL, SOD_k_NO_CALLBACK},
    {0x6200, 0x04, {RW    | PDO, U8, 0x1UL, &b_0_def_6200_04}, &traspSafeOUT_g.SafeOutput04, NULL, SOD_k_NO_CALLBACK},

    /* TxSPDO mapping Parameter */
    {0xC000, 0x00, {RW | CRC | BEF_WR, U8, 0x1UL, &b_0_def_C000_00}, &s_0_act_general.aTxMapParam[0].ucNoMappingEntries, &s_rg_b_0_4, SPDO_SOD_TxMappPara_CLBK},
    {0xC000, 0x01, {RW | CRC | BEF_WR, U32, 0x4UL, &dw_0_def_C000_01}, &s_0_act_general.aTxMapParam[0].aulMappingEntry[0], NULL, SPDO_SOD_TxMappPara_CLBK},
    {0xC000, 0x02, {RW | CRC | BEF_WR, U32, 0x4UL, &dw_0_def_C000_01}, &s_0_act_general.aTxMapParam[0].aulMappingEntry[1], NULL, SPDO_SOD_TxMappPara_CLBK},
    {0xC000, 0x03, {RW | CRC | BEF_WR, U32, 0x4UL, &dw_0_def_C000_01}, &s_0_act_general.aTxMapParam[0].aulMappingEntry[2], NULL, SPDO_SOD_TxMappPara_CLBK},
    {0xC000, 0x04, {RW | CRC | BEF_WR, U32, 0x4UL, &dw_0_def_C000_01}, &s_0_act_general.aTxMapParam[0].aulMappingEntry[3], NULL, SPDO_SOD_TxMappPara_CLBK},

    /* end of SOD */
    {SOD_k_END_OF_THE_OD, 0xFF, {0, EPLS_k_BOOLEAN, 0x1UL, 0}, NULL, NULL, SOD_k_NO_CALLBACK}
};

/* global structure for all object dictionaries */
SOD_cfg_TO_RAM_ROM SOD_t_OBJECT *SAPL_ps_OD[EPLS_cfg_MAX_INSTANCES]=
{
    SAPL_s_OD_INST_0
};

SOD_t_ENTRY_VIRT SAPL_s_SOD_VIRT_INST =
{
    {0, EPLS_k_BOOLEAN, 0x1UL, NULL}, SOD_k_END_OF_THE_OD, SOD_k_END_OF_THE_OD, 0xFF, 0xFF, NULL, NULL, (SOD_t_CLBK)0
};


/* dummy for virtual SOD */
const SOD_t_ENTRY_VIRT *const SAPL_ps_SOD_VIRT[EPLS_cfg_MAX_INSTANCES] =
{
    &SAPL_s_SOD_VIRT_INST
};

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/


/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/**
 * \}
 * \}
 */

