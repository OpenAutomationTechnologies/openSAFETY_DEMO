################################################################################
#
# CMake slim interface library tests for the internal module
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

CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (tstinternal)

FILE ( GLOB TST_DRIVER_SRC "${PROJECT_SOURCE_DIR}/Driver/*.c" )
SOURCE_GROUP ( Driver FILES ${TST_DRIVER_SRC} )

FILE ( GLOB COMMON_STUBS_SRC "${PROJECT_SOURCE_DIR}/../common/general/Stubs/*.c" )
FILE ( GLOB TST_STUBS_SRC "${PROJECT_SOURCE_DIR}/Stubs/*.c" )
SOURCE_GROUP ( Driver FILES ${TST_STUBS_SRC} ${COMMON_STUBS_SRC} )

SET ( PSI_SUPPORT
        ${psi_SOURCE_DIR}/error.c
        ${psi_SOURCE_DIR}/stream.c
        ${psi_SOURCE_DIR}/cc.c
        ${psi_SOURCE_DIR}/ssdo.c
        ${psi_SOURCE_DIR}/status.c
        ${psi_SOURCE_DIR}/logbook.c
)

SET ( PSI_UUT
        ${psi_SOURCE_DIR}/internal.c
)

SOURCE_GROUP ( Support FILES ${PSI_SUPPORT} )
SOURCE_GROUP ( Uut FILES ${PSI_UUT} )

SET ( TST_SOURCES
    ${TST_DRIVER_SRC}
    ${TST_STUBS_SRC}
    ${COMMON_STUBS_SRC}
    ${PSI_UUT}
    ${PSI_SUPPORT}
    ${PROJECT_SOURCE_DIR}/../../common/cunit_main.c
)

SimpleTest ( "TSTinternal" "tstinternal" "${TST_SOURCES}" )
SET_TARGET_INCLUDE ( "tstinternal" "${PROJECT_SOURCE_DIR}" )

IF (WIN32)
    SET_TARGET_INCLUDE ( tstinternal "${CMAKE_SOURCE_DIR}/blackchannel/POWERLINK/contrib/win32" )

    TARGET_LINK_LIBRARIES( tstinternal "win32" )
    ADD_DEPENDENCIES ( tstinternal "win32")
endif (WIN32)

TARGET_LINK_LIBRARIES( tstinternal "psicommon" )
ADD_DEPENDENCIES ( tstinternal "psicommon")
EnsureLibraries( tstinternal "psicommon" )

AddCoverage ( "PSI" "tstinternal" )
