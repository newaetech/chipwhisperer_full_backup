/*
 * Copyright (c) 2016, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the Intel Corporation nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __SOC_FLASH_PARTITIONS_H__
#define __SOC_FLASH_PARTITIONS_H__

#include "flash_layout.h"

/**
 * Flash partition definition for Quark SE C1000 Microcontrollers.
 *
 * @defgroup groupSEFlashPart Quark SE C1000 Flash Partitions
 * @{
 */

/*
 * Bootloader data
 */

/* The flash controller where BL-Data is stored. */
#define BL_DATA_FLASH_CONTROLLER QM_FLASH_0
/* The flash region where BL-Data is stored. */
#define BL_DATA_FLASH_REGION QM_FLASH_REGION_SYS
/* The flash address where the BL-Data Section starts. */
#define BL_DATA_FLASH_REGION_BASE QM_FLASH_REGION_SYS_0_BASE
/* The flash page where the BL-Data Section starts. */
#define BL_DATA_SECTION_BASE_PAGE (94)

/* The size (in pages) of the System_0 flash region of Quark SE. */
#define QM_FLASH_REGION_SYS_0_PAGES (96)
/* The size (in pages) of the System_1 flash region of Quark SE. */
#define QM_FLASH_REGION_SYS_1_PAGES (96)

/* The size (in pages) of the Bootloader Data section. */
#define BL_DATA_SECTION_PAGES (2)

/* TODO: Remove after conflicting macro is removed from BSP */
#undef BL_PARTITION_SIZE_LMT

/* The size (in pages) of the 2nd-stage bootloader. */
#if (BL_HAS_2ND_STAGE)
#define BL_2ND_STAGE_BOOTLOADER_PAGES (10)
#else
#define BL_2ND_STAGE_BOOTLOADER_PAGES (0)
#endif

#if (BL_CONFIG_DUAL_BANK)
/* ARC Partition size, in pages */
#define BL_PARTITION_SIZE_ARC                                                  \
	((QM_FLASH_REGION_SYS_0_PAGES - BL_DATA_SECTION_PAGES) / 2)
#define BL_PARTITION_SIZE_LMT (QM_FLASH_REGION_SYS_1_PAGES / 2)
#else /* !BL_CONFIG_DUAL_BANK */
#define BL_PARTITION_SIZE_ARC                                                  \
	((QM_FLASH_REGION_SYS_0_PAGES - BL_DATA_SECTION_PAGES))
#define BL_PARTITION_SIZE_LMT                                                  \
	(QM_FLASH_REGION_SYS_1_PAGES - BL_2ND_STAGE_BOOTLOADER_PAGES)
#endif /* BL_CONFIG_DUAL_BANK */

/* Number of boot targets. */
#define BL_BOOT_TARGETS_NUM (2)

#define BL_TARGET_IDX_LMT (0)
#define BL_TARGET_IDX_ARC (1)

#define BL_PARTITION_IDX_LMT0 (0)
#define BL_PARTITION_IDX_ARC0 (1)
#define BL_PARTITION_IDX_LMT1 (2)
#define BL_PARTITION_IDX_ARC1 (3)

#define BL_TARGET_0_LMT                                                        \
	{                                                                      \
		.active_partition_idx = BL_PARTITION_IDX_LMT0, .svn = 0        \
	}

#define BL_TARGET_1_ARC                                                        \
	{                                                                      \
		.active_partition_idx = BL_PARTITION_IDX_ARC0, .svn = 0        \
	}

/*
 * Macro for defining an application flash partition.
 *
 * @param[in] target The index of the target associated with the partition.
 * @param[in] ctrl   The flash controller on which the partition is located.
 * @param[in] region_addr The base address of the region where the partition is
 * 			  located.
 * @param[in] size The size in pages of the partition.
 * @param[in] idx  The index of the partition within the flash region (0 for
 * 		   the first partition in the region, 1 for the second one).
 */
#define DEFINE_PARTITION(target, ctrl, region_addr, size, idx)                 \
	{                                                                      \
		.target_idx = target, .controller = ctrl,                      \
		.first_page = (idx * size), .num_pages = size,                 \
		.start_addr = ((uint32_t *)region_addr) +                      \
			      (idx * size * QM_FLASH_PAGE_SIZE_DWORDS),        \
		.is_consistent = true                                          \
	}

/* PARTITION 0: LMT-0 */
#define BL_PARTITION_0                                                         \
	DEFINE_PARTITION(BL_TARGET_IDX_LMT, QM_FLASH_1,                        \
			 QM_FLASH_REGION_SYS_1_BASE, BL_PARTITION_SIZE_LMT, 0)

/* PARTITION 1: ARC-0 */
#define BL_PARTITION_1                                                         \
	DEFINE_PARTITION(BL_TARGET_IDX_ARC, QM_FLASH_0,                        \
			 QM_FLASH_REGION_SYS_0_BASE, BL_PARTITION_SIZE_ARC, 0)

/* PARTITION 2: LMT-1 */
#define BL_PARTITION_2                                                         \
	DEFINE_PARTITION(BL_TARGET_IDX_LMT, QM_FLASH_1,                        \
			 QM_FLASH_REGION_SYS_1_BASE, BL_PARTITION_SIZE_LMT, 1)

/* PARTITION 3: ARC-1 */
#define BL_PARTITION_3                                                         \
	DEFINE_PARTITION(BL_TARGET_IDX_ARC, QM_FLASH_0,                        \
			 QM_FLASH_REGION_SYS_0_BASE, BL_PARTITION_SIZE_ARC, 1)

#define BL_TARGET_LIST                                                         \
	{                                                                      \
		BL_TARGET_0_LMT, BL_TARGET_1_ARC                               \
	}

#if BL_CONFIG_DUAL_BANK
#define BL_PARTITION_LIST                                                      \
	{                                                                      \
		BL_PARTITION_0, BL_PARTITION_1, BL_PARTITION_2, BL_PARTITION_3 \
	}
#else /* !BL_CONFIG_DUAL_BANK */
#define BL_PARTITION_LIST                                                      \
	{                                                                      \
		BL_PARTITION_0, BL_PARTITION_1                                 \
	}

#endif /* BL_CONFIG_DUAL_BANK */

/**
 * @}
 */

#endif /* __SOC_FLASH_PARTITIONS_H__ */
