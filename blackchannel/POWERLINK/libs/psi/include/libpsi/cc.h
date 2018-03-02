/**
********************************************************************************
\file   libpsi/cc.h

\brief  Application interface configuration channel module header

This header provides all public functions or types of the cc module. It
is the user interface to the cc module.

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

#ifndef _INC_libpsi_cc_H_
#define _INC_libpsi_cc_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/

#include <libpsi/apglobal.h>

#include <libpsicommon/cc.h>

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

/**
 * \brief Status of the configuration channel write object
 */
typedef enum {
    kCcWriteStatusError       = 0x00,   /**< Error on writing an object */
    kCcWriteStatusSuccessful = 0x01,    /**< Writing the object successful */
    kCcWriteStatusBusy        = 0x02,   /**< Unable to write the object! Channel is busy */
} tCcWriteStatus;

/**
 * \brief  Cc module initialization structure
 */
typedef struct {
    tTbufNumLayout     iccId_m;          /**< Id of the icc buffer */
    tTbufNumLayout     occId_m;          /**< Id of the occ buffer */
    tPsiCritSec      pfnCritSec_p;       /**< Pointer to critical section entry function */
} tCcInitParam;


/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/
DLLEXPORT BOOL cc_init(tCcInitParam* pCcInitParam_p);
DLLEXPORT void cc_exit(void);

DLLEXPORT tCcWriteStatus cc_writeObject(tConfChanObject* pObject_p);
DLLEXPORT tConfChanObject* cc_readObject(UINT16 objIdx_p, UINT8 objSubIdx_p);

#endif /* _INC_libpsi_cc_H_ */
