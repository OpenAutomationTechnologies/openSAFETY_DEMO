/**
********************************************************************************
\file   demo-sn-gpio/boot/pingpong-sl.c

\defgroup module_sn_boot_pipo_sl Ping pong module (Slave side)
\{

\brief  Implements a ping pong message transfer between two processors

This module implements the a ping pong message transfer by using the
local serial device to send and receive messages. This file implements
the ping pong slave which sends a message and waits for the response

\ingroup group_app_sn_boot
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
#include <boot/internal/pingpong-sl.h>

#include <sn/upserial.h>

#include <common/platform.h>

/*============================================================================*/
/*            G L O B A L   D E F I N I T I O N S                             */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

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

/**
 * \brief Response message receive state
 */
typedef enum
{
    kRcvStateInvalid    = 0x0,      /**< Invalid state reached */
    kRcvStateSuccessful = 0x1,      /**< Receive was successful */
    kRcvStateError      = 0x2,      /**< Error on receive */
    kRcvStateRetry      = 0x3,      /**< Timeout occurred -> Retry */
} tReceiveState;

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static BOOLEAN sendPing(volatile UINT8 * pPingBase_p, UINT16 pingSize_p,
                        char * pingName_p);
static tReceiveState receivePong(volatile UINT8 * pPongBase_p, UINT16 pongSize_p,
                                 tPongRecv pfnPongRcv_p, UINT32 pongTimeout_p);

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Perform a ping pong message transfer between both processors

\param[in] pPingBase_p    Pointer to the ping base address
\param[in] pingSize_p     Size of the ping message
\param[in] pPongBase_p    Pointer to the pong base address
\param[in] pongSize_p     Size of the pong message
\param[in] pfnPongRcv_p   Callback which is called after the pong was received
\param[in] pingName_p     Name of the ping message
\param[in] pongTimeout_p  Time until a pong should arrive

\retval TRUE        Ping pong was successful
\retval FALSE       Error on transfer
*/
/*----------------------------------------------------------------------------*/
BOOLEAN pipo_doTransfer(volatile UINT8 * pPingBase_p, UINT16 pingSize_p,
                        volatile UINT8 * pPongBase_p, UINT16 pongSize_p,
                        tPongRecv pfnPongRcv_p, char * pingName_p,
                        UINT32 pongTimeout_p)
{
    BOOLEAN fReturn = FALSE;
    tReceiveState rcvState;

    if(pPingBase_p != NULL && pingSize_p > 0 &&
       pPongBase_p != NULL && pongSize_p > 0 &&
       pfnPongRcv_p != NULL)
    {
        upserial_deRegisterCb();

        /* Wait for uP-Master */
        platform_msleep(100);

        do
        {
            /* Send ping message to the other processor */
            if(sendPing(pPingBase_p, pingSize_p, pingName_p))
            {
                /* Wait for pong message */
                rcvState = receivePong(pPongBase_p, pongSize_p, pfnPongRcv_p, pongTimeout_p);
            }
            else
            {
                /* On send error -> Terminate! */
                break;
            }
        } while(rcvState == kRcvStateRetry);

        /* Set return value */
        if(rcvState == kRcvStateSuccessful)
        {
            fReturn = TRUE;
        }
    }
    else
    {
        errh_postFatalError(kErrSourcePeriph, kErrorInvalidParameter, 0);
    }

    return fReturn;
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/*----------------------------------------------------------------------------*/
/**
\brief    Send the ping message to the master processor

\param[in] pPingBase_p    Pointer to the ping base address
\param[in] pingSize_p     Size of the ping message
\param[in] pingName_p     Name of the ping message (debug only!)

\retval TRUE        Successfully sent the ping message
\retval FALSE       Error on sending
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN sendPing(volatile UINT8 * pPingBase_p, UINT16 pingSize_p,
                        char * pingName_p)
{
    BOOLEAN fReturn = FALSE;

#ifdef _DEBUG
    DEBUG_TRACE(DEBUG_LVL_ALWAYS, "\nSend %s message -> ", pingName_p);
#else
    UNUSED_PARAMETER(pingName_p);
#endif

    /* Forward the data to the serial module */
    if(upserial_transmitBlock(pPingBase_p, pingSize_p))
    {
        DEBUG_TRACE(DEBUG_LVL_ALWAYS, "SUCCESS!\n");

        fReturn = TRUE;
    }
    else
    {
        errh_postFatalError(kErrSourcePeriph, kErrorSerialTransmitFailed, 0);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Wait for the pong from the master processor

\param[in] pPongBase_p    Pointer to the pong base address
\param[in] pongSize_p     Size of the pong message
\param[in] pfnPongRcv_p   Callback which is called after the pong was received
\param[in] pongTimeout_p  Time until a pong should be retried

\return The receive state of the pong message
*/
/*----------------------------------------------------------------------------*/
static tReceiveState receivePong(volatile UINT8 * pPongBase_p, UINT16 pongSize_p,
                                 tPongRecv pfnPongRcv_p, UINT32 pongTimeout_p)
{
    tReceiveState rcvState = kRcvStateError;

    if(upserial_receiveBlock(pPongBase_p, pongSize_p, pongTimeout_p))
    {
        /* Check if the received data is correct */
        if(pfnPongRcv_p(pPongBase_p, pongSize_p))
        {
            rcvState = kRcvStateSuccessful;
        }
    }
    else
    {
        rcvState = kRcvStateRetry;
    }

    return rcvState;
}

/**
 * \}
 * \}
 */
