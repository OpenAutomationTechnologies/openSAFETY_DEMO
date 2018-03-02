################################################################################
#
# Linux configuration options for POWERLINK Interface For Applications
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

############################################################
# Check includes
SET ( CMAKE_REQUIRED_INCLUDES "/usr/include" )

CHECK_INCLUDE_FILE ( "sys/socket.h" HAVE_SYS_SOCKET_H )
CHECK_INCLUDE_FILE ( "netinet/in.h" HAVE_NETINET_IN_H )
CHECK_INCLUDE_FILE ( "arpa/inet.h" HAVE_ARPA_INET_H )
CHECK_INCLUDE_FILE ( "features.h" HAVE_FEATURES_H )
CHECK_INCLUDE_FILE ( "arpa/nameser.h" HAVE_ARPA_NAMESER_H )
CHECK_INCLUDE_FILE ( "sys/param.h" HAVE_SYS_PARAM_H )
CHECK_INCLUDE_FILE ( "sys/types.h" HAVE_SYS_TYPES_H )
CHECK_INCLUDE_FILE ( "sys/libgen.h" HAVE_LIBGEN_H )
CHECK_INCLUDE_FILE ( "stdarg.h" HAVE_STDARG_H )

CHECK_FUNCTION_EXISTS( "bzero" HAVE_BZERO )
CHECK_FUNCTION_EXISTS( "inet_pton" HAVE_INET_PTON )

CHECK_FUNCTION_EXISTS( "snprintf" HAVE_SNPRINTF )
CHECK_FUNCTION_EXISTS( "strdup" HAVE_STRDUP )
CHECK_FUNCTION_EXISTS( "vsnprintf" HAVE_VSNPRINTF )

IF (BUILD_SHARED_LIBS)
  # User wants to build Dynamic Libraries, so change the LIB_TYPE variable to CMake keyword 'SHARED'
  SET (LIB_TYPE SHARED)
ENDIF (BUILD_SHARED_LIBS)

IF ( CYGWIN )
    MESSAGE ( STATUS "Building on Cygwin/Windows" )
ELSE ( CYGWIN )
    MESSAGE ( STATUS "Building on Linux" )

    # Workaround for 64-Bit
    # The current stack implementation is not able to handle real 64-bit mode.
    # Problems occur, because the code tries to assign pointers (which have 8 byte in 64-bit)
    # to UINT32 which are by nature only 32bit. Solution for now is to allow only 32bit target
    # being built on Linux, which is handled by adding -m32 to gcc
    IF(UNIX)
        IF (CMAKE_COMPILER_IS_GNUCC)
            # We have a 64 bit compiler, let's add a switch for 32 bit
            IF ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
                SET( CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-m32 -Wformat=0")
            ENDIF ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
        ENDIF(CMAKE_COMPILER_IS_GNUCC)
    ENDIF(UNIX)
ENDIF ( CYGWIN )

SET( WIN32_EXECUTABLE "" )

SET( TARGET_DIR ${APP_TARGET_DIR}/x86 )