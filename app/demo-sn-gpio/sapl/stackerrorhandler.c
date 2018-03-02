/**
********************************************************************************
\file   demo-sn-gpio/sapl/stackerrorhandler.c

\defgroup module_sn_sapl_errh Error handler module
\{

\brief  This module implements the error handler of the openSAFETY stack.

Implements the error handler of the openSAFETY stack and forwards error
information.

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

#include <sn/global.h>
#include <sn/errorhandler.h>

#include <SCFMapi.h>
#include <SODapi.h>
#include <SERRapi.h>

/*============================================================================*/
/*            G L O B A L   D E F I N I T I O N S                             */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/
#define EPS_ERROR_UNIT_MASK         0x0F00      /**< Mask the unit bits of the error code */
#define EPS_ERROR_UNIT_OFFSET       8           /**< Offset of the unit bits */

/*----------------------------------------------------------------------------*/
/* module global vars                                                         */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* global function prototypes                                                 */
/*----------------------------------------------------------------------------*/


/*============================================================================*/
/*            P R I V A T E   D E F I N I T I O N S                           */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* local types                                                                */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/**
 * @brief This function is a callback function which is provided by the openSAFETY Application.
 *
 * The function is called by the openSAFETY Stack (unit SNMTS) in case of reception of SNMT Service "SNMT SN ack".
 * This callback function passes the acknowledge on a reported error to the SN's application.
 *
 * @param        b_instNum           instance number, valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        b_errorGroup        error group see {fail error group} (not checked) valid range: any 8 bit value
 *
 * @param        b_errorCode         error code see {fail error code} (not checked) valid range: any 8 bit value
 */
void SAPL_SNMTS_ErrorAckClbk(BYTE_B_INSTNUM_ UINT8 b_errorGroup,
                             UINT8 b_errorCode)
{
    /* to avoid PC-lint Info */
#if (EPLS_cfg_MAX_INSTANCES > 1)
    UNUSED_PARAMETER(b_instNum);
#endif

    UNUSED_PARAMETER(b_errorCode);
    UNUSED_PARAMETER(b_errorGroup);

    DEBUG_TRACE(DEBUG_LVL_ERROR, "\n\nSNMT SN ack received:\n");
    DEBUG_TRACE(DEBUG_LVL_ERROR, "    Instance number = %u\n", B_INSTNUMidx);
    DEBUG_TRACE(DEBUG_LVL_ERROR, "    Error group = 0x%X\n", b_errorGroup);
    DEBUG_TRACE(DEBUG_LVL_ERROR, "    Error code = 0x%X\n\n", b_errorCode);
}

/**
 * @brief This function is a callback function which is provided by the EPLS Application. The function is called by the EPLS Stack (unit SERR) to signal an internal error of the EPLS Stack.
 *
 * @param        b_instNum          instance number w_errorCode error class, unit in which the error occured and the error
 * @param       w_errorCode         16 bit value that holds Error Class, Error ID and the error itselves (checked), valid range: any 16 bit value
 *
 * @param        dw_addInfo         additional error information
 */
void SAPL_SERR_SignalErrorClbk(BYTE_B_INSTNUM_ UINT16 w_errorCode,
                               UINT32 dw_addInfo)
{
    tErrorDesc errDesc;

#if (EPLS_cfg_MAX_INSTANCES > 1)
    UNUSED_PARAMETER(b_instNum);
#endif

    /* Reformat error information */
    errDesc.source_m = kErrSourceStack;
    errDesc.class_m = SERR_GET_ERROR_CLASS(w_errorCode);

    if(SERR_GET_ERROR_TYPE(w_errorCode) == SERR_TYPE_FS)
        errDesc.fFailSafe_m = TRUE;
    else
        errDesc.fFailSafe_m = FALSE;

    errDesc.unit_m = ((w_errorCode & EPS_ERROR_UNIT_MASK) >> EPS_ERROR_UNIT_OFFSET);
    errDesc.code_m = (UINT8)w_errorCode;
    errDesc.addInfo_m = dw_addInfo;

    /* Forward error to global error handler */
    errh_postError(&errDesc);
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/**
 * \}
 * \}
 */
