################################################################################
#
# Cortex-Mx configuration options for POWERLINK Slim Interface
#
# Copyright (c) 2017, B&R Industrial Automation GmbH
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

################################################################################
# Handle target specific includes
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/cortex-mx" ${CMAKE_MODULE_PATH})

INCLUDE(AppPostAction)
INCLUDE(FindStLink)

################################################################################
# Create user options
IF(CFG_DEMO_TYPE STREQUAL "sn-gpio")
    IF(NOT CFG_DUAL_CHANNEL)
        SET(CFG_DUAL_CHANNEL ${SN_PROC_SINGLE_CHAN} CACHE STRING
            "Enable dual channel openSAFETY demo"
            FORCE
        )
        SET_PROPERTY(CACHE CFG_DUAL_CHANNEL PROPERTY STRINGS "${SN_PROC_SINGLE_CHAN};${SN_PROC_DUAL_CHAN};${SN_PROC_UP_MASTER};${SN_PROC_UP_SLAVE}")
    ENDIF(NOT CFG_DUAL_CHANNEL)
ELSE()
    UNSET(CFG_DUAL_CHANNEL CACHE)
ENDIF()

OPTION(CFG_BENCHMARK_ENABLED "Enable application benchmark module" ON)

OPTION(CFG_PROG_FLASH_ENABLE "Enable the program to flash target" OFF)

################################################################################
# This target only supports application style projects
SET(CFG_INCLUDE_SUBPROJECTS "application")

################################################################################
# Set compiler flags
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mthumb -ffunction-sections -fdata-sections ")

################################################################################
# Enable benchmarking
IF(CFG_BENCHMARK_ENABLED)
    ADD_DEFINITIONS(-DBENCHMARK_ENABLED -DBENCHMARK_MODULES=0xEE800043L)
ENDIF()