################################################################################
#
# CMake file of slim interface on demo-sn-gpio for PSI
#
# Copyright (c) 2013, Bernecker+Rainer Industrie-Elektronik Ges.m.b.H. (B&R)
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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Generate a target for this library in the CMake Makefile
SET(GEN_LIB_TARGET ON)

########################################################################
# Parse the current context of the demo (single or dual channel)
GET_FILENAME_COMPONENT(CURRENT_DEMO_CONTEXT ${CMAKE_CURRENT_BINARY_DIR} NAME)

########################################################################
# Reset demo context variable to simpler strings, Set SYSTEM_PATH id
SET(ID_TARG_SINGLE 1)
SET(ID_TARG_UP_MASTER 2)
SET(ID_TARG_UP_SLAVE 3)

IF(CURRENT_DEMO_CONTEXT STREQUAL ${SN_PROC_UP_MASTER})
    SET(CURRENT_DEMO_CONTEXT "upm")
    SET(SYSTEM_PATH ${ID_TARG_UP_MASTER})
ELSEIF(CURRENT_DEMO_CONTEXT STREQUAL ${SN_PROC_UP_SLAVE})
    SET(CURRENT_DEMO_CONTEXT "ups")
    SET(SYSTEM_PATH ${ID_TARG_UP_SLAVE})
ELSE()
    SET(CURRENT_DEMO_CONTEXT "single")
    SET(SYSTEM_PATH ${ID_TARG_SINGLE})
ENDIF()

########################################################################
# Create project
PROJECT(demo-${CFG_DEMO_TYPE}-${CURRENT_DEMO_CONTEXT} C)

SET (DEMO_SAFETY_APP_TARGET_NAME "${PROJECT_NAME}.elf" CACHE INTERNAL "")

########################################################################
# Create user options
########################################################################
SET(CFG_SAPL_SN_UDID "{0x02U,0x12U,0x34U,0x01U,0x02U,0x03U}" CACHE STRING "Unique id of this SN")

SET(CFG_SAPL_SN_VENDORID "0x00021234UL" CACHE STRING "Vendor ID (0x1018/0x1)")
SET(CFG_SAPL_SN_PRODUCT_CODE "0x00000001UL" CACHE STRING "Product Code (0x1018/0x2)")
SET(CFG_SAPL_SN_REVISION_NR "0x00010100UL" CACHE STRING "Revision Number (0x1018/0x3)")
SET(CFG_SAPL_SN_SERIAL_NR "0x5A5A5A5AUL" CACHE STRING "Serial Number (0x1018/0x4)")
SET(CFG_SAPL_SN_FW_CHKSUM "0x5A5A5A5AUL" CACHE STRING "Firmware Checksum (0x1018/0x4)")

# Enable rotated CRCs in oschecksum
SET(CRC_ROTATED_ENABLE ON)

########################################################################
# Set paths
SET(OSFT_BASE_DIR ${CMAKE_SOURCE_DIR}/openSAFETY)
SET(SN_BASE_DIR ${OSFT_BASE_DIR}/src/eplssrc/SN)
SET(OSCHECKSUM_BASE_DIR ${OSFT_BASE_DIR}/src/tools/oschecksum)

SET(SHNF_DIR ${PROJECT_SOURCE_DIR}/shnf)
SET(SAPL_DIR ${PROJECT_SOURCE_DIR}/sapl)
SET(BOOT_DIR ${PROJECT_SOURCE_DIR}/boot)

SET(COMMON_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../common)
SET(SN_TARGET_SOURCE_DIR ${PROJECT_SOURCE_DIR}/target)

# Check for openSAFETY distribution
IF ( NOT EXISTS ${OSCHECKSUM_BASE_DIR}/crc.c )
    MESSAGE(FATAL_ERROR "openSAFETY is not installed in ${OSFT_BASE_DIR}. Please download "
                        "the distribution from http://sourceforge.net/projects/opensafety/ and install it to "
                        "${OSFT_BASE_DIR}" )
ENDIF()

########################################################################
# Set all source files
########################################################################
FindSourceFiles("${SN_BASE_DIR}" OSFT_SN_SRCS)

## if pysodbuilder enable, use generated sodc, add according include path
SET (SAPL_SRCS_APP_C ${CUSTOMISED_FILES_BUILD_DIR}/app.c)
IF ( PYSODBUILDER_ENABLE )
    SET (SN_SRCS_SOD_C ${PYSODBUILDER_OUTPUT_DIR}/sod.c)
    #SET (SAPL_SRCS_APP_C ${PYSODBUILDER_OUTPUT_DIR}/app.c)
    SET (DEMO_CONFIG_DIR_SN ${PYSODBUILDER_OUTPUT_DIR})
ELSE ()
    SET (SN_SRCS_SOD_C ${DEMO_CONFIG_DIR}/sn/sod.c)
    #SET (SAPL_SRCS_APP_C ${SAPL_DIR}/app.c)
    SET (DEMO_CONFIG_DIR_SN ${DEMO_CONFIG_DIR}/sn)
