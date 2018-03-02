/**
********************************************************************************
\file   stm32f401re/include/apptarget/target.h

\brief  Target specific header file of the slim interface module

This file contains definitions for the platform of the slim interface
user library for target arm cortex-m3.

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

#ifndef _INC_apptarget_H_
#define _INC_apptarget_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/
#include <string.h>    /* for memcpy() memset() */
#include <stdint.h>

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

#if defined(__GNUC__)

#ifndef CHAR
    #define CHAR char
#endif

#ifndef INT8
    #define INT8 char
#endif

#ifndef UINT8
    #define UINT8 unsigned char
#endif

#ifndef BOOL
    #define BOOL UINT8
#endif

#ifndef INT16
    #define INT16 short int
#endif

#ifndef UINT16
    #define UINT16 unsigned short int
#endif

#ifndef INT
    #define INT int
#endif

#ifndef INT32
    #define INT32 int
#endif

#ifndef UINT
    #define UINT unsigned int
#endif

#ifndef UINT32
    #define UINT32 unsigned int
#endif

#ifndef INT64
    #define INT64 long long int
#endif

#ifndef UINT64
    #define UINT64 unsigned long long int
#endif

#ifndef NULL
    #define NULL ((void *) 0)
#endif

#else
    #error "Please define the datatypes for your compiler here!"
#endif


/* Guard standard library functions */
#define PSI_MEMSET(ptr, bVal, bCnt)  memset(ptr, bVal, bCnt)
#define PSI_MEMCPY(ptr, bVal, bSize) memcpy(ptr, bVal, bSize)

#define DLLEXPORT

#define PACK_STRUCT __attribute__((packed))

/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/


#endif /* _INC_apptarget_H_ */


