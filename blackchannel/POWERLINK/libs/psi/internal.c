/**
********************************************************************************
\file   internal.c

\defgroup module_psi_int Internal module
\{

\brief  Application interface internal configuration module

This module handlers library internals. It initializes internal modules and
processes the synchronous and asynchronous task.

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

#include <libpsi/psi.h>

#include <libpsi/internal/stream.h>
#include <libpsi/internal/ssdo.h>
#include <libpsi/internal/logbook.h>
#include <libpsi/internal/cc.h>
#include <libpsi/internal/error.h>

#include <libpsicommon/timeout.h>

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

#define ACK_REG_INITIAL_VALUE    0xFFFFFFFF     /**< Initial value of the ACK register */

/*----------------------------------------------------------------------------*/
/* local types                                                                */
/*----------------------------------------------------------------------------*/

/**
\brief Internal user instance type
*/
typedef struct
{
    tTbufAckRegister*    pConsAckRegister_m;      /**< Pointer to the consumer ACK register */
    tTbufAckRegister*    pProdAckRegister_m;      /**< Pointer to the producer ACK register */
} tInternalInstance;

/*----------------------------------------------------------------------------*/
/* local vars                                                                 */
/*----------------------------------------------------------------------------*/

static tInternalInstance          intInstance_l;

/*----------------------------------------------------------------------------*/
/* local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static BOOL psi_initIntModules(tPsiInitParam* pInitParam_p);
static tTbufAckRegister* psi_initAckRegister(tTbufNumLayout idAckReg_p);

/*============================================================================*/
/*            P U B L I C   F U N C T I O N S                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
\brief    Application interface initialization function

\param[in]  pInitParam_p       Initialization parameters

\retval TRUE        Successfully initialized the internal module
\retval FALSE       Error during initialization
*/
/*----------------------------------------------------------------------------*/
BOOL psi_init(tPsiInitParam* pInitParam_p)
{
    BOOL fReturn = FALSE;

    PSI_MEMSET(&intInstance_l, 0 , sizeof(tInternalInstance));

    if(pInitParam_p == NULL)
    {
        /* Invalid input parameters provided */
        error_setError(kPsiModuleInternal, kPsiInitError);
    }
    else
    {
        if(pInitParam_p->idConsAck_m >= kTbufCount ||
           pInitParam_p->idProdAck_m >= kTbufCount ||
           pInitParam_p->idConsAck_m == pInitParam_p->idProdAck_m )
        {
            /* Invalid input parameters provided */
            error_setError(kPsiModuleInternal, kPsiInitError);
        }
        else
        {
            /* Initialize internal library modules */
            if(psi_initIntModules(pInitParam_p))
            {
                /* Initialize consumer acknowledge register */
                intInstance_l.pConsAckRegister_m = psi_initAckRegister(pInitParam_p->idConsAck_m);
                if(intInstance_l.pConsAckRegister_m != NULL)
                {
                    /* Initialize producer acknowledge register */
                    intInstance_l.pProdAckRegister_m = psi_initAckRegister(pInitParam_p->idProdAck_m);
                    if(intInstance_l.pProdAckRegister_m != NULL)
                    {
                        fReturn = TRUE;
                    }
                    else
                    {
                        /* Producer acknowledge register initialization failed */
                        error_setError(kPsiModuleInternal, kPsiInitError);
                    }
                }
                else
                {
                    /* Consumer acknowledge register initialization failed */
                    error_setError(kPsiModuleInternal, kPsiInitError);
                }
            }
            else
            {
                /* Initializing internal modules failed */
                error_setError(kPsiModuleInternal, kPsiInitError);
            }
        }
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Close the slim interface
*/
/*----------------------------------------------------------------------------*/
void psi_exit(void)
{
    /* Destroy initialized modules */
    stream_exit();

    /* Destroy error module */
    error_exit();
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process slim interface synchronous task

\retval  TRUE      Sync processing successful
\retval  FALSE     Error while processing sync
*/
/*----------------------------------------------------------------------------*/
BOOL psi_processSync(void)
{
    BOOL fReturn = FALSE;

    if(stream_processSync() != FALSE)
    {
        fReturn = TRUE;
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process slim interface post transfer actions

Process all library actions which shall be triggered after a successful
exchange of the input/output image.

\retval  TRUE      Post action executed successfully
\retval  FALSE     Error in post action
*/
/*----------------------------------------------------------------------------*/
BOOL psi_processPostTransferActions(void)
{
    BOOL fReturn = FALSE;

    if(stream_processPostActions() != FALSE)
    {
        fReturn = TRUE;
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Process slim interface asynchronous task

\retval  TRUE         Async processing successful
\retval  FALSE        Error while processing async
*/
/*----------------------------------------------------------------------------*/
BOOL psi_processAsync(void)
{
#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_CC)) != 0)
    /* Process configuration channel objects */
    cc_process();
#endif

    return TRUE;
}

/*============================================================================*/
/*            P R I V A T E   F U N C T I O N S                               */
/*============================================================================*/
/** \name Private Functions */
/** \{ */

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize all internal modules

\param[in]  pInitParam_p         Internal module initialization parameters

\retval  TRUE      Successfully initialized modules
\retval  FALSE     Error while initializing internal modules
*/
/*----------------------------------------------------------------------------*/
static BOOL psi_initIntModules(tPsiInitParam* pInitParam_p)
{
    BOOL fReturn = FALSE;
    tStreamInitParam streamInitParam;

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_SSDO)) != 0)
    /* Initialize the SSDO module */
    ssdo_init();
#endif

#if(((PSI_MODULE_INTEGRATION) & (PSI_MODULE_LOGBOOK)) != 0)
    /* Initialize the logbook module */
    log_init();
#endif

    /* Initialize the timeout module */
    timeout_init();

    /* Initialize the error module */
    error_init(pInitParam_p->pfnErrorHandler_m);

    /* Initialize stream module */
    streamInitParam.pBuffDescList_m = pInitParam_p->pBuffDescList_m;
    streamInitParam.pfnStreamHandler_m = pInitParam_p->pfnStreamHandler_m;
    streamInitParam.idConsAck_m = pInitParam_p->idConsAck_m;
    streamInitParam.idFirstProdBuffer_m = pInitParam_p->idFirstProdBuffer_m;

    if(stream_init(&streamInitParam) != FALSE)
    {
        fReturn = TRUE;
    }

    return fReturn;
}

/*----------------------------------------------------------------------------*/
/**
\brief    Initialize the acknowledge registers

\param[in]  idAckReg_p         Id of the acknowledge register

\retval Address      Address of the initialized ACK register
\retval NULL         Error while initializing the acknowledge register
*/
/*----------------------------------------------------------------------------*/
static tTbufAckRegister* psi_initAckRegister(tTbufNumLayout idAckReg_p)
{
    tBuffDescriptor* pDescAckReg;
    tTbufAckRegister* pAckRegBase = NULL;

    pDescAckReg = stream_getBufferParam(idAckReg_p);
    if(pDescAckReg->pBuffBase_m != NULL)
    {
        if(pDescAckReg->buffSize_m == sizeof(tTbufAckRegister))
        {
            /* Set ACK register address */
            pAckRegBase = (tTbufAckRegister*)pDescAckReg->pBuffBase_m;

            /* Set register to always ack all buffers (Needed for streamed access) */
            *pAckRegBase = ACK_REG_INITIAL_VALUE;
        }
    }

    return pAckRegBase;
}

/**
 * \}
 * \}
 */
