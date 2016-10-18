################################################################################
#
# CMake file of slim interface on pcp (nios2 target) for PSI
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

##########################################################################
# Set build directory for the Altera Makefile
SET(ALT_BUILD_DIR ${PROJECT_BINARY_DIR}/${ALT_BUILD_DIR_NAME})

FILE(MAKE_DIRECTORY ${ALT_BUILD_DIR})

##########################################################################
# Instance ID user option
SET(ALT_PCP_INSTANCE_ID "0" CACHE STRING "Instance ID of the POWERLINK processor")

##########################################################################
# Set build parameters
SET( NIOS2_QSYS_SUB pcp_0 )
SET( NIOS2_QSYS_SUB_CPU cpu_0 )

SET( BSP_TYPE hal )
SET( ALT_PCP_BSP_DIR ${ALT_BUILD_DIR}/bsp )

SET( NIOS2_TC_I_MEM ${NIOS2_QSYS_SUB}_tc_i_mem )

# Path to the nios2 flash override file
SET(ALT_FLASH_OVERRIDE ${ALT_MISC_DIR}/scripts/nios2-flash-override.txt)

SET(ALT_TARGET_SRCS
    ${PROJECT_SOURCE_DIR}/target/altera/target.c
    ${ARCH_SOURCE_DIR}/altera-nios2/target-nios2.c
    ${ARCH_SOURCE_DIR}/altera-nios2/openmac-nios2.c
    ${ARCH_SOURCE_DIR}/altera-nios2/lock-localnoos.c
    ${ARCH_SOURCE_DIR}/altera-nios2/target-mutex.c
    ${OPLK_BOARD_DIR}/common/drivers/openmac/omethlib_phycfg.c
   )

# Create Altera source list
SET(ALT_PSI_SRCS
    ${PSI_SRCS}
    ${ALT_TARGET_SRCS}
    ${IP_SRCS}
    ${OPLK_SRCS}
    ${OMETHLIB_SRCS}
   )

# Create Altera include list
SET(ALT_PSI_INCS
    ${PSI_INCS}
    ${OPLK_INCS}
    ${OMETHLIB_INCS}
    ${ALT_PCP_BSP_DIR}
    ${ALT_PCP_BSP_DIR}/HAL/inc
    ${ALT_PCP_BSP_DIR}/drivers/inc
    ${ARCH_SOURCE_DIR}/altera-nios2
    ${PROJECT_SOURCE_DIR}/target/altera/include
   )

########################################################################
# Board Support Package
########################################################################

SET( NIOS2_BSP_ARGS
                    "--set hal.sys_clk_timer ${NIOS2_QSYS_SUB}_timer_0"
                    "--cpu-name ${NIOS2_QSYS_SUB}_${NIOS2_QSYS_SUB_CPU}"
                    "--set hal.enable_c_plus_plus false"
                    "--set hal.linker.enable_alt_load_copy_exceptions false"
                    "--set hal.enable_clean_exit false"
                    "--set hal.enable_exit false"
                    "--cmd add_section_mapping .tc_i_mem ${NIOS2_TC_I_MEM}"
                    "--set hal.make.bsp_cflags_optimization ${OPT_LEVEL}"
   )

IF( ${CMAKE_BUILD_TYPE} MATCHES "Release" )
    SET( NIOS2_BSP_ARGS "${NIOS2_BSP_ARGS} --set hal.stdout none --set hal.stderr none" )
ENDIF ( ${CMAKE_BUILD_TYPE} MATCHES "Release" )

EXECUTE_PROCESS( COMMAND bash ${ALT_BSP_GEN_DIR}/nios2-bsp ${BSP_TYPE} ${ALT_PCP_BSP_DIR} ${CFG_NIOS2_QUARTUS_DIR} ${NIOS2_BSP_ARGS}
                 WORKING_DIRECTORY ${ALT_BUILD_DIR}
                 RESULT_VARIABLE GEN_BSP_RES
                 OUTPUT_VARIABLE GEN_BSP_STDOUT
                 ERROR_VARIABLE GEN_BSP_STDERR
)

IF( NOT ${GEN_BSP_RES} MATCHES "0" )
    MESSAGE ( FATAL_ERROR "nios2-bsp generation error: ${GEN_BSP_STDERR}" )
ENDIF ( NOT ${GEN_BSP_RES} MATCHES "0" )

MESSAGE ( STATUS  "Generate board support package: ${GEN_BSP_STDOUT}" )

########################################################################
# Application Makefile
########################################################################

SET( ALT_BSP_QUEUE_ARGS
                       "--settings ${ALT_PCP_BSP_DIR}/settings.bsp"
                       "--cmd puts [get_addr_span ${NIOS2_TC_I_MEM}]"
   )

# Get TCI memory size
EXECUTE_PROCESS( COMMAND ${ALT_BSP_QUEUE} ${ALT_BSP_QUEUE_ARGS}
                 WORKING_DIRECTORY ${ALT_BUILD_DIR}
                 RESULT_VARIABLE GET_TCM_RES
                 OUTPUT_VARIABLE GET_TCM_STDOUT
                 ERROR_VARIABLE GET_TCM_STDERR
                 OUTPUT_STRIP_TRAILING_WHITESPACE
)

IF( NOT ${GET_TCM_RES} MATCHES "0" )
    MESSAGE ( FATAL_ERROR "${ALT_BSP_QUEUE} failed with: ${GET_TCM_STDERR}" )
