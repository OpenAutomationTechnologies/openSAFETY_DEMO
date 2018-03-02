/**
********************************************************************************
\file   demo-sn-gpio/boot/handshake-sl.c

\defgroup module_sn_boot_hands_sl Handshake module (Slave side)
\{

\brief  Implements the handshake of uP-Master and uP-Slave

This module implements the handshake of both safe processors on the uP-Slave
side. The slave sends the welcome message to the uP-Master and waits for the
response.

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
#include <boot/handshake.h>
#include <boot/internal/handshake.h>

#include <boot/internal/pingpong-sl.h>

/*============================================================================*/
/*            G L O B A L   D E F I N I T I O N S                             */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/
#define RESPONSE_TIMEOUT_MS           (UINT32)0x2000      /**< Wait time until a timeout occurs */

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
 * \brief Handshake module instance type
 */
typedef struct
{
    volatile tWelcMsg welcMsg_m;      /**< Buffer the welcome message is stored */
    volatile tRespMsg respMsg_m;      /**< Buffer the response message is stored */
    BOOLEAN * pRestoreSod_m;         /**< Pointer to the SOD restore flag */
} tHandsInstance;

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/
static tHandsInstance handsInstance_l SAFE_INIT_SEKTOR;

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static BOOLEAN responseReceived(volatile UINT8* pRespBase_p, UINT16 respSize_p);
static BOOLEAN verifyResponseMessage(volatile tRespMsg * pRespMsg_p, BOOLEAN * pRestoreSod_p);
static void fillWelcomeMsg(BOOLEAN * pRestoreSod_p);

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Carry out the handshake between both processors

\param[inout] pRestoreSod_p     Pointer to the SOD restore flag

\retval TRUE        Handshake was successful
\retval FALSE       Error on handshake
*/
/*----------------------------------------------------------------------------*/
BOOLEAN hands_perform(BOOLEAN * pRestoreSod_p)
{
    BOOLEAN fReturn = FALSE;

    if(pRestoreSod_p != NULL)
    {
        /* Reset global variables */
        MEMSET(&handsInstance_l, 0, sizeof(tHandsInstance));

        /* Save location of restore SOD flag */
        handsInstance_l.pRestoreSod_m = pRestoreSod_p;

        /* Create the welcome message */
        fillWelcomeMsg(pRestoreSod_p);

        /* Call the ping/pong module transfer function to start the message exchange */
        if(pipo_doTransfer((volatile UINT8 *)&handsInstance_l.welcMsg_m, sizeof(tWelcMsg),
                           (volatile UINT8 *)&handsInstance_l.respMsg_m, sizeof(tRespMsg),
                           responseReceived, "welcome", RESPONSE_TIMEOUT_MS))
        {
            fReturn = TRUE;
        }   /* no else: Error is handled in the called function */
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
\brief    This function is called when the response is received

\param[in] pRespBase_p    Pointer to the SOD restore flag
\param[in] respSize_p     The size of the response message

\return The receive state of the response message
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN responseReceived(volatile UINT8* pRespBase_p, UINT16 respSize_p)
{
    BOOLEAN fReturn = FALSE;

    if((volatile UINT8*)&handsInstance_l.respMsg_m == pRespBase_p &&
       respSize_p == sizeof(tRespMsg)             )
    {
        /* Check if the received data is correct */
        if(verifyResponseMessage((volatile tRespMsg*)pRespBase_p, handsInstance_l.pRestoreSod_m))
        {
            fReturn = TRUE;
        }
        else
        {
            /* Response message is invalid */
            errh_postFatalError(kErrSourcePeriph, kHandSResponseMsgInvalid, 0);
        }
    }
    else
    {
        /* The provided receive buffer is invalid */
        errh_postFatalError(kErrSourcePeriph, kHandSReceiveBufferInvalid, 0);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Verify the response message correctness

\param[in]    pRespMsg_p           Pointer to the received response message
\param[inout] pRestoreSod_p        Pointer to the SOD restore flag

\retval TRUE    The response message is correct
\retval FALSE   Invalid response received
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN verifyResponseMessage(volatile tRespMsg * pRespMsg_p, BOOLEAN * pRestoreSod_p)
{
    BOOLEAN fMsgCorrect = FALSE;

    /* Verify the correctness of the response message header */
    if(pRespMsg_p->msgHeader_m ==  handsInstance_l.welcMsg_m.msgHeader_m)
    {
        /* Verify the SN state field and take action */
        hands_verifySnStateField(pRespMsg_p->snState_m, pRestoreSod_p);

        fMsgCorrect = TRUE;
    }

    return fMsgCorrect;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Fill the welcome message buffer

\param[inout] pRestoreSod_p     Pointer to the SOD restore flag
*/
/*----------------------------------------------------------------------------*/
static void fillWelcomeMsg(BOOLEAN * pRestoreSod_p)
{
    volatile tWelcMsg * pWelcomeMsg = &handsInstance_l.welcMsg_m;

    /* Fill the welcome message header */
    pWelcomeMsg->msgHeader_m = WELCOME_MSG_CONTENT;

    /* Set the welcome message SN state variable */
    /* Fill the SOD state field */
    hands_fillStateField(&pWelcomeMsg->snState_m, pRestoreSod_p);
}

/**
 * \}
 * \}
 */
