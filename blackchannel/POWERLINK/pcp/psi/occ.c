/**
********************************************************************************
\file   occ.c

\brief  Configuration channel output module

Output module of the configuration channel module. Passes the objects to the
triple buffers.

\ingroup module-occ
*******************************************************************************/

/*------------------------------------------------------------------------------
* License Agreement
*
* Copyright (c) 2014, B&R Industrial Automation GmbH
* Copyright (c) 2016, Kalycito Infotech Private Ltd
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

#include <psi/occ.h>

#include <psi/tbuf.h>
#include <libpsicommon/ccobject.h>
#include <psi/obdict.h>

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

extern tOplkError cc_obdAccessCb(tObdAlConHdl* pParam_p);

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
\brief Configuration channel output instance type

The configuration channel output instance holds information about the
output triple buffer.
*/
typedef struct
{
    tTbufInstance        pTbufInstance_m;      ///< Instance pointer to the triple buffer
    UINT8                fSeqNr_m;             ///< Sequence flag to indicate new data
    UINT16               objSize_m;            ///< Size of an incomming object
} tConfChanOutInstance;

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static tConfChanOutInstance          occInstance_l;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

static tPsiStatus occ_postObject(UINT8 seqNr_p, tConfChanObject* pObject_p);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Initialize the configuration channel output module

\param[in] pInitParam_p            Occ initialization parameters

\return  tPsiStatus
\retval  kPsiSuccessful              On success
\retval  kPsiConfChanInitError       Unable to init conf chan module

