/**
********************************************************************************
\file   demo-sn-gpio/sapl/parameterset.c

\defgroup module_sn_sapl_paramset Parameter parser module
\{

\brief  This module handles the parameter download

This module forwards the parameter stream from object 0x10A1 to the object
dictionary and performs checks of the stream.

\ingroup group_app_sn_sapl
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

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/

#include <sapl/parameterset.h>

#include <SODapi.h>
#include <SERRapi.h>
#include <SCFMapi.h>
#include <SHNF.h>
#include <SFS.h>


/*============================================================================*/
/*            G L O B A L   D E F I N I T I O N S                             */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* module global vars                                                         */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* global function prototypes                                                 */
/*----------------------------------------------------------------------------*/


/*============================================================================*/
/*            P R I V A T E   D E F I N I T I O N S                           */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/
#define EPS_PARAMSET_IDX_LEN         2      /**< Size of the param stream index field */
#define EPS_PARAMSET_SUBIDX_LEN      1      /**< Size of the param stream sub index field */
#define EPS_PARAMSET_LENGTH_LEN      4      /**< Size of the param stream length field */


#define EPS_PARAMSET_HEADER_LENGTH      EPS_PARAMSET_IDX_LEN + \
                                        EPS_PARAMSET_SUBIDX_LEN + \
                                        EPS_PARAMSET_LENGTH_LEN        /**< Length of the header of an object in a parameter set (idx, subidx and length)  */

/*----------------------------------------------------------------------------*/
/* local types                                                                */
/*----------------------------------------------------------------------------*/

/**
 * \brief Return value of the object parser
 */
typedef enum
{
    kParseObjInvalid          = 0x0,    /**< Invalid return value  */
    kParseObjFinished         = 0x1,    /**< Finished to parse the object  */
    kParseObjBusy             = 0x2,    /**< Object parser is currently busy  */
    kParseObjError            = 0x3,    /**< Error during oject parsing */
} tParseObjStatus;

/**
 * \brief States of the object parser
 */
typedef enum
{
    kObjStateInvalid            = 0x0,      /**< Invalid state */
    kObjStateGetParameters      = 0x1,      /**< Fetch index, subindex and size from the image */
    kObjStateVerifyParameters   = 0x2,      /**< Verify the parsed attributes from the current object */
    kObjStateGetSodAttributes   = 0x3,      /**< Get the attributes of the current object */
    kObjStateWriteObjectToSod   = 0x4,      /**< Write the valid object to the SOD */
} tObjectState;

/**
 * \brief Stores the attributes of the current handled object
 */
typedef struct
{
    UINT16 index_m;                 /**< The index of the current object */
    UINT8 subIndex_m;               /**< The subindex of the current object */
    UINT32 length_m;                /**< Length of the current object */
    UINT8 * pData_m;                /**< Pointer to the object data */
    SOD_t_ATTR * pCurrSodAttr_m;    /**< Attribute of the current SOD entry */
    UINT32 objHdl_m;                /**< SOD handle of the current object */
} tObjectAttr;

/**
 * \brief Parameter set processing state machine
 */
typedef enum
{
    kParamSetStateInvalid             = 0x0,    /**< Invalid state */
    kParamSetStateInitiateProcessing  = 0x2,    /**< Start the processing of the downloaded parameter set */
    kParamSetStateProcessing          = 0x3,    /**< Process the arrived parameter set */
    kParamSetStateFinishProcessing    = 0x4,    /**< Processing of the parameter set is finished */
} tParamSetProcState;

/**
 * \brief ParameterSet module instance structure
 */
typedef struct
{
    tParamSetProcState paramSetState_m;     /**< Current state of the parameter set processing */
    tObjectState objectState_m;             /**< Processing state of the current object in the parameter set */
    tObjectAttr currObjAttr_m;              /**< Attributes of the current handled object */
} tParamSetInstance;

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/
static tParamSetInstance paramSetInstance_l SAFE_INIT_SEKTOR;

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static tParseObjStatus processParameterSetObject(UINT8 * pObjData_p,
                                                 UINT32 currParamSetPos,
                                                 UINT32 * pCurrObjLen_p,
                                                 UINT32 paramSetLen_p);
