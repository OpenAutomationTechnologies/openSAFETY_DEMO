/**
********************************************************************************
\file   tpdo.c

\brief  Module for the synchronous receive PDO exchange.

This module forwards the mapped PDO's from the payload to the corresponding
receive PDO triple buffers.

\ingroup module_tpdo
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

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <psi/tpdo.h>

#include <psi/pdo.h>
#include <psi/tbuf.h>

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

/**
\brief Tpdo channel instance type

The tpdo instance structure holds information about module internal data.
*/
typedef struct
{
    tTbufInstance        pTbufInstance_m;      ///< Instance pointer to the triple buffer
    tTbufTpdoImage*      pTbufBase_m;          ///< Base address of triple buffer
} tTpdoInstance;

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static tTpdoInstance tpdoInstance_l;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------


//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Create an tpdo instance

Creates an tpdo instance and also instantiates a tbuf module for the tpdo.

\param[in] pInitParam_p       The initialization parameters of the tpdo

\return tPsiStatus
\retval kPsiSuccessful         On Success
\retval kPsiRpdoInitError      Unable to initialize the module

\ingroup module_tpdo
*/
//------------------------------------------------------------------------------
tPsiStatus tpdo_init(tTpdoInitStruct* pInitParam_p)
{
    tPsiStatus ret = kPsiSuccessful;
    tTbufInitStruct tbufInitParam;

    PSI_MEMSET(&tpdoInstance_l, 0 , sizeof(tTpdoInstance));

#if _DEBUG
    if(pInitParam_p->tbufSize_m != sizeof(tTbufTpdoImage))
    {
        ret = kPsiTpdoBufferSizeMismatch;
        goto Exit;
    }
#endif

    // init the triple buffer module
    tbufInitParam.id_m = pInitParam_p->id_m;
    tbufInitParam.pAckBase_m = pInitParam_p->pConsAckBase_m;
    tbufInitParam.pBase_m = (UINT8 *)pInitParam_p->pTbufBase_m;
    tbufInitParam.size_m = pInitParam_p->tbufSize_m;

    tpdoInstance_l.pTbufInstance_m = tbuf_create(&tbufInitParam);
    if(tpdoInstance_l.pTbufInstance_m == NULL)
    {
        ret = kPsiTpdoInitError;
        goto Exit;
    }

    // Remember base address of the triple buffer
    tpdoInstance_l.pTbufBase_m = pInitParam_p->pTbufBase_m;

Exit:
    return ret;
}



//------------------------------------------------------------------------------
/**
\brief    Destroy an tpdo module

\ingroup module_tpdo
*/
//------------------------------------------------------------------------------
void tpdo_exit(void)
{
    // Free allocated memory!
}

//------------------------------------------------------------------------------
/**
\brief    Link all TPDOs to the PDO data structure

\return tPsiStatus
\retval kPsiSuccessful                On success
\retval kPsiMainInitObjLinkFailed     Unable to link PDO to memory

\ingroup module_tpdo
*/
//------------------------------------------------------------------------------
tPsiStatus tpdo_linkTpdos(void)
{
    tPsiStatus     ret = kPsiSuccessful;
    UINT8            i;
    tObjLinkingData  initObjList[TPDO_NUM_OBJECTS] = TPDO_LINKING_LIST_INIT_VECTOR;

    tTbufTpdoImage*  pTargBase = tpdo_getBaseAddr();

    for(i=0; i < TPDO_NUM_OBJECTS; i++)
    {
        // Link object to address
        ret = psi_linkPdo(initObjList[i].objIdx, initObjList[i].objSubIdx,
                (UINT8 *)pTargBase, initObjList[i].objDestOffset, initObjList[i].objSize);
        if(ret != kPsiSuccessful)
        {
            break;
        }
    }

    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Access to the tpdo is finished

\ingroup module_tpdo
*/
//------------------------------------------------------------------------------
void tpdo_procFinished(void)
{
    // Acknowledge triple buffer
    tbuf_setAck(tpdoInstance_l.pTbufInstance_m);
}

//------------------------------------------------------------------------------
/**
\brief    Get the base address of the tpdo triple buffer

\return tTbufTpdoImage
\retval Pointer             Pointer to base address

\ingroup module_tpdo
*/
//------------------------------------------------------------------------------
tTbufTpdoImage* tpdo_getBaseAddr(void)
{
    return tpdoInstance_l.pTbufBase_m;
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{


/// \}


