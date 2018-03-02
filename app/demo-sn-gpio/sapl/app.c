/**
********************************************************************************
\file   demo-sn-gpio/sapl/app.c

\defgroup module_sn_sapl_app User application module
\{

\brief  This module hosts the user application

This module implements the generic part of the user application. This application
reads and writes the SPDOs with meaningful data which is than transported
to the SCM. The application implements a gpio processing example.

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
#include <sapl/app.h>

#include <sodutil.h>

#include <common/app-gpio.h>


/*============================================================================*/
/*            G L O B A L   D E F I N I T I O N S                             */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/
#define APP_SPDO_NUM        (UINT16)0       /**< Id of the SPDO of the application */

/*----------------------------------------------------------------------------*/
/* module global vars                                                         */
/*----------------------------------------------------------------------------*/
tSPDOTransportSafeIN traspSafeIN_g SAFE_NO_INIT_SEKTOR;      /**< The SafeIn SPDOTransport channels */
tSPDOTransportSafeOUT traspSafeOUT_g SAFE_NO_INIT_SEKTOR;    /**< The SafeOUT SPDOTransport channels */

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
static tGetConValidCb pfnGetConValid_l = NULL;

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static BOOLEAN processGPIO(UINT8 inPort_p, UINT32* pOutport_p);

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the application module

\param[in] pfnGetConValid_p     Pointer to the get connection valid callback

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
BOOLEAN app_init(tGetConValidCb pfnGetConValid_p)
{
    BOOLEAN fReturn = FALSE;

    /* Check if the connection valid callback is set */
    if(pfnGetConValid_p != NULL)
    {
        pfnGetConValid_l = pfnGetConValid_p;

        /* Initialize the target specific parts of the gpio application */
        if(appgpio_init() == 0)
        {
            /* Initialize the SPDOTransport data */
            app_reset();

            fReturn = TRUE;
        }
        else
        {
            errh_postFatalError(kErrSourceSapl, kErrorInitApplicationFailed, 0);
        }
    }
    else
    {
        errh_postFatalError(kErrSourceSapl, kErrorInvalidParameter, 0);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Close the application module
*/
/*----------------------------------------------------------------------------*/
void app_exit(void)
{
    pfnGetConValid_l = NULL;

    app_reset();

    /* Shutdown peripherals */
    appgpio_exit();
}

/*----------------------------------------------------------------------------*/
/**
\brief    Reset the application input and outputs
*/
/*----------------------------------------------------------------------------*/
void app_reset(void)
{
    UINT32 outPort = 0;

    traspSafeIN_g.SafeInput01 = 0;
    traspSafeIN_g.SafeInput02 = 0;
    traspSafeIN_g.SafeInput03 = 0;
    traspSafeIN_g.SafeInput04 = 0;

    traspSafeOUT_g.SafeOutput01 = 0;
    traspSafeOUT_g.SafeOutput02 = 0;
    traspSafeOUT_g.SafeOutput03 = 0;
    traspSafeOUT_g.SafeOutput04 = 0;

    /* Write zero to outport */
    appgpio_writeOutputPort(outPort);
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process the application

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
BOOLEAN app_process(void)
{
    BOOLEAN fReturn = FALSE;
    UINT32 outPort = 0;
    UINT8 inPort;

    if(pfnGetConValid_l != NULL)
    {
        if(pfnGetConValid_l(APP_SPDO_NUM))
        {
            /* Read the digital input port from hardware */
            inPort = appgpio_readInputPort();

            /* Call the simple gpio processing application */
            if(processGPIO(inPort, &outPort))
            {
                fReturn = TRUE;
            }

            /* Write the digital output port to hardware */
            appgpio_writeOutputPort(outPort);
        }
        else
        {
            /* Connection is not valid! Application processing not allowed! */
            app_reset();

            /* Write zero to outport */
            appgpio_writeOutputPort(outPort);

            fReturn = TRUE;
        }
    }
    else
    {
        errh_postFatalError(kErrSourceSapl, kErrorCallbackNotInitialized, 0);
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
\brief    Read input data and write output data

This function implements a simple gpio application which stores the
read in value of the gpio pins in tx data and writes the value of rx data
to the gpio pins

\param[in] inPort_p     The current value of the inport
\param[out] pOutport_p  Pointer to the current value of the outport

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN processGPIO(UINT8 inPort_p, UINT32* pOutport_p)
{
    BOOLEAN fReturn = FALSE;

    if(pOutport_p != NULL)
    {
        traspSafeIN_g.SafeInput01 = inPort_p;
        traspSafeIN_g.SafeInput02 = inPort_p;
        traspSafeIN_g.SafeInput03 = inPort_p;
        traspSafeIN_g.SafeInput04 = inPort_p;

        *pOutport_p = traspSafeOUT_g.SafeOutput01;

        fReturn = TRUE;
    }
    else
    {
        errh_postFatalError(kErrSourceSapl, kErrorInvalidParameter, 0);
    }

    return fReturn;
}

/**
 * \}
 * \}
 */

