/**
********************************************************************************
\file   rpdo.c

\brief  Module for the synchronous receive PDO exchange.

This module forwards the mapped PDO's from the payload to the corresponding
receive PDO triple buffers.

\ingroup module_rpdo
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

#include <psi/rpdo.h>

#include <psi/pdo.h>
#include <psi/tbuf.h>
#include <psi/status.h>

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
\brief Rpdo channel instance type

The rpdo instance structure holds information about module internal data.
*/
typedef struct
{
    tTbufInstance        pTbufInstance_m;      ///< Instance pointer to the triple buffer
    tTbufRpdoImage*      pTbufBase_m;          ///< Base address of triple buffer
} tRpdoInstance;

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static tRpdoInstance rpdoInstance_l;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static tPsiStatus rpdo_updateRelativeTime(void);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Create an rpdo instance

Creates an rpdo instance and also instantiates a tbuf module for the rpdo.

\param[in] pInitParam_p       The initialization parameters of the rpdo

\return tPsiStatus
\retval kPsiSuccessful         On Success
\retval kPsiRpdoInitError      Unable to initialize the module

\ingroup module_rpdo
*/
//------------------------------------------------------------------------------
tPsiStatus rpdo_init(tRpdoInitStruct* pInitParam_p)
{
    tPsiStatus ret = kPsiSuccessful;
    tTbufInitStruct tbufInitParam;

    PSI_MEMSET(&rpdoInstance_l, 0 , sizeof(tRpdoInstance));

#if _DEBUG
    if(pInitParam_p->tbufSize_m != sizeof(tTbufRpdoImage))
    {
        ret = kPsiRpdoBufferSizeMismatch;
        goto Exit;
    }
#endif

    // init the triple buffer module
    tbufInitParam.id_m = pInitParam_p->id_m;
    tbufInitParam.pAckBase_m = pInitParam_p->pProdAckBase_m;
    tbufInitParam.pBase_m = (UINT8 *)pInitParam_p->pTbufBase_m;
    tbufInitParam.size_m = pInitParam_p->tbufSize_m;

    rpdoInstance_l.pTbufInstance_m = tbuf_create(&tbufInitParam);
    if(rpdoInstance_l.pTbufInstance_m == NULL)
    {
        ret = kPsiRpdoInitError;
        goto Exit;
    }

    // Remember base address of the triple buffer
    rpdoInstance_l.pTbufBase_m = pInitParam_p->pTbufBase_m;

Exit:
    return ret;
}



//------------------------------------------------------------------------------
/**
\brief    Destroy an rpdo module

\ingroup module_rpdo
*/
//------------------------------------------------------------------------------
void rpdo_exit(void)
{
    // Free allocated memory!
}

//------------------------------------------------------------------------------
/**
\brief    Link all RPDOs to the PDO data structure

\return tPsiStatus
\retval kPsiSuccessful                On success
\retval kPsiMainInitObjLinkFailed     Unable to link PDO to memory

\ingroup module_rpdo
*/
//------------------------------------------------------------------------------
tPsiStatus rpdo_linkRpdos(void)
{
    tPsiStatus     ret = kPsiSuccessful;
    UINT8            i;
    tObjLinkingData  initObjList[RPDO_NUM_OBJECTS] = RPDO_LINKING_LIST_INIT_VECTOR;
    tTbufRpdoImage*  pTargBase = rpdo_getBaseAddr();
    UINT32           destOffset;

    for(i=0; i < RPDO_NUM_OBJECTS; i++)
    {
        // Add offset of relative time to mapping object offset
        destOffset = initObjList[i].objDestOffset + TBUF_RPDO_MAPPED_OBJ_OFF;

        // Link object to address
        ret = psi_linkPdo(initObjList[i].objIdx, initObjList[i].objSubIdx,
                (UINT8 *)pTargBase, destOffset, initObjList[i].objSize );
        if(ret != kPsiSuccessful)
        {
            break;
        }
    }

    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Access to the rpdo is finished

\ingroup module_rpdo
*/
//------------------------------------------------------------------------------
void rpdo_procFinished(void)
{
    // Update the relative time
    rpdo_updateRelativeTime();

    // Acknowledge triple buffer
    tbuf_setAck(rpdoInstance_l.pTbufInstance_m);
}

//------------------------------------------------------------------------------
/**
\brief    Get the base address of the rpdo triple buffer

\return tTbufRpdoImage
\retval Pointer             Pointer to base address

\ingroup module_rpdo
*/
//------------------------------------------------------------------------------
tTbufRpdoImage* rpdo_getBaseAddr(void)
{
    return rpdoInstance_l.pTbufBase_m;
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

//------------------------------------------------------------------------------
/**
\brief    Update the RPDO relative time

\return tPsiStatus
\retval kPsiSuccessful        On success
\retval kPsiTbuffWriteError   Error on writing

\ingroup module_rpdo
*/
//------------------------------------------------------------------------------
static tPsiStatus rpdo_updateRelativeTime(void)
{
    tPsiStatus ret = kPsiSuccessful;
    UINT32 relativeTimeLow;

    // Get relative time from status module
    status_getRelativeTimeLow(&relativeTimeLow);

    // Write relative time to RPDO image
    ret = tbuf_writeDword(rpdoInstance_l.pTbufInstance_m, TBUF_RPDO_RELTIME_OFF, relativeTimeLow);

    return ret;
}

/// \}


