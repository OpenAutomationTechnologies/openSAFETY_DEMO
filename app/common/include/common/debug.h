/**
********************************************************************************
\file   common/include/common/debug.h

\brief  Header file for debugging

This header enables the printing of debug messages according to there debug
level.

*******************************************************************************/

/*------------------------------------------------------------------------------
* License Agreement
*
* Copyright (c) 2013, B&R Industrial Automation GmbH
* Copyright (c) 2016, Kalycito Infotech Private Ltd
* All rights reserved.

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

#ifndef _INC_app_common_debug_H_
#define _INC_app_common_debug_H_

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/

#define DEBUG_LVL_01                    0x00000001
#define DEBUG_LVL_02                    0x00000002
#define DEBUG_LVL_03                    0x00000004
#define DEBUG_LVL_04                    0x00000008
#define DEBUG_LVL_05                    0x00000010
#define DEBUG_LVL_06                    0x00000020
#define DEBUG_LVL_07                    0x00000040
#define DEBUG_LVL_08                    0x00000080
#define DEBUG_LVL_09                    0x00000100
#define DEBUG_LVL_10                    0x00000200
#define DEBUG_LVL_11                    0x00000400
#define DEBUG_LVL_12                    0x00000800
#define DEBUG_LVL_13                    0x00001000
#define DEBUG_LVL_14                    0x00002000
#define DEBUG_LVL_15                    0x00004000
#define DEBUG_LVL_16                    0x00008000
#define DEBUG_LVL_17                    0x00010000
#define DEBUG_LVL_18                    0x00020000
#define DEBUG_LVL_19                    0x00040000
#define DEBUG_LVL_20                    0x00080000
#define DEBUG_LVL_21                    0x00100000
#define DEBUG_LVL_22                    0x00200000
#define DEBUG_LVL_23                    0x00400000
#define DEBUG_LVL_24                    0x00800000
#define DEBUG_LVL_25                    0x01000000
#define DEBUG_LVL_26                    0x02000000
#define DEBUG_LVL_27                    0x04000000
#define DEBUG_LVL_28                    0x08000000
#define DEBUG_LVL_29                    0x10000000
#define DEBUG_LVL_ASSERT                0x20000000
#define DEBUG_LVL_ERROR                 0x40000000
#define DEBUG_LVL_ALWAYS                0x80000000

/*----------------------------------------------------------------------------*/
/* The default debug-level is: ERROR and ALWAYS. */
/* You can define an other debug-level in project settings. */

#ifndef DEF_DEBUG_LVL
    #define DEF_DEBUG_LVL                 (DEBUG_LVL_ALWAYS | DEBUG_LVL_ERROR)
#endif
#ifndef DEBUG_GLB_LVL
    #define DEBUG_GLB_LVL                 (DEF_DEBUG_LVL)
#endif

/* At microcontrollers we do reduce the memory usage by deleting DEBUG_TRACE-lines */
/* (compiler does delete the lines). */

#if (DEBUG_GLB_LVL & DEBUG_LVL_ALWAYS)
    #define DEBUG_LVL_ALWAYS_TRACE(...)                TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_ALWAYS_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_ERROR)
    #define DEBUG_LVL_ERROR_TRACE(...)                 TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_ERROR_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_ASSERT)
    #define DEBUG_LVL_ASSERT_TRACE(...)                TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_ASSERT_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_29)
    #define DEBUG_LVL_29_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_29_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_28)
    #define DEBUG_LVL_28_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_28_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_27)
    #define DEBUG_LVL_27_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_27_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_26)
    #define DEBUG_LVL_26_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_26_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_25)
    #define DEBUG_LVL_25_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_25_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_24)
    #define DEBUG_LVL_24_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_24_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_23)
    #define DEBUG_LVL_23_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_23_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_22)
    #define DEBUG_LVL_22_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_22_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_21)
    #define DEBUG_LVL_21_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_21_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_20)
    #define DEBUG_LVL_20_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_20_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_19)
    #define DEBUG_LVL_19_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_19_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_18)
    #define DEBUG_LVL_18_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_18_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_17)
    #define DEBUG_LVL_17_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_17_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_16)
    #define DEBUG_LVL_16_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_16_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_15)
    #define DEBUG_LVL_15_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_15_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_14)
    #define DEBUG_LVL_14_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_14_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_13)
    #define DEBUG_LVL_13_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_13_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_12)
    #define DEBUG_LVL_12_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_12_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_11)
    #define DEBUG_LVL_11_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_11_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_10)
    #define DEBUG_LVL_10_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_10_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_09)
    #define DEBUG_LVL_09_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_09_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_08)
    #define DEBUG_LVL_08_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_08_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_07)
    #define DEBUG_LVL_07_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_07_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_06)
    #define DEBUG_LVL_06_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_06_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_05)
    #define DEBUG_LVL_05_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_05_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_04)
    #define DEBUG_LVL_04_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_04_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_03)
    #define DEBUG_LVL_03_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_03_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_02)
    #define DEBUG_LVL_02_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_02_TRACE(...)
#endif

#if (DEBUG_GLB_LVL & DEBUG_LVL_01)
    #define DEBUG_LVL_01_TRACE(...)                    TRACE(__VA_ARGS__)
#else
    #define DEBUG_LVL_01_TRACE(...)
#endif

#if !defined (NDEBUG)
    #define DEBUG_TRACE(lvl,...)                           lvl##_TRACE(__VA_ARGS__)
#else
    #define DEBUG_TRACE(lvl,...)
#endif

/* Create TRACE macro for actual printing */
#ifndef NDEBUG

    #include <stdio.h>              /* prototype printf() (for TRACE) */

    #ifndef TRACE
        #define TRACE(...)    printf(__VA_ARGS__)
    #endif

#else

    #ifndef TRACE
        #define TRACE
    #endif

#endif

/*----------------------------------------------------------------------------*/
/* typedef                                                                    */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* function prototypes                                                        */
/*----------------------------------------------------------------------------*/

#endif /* _INC_app_common_debug_H_ */
