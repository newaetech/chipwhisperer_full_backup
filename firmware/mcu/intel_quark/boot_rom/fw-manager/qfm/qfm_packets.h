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

#ifndef __QFM_PACKETS_H__
#define __QFM_PACKETS_H__

#include <stdint.h>

#include "bl_data.h"

/*
 * NOTE: different compilers use different directives for packing structs;
 * therefore we define a macro whose value will depend on the specific compiler
 * used.
 * For now, we set it to the GCC directive.
 */
#define __QM_ATTR_PACKED__ __attribute__((__packed__))

/**
 * The enumeration of QFM packet types.
 */
typedef enum {
	/* Requests */
	QFM_SYS_INFO_REQ = 0x444D0000, /**< System Information request. */
	QFM_APP_ERASE = 0x444D0001,    /**< Application Erase request. */
	QFM_UPDATE_KEY = 0x444D0002,   /**< Crypto Key Update request. */
	/* Responses */
	QFM_SYS_INFO_RSP = 0x444D8000, /**< System Information response. */
} qfm_pkt_type_t;

/**
 * The enumeration of SoC types.
 */
typedef enum {
	QFM_SOC_TYPE_QUARK_D2000 = 0, /**< Quark D2000 SoC. */
	QFM_SOC_TYPE_QUARK_SE = 1,    /**< Quark SE SoC. */
} qfm_soc_type_t;

/**
 * The enumeration of Target types.
 */
typedef enum {
	QFM_TARGET_TYPE_X86 = 0,    /**< x86 target. */
	QFM_TARGET_TYPE_SENSOR = 1, /**< Sensor target. */
} qfm_target_type_t;

/**
 * The generic structure of a QFM packet.
 */
typedef struct __QM_ATTR_PACKED__ {
	uint32_t type;     /**< The packet type, see qfm_pkt_type_t enum. */
	uint8_t payload[]; /**< The type-specific payload/structure. */
} qfm_generic_pkt_t;

typedef struct __QM_ATTR_PACKED__ {
	uint8_t app_present;
	uint32_t app_version;
} qfm_partition_dsc_t;

typedef struct __QM_ATTR_PACKED__ {
	const uint8_t target_type;
	uint8_t active_partition_idx;
} qfm_target_dsc_t;

/**
 * Type-specific structure for the QFM System Information response packet.
 */
typedef struct __QM_ATTR_PACKED__ {
	const uint32_t qfm_pkt_type;
	const uint32_t sysupd_version; /**< The bootloader version. */
	const uint8_t soc_type;	/**< The SOC type. */
	const uint8_t auth_type;       /**< The active authentication type. */
	const uint8_t target_count;    /**< Number of boot targets. */
	const uint8_t partition_count; /**< Number of boot partitions. */

	/** List of target descriptors. */
	qfm_target_dsc_t targets[BL_BOOT_TARGETS_NUM];
	/** List of partition descriptors. */
	qfm_partition_dsc_t partitions[BL_FLASH_PARTITIONS_NUM];
} qfm_sys_info_rsp_t;

#endif /* __QFM_PACKETS_H__ */
