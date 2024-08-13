/**
 * \file
 *
 * \brief Matrix driver for SAM.
 *
 * Copyright (c) 2012-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

#include "compiler.h"

/* / @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* / @endcond */

#ifndef MATRIX_MCFG_ULBT
#define MATRIX_MCFG_ULBT(value)    MATRIX_MCFG0_ULBT(value)
#endif
/** \brief Matrix master: undefined length burst type */
typedef enum {
	MATRIX_ULBT_INFINITE_LENGTH_BURST = MATRIX_MCFG_ULBT(0),
	MATRIX_ULBT_SINGLE_ACCESS         = MATRIX_MCFG_ULBT(1),
	MATRIX_ULBT_4_BEAT_BURST          = MATRIX_MCFG_ULBT(2),
	MATRIX_ULBT_8_BEAT_BURST          = MATRIX_MCFG_ULBT(3),
	MATRIX_ULBT_16_BEAT_BURST         = MATRIX_MCFG_ULBT(4),
#if SAM4C || SAM4CP || SAM4CM || SAM4E || SAMV71 || SAMV70 || SAME70 || SAMS70
	MATRIX_ULBT_32_BEAT_BURST         = MATRIX_MCFG_ULBT(5),
	MATRIX_ULBT_64_BEAT_BURST         = MATRIX_MCFG_ULBT(6),
	MATRIX_ULBT_128_BEAT_BURST        = MATRIX_MCFG_ULBT(7),
#endif
} burst_type_t;

/** \brief Matrix slave: default master type */
typedef enum {
	MATRIX_DEFMSTR_NO_DEFAULT_MASTER    = MATRIX_SCFG_DEFMSTR_TYPE(0),
	MATRIX_DEFMSTR_LAST_DEFAULT_MASTER  = MATRIX_SCFG_DEFMSTR_TYPE(1),
	MATRIX_DEFMSTR_FIXED_DEFAULT_MASTER = MATRIX_SCFG_DEFMSTR_TYPE(2)
} defaut_master_t;

#if !SAM4E && !SAM4C && !SAM4CP && !SAM4CM && \
	 !SAMV71 && !SAMV70 && !SAMS70 && !SAME70
/** \brief Matrix slave: arbitration type */
typedef enum {
	MATRIX_ARBT_ROUND_ROBIN    = MATRIX_SCFG_ARBT(0),
	MATRIX_ARBT_FIXED_PRIORITY = MATRIX_SCFG_ARBT(1)
} arbitration_type_t;
#endif

void matrix_set_master_burst_type(uint32_t ul_id, burst_type_t burst_type);
burst_type_t matrix_get_master_burst_type(uint32_t ul_id);
void matrix_set_slave_slot_cycle(uint32_t ul_id, uint32_t ul_slot_cycle);
uint32_t matrix_get_slave_slot_cycle(uint32_t ul_id);
void matrix_set_slave_default_master_type(uint32_t ul_id, defaut_master_t type);
defaut_master_t matrix_get_slave_default_master_type(uint32_t ul_id);
void matrix_set_slave_fixed_default_master(uint32_t ul_id,
		uint32_t ul_fixed_id);
uint32_t matrix_get_slave_fixed_default_master(uint32_t ul_id);

#if !SAM4E && !SAM4C && !SAM4CP && !SAM4CM && \
	 !SAMV71 && !SAMV70 && !SAMS70 && !SAME70
void matrix_set_slave_arbitration_type(uint32_t ul_id, arbitration_type_t type);
arbitration_type_t matrix_get_slave_arbitration_type(uint32_t ul_id);
#endif

void matrix_set_slave_priority(uint32_t ul_id, uint32_t ul_prio);
uint32_t matrix_get_slave_priority(uint32_t ul_id);
#if (SAMV71 || SAMV70|| SAME70 || SAMS70)
void matrix_set_slave_priority_b(uint32_t ul_id, uint32_t ul_prio_b);
uint32_t matrix_get_slave_priority_b(uint32_t ul_id);
#endif

#if (SAM3XA || SAM3U || SAM4E || \
	 SAMV71 || SAMV70 || SAMS70 || SAME70)
void matrix_set_master_remap(uint32_t ul_remap);
uint32_t matrix_get_master_remap(void);

#endif

#if (SAM3S || SAM3XA || SAM3N || SAM4S || SAM4E || SAM4N || SAM4C || SAMG || SAM4CP || SAM4CM || \
	 SAMV71 || SAMV70 || SAMS70 || SAME70)
void matrix_set_system_io(uint32_t ul_io);
uint32_t matrix_get_system_io(void);

#endif

#if (SAM3S || SAM4S || SAM4E || SAM4C || SAM4CP || SAM4CM || \
	 SAMV71 || SAMV70 || SAMS70 || SAME70)
void matrix_set_nandflash_cs(uint32_t ul_cs);
uint32_t matrix_get_nandflash_cs(void);
#endif

#if (!SAMG)
void matrix_set_writeprotect(uint32_t ul_enable);
uint32_t matrix_get_writeprotect_status(void);
#endif

#if SAMG55
void matrix_set_usb_device(void);
void matrix_set_usb_host(void);
#endif

#if (SAMV71 || SAMV70|| SAME70)
void matrix_set_can0_addr(uint32_t base_addr);
void matrix_set_can1_addr(uint32_t base_addr);
#endif
/* / @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* / @endcond */

#endif /* MATRIX_H_INCLUDED */
