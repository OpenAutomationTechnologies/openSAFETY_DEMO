/**
********************************************************************************
\file   demo-sn-gpio/main.c

\defgroup module_sn_main Main module
\{

\brief Safety node firmware main instance

Entry point of the safety node firmware. Initializes all submodules and the
stack and processes the background task.

\ingroup group_app_sn

*******************************************************************************/

/*------------------------------------------------------------------------------
* License Agreement
*
* Copyright 2014 BERNECKER + RAINER, AUSTRIA, 5142 EGGELSBERG, B&R STRASSE 1
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
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS && CONTRIBUTORS "AS IS"
* && ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
* THE IMPLIED WARRANTIES OF MERCHANTABILITY && FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED &&
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/

#include <sn/global.h>
#include <sn/cyclemon.h>
#include <sn/statehandler.h>

#include <common/platform.h>

#include <shnf/shnf.h>
#include <sapl/sapl.h>

#if (defined SYSTEM_PATH) & (SYSTEM_PATH > ID_ID_TARG_SINGLE )
#include <sn/upserial.h>
#include <boot/handshake.h>
#include <boot/sync.h>

#include <shnf/xcom.h>
#endif /* #if (defined SYSTEM_PATH) && (SYSTEM_PATH > ID_TARG_SINGLE) */

#include <SNMTSapi.h>
#include <SCFMapi.h>
#include <SSCapi.h>

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
 * \brief Slots to process in the synchronous IR
 */
typedef enum
{
    kSyncTaskInvalid  = 0x0,    /**< Invalid state */
    kSyncTaskShnf     = 0x1,    /**< The SHNF task is processed in this slot */
    kSyncTaskSapl     = 0x2,    /**< The SAPL task is processed in this slot */
    kSyncTaskState    = 0x3,    /**< State changes are processed in this slot */
    kSyncTaskCount    = 0x4,    /**< Count of states */
} tProcSyncTask;

static tProcSyncTask syncTaskState_l;     /**< The current state of the sync task */

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static BOOLEAN initOpenSafety(void);
#if (defined SYSTEM_PATH) && (SYSTEM_PATH > ID_TARG_SINGLE)
static BOOLEAN initXCom(void);
static BOOLEAN setXComParameters(UINT64 * pCurrTime_p);
#endif /* #if (defined SYSTEM_PATH) && (SYSTEM_PATH > ID_TARG_SINGLE) */

static BOOLEAN processAsync(void);
static BOOLEAN processSync(void);
static BOOLEAN syncCycle(void);

static BOOLEAN processTaskSchedule(void);

static void enterReset(void);
static void shutdown(void);


/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Entry function of SN example example

main() implements the main program function of the CN API example.
First all initialization is done, then the program runs in a loop where the
APs state machine will be updated and input/output ports will be processed.

