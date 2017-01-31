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

#ifndef __DFU_CONFIG_H__
#define __DFU_CONFIG_H__

#include "../bl_data.h"

/**
 * DFU configuration defines.
 */

/* These are exposed in Device Descriptors. */
/* Vendor ID. */
#define DFU_CFG_VID (0xBEEF)

/* Product ID in run-time. */
#define DFU_CFG_PID (0x00DA)

/* Product ID in DFU mode. */
#define DFU_CFG_PID_DFU (0xFFDA)

/* Device release number (as a BCD). */
#define DFU_CFG_DEV_BCD (0x0100)


/* These are exposed in DFU Descriptors. */
/* Detach timeout. */
#define DFU_CFG_DETACH_TIMEOUT (0xFFFF)

/* Maximum supported block size. */
#define DFU_CFG_MAX_BLOCK_SIZE (2048)

/* DFU Version (as BCD). */
#define DFU_CFG_DFU_VERSION_BCD (0x0101)

/**
 * DFU attributes (bitfield).
 *
 * DFU bmAttributes (bitfield): 0x07 (bitWillDetach = 0,
 * bitManifestationTollerant = 1, bitCanUpload = 1, bitCanDnload = 1)
 */
#define DFU_CFG_DFU_ATTRIBUTES (0x07)

/**
 * Number of alternate settings.
 *
 * Number of partitions + QFM alternate setting (i.e., alt setting 0).
 */
#define DFU_CFG_NUM_ALT_SETTINGS (1 + BL_FLASH_PARTITIONS_NUM)

#endif /* __DFU_CONFIG_H__ */
