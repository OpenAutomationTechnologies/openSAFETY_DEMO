/**
********************************************************************************
\file   demo-sn-gpio/errorhandler.c

\defgroup module_sn_errorhandler Error handler module
\{

\brief  This module implements the error handler of the SN

This module is the main point of all errors in the whole firmware. All
errors which are reported to this module are forwarded via the logbook
channel to the PLC.

\ingroup group_app_sn

*******************************************************************************/

/*------------------------------------------------------------------------------
* License Agreement
*
* Copyright 2017 BERNECKER + RAINER, AUSTRIA, 5142 EGGELSBERG, B&R STRASSE 1
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

#include <sn/errorhandler.h>

#include <shnf/hnf.h>

#include <sn/statehandler.h>

#include <libpsicommon/ami.h>

/*============================================================================*/
/*            G L O B A L   D E F I N I T I O N S                             */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* module global vars                                                         */
/*----------------------------------------------------------------------------*/
static UINT32 lostErrors_l = 0;     /**< Is incremented when an error was not forwarded to the hnf */


/*----------------------------------------------------------------------------*/
/* global function prototypes                                                 */
/*----------------------------------------------------------------------------*/


/*============================================================================*/
/*            P R I V A T E   D E F I N I T I O N S                           */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

#define ERR_LIFETIME_EXCEEDED     0x49        /**< unit_m is set to this value of the lifetime is exceeded */

/*----------------------------------------------------------------------------*/
/* local types                                                                */
/*----------------------------------------------------------------------------*/

/**
 * \brief Error handler instance parameter
 */
typedef struct
{
    BOOLEAN    fErrDescFull_m;   /**< TRUE if the local error descriptor is full */
    tErrorDesc errDesc_m;        /**< Place for one error to be printed in the background loop */
} tErrhInstance;

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/
static tErrhInstance errHanInstance_l SAFE_INIT_SEKTOR;

#ifdef _DEBUG
static char *errSource[] = { "Invalid", "openSAFETY", "HNF", "SHNF", "SAPL", "Periph", "XCom" };
#endif

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static BOOLEAN enterPreopOnError(tErrorDesc * pErrDesc_p);

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the error handler module
*/
/*----------------------------------------------------------------------------*/
void errh_init(void)
{
    MEMSET(&errHanInstance_l, 0, sizeof(tErrhInstance));
}

/*----------------------------------------------------------------------------*/
/**
\brief    Close the error handler module
*/
/*----------------------------------------------------------------------------*/
void errh_exit(void)
{
    /* Nothing to free */
}

/*----------------------------------------------------------------------------*/
/**
\brief    Post information to erro handler

\param[in] source_p     The source module of the error
\param[in] code_p       The error code
\param[in] addInfo_p    Additional error info
*/
/*----------------------------------------------------------------------------*/
void errh_postInfo(tErrSource source_p, tErrorTypes code_p, UINT32 addInfo_p)
{
    tErrorDesc errDesc;

    errDesc.fFailSafe_m = FALSE;
    errDesc.source_m = source_p;
    errDesc.class_m = kErrLevelInfo;
    errDesc.unit_m = 0;
    errDesc.code_m = (UINT8)code_p;
    errDesc.addInfo_m = addInfo_p;

    errh_postError(&errDesc);
}


/*----------------------------------------------------------------------------*/
/**
\brief    Post an minor error to the error handler

\param[in] source_p     The source module of the error
\param[in] code_p       The error code
\param[in] addInfo_p    Additional error info
*/
/*----------------------------------------------------------------------------*/
void errh_postMinorError(tErrSource source_p, tErrorTypes code_p, UINT32 addInfo_p)
{
    tErrorDesc errDesc;

    errDesc.fFailSafe_m = FALSE;
    errDesc.source_m = source_p;
    errDesc.class_m = kErrLevelMinor;
    errDesc.unit_m = 0;
    errDesc.code_m = (UINT8)code_p;
    errDesc.addInfo_m = addInfo_p;

    errh_postError(&errDesc);
}

/*----------------------------------------------------------------------------*/
/**
\brief    Post an fatal error to the error handler

Posting an error of this kind produces a shutdown of the SN.

\param[in] source_p     The source module of the error
\param[in] code_p       The error code
\param[in] addInfo_p    Additional error info
*/
/*----------------------------------------------------------------------------*/
void errh_postFatalError(tErrSource source_p, tErrorTypes code_p, UINT32 addInfo_p)
{
    tErrorDesc errDesc;

    errDesc.fFailSafe_m = TRUE;         /* All fatal errors indicate a shutdown */
    errDesc.source_m = source_p;
    errDesc.class_m = kErrLevelFatal;
    errDesc.unit_m = 0;
    errDesc.code_m = (UINT8)code_p;
    errDesc.addInfo_m = addInfo_p;

    errh_postError(&errDesc);
}