ENDIF ( NOT  ${GET_TCM_RES} MATCHES "0" )

SET( TCI_MEM_SIZE ${GET_TCM_STDOUT} )

MESSAGE( STATUS "Size TCIMEM: ${TCI_MEM_SIZE}" )

# Generate application Makefile
SET( APP_CFLAGS "${CMAKE_C_FLAGS} -DALT_TCIMEM_SIZE=${TCI_MEM_SIZE}" )

SET( ALT_APP_GEN_ARGS
                      "--bsp-dir ${ALT_PCP_BSP_DIR}"
                      "--set QUARTUS_PROJECT_DIR=${CFG_NIOS2_QUARTUS_DIR}"
                      "--src-files ${ALT_PSI_SRCS}"
                      "--set APP_INCLUDE_DIRS=${ALT_PSI_INCS}"
                      "--set APP_CFLAGS_DEFINED_SYMBOLS"
                      "--set QSYS_SUB_CPU=${NIOS2_QSYS_SUB_CPU}"
                      "--set OBJDUMP_INCLUDE_SOURCE=1"
                      "--set CREATE_OBJDUMP=0"
                      "--set FLASH_OVERRIDE=${ALT_FLASH_OVERRIDE}"
                      "--set INSTANCE_ID=${ALT_PCP_INSTANCE_ID}"
                      "--set CFLAGS=${APP_CFLAGS}"
                      "--set APP_CFLAGS_OPTIMIZATION=${OPT_LEVEL}"
                      "--elf-name ${PROJECT_NAME}.elf"
                      "--use-lib-dir ${psicommonpcp_BINARY_DIR}/${ALT_BUILD_DIR_NAME}"
   )

EXECUTE_PROCESS( COMMAND ${ALT_APP_GEN_MAKEFILE} ${ALT_APP_GEN_ARGS}
                 WORKING_DIRECTORY ${ALT_BUILD_DIR}
                 RESULT_VARIABLE GEN_MAKE_RES
                 OUTPUT_VARIABLE GEN_MAKE_STDOUT
                 ERROR_VARIABLE GEN_MAKE_STDERR
)

IF( NOT ${GEN_MAKE_RES} MATCHES "0" )
    MESSAGE ( FATAL_ERROR "${ALT_APP_GEN_MAKEFILE} failed with: ${GEN_MAKE_STDERR}" )
ENDIF ( NOT  ${GEN_MAKE_RES} MATCHES "0" )

MESSAGE ( STATUS "Generate application Makefile: ${GEN_MAKE_STDOUT}" )

# Fix Makefile issues
EXECUTE_PROCESS( COMMAND bash ${ALT_MISC_DIR}/scripts/fix_makefile.sh
                 WORKING_DIRECTORY ${ALT_BUILD_DIR}
                 RESULT_VARIABLE FIX_RES
                 OUTPUT_VARIABLE FIX_STDOUT
                 ERROR_VARIABLE FIX_STDERR
)

IF( NOT ${FIX_RES} MATCHES "0" )
    MESSAGE ( FATAL_ERROR "Failed to fix Makefile with: ${FIX_STDERR}" )
ENDIF ( NOT  ${FIX_RES} MATCHES "0" )

# Copy triple buffer configuration file
FILE( MAKE_DIRECTORY ${DEMO_CONFIG_DIR}/tbuf/include/ipcore )
FILE( COPY ${ALT_PCP_BSP_DIR}/tbuf-cfg.h DESTINATION ${DEMO_CONFIG_DIR}/tbuf/include/ipcore )

########################################################################
# Connect the CMake Makefile with the Altera Makefile
########################################################################
ConnectCMakeAlteraExeTargets(${PROJECT_NAME} ${ALT_BUILD_DIR})

########################################################################
# Eclipse project files
########################################################################
GenEclipseFileList("${PSI_SRCS}" "" PART_ECLIPSE_FILE_LIST )
SET( ECLIPSE_FILE_LIST "${ECLIPSE_FILE_LIST} ${PART_ECLIPSE_FILE_LIST}" )

GenEclipseFileList("${IP_SRCS}" "ip" PART_ECLIPSE_FILE_LIST )
SET( ECLIPSE_FILE_LIST "${ECLIPSE_FILE_LIST} ${PART_ECLIPSE_FILE_LIST}" )

GenEclipseFileList("${OPLK_SRCS}" "oplk" PART_ECLIPSE_FILE_LIST )
SET( ECLIPSE_FILE_LIST "${ECLIPSE_FILE_LIST} ${PART_ECLIPSE_FILE_LIST}" )

GenEclipseFileList("${OMETHLIB_SRCS}" "omethlib" PART_ECLIPSE_FILE_LIST )
SET( ECLIPSE_FILE_LIST "${ECLIPSE_FILE_LIST} ${PART_ECLIPSE_FILE_LIST}" )

GenEclipseFileList("${ALT_TARGET_SRCS}" "target" PART_ECLIPSE_FILE_LIST )
SET( ECLIPSE_FILE_LIST "${ECLIPSE_FILE_LIST} ${PART_ECLIPSE_FILE_LIST}" )

CONFIGURE_FILE( ${ALT_MISC_DIR}/project/appproject.in ${ALT_BUILD_DIR}/.project @ONLY )
CONFIGURE_FILE( ${ALT_MISC_DIR}/project/appcproject.in ${ALT_BUILD_DIR}/.cproject @ONLY )
