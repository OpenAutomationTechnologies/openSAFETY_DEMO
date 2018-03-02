/**
********************************************************************************
\file   demo-sn-gpio/include/sn/errorhandler.h

\brief  Error handler of the SN firmware

*******************************************************************************/

/*------------------------------------------------------------------------------
Copyright (c) 2014, B&R Industrial Automation GmbH
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

#ifndef _INC_sn_errorhandler_H_
#define _INC_sn_errorhandler_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/
#include <sn/global.h>


/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

/**
 * \brief Error types of the firmware
 */
typedef enum
{
    kErrorInvalid                           = 0x00,     /**< Zero is no error! */
    kErrorInvalidParameter                  = 0x01,     /**< An invalid parameter is passed to a function */
    kErrorInvalidState                      = 0x02,     /**< A state machine has reached an invalid state */
    kErrorInvalidFrameType                  = 0x03,     /**< This type of frame is not an openSAFETY frame */
    kErrorCallbackNotInitialized            = 0x04,     /**< A callback function should be called but is not initialized */
    kErrorInvalidTxMemory                   = 0x05,     /**< There is no transmit memory left for sending */
    kErrorUnableToGenerateStreamParams      = 0x06,     /**< Unable to generate parameters for the stream interface */
    kErrorInitConsTimeFailed                = 0x07,     /**< Unable to init the consecutive timebase */
    kErrorInitApplicationFailed             = 0x08,     /**< Unable to init the application module */

    kErrorSyncProcessFailed                 = 0x20,     /**< Processing the synchronous task has failed */
    kErrorSyncProcessActionFailed           = 0x21,     /**< Processing the post action has failed */
    kErrorSyncFramePostingFailed            = 0x22,
    kErrorSyncFrameCopyFailed               = 0x23,
    kErrorSyncFrameNoBuffer                 = 0x24,

    kErrorAsyncProcessFailed                = 0x30,     /**< Processing the asynchronous task has failed */
    kErrorAsyncFramePostingFailed           = 0x31,
    kErrorAsyncFrameCopyFailed              = 0x32,
    kErrorAsyncFrameNoBuffer                = 0x33,
    kErrorAsyncFrameTooLarge                = 0x34,

    kErrorEnterPreOperationalFailed         = 0x41,
    kErrorEnterOperationalFailed            = 0x42,
    kErrorEnterFailSafeFailed               = 0x43,

    kErrorParamSetInvalidSize               = 0x51,
    kErrorParamSetUnableToParseObj          = 0x52,
    kErrorObjTooLongForParameterSet         = 0x53,
    kErrorUnableToObjAttr                   = 0x54,
    kErrorUnableToWriteToSod                = 0x55,
    kErrorUnableToReadFromSod               = 0x56,
    kErrorUnableToSetLengthInSod            = 0x57,
    kErrorNoCrcFieldAllocated               = 0x58,
    kErrorUnableToOpenNvm                   = 0x59,

    kErrorSodStoreSizeMissmatch             = 0x60,
    kErrorSodStoreWriteError                = 0x61,
    kErrorSodStoreCrcError                  = 0x62,

    kErrorCycleMonStateInvalid              = 0x70,     /**< Invalid cycle monitoring state */
    kErrorInvalidTimeBase                   = 0x71,     /**< The time module returned an invalid time */
    kErrorInvalidFlowCount                  = 0x72,     /**< The flow count returned an invalid value */
    kErrorInvalidCrcValue                   = 0x73,     /**< The CRC returned an invalid value */
    kErrorInvalidMsgFormatValue             = 0x74,     /**< Failed to retrieve message format field */
    kErrorInvalidCycleTime                  = 0x75,     /**< The measured cycle time is out of the valid range */

    kErrorSerialInitFailed                  = 0x80,     /**< Unable to initialize the serial device */
    kErrorSerialTransferFailed              = 0x81,     /**< Error during the serial transfer */
    kErrorSerialTransmitFailed              = 0x82,     /**< Failed to transmit data */
    kErrorSerialReceiveFailed               = 0x83,     /**< Failed to receive data */

    kHandSWelcomeMsgInvalid                 = 0x90,     /**< Unable to verify the welcome message */
    kHandSResponseMsgInvalid                = 0x91,     /**< Unable to create of verify the response message */
    kHandSReceiveBufferInvalid              = 0x92,     /**< The provided receive buffer is invalid */
    kHandSReadyMsgInvalid                   = 0x93,     /**< Unable to verify the ready message */
    kHandSSyncMsgInvalid                    = 0x94,     /**< Unable to verify the synchronization message */

    kXComTimeDeviationTooHigh               = 0xA0,     /**< The deviation of the local time and the time from the other processor is too high */
    kXComFlowCountMissmatch                 = 0xA1,     /**< The flow counter on the target is not matching */
    kXComMissingCrossCommunication          = 0xA2,     /**< There was no cross communication in the last cycle */
    kXComIdValueInvalid                     = 0xA3,     /**< The id value of the incoming message is not valid */
    kXComSpdoCrcMissmatch                   = 0xA4,     /**< The SPDO CRC is different on uP-Master and uP-Slave */
    kXComSsdoSnmtCrcMissmatch               = 0xA5,     /**< The SSDO/SNMT CRC is different on uP-Master and uP-Slave */
    kXComFrameMissing                       = 0xA6,     /**< A frame was produced on the remote processor but locally non is available */

    kErrorStatusModuleInitFailed            = 0xB0,     /**< Unable to initialize the PSI status module */
    kErrorPdoModuleInitFailed               = 0xB1,     /**< Unable to initialize the PSI pdo module */
    kErrorSsdoModuleInitFailed              = 0xB2,     /**< Unable to initialize the PSI ssdo module */
    kErrorLoggerModuleInitFailed            = 0xB3,     /**< Unable to initialize the PSI logger module */
} tErrorTypes;


