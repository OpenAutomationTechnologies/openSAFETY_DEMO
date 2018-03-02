/**
********************************************************************************
\file   demo-sn-gpio/statehandler.c

\defgroup module_sn_stateh State handler module
\{

\brief  This module handles the current SN state

This module consists of the current CN state which. It provides access
to this state in the whole firmware.

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
#include <sn/statehandler.h>

#include <SNMTSapi.h>

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
 * \brief The state handler instance type
 */
typedef struct
{
    BOOLEAN fEnterPreop_m;                 /**< TRUE if the SN should enter pre-operational state */
    BOOLEAN fEnterOperational_m;           /**< TRUE if the SN should enter operational state */
    BOOLEAN fShutdown_m;                   /**< TRUE if the SN should perform a shutdown */
    tSnState currSnState_m;                /**< Stores the current SN state */
} tStateHInstance;

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/
static tStateHInstance statehInstance_l SAFE_INIT_SEKTOR;

#ifdef _DEBUG
static char *strSnStates[] = { "SNMTS_k_ST_BOOTING",
                               "SNMTS_k_ST_INITIALIZATION",
                               "SNMTS_k_ST_PRE_OPERATIONAL",
                               "SNMTS_k_ST_OPERATIONAL"};
#endif

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static BOOLEAN enterOperational(void);

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the state handler module

\param[in] snInitState_p    The initial state of the SN

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
BOOLEAN stateh_init(tSnState snInitState_p)
{
    BOOLEAN fReturn = FALSE;

    if(snInitState_p < kSnStateCount)
    {
        MEMSET(&statehInstance_l, 0, sizeof(tStateHInstance));

        /* Set the SN initial state */
        statehInstance_l.currSnState_m = snInitState_p;

        fReturn = TRUE;
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Close the state handler module
*/
/*----------------------------------------------------------------------------*/
void stateh_exit(void)
{
    MEMSET(&statehInstance_l, 0, sizeof(tStateHInstance));
}

/*----------------------------------------------------------------------------*/
/**
\brief    Set the SN state variable to a new value

\param[in] newState_p       The new state of the SN
*/
/*----------------------------------------------------------------------------*/
void stateh_setSnState(tSnState newState_p)
{
    statehInstance_l.currSnState_m = newState_p;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Get the current value of the SN state variables

\return   The current state of the SN
*/
/*----------------------------------------------------------------------------*/
tSnState stateh_getSnState(void)
{
    return statehInstance_l.currSnState_m;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Set the enter operational flag to a new value

\param[in] newVal_p       The new value of the enter op flag
*/
/*----------------------------------------------------------------------------*/
void stateh_setEnterOpFlag(BOOLEAN newVal_p)
{
    statehInstance_l.fEnterOperational_m = newVal_p;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Get the new value of the enter operational flag

\return   The current value of the enter operational flag
*/
/*----------------------------------------------------------------------------*/
BOOLEAN stateh_getEnterOpFlag(void)
{
    return statehInstance_l.fEnterOperational_m;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Set the enter pre-operational flag to a new value

\param[in] newVal_p       The new value of the enter preop flag
*/
/*----------------------------------------------------------------------------*/
void stateh_setEnterPreOpFlag(BOOLEAN newVal_p)
{
    statehInstance_l.fEnterPreop_m = newVal_p;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Get the new value of the enter pre-operational flag

\return   The current value of the enter pre-operational flag
*/
/*----------------------------------------------------------------------------*/
BOOLEAN stateh_getEnterPreOpFlag(void)
{
    return statehInstance_l.fEnterPreop_m;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Set the enter shutdown flag to a new value

\param[in] newVal_p       The new value of the enter shutdown flag
*/
/*----------------------------------------------------------------------------*/
void stateh_setShutdownFlag(BOOLEAN newVal_p)
{
    statehInstance_l.fShutdown_m = newVal_p;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Get the new value of the enter shutdown flag

\return   The current value of the enter shutdown flag
*/
/*----------------------------------------------------------------------------*/
BOOLEAN stateh_getShutdownFlag(void)
{
    return statehInstance_l.fShutdown_m;
}


/*----------------------------------------------------------------------------*/
/**
\brief    Print the current state of the SN

If changed this function prints the current state of the SN to stdout.

\ingroup module_sn_main*/
/*----------------------------------------------------------------------------*/
void stateh_printSNState(void)
{
#ifdef _DEBUG
    static tSnState lastState = kSnStateBooting;
    tSnState currState = statehInstance_l.currSnState_m;

    /* if the SN state changed */
    if(currState != lastState)
    {
        if(lastState < kSnStateCount && currState < kSnStateCount)
        {
            /* signal the actual SN state */
            DEBUG_TRACE(DEBUG_LVL_ALWAYS, "\nCHANGE STATE: %s -> %s\n", strSnStates[lastState],
                                                                        strSnStates[currState]);

            /*store the last SN state */
            lastState = currState;
        }
    }
#endif
}


/*----------------------------------------------------------------------------*/
/**
\brief    Handle state changes of the openSAFETY stack

\return Processing successful; Error on state change
*/
/*----------------------------------------------------------------------------*/
BOOLEAN stateh_handleStateChange(void)
{
    BOOLEAN fReturn = FALSE;

    if(stateh_getEnterOpFlag())
    {
        /* Perform state change to operational */
        if(enterOperational())
        {
            fReturn = TRUE;
        }
    }
    else if(stateh_getEnterPreOpFlag())
    {
        /* Perform state change to pre operational */
        if(stateh_enterPreOperational())
        {
            fReturn = TRUE;
        }
    }
    else
    {
        /* No state change in this cycle */
        fReturn = TRUE;
    }

    return fReturn;
}


/*----------------------------------------------------------------------------*/
/**
\brief    Perform state change to pre operational state

\retval TRUE    Change to pre operational successful
\retval FALSE   Failed to change the state
*/
/*----------------------------------------------------------------------------*/
BOOLEAN stateh_enterPreOperational(void)
{
    BOOLEAN fReturn = FALSE;
    UINT32 consTime = 0;

    consTime = constime_getTime();

    /* transition to PreOperational */
    if(SNMTS_PerformTransPreOp(B_INSTNUM_ consTime))
    {
        stateh_setSnState(kSnStatePreOperational);

        fReturn = TRUE;
    }
    else
    {
        errh_postFatalError(kErrSourcePeriph, kErrorEnterPreOperationalFailed, 0);
    }

    /* Reset the enter pre operation flag */
     stateh_setEnterPreOpFlag(FALSE);

    return fReturn;
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/*----------------------------------------------------------------------------*/
/**
\brief    Perform state change to operational state

\retval TRUE    Change to operational successful
\retval FALSE   Failed to change the state
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN enterOperational(void)
{
    BOOLEAN fReturn = FALSE;
    UINT8 errGrp = 0;
    UINT8 errCode = 0;

    /* Perform transition to operational */
    if(SNMTS_EnterOpState(B_INSTNUM_ TRUE, errGrp, errCode))
    {
        /* Forward new state to SHNF */
        stateh_setSnState(kSnStateOperational);

        fReturn = TRUE;
    }
    else
    {
        errh_postFatalError(kErrSourcePeriph, kErrorEnterOperationalFailed, (errGrp<<8 | errCode));
    }

    /* Reset the operation flag */
    stateh_setEnterOpFlag(FALSE);

    return fReturn;
}

/**
 * \}
 * \}
 */
