################################################################################
#
# CMake file of slim interface on demo-sn-gpio for PSI (Target is stm32f401re)
#
# Copyright (c) 2017, B&R Industrial Automation GmbH
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

ENABLE_LANGUAGE(ASM-ATT)

################################################################################
# Set C flags for this board configuration
ADD_DEFINITIONS(-DUSE_HAL_DRIVER -DSTM32F401xE)

################################################################################
# Add board support package for target stm32f4xx
ADD_SUBDIRECTORY(${APP_TARGET_DIR}/stm32f4xx ${PROJECT_BINARY_DIR}/bsp)

################################################################################
# Set paths
SET(BOARD_TARGET_DIR ${SN_TARGET_SOURCE_DIR}/stm32f401rb)

################################################################################
# Set demo linker script
SET(DEMO_LINKER_SCRIPT ${TARGET_DIR}/STM32F401XE.ld)

################################################################################
# Set architecture specific sources
SET ( APP_GPIO_C ${CUSTOMISED_FILES_BUILD_DIR}/app-gpio.c)

SET(DEMO_ARCH_SRCS
                    ${TARGET_DIR}/platform.c
                    ${TARGET_DIR}/syncir.c
                    ${TARGET_DIR}/startup_stm32f401xe.s
                    ${TARGET_DIR}/syscalls.c
                    #${TARGET_DIR}/app-gpio.c
                    ${APP_GPIO_C}
                    ${TARGET_DIR}/stm32f4xx_it.c
                    ${TARGET_DIR}/system_stm32f4xx.c
                    ${BOARD_TARGET_DIR}/timer.c
                    ${BOARD_TARGET_DIR}/nvs.c
                    ${BOARD_TARGET_DIR}/gpio.c
)

IF(${CURRENT_DEMO_CONTEXT} STREQUAL "ups")
    # This project is for the processor up-slave
    SET(DEMO_ARCH_SRCS
                      ${DEMO_ARCH_SRCS}
                      ${TARGET_DIR}/pcpserial-sl.c
                      ${BOARD_TARGET_DIR}/upserial.c
    )
ELSEIF(${CURRENT_DEMO_CONTEXT} STREQUAL "upm")
    # This project is for the processor up-master
    SET(DEMO_ARCH_SRCS
                      ${DEMO_ARCH_SRCS}
                      ${TARGET_DIR}/pcpserial-ma.c
                      ${BOARD_TARGET_DIR}/upserial.c
    )
ELSE()
    # This project is a single processor demo
    SET(DEMO_ARCH_SRCS
                      ${DEMO_ARCH_SRCS}
                      ${TARGET_DIR}/pcpserial-ma.c
    )
ENDIF()

SET(DEMO_ARCH_INCS
                  ${TARGET_DIR}
                  ${HAL_LIB_INCS}
                  ${BOARD_TARGET_DIR}/include
)

################################################################################
# Set list of target specific libraries
SET(DEMO_BSP_NAME "bsp-${CFG_ARM_BOARD_TYPE}-${CURRENT_DEMO_CONTEXT}")
SET(DEMO_ARCH_LIBS "${DEMO_BSP_NAME}")

#################################################################################
# Set compile flags
STRING (REPLACE "-pedantic" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})

################################################################################
# Set architecture specific compile flags
SET(DEMO_ARCH_LINKER_FLAGS "-T${DEMO_LINKER_SCRIPT} -Wl,--start-group -lc -lm -Wl,--end-group -static -Wl,-cref,-u,Reset_Handler -Wl,-Map=${PROJECT_NAME}.map -Wl,--gc-sections -Wl,--defsym=malloc_getpagesize_P=0x1000")

################################################################################
# Create program flash target
IF(CFG_PROG_FLASH_ENABLE)
    CreateProgFlash(${PROJECT_NAME} 0x08000000)
ENDIF()
