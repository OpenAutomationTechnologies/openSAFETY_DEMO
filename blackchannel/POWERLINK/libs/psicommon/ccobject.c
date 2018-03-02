/**
********************************************************************************
\file   psicommon/ccobject.c

\defgroup module_psicom_ccobject Configuration channel object list module
\{

\brief  Application interface configuration channel object list.

This module implements a list of objects which can be accessed for reading and writing.
This object list is used by the configuration channel to forward configuration data
during runtime.

\ingroup group_libpsicommon
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

#include <libpsicommon/ccobject.h>

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

/*----------------------------------------------------------------------------*/
/* local types                                                                */
/*----------------------------------------------------------------------------*/


/**
\brief Configuration channel user instance type

The configuration channel instance holds the periodic object access information
of the slim interface.
*/
typedef struct
{
    tConfChanObject      objectList_m[CONF_CHAN_NUM_OBJECTS];  /**< List of all objects to transfer */
    UINT8                currReadObj_m;                        /**< Current object read pointer */
    UINT8                currWriteObj_m;                       /**< Current object write pointer */
    tPsiCritSec          pfnCritSec_m;                         /**< Function pointer to the critical section */
} tConfChanInstance;

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/

static tConfChanInstance          ccobjInstance_l;

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/


/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the configuration channel output module

\param[in]  pfnCritSec_p       Pointer to the critical section entry function

