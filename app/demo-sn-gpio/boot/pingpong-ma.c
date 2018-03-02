/**
********************************************************************************
\file   demo-sn-gpio/boot/pingpong-ma.c

\defgroup module_sn_boot_pipo_ma Ping pong module (Master side)
\{

\brief  Implements a ping pong message transfer between two processors

This module implements the a ping pong message transfer by using the
local serial device to send and receive messages. This file implements
the ping pong master which waits for an incoming message and sends a response.

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
#include <boot/internal/pingpong-ma.h>

#include <sn/upserial.h>

/*============================================================================*/
/*            G L O B A L   D E F I N I T I O N S                             */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/
#define PING_RCV_TIMEOUT_MS  0xFFFFF     /**< Receive timeout of the incomming message */

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
static BOOLEAN receivePing(volatile UINT8 * pPingBase_p, UINT16 pingSize_p,
                           tPingRecv pfnPingRcv_p, char * pingName_p);
static BOOLEAN sendPong(volatile UINT8 * pTargBase_p, UINT16 targSize_p);

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
\param[in] pfnPingRcv_p   Callback which is called after the ping was received
\param[in] pingName_p     Name of the ping message (debug only)

\retval TRUE        Ping pong was successful
\retval FALSE       Error on transfer
*/
/*----------------------------------------------------------------------------*/
BOOLEAN pipo_doTransfer(volatile UINT8 * pPingBase_p, UINT16 pingSize_p,
                        volatile UINT8 * pPongBase_p, UINT16 pongSize_p,
                        tPingRecv pfnPingRcv_p, char * pingName_p)
{
    BOOLEAN fReturn = FALSE;

    if(pPingBase_p != NULL && pingSize_p > 0 &&
       pPongBase_p != NULL && pongSize_p > 0 &&
       pfnPingRcv_p != NULL && pingName_p != NULL)
    {
        upserial_deRegisterCb();

        /* Receive the ping from the other processor */
        if(receivePing(pPingBase_p, pingSize_p, pfnPingRcv_p, pingName_p))
        {
            /* Send a pong back to the other processor */
            if(sendPong(pPongBase_p, pongSize_p))
            {
                fReturn = TRUE;
            }
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
\brief    Receive the ping from the slave processor

\param[in] pPingBase_p    Pointer to the ping base address
\param[in] pingSize_p     Size of the ping message
\param[in] pfnPingRcv_p   Pointer to the ping received callback function
\param[in] pingName_p     Name of the ping message (debug only)

\retval TRUE        Successfully received the ping message
\retval FALSE       Error on receiving
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN receivePing(volatile UINT8 * pPingBase_p, UINT16 pingSize_p,
                           tPingRecv pfnPingRcv_p, char * pingName_p)
{
    BOOLEAN fReturn = FALSE;

#ifdef _DEBUG
    DEBUG_TRACE(DEBUG_LVL_ALWAYS, "\nWait for %s message -> ", pingName_p);
#else
    UNUSED_PARAMETER(pingName_p);
#endif

    /* Enable receive channel */
    if(upserial_receiveBlock(pPingBase_p, pingSize_p, PING_RCV_TIMEOUT_MS))
    {
        DEBUG_TRACE(DEBUG_LVL_ALWAYS, "SUCCESS!\n");

        /* Forward the received data to the upper module */
        if(pfnPingRcv_p(pPingBase_p, pingSize_p))
        {
            fReturn = TRUE;
        }
    }
    else
    {
        errh_postFatalError(kErrSourcePeriph, kErrorSerialReceiveFailed, 0);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Send a response to uP-Slave

\param[in] pPongBase_p    Pointer to the pong base address
\param[in] pongSize_p     Size of the pong message

\retval TRUE        Response sent to the slave processor
\retval FALSE       Error on sending
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN sendPong(volatile UINT8 * pPongBase_p, UINT16 pongSize_p)
{
    BOOLEAN fReturn = FALSE;

    /* Forward the data to the serial module */
    if(upserial_transmitBlock(pPongBase_p, pongSize_p))
    {
        fReturn = TRUE;
    }
    else
    {
        errh_postFatalError(kErrSourcePeriph, kErrorSerialTransmitFailed, 0);
    }

    return fReturn;
}

/**
 * \}
 * \}
 */