/**
 * \brief Indicates all available error sources
 */
typedef enum
{
    kErrSourceInvalid      = 0x0,   /**< Error source is not net */
    kErrSourceStack        = 0x1,   /**< Error source = openSafety stack */
    kErrSourceHnf          = 0x2,   /**< Error source = Hardware near firmware interface */
    kErrSourceShnf         = 0x3,   /**< Error source = SHNF managing module */
    kErrSourceSapl         = 0x4,   /**< Error source = Safe application */
    kErrSourcePeriph       = 0x5,   /**< Error source = Managing module */
    kErrSourceXCom         = 0x6,   /**< Error source = Cross communication module */
} tErrSource;

/**
 * \brief Indicates how serious or expected an error is
 */
typedef enum
{
    kErrLevelInvalid      = 0x0,    /**< Invalid error level */
    kErrLevelInfo         = 0x1,    /**< Error level = INFO */
    kErrLevelMinor        = 0x2,    /**< Error level = MINOR */
    kErrLevelFatal        = 0x3,    /**< Error level = FATAL */
} tErrLevel;

/**
 * \brief Defines the structure of the error descriptor
 */
typedef struct
{
    BOOLEAN fFailSafe_m;    /**< TRUE if the device shall enter fail safe on this error */
    tErrSource source_m;    /**< The origin of this error */
    tErrLevel class_m;      /**< Provides information about the criticality of the error */
    UINT8 unit_m;           /**< Origin module of the error */
    UINT8 code_m;           /**< Error code to identify the error within the unit */
    UINT32 addInfo_m;       /**< Additional error information */
} tErrorDesc;

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
    extern "C" {
#endif

void errh_init(void);
void errh_exit(void);

void errh_postInfo(tErrSource source_p, tErrorTypes code_p, UINT32 addInfo_p);
void errh_postMinorError(tErrSource source_p, tErrorTypes code_p, UINT32 addInfo_p);
void errh_postFatalError(tErrSource source_p, tErrorTypes code_p, UINT32 addInfo_p);

void errh_postError(tErrorDesc * pErrInfo_p);

void errh_proccessError(void);


#ifdef __cplusplus
    }
#endif


#endif /* _INC_sn_errorhandler_H_ */
