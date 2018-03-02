/**
********************************************************************************
\file   demo-sn-gpio/boot/sync-sl.c

\defgroup module_sn_boot_sync_sl Synchronization module (Slave side)
\{

\brief  Implements the synchronization of uP-Master and uP-Slave

This module implements the synchronization of both safe processors on the uP-Slave
side. It sends out the ready message and waits for the sync message to arrive.
In addition it overtakes the current time of the sync timebase.

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
#include <boot/sync.h>
#include <boot/internal/sync.h>

#include <boot/internal/pingpong-sl.h>

#include <shnf/constime.h>

/*============================================================================*/
/*            G L O B A L   D E F I N I T I O N S                             */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/
#define SYNC_TIMEOUT_MS           (UINT32)0xFFFFF      /**< Wait time until a timeout occurs */

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
 * \brief Synchronization module instance type
 */
typedef struct
{
    volatile tReadyMsg readyMsg_m;    /**< Buffer the ready message is stored */
    volatile tSyncMsg syncMsg_m;      /**< Buffer the synchronization message is stored */
} tSyncInstance;

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/
static tSyncInstance syncInstance_l SAFE_INIT_SEKTOR;

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static BOOLEAN syncReceived(volatile UINT8* pSyncBase_p, UINT16 syncSize_p);
static BOOLEAN verifySyncMessage(volatile tSyncMsg * pSyncMsg_p);

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Carry out the synchronization between both processors

\retval TRUE        Synchronization was successful
\retval FALSE       Error on sync
*/
/*----------------------------------------------------------------------------*/
BOOLEAN sync_perform(void)
{
    BOOLEAN fReturn = FALSE;

    /* Reset global variables */
    MEMSET(&syncInstance_l, 0, sizeof(tSyncInstance));

    /* Create the ready message */
    syncInstance_l.readyMsg_m.msgHeader_m = READY_MSG_CONTENT;

    /* Call the ping/pong module transfer function to start the message exchange */
    if(pipo_doTransfer((volatile UINT8 *)&syncInstance_l.readyMsg_m, sizeof(tReadyMsg),
                       (volatile UINT8 *)&syncInstance_l.syncMsg_m, sizeof(tSyncMsg),
                       syncReceived, "ready", SYNC_TIMEOUT_MS))
    {
        fReturn = TRUE;
    }   /* no else: Error is handled in the called function */

    return fReturn;
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/*----------------------------------------------------------------------------*/
/**
\brief    This function is called when the sync message is received

\param[in] pSyncBase_p    The base address of the sync message
\param[in] syncSize_p     The size of the synchronization message

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN syncReceived(volatile UINT8* pSyncBase_p, UINT16 syncSize_p)
{
    BOOLEAN fReturn = FALSE;

    if((volatile UINT8 *)&syncInstance_l.syncMsg_m == pSyncBase_p &&
       syncSize_p == sizeof(tSyncMsg)                              )
    {
        /* Check if the synchronization message is correct */
        if(verifySyncMessage((volatile tSyncMsg *)pSyncBase_p))
        {
            fReturn = TRUE;
        }
        else
        {
            /* Response message is invalid */
            errh_postFatalError(kErrSourcePeriph, kHandSSyncMsgInvalid, 0);
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
\brief    Verify the synchronization message correctness

\param[in]    pSyncMsg_p           Pointer to the sync response message

\retval TRUE    The sync message is correct
\retval FALSE   Invalid response received
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN verifySyncMessage(volatile tSyncMsg * pSyncMsg_p)
{
    BOOLEAN fMsgCorrect = FALSE;
    UINT64 currTime = 0;

    /* Verify the correctness of the response message header */
    if(pSyncMsg_p->msgHeader_m == syncInstance_l.syncMsg_m.msgHeader_m)
    {
        /* Overtake the new consecutive timebase value */
        MEMCOPY(&currTime, &pSyncMsg_p->consTime_m, sizeof(UINT64));

        /* Forward the new time value to the consecutive time module */
        constime_setTimebase(currTime);

        fMsgCorrect = TRUE;
    }

    return fMsgCorrect;
}

/**
 * \}
 * \}
 */