\retval 0          On successful shutdown
\retval -1         Shutdown on error
*/
/*----------------------------------------------------------------------------*/
int main (void)
{
    int retVal = -1;
    tShnfInitParam shnfInitParam;

    MEMSET(&shnfInitParam, 0, sizeof(tShnfInitParam));

    /* Set initial sync task state */
    syncTaskState_l = kSyncTaskShnf;

    /* Initialize target specific functions */
    platform_init();

    DEBUG_TRACE(DEBUG_LVL_ALWAYS, "\n\n********************************************************************\n");
    DEBUG_TRACE(DEBUG_LVL_ALWAYS, "\n\topenSAFETY SafetyNode Demo V%s \n\n ", OS_DEMO_VERSION);
    DEBUG_TRACE(DEBUG_LVL_ALWAYS, "\tStack version: \t\t%s\n", EPLS_k_STACK_VERSION );
    DEBUG_TRACE(DEBUG_LVL_ALWAYS, "\tConfiguration: \t\t%s\n", EPLS_k_CONFIGURATION_STRING );
    DEBUG_TRACE(DEBUG_LVL_ALWAYS, "\tTarget System: \t\t%s\n", EPLS_k_TARGET_STRING );
    DEBUG_TRACE(DEBUG_LVL_ALWAYS, "********************************************************************\n");

    /* Initialize the state handler */
    if(stateh_init(kSnStateBooting))
    {
        errh_init();

        /* Initialize the openSAFETY stack and change state to init */
        if(initOpenSafety())
        {
            /* Initialize the consecutive time module */
            if(constime_init())
            {
                /* Initialize the cycle monitoring module */
                if(cyclemon_init())
                {
                    /* Initialize the SHNF module */
                    shnfInitParam.pfnSyncronize_m = syncCycle;
                    shnfInitParam.pfnProcSync_m = processSync;

                    if(shnf_init(&shnfInitParam))
                    {
                        /* Initialize the safe application module */
                        DEBUG_TRACE(DEBUG_LVL_ALWAYS, "\nInitialize the SAPL -> ");
                        if(sapl_init())
                        {
                            DEBUG_TRACE(DEBUG_LVL_ALWAYS, "SUCCESS!\n");

#if (defined SYSTEM_PATH) && (SYSTEM_PATH > ID_TARG_SINGLE)
                            /* Initialize the uP-Master <-> uP-Slave cross communication */
                            if(initXCom())
                            {
#else
                            /* Perform the SOD restore if needed (Single channeled only) */
                            if(sapl_restoreSod(TRUE))
                            {
#endif /* #if (defined SYSTEM_PATH) && (SYSTEM_PATH > ID_TARG_SINGLE) */
                                /* Change state of the openSAFETY stack to pre operational */
                                if(stateh_enterPreOperational())
                                {
                                    /* Enable the synchronous interrupt */
                                    shnf_enableSyncIr();

                                    DEBUG_TRACE(DEBUG_LVL_ALWAYS, "\nStart processing ... \n ");

                                    if(processAsync())
                                    {
                                        /* Shutdown triggered -> Terminate! */
                                        retVal = 0;
                                    }
                                }   /* no else: Error is already reported in the called function */

                           }   /* no else: Error is already reported in the called function */
                        }   /* no else: Error is already reported in the called function */
                    }   /* no else: Error is already reported in the called function */
                }   /* no else: Error is already reported in the called function */
            }   /* no else: Error is already reported in the called function */
        }   /* no else: Error is already reported via SAPL_SERR_SignalErrorClbk */
    }   /* no else: Error is already reported in the called function */

    /* Print error before shutdown */
    errh_proccessError();

    shutdown();

    return retVal;
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the openSAFETY stack

Initialize the openSAFETY stack and switch the SN to the Pre-operational state.

\retval TRUE    Stack initialization was successful
\retval FALSE   Error during stack initialization
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN initOpenSafety(void)
{
    BOOLEAN fReturn = FALSE; /* return value */
    SNMTS_t_SN_STATE_MAIN snState = 0;

    DEBUG_TRACE(DEBUG_LVL_ALWAYS, "\nInitialize the openSAFETY stack -> ");
    if(SSC_InitAll())
    {
        DEBUG_TRACE(DEBUG_LVL_ALWAYS, "SUCCESS!\n");

        /* if the initialization of the last SN state succeeded */
        if(SNMTS_GetSnState(B_INSTNUM_ &snState))
        {
            /* Verify state change to init */
            if(snState == SNMTS_k_ST_INITIALIZATION)
            {
                stateh_setSnState(kSnStateInitializing);

                stateh_printSNState();

                fReturn = TRUE;
            }
        }
    }

    return fReturn;
}

#if (defined SYSTEM_PATH) && (SYSTEM_PATH > ID_TARG_SINGLE)
/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the uP-Master <-> uP-Slave cross communication

\retval TRUE    XCom initialization successful
\retval FALSE   Error on initialization
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN initXCom(void)
{
    BOOLEAN fReturn = FALSE;
    BOOLEAN fRestoreSod = FALSE;

    /* Initialize the uP-Master <-> uP-Slave serial device */
    if(upserial_init())
    {
        /* Check if there is a valid SOD image */
        fRestoreSod = sapl_checkSodStorage();

        /* Carry out the boot-up handshake of uP-Master and uP-Slave */
        if(hands_perform(&fRestoreSod))
        {
            /* Perform the SOD restore if needed (Dual channeled only) */
            if(sapl_restoreSod(fRestoreSod))
            {
                /* Carry out the synchronization to ensure a synchronous bootup */
                if(sync_perform())
                {
                    /* Initialize the cross communication module */
                    if(xcom_init())
                    {
                        fReturn = TRUE;
                    }   /* no else: Error is reported in the called function */
                }   /* no else: Error is reported in the called function */
            }   /* no else: Error is reported in called function */
        }   /* no else: Error is reported in called function */
    }
    else
    {
        errh_postFatalError(kErrSourcePeriph, kErrorSerialInitFailed, 0);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Set cross communication parameters on cycle start

This function sets the current timestamp in the xcom module and also enables
the cross communication checks.

\param[in] pCurrTime_p      Pointer to the current time

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN setXComParameters(UINT64 * pCurrTime_p)
{
    BOOLEAN fReturn = FALSE;

    /* Set a flag to ensure a cross communication in each cycle */
    if(xcom_enableReceiveCheck())
    {
        /* Forward the current time to the xcom module */
        xcom_setCurrentTimebase(pCurrTime_p);

        fReturn = TRUE;
    }

    return fReturn;
}

#endif /* #if (defined SYSTEM_PATH) && (SYSTEM_PATH > ID_TARG_SINGLE) */

/*----------------------------------------------------------------------------*/
/**
\brief    Process all asynchronous actions

Call all non time critical tasks in the background loop. No function of the
openSAFETY stack where the consecutive timebase is needed shall be
called in this context.

\retval TRUE    Shutdown is triggered
\retval FALSE   Processing failed due to error
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN processAsync(void)
{
    BOOLEAN fReturn = FALSE;
    BOOLEAN fTimeout = FALSE;

    while(TRUE)
    {
        stateh_printSNState();

        /* Process a possible error */
        errh_proccessError();

        if(stateh_getShutdownFlag())
        {
            fReturn = TRUE;
            break;
        }

        /* Periodically process the asynchronous task of the SAPL */
        if(sapl_process() == FALSE)
        {
            DEBUG_TRACE(DEBUG_LVL_ALWAYS, "\nProcessing the SAPL failed ...\n");
            break;
        }

        /* Check if the cycle monitoring has a timeout */
        fTimeout = cyclemon_checkTimeout();
        if(fTimeout == TRUE)
        {
            enterReset();
        }

    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process all synchronous actions

Calls all synchronous actions which need to be done after the openSAFETY stack
had time to calculate.

\retval TRUE    Processing of sync task successful
\retval FALSE   Processing failed due to error
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN processSync(void)
{
    BOOLEAN fReturn = FALSE;

    /* Process the SAPL user application task */
    if(sapl_processApp())
    {
        /* Call the task scheduler at the end of the cycle */
        if(processTaskSchedule())
        {
            fReturn = TRUE;
        }   /* no else: Error already reported in the called function */
    }   /* no else: Error already reported in the called function */

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Synchronize to the current cycle

This function is used to synchronize to the current synchronous IR cycle. It
monitors the cycle time and processes the consecutive time.

\note This function is always called at the start of the sync ISR.

\retval TRUE    Processing of sync cycle successful
\retval FALSE   Processing failed due to error
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN syncCycle(void)
{
    BOOLEAN fReturn = FALSE;
    UINT64 * pCurrTime = NULL;

    /* Call the consecutive time process function in each cycle */
    pCurrTime = constime_process();
    if(pCurrTime != NULL)
    {
        /* Call the cycle monitoring process function */
        if(cyclemon_process())
        {
#if (defined SYSTEM_PATH) && (SYSTEM_PATH > ID_TARG_SINGLE)
            if(setXComParameters(pCurrTime))
            {
                fReturn = TRUE;
            }
#else
            /* For single channeled demos perform sync with one value */
            if(constime_setConsTime(pCurrTime))
            {
                fReturn = TRUE;
            }   /* no else: Error is handled in the called function */
#endif /* #if (defined SYSTEM_PATH) && (SYSTEM_PATH > ID_TARG_SINGLE) */
        }
    }
    else
    {
        errh_postFatalError(kErrSourcePeriph, kErrorInvalidTimeBase, 0);
    }

    return fReturn;
}


/*----------------------------------------------------------------------------*/
/**
\brief    Process the end of cycle task scheduler

Process all openSAFETY stack tasks which would be possible to be called in the
background loop at the end of the synchronous task. In order to reduce the cycle
time this tasks are multiplexed over multiple cycles.

\note Calling these functions synchronous ensures a valid program flow counter.

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN processTaskSchedule(void)
{
    BOOLEAN fReturn = FALSE;

    /* Process one synchronous action in each cycle */
    switch(syncTaskState_l)
    {
        case kSyncTaskShnf:
            /* Periodically process the asynchronous task of the SHNF */
            if(shnf_process())
            {
                fReturn = TRUE;
            }

            /* Switch to next state */
            syncTaskState_l = kSyncTaskSapl;

            break;

        case kSyncTaskSapl:
            /* Periodically process the asynchronous task of the SAPL */
            if(sapl_processSync())
            {
                fReturn = TRUE;
            }

            /* Switch to next state */
            syncTaskState_l = kSyncTaskState;

            break;

        case kSyncTaskState:
            /* Handle internal state changes */
            if(stateh_handleStateChange())
            {
                fReturn = TRUE;
            }

            /* Switch to first state */
            syncTaskState_l = kSyncTaskShnf;
            break;

        default:
            errh_postFatalError(kErrSourcePeriph, kErrorInvalidState, 0);
            break;
    }

    return fReturn;
}


/*----------------------------------------------------------------------------*/
/**
\brief    On a cycle time violation a reset needs to be performed
*/
/*----------------------------------------------------------------------------*/
static void enterReset(void)
{
    shnf_reset();
    sapl_reset();

    /* Switch to preop */
    (void)stateh_enterPreOperational();
}

/*----------------------------------------------------------------------------*/
/**
\brief    Shutdown the SN and cleanup all structures
*/
/*----------------------------------------------------------------------------*/
static void shutdown(void)
{
    DEBUG_TRACE(DEBUG_LVL_ALWAYS, "\n\nShutdown ...\n");

    shnf_exit();
    sapl_exit();

    errh_exit();

#if (defined SYSTEM_PATH) && (SYSTEM_PATH > ID_TARG_SINGLE)
    upserial_exit();
    xcom_exit();
#endif /* #if (defined SYSTEM_PATH) && (SYSTEM_PATH > ID_TARG_SINGLE) */

    constime_exit();
    platform_exit();
}

/**
 * \}
 * \}
 */
