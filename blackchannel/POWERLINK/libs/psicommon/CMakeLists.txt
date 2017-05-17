 ################################################################################
#
# CMake file of slim interface common library for PSI
#
# Copyright (c) 2017, Bernecker+Rainer Industrie-Elektronik Ges.m.b.H. (B&R)
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the copyright holders nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
################################################################################

CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

SET(LIB_NAME "psicommon${CURR_APPLICATION}")

PROJECT(${LIB_NAME} C)

# Generate a target for this library in the CMake Makefile
SET(GEN_LIB_TARGET ON)

########################################################################
# Set all source files
########################################################################
SET ( LIB_SRCS
    ${PROJECT_SOURCE_DIR}/ccobject.c
    ${PROJECT_SOURCE_DIR}/timeout.c
    ${PROJECT_SOURCE_DIR}/amile.c
)

########################################################################
# Set include paths
########################################################################
SET ( LIB_INCS
    ${PROJECT_SOURCE_DIR}/include
    ${DEMO_CONFIG_DIR}/tbuf/include
    ${TARGET_DIR}/include
)

###############################################################################
# Set CFLAGS depending on build type
IF(${CMAKE_BUILD_TYPE} MATCHES "Debug" OR ${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
    SET(DBG_MODE _DEBUG)
    SET(CFG_PSICOMMON_DEBUG_LVL 0xEC000000L CACHE STRING "Set the level of the debug output")
ELSE()
    #All other builds are release builds
    SET(DBG_MODE NDEBUG)
    UNSET(CFG_PSICOMMON_DEBUG_LVL CACHE)
    SET(CFG_PSICOMMON_DEBUG_LVL 0x00000000L)
ENDIF()

IF("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    ADD_DEFINITIONS(-D${DBG_MODE} -DDEF_DEBUG_LVL=${CFG_PSICOMMON_DEBUG_LVL})
ENDIF()

###############################################################################
# Add target specific section
IF((CMAKE_SYSTEM_NAME STREQUAL "Generic") AND (CMAKE_SYSTEM_PROCESSOR STREQUAL "nios2"))
    INCLUDE (nios2.cmake)
ENDIF()

########################################################################
# Build library
########################################################################
IF(GEN_LIB_TARGET)
    INCLUDE_DIRECTORIES(${LIB_INCS})

    ADD_LIBRARY (${PROJECT_NAME} ${LIB_TYPE} ${LIB_SRCS})
ENDIF()

################################################################################
# Add clean files
SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${ADD_CLEAN_FILES}")
