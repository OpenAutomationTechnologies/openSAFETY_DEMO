################################################################################
#
# CMake file for the openSAFETY DEMO for getting / setting the openSAFETY
# version
#
# Copyright (c) 2015, B&R Industrial Automation GmbH
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

# sets openSAFETY version information variables
# overwrites existing variables if passed parameter is TRUE,
# else no overwrite.
MACRO (get_openSAFETY_version OVERWRITE_EXISTING_VARIABLES )

    SET (OVERWRITE ${OVERWRITE_EXISTING_VARIABLES})

    IF ( NOT DEFINED openSAFETY_VERSION OR (openSAFETY_VERSION AND OVERWRITE) )
        SET (openSAFETY_VERSION 1)
    ENDIF()

    IF ( NOT DEFINED openSAFETY_CERTIFICATION OR (openSAFETY_CERTIFICATION AND OVERWRITE) )
        SET (openSAFETY_CERTIFICATION 5)
    ENDIF()

    IF ( NOT DEFINED openSAFETY_TOOLSET OR (openSAFETY_TOOLSET AND OVERWRITE) )
        SET (openSAFETY_TOOLSET 0)
    ENDIF()

    IF ( NOT DEFINED openSAFETY_VERSION_RELEASE OR (openSAFETY_VERSION_RELEASE AND OVERWRITE) )
        SET (openSAFETY_VERSION_RELEASE "${openSAFETY_VERSION}.${openSAFETY_CERTIFICATION}.${openSAFETY_TOOLSET}")
    ENDIF()

    IF ( NOT DEFINED openSAFETY_VERSION_NAME OR (openSAFETY_VERSION_NAME AND OVERWRITE_EXISTING_VARIABLES) )
        SET (openSAFETY_VERSION_NAME "'CMake Release'")
    ENDIF()

ENDMACRO()