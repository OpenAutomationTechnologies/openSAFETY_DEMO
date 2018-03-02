/**
********************************************************************************
\file   TSTccReadWriteObject.c

\brief  Test cc module read and write object API functions

\ingroup module_unittests
*******************************************************************************/

/*------------------------------------------------------------------------------
* License Agreement
*
* Copyright (c) 2013, B&R Industrial Automation GmbH
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
#include <cunit/CUnit.h>

#include <Driver/TSTccConfig.h>
#include <Stubs/STBdescList.h>
#include <Stubs/STBdummyHandler.h>

#include <libpsi/internal/cc.h>
#include <libpsicommon/ccobject.h>
#include <libpsi/internal/stream.h>

#if (((PSI_MODULE_INTEGRATION) & (PSI_MODULE_CC)) != 0)

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
#define OBJECT_INVALID_INDEX        0xFFFF
#define OBJECT_INVALID_SUBINDEX     0xF

#define VALID_DUMMY_OBJECT          0

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static BOOL enRxObject_l = FALSE;
static tCcObject objRxStateList_l[CONF_CHAN_NUM_OBJECTS] = CCOBJECT_LIST_INIT_VECTOR;
static UINT16 currObj_l = 0;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static BOOL streamHandlerRxObjects(tHandlerParam* pHandlParam_p);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Full init with valid parameters

\return int
\retval 0       Init successful
\retval other   Init failed

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
int TST_initFull(void)
{
    BOOL fReturn;
    tStreamInitParam streamInitParam;
    tCcInitParam ccInitParam;
    tStatusInitParam statusInitParam;

    stb_initBuffers();

    // Call init of stream module
    streamInitParam.pfnStreamHandler_m = streamHandlerRxObjects;
    streamInitParam.pBuffDescList_m = stb_getDescList();
    streamInitParam.idConsAck_m = (tTbufNumLayout)0;
    streamInitParam.idFirstProdBuffer_m = (tTbufNumLayout)(TBUF_NUM_CON + 1);

    fReturn = stream_init(&streamInitParam);

    if(fReturn != FALSE)
    {
        statusInitParam.buffInId_m = kTbufNumStatusIn;
        statusInitParam.buffOutId_m = kTbufNumStatusOut;
        statusInitParam.pfnProcSyncCb_m = stb_dummySyncHandlerSuccess;

        fReturn = status_init(&statusInitParam);

        if(fReturn != FALSE)
        {
            // Call init of CC module
            ccInitParam.pfnCritSec_p = stb_dummyCriticalSection;
            ccInitParam.iccId_m = kTbufNumInputConfChan;
            ccInitParam.occId_m = kTbufNumOutputConfChan;

            fReturn = cc_init(&ccInitParam);
        }
    }

    return (fReturn != FALSE) ? 0 : 1;
}

//------------------------------------------------------------------------------
/**
\brief Test cc read object API function

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_ccReadObject(void)
{
    tConfChanObject* pReadObj;
    tCcObject        initObjList[CONF_CHAN_NUM_OBJECTS] = CCOBJECT_LIST_INIT_VECTOR;
    UINT16 objIdx;
    UINT8 objSubIdx;

    // Read not existing object
    pReadObj = cc_readObject(OBJECT_INVALID_INDEX, OBJECT_INVALID_SUBINDEX);

    CU_ASSERT_EQUAL( pReadObj, NULL );

    // Read existing object
    objIdx = initObjList[VALID_DUMMY_OBJECT].objIdx;
    objSubIdx = initObjList[VALID_DUMMY_OBJECT].objSubIdx;

    pReadObj = cc_readObject(objIdx, objSubIdx);

    CU_ASSERT_NOT_EQUAL( pReadObj, NULL );
}

//------------------------------------------------------------------------------
/**
\brief Test cc write object API function

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
void TST_ccWriteObject(void)
{
    BOOL fReturn;
    tConfChanObject dummyObj;
    tCcObject       initObjList[CONF_CHAN_NUM_OBJECTS] = CCOBJECT_LIST_INIT_VECTOR;
    UINT16 i;
    tCcWriteStatus writeState;

    // Perform write with no parameters
    writeState = cc_writeObject(NULL);

    CU_ASSERT_EQUAL( writeState, kCcWriteStatusError );

    // Perform write with invalid parameters
    dummyObj.objSize_m = sizeof(UINT64) + 1;

    writeState = cc_writeObject(&dummyObj);

    CU_ASSERT_EQUAL( writeState, kCcWriteStatusError );

    dummyObj.objSize_m = 0;

    writeState = cc_writeObject(&dummyObj);

    CU_ASSERT_EQUAL( writeState, kCcWriteStatusError );

    // Write with invalid object details but correct size
    dummyObj.objIdx_m = 0xFFFF;
    dummyObj.objSubIdx_m = 0xFF;
    dummyObj.objSize_m = 2;

    writeState = cc_writeObject(&dummyObj);

    CU_ASSERT_EQUAL( writeState, kCcWriteStatusError );

    // Perform valid write of object
    dummyObj.objIdx_m = initObjList[VALID_DUMMY_OBJECT].objIdx;
    dummyObj.objSubIdx_m = initObjList[VALID_DUMMY_OBJECT].objSubIdx;
    dummyObj.objSize_m = initObjList[VALID_DUMMY_OBJECT].objSize;
    dummyObj.objPayloadLow_m = 0xFFEEAABB;
    dummyObj.objPayloadHigh_m = 0x00;

    writeState = cc_writeObject(&dummyObj);

    CU_ASSERT_EQUAL( writeState, kCcWriteStatusSuccessful );

    cc_process();

    // Perform second write to signal channel busy
    writeState = cc_writeObject(&dummyObj);

    CU_ASSERT_EQUAL( writeState, kCcWriteStatusBusy );

    // Process sync until timeout occures
    for(i=0; i < CC_TX_TIMEOUT_CYCLE_COUNT + 1; i++)
    {
        fReturn = stream_processSync();
        CU_ASSERT_TRUE( fReturn );

        fReturn = stream_processPostActions();
        CU_ASSERT_TRUE( fReturn );
    }

    // Process for timeout
    cc_process();

    // Write again to post an other frame
    writeState = cc_writeObject(&dummyObj);

    CU_ASSERT_EQUAL( writeState, kCcWriteStatusSuccessful );

    // Process new frame
    cc_process();

    // Process sync to enable rx receive
    enRxObject_l = TRUE;

    for(i=0; i < CONF_CHAN_NUM_OBJECTS; i++)
    {
        fReturn = stream_processSync();
        CU_ASSERT_TRUE( fReturn );

        fReturn = stream_processPostActions();
        CU_ASSERT_TRUE( fReturn );
    }

    // Cleanup cc module
    cc_exit();
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

//------------------------------------------------------------------------------
/**
\brief Stream handler which simulates arriving objects

\param pHandlParam_p        Stream handler parameter

\return BOOL
\retval TRUE        Handler processing successful
\retval FALSE       Error in stream handler

\ingroup module_unittests
*/
//------------------------------------------------------------------------------
static BOOL streamHandlerRxObjects(tHandlerParam* pHandlParam_p)
{
    tBuffDescriptor* pBuffDesc;
    tTbufCcStructure* pOccRxStruct;

    UNUSED_PARAMETER(pHandlParam_p);

    if(enRxObject_l)
    {
        // Get occ receive buffer descriptor
        pBuffDesc = stb_getDescElement(kTbufNumOutputConfChan);

        pOccRxStruct = (tTbufCcStructure*)pBuffDesc->pBuffBase_m;

        // Object RX test enabled
        pOccRxStruct->objIdx_m = objRxStateList_l[currObj_l].objIdx;
        pOccRxStruct->objSubIdx_m = objRxStateList_l[currObj_l].objSubIdx;
        pOccRxStruct->objPayloadLow_m = 0;
        pOccRxStruct->objPayloadHigh_m = 0;

        currObj_l++;

        if(currObj_l > CONF_CHAN_NUM_OBJECTS)
        {
            currObj_l = 0;
        }
    }

    return TRUE;
}

/// \}

#endif // #if (((PSI_MODULE_INTEGRATION) & (PSI_MODULE_CC)) != 0)