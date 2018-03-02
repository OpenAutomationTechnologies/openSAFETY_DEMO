################################################################################
#
# CMake file for the openSAFETY DEMO for processing flash files and adding
# targets for flashing the boards used in the demo
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

# This macro generates a .jic file from a .sof file and a related .elf file.
# The .jic file can then be flashed onto the FPGA board via the Quartus
# Programmer.
MACRO (GenerateJICFile SOF_FILE ELF_FILE CHAIN_FILE CONVERSION_FILE OUTPUT_DIR)

    GET_FILENAME_COMPONENT (GEN_JIC_SOF_FILE_NAME ${SOF_FILE} NAME)
    SET (HW_IMAGE hwimage.flash )
    SET (SW_IMAGE swimage.flash)
    SET (HEX_FILE swimage.hex)

    ADD_CUSTOM_TARGET ( generate_default_fpga_image
        COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR} )

    ADD_CUSTOM_COMMAND ( TARGET generate_default_fpga_image
        COMMAND ${CMAKE_COMMAND} -E copy ${SOF_FILE}
        ${OUTPUT_DIR}/${GEN_JIC_SOF_FILE_NAME}
        COMMAND sof2flash --input=${GEN_JIC_SOF_FILE_NAME}
        --output=${HW_IMAGE} --epcs --verbose
        COMMAND elf2flash --input=${ELF_FILE} --output=${SW_IMAGE}
        --epcs --after=${HW_IMAGE} --verbose
        COMMAND nios2-elf-objcopy --input-target srec --output-target
        ihex ${SW_IMAGE} ${HEX_FILE}
        COMMAND quartus_cpf -c ${CONVERSION_FILE}
        COMMAND ${CMAKE_COMMAND} -E remove *.flash *.hex *.sof *.map
        COMMENT "Generating default FPGA image file"
        WORKING_DIRECTORY ${OUTPUT_DIR}
                      )

ENDMACRO (GenerateJICFile)

# This macro adds a target for flashing a .jic file to the FPGA board using
# the Quartus Programmer
MACRO (AddFlashJICFileTarget CHAIN_FILE JIC_FILE_DIRECTORY)

    FILE (GLOB JIC_FILES ${JIC_FILE_DIRECTORY}/*.jic)

    IF ( NOT EXISTS ${CHAIN_FILE} OR NOT JIC_FILES)

        MESSAGE (WARNING "Cannot find file ${CHAIN_FILE} or no .jic file in directory ${JIC_FILE_DIRECTORY}. Target for flashing default FPGA image not created.")
    ELSE ()
        ADD_CUSTOM_TARGET ( flash_default_fpga_image
                            COMMAND quartus_pgm ${CHAIN_FILE}
                            COMMENT "Flashing default FPGA image for ink-de2-115"
                            WORKING_DIRECTORY ${JIC_FILE_DIRECTORY})
    ENDIF()

ENDMACRO(AddFlashJICFileTarget)

# This macro adds a target for flashing a .elf file onto a STM32Nucleo board
MACRO(FlashSTM32WithDefaultELF ELF_FILE WORKING_DIR FLASH_BASE_ADDR)
    IF(STLINK_UTIL_LOC AND STLINK_FLASH_LOC)
        GET_FILENAME_COMPONENT(FILE_NAME ${ELF_FILE} NAME)
        # Target to write the executable to the local flash
        ADD_CUSTOM_TARGET(
            flash-default-${ELF_FILE}.elf
            COMMAND ${CMAKE_OBJCOPY} -O binary ${ELF_FILE}.elf
            ${FILE_NAME}_temp.bin
            COMMAND ${STLINK_FLASH_LOC} write ${FILE_NAME}_temp.bin
            ${FLASH_BASE_ADDR}
            COMMAND ${CMAKE_COMMAND} -E remove *_temp.bin
            WORKING_DIRECTORY ${WORKING_DIR}
        )
    ELSE()
        MESSAGE(FATAL_ERROR "The tool st-util was not found! The 'flash-default target was not created for '${FILE_NAME}'")
    ENDIF()
ENDMACRO(FlashSTM32WithDefaultELF)

# This macro adds a target for flashing a .elf file onto a STM32Nucleo board
MACRO(FlashSTM32WithDefaultBIN BIN_FILE WORKING_DIR FLASH_BASE_ADDR)
    IF(STLINK_UTIL_LOC AND STLINK_FLASH_LOC)
        GET_FILENAME_COMPONENT(FILE_NAME ${BIN_FILE} NAME)
        # Target to write the executable to the local flash
        ADD_CUSTOM_TARGET(
            flash-default-${BIN_FILE}.bin
            COMMAND ${STLINK_FLASH_LOC} write ${FILE_NAME}.bin ${FLASH_BASE_ADDR}
            WORKING_DIRECTORY ${WORKING_DIR}
        )
    ELSE()
        MESSAGE(FATAL_ERROR "The tool st-util was not found! The 'flash-default target was not created for '${FILE_NAME}'")
    ENDIF()
ENDMACRO(FlashSTM32WithDefaultBIN)