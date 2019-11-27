################################################################################
# \file module.mk
# \version 1.10
#
# \brief
# This is the PSoC6 PDL drivers component
#
################################################################################
# \copyright
# Copyright 2018-2019 Cypress Semiconductor Corporation
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

ifeq ($(WHICHFILE),true)
$(info Processing $(lastword $(MAKEFILE_LIST)) file from directory $(PWD))
$(info Path: $(MAKEFILE_LIST))
endif

#
# Needed by describe goal processing
#
ifeq ($(MAKECMDGOALS),describe)
ifndef PLATFORMS_VERSION
PLATFORMS_VERSION=1.0
endif
include $(CYSDK)/libraries/platforms-$(PLATFORMS_VERSION)/common/swcomps.mk
endif

#
# The internal tag name of the software component
#
PSOC6_PDL_NAME=PSOC6_PDL

#
# If defined, the list of legal PLATFORM values for this component.
# If not defined, this component is valid for all values of PLATFORM
#
CY_SUPPORTED_PLATFORM_LIST=PSOC6_DUAL_CORE PSoC6_cm4_dual PSoC6_cm0p PSOC6_SINGLE_CORE PSoC6_cm4_single $(PSOC_EXTRA_PLATFORMS)

#
# If defined, the list of legal TOOLCHAIN values for this component.  If not
# defined, this component is valid for all values of TOOLCHAIN
#
#CY_SUPPORTED_TOOLCHAIN_LIST=

#
# Used by the IDE to group and categorize components.
#
CY_COMP_CATEGORY=Drivers

#
# The displayed human readable name of the component
#
CY_COMP_NAME_DISPLAY=PSoC6 PDL drivers

#
# The name in the form of an identifier ([a-z_][a-z0-9_]*).
# Used to generate directories in the IDE.
#
CY_COMP_NAME_ID=psoc6pdl

#
# The human readable description of the component
#
CY_COMP_DESCR=This is the PSoC6 PDL drivers component

#
# The type of component ...
#   link - means link the source code from the IDE project to the SDK
#   copy - means copy the source code into the IDE project
#
CY_COMP_TYPE=link

#
# Defines if the component can change based on which artifact is being used
#
CY_COMP_PER_ARTIFACT=false

#
# The list of components this component is dependent on. Path is relative the
# the SDK's libraries folder.
#
CY_COMP_DEPS=

#
# Used by the build recipe for an ELF file to add this component
# to the list of components that must be included
#
$(CYARTIFACT)_SHARED_COMP_TAG_LIST+=$(PSOC6_PDL_NAME)
$(PSOC6_PDL_NAME)_SHARED=true
$(PSOC6_PDL_NAME)_LOCAL_INCLUDES=\
-I$(CY_PSOC_LIB_COMP_FULL)/cmsis/include\
-I$(CY_PSOC_LIB_COMP_FULL)/devices/psoc6/include\
-I$(CY_PSOC_LIB_COMP_FULL)/drivers/include
$(PSOC6_PDL_NAME)_LOCAL_DEFINES=-DCY_DEVICE_COMMON

ifeq ($(TOOLCHAIN),GCC)
PDL_SSRC = ./gcc/cy_syslib_gcc.S
else
ifeq ($(TOOLCHAIN),IAR)
PDL_SSRC = ./iar/cy_syslib_iar.s
else
ifeq ($(TOOLCHAIN),MDK)
PDL_SSRC = ./mdk/cy_syslib_mdk.s
endif
endif
endif


