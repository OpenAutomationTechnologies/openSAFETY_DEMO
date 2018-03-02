#!/bin/bash
################################################################################
#
# Workaround and extensions for Nios build tools issues
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

fail () {
   echo "ERROR: ${1}"
   exit 1

}

PERM_TOOL=chmod
SED_TOOL=sed
AMEND_TOOL=cat

MAKEFILE_NAME=Makefile

LINKER_X_DIR=bsp
LINKER_X_FILE=linker.x
LINKER_X_PATH=$LINKER_X_DIR/$LINKER_X_FILE

$PERM_TOOL u+rw $LINKER_X_PATH
[ $? -ne 0 ] && fail "$PERM_TOOL failed for $LINKER_X_PATH!"

$SED_TOOL -i 's/AT ( LOADADDR (.rodata) + SIZEOF (.rodata)+ SIZEOF (.rwdata) )/AT ( LOADADDR (.rodata) + SIZEOF (.rodata) )/g' $LINKER_X_PATH
[ $? -ne 0 ] && fail "$SED_TOOL failed for $LINKER_X_PATH!"

$AMEND_TOOL >> $MAKEFILE_NAME <<'Heredoc'

# Rules for EPCS flash programming commands (EPCS contains SOF and application)
PROGRAM_EPCS_SUFFIX := -epcs
PROGRAM_EPCS_TARGET := $(addsuffix $(PROGRAM_EPCS_SUFFIX), $(FLASH_FILES))

# Rule for programming the NVS memory content
.PHONY : program-nvm
program-nvm : $(PROGRAM_EPCS_TARGET)

SOF_FILE := $(wildcard $(QUARTUS_PROJECT_DIR)/*.sof)

.PHONY : $(PROGRAM_EPCS_TARGET)
$(PROGRAM_EPCS_TARGET) : $(ELF)
	@$(ECHO) Info: Programming $(basename $@).flash $($(basename $@)_EPCS_FLAGS)
	@nios2-configure-sof $(DOWNLOAD_CABLE_FLAG) -C $(QUARTUS_PROJECT_DIR)
	@if [ -n "$($(basename $@)_EPCS_FLAGS)" ]; \
	then \
		$(ECHO) Info: Flash is an EPCS flash!;\
		sof2flash --epcs --input=$(SOF_FILE) --output=sof.flash; \
		$(ELF2FLASH) --after=sof.flash --input=$(ELF) --outfile=$(basename $@)_after_sof.flash --sim_optimize=$(SIM_OPTIMIZE) $(elf2flash_extra_args); \
		$(ECHO) $(FLASHPROG) --instance=$(INSTANCE_ID) $(DOWNLOAD_CABLE_FLAG) $(SOPC_SYSID_FLAG) --epcs --base=$($(basename $@)_START) -g --override=$(FLASH_OVERRIDE) sof.flash $(basename $@)_after_sof.flash; \
		$(FLASHPROG) --instance=$(INSTANCE_ID) $(DOWNLOAD_CABLE_FLAG) $(SOPC_SYSID_FLAG) --epcs --base=$($(basename $@)_START) -g --override=$(FLASH_OVERRIDE) sof.flash $(basename $@)_after_sof.flash; \
	else \
		$(ECHO) Info: Flash is a CFI flash!;\
		$(ECHO) $(ELF2FLASH) --input=$(ELF) --outfile=$(basename $@).flash --base=$($(basename $@)_START) --end=$($(basename $@)_END) --reset=$($(basename $@)_START) $(elf2flash_extra_args) --verbose; \
		$(ELF2FLASH) --input=$(ELF) --outfile=$(basename $@).flash --base=$($(basename $@)_START) --end=$($(basename $@)_END) --reset=$($(basename $@)_START) $(elf2flash_extra_args) --verbose; \
		$(ECHO) $(FLASHPROG) --instance=$(INSTANCE_ID) $(DOWNLOAD_CABLE_FLAG) $(SOPC_SYSID_FLAG) --base=$($(basename $@)_START) -g --override=$(FLASH_OVERRIDE) $(basename $@).flash --program --verbose; \
		$(FLASHPROG) --instance=$(INSTANCE_ID) $(DOWNLOAD_CABLE_FLAG) $(SOPC_SYSID_FLAG) --base=$($(basename $@)_START) -g --override=$(FLASH_OVERRIDE) $(basename $@).flash --program --verbose; \
	fi

# Rule for erasing the NVS memory content
.PHONY : erase-nvm
erase-nvm:
	@nios2-configure-sof $(DOWNLOAD_CABLE_FLAG) -C $(QUARTUS_PROJECT_DIR)
	$(ECHO) $(FLASHPROG) --instance=$(INSTANCE_ID) $(DOWNLOAD_CABLE_FLAG) $(SOPC_SYSID_FLAG) $($(basename $(PROGRAM_EPCS_TARGET))_EPCS_FLAGS) --base=$($(basename $(PROGRAM_EPCS_TARGET))_START) --override=$(FLASH_OVERRIDE) --accept-bad-sysid --erase-all
	$(FLASHPROG) --instance=$(INSTANCE_ID) $(DOWNLOAD_CABLE_FLAG) $(SOPC_SYSID_FLAG) $($(basename $(PROGRAM_EPCS_TARGET))_EPCS_FLAGS) --base=$($(basename $(PROGRAM_EPCS_TARGET))_START) --override=$(FLASH_OVERRIDE) --accept-bad-sysid --erase-all

# Rule for downloading the FPGA bitstream to the target
.PHONY : download-bits
download-bits:
	nios2-configure-sof $(DOWNLOAD_CABLE_FLAG) -C $(QUARTUS_PROJECT_DIR)

# Rule for doing magic
.PHONY: download-all
download-all: download-bits download-elf

Heredoc
[ $? -ne 0 ] && fail "$AMEND_TOOL failed for $MAKEFILE_NAME!"

$PERM_TOOL u+rw $MAKEFILE_NAME
[ $? -ne 0 ] && fail "$PERM_TOOL failed for $MAKEFILE_NAME!"

$SED_TOOL -i 's/ --cpu_name=$(CPU_NAME) / --cpu_name=$(QSYS_SUB_CPU) /g' $MAKEFILE_NAME
[ $? -ne 0 ] && fail "$SED_TOOL failed for $MAKEFILE_NAME!"

exit 0;