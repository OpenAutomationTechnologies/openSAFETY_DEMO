/**
********************************************************************************
\file   libpsicommon/global.h

\brief  Global header file for the slim interface project

Global header file for the slim interface project

*******************************************************************************/

/*------------------------------------------------------------------------------
Copyright (c) 2013, Bernecker+Rainer Industrie-Elektronik Ges.m.b.H. (B&R)
Copyright (c) 2016, Kalycito Infotech Private Ltd
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

#ifndef _INC_psicommon_global_H_
#define _INC_psicommon_global_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/
#include <ipcore/tbuf-cfg.h>

#ifdef PSI_BUILD_PCP
  #include <pcptarget/target.h>
  #include <libpsicommon/ami.h>
#else
  #include <apptarget/target.h>
  #include <libpsicommon/ami.h>
#endif

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

/* Create module defines */
#define PSI_MODULE_STATUS      0x00000001L    /**< Status module */
#define PSI_MODULE_CC          0x00000002L    /**< Configuration channel module */
#define PSI_MODULE_PDO         0x00000004L    /**< PDO module */
#define PSI_MODULE_SSDO        0x00000008L    /**< SSDO module */
#define PSI_MODULE_LOGBOOK     0x00000010L    /**< Logger module */

/* Alignment macros */
#define ALIGN16(ptr)        (((UINT16)(ptr) + 1U) & 0xFFFFFFFEU)   /**< aligns the pointer to UINT16 (2 byte) */
#define ALIGN32(ptr)        (((UINT32)(ptr) + 3U) & 0xFFFFFFFCU)   /**< aligns the pointer to UINT32 (4 byte) */

#define UNALIGNED16(ptr)    ((UINT16)(ptr) & 1U)    /**< checks if the pointer is UINT16-aligned */
#define UNALIGNED32(ptr)    ((UINT32)(ptr) & 3U)    /**< checks if the pointer is UINT32-aligned */

/* Check if a bit is set */
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

#ifndef UNUSED_PARAMETER
  #define UNUSED_PARAMETER(par)   (void)par
#endif

#ifndef max
    #define max(a, b)           (((a) > (b)) ? (a) : (b))
#endif

/*----------------------------------------------------------------------------*/
/* Boolean values                                                             */
/*----------------------------------------------------------------------------*/

#ifndef TRUE
  #define TRUE  0xFF
#endif

#ifndef FALSE
  #define FALSE 0x00
#endif


/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

/**
 * \brief Asynchronous frame handling sequence number type
 */
typedef enum {
    kSeqNrValueInvalid    = 0x00,   /**< Invalid sequence number (Other values are also invalid) */
    kSeqNrValueFirst      = 0x56,   /**< Sequence number first packet arriving */
    kSeqNrValueSecond     = 0xAA    /**< Sequence number second packet arriving */
} tSeqNrValue;

/**
 * \brief Descriptor element of descriptor list
 */
typedef struct {
    UINT32      buffOffset_m;       /**< Offset of the triple buffer */
    UINT16      buffSize_m;         /**< Size of the buffer */
    INT8        isProducer_m;       /**< Descriptor is a producing buffer (app side) */
} tTbufDescriptor;

/**
 * \brief List of object sizes
 */
typedef enum eObjTypeSize {
    kTypeUint8Size   = 0x01,
    kTypeInt8Size    = 0x01,
    kTypeUint16Size  = 0x02,
    kTypeInt16Size   = 0x02,
    kTypeUint32Size  = 0x04,
    kTypeInt32Size   = 0x04,
    kTypeUint64Size  = 0x08,
    kTypeInt64Size   = 0x08
} tObjTypeSize;

/**
 * \brief One object from the cc object list
 */
typedef struct {
    UINT16 objIdx;
    UINT8  objSubIdx;
    UINT8  objSize;
} tCcObject;

/**
 * \brief Configuration channel object of local object list
 */
typedef struct {
    UINT16 objIdx_m;
    UINT8 objSubIdx_m;
    UINT8 objSize_m;
    UINT32 objPayloadLow_m;
    UINT32 objPayloadHigh_m;
} tConfChanObject;

/**
 * \brief List of all available slim interface modules
 */
typedef enum {
    kPsiModuleInvalid   = 0x00,
    kPsiModuleStatus    = 0x01,
    kPsiModuleCc        = 0x02,
    kPsiModuleCcObject  = 0x03,
    kPsiModulePdo       = 0x04,
    kPsiModuleSsdo      = 0x05,
    kPsiModuleLogbook   = 0x06,
    kPsiModuleTimeout   = 0x07,
    kPsiModuleStream    = 0x08,
    kPsiModuleInternal  = 0x09,
    kPsiModuleExternal  = 0x0A,
} tPsiModules;

/**
 * \brief Global slim interface status type
 */