/*----------------------------------------------------------------------------*/
/**
\brief    Post and error to the error handler

\param[in] pErrDesc_p   Pointer to the error description
*/
/*----------------------------------------------------------------------------*/
void errh_postError(tErrorDesc * pErrDesc_p)
{
    if(pErrDesc_p != NULL)
    {
        /* Check if the local error buffer can handle this error */
        if(errHanInstance_l.fErrDescFull_m == FALSE)
        {
            /* Store this error for later printing */
            MEMCOPY(&errHanInstance_l.errDesc_m, pErrDesc_p, sizeof(tErrorDesc));

            errHanInstance_l.fErrDescFull_m = TRUE;
        }

        /* React on errors */
        if(pErrDesc_p->fFailSafe_m)
        {
            /* On errors with failsafe set -> shutdown the firmware */
            stateh_setShutdownFlag(TRUE);
        }
        else
        {
            /* On some unique errors/infos we perform a switch to preop! */
            if(enterPreopOnError(pErrDesc_p))
            {
                stateh_setEnterPreOpFlag(TRUE);
            }
        }

        /* Forward the error to the logger module if the SN state is preop */
        if(stateh_getSnState() > kSnStateInitializing)
        {
            /* The error is reported via the logbook to the PLC */
            if(hnf_postLogChannel0(pErrDesc_p) == FALSE)
                lostErrors_l++;
        }
    }
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process the posted errors

This function is called in the background loop to print the error messages.
This is needed because error message printing can take very long.

\note No error FIFO is implemented. A possible loss of an error shall be expected!
*/
/*----------------------------------------------------------------------------*/
void errh_proccessError(void)
{
#ifdef _DEBUG
    tErrorDesc * pErrDesc_p = &errHanInstance_l.errDesc_m;

    if(errHanInstance_l.fErrDescFull_m == TRUE)
    {
        switch(pErrDesc_p->class_m)
        {
            case kErrLevelInfo:
            {
                DEBUG_TRACE(DEBUG_LVL_ALWAYS, "\n\n!!! Information !!!\n");
                DEBUG_TRACE(DEBUG_LVL_ALWAYS, "    Info source = %s\n", errSource[pErrDesc_p->source_m]);
                DEBUG_TRACE(DEBUG_LVL_ALWAYS, "    Unit = 0x%X\n", pErrDesc_p->unit_m);
                DEBUG_TRACE(DEBUG_LVL_ALWAYS, "    Code = 0x%X\n", pErrDesc_p->code_m);
                DEBUG_TRACE(DEBUG_LVL_ALWAYS, "    Additional information = 0x%X\n\n", pErrDesc_p->addInfo_m);
                break;
            }
            case kErrLevelMinor:
            {
                DEBUG_TRACE(DEBUG_LVL_ERROR, "\n\n!!! Minor error happened !!!\n");
                DEBUG_TRACE(DEBUG_LVL_ALWAYS, "    Error source = %s\n", errSource[pErrDesc_p->source_m]);
                DEBUG_TRACE(DEBUG_LVL_ALWAYS, "    Unit = 0x%X\n", pErrDesc_p->unit_m);
                DEBUG_TRACE(DEBUG_LVL_ALWAYS, "    Code = 0x%X\n", pErrDesc_p->code_m);
                DEBUG_TRACE(DEBUG_LVL_ALWAYS, "    Additional information = 0x%X\n\n", pErrDesc_p->addInfo_m);
                break;
            }
            case kErrLevelFatal:
            {
                DEBUG_TRACE(DEBUG_LVL_ERROR, "\n\n!!! FATAL ERROR HAPPENED !!!\n");
                DEBUG_TRACE(DEBUG_LVL_ALWAYS, "    Error source = %s\n", errSource[pErrDesc_p->source_m]);
                DEBUG_TRACE(DEBUG_LVL_ALWAYS, "    Unit = 0x%X\n", pErrDesc_p->unit_m);
                DEBUG_TRACE(DEBUG_LVL_ALWAYS, "    Code = 0x%X\n", pErrDesc_p->code_m);
                DEBUG_TRACE(DEBUG_LVL_ALWAYS, "    Additional information = 0x%X\n\n", pErrDesc_p->addInfo_m);
                break;
            }
            default:
                break;
        }

        errHanInstance_l.fErrDescFull_m = FALSE;
    }

#endif /* #ifdef _DEBUG */
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/*----------------------------------------------------------------------------*/
/**
\brief    Check if this error shall trigger a switch to preop

\param[in] pErrDesc_p   Pointer to the error description

\ingroup module_sn_sapl*/
/*----------------------------------------------------------------------------*/
static BOOLEAN enterPreopOnError(tErrorDesc * pErrDesc_p)
{
    BOOLEAN fEnterPreop = FALSE;

    if(pErrDesc_p->source_m == kErrSourceStack      &&
       pErrDesc_p->class_m == kErrLevelInfo         &&
       pErrDesc_p->unit_m == SNMTS_k_UNIT_ID        &&
       pErrDesc_p->code_m == ERR_LIFETIME_EXCEEDED   )
    {
        fEnterPreop = TRUE;
    }

    return fEnterPreop;
}

/**
 * \}
 * \}
 */
