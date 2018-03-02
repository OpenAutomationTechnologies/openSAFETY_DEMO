/**
********************************************************************************
\file   psi/psi.h

\brief  Header file for the slim interface main module

This file contains definitions for the slim interface main module.

*******************************************************************************/

/*------------------------------------------------------------------------------
Copyright (c) 2018, B&R Industrial Automation GmbH
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

#ifndef _INC_psi_H_
#define _INC_psi_H_

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <psi/pcpglobal.h>

#include <oplk/oplk.h>

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------

/**
 *  \brief Direction of the PDO
 */
typedef enum {
    tPdoDirInvalid = 0x00,
    tPdoDirRpdo    = 0x01,
    tPdoDirTpdo    = 0x02,
} tPsiPdoDir;

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

tPsiStatus psi_init(UINT8 nodeId_p, tPsiCritSec pfnCritSec_p);
void psi_exit(void);
tPsiStatus psi_configureModules(void);
tPsiStatus psi_setNettime(tNetTime * pNetTime_p);

tPsiStatus psi_handleAsync(void);
tPsiStatus psi_handleSync(void);

void psi_pdoProcFinished(tPsiPdoDir pdoDir_p);
tPsiStatus psi_sdoAccFinished(const tSdoComFinished* pSdoComFinParam_p );
tPsiStatus psi_closeSdoChannels(void);


#endif /* _INC_psi_H_ */


