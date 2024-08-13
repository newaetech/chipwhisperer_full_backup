/**************************************************************************//**
 * @file
 * @brief EFR32MG21 LVGD register and bit field definitions
 ******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories, Inc. www.silabs.com</b>
 ******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *****************************************************************************/
#ifndef EFR32MG21_LVGD_H
#define EFR32MG21_LVGD_H
#define LVGD_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32MG21_LVGD LVGD
 * @{
 * @brief EFR32MG21 LVGD Register Declaration.
 *****************************************************************************/

/** LVGD Register Declaration. */
typedef struct {
  uint32_t RESERVED0[1U];                       /**< Reserved for future use                            */
  uint32_t RESERVED1[1023U];                    /**< Reserved for future use                            */
  uint32_t RESERVED2[1U];                       /**< Reserved for future use                            */
  uint32_t RESERVED3[1023U];                    /**< Reserved for future use                            */
  uint32_t RESERVED4[1U];                       /**< Reserved for future use                            */
  uint32_t RESERVED5[1023U];                    /**< Reserved for future use                            */
  uint32_t RESERVED6[1U];                       /**< Reserved for future use                            */
} LVGD_TypeDef;
/** @} End of group EFR32MG21_LVGD */

/**************************************************************************//**
 * @addtogroup EFR32MG21_LVGD
 * @{
 * @defgroup EFR32MG21_LVGD_BitFields LVGD Bit Fields
 * @{
 *****************************************************************************/

/** @} End of group EFR32MG21_LVGD_BitFields */
/** @} End of group EFR32MG21_LVGD */
/** @} End of group Parts */
#endif /* EFR32MG21_LVGD_H */