\ingroup module_occ
*/
//------------------------------------------------------------------------------
tPsiStatus occ_init(tOccInitStruct* pInitParam_p)
{
    tPsiStatus ret = kPsiSuccessful;
    tTbufInitStruct tbufInitParam;

    PSI_MEMSET(&occInstance_l, 0 , sizeof(tConfChanOutInstance));

#if _DEBUG
    if(pInitParam_p->tbufSize_m != sizeof(tTbufCcStructure))
    {
        ret = kPsiConfChanBufferSizeMismatch;
        goto Exit;
    }
#endif

    // init the triple buffer module
    tbufInitParam.id_m = pInitParam_p->id_m;
    tbufInitParam.pAckBase_m = pInitParam_p->pProdAckBase_m;
    tbufInitParam.pBase_m = (UINT8 *)pInitParam_p->pTbufBase_m;
    tbufInitParam.size_m = pInitParam_p->tbufSize_m;

    occInstance_l.pTbufInstance_m = tbuf_create(&tbufInitParam);
    if(occInstance_l.pTbufInstance_m == NULL)
    {
        ret = kPsiConfChanInitError;
        goto Exit;
    }

Exit:
    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Destroy the configuration channel output module

\ingroup module_occ
*/
//------------------------------------------------------------------------------
void occ_exit(void)
{
    // Destroy triple buffer
    tbuf_destroy(occInstance_l.pTbufInstance_m);

}

//------------------------------------------------------------------------------
/**
\brief    Handle outgoing objects

Grab the next object out of the object list and forward it to the triple buffer.
(This function is called in interrupt context)

\return  tPsiStatus
\retval  kPsiSuccessful              On success
\retval  kPsiConfChanObjectNotFound  Object not found in list
\retval  other error                   Unable to forward objects to buffer

\ingroup module_occ
*/
//------------------------------------------------------------------------------
tPsiStatus occ_handleOutgoing(void)
{
    tPsiStatus ret = kPsiSuccessful;
    tConfChanObject *pObject;

    // Get object from object list
    pObject = ccobject_readCurrObject();
    if(pObject == NULL)
    {
        goto Exit;
    }

    // Change sequence flag
    if(occInstance_l.fSeqNr_m == 0x00)
        occInstance_l.fSeqNr_m = 0x01;
    else
        occInstance_l.fSeqNr_m = 0x00;

    ret = occ_postObject(occInstance_l.fSeqNr_m, pObject);
    if(ret != kPsiSuccessful)
    {
        goto Exit;
    }

    // Acknowledge producing buffer
    ret = tbuf_setAck(occInstance_l.pTbufInstance_m);
    if(ret != kPsiSuccessful)
    {
        goto Exit;
    }

    // Step to next object
    ccobject_incObjReadPointer();

Exit:
    return ret;
}

//------------------------------------------------------------------------------
/**
\brief    Configuration channel object access occurred

In case of an access to the local obdict.h of objects which are configuration
channel objects the local object list needs to be forwarded.

\param[in] pParam_p       Object access parameter.

\return    tOplkError
\retval    kErrorOk       On success.

\ingroup   module_main
*/
//------------------------------------------------------------------------------
tOplkError cc_obdAccessCb(tObdAlConHdl* pParam_p)
{
    tOplkError       oplkret = kErrorOk;
    tConfChanObject  object;

    PSI_MEMSET(&object, 0, sizeof(tConfChanObject));

    if(pParam_p == NULL)
    {
        oplkret = kErrorApiInvalidParam;
        goto Exit;
    }

    // Make object size global
    occInstance_l.objSize_m = pParam_p->totalPendSize;

    object.objIdx_m = pParam_p->index;
    object.objSubIdx_m = pParam_p->subIndex;
    object.objSize_m = (UINT16)occInstance_l.objSize_m;
    PSI_MEMCPY(&object.objPayloadLow_m, pParam_p->pSrcData, occInstance_l.objSize_m);

    if(ccobject_writeObject(&object) == FALSE)
    {
        oplkret = kErrorObdAccessViolation;
    }

Exit:
    return oplkret;
}


//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

//------------------------------------------------------------------------------
/**
\brief    Write object to triple buffer

\param[in] seqNr_p       Sequence number of the new message
\param[in] pObject_p     Pointer to object data to write

\return  tPsiStatus
\retval  kPsiSuccessful              On success
\retval  kPsiTbuffWriteError         Unable to write to buffer

\ingroup module_occ
*/
//------------------------------------------------------------------------------
static tPsiStatus occ_postObject(UINT8 seqNr_p, tConfChanObject* pObject_p)
{
    tPsiStatus ret = kPsiSuccessful;

    ret = tbuf_writeByte(occInstance_l.pTbufInstance_m, TBUF_SEQNR_OFF,
            seqNr_p);
    if(ret != kPsiSuccessful)
    {
        goto Exit;
    }

    ret = tbuf_writeByte(occInstance_l.pTbufInstance_m, TBUF_OBJSUBIDX_OFF,
            pObject_p->objSubIdx_m);
    if(ret != kPsiSuccessful)
    {
        goto Exit;
    }

    ret = tbuf_writeWord(occInstance_l.pTbufInstance_m, TBUF_OBJIDX_OFF,
            pObject_p->objIdx_m);
    if(ret != kPsiSuccessful)
    {
        goto Exit;
    }

    switch(pObject_p->objSize_m)
    {
        case sizeof(UINT8):
        {
            ret = tbuf_writeByte(occInstance_l.pTbufInstance_m, TBUF_PAYLOADLOW_OFF,
                    pObject_p->objPayloadLow_m);
            break;
        }
        case sizeof(UINT16):
        {
            ret = tbuf_writeWord(occInstance_l.pTbufInstance_m, TBUF_PAYLOADLOW_OFF,
                    pObject_p->objPayloadLow_m);
            break;
        }
        case sizeof(UINT32):
        {
            ret = tbuf_writeDword(occInstance_l.pTbufInstance_m, TBUF_PAYLOADLOW_OFF,
                    pObject_p->objPayloadLow_m);
            break;
        }
        case sizeof(UINT64):
        {
            ret = tbuf_writeDword(occInstance_l.pTbufInstance_m, TBUF_PAYLOADLOW_OFF,
                    pObject_p->objPayloadLow_m);
            if(ret != kPsiSuccessful)
            {
                goto Exit;
            }

            ret = tbuf_writeDword(occInstance_l.pTbufInstance_m, TBUF_PAYLOADHIGH_OFF,
                    pObject_p->objPayloadHigh_m);
            break;
        }
        default:
        {
            ret = kPsiConfChanInvalidSizeOfObj;
            break;
        }
    }


Exit:
    return ret;
}

/// \}