ENDIF()


SET (PYSODBUILDER_GENERATED_FILES ${SN_SRCS_SOD_C} ${SAPL_SRCS_APP_C})

SET(SHNF_SRCS
    ${SHNF_DIR}/shnf.c
    ${SHNF_DIR}/constime.c
    ${SHNF_DIR}/hnfpsi.c
)

SET(SAPL_SRCS
    ${SAPL_DIR}/sapl.c
    ${SAPL_DIR}/parameterset.c
    ${SAPL_DIR}/parametercrc.c
    ${SAPL_DIR}/sodstore.c
    ${SAPL_DIR}/stackerrorhandler.c
    ${SAPL_SRCS_APP_C}
)

SET(LIBOSCHECKSUM_SRCS
    ${OSCHECKSUM_BASE_DIR}/crc.c
    ${OSCHECKSUM_BASE_DIR}/crc_protocol.c
)

SET(SN_SRCS
    ${PROJECT_SOURCE_DIR}/main.c
    ${APP_COMMON_DIR}/tbufparams.c
    ${PROJECT_SOURCE_DIR}/errorhandler.c
    ${PROJECT_SOURCE_DIR}/statehandler.c
    ${SN_SRCS_SOD_C}
    ${PROJECT_SOURCE_DIR}/util.c
    ${PROJECT_SOURCE_DIR}/cyclemon.c
)

IF(${CURRENT_DEMO_CONTEXT} STREQUAL "ups")
    # This project is for the processor up-slave
    SET(SHNF_SRCS
                  ${SHNF_SRCS}
                  ${SHNF_DIR}/xcom.c
                  ${SHNF_DIR}/xcom-sl.c
                  ${SHNF_DIR}/shnftx-dual.c
    )

    SET(SN_SRCS
                  ${SN_SRCS}
                  ${BOOT_DIR}/pingpong-sl.c
                  ${BOOT_DIR}/handshake-sl.c
                  ${BOOT_DIR}/handshake.c
                  ${BOOT_DIR}/sync-sl.c
    )
ELSEIF(${CURRENT_DEMO_CONTEXT} STREQUAL "upm")
    # This project is for the processor up-master
    SET(SHNF_SRCS
                  ${SHNF_SRCS}
                  ${SHNF_DIR}/xcom.c
                  ${SHNF_DIR}/xcom-ma.c
                  ${SHNF_DIR}/shnftx-dual.c
    )

    SET(SN_SRCS
                  ${SN_SRCS}
                  ${BOOT_DIR}/pingpong-ma.c
                  ${BOOT_DIR}/handshake-ma.c
                  ${BOOT_DIR}/handshake.c
                  ${BOOT_DIR}/sync-ma.c
    )
ELSE()
    # This project is single channeled
    SET(SHNF_SRCS
                  ${SHNF_SRCS}
                  ${SHNF_DIR}/shnftx-single.c
    )
ENDIF()

SET(DEMO_SRCS
                ${SN_SRCS}
                ${SHNF_SRCS}
                ${SAPL_SRCS}
                ${OSFT_SN_SRCS}
                ${LIBOSCHECKSUM_SRCS}
)

########################################################################
# Set include paths
########################################################################
SET(DEMO_INCS
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}/include
    ${COMMON_SOURCE_DIR}/include
    ${SHNF_DIR}/include
    ${SAPL_DIR}/include
    ${BOOT_DIR}/include
    ${psi_SOURCE_DIR}/include
    ${psicommonapp_SOURCE_DIR}/include
    ${DEMO_CONFIG_DIR}/tbuf/include
    ${DEMO_CONFIG_DIR_SN}
    ${DEMO_CONFIG_DIR}/pcp
    ${SN_BASE_DIR}
    ${OSCHECKSUM_BASE_DIR}/include
    ${TARGET_DIR}/include
)

########################################################################
# Generate demo configuration header file
########################################################################
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/include/sn/config.h.in ${PROJECT_BINARY_DIR}/include/sn/config.h)
CONFIGURE_FILE(${OSCHECKSUM_BASE_DIR}/include/oschecksum/config.h.in ${PROJECT_BINARY_DIR}/include/oschecksum/config.h)

