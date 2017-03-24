################################################################################
#
# CMake file of slim interface on pcp for PSI
#
# Copyright (c) 2013, Bernecker+Rainer Industrie-Elektronik Ges.m.b.H. (B&R)
# Copyright (c) 2016, Kalycito Infotech Private Ltd
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

PROJECT( pcppsi C )

########################################################################
# Set paths
SET(DEMO_BOARD_DIR ${CMAKE_SOURCE_DIR}/blackchannel/POWERLINK/fpga/boards/altera/terasic-de2-115)
SET(OPLK_BASE_DIR ${CMAKE_SOURCE_DIR}/blackchannel/POWERLINK/stacks/openPOWERLINK)
SET(IP_BASE_DIR ${CMAKE_SOURCE_DIR}/blackchannel/POWERLINK/stacks/ip)

SET(OPLK_STACK_DIR ${OPLK_BASE_DIR}/stack)
SET(OPLK_APPS_DIR ${OPLK_BASE_DIR}/apps)
SET(OPLK_HW_DIR ${OPLK_BASE_DIR}/hardware)
SET(OPLK_OMETHLIB_DIR ${OPLK_HW_DIR}/drivers/openmac)
SET(APP_COMMON_SOURCE_DIR ${OPLK_BASE_DIR}/apps/common/src)
SET(OBJDICT_DIR ${OPLK_BASE_DIR}/apps/common/objdicts/CiA401_CN)
########################################################################
# Set includes
INCLUDE(${OPLK_STACK_DIR}/cmake/directories.cmake)
INCLUDE(${OPLK_STACK_DIR}/cmake/stackfiles.cmake)

########################################################################
# Set all source files
########################################################################
SET(PSI_SRCS
    ${PROJECT_SOURCE_DIR}/psi.c
    ${PROJECT_SOURCE_DIR}/main.c
    ${PROJECT_SOURCE_DIR}/tbuf.c
    ${PROJECT_SOURCE_DIR}/status.c
    ${PROJECT_SOURCE_DIR}/logbook.c
    ${PROJECT_SOURCE_DIR}/cc.c
    ${PROJECT_SOURCE_DIR}/icc.c
    ${PROJECT_SOURCE_DIR}/occ.c
    ${PROJECT_SOURCE_DIR}/pdo.c
    ${PROJECT_SOURCE_DIR}/rpdo.c
    ${PROJECT_SOURCE_DIR}/tpdo.c
    ${PROJECT_SOURCE_DIR}/rssdo.c
    ${PROJECT_SOURCE_DIR}/tssdo.c
    ${PROJECT_SOURCE_DIR}/fifo.c
    ${PROJECT_SOURCE_DIR}/event.c
   )

SET(IP_SRCS
    ${IP_BASE_DIR}/ip.c
    ${IP_BASE_DIR}/ip_name.c
    ${IP_BASE_DIR}/ip_dhcp.c
    ${IP_BASE_DIR}/edrv2veth.c
    ${IP_BASE_DIR}/hton.c
    ${IP_BASE_DIR}/socketwrapper.c
   )

SET(OPLK_SRCS
     ${KERNEL_SOURCES}
     ${CTRL_KCAL_DIRECT_SOURCES}
     ${DLL_KCAL_CIRCBUF_SOURCES}
     ${EVENT_KCAL_NOOSKERNEL_SOURCES}
     ${ERRHND_KCAL_LOCAL_SOURCES}
     ${PDO_KCAL_LOCAL_SOURCES}
     ${HARDWARE_DRIVER_OPENMAC_SOURCES}
     ${HARDWARE_DRIVER_OPENMAC_CN_SOURCES}
     ${USER_SOURCES}
     ${CTRL_UCAL_DIRECT_SOURCES}
     ${DLL_UCAL_CIRCBUF_SOURCES}
     ${ERRHND_UCAL_LOCAL_SOURCES}
     ${PDO_UCAL_LOCAL_SOURCES}
     ${EVENT_UCAL_NOOSKERNEL_SOURCES}
     ${USER_TIMER_GENERIC_SOURCES}
     ${VETH_USER_NOOS_SOURCES}
     ${COMMON_SOURCES}
     ${APP_COMMON_SOURCE_DIR}/obdcreate/obdcreate.c
     ${COMMON_CAL_DIRECT_SOURCES}
     ${CIRCBUF_NOOS_SOURCES}
     ${COMMON_NOOS_SOURCES}
     ${ARCH_LE_SOURCES}
     ${LIB_KERNEL_SOURCES}
     ${LIB_USER_SOURCES}
     ${LIB_COMMON_SOURCES}
     ${SDO_SOCKETWRAPPER_SOURCES}
     ${MEMMAP_NOOSLOCAL_SOURCES}
   )

SET(OMETHLIB_SRCS
    ${OPLK_OMETHLIB_DIR}/src/omethlib.c
    ${OPLK_OMETHLIB_DIR}/src/omethlibint.c
   )

###############################################################################
# Set CFLAGS depending on build type
IF(${CMAKE_BUILD_TYPE} MATCHES "Debug" OR ${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
    SET(DBG_MODE _DEBUG)
    SET(CFG_PCP_DEBUG_LVL 0xC0000000L CACHE STRING "Set the level of the debug output")
ELSE()
    #All other builds are release builds
    SET(DBG_MODE NDEBUG)
    UNSET(CFG_PCP_DEBUG_LVL CACHE)
    SET(CFG_PCP_DEBUG_LVL 0x00000000L)
ENDIF()

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D${DBG_MODE} -DDEF_DEBUG_LVL=${CFG_PCP_DEBUG_LVL} -DPSI_BUILD_PCP -DCONFIG_CN")

########################################################################
# Set include paths
########################################################################
SET(PSI_INCS
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
    ${psicommonpcp_SOURCE_DIR}/include
    ${DEMO_CONFIG_DIR}/tbuf/include
    ${IP_BASE_DIR}
    ${APP_COMMON_DIR}/include/common
    ${APP_COMMON_SOURCE_DIR}
    )

SET(OPLK_INCS
    ${STACK_INCLUDE_DIR}
    ${STACK_SOURCE_DIR}
    ${CONTRIB_SOURCE_DIR}
    ${DEMO_CONFIG_DIR}/pcp
    ${CONTRIB_SOURCE_DIR}/socketwrapper
    ${DEMO_BOARD_DIR}/include
    ${OPLK_HW_DIR}/boards/terasic-de2-115/cn-single-gpio/include
   )

SET(OMETHLIB_INCS
    ${OPLK_OMETHLIB_DIR}/include
    ${OPLK_OMETHLIB_DIR}/src
   )

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCONFIG_DLL_PRES_CHAINING_CN=TRUE -DNMT_MAX_NODE_ID=254 -DCONFIG_INCLUDE_PDO -DCONFIG_INCLUDE_SDO_ASND -DCONFIG_INCLUDE_MASND -DCONFIG_INCLUDE_SDO_UDP")

IF((CMAKE_SYSTEM_NAME STREQUAL "Generic") AND (CMAKE_SYSTEM_PROCESSOR STREQUAL "nios2"))
    INCLUDE(nios2.cmake)
ENDIF()

################################################################################
# Add clean files
SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${ADD_CLEAN_FILES}")