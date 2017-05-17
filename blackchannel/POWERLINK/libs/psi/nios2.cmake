 ################################################################################
#
# CMake file of slim interface library (nios2 target) for PSI
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

# Disable CMake library target
UNSET(GEN_LIB_TARGET)
SET(GEN_LIB_TARGET OFF)

##########################################################################
# Set build directory for the Altera Makefile
SET(ALT_BUILD_DIR ${PROJECT_BINARY_DIR}/${ALT_BUILD_DIR_NAME})

FILE(MAKE_DIRECTORY ${ALT_BUILD_DIR})

########################################################################
# Adapt source file lists and includes
########################################################################
SET( ALT_LIB_SRCS ${LIB_SRCS} )

SET( ALT_LIB_INCS ${LIB_INCS} )

########################################################################
# Library Makefile
########################################################################
get_directory_property(DEF_LIST COMPILE_DEFINITIONS)
SET ( DEFINITIONS "" )
GenerateCompileDefinitionFlagsFromList ("${DEF_LIST}" DEFINITIONS)
SET (FLAGS "${CMAKE_C_FLAGS} ${DEFINITIONS}")

SET( ALT_LIB_GEN_ARGS
                     "--bsp-dir ${ALT_APP_BSP_DIR}"
                     "--lib-dir ${ALT_BUILD_DIR}"
                     "--lib-name ${PROJECT_NAME}"
                     "--set LIB_CFLAGS_DEFINED_SYMBOLS=${FLAGS}"
                     "--set LIB_CFLAGS_OPTIMIZATION=${OPT_LEVEL}"
                     "--set LIB_INCLUDE_DIRS=${ALT_LIB_INCS}"
                     "--src-files ${ALT_LIB_SRCS}"
   )

EXECUTE_PROCESS( COMMAND ${ALT_LIB_GEN_MAKEFILE} ${ALT_LIB_GEN_ARGS}
                 WORKING_DIRECTORY ${ALT_BUILD_DIR}
                 RESULT_VARIABLE GEN_LIB_RES
                 OUTPUT_VARIABLE GEN_LIB_STDOUT
                 ERROR_VARIABLE GEN_LIB_STDERR
)

IF( NOT ${GEN_LIB_RES} MATCHES "0" )
    MESSAGE ( FATAL_ERROR "${ALT_LIB_GEN_MAKEFILE} failed with: ${GEN_LIB_STDERR}" )
ENDIF ( NOT  ${GEN_LIB_RES} MATCHES "0" )

MESSAGE ( STATUS "Generate ${PROJECT_NAME} Makefile: ${GEN_LIB_STDOUT}" )

########################################################################
# Connect the CMake Makefile with the Altera Makefile
########################################################################
ConnectCMakeAlteraLibTargets(${PROJECT_NAME} ${ALT_BUILD_DIR})

########################################################################
# Eclipse project files
########################################################################
GenEclipseFileList("${ALT_LIB_SRCS}" "" ECLIPSE_FILE_LIST)

CONFIGURE_FILE( ${ALT_MISC_DIR}/project/libproject.in ${ALT_BUILD_DIR}/.project @ONLY )
CONFIGURE_FILE( ${ALT_MISC_DIR}/project/libcproject.in ${ALT_BUILD_DIR}/.cproject @ONLY )