typedef enum {
    kPsiSuccessful                  = 0x00,
    kPsiInitError                   = 0x01,
    kPsiGeneralError                = 0x02,
    kPsiSpiSendReceiveError         = 0x03,
    kPsiBufferSizeMismatch          = 0x04,
    kPsiInvalidBuffer               = 0x05,
    kPsiProcessSyncFailed           = 0x06,
    kPsiProcessAsyncFailed          = 0x07,
    kPsiInvalidHandle               = 0x08,

    kPsiMainPlkStackInitError       = 0x12,
    kPsiMainPlkStackStartError      = 0x13,
    kPsiMainPlkStackShutdownError   = 0x14,
    kPsiMainPlkStackProcessError    = 0x15,

    kPsiStatusInitError             = 0x20,
    kPsiStatusRelTimeStateError     = 0x21,
    kPsiStatusBufferSizeMismatch    = 0x22,
    kPsiStatusProcessSyncFailed     = 0x23,

    kPsiTbuffInitError              = 0x30,
    kPsiTbuffReadError              = 0x31,
    kPsiTbuffWriteError             = 0x32,

    kPsiConfChanInitError           = 0x40,
    kPsiConfChanBufferSizeMismatch  = 0x41,
    kPsiConfChanObjectInitFailed    = 0x42,
    kPsiConfChanObjectNotFound      = 0x43,
    kPsiConfChanWriteToObDictFailed = 0x44,
    kPsiConfChanInvalidSizeOfObj    = 0x45,
    kPsiConfChanChannelBusy         = 0x46,
    kPsiConfChanObjListOutOfSync    = 0x47,
    kPsiConfChanInvalidParameter    = 0x48,
    kPsiConfChanInvalidBuffer       = 0x49,
    kPsiConfChanWriteToObjectFailed = 0x4A,
    kPsiConfChanInitCcObjectFailed  = 0x4B,
    kPsiConfChanObjLinkFailed       = 0x4C,

    kPsiPdoInitError                = 0x50,
    kPsiPdoProcessSyncFailed        = 0x51,

    kPsiRpdoInitError               = 0x60,
    kPsiRpdoBufferSizeMismatch      = 0x61,
    kPsiRpdoInvalidBuffer           = 0x62,

    kPsiTpdoInitError               = 0x70,
    kPsiTpdoBufferSizeMismatch      = 0x71,
    kPsiTpdoInvalidBuffer           = 0x72,

    kPsiStreamInitError             = 0x80,
    kPsiStreamInvalidBuffer         = 0x81,
    kPsiStreamTransferError         = 0x82,
    kPsiStreamInvalidParameter      = 0x83,
    kPsiStreamNoFreeElementFound    = 0x84,
    kPsiStreamProcessActionFailed   = 0x85,
    kPsiStreamSyncError             = 0x86,

    kPsiSsdoInitError               = 0x90,
    kPsiSsdoInvalidParameter        = 0x91,
    kPsiSsdoProcessingFailed        = 0x92,
    kPsiSsdoWriteToObDictFailed     = 0x93,
    kPsiSsdoDestinationUnknown      = 0x94,
    kPsiSsdoTxConsSizeInvalid       = 0x95,
    kPsiSsdoSendError               = 0x96,
    kPsiSsdoNoFreeBuffer            = 0x97,
    kPsiSsdoBufferSizeMismatch      = 0x98,
    kPsiSsdoInvalidBuffer           = 0x99,
    kPsiSsdoInvalidState            = 0x9A,
    kPsiSsdoChannelBusy             = 0x9B,
    kPsiSsdoInvalidTargetInfo       = 0x9C,
    kPsiSsdoCloseSdoConError        = 0x9D,

    kPsiLogInitError                = 0xA0,
    kPsiLogInvalidParameter         = 0xA1,
    kPsiLogProcessingFailed         = 0xA2,
    kPsiLogInvalidState             = 0xA3,
    kPsiLogDestinationUnknown       = 0xA4,
    kPsiLogWriteToObDictFailed      = 0xA5,
    kPsiLogInvalidTargetInfo        = 0xA6,
    kPsiLogSendError                = 0xA7,
    kPsiLogTxConsSizeInvalid        = 0xA8,
    kPsiLogBufferSizeMismatch       = 0xA9,
    kPsiLogInvalidBuffer            = 0xAA,
    kPsiLogEntryReformatFailed      = 0xAB,
    kPsiLogInvalidErrorLevel        = 0xAC,
    kPsiLogNettimeInvalid           = 0xAD,
    kPsiLogCloseSdoConError         = 0xAE,

    kPsiFifoInitFailed              = 0x100,
    kPsiFifoInvalidParam            = 0x101,
    kPsiFifoEmpty                   = 0x102,
    kPsiFifoFull                    = 0x103,
    kPsiFifoElementSizeOverflow     = 0x104,
    kPsiFifoAlignError              = 0x105,

    kPsiTimeoutOccured              = 0x200,
} tPsiStatus;

/**
 * \brief Type for critical section entry function
 */
typedef void (* tPsiCritSec) ( UINT8 fEnable_p );

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/

#endif /* _INC_psicommon_global_H_ */
