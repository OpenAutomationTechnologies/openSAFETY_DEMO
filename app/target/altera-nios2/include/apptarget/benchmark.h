/**
********************************************************************************
\file   altera-nios2/include/apptarget/benchmark.h

\brief  Header file for debugging. Enables setting of benchmark pins

This header is used to set benchmark pins in order to enable timint
measurements on the ap processor.

*******************************************************************************/

/*------------------------------------------------------------------------------
Copyright (c) 2013, B&R Industrial Automation GmbH
All rights reserved.

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

#ifndef _INC_apptarget_benchmark_H_
#define _INC_apptarget_benchmark_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/
#include <apptarget/target.h>


/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/
#include "system.h"

#ifdef APP_0_BENCHMARK_PIO_BASE
    #include "altera_avalon_pio_regs.h"       /* PIO access */

    #if APP_0_BENCHMARK_PIO_BIT_MODIFYING_OUTPUT_REGISTER == 0
        #error Please enable individual bit setting/clearing of output register for benchmark_pio module in SOPC Builder
    #endif
    #define BENCHMARK_SET(x)    IOWR_ALTERA_AVALON_PIO_SET_BITS(APP_0_BENCHMARK_PIO_BASE, \
                                (1 << (x)))
    #define BENCHMARK_RESET(x)  IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(APP_0_BENCHMARK_PIO_BASE, \
                                (1 << (x)))
    #define BENCHMARK_TOGGLE(x) IOWR_ALTERA_AVALON_PIO_DATA(APP_0_BENCHMARK_PIO_BASE, \
                                ((IORD_ALTERA_AVALON_PIO_DATA(APP_0_BENCHMARK_PIO_BASE)) \
                                ^ (1 << (x))))
#else
    #undef BENCHMARK_MODULES
    #define BENCHMARK_MODULES           0x00000000
#endif

/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/

#endif /* _INC_apptarget_benchmark_H_ */

