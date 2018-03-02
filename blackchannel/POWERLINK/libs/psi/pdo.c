/**
********************************************************************************
\file   pdo.c

\defgroup module_psi_pdo Process data objects (PDO) module
\{

\brief  Module for handling of incoming and outgoing pdo buffers

This module forwards the mapped PDO's from the incoming buffers to the user
application and back.

\ingroup group_libpsi
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

#include <libpsi/internal/pdo.h>
#include <libpsi/internal/stream.h>

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_PDO)) != 0)

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
* \brief PDO user instance type
*/
typedef struct
{
    UINT8           rpdoId_m;           /**< Id of the rpdo buffer */
    tTbufRpdoImage* pRpdoLayout_m;      /**< Pointer to the rpdo triple buffer */
    UINT32          rpdoRelTimeLow_m;   /**< Low value of the relative time */
    UINT8           tpdoId_m;           /**< Id of the tpdo buffer */
    tTbufTpdoImage* pTpdoLayout_m;      /**< Pointer to the tpdo triple buffer */
    tPsiPdoCb       pfnPdoCb_m;         /**< Process PDO user callback function */
} tPdoInstance;

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/

static tPdoInstance          pdoInstance_l;

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/

static BOOL pdo_process(void);
static BOOL pdo_initRpdoBuffer(tTbufNumLayout rpdoId_p);
static BOOL pdo_initTpdoBuffer(tTbufNumLayout tpdoId_p);
static BOOL pdo_processRpdo(UINT8* pBuffer_p, UINT16 bufSize_p, void * pUserArg_p);

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the PDO module

\param[in]  pfnPdoCb_p          PDO process user callback function
\param[in]  pPdoInitParam_p     Initialization structure of the PDO buffers

