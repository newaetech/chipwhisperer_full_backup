/***************************************************************************//**
 * @file
 * @brief Silicon Labs CRYPTOACC device management interface.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: APACHE-2.0
 *
 * This software is subject to an open source license and is distributed by
 * Silicon Laboratories Inc. pursuant to the terms of the Apache License,
 * Version 2.0 available at https://www.apache.org/licenses/LICENSE-2.0.
 * Such terms and conditions may be further supplemented by the Silicon Labs
 * Master Software License Agreement (MSLA) available at www.silabs.com and its
 * sections applicable to open source software.
 *
 ******************************************************************************/

#ifndef CRYPTOACC_MANAGEMENT_H
#define CRYPTOACC_MANAGEMENT_H

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

/***************************************************************************//**
 * \addtogroup sl_crypto
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_cryptoacc_management CRYPTOACC device instance management
 * \brief Management functions for the CRYPTOACC. These functions take care
 *        of not having two 'owners' simultaneously for the same CRYPTOACC
 *        device, which could potentially be causing conflicts and system
 *        lock-up.
 * \{
 ******************************************************************************/

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <stdlib.h>

#include "em_device.h"

#if defined(CRYPTOACC_PRESENT)

#include "mbedtls_ecode.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MBEDTLS_ERR_CRYPTOACC_TRNG_FAILED ((int)MBEDTLS_ERR_CRYPTOACC_BASE | 0x00000001)

/**
 * \brief		   Get ownership of the crypto device
 *
 * \return         0 if successful, negative on error
 */
int cryptoacc_management_acquire( void );

/**
 * \brief          Release ownership of the crypto device
 *
 * \return         0 if successful, negative on error
 */
int cryptoacc_management_release( void );

/**
 * \brief          TRNG initialization
 *
 * \return         0 if successful, MBEDTLS_ERR_CRYPTOACC_TRNG_FAILED on error
 */
int cryptoacc_trng_initialize( void );

#ifdef __cplusplus
}
#endif

#endif /* CRYPTOACC_PRESENT */

/** \} (end addtogroup sl_cryptoacc_management) */
/** \} (end addtogroup sl_crypto) */

#endif /* CRYPTOACC_MANAGEMENT_H */
