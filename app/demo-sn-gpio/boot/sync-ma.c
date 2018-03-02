/**
********************************************************************************
\file   demo-sn-gpio/boot/sync-ma.c

\defgroup module_sn_boot_sync_ma Synchronization module (Master side)
\{

\brief  Implements the synchronization of uP-Master and uP-Slave

This module implements the synchronization of both safe processors on the uP-Master
side. The master waits until the uP-Slave is ready and the first synchronous
interrupt has occurred. If this condition is met a response is sent to uP-Slave
and the processing begins.

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

#include <boot/internal/pingpong-ma.h>

#include <common/syncir.h>

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
 * \brief Synchronization module instance type
 */
typedef struct
{
    volatile tReadyMsg readyMsg_m;      /**< Buffer the uP-Slave ready message is stored */
    volatile tSyncMsg syncMsg_m;        /**< Buffer the synchronization message is stored */
    volatile BOOLEAN fSyncIrOccured_m;  /**< TRUE if the synchronous interrupt has occurred */
} tSyncInstance;

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/
static tSyncInstance syncInstance_l SAFE_INIT_SEKTOR;

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static BOOLEAN readyReceived(volatile UINT8* pReadyBase_p, UINT16 readySize_p);
static BOOLEAN verifyReadyMessage(volatile tReadyMsg * pReadyMsg_p);
static void fillSyncMsg(void);

static BOOLEAN waitForSyncIrq(void);
static void syncInterruptCb(void *);

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

    /* Call the ping/pong module transfer function to start the message exchange */
    if(pipo_doTransfer((volatile UINT8*)&syncInstance_l.readyMsg_m, sizeof(tReadyMsg),
                       (volatile UINT8*)&syncInstance_l.syncMsg_m, sizeof(tSyncMsg),
                       readyReceived, "ready"))
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
\brief    Callback which is called after the ready message is received

\param[in] pReadyBase_p   Pointer to the ready message base address
\param[in] readySize_p    Size of the ready message

\retval TRUE    The ready message is correct
\retval FALSE   Invalid message received
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN readyReceived(volatile UINT8* pReadyBase_p, UINT16 readySize_p)
{
    BOOLEAN fReturn = FALSE;

    /* Check if the provided buffer is valid */
    if(pReadyBase_p == (volatile UINT8*)&syncInstance_l.readyMsg_m &&
       readySize_p == sizeof(tReadyMsg)            )
    {
        /* Verify the ready message validity */
        if(verifyReadyMessage((volatile tReadyMsg *)pReadyBase_p))
        {
            /* Wait for the first synchronous interrupt from the PCP */
            if(waitForSyncIrq())
            {
                /* Create the synchronization response message */
                fillSyncMsg();

                fReturn = TRUE;
            }
        }
        else
        {
            /* Ready message is invalid */
            errh_postFatalError(kErrSourcePeriph, kHandSReadyMsgInvalid, 0);
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
\brief    Verify the ready message correctness

\param[in]    pReadyMsg_p    Pointer to the ready message

\retval TRUE    The ready message is correct
\retval FALSE   Invalid message received
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN verifyReadyMessage(volatile tReadyMsg * pReadyMsg_p)
{
    BOOLEAN fMsgCorrect = FALSE;

    /* Verify the correctness of the ready message */
    if(pReadyMsg_p->msgHeader_m == (UINT32)READY_MSG_CONTENT)
    {
        fMsgCorrect = TRUE;
    }

    return fMsgCorrect;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Fill the synchronization response message buffer
*/
/*----------------------------------------------------------------------------*/
static void fillSyncMsg(void)
{
    volatile tSyncMsg * pSyncMsg = &syncInstance_l.syncMsg_m;
    UINT64 currTime = constime_getTimeBase();

    /* Fill the sync response message header */
    pSyncMsg->msgHeader_m = syncInstance_l.readyMsg_m.msgHeader_m;

    /* Set the current consecutive timebase in the sync message */
    MEMCOPY(&pSyncMsg->consTime_m, &currTime, sizeof(UINT64));
}

/*----------------------------------------------------------------------------*/
/**
\brief    Wait until the synchronous interrupt from PCP occurred once

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN waitForSyncIrq(void)
{
    BOOLEAN fReturn = TRUE;
    tPlatformSyncIrq pfnSyncIrCb = syncir_getSyncCallback();

    /* Set new local synchronous interrupt callback */
    syncir_setSyncCallback(syncInterruptCb);

    DEBUG_TRACE(DEBUG_LVL_ALWAYS, "\nWait for sync interrupt ... \n");

    syncir_enable();

    while(syncInstance_l.fSyncIrOccured_m == FALSE)
        ;

    /* Disable the synchronous interrupt */
    syncir_disable();

    /* Restore the old value of the synchronous interrupt */
    syncir_setSyncCallback(pfnSyncIrCb);

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Local synchronous interrupt callback function

\param[in] pArg_p       Pointer to the ir arguments
*/
/*----------------------------------------------------------------------------*/
static void syncInterruptCb(void * pArg_p)
{
    UNUSED_PARAMETER(pArg_p);

    syncInstance_l.fSyncIrOccured_m = TRUE;
}

/**
 * \}
 * \}
 */
