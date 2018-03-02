################################################################################
#
# CMake macro to connect the Altera generated Makefile with the CMake Makefile
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
MACRO(ConnectCMakeAlteraTargets PROJ_NAME BUILD_DIR)
    ADD_CUSTOM_TARGET(
        ${PROJ_NAME} ALL
        COMMAND make all
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMENT "Build the application/library, the BSP and all referenced libraries"
    )

    ADD_CUSTOM_TARGET(
        bsp-${PROJ_NAME}
        COMMAND make bsp
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMENT "Just the BSP"
    )

    ADD_CUSTOM_TARGET(
        libs-${PROJ_NAME}
        COMMAND make libs
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMENT "BSP and all libraries used by this application or library"
    )

    ADD_CUSTOM_TARGET(
        clean-${PROJ_NAME}
        COMMAND make clean
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMENT "Just the application or library"
    )

    ADD_CUSTOM_TARGET(
        clean_all-${PROJ_NAME}
        COMMAND make clean_all
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMENT "Clean the application/library, the BSP and all referenced libraries"
    )

    ADD_CUSTOM_TARGET(
        clean_bsp-${PROJ_NAME}
        COMMAND make clean_bsp
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMENT "Just the BSP"
    )

    ADD_CUSTOM_TARGET(
        clean_libs-${PROJ_NAME}
        COMMAND make clean_libs
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMENT "BSP and all libraries used by this application or library"
    )
ENDMACRO()

MACRO(ConnectCMakeAlteraExeTargets PROJ_NAME BUILD_DIR)
    ConnectCMakeAlteraTargets(${PROJ_NAME} ${BUILD_DIR})

    ADD_CUSTOM_TARGET(
        download-elf-${PROJ_NAME}
        COMMAND make download-elf
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMENT "Download the executable"
    )

    ADD_CUSTOM_TARGET(
        download-bits-${PROJ_NAME}
        COMMAND make download-bits
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMENT "Download the FPGA configuration"
    )
    ADD_CUSTOM_TARGET(
        program-nvm-${PROJ_NAME}
        COMMAND make program-nvm
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMENT "Program the local NVM storage (Can be either an epcs or parallel flash)"
    )

    ADD_CUSTOM_TARGET(
        erase-nvm-${PROJ_NAME}
        COMMAND make erase-nvm
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMENT "Erase the local NVM storage (Can be either an epcs or parallel flash)"
    )

    SET( ADD_CLEAN_FILES ${ADD_CLEAN_FILES}
                         ${ALT_BUILD_DIR}/${PROJECT_NAME}.elf
                         ${ALT_BUILD_DIR}/${PROJECT_NAME}.map
                         ${ALT_BUILD_DIR}/${PROJECT_NAME}.objdump
                         ${ALT_BUILD_DIR}/obj
                         ${ALT_BUILD_DIR}/bsp/obj
                         ${ALT_BUILD_DIR}/bsp/libhal_bsp.a
    )
ENDMACRO()

MACRO(ConnectCMakeAlteraLibTargets PROJ_NAME BUILD_DIR)
    ConnectCMakeAlteraTargets(${PROJ_NAME} ${BUILD_DIR})

    ADD_CUSTOM_TARGET(
        lib-${PROJ_NAME}
        COMMAND make lib
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMENT "Just the library"
    )

    SET( ADD_CLEAN_FILES ${ADD_CLEAN_FILES}
                         ${ALT_BUILD_DIR}/lib${PROJECT_NAME}.a
                         ${ALT_BUILD_DIR}/obj
    )
ENDMACRO()