static BOOLEAN writeDomainToSod(void);

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the parameter set module

\retval TRUE        Init of the module successful
\retval FALSE       Error on init
*/
/*----------------------------------------------------------------------------*/
BOOLEAN paramset_init(void)
{
    BOOLEAN fReturn = FALSE;

    MEMSET(&paramSetInstance_l, 0, sizeof(tParamSetInstance));

    /* Initialize ParameterSet state machine */
    paramSetInstance_l.paramSetState_m = kParamSetStateInitiateProcessing;
    paramSetInstance_l.objectState_m = kObjStateGetParameters;

    fReturn = TRUE;

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Close the parameter set module
*/
/*----------------------------------------------------------------------------*/
void paramset_exit(void)
{
    /* Nothing to free */
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process the parameter set state machine

This state machine checks if the parameter set image has arrived and iterates
over all objects and forwards them to the SOD.

\param pParamSetBase_p      Base address of the parameter set
\param paramSetLen_p        Length of the parameter set

\retval kParamProcFinished     Parameter set processing finished
\retval kParamProcBusy         Parameter processing is busy
\retval kParamProcError        Error during parameter processing
*/
/*----------------------------------------------------------------------------*/
tProcParamRet paramset_process(UINT8* pParamSetBase_p, UINT32 paramSetLen_p)
{
    tProcParamRet procRet = kParamProcError;
    tParseObjStatus objParseState = kParseObjInvalid;
    static UINT8 * pCurrDataPos = (UINT8 *)NULL;
    static UINT32 currParamSetPos = 0;
    UINT32 currObjLen = 0;

    if(pParamSetBase_p != NULL && paramSetLen_p > 0)
    {
        switch(paramSetInstance_l.paramSetState_m)
        {
            case kParamSetStateInitiateProcessing:
            {
                DEBUG_TRACE(DEBUG_LVL_SAPL, "\nParse parameter set: "
                                              "(Size: %dBytes)\n", paramSetLen_p);

                /* Reset global object attributes structures */
                MEMSET(&paramSetInstance_l.currObjAttr_m, 0, sizeof(tObjectAttr));

                /* Set start of parameter set image */
                pCurrDataPos = pParamSetBase_p;
                currParamSetPos = 0;

                paramSetInstance_l.paramSetState_m = kParamSetStateProcessing;
                procRet = kParamProcBusy;
                break;
            }
            case kParamSetStateProcessing:
            {
                /* Process next parameter set object */
                objParseState = processParameterSetObject(pCurrDataPos, currParamSetPos,
                                                          &currObjLen, paramSetLen_p);
                if(objParseState == kParseObjBusy)
                {
                    /* Object parser is currently busy */
                    procRet = kParamProcBusy;
                }
                else if(objParseState == kParseObjFinished)
                {
                    if(currParamSetPos + currObjLen < paramSetLen_p)
                    {
                        /* Current object finished -> Move data pointer to next object */
                        pCurrDataPos = &pCurrDataPos[currObjLen];
                        currParamSetPos += currObjLen;
                        procRet = kParamProcBusy;
                    }
                    else if(currParamSetPos + currObjLen == paramSetLen_p)
                    {
                        /* End of parameter set reached */
                        paramSetInstance_l.paramSetState_m = kParamSetStateFinishProcessing;
                        procRet = kParamProcBusy;
                    }
                    else
                    {
                        /* Error during parameter set processing */
                        errh_postFatalError(kErrSourceSapl, kErrorParamSetInvalidSize, (currParamSetPos + currObjLen));
                    }
                }
                else
                {
                    /* Error occurred -> Return FALSE */
                    errh_postFatalError(kErrSourceSapl, kErrorParamSetUnableToParseObj, 0);
                }

                break;
            }
            case kParamSetStateFinishProcessing:
            {
                DEBUG_TRACE(DEBUG_LVL_SAPL, "Parsing finished!\n");

                /* Reset global object attributes structures */
                MEMSET(&paramSetInstance_l.currObjAttr_m, 0, sizeof(tObjectAttr));

                paramSetInstance_l.objectState_m = kObjStateGetParameters;

                paramSetInstance_l.paramSetState_m = kParamSetStateInitiateProcessing;

                procRet = kParamProcFinished;
                break;
            }
            default:
            {
                /* Invalid state reached */
                errh_postFatalError(kErrSourceSapl, kErrorInvalidState, 0);

                break;
            }
        }
    }
    else
    {
        /* Parameter set not provided to process function */
        errh_postFatalError(kErrSourceSapl, kErrorInvalidParameter, 0);
    }


    return procRet;
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/*----------------------------------------------------------------------------*/
/**
\brief    Process the current parameter set object

This function parses an object from the parameter set and writes the payload
to the corresponding SOD entry.

\param[in]  pObjData_p      Pointer to the start of the current object
\param[in]  currParamSetPos Current length position in the parameter set
\param[out] pCurrObjLen_p   The length of the current object (Including header)
\param[in]  paramSetLen_p   The length of the parameter set

\retval kParseObjBusy        Object processing state machine is currently busy
\retval kParseObjFinished    Current object processing is finished
\retval kParseObjError       Error during processing
*/
/*----------------------------------------------------------------------------*/
static tParseObjStatus processParameterSetObject(UINT8 * pObjData_p,
                                                 UINT32 currParamSetPos,
                                                 UINT32 * pCurrObjLen_p,
                                                 UINT32 paramSetLen_p)
{
    tParseObjStatus parseObjRet = kParseObjError;
    SOD_t_ERROR_RESULT errRes;
    BOOLEAN fIsApplObj = FALSE;
    /* Consists of the object payload if datatype is known. (Provides aligned address) */
    UINT64 objTmpData = 0;

    switch(paramSetInstance_l.objectState_m)
    {
        case kObjStateGetParameters:
        {
            /*
             * Copy attributes of the current object byte wise because data
             * inside the parameter set is not guaranteed to be aligned in memory!
             */

            /* Get index of the current object */
            SFS_NET_CPY16(&paramSetInstance_l.currObjAttr_m.index_m, &pObjData_p[0]);

            /* Get subindex of the current object */
            SFS_NET_CPY8(&paramSetInstance_l.currObjAttr_m.subIndex_m, &pObjData_p[2]);

            /* Get length of the current object */
            SFS_NET_CPY32(&paramSetInstance_l.currObjAttr_m.length_m, &pObjData_p[3]);

            /* Set data pointer to object payload */
            paramSetInstance_l.currObjAttr_m.pData_m = &pObjData_p[7];

            parseObjRet = kParseObjBusy;
            paramSetInstance_l.objectState_m = kObjStateVerifyParameters;

            DEBUG_TRACE(DEBUG_LVL_SAPL, "Idx: 0x%x Sub: 0x%x Len: %d\n", paramSetInstance_l.currObjAttr_m.index_m,
                                        paramSetInstance_l.currObjAttr_m.subIndex_m,
                                        paramSetInstance_l.currObjAttr_m.length_m);

            break;
        }
        case kObjStateVerifyParameters:
        {
            /* Check if current object is still inside the parameter set image */
            if((currParamSetPos + paramSetInstance_l.currObjAttr_m.length_m) < paramSetLen_p)
            {
                parseObjRet = kParseObjBusy;
                paramSetInstance_l.objectState_m = kObjStateGetSodAttributes;
            }
            else
            {
                /* Return kParseObjError! Length check failed! */
                errh_postFatalError(kErrSourceShnf, kErrorObjTooLongForParameterSet, 0);
            }

            break;
        }
        case kObjStateGetSodAttributes:
        {
            /* Get the SOD entry attribute of the current object */
            paramSetInstance_l.currObjAttr_m.pCurrSodAttr_m = (SOD_t_ATTR *)SOD_AttrGet(B_INSTNUM_
                          paramSetInstance_l.currObjAttr_m.index_m,
                          paramSetInstance_l.currObjAttr_m.subIndex_m,
                          &paramSetInstance_l.currObjAttr_m.objHdl_m,
                          &fIsApplObj, &errRes
                          );
            if(paramSetInstance_l.currObjAttr_m.pCurrSodAttr_m != NULL)
            {
                parseObjRet = kParseObjBusy;
                paramSetInstance_l.objectState_m = kObjStateWriteObjectToSod;
            }   /* no else: Error is already reported via SAPL_SERR_SignalErrorClbk */

            break;
        }
        case kObjStateWriteObjectToSod:
        {
            if((paramSetInstance_l.currObjAttr_m.pCurrSodAttr_m->e_dataType == EPLS_k_DOMAIN) ||
               (paramSetInstance_l.currObjAttr_m.pCurrSodAttr_m->e_dataType == EPLS_k_VISIBLE_STRING) ||
               (paramSetInstance_l.currObjAttr_m.pCurrSodAttr_m->e_dataType == EPLS_k_OCTET_STRING))
            {
                /* Object is from type domain */
                if(writeDomainToSod())
                {
                    parseObjRet = kParseObjFinished;
                    paramSetInstance_l.objectState_m = kObjStateGetParameters;
                }
            }
            else
            {
                /* The object has a known datatype (e.g: UINT32) */
                MEMCOPY(&objTmpData, paramSetInstance_l.currObjAttr_m.pData_m,
                        paramSetInstance_l.currObjAttr_m.length_m);

                if(SOD_Write(B_INSTNUM_
                             paramSetInstance_l.currObjAttr_m.objHdl_m,
                             FALSE,
                             &objTmpData,
                             TRUE, 0, 0))
                {
                    parseObjRet = kParseObjFinished;
                    paramSetInstance_l.objectState_m = kObjStateGetParameters;
                }   /* no else: Error is already reported via SAPL_SERR_SignalErrorClbk */

            }
            break;
        }
        default:
        {
            /* Invalid object parsing state */
            errh_postFatalError(kErrSourceShnf, kErrorInvalidState, 0);
            break;
        }
    }

    /* Set return value to length of the current handled object */
    *pCurrObjLen_p = EPS_PARAMSET_HEADER_LENGTH + paramSetInstance_l.currObjAttr_m.length_m;

    return parseObjRet;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Write the data of a DOMAIN, O/VSTRING to the SOD

\retval TRUE    Write to the SOD successful
\retval FALSE   Unable to write to the SOD
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN writeDomainToSod(void)
{
    BOOLEAN fReturn = FALSE;

    /* Object is a DOMAIN, VSTRING or OSTRING */
    if(SOD_Write(B_INSTNUM_
                 paramSetInstance_l.currObjAttr_m.objHdl_m,
                 FALSE,
                 paramSetInstance_l.currObjAttr_m.pData_m,
                 TRUE, 0,
                 paramSetInstance_l.currObjAttr_m.length_m))
    {
        /* Write the actual length of the object into the SOD */
        if(SOD_ActualLenSet(B_INSTNUM_
                            paramSetInstance_l.currObjAttr_m.objHdl_m,
                            FALSE,
                            paramSetInstance_l.currObjAttr_m.length_m))
        {
            fReturn = TRUE;
        }   /* no else: Error is already reported via SAPL_SERR_SignalErrorClbk */

    }   /* no else: Error is already reported via SAPL_SERR_SignalErrorClbk */

    return fReturn;
}

/**
 * \}
 * \}
 */