\retval TRUE        Object list successfully initialized
\retval FALSE       Error while initializing
*/
/*----------------------------------------------------------------------------*/
BOOL ccobject_init(tPsiCritSec pfnCritSec_p)
{
    BOOL fReturn = FALSE;

    PSI_MEMSET(&ccobjInstance_l, 0 , sizeof(tConfChanInstance));

    if(pfnCritSec_p != NULL)
    {
        ccobjInstance_l.pfnCritSec_m = pfnCritSec_p;
        fReturn = TRUE;
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Destroy object list module
*/
/*----------------------------------------------------------------------------*/
void ccobject_exit(void)
{
    /* Free object list */
}

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize an object in the object list

\param[in]  objId_p         The id of the object
\param[in]  pObjDef_p       Definition of the object

\retval  TRUE      Successfully initialized object
\retval  FALSE     Invalid object initialization parameters
*/
/*----------------------------------------------------------------------------*/
BOOL ccobject_initObject(UINT8 objId_p, tConfChanObject* pObjDef_p)
{
    BOOL fReturn = FALSE;

    if(pObjDef_p != NULL)
    {
        if(objId_p < CONF_CHAN_NUM_OBJECTS)
        {
            /* Copy object to list */
            PSI_MEMCPY(&ccobjInstance_l.objectList_m[objId_p], pObjDef_p,
                    sizeof(tConfChanObject));

            fReturn = TRUE;
        }
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief   Write an object in the object list

\param[in] pObjDef_p        The object to write

\retval  TRUE      Write to object successful
\retval  FALSE     Failed to update the object
*/
/*----------------------------------------------------------------------------*/
BOOL ccobject_writeObject(tConfChanObject* pObjDef_p)
{
    BOOL fReturn = FALSE;
    UINT8 i;

    for(i=0; i < CONF_CHAN_NUM_OBJECTS; i++)
    {
        if(ccobjInstance_l.objectList_m[i].objIdx_m == pObjDef_p->objIdx_m        &&
           ccobjInstance_l.objectList_m[i].objSubIdx_m == pObjDef_p->objSubIdx_m  &&
           ccobjInstance_l.objectList_m[i].objSize_m == pObjDef_p->objSize_m        )
        {
            /* Enter critical section */
            ccobjInstance_l.pfnCritSec_m(FALSE);

            /* object found in list! Remember object data */
            PSI_MEMCPY(&ccobjInstance_l.objectList_m[i].objPayloadLow_m,
                    &pObjDef_p->objPayloadLow_m,
                    pObjDef_p->objSize_m);

            ccobjInstance_l.pfnCritSec_m(TRUE);
            /* Leave critical section */

            fReturn = TRUE;

            break;
        }
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief   Write next object in object list

\param[in] objIdx_p        The index of the object to write
\param[in] objSubIdx_p     The subindex of the object to write
\param[in] pData_p         Pointer to the payload of the object

\retval  kCcWriteStateSuccessful     Successfully written to object list
\retval  kCcWriteStateError          Error while writing to object list
\retval  kCcWriteStateOutOfSync      Local object pointer out of sync
*/
/*----------------------------------------------------------------------------*/
tCcWriteState ccobject_writeCurrObject(UINT16 objIdx_p, UINT8 objSubIdx_p,
        UINT8* pData_p)
{
    tCcWriteState writeState = kCcWriteStateError;
    tConfChanObject* pObjDest = &ccobjInstance_l.objectList_m[ccobjInstance_l.currWriteObj_m];

    if(pObjDest->objIdx_m == objIdx_p        &&
       pObjDest->objSubIdx_m == objSubIdx_p   )
    {
        /* Enter critical section */
        ccobjInstance_l.pfnCritSec_m(FALSE);

        /* object found in list -> Copy object data and convert endian! */
        switch(pObjDest->objSize_m)
        {
            case sizeof(UINT8):
            {
                pObjDest->objPayloadLow_m = ami_getUint8Le((UINT8 *)pData_p);
                writeState = kCcWriteStateSuccessful;
                break;
            }
            case sizeof(UINT16):
            {
                pObjDest->objPayloadLow_m = ami_getUint16Le((UINT8 *)pData_p);
                writeState = kCcWriteStateSuccessful;
                break;
            }
            case sizeof(UINT32):
            {
                pObjDest->objPayloadLow_m = ami_getUint32Le((UINT8 *)pData_p);
                writeState = kCcWriteStateSuccessful;
                break;
            }
            default:
            {
                /* Default use UINT64 */
                pObjDest->objPayloadLow_m = ami_getUint32Le((UINT8 *)pData_p);
                pObjDest->objPayloadHigh_m = ami_getUint32Le((UINT8 *)pData_p + 4);
                writeState = kCcWriteStateSuccessful;
                break;
            }
        }

        ccobjInstance_l.pfnCritSec_m(TRUE);
        /* Leave critical section */
    }
    else
    {
        writeState = kCcWriteStateOutOfSync;
    }

    return writeState;
}

/*----------------------------------------------------------------------------*/
/**
\brief   Write an object in the object list

\param[in]  objIdx_p          The object index to read
\param[in]  objSubIdx_p       The object subindex to read

\retval  Address    Object read successful
\retval  Null       Unable to read object data
*/
/*----------------------------------------------------------------------------*/
tConfChanObject* ccobject_readObject(UINT16 objIdx_p, UINT8 objSubIdx_p)
{
    tConfChanObject* pObjDef = NULL;
    UINT8 i;

    for(i=0; i < CONF_CHAN_NUM_OBJECTS; i++)
    {
        if(ccobjInstance_l.objectList_m[i].objIdx_m == objIdx_p        &&
           ccobjInstance_l.objectList_m[i].objSubIdx_m == objSubIdx_p   )
        {
            pObjDef = &ccobjInstance_l.objectList_m[i];

            break;
        }
    }

    return pObjDef;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Grab the current object from the object list

\retval  Address       Current object read successful
\retval  Null          Unable to read current object data
*/
/*----------------------------------------------------------------------------*/
tConfChanObject* ccobject_readCurrObject(void)
{
    tConfChanObject* pObject = &ccobjInstance_l.objectList_m[ccobjInstance_l.currReadObj_m];

    return pObject;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Increment the current object read pointer
*/
/*----------------------------------------------------------------------------*/
void ccobject_incObjReadPointer(void)
{
    ccobjInstance_l.currReadObj_m++;

    /* Check overflow */
    if(ccobjInstance_l.currReadObj_m >= CONF_CHAN_NUM_OBJECTS)
    {
        ccobjInstance_l.currReadObj_m = 0;
    }
}

/*----------------------------------------------------------------------------*/
/**
\brief    Increment the current object write pointer
*/
/*----------------------------------------------------------------------------*/
void ccobject_incObjWritePointer(void)
{
    ccobjInstance_l.currWriteObj_m++;

    /* Check overflow */
    if(ccobjInstance_l.currWriteObj_m >= CONF_CHAN_NUM_OBJECTS)
    {
        ccobjInstance_l.currWriteObj_m = 0;
    }
}

/*----------------------------------------------------------------------------*/
/**
\brief    Get the size of an object

\param[in] objIdx_p         Index of the object
\param[in] objSubIdx_p      Subindex of the object
\param[out] pSize_p         The resulting size of the object

\retval TRUE     Size of object valid
\retval FALSE    Unable to read the size of the object
*/
/*----------------------------------------------------------------------------*/
BOOL ccobject_getObjectSize(UINT16 objIdx_p, UINT8 objSubIdx_p,
        UINT8* pSize_p)
{
    BOOL fReturn = FALSE;
    UINT8 i;

    for(i=0; i < CONF_CHAN_NUM_OBJECTS; i++)
    {
        if(ccobjInstance_l.objectList_m[i].objIdx_m == objIdx_p        &&
           ccobjInstance_l.objectList_m[i].objSubIdx_m == objSubIdx_p  )
        {
            /* object found in list! Remember object data */
            *pSize_p = ccobjInstance_l.objectList_m[i].objSize_m;

            fReturn = TRUE;

            break;
        }
    }

    return fReturn;
}


/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */


/**
 * \}
 * \}
 */
