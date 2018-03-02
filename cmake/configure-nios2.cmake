################################################################################
#
# Nios2 configuration options for POWERLINK Interface For Applications
#
# Copyright (c) 2013, B&R Industrial Automation GmbH
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

MESSAGE ( STATUS "Generating build files for platform Altera/Nios2 ..." )

################################################################################
# Setup information for used HW
SET(TARGET_HW altera)
SET(BOARD_NAME terasic-de2-115)
SET(EXAMPLE_NAME cn-single-gpio)

################################################################################
# Handle target specific includes
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/nios2" ${CMAKE_MODULE_PATH})

INCLUDE(ConnectCMakeTargets)
INCLUDE(GenEclipseFileList)

###############################################################################
# User settings
IF(NOT CFG_NIOS2_QUARTUS_DIR)
    SET(CFG_NIOS2_QUARTUS_DIR ${FPGA_DIR}/boards/altera/terasic-de2-115/cn-pcp-spi
        CACHE PATH "Set the location of the FPGA hardware project you want to build"
       )
ENDIF(NOT CFG_NIOS2_QUARTUS_DIR)

UNSET(CFG_DUAL_CHANNEL CACHE)

###############################################################################
# Include new configuration file
IF(EXISTS "${CFG_NIOS2_QUARTUS_DIR}/config.cmake")
    INCLUDE(${CFG_NIOS2_QUARTUS_DIR}/config.cmake)
ELSE()
    MESSAGE(FATAL_ERROR "Settings file for demo ${CFG_NIOS2_QUARTUS_DIR}/config.cmake does not exist! Adapt 'CFG_NIOS2_QUARTUS_DIR' and continue!")
ENDIF()

GET_FILENAME_COMPONENT(CFG_NIOS2_QUARTUS_DIR "${CFG_NIOS2_QUARTUS_DIR}" ABSOLUTE)

MESSAGE(STATUS "\nConfiguration for this demo is:")
MESSAGE(STATUS "CFG_INCLUDE_SUBPROJECTS=${CFG_INCLUDE_SUBPROJECTS}")
MESSAGE(STATUS "CFG_DEMO_INTERCONNECT=${CFG_DEMO_INTERCONNECT}")
MESSAGE(STATUS "CFG_QSYS_SYSTEM_NAME=${CFG_QSYS_SYSTEM_NAME}")
MESSAGE(STATUS "\n")

###############################################################################
# Hardware settings
SET(QSYS_SYSTEM_FILE ${CFG_NIOS2_QUARTUS_DIR}/${CFG_QSYS_SYSTEM_NAME}.qsys)

###############################################################################
# Check if QSYS system is generated
IF(NOT EXISTS "${CFG_NIOS2_QUARTUS_DIR}/${CFG_QSYS_SYSTEM_NAME}.sopcinfo")
    MESSAGE(FATAL_ERROR "unexpected: The Qsys system from ${CFG_NIOS2_QUARTUS_DIR} is not generated! Please build the project first!")
ENDIF()

###############################################################################
# Target dependent paths
SET( TARGET_DIR ${APP_TARGET_DIR}/altera-nios2 )

SET( ALT_MISC_DIR ${MISC_DIR}/altera_nios2 )
SET( ALT_DRIVERS_DIR ${FPGA_DIR}/drivers/altera )

SET(ALT_BUILD_DIR_NAME "altera")

# Set path to application board support package -> Needed by the libraries
SET(ALT_APP_BUILD_DIR ${CMAKE_BINARY_DIR}/app/demo-${CFG_DEMO_TYPE}/${ALT_BUILD_DIR_NAME})
SET(ALT_APP_BSP_DIR ${ALT_APP_BUILD_DIR}/bsp)

# Set path to PCP board support package -> Needed by the libraries
SET(ALT_PCP_BUILD_DIR ${CMAKE_BINARY_DIR}/pcp/psi/${ALT_BUILD_DIR_NAME})
SET(ALT_PCP_BSP_DIR ${ALT_PCP_BUILD_DIR}/bsp)

###############################################################################
# Find Altera Nios2 tools
FIND_PATH( ALT_BSP_GEN_DIR nios2-bsp
           DOC "Path of the Altera Nios2 board support package generator"
         )
MARK_AS_ADVANCED(ALT_BSP_GEN_DIR)

FIND_PROGRAM(ALT_BSP_QUEUE nios2-bsp-query-settings
             DOC "Queue information from a BSP package"
            )
MARK_AS_ADVANCED(ALT_BSP_QUEUE)

FIND_PROGRAM(ALT_APP_GEN_MAKEFILE nios2-app-generate-makefile
             DOC "Generate an application Makefile for the Nios2 processor"
            )
MARK_AS_ADVANCED(ALT_APP_GEN_MAKEFILE)

FIND_PROGRAM(ALT_LIB_GEN_MAKEFILE nios2-lib-generate-makefile
             DOC "Generate a library Makefile for the Nios2 processor"
            )
MARK_AS_ADVANCED(ALT_LIB_GEN_MAKEFILE)

FIND_PROGRAM(ALT_QSYS_SCRIPT qsys-script
             DOC "Queue information from a qsys project"
            )
MARK_AS_ADVANCED(ALT_QSYS_SCRIPT)

###############################################################################
# Get opt level from build type
IF(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    SET(OPT_LEVEL -O0)
ELSEIF(${CMAKE_BUILD_TYPE} MATCHES "Release")
    SET(OPT_LEVEL -O2)
ELSEIF(${CMAKE_BUILD_TYPE} MATCHES "MinSizeRel")
    SET(OPT_LEVEL -Os)
ELSE()
    SET(OPT_LEVEL -O2)
ENDIF()

###############################################################################
# Check plkif ipcore parameters
IF( ${ALT_QSYS_SCRIPT} STREQUAL "ALT_QSYS_SCRIPT-NOTFOUND" )
    MESSAGE( FATAL_ERROR "unexpected: The variable ALT_QSYS_SCRIPT is set to ${ALT_QSYS_SCRIPT}! Start CMake from the nios2 shell to solve this issue!" )
ENDIF ()

SET( QSYS_SCRIPT_COMMAND "set selDemo ${CFG_DEMO_TYPE}\; set qsysSystemName ${CFG_QSYS_SYSTEM_NAME}" )

EXECUTE_PROCESS( COMMAND ${ALT_QSYS_SCRIPT} --cmd=${QSYS_SCRIPT_COMMAND} --script=${ALT_MISC_DIR}/scripts/plkif-config.tcl --system-file=${QSYS_SYSTEM_FILE}
                 WORKING_DIRECTORY ${CFG_NIOS2_QUARTUS_DIR}
                 RESULT_VARIABLE QSYS_RES
                 OUTPUT_VARIABLE QSYS_STDOUT
                 ERROR_VARIABLE QSYS_STDERR
)

IF( ${QSYS_RES} MATCHES "0" )
    MESSAGE ( STATUS "Change configuration of ${QSYS_SYSTEM_FILE} succeeded!\n${QSYS_STDERR}\n" )
ELSEIF( ${QSYS_RES} MATCHES "2" )
    MESSAGE ( STATUS "\n${QSYS_STDERR}\n" )
    MESSAGE ( SEND_ERROR "POWERLINK interface ipcore buffer layout is not set to demo '${CFG_DEMO_TYPE}'! Change the ipcore configuration and rebuild the FPGA design!" )
ELSE ( ${QSYS_RES} MATCHES "0" )
    MESSAGE ( FATAL_ERROR "qsys-script failed with:\n${QSYS_STDERR}\n" )
ENDIF ( ${QSYS_RES} MATCHES "0" )
