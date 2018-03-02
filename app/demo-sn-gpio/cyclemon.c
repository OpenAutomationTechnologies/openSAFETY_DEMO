/**
********************************************************************************
\file   demo-sn-gpio/cyclemon.c

\defgroup module_sn_cyclemon Cycle monitoring module
\{

\brief  This module implements the cycle monitoring

This module checks if the synchronous interrupt is called periodically. If
a timeout occurs the firmware switches to asynchronous mode until the sync
interrupt occurs again.

\ingroup group_app_sn

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

#include <sn/cyclemon.h>

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
#define CYCLE_TIME_THRESHOLD_US     (UINT32)10      /**< Cycle time threshold value (us) */

#define CYCLE_TIME_MIN_US           (UINT32)400     /**< Cycle time minimum value (us) */
#define CYCLE_TIME_MAX_US           (UINT32)65000   /**< Cycle time maximum value (us) (Limited by the internal 16bit microsecond counter) */

#define CYCLE_TIME_TIMEOUT_US       (UINT32)100    /**< Timeout after an expected sync interrupt is considered to be missing (us) */

/*----------------------------------------------------------------------------*/
/* local types                                                                */
/*----------------------------------------------------------------------------*/

/**
 * \brief Cycle monitoring instance type
 */
typedef struct
{
    tCycleMonState  cycMonState_m;      /**< The current state of the cycle monitoring */
    UINT64 lastTimeStamp_m;             /**< The timestamp of the last cycle */
    UINT32 cycleTime_m;                 /**< The calculated cycletime */
} tCycleMonInstance;

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/
tCycleMonInstance cycMonInstance_l SAFE_INIT_SEKTOR;

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static BOOLEAN calculateCycleTime(void);
static BOOLEAN verifyCycleTime(void);

static void resetCycleMon(void);

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the cycle monitoring module

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
BOOLEAN cyclemon_init(void)
{
    resetCycleMon();

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Close the cycle monitoring
*/
/*----------------------------------------------------------------------------*/
void cyclemon_exit(void)
{
    resetCycleMon();
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process the cycle monitoring state machine

\retval TRUE        Successfully processed the state machine
\retval FALSE       Error on processing
*/
/*----------------------------------------------------------------------------*/
BOOLEAN cyclemon_process(void)
{
    BOOLEAN fReturn = FALSE;
    BOOLEAN fCycleValid = FALSE;

    switch(cycMonInstance_l.cycMonState_m)
    {
        case kCycleMonStateInit:
            /* Evaluate the current cycle time */
            fCycleValid = calculateCycleTime();
            if(fCycleValid)
            {
                /* Check if the cycle time is in the allowed range */
                if(verifyCycleTime())
                {
                    cycMonInstance_l.cycMonState_m = kCycleMonStateActive;
                    fReturn = TRUE;
                }
                else
                {
                    errh_postFatalError(kErrSourcePeriph, kErrorInvalidCycleTime, cycMonInstance_l.cycleTime_m);
                }
            }
            else
            {
                /* Currently evaluating the cycle time */
                fReturn = TRUE;
            }

            break;
        case kCycleMonStateActive:

             /* Always remember the last timestamp */
            cycMonInstance_l.lastTimeStamp_m = constime_getTimeBase();

            fReturn = TRUE;
            break;
        case kCycleMonStateTimeout:
            resetCycleMon();

            fReturn = TRUE;
            break;
        default:
            /* Should not happen */
            errh_postFatalError(kErrSourcePeriph, kErrorCycleMonStateInvalid, 0);
            break;
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Verify if the cycle monitoring has a timeout

\retval TRUE        A timeout has occurred
\retval FALSE       No timeout occurred
*/
/*----------------------------------------------------------------------------*/
BOOLEAN cyclemon_checkTimeout(void)
{
    BOOLEAN fTimeout = FALSE;
    UINT64 currTime = 0;
    UINT64 compVal = 0;

    if(cycMonInstance_l.cycMonState_m == kCycleMonStateActive)
    {
        util_enterCriticalSection(FALSE);
        compVal = cycMonInstance_l.lastTimeStamp_m + cycMonInstance_l.cycleTime_m;
        currTime = constime_getTimeBase();
        util_enterCriticalSection(TRUE);

        if(currTime > (compVal + CYCLE_TIME_TIMEOUT_US))
        {
            /* Cycle monitoring encountered a timeout */
            cycMonInstance_l.cycMonState_m = kCycleMonStateTimeout;
            fTimeout = TRUE;
        }
    }

    return fTimeout;
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/*----------------------------------------------------------------------------*/
/**
\brief    Calculate the current active cycle time

\retval TRUE        Found valid cycle time
\retval FALSE       Currently no cycle time known
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN calculateCycleTime(void)
{
    BOOLEAN fCycleValid = FALSE;
    UINT64 currTimeStamp = 0;
    UINT32 cycleTime = 0;
    UINT32 delta = 0;

    /* Check if it is the first run */
    if(cycMonInstance_l.lastTimeStamp_m == 0)
    {
        /* Save the timestamp for next cycle */
        cycMonInstance_l.lastTimeStamp_m = constime_getTimeBase();
    }
    else
    {
        /* Verify the cycle time */
        currTimeStamp = constime_getTimeBase();
        cycleTime = currTimeStamp - cycMonInstance_l.lastTimeStamp_m;
        delta = (cycleTime - cycMonInstance_l.cycleTime_m);
        if(delta < CYCLE_TIME_THRESHOLD_US)
        {
            /* Overtake the average of the cycle time */
            cycMonInstance_l.cycleTime_m = (cycMonInstance_l.cycleTime_m + cycleTime) >> 1;
            cycMonInstance_l.lastTimeStamp_m = currTimeStamp;
            fCycleValid = TRUE;
        }
        else
        {
            /* Retry next cycle */
            cycMonInstance_l.cycleTime_m = cycleTime;
            cycMonInstance_l.lastTimeStamp_m = currTimeStamp;
        }
    }

    return fCycleValid;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Validate the cycle time ranges

\retval TRUE        Cycle time is inside the known range
\retval FALSE       Cycle time out of bounds
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN verifyCycleTime(void)
{
    BOOLEAN fCycleTimeValid = FALSE;

    if(cycMonInstance_l.cycleTime_m >= CYCLE_TIME_MIN_US &&
       cycMonInstance_l.cycleTime_m <= CYCLE_TIME_MAX_US  )
    {
        fCycleTimeValid = TRUE;
    }

    return fCycleTimeValid;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Reset the cycle monitoring
*/
/*----------------------------------------------------------------------------*/
static void resetCycleMon(void)
{
    cycMonInstance_l.lastTimeStamp_m = 0;
    cycMonInstance_l.cycleTime_m = 0;
    cycMonInstance_l.cycMonState_m = kCycleMonStateInit;
}

/**
 * \}
 * \}
 */
