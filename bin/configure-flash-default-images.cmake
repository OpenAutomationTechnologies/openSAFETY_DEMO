################################################################################
#
# Default image flashing configuration options for openSAFETY DEMO
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

INCLUDE (FlashDefaultImageTargets)

IF ( CFG_ARM_BOARD_TYPE )
    SET ( DEFAULT_BINARY_DIR ${BIN_DIR}/${CFG_ARM_BOARD_TYPE} )

    FILE ( GLOB ARM_ELF_FILES ${DEFAULT_BINARY_DIR}/*.elf )

    #add flash targets for .elf files
    FOREACH (_item ${ARM_ELF_FILES} )
        GET_FILENAME_COMPONENT(ELF_FILE ${_item} NAME)
        STRING(REGEX REPLACE "\\.[^.]*$" "" ELF_FILENAME_WITHOUT_EXT ${ELF_FILE})

        # add target for flashing stm32 nucleo board
        FlashSTM32WithDefaultELF( ${ELF_FILENAME_WITHOUT_EXT} ${DEFAULT_BINARY_DIR} 0x08000000 )
    ENDFOREACH()


    #add flash targets for .bin files
    FILE ( GLOB ARM_BIN_FILES ${DEFAULT_BINARY_DIR}/*.bin )

    FOREACH (_item ${ARM_BIN_FILES} )
        GET_FILENAME_COMPONENT(BIN_FILE ${_item} NAME)
        STRING(REGEX REPLACE "\\.[^.]*$" "" BIN_FILENAME_WITHOUT_EXT ${BIN_FILE})

        # add target for flashing stm32 nucleo board
        FlashSTM32WithDefaultBIN( ${BIN_FILENAME_WITHOUT_EXT} ${DEFAULT_BINARY_DIR} 0x08000000 )
    ENDFOREACH()

ENDIF()

IF((CMAKE_SYSTEM_NAME STREQUAL "Generic") AND (CMAKE_SYSTEM_PROCESSOR STREQUAL "nios2"))

    SET ( ALTERA_DEFAULT_SOF_DIR ${BIN_DIR}/fpga/boards/terasic-de2-115 )
    SET ( ALTERA_CHAIN_DESCRIPTION_FILE ${MISC_DIR}/altera_nios2/scripts/chain.cdf )

    IF ( GEN_JIC_FILE )

        SET ( ALTERA_SOF_FILE  ${CFG_NIOS2_QUARTUS_DIR}/ink-de2-115.sof )
        SET ( ALTERA_PCPPSI_ELF_FILE ${ALT_PCP_BUILD_DIR}/pcppsi.elf )
        SET ( ALTERA_CHAIN_DESCRIPTION_FILE ${MISC_DIR}/altera_nios2/scripts/chain.cdf )
        SET ( ALTERA_CONVERSION_SETTINGS_FILE ${MISC_DIR}/altera_nios2/scripts/convert.cof )

        GenerateJICFile(${ALTERA_SOF_FILE} ${ALTERA_PCPPSI_ELF_FILE} ${ALTERA_CHAIN_DESCRIPTION_FILE} ${ALTERA_CONVERSION_SETTINGS_FILE} ${ALTERA_DEFAULT_SOF_DIR})

    ENDIF()

    # add target for flashing ink-de2-115 board
    AddFlashJICFileTarget(${ALTERA_CHAIN_DESCRIPTION_FILE} ${ALTERA_DEFAULT_SOF_DIR})

ENDIF()

