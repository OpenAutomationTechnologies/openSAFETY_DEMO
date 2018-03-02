/**
********************************************************************************
\file   demo-sn-gpio/target/altera-nios2/timer.c

\defgroup module_sn_nios2_timer Timer module
\{

\brief  Target specific functions of the system timer

This module implements the hardware near target specific functions of the
system timer for Altera Nios2.

\ingroup group_app_sn_targ_nios2
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
#include <sn/timer.h>

#include <system.h>
#include <io.h>

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
#define COUNTER_BASE  APP_0_COUNTER_0_BASE

#define COUNTER_TIME_REG           0
#define COUNTER_TICKCNT_REG        4

/*----------------------------------------------------------------------------*/
/* local types                                                                */
/*----------------------------------------------------------------------------*/

#define TIMER_TICKS_1US         50
#define TIMER_TICKS_10US        500
#define TIMER_TICKS_100US       5000
#define TIMER_TICKS_1MS         50000

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the timer module

\return TRUE on success; FALSE on error
*/
/*----------------------------------------------------------------------------*/
BOOLEAN timer_init(void)
{
    IOWR_32DIRECT(COUNTER_BASE, COUNTER_TICKCNT_REG, 0);

    /* Set default timerbase to ipcore */
    IOWR_32DIRECT(COUNTER_BASE, COUNTER_TICKCNT_REG, TIMER_TICKS_1US);

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Close the timer module
*/
/*----------------------------------------------------------------------------*/
void timer_close(void)
{
    /* Disable the counter by writing zero to the tickcnt register*/
    IOWR_32DIRECT(COUNTER_BASE, COUNTER_TICKCNT_REG, 0);
}

/*----------------------------------------------------------------------------*/
/**
\brief    Get current system tick

This function returns the current system tick determined by the system timer.

\return Returns the system tick in microseconds
*/
/*----------------------------------------------------------------------------*/
UINT16 timer_getTickCount(void)
{
    UINT32 time;

    time = IORD_32DIRECT(COUNTER_BASE, COUNTER_TIME_REG);

    return (UINT16)time;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Set the current system tick to a desired value

\param[in] newVal_p     The new value for the timer
*/
/*----------------------------------------------------------------------------*/
void timer_setTickCount(UINT16 newVal_p)
{
    /* Disable counter */
    IOWR_32DIRECT(COUNTER_BASE, COUNTER_TICKCNT_REG, 0);

    /* Write new value */
    IOWR_32DIRECT(COUNTER_BASE, COUNTER_TIME_REG, newVal_p);

    /* Re-enable counter */
    IOWR_32DIRECT(COUNTER_BASE, COUNTER_TICKCNT_REG, TIMER_TICKS_1US);
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/**
 * \}
 * \}
 */
