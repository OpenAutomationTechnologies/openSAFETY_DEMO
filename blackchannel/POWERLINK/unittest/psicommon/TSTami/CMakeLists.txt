################################################################################
#
# CMake slim interface library tests for the ami module
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

PROJECT (tstami)

FILE ( GLOB TST_DRIVER_SRC "${PROJECT_SOURCE_DIR}/Driver/*.c" )
SOURCE_GROUP ( Driver FILES ${TST_DRIVER_SRC} )

FILE ( GLOB TST_STUBS_SRC "${PROJECT_SOURCE_DIR}/Stubs/*.c" )
SOURCE_GROUP ( Driver FILES ${TST_STUBS_SRC} )

SET ( PSI_UUT
        ${psicommon_SOURCE_DIR}/amile.c
)

SOURCE_GROUP ( Uut FILES ${PSI_UUT} )

SET ( TST_SOURCES
    ${TST_DRIVER_SRC}
    ${TST_STUBS_SRC}
    ${PSI_UUT}
    ${PROJECT_SOURCE_DIR}/../../common/cunit_main.c
)

# Set macro to indicate expected endianess [PLATFORM_LE, PLATFORM_BE]
ADD_DEFINITIONS(-DPLATFORM_LE)

SimpleTest ( "TSTami" "tstami" "${TST_SOURCES}" )
SET_TARGET_INCLUDE ( "tstami" "${PROJECT_SOURCE_DIR}" )

IF (WIN32)
    SET_TARGET_INCLUDE ( tstami "${CMAKE_SOURCE_DIR}/blackchannel/POWERLINK/contrib/win32" )

    TARGET_LINK_LIBRARIES( tstami "win32" )
    ADD_DEPENDENCIES ( tstami "win32")
endif (WIN32)

AddCoverage ( "PSI" "tstami" )
