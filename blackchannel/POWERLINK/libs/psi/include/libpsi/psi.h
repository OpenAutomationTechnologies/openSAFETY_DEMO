/**
********************************************************************************
\file   libpsi/psi.h

\brief  Header file for slim interface module

This file contains definitions for the slim interface library. All public
headers are included in this file which makes this file the only header which
needs to be included by the user application.

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

#ifndef _INC_libpsi_psi_H_
#define _INC_libpsi_psi_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/

#include <libpsi/apglobal.h>

#include <libpsi/stream.h>
#include <libpsi/status.h>
#include <libpsi/pdo.h>
#include <libpsi/ssdo.h>
#include <libpsi/logbook.h>
#include <libpsi/cc.h>



/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

/**
 * \brief Application interface initialization parameters
 */
typedef struct {
    tBuffDescriptor*    pBuffDescList_m;      /**< Triple buffer descriptor list */
    tErrorHandler       pfnErrorHandler_m;    /**< Error handler callback function */
    tStreamHandler      pfnStreamHandler_m;   /**< Stream handler callback function */
    tTbufNumLayout      idConsAck_m;          /**< Id of the consumer acknowledge register */
    tTbufNumLayout      idProdAck_m;          /**< Id of the producer acknowledge register */
    tTbufNumLayout      idFirstProdBuffer_m;  /**< Id of the first producing buffer */
} tPsiInitParam;

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/
DLLEXPORT BOOL psi_init(tPsiInitParam* pInitParam_p);
DLLEXPORT void psi_exit(void);

DLLEXPORT BOOL psi_processSync(void);

DLLEXPORT BOOL psi_processPostTransferActions(void);

DLLEXPORT BOOL psi_processAsync(void);

#endif /* _INC_libpsi_psi_H_ */


