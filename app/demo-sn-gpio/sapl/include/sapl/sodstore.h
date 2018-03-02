/**
********************************************************************************
\file   demo-sn-gpio/sapl/include/sapl/sodstore.h

\brief  This module implements the store and restore mechanism of the SOD.

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

#ifndef _INC_sapl_sodstore_H_
#define _INC_sapl_sodstore_H_

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
 * \brief Return value of the SOD store processing
 */
typedef enum
{
    kSodStoreProcError          = 0x00,     /**< Error during SOD store processing */
    kSodStoreProcFinished       = 0x01,     /**< Storing of the SOD to NVS finished */
    kSodStoreProcBusy           = 0x02,     /**< Storing is currently busy */
    kSodStoreProcNotApplicable  = 0x03,     /**< Storing the SOD image is not applicable (The NVM is not empty!) */
} tProcStoreRet;

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
    extern "C" {
#endif

BOOLEAN sodstore_init(void);
void sodstore_close(void);

BOOLEAN sodstore_prepareStorage(void);
tProcStoreRet sodstore_process(UINT8* pParamSetBase_p, UINT32 paramSetLen_p);

BOOLEAN sodstore_getSodImage(UINT8** ppParamSetBase_p, UINT32* pParamSetLen_p);

#ifdef __cplusplus
    }
#endif


#endif /* _INC_sapl_sodstore_H_ */
