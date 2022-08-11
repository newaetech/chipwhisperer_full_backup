/**
 * \file
 *
 * \brief Embedded Flash Controller (EFC) driver for SAM.
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

#ifndef EFC_H_INCLUDED
#define EFC_H_INCLUDED

#include "compiler.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/*! \name EFC return codes */
//! @{
typedef enum efc_rc {
	EFC_RC_OK = 0,      //!< Operation OK
	EFC_RC_YES = 0,     //!< Yes
	EFC_RC_NO = 1,      //!< No
	EFC_RC_ERROR = 1,   //!< General error
	EFC_RC_INVALID,     //!< Invalid argument input
	EFC_RC_NOT_SUPPORT = 0xFFFFFFFF //!< Operation is not supported
} efc_rc_t;
//! @}

/*! \name EFC command */
//! @{
#define EFC_FCMD_GETD    0x00  //!< Get Flash Descriptor
#define EFC_FCMD_WP      0x01  //!< Write page
#define EFC_FCMD_WPL     0x02  //!< Write page and lock
#define EFC_FCMD_EWP     0x03  //!< Erase page and write page
#define EFC_FCMD_EWPL    0x04  //!< Erase page and write page then lock
#define EFC_FCMD_EA      0x05  //!< Erase all
#if (SAM3S8 || SAM3SD8)
#define EFC_FCMD_EPL     0x06  //!< Erase plane
#endif
#if (SAM4S || SAM4E || SAM4N || SAM4C || SAMG || SAM4CP || SAM4CM || \
	 SAMV71 || SAMV70 || SAMS70 || SAME70)
#define EFC_FCMD_EPA     0x07  //!< Erase pages
#endif
#define EFC_FCMD_SLB     0x08  //!< Set Lock Bit
#define EFC_FCMD_CLB     0x09  //!< Clear Lock Bit
#define EFC_FCMD_GLB     0x0A  //!< Get Lock Bit
#define EFC_FCMD_SGPB    0x0B  //!< Set GPNVM Bit
#define EFC_FCMD_CGPB    0x0C  //!< Clear GPNVM Bit
#define EFC_FCMD_GGPB    0x0D  //!< Get GPNVM Bit
#define EFC_FCMD_STUI    0x0E  //!< Start unique ID
#define EFC_FCMD_SPUI    0x0F  //!< Stop unique ID
#if (!SAM3U && !SAM3SD8 && !SAM3S8)
#define EFC_FCMD_GCALB   0x10  //!< Get CALIB Bit
#endif
#if (SAM4S || SAM4E || SAM4N || SAM4C || SAMG || SAM4CP || SAM4CM || \
	 SAMV71 || SAMV70 || SAMS70 || SAME70)
#define EFC_FCMD_ES      0x11  //!< Erase sector
#define EFC_FCMD_WUS     0x12  //!< Write user signature
#define EFC_FCMD_EUS     0x13  //!< Erase user signature
#define EFC_FCMD_STUS    0x14  //!< Start read user signature
#define EFC_FCMD_SPUS    0x15  //!< Stop read user signature
#endif
//! @}

/*! The IAP function entry address */
#define CHIP_FLASH_IAP_ADDRESS  (IROM_ADDR + 8)

/*! \name EFC access mode */
//! @{
#define EFC_ACCESS_MODE_128    0
#define EFC_ACCESS_MODE_64     EEFC_FMR_FAM
//! @}

uint32_t efc_init(Efc *p_efc, uint32_t ul_access_mode, uint32_t ul_fws);
#if (SAM4S || SAM4E || SAM4N || SAM4C || SAMG || SAM4CP || SAM4CM || \
	 SAMV71 || SAMV70 || SAMS70 || SAME70)
void efc_enable_cloe(Efc *p_efc);
void efc_disable_cloe(Efc *p_efc);
#endif
void efc_enable_frdy_interrupt(Efc *p_efc);
void efc_disable_frdy_interrupt(Efc *p_efc);
#if (SAMV71 || SAMV70 || SAMS70 || SAME70)
void efc_enable_write_protection(Efc *p_efc);
void efc_disable_write_protection(Efc *p_efc);
#else
void efc_set_flash_access_mode(Efc *p_efc, uint32_t ul_mode);
uint32_t efc_get_flash_access_mode(Efc *p_efc);
#endif
void efc_set_wait_state(Efc *p_efc, uint32_t ul_fws);
uint32_t efc_get_wait_state(Efc *p_efc);
uint32_t efc_perform_command(Efc *p_efc, uint32_t ul_command,
		uint32_t ul_argument);
uint32_t efc_get_status(Efc *p_efc);
uint32_t efc_get_result(Efc *p_efc);
uint32_t efc_perform_read_sequence(Efc *p_efc,
		uint32_t ul_cmd_st, uint32_t ul_cmd_sp,
		uint32_t *p_ul_buf, uint32_t ul_size);

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

#endif /* EFC_H_INCLUDED */