#
# Defines the series of needed make variables that include this component in the
# build process.  Also defines the describe target if we are describing a component
#
$(eval $(call \
	CY_DECLARE_SWCOMP_LIBRARY,$(PSOC6_PDL_NAME),\
	$(lastword $(MAKEFILE_LIST)),\
	$(PDL_SSRC)\
	../include/cy_ble_clk.h\
	../include/cy_crypto.h\
	../include/cy_crypto_common.h\
	../include/cy_crypto_server.h\
	../include/cy_crypto_core.h\
	../include/cy_crypto_core_vu.h\
	../include/cy_crypto_core_trng.h\
	../include/cy_crypto_core_sha.h\
	../include/cy_crypto_core_rsa.h\
	../include/cy_crypto_core_prng.h\
	../include/cy_crypto_core_mem.h\
	../include/cy_crypto_core_hw_vu.h\
	../include/cy_crypto_core_hw.h\
	../include/cy_crypto_core_hmac.h\
	../include/cy_crypto_core_des.h\
	../include/cy_crypto_core_crc.h\
	../include/cy_crypto_core_cmac.h\
	../include/cy_crypto_core_aes.h\
	../include/cy_crypto_core_trng_v1.h\
	../include/cy_crypto_core_trng_config_v1.h\
	../include/cy_crypto_core_sha_v1.h\
	../include/cy_crypto_core_prng_v1.h\
	../include/cy_crypto_core_mem_v1.h\
	../include/cy_crypto_core_hw_v1.h\
	../include/cy_crypto_core_hmac_v1.h\
	../include/cy_crypto_core_des_v1.h\
	../include/cy_crypto_core_crc_v1.h\
	../include/cy_crypto_core_cmac_v1.h\
	../include/cy_crypto_core_aes_v1.h\
	../include/cy_crypto_core_trng_v2.h\
	../include/cy_crypto_core_trng_config_v2.h\
	../include/cy_crypto_core_sha_v2.h\
	../include/cy_crypto_core_prng_v2.h\
	../include/cy_crypto_core_mem_v2.h\
	../include/cy_crypto_core_hw_v2.h\
	../include/cy_crypto_core_hmac_v2.h\
	../include/cy_crypto_core_des_v2.h\
	../include/cy_crypto_core_crc_v2.h\
	../include/cy_crypto_core_cmac_v2.h\
	../include/cy_crypto_core_aes_v2.h\
	../include/cy_crypto_core_ecc.h\
	../include/cy_crypto_core_ecc_nist_p.h\
	../include/cy_csd.h\
	../include/cy_ctb.h\
	../include/cy_ctdac.h\
	../include/cy_device.h\
	../include/cy_dma.h\
	../include/cy_dmac.h\
	../include/cy_efuse.h\
	../include/cy_flash.h\
	../include/cy_gpio.h\
	../include/cy_i2s.h\
	../include/cy_ipc_drv.h\
	../include/cy_ipc_pipe.h\
	../include/cy_ipc_sema.h\
	../include/cy_lpcomp.h\
	../include/cy_lvd.h\
	../include/cy_mcwdt.h\
	../include/cy_pdl.h\
	../include/cy_pdm_pcm.h\
	../include/cy_profile.h\
	../include/cy_prot.h\
	../include/cy_rtc.h\
	../include/cy_sar.h\
	../include/cy_scb_common.h\
	../include/cy_scb_ezi2c.h\
	../include/cy_scb_i2c.h\
	../include/cy_scb_spi.h\
	../include/cy_scb_uart.h\
	../include/cy_sd_host.h\
	../include/cy_smartio.h\
	../include/cy_smif.h\
	../include/cy_smif_memslot.h\
	../include/cy_sysanalog.h\
	../include/cy_sysclk.h\
	../include/cy_sysint.h\
	../include/cy_syslib.h\
	../include/cy_syspm.h\
	../include/cy_systick.h\
	../include/cy_tcpwm.h\
	../include/cy_tcpwm_counter.h\
	../include/cy_tcpwm_pwm.h\
	../include/cy_tcpwm_quaddec.h\
	../include/cy_trigmux.h\
	../include/cy_usbfs_dev_drv.h\
	../include/cy_usbfs_dev_drv_pvt.h\
	../include/cy_usbfs_dev_drv_reg.h\
	../include/cy_wdt.h\
	cy_ble_clk.c\
	cy_crypto.c\
	cy_crypto_server.c\
	cy_crypto_core_hw.c\
	cy_crypto_core_vu.c\
	cy_crypto_core_rsa.c\
	cy_crypto_core_trng_v1.c\
	cy_crypto_core_sha_v1.c\
	cy_crypto_core_prng_v1.c\
	cy_crypto_core_mem_v1.c\
	cy_crypto_core_hw_v1.c\
	cy_crypto_core_hmac_v1.c\
	cy_crypto_core_des_v1.c\
	cy_crypto_core_crc_v1.c\
	cy_crypto_core_cmac_v1.c\
	cy_crypto_core_aes_v1.c\
	cy_crypto_core_trng_v2.c\
	cy_crypto_core_sha_v2.c\
	cy_crypto_core_prng_v2.c\
	cy_crypto_core_mem_v2.c\
	cy_crypto_core_hmac_v2.c\
	cy_crypto_core_des_v2.c\
	cy_crypto_core_crc_v2.c\
	cy_crypto_core_cmac_v2.c\
	cy_crypto_core_aes_v2.c\
	cy_crypto_core_ecc_ecdsa.c\
	cy_crypto_core_ecc_key_gen.c\
	cy_crypto_core_ecc_nist_p.c\
	cy_crypto_core_ecc_domain_params.c\
	cy_csd.c\
	cy_ctb.c\
	cy_ctdac.c\
	cy_device.c\
	cy_dma.c\
	cy_dmac.c\
	cy_efuse.c\
	cy_flash.c\
	cy_gpio.c\
	cy_i2s.c\
	cy_ipc_drv.c\
	cy_ipc_pipe.c\
	cy_ipc_sema.c\
	cy_lpcomp.c\
	cy_lvd.c\
	cy_mcwdt.c\
	cy_pdm_pcm.c\
	cy_profile.c\
	cy_prot.c\
	cy_rtc.c\
	cy_sar.c\
	cy_scb_common.c\
	cy_scb_ezi2c.c\
	cy_scb_i2c.c\
	cy_scb_spi.c\
	cy_scb_uart.c\
	cy_sd_host.c\
	cy_smartio.c\
	cy_smif.c\
	cy_smif_memslot.c\
	cy_sysanalog.c\
	cy_sysclk.c\
	cy_sysint.c\
	cy_syslib.c\
	cy_syspm.c\
	cy_systick.c\
	cy_tcpwm_counter.c\
	cy_tcpwm_pwm.c\
	cy_tcpwm_quaddec.c\
	cy_trigmux.c\
	cy_usbfs_dev_drv.c\
	cy_usbfs_dev_drv_io.c\
	cy_usbfs_dev_drv_io_dma.c\
	cy_wdt.c))

