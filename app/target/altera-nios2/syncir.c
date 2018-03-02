/**
********************************************************************************
\file   target/altera-nios2/syncir.c

\defgroup module_targ_nios2_syncir Synchronous interrupt module
\{

\brief  Implements the driver for the synchronous interrupt

Defines the platform specific functions for the synchronous interrupt for target
Altera Nios2.

\ingroup group_app_targ_nios2
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
#include <common/syncir.h>

#include <system.h>
#include <string.h>
#include <stdio.h>

#include <alt_types.h>
#include <sys/alt_irq.h>
#include <altera_avalon_pio_regs.h>


/*============================================================================*/
/*            G L O B A L   D E F I N I T I O N S                             */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

/* SYNC IRQ dependencies */
#if APP_0_SYNC_IRQ_BASE
  #define SYNC_IRQ_NUM                    0     /**< Id of the synchronous interrupt (Workaround: Parameter is not forwarded to system.h when CPU is in a subsystem) */
  #define SYNC_IRQ_BASE                   APP_0_SYNC_IRQ_BASE
  #define APP_INTERRUPT_CONTROLLER_ID     0     /**< Id of the Nios ISR controller (Workaround: Parameter is not forwarded to system.h when CPU is in a subsystem) */
#endif

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
static tPlatformSyncIrq pfnSyncIrq_l = NULL;        /**< Pointer to the sync IR callback function */

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief  Initialize the synchronous interrupt

syncir_init() initializes the synchronous interrupt. The timing parameters
will be initialized, the interrupt handler will be connected to the ISR.

\param[in] pfnSyncIrq_p       The callback of the sync interrupt

\retval TRUE        Synchronous interrupt initialization successful
\retval FALSE       Error while initializing the synchronous interrupt
*/
/*----------------------------------------------------------------------------*/
BOOL syncir_init(tPlatformSyncIrq pfnSyncIrq_p)
{
    BOOL fReturn = FALSE;

    syncir_disable();

    if(pfnSyncIrq_p != NULL)
    {
        pfnSyncIrq_l = pfnSyncIrq_p;

        fReturn = TRUE;
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief  Shutdown the synchronous interrupt
*/
/*----------------------------------------------------------------------------*/
void syncir_exit(void)
{

}

/*----------------------------------------------------------------------------*/
/**
\brief  Acknowledge the synchronous interrupt
*/
/*----------------------------------------------------------------------------*/
void syncir_acknowledge(void)
{
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SYNC_IRQ_BASE, 0x01);
}

/*----------------------------------------------------------------------------*/
/**
\brief  Enable the synchronous interrupt

syncir_enable() enables the synchronous interrupt.
*/
/*----------------------------------------------------------------------------*/
void syncir_enable(void)
{
    IOWR_ALTERA_AVALON_PIO_IRQ_MASK(SYNC_IRQ_BASE, 0x01);

    /* register interrupt handler */
#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
    (void)alt_ic_isr_register(APP_INTERRUPT_CONTROLLER_ID, SYNC_IRQ_NUM, pfnSyncIrq_l, NULL, 0);
#else
    (void)alt_irq_register(SYNC_IRQ_NUM, NULL, pfnSyncIrq_p);
#endif
}

/*----------------------------------------------------------------------------*/
/**
\brief  Disable the synchronous interrupt

syncir_disable() disable the synchronous interrupt.
*/
/*----------------------------------------------------------------------------*/
void syncir_disable(void)
{
    alt_ic_irq_disable(0, SYNC_IRQ_NUM);  /* disable specific IRQ Number */
}


/*----------------------------------------------------------------------------*/
/**
\brief Enter/leave the critical section

This function enables/disables the interrupts of the AP processor

\param[in]  fEnable_p       TRUE = enable interrupts; FALSE = disable interrupts
*/
/*----------------------------------------------------------------------------*/
void syncir_enterCriticalSection(UINT8 fEnable_p)
{

    if(fEnable_p)
    {
        syncir_enable();
    }
    else
    {
        syncir_disable();
    }
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

