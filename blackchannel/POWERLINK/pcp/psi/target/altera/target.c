/**
********************************************************************************
\file   target.c

\brief  Target specific file for Altera Nios II

The file implements target specific functions for Altera Nios II used by the
slim interface.

\ingroup module_psi_target
*******************************************************************************/

/*------------------------------------------------------------------------------
Copyright (c) 2014, B&R Industrial Automation GmbH

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holders nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <pcptarget/target.h>

#include <system.h>
#include <altera_avalon_pio_regs.h>
#include <sys/alt_irq.h>
#include <sys/alt_alarm.h>

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief  Gets the node switch value

The function returns the node ID set by the node switches.

\return Returns the set node ID

\ingroup module_psi_target
*/
//------------------------------------------------------------------------------
UINT8 target_getNodeid(void)
{
    UINT8 nodeid;
#ifdef NODE_SWITCH_PIO_BASE
    nodeid = IORD_ALTERA_AVALON_PIO_DATA(NODE_SWITCH_PIO_BASE);
#else
    nodeid = 0;
#endif

    return nodeid;
}

//------------------------------------------------------------------------------
/**
\brief    Enter or leave the critical section

This function enabels/disables global interrupts and implementes the critical
section.

\param  fEnable_p               TRUE = enable interrupts
                                FALSE = disable interrupts

\ingroup module_psi_target
*/
//------------------------------------------------------------------------------
void target_criticalSection(BYTE fEnable_p)
{
static alt_irq_context  irq_context = 0;
static int              iLockCount = 0;

    if (fEnable_p != FALSE)
    {   // restore interrupts
        if (--iLockCount == 0)
        {
            alt_irq_enable_all(irq_context);
        }
    }
    else
    {   // disable interrupts
        if (iLockCount == 0)
        {
            irq_context = alt_irq_disable_all();
        }
        iLockCount++;
    }
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

///\}
