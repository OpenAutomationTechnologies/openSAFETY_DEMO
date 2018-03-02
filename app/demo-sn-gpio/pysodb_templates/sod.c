/*[[[cog
from pysodb.codegeneration.gensodc import init
cg = init()
cg.init()
]]]*/
/*[[[end]]]*/
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
/*[[[cog cg.number_of_entries_definitions() ]]]*/
/*[[[end]]]*/

/* Variable definition to set the range in the object dictionary.
   _rg_type_min_max  : range checking structure (range : min <= data <= max) */
/*[[[cog cg.range_definitions() ]]]*/
/*[[[end]]]*/
#define PROP_DELAY_RANGE s_rg_w_1_65535


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


/*[[[cog cg.default_values_definitions() ]]]*/
/*[[[end]]]*/

/* Begin of the parameter checksum definition */
/* length of the parameter checksum domain */
#define k_LEN_PARAM_CHKSUM_DOM sizeof(tParamChksum)

/* End of the parameter checksum definition */

/*[[[cog cg.paramstream_settings_definition() ]]]*/
/*[[[end]]]*/
/*[[[cog cg.sodpos_settings_definition() ]]]*/
/*[[[end]]]*/

static tUsedChannelsInfo usedChannels = {0, { 0 }};

/*[[[cog cg.act_data_definitions() ]]]*/
/*[[[end]]]*/


/*----------------------------------------------------------------------------*/
/* MFW SOD variables                                                          */
/*----------------------------------------------------------------------------*/

/**
    Object dictionary for intance 0. E.g. for one object entry:
    see {SOD_t_OBJECT}
    1   : Object index
    2   : Object sub-index
    3-6 : see {SOD_t_ATTR}
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
    /*[[[cog cg.make_sod_entries() ]]]*/
    /*[[[end]]]*/

};

/* global structure for all object dictionaries */
SOD_cfg_TO_RAM_ROM SOD_t_OBJECT *SAPL_ps_OD[EPLS_cfg_MAX_INSTANCES]=
{
    SAPL_s_OD_INST_0
};

/* dummy for virtual SOD */
const SOD_t_ENTRY_VIRT *const SAPL_ps_SOD_VIRT[EPLS_cfg_MAX_INSTANCES] =
{
    NULL
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