\retval  TRUE       Successfully initialized the PDO module
\retval  FALSE      Error while initializing the PDO module
*/
/*----------------------------------------------------------------------------*/
BOOL pdo_init(tPsiPdoCb pfnPdoCb_p, tPdoInitParam* pPdoInitParam_p)
{
    BOOL fReturn = FALSE, fError = FALSE;

    PSI_MEMSET(&pdoInstance_l, 0 , sizeof(tPdoInstance));

    if(pfnPdoCb_p == NULL      ||
       pPdoInitParam_p == NULL  )
    {
        /* Wrong parameters passed to module */
        error_setError(kPsiModulePdo, kPsiPdoInitError);
    }
    else
    {
        if((pPdoInitParam_p->buffIdRpdo_m >= kTbufCount &&
            pPdoInitParam_p->buffIdTpdo_m >= kTbufCount   )               ||
            pPdoInitParam_p->buffIdRpdo_m == pPdoInitParam_p->buffIdTpdo_m )
        {
            /* At least one PDO needs to be initialized */
            error_setError(kPsiModulePdo, kPsiPdoInitError);
        }
        else
        {
            if(pPdoInitParam_p->buffIdRpdo_m < kTbufCount)
            {
                /* Initialize the Rpdo buffer */
                if(pdo_initRpdoBuffer(pPdoInitParam_p->buffIdRpdo_m) == FALSE)
                {
                    fError = TRUE;
                }

                pdoInstance_l.rpdoId_m = pPdoInitParam_p->buffIdRpdo_m;
            }
            else
            {
                pdoInstance_l.rpdoId_m = PDO_CHANNEL_DEACTIVATED;
            }

            if(pPdoInitParam_p->buffIdTpdo_m < kTbufCount &&
               !fError                                     )
            {
                /* Initialize the Tpdo buffer */
                if(pdo_initTpdoBuffer(pPdoInitParam_p->buffIdTpdo_m) == FALSE)
                {
                    fError = TRUE;
                }

                pdoInstance_l.tpdoId_m = pPdoInitParam_p->buffIdTpdo_m;
            }
            else
            {
                pdoInstance_l.tpdoId_m = PDO_CHANNEL_DEACTIVATED;
            }

            if(fError == FALSE)
            {
                /* Register PDO process function */
                stream_registerSyncCb(pdo_process);

                /* Remember process PDO user callback */
                pdoInstance_l.pfnPdoCb_m = pfnPdoCb_p;

                fReturn = TRUE;
            }
        }
    }


    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Cleanup pdo module
*/
/*----------------------------------------------------------------------------*/
void pdo_exit(void)
{
    /* Free module internals */
}

/*----------------------------------------------------------------------------*/
/**
\brief    Get the base address of the Tpdo image

\return Base address of the Tpdo image
*/
/*----------------------------------------------------------------------------*/
tTpdoMappedObj * pdo_getTpdoImage(void)
{
    return &pdoInstance_l.pTpdoLayout_m->mappedObjList_m;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Get the base address of the Rpdo image

\return Base address of the Rpdo image
*/
/*----------------------------------------------------------------------------*/
tRpdoMappedObj * pdo_getRpdoImage(void)
{
    return &pdoInstance_l.pRpdoLayout_m->mappedObjList_m;
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the RPDO buffer image

\param[in] rpdoId_p               Id of the RPDO buffer

\retval TRUE        Successfully initializing the rpdo buffer
\retval FALSE       Error while initializing the rpdo buffer
*/
/*----------------------------------------------------------------------------*/
static BOOL pdo_initRpdoBuffer(tTbufNumLayout rpdoId_p)
{
    BOOL fReturn = FALSE;
    tBuffDescriptor* pDescRpdo;

    pDescRpdo = stream_getBufferParam(rpdoId_p);
    if(pDescRpdo->pBuffBase_m != NULL)
    {
        if(pDescRpdo->buffSize_m == sizeof(tTbufRpdoImage))
        {
            /* Remember buffer address for later usage */
            pdoInstance_l.pRpdoLayout_m = (tTbufRpdoImage *)pDescRpdo->pBuffBase_m;

            /* Register rpdo acknowledge action */
            if(stream_registerAction(kStreamActionPost, rpdoId_p,
                    pdo_processRpdo, NULL) != FALSE)
            {
                fReturn = TRUE;
            }
        }
        else
        {
            error_setError(kPsiModulePdo, kPsiRpdoBufferSizeMismatch);
        }
    }
    else
    {
        error_setError(kPsiModulePdo, kPsiRpdoInvalidBuffer);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the TPDO buffer image

\param[in] tpdoId_p               Id of the TPDO buffer

\retval TRUE        Successfully initializing the tpdo buffer
\retval FALSE       Error while initializing the tpdo buffer
*/
/*----------------------------------------------------------------------------*/
static BOOL pdo_initTpdoBuffer(tTbufNumLayout tpdoId_p)
{
    BOOL fReturn = FALSE;
    tBuffDescriptor* pDescTpdo;

    pDescTpdo = stream_getBufferParam(tpdoId_p);
    if(pDescTpdo->pBuffBase_m != NULL)
    {
        if(pDescTpdo->buffSize_m == sizeof(tTbufTpdoImage))
        {
            /* Remember buffer address for later usage */
            pdoInstance_l.pTpdoLayout_m = (tTbufTpdoImage *)pDescTpdo->pBuffBase_m;

            fReturn = TRUE;
        }
        else
        {
            error_setError(kPsiModulePdo, kPsiTpdoBufferSizeMismatch);
        }
    }
    else
    {
        error_setError(kPsiModulePdo, kPsiTpdoInvalidBuffer);
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process the PDO user callback function

\retval TRUE        Successfully processed synchronous task
\retval FALSE       Error while processing the synchronous task
*/
/*----------------------------------------------------------------------------*/
static BOOL pdo_process(void)
{
    BOOL fReturn = FALSE;

    /* Call the PDO user callback function */
    if(pdoInstance_l.rpdoId_m == PDO_CHANNEL_DEACTIVATED)
    {
        /* The Rpdo buffer is not initialized */
        if(pdoInstance_l.pfnPdoCb_m(pdoInstance_l.rpdoRelTimeLow_m,
                    NULL,
                    &pdoInstance_l.pTpdoLayout_m->mappedObjList_m) != FALSE)
        {
            fReturn = TRUE;
        }
        else
        {
            error_setError(kPsiModulePdo, kPsiPdoProcessSyncFailed);
        }
    }
    else if(pdoInstance_l.tpdoId_m == PDO_CHANNEL_DEACTIVATED)
    {
        /* The Tpdo buffer is not initialized */
        if(pdoInstance_l.pfnPdoCb_m(pdoInstance_l.rpdoRelTimeLow_m,
                    &pdoInstance_l.pRpdoLayout_m->mappedObjList_m,
                    NULL) != FALSE)
        {
            fReturn = TRUE;
        }
        else
        {
            error_setError(kPsiModulePdo, kPsiPdoProcessSyncFailed);
        }
    }
    else
    {
        /* Both PDO buffers are initialized */
        if(pdoInstance_l.pfnPdoCb_m(pdoInstance_l.rpdoRelTimeLow_m,
                    &pdoInstance_l.pRpdoLayout_m->mappedObjList_m,
                    &pdoInstance_l.pTpdoLayout_m->mappedObjList_m) != FALSE)
        {
            fReturn = TRUE;
        }
        else
        {
            error_setError(kPsiModulePdo, kPsiPdoProcessSyncFailed);
        }
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process the RPDO buffer

\param[in] pBuffer_p        Pointer to the base address of the buffer
\param[in] bufSize_p        Size of the buffer
\param[in] pUserArg_p       User defined argument

\retval TRUE     Processing of RPDO buffer successful
\retval FALSE    On error
*/
/*----------------------------------------------------------------------------*/
static BOOL pdo_processRpdo(UINT8* pBuffer_p, UINT16 bufSize_p, void * pUserArg_p)
{
    tTbufRpdoImage*  pRpdoImage;

    UNUSED_PARAMETER(bufSize_p);
    UNUSED_PARAMETER(pUserArg_p);

    /* Convert to configuration channel buffer structure */
    pRpdoImage = (tTbufRpdoImage*) pBuffer_p;

    /* Write relative time to local structure */
    pdoInstance_l.rpdoRelTimeLow_m = ami_getUint32Le((UINT8 *)&pRpdoImage->relativeTimeLow_m);

    return TRUE;
}

/**
 * \}
 */

#endif /* #if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_PDO)) != 0) */

/**
 * \}
 */
