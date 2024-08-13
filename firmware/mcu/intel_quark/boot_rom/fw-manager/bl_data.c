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

#include <string.h>
#include <stddef.h>

#include "fw-manager_comm.h"

#include "fw-manager_utils.h"
#include "bl_data.h"

/** The page where BL-Data main copy is located. */
#define BL_DATA_SECTION_MAIN_PAGE (BL_DATA_SECTION_BASE_PAGE)
/** The page where BL-Data backup copy is located. */
#define BL_DATA_SECTION_BACKUP_PAGE (BL_DATA_SECTION_BASE_PAGE + 1)

/** The address where BL-Data main copy is located. */
#define BL_DATA_SECTION_MAIN_ADDR                                              \
	(BL_DATA_FLASH_REGION_BASE +                                           \
	 (BL_DATA_SECTION_MAIN_PAGE * QM_FLASH_PAGE_SIZE_BYTES))

/** The address where BL-Data backup copy is located. */
#define BL_DATA_SECTION_BACKUP_ADDR                                            \
	(BL_DATA_FLASH_REGION_BASE +                                           \
	 (BL_DATA_SECTION_BACKUP_PAGE * QM_FLASH_PAGE_SIZE_BYTES))

#if (UNIT_TEST)
/* test_flash_page is already defined qm_soc_regs.h but we need 2 pages for our
 * test. */
uint8_t test_flash_backup_page[QM_FLASH_PAGE_SIZE_BYTES];

static const bl_data_t *const bl_data_main = (bl_data_t *)test_flash_page;
static const bl_data_t *const bl_data_bck = (bl_data_t *)test_flash_backup_page;
#else
static const bl_data_t *const bl_data_main =
    (bl_data_t *)BL_DATA_SECTION_MAIN_ADDR;
static const bl_data_t *const bl_data_bck =
    (bl_data_t *)BL_DATA_SECTION_BACKUP_ADDR;
#endif

static bl_data_t bl_data_shadow = {
    .targets = BL_TARGET_LIST, .partitions = BL_PARTITION_LIST, .crc = 0};

/** Pointer to the bootloader data */
bl_data_t *const bl_data = &bl_data_shadow;

static void bl_data_init()
{
	/*
	 * NOTE: trim codes are previously computed and here we just copy them
	 * to shadowed BL-Data which is then stored to flash (both in main and
	 * backup copy). This works, but it is not optimal and must be changed.
	 * Ideally, trim codes should be compute directly here.
	 */
	memcpy(bl_data, QM_FLASH_DATA_TRIM, sizeof(bl_data->trim_codes));
	bl_data_shadow_writeback();
}

/**
 * Copy the BL-Data struct passed as input to a specific flash page.
 *
 * @param[in] A pointer to the BL-Data to be saved on flash.
 * @patam[in] The flash page where to save the BL-Data.
 */
static void bl_data_copy(const bl_data_t *data, int bl_page)
{
	qm_flash_page_write(BL_DATA_FLASH_CONTROLLER, BL_DATA_FLASH_REGION,
			    bl_page, (uint32_t *)data,
			    sizeof(bl_data_t) / sizeof(uint32_t));
}

/**
 * Sanitize application flash partitions.
 *
 * Check and fix inconsistent partitions. Fixing consists in erasing the first
 * page of the partition to ensure that it will not be booted.
 *
 * @return Whether or not a writeback of bl-data is needed.
 * @retval false Writeback not needed (no partition has been fixed and bl-data
 * 		 has not been updated).
 * @retval true  Writeback needed (at least one partition has been fixed and
 *		  therefore bl-data has been updated).
 */
static bool bl_data_sanitize_partitions()
{
	int wb_needed;
	int i;

	wb_needed = false;
	bl_flash_partition_t *part;

	for (i = 0; i < BL_FLASH_PARTITIONS_NUM; i++) {
		part = &bl_data->partitions[i];
		if (part->is_consistent == false) {
			qm_flash_page_erase(part->controller,
					    QM_FLASH_REGION_SYS,
					    part->first_page);
			part->is_consistent = true;
			wb_needed = true;
		}
	}

	return wb_needed;
}

/*
 * Check the validity of BL-Data and fix/init it if necessary.
 *
 * The logic of this functions is defined in conjunction with the firmware
 * image update logic (see QFU module).
 */
int bl_data_sanitize()
{
	uint32_t crc_main;
	uint32_t crc_bck;

	crc_main = (uint16_t)fm_crc16_ccitt((uint8_t *)bl_data_main,
					    offsetof(bl_data_t, crc));
	crc_bck = (uint16_t)fm_crc16_ccitt((uint8_t *)bl_data_bck,
					   offsetof(bl_data_t, crc));

	if (bl_data_main->crc != crc_main) {
		if (bl_data_bck->crc != crc_bck) {
			/*
			 * Both BL-Data Main and BL-Data Backup are invalid.
			 * This is expected when the BL-Data flash section has
			 * not been initialized yet.
			 *
			 * Perform initial device provisioning: initialize the
			 * BL-Data regions of flash.
			 */
			bl_data_init();
		} else {
			/*
			 * BL-Main is corrupted. This is expected when a
			 * previous firmware image update failed.
			 *
			 * Restore BL-Data Main by copying the content of
			 * BL-Data Backup into it.
			 */
			bl_data_copy(bl_data_bck, BL_DATA_SECTION_MAIN_PAGE);
		}
	} else {
		if (crc_main != crc_bck) {
			/*
			 * BL-Data Main is valid and up to date, but BL-Data
			 * Backup has a different checksum than BL-Data Main.
			 * This means that BL-Data Backup is either corrupted
			 * (expected when the previous firmware update
			 * has failed while updating BL-Data backup) or
			 * outdated (expected when the previous firmware
			 * update has failed after updating BL-Data Main, but
			 * before beginning to update BL-Data Backup).
			 *
			 * Restore BL-Data Backup with the content of BL-Data
			 * Main.
			 */
			bl_data_copy(bl_data_main, BL_DATA_SECTION_BACKUP_PAGE);
		}
	}
	/*
	 * Update the shadowed BL-Data in RAM with the content of BL-Data
	 * Main
	 */
	memcpy(bl_data, bl_data_main, sizeof(*bl_data));
	/*
	 * NOTE: if any partition is sanitized, shadowed bl-data is updated and
	 * need to be written back.
	 * Consider restructuring the code in this function to reduce the
	 * number of bl-data writes (the restructuring will probably increase
	 * footprint).
	 */
	if (bl_data_sanitize_partitions()) {
		bl_data_shadow_writeback();
	}

	return 0;
}

/* Store BL-Data to flash */
int bl_data_shadow_writeback()
{
	bl_data->crc = (uint16_t)fm_crc16_ccitt((uint8_t *)bl_data,
						offsetof(bl_data_t, crc));
	bl_data_copy(bl_data, BL_DATA_SECTION_MAIN_PAGE);
	bl_data_copy(bl_data, BL_DATA_SECTION_BACKUP_PAGE);

	return 0;
}
