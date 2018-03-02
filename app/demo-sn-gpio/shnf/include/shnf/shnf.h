/**
********************************************************************************
\file   demo-sn-gpio/shnf/include/shnf/shnf.h

\brief  This header implements the interface to the SHNF module

*******************************************************************************/

/*------------------------------------------------------------------------------
Copyright (c) 2014, B&R Industrial Automation GmbH
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

#ifndef _INC_shnf_shnf_H_
#define _INC_shnf_shnf_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/
#include <sn/global.h>

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

/**
 * \brief Type of the synchronous process function
 */
typedef BOOLEAN (*tShnfProcSync)(void);

/**
 * \brief Shnf module initialization parameters
 */
typedef struct
{
    tShnfProcSync pfnProcSync_m;    /**< Pointer to the process sync callback */
    tSyncCycle pfnSyncronize_m;     /**< Pointer to the sync cycle callback function */
} tShnfInitParam;

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
    extern "C" {
#endif

BOOLEAN shnf_init(tShnfInitParam * pInitParam_p);
void shnf_exit(void);

void shnf_reset(void);

BOOLEAN shnf_process(void);

void shnf_enableSyncIr(void);

#ifdef __cplusplus
    }
#endif


#endif /* _INC_shnf_shnf_H_ */
