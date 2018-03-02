/**
********************************************************************************
\file   TSTssdoConfig.h

\brief  SSDO tests configuration header

The configuration header provides the function prototypes for each module test

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

#pragma once

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <cunit/CUnit.h>

#include <config/triplebuffer.h>

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

// Tests for CC module processing
int TST_streamInit(void);

void TST_ssdoInit(void);
void TST_ssdoProcess(void);

// SSDO module tests for invalid initialization
int TST_initSsdoRxAddrInvalid(void);
int TST_initSsdoTxAddrInvalid(void);
int TST_initSsdoRxSizeInvalid(void);
int TST_initSsdoTxSizeInvalid(void);
int TST_initRxPostActionListFull(void);
int TST_initTxTimeoutInitFails(void);

void TST_ssdoInitFail(void);

// Functions for the read/write tests
int TST_initFull(void);

void TST_ssdoWritePayload(void);
void TST_receivePayload(void);
void TST_receivePayloadInvalidHandler(void);

// Tests for the internal module
int TST_initInternal(void);
void TST_internalProcess(void);
void TST_internalProcessRxHandlerFail(void);
