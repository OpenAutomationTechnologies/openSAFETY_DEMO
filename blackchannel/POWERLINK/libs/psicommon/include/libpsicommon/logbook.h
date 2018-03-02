/**
********************************************************************************
\file   libpsicommon/logbook.h

\brief  Header defines the layout of the logbook triple buffer

This header gives the basic structure of the logbook buffer.

*******************************************************************************/

/*------------------------------------------------------------------------------
Copyright (c) 2013, B&R Industrial Automation GmbH
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

#ifndef _INC_psicommon_logbook_H_
#define _INC_psicommon_logbook_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/
#include <config/logbook.h>

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/
#define LOG_CHANNEL_SIZE   0x0C        /**< Size of the logger channel */

/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

/**
 * \brief Defines the level of an error in the logbook
 */
typedef enum
{
    kLogLevelInvalid        = 0x0,      /**< Error level not defined */
    kLogLevelInfo           = 0x1,      /**< Error level is info */
    kLogLevelMinor          = 0x2,      /**< Error level is minor */
    kLogLevelFatal          = 0x3,      /**< Error level is fatal */
} tLogLevel;


/**
 * \brief This type defines the format of a logbook entry
 */
typedef struct
{
    UINT8 level_m;          /**< The error level (Info, Minor, Fatal) */
    UINT16 source_m;        /**< The source of this error */
    UINT32 code_m;          /**< The error code */
    UINT32 addInfo_m;       /**< Additional info of this error */
} PACK_STRUCT tLogFormat;

/**
 * \brief Memory layout of the logbook channel
 */
typedef struct {
    UINT8        seqNr_m;
    tLogFormat   logData_m;
} PACK_STRUCT tTbufLogStructure;

/*----------------------------------------------------------------------------*/
/* offsetof defines                                                           */
/*----------------------------------------------------------------------------*/

#define TBUF_LOG_SEQNR_OFF      offsetof(tTbufLogStructure, seqNr_m)
#define TBUF_LOG_DATA_OFF       offsetof(tTbufLogStructure, logData_m)

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/

#endif /* _INC_psicommon_logbook_H_ */
