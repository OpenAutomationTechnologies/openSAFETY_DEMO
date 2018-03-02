################################################################################
#
# Generates the board support package for stm32f10x boards
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

IF(CFG_DEMO_TYPE STREQUAL "sn-gpio")
    PROJECT(bsp-${CFG_ARM_BOARD_TYPE}-${CURRENT_DEMO_CONTEXT} C)
ELSE()
    PROJECT(bsp-${CFG_ARM_BOARD_TYPE} C)
ENDIF()

#################################################################################
# USER OPTIONS
IF(NOT CFG_STM32_HAL_LIB_LOC)
    SET(CFG_STM32_HAL_LIB_LOC
        ${PROJECT_SOURCE_DIR}
        CACHE PATH "Set the location of the STM32CUBEF1 HAL library V1.2.0"
       )
ELSEIF(NOT EXISTS ${CFG_STM32_HAL_LIB_LOC}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c)
    MESSAGE(FATAL_ERROR "The (H)ardware (A)bstration (L)ibrary does not seem to exist in "
                        "${CFG_STM32_HAL_LIB_LOC}. Please download the correct library from "
                        "http://www.st.com/web/catalog/tools/FM147/CL1794/SC961/SS1743/LN1897/PF260820 and copy it to the path." )
ENDIF()

#################################################################################
# Target for creating the board support package
SET(STM32_HAL_LIB_PATH ${CFG_STM32_HAL_LIB_LOC}/Drivers)

SET(STM32_HAL_LIB_DIR ${STM32_HAL_LIB_PATH}/STM32F1xx_HAL_Driver)
SET(STM32_HAL_CMSIS_DIR ${STM32_HAL_LIB_PATH}/CMSIS)
SET(STM32_HAL_CMSIS_TARG_DIR ${STM32_HAL_CMSIS_DIR}/Device/ST/STM32F1xx)

SET(HAL_LIB_SRC
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_adc.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_adc_ex.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_can.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_cec.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_cortex.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_crc.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_dac.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_dac_ex.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_dma.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_eth.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_flash.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_flash_ex.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_gpio.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_gpio_ex.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_hcd.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_i2c.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_i2s.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_irda.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_iwdg.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_msp_template.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_nand.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_nor.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_pccard.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_pcd.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_pcd_ex.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_pwr.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_rcc.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_rcc_ex.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_rtc.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_rtc_ex.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_sd.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_smartcard.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_spi.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_spi_ex.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_sram.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_tim.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_tim_ex.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_uart.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_usart.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_hal_wwdg.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_ll_fsmc.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_ll_sdmmc.c
    ${STM32_HAL_LIB_DIR}/Src/stm32f1xx_ll_usb.c
)

SET(HAL_LIB_INCS
                ${PROJECT_SOURCE_DIR}
                ${STM32_HAL_LIB_DIR}/Inc
                ${STM32_HAL_CMSIS_TARG_DIR}/Include
                ${STM32_HAL_CMSIS_DIR}/Include
                CACHE INTERNAL ""
)

INCLUDE_DIRECTORIES(${HAL_LIB_INCS})

#################################################################################
# Set compile flags
STRING (REPLACE "-pedantic" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})

#################################################################################
# Create bsp library
ADD_LIBRARY(${PROJECT_NAME} ${HAL_LIB_SRC} ${HAL_LIB_LOCAL_SRCS})