########################################################################
# Set CFLAGS depending on build type
########################################################################
IF(${CMAKE_BUILD_TYPE} MATCHES "Debug" OR ${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
    SET(DBG_MODE _DEBUG)
    SET(CFG_APP_DEBUG_LVL 0xEC000000L CACHE STRING "Set the level of the debug output")
ELSE()
    #All other builds are release builds
    SET(DBG_MODE NDEBUG)
    UNSET(CFG_APP_DEBUG_LVL CACHE)
    SET(CFG_APP_DEBUG_LVL 0x00000000L)
ENDIF()


IF("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D${DBG_MODE} -DDEF_DEBUG_LVL=${CFG_APP_DEBUG_LVL}")

    # Remove unknown pragma warning (This disables a warning on CTC_SKIP)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unknown-pragmas")

    # Set C macro to forward SYSTEM_PATH to code
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSYSTEM_PATH=${SYSTEM_PATH}")
ELSE()
    MESSAGE(FATAL_ERROR "Only GNU compilers allowed in this project!")
ENDIF()

########################################################################
# Add target specific build information
########################################################################
IF((CMAKE_SYSTEM_NAME STREQUAL "Generic") AND (CMAKE_SYSTEM_PROCESSOR STREQUAL "nios2"))
    INCLUDE(nios2.cmake)
ELSEIF((CMAKE_SYSTEM_NAME STREQUAL "Generic") AND (CMAKE_SYSTEM_PROCESSOR STREQUAL "cortex-m3"))
    IF(CFG_ARM_BOARD_TYPE STREQUAL "stm32f103rb")
        INCLUDE(stm32f103rb.cmake)
    ELSE()
        MESSAGE(FATAL_ERROR "The board type CFG_ARM_BOARD_TYPE=${CFG_ARM_BOARD_TYPE} is not supported by this application!")
    ENDIF()
ELSEIF((CMAKE_SYSTEM_NAME STREQUAL "Generic") AND (CMAKE_SYSTEM_PROCESSOR STREQUAL "cortex-m4"))
    IF(CFG_ARM_BOARD_TYPE STREQUAL "stm32f401re")
        INCLUDE(stm32f401re.cmake)
    ELSE()
        MESSAGE(FATAL_ERROR "The board type CFG_ARM_BOARD_TYPE=${CFG_ARM_BOARD_TYPE} is not supported by this application!")
    ENDIF()
ELSE()
    MESSAGE(FATAL_ERROR "The selected platform is not supported by this application!")
ENDIF()

########################################################################
# Set include paths
########################################################################
INCLUDE_DIRECTORIES(
                    ${DEMO_INCS}
                    ${DEMO_ARCH_INCS}
)

########################################################################
# Build Executable
########################################################################
IF(GEN_LIB_TARGET)
    ADD_EXECUTABLE(${PROJECT_NAME}.elf ${DEMO_SRCS} ${DEMO_ARCH_SRCS})

    SET_PROPERTY(TARGET ${PROJECT_NAME}.elf PROPERTY LINK_FLAGS ${DEMO_ARCH_LINKER_FLAGS})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME}.elf "psi" "psicommonapp" ${DEMO_ARCH_LIBS})

    # Call target post actions (Target specific!)
    AppPostAction(${PROJECT_NAME}.elf)

    # append pysodbuilder target
    IF ( PYSODBUILDER_ENABLE )
    # add dependency pysodbuilder to target demo-sn-single
    # copy sourcefiles to binary directory, from where the pysodbuilder will be
    # executed

        FOREACH (_pysodbuilder_generated_file IN ITEMS ${PYSODBUILDER_GENERATED_FILES})
            IF (NOT EXISTS ${_pysodbuilder_generated_file})
                SET ( PYSODBUILDER_GENERATED_FILES_MISSING TRUE)
                break()
            ENDIF()
        ENDFOREACH()

        IF (PYSODBUILDER_GENERATED_FILES_MISSING)
            EXECUTE_PROCESS (COMMAND ${CMAKE_COMMAND} -E copy_directory
                             ${pysodbuilder_SOURCE_DIR} ${pysodbuilder_BINARY_DIR}
            )

            # command / target for binding file generation
            EXECUTE_PROCESS ( COMMAND ${PYTHON_EXECUTABLE} -m pysodb.pysodbmain
                                --generate-binding-file ${PYSODBUILDER_OSDD_SCHEMA_FILE}
                                ${pysodbuilder_BINARY_DIR}/pysodb/binding/xosddbinding.py
                                WORKING_DIRECTORY ${pysodbuilder_BINARY_DIR}
                                )

            IF ( NOT "${PYSODBUILDER_MODULE_ID}" STREQUAL "" )
                SET ( PYSODBUILDER_MODULE_ARGUMENT "-m ${MODULE_ID}" )
            ENDIF ()

            EXECUTE_PROCESS (
                        COMMAND ${PYTHON_EXECUTABLE} -m pysodb.pysodbmain
                        -i ${PYSODBUILDER_INPUT_DIR}
                        -o ${PYSODBUILDER_OUTPUT_DIR}
                        -d ${PYSODBUILDER_OSDD_FILE} ${PYSODBUILDER_MODULE_ARGUMENT}
                        -s ${PYSODBUILDER_SETTINGS_FILE}
                        --remove-cog-comments
                        WORKING_DIRECTORY ${pysodbuilder_BINARY_DIR}
                        )
        ENDIF()

        ADD_DEPENDENCIES(${PROJECT_NAME}.elf pysodbuilder )

    ENDIF()
ENDIF()


################################################################################
# Add clean files
SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${ADD_CLEAN_FILES}")
