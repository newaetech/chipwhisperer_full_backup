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

#include "chipid.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \defgroup sam_drivers_chipid_group Chip Identifier (CHIPID)
 *
 * \par Purpose
 *
 * Driver for the Chip Identifier. This driver provides access to the main 
 * features of the CHIPID.
 *
 * \par Usage
 *
 * -# Read whole Chip ID information to a struct using \ref chipid_read().
 *    The read data is defined using \ref chipid_data_t.
 * -# Read Chip ID fields using following functions:
 *    - Device version: \ref chipid_read_version();
 *    - Embedded processor: \ref chipid_read_processor();
 *    - Non volatile program memory size: \ref chipid_read_sramsize();
 *    - Second non volatile program memory size: \ref chipid_read_nvpm2size();
 *    - Internal SRAM size: \ref chipid_read_sramsize();
 *    - Architecture Identifier: \ref chipid_read_arch();
 *    - Non volatile program memory type: \ref chipid_read_nvpmtype();
 *    - Extension flag: \ref chipid_read_extchipid().
 *
 * \section dependencies Dependencies
 * This driver does not depend on other modules.
 *
 * @{
 */

/**
 * \brief Get chip identifier information.
 *
 * \param p_chipid Pointer to a CHIPID instance.
 * \param p_chipid_data Pointer to a data structure to store chip information.
 *
 * \return 0 on success.
 */
uint32_t chipid_read(Chipid *p_chipid, chipid_data_t *p_chipid_data)
{
	if (NULL == p_chipid_data) {
		return 0xFFFFFFFF;
	}
	
	p_chipid_data->ul_version =
		(p_chipid->CHIPID_CIDR & CHIPID_CIDR_VERSION_Msk) >>
			CHIPID_CIDR_VERSION_Pos;
	p_chipid_data->ul_eproc =
		(p_chipid->CHIPID_CIDR & CHIPID_CIDR_EPROC_Msk) >>
			CHIPID_CIDR_EPROC_Pos;
	p_chipid_data->ul_nvpsiz =
		(p_chipid->CHIPID_CIDR & CHIPID_CIDR_NVPSIZ_Msk) >>
			CHIPID_CIDR_NVPSIZ_Pos;
	p_chipid_data->ul_nvpsiz2 =
		(p_chipid->CHIPID_CIDR & CHIPID_CIDR_NVPSIZ2_Pos) >>
			CHIPID_CIDR_NVPSIZ2_Pos;
	p_chipid_data->ul_sramsiz =
		(p_chipid->CHIPID_CIDR & CHIPID_CIDR_SRAMSIZ_Msk) >>
			CHIPID_CIDR_SRAMSIZ_Pos;
	p_chipid_data->ul_arch =
		(p_chipid->CHIPID_CIDR & CHIPID_CIDR_ARCH_Msk) >>
			CHIPID_CIDR_ARCH_Pos;
	p_chipid_data->ul_nvptyp =
		(p_chipid->CHIPID_CIDR & CHIPID_CIDR_NVPTYP_Msk) >>
			CHIPID_CIDR_NVPTYP_Pos;
	p_chipid_data->ul_extflag =
		(p_chipid->CHIPID_CIDR & CHIPID_CIDR_EXT) >> 31;
		
	if (p_chipid_data->ul_extflag) {
		p_chipid_data->ul_extid =
			p_chipid->CHIPID_EXID & CHIPID_EXID_EXID_Msk;
	}
	return 0;
}

/**
 * \brief Get the revision number of the silicon.
 *
 * \param p_chipid Pointer to a CHIPID instance.
 *
 * \return Revision number of the silicon.
 */
uint32_t chipid_read_version(Chipid *p_chipid)
{
	return p_chipid->CHIPID_CIDR & CHIPID_CIDR_VERSION_Msk;
}

/**
 * \brief Get the version of the embedded ARM processor.
 *
 * \param p_chipid Pointer to a CHIPID instance.
 *
 * \return Version of the embedded ARM processor.
 */
uint32_t chipid_read_processor(Chipid *p_chipid)
{
	return p_chipid->CHIPID_CIDR & CHIPID_CIDR_EPROC_Msk;
}

/**
 * \brief Get the identifier of the architecture.
 *
 * \param p_chipid Pointer to a CHIPID instance.
 *
 * \return Identifier of the architecture.
 */
uint32_t chipid_read_arch(Chipid *p_chipid)
{
	return p_chipid->CHIPID_CIDR & CHIPID_CIDR_ARCH_Msk;
}

/**
 * \brief Get the size of the embedded SRAM.
 *
 * \param p_chipid Pointer to a CHIPID instance.
 *
 * \return SRAM size value.
 */
uint32_t chipid_read_sramsize(Chipid *p_chipid)
{
	return p_chipid->CHIPID_CIDR & CHIPID_CIDR_SRAMSIZ_Msk;
}

/**
 * \brief Get the size of the first non-volatile program memory.
 *
 * \param p_chipid Pointer to a CHIPID instance.
 *
 * \return Memory size value.
 */
uint32_t chipid_read_nvpmsize(Chipid *p_chipid)
{
	return p_chipid->CHIPID_CIDR & CHIPID_CIDR_NVPSIZ_Msk;
}

/**
 * \brief Get the size of the second non-volatile program memory.
 *
 * \param p_chipid Pointer to a CHIPID instance.
 *
 * \return Memory size value.
 */
uint32_t chipid_read_nvpm2size(Chipid *p_chipid)
{
	return p_chipid->CHIPID_CIDR & CHIPID_CIDR_NVPSIZ2_Pos;
}

/**
 * \brief Get the type of non-volatile program memory.
 *
 * \param p_chipid Pointer to a CHIPID instance.
 *
 * \return Memory type.
 */
uint32_t chipid_read_nvpmtype(Chipid *p_chipid)
{
	return p_chipid->CHIPID_CIDR & CHIPID_CIDR_NVPTYP_Msk;
}

/**
 * \brief Get the chip extension identifier.
 *
 * \param p_chipid Pointer to a CHIPID instance.
 *
 * \return Chip extension identifier if available, else 0.
 */
uint32_t chipid_read_extchipid(Chipid *p_chipid)
{
	if ((p_chipid->CHIPID_CIDR) & CHIPID_CIDR_EXT)
		return p_chipid->CHIPID_EXID & CHIPID_EXID_EXID_Msk;
	else
		return 0;
}

//@}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
