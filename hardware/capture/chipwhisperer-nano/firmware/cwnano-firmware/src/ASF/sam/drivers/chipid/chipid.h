/**
 * \file
 *
 * \brief Chip Identifier (CHIPID) driver for SAM.
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef CHIPID_H_INCLUDED
#define CHIPID_H_INCLUDED

#include "compiler.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

//! Definition for chip id register data struct
typedef struct chipid_data {

	//! Version of the device
	uint32_t ul_version;
	//! Embedded processor
	uint32_t ul_eproc;
	//! Non-volatile program memory size
	uint32_t ul_nvpsiz;
	//! Second non-volatile program memory size
	uint32_t ul_nvpsiz2;
	//! Internal SRAM size
	uint32_t ul_sramsiz;
	//! Architecture identifier
	uint32_t ul_arch;
	//! Non-volatile program memory type
	uint32_t ul_nvptyp;
	//! Extension flag
	uint32_t ul_extflag;
	//! Chip ID extension
	uint32_t ul_extid;
} chipid_data_t;

uint32_t chipid_read(Chipid *p_chipid, chipid_data_t *p_chipid_data);
uint32_t chipid_read_version(Chipid *p_chipid);
uint32_t chipid_read_processor(Chipid *p_chipid);
uint32_t chipid_read_arch(Chipid *p_chipid);
uint32_t chipid_read_sramsize(Chipid *p_chipid);
uint32_t chipid_read_nvpmsize(Chipid *p_chipid);
uint32_t chipid_read_nvpm2size(Chipid *p_chipid);
uint32_t chipid_read_nvpmtype(Chipid *p_chipid);
uint32_t chipid_read_extchipid(Chipid *p_chipid);

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

#endif /* CHIPID_H_INCLUDED */
