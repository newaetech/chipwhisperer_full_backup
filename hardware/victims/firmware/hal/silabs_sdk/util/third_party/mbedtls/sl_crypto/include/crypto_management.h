/***************************************************************************//**
 * @file
 * @brief Silicon Labs CRYPTO device management interface.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef CRYPTO_MANAGEMENT_H
#define CRYPTO_MANAGEMENT_H

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

/***************************************************************************//**
 * \addtogroup sl_crypto
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_crypto_management Peripheral Instance Management: CRYPTO
 * \brief Resource management functions for the CRYPTO peripheral
 *
 * \{
 ******************************************************************************/

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <stdlib.h>

#include "em_device.h"

#if defined( CRYPTO_PRESENT )
#include "em_crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Save DDATA0 register when preempting */
#define CRYPTO_MANAGEMENT_SAVE_DDATA0 (0x1U << 3)
/** Save DDATA1 register when preempting */
#define CRYPTO_MANAGEMENT_SAVE_DDATA1 (0x1U << 4)
/** Save DDATA2 register when preempting */
#define CRYPTO_MANAGEMENT_SAVE_DDATA2 (0x1U << 5)
/** Save DDATA3 register when preempting */
#define CRYPTO_MANAGEMENT_SAVE_DDATA3 (0x1U << 6)
/** Save DDATA4 register when preempting */
#define CRYPTO_MANAGEMENT_SAVE_DDATA4 (0x1U << 7)
/** Save SEQ0 register when preempting */
#define CRYPTO_MANAGEMENT_SAVE_UPTO_SEQ0 (0x1U)
/** Save SEQ0 through SEQ1 register when preempting */
#define CRYPTO_MANAGEMENT_SAVE_UPTO_SEQ1 (0x2U)
/** Save SEQ0 through SEQ2 register when preempting */
#define CRYPTO_MANAGEMENT_SAVE_UPTO_SEQ2 (0x3U)
/** Save SEQ0 through SEQ3 register when preempting */
#define CRYPTO_MANAGEMENT_SAVE_UPTO_SEQ3 (0x4U)
/** Save SEQ0 through SEQ4 register when preempting */
#define CRYPTO_MANAGEMENT_SAVE_UPTO_SEQ4 (0x5U)

/**
 * \brief		   Get ownership of a CRYPTO peripheral
 *
 * \return         Handle of assigned CRYPTO peripheral
 */
CRYPTO_TypeDef *crypto_management_acquire( void );

/**
 * \brief		   Get ownership of the default CRYPTO peripheral
 *
 * \return         Handle of default CRYPTO peripheral
 */
CRYPTO_TypeDef *crypto_management_acquire_default( void );

/**
 * \brief          Release ownership of a CRYPTO peripheral
 *
 * \param device   Handle of CRYPTO peripheral to be released
 */
void crypto_management_release( CRYPTO_TypeDef *device );

/**
 * \brief          Acquire preempting ownership of a CRYPTO peripheral.
 *                 NOTE: this function is not meant for general use, it
 *                 is not thread-safe, and must be called form the
 *                 highest priority thread/interrupt allowed to use mbed TLS.
 *
 * \param regmask  Bitmask of CRYPTO_MANAGEMENT_ defines instructing what
 *                 parts of the device state will be clobbered during
 *                 preemption.
 *
 * \return         Handle of assigned CRYPTO peripheral
 */
CRYPTO_TypeDef *crypto_management_acquire_preemption( uint32_t regmask );

/**
 * \brief          Releasing preempting ownership of a CRYPTO peripheral.
 *                 NOTE: this function is not meant for general use, it
 *                 is not thread-safe, and must be called form the
 *                 highest priority thread/interrupt allowed to use mbed TLS.
 *
 * \param device   Handle of preempted CRYPTO peripheral to be released
 */
void crypto_management_release_preemption( CRYPTO_TypeDef *device );

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_PRESENT */

/** \} (end addtogroup sl_crypto_management) */
/** \} (end addtogroup sl_crypto) */

#endif /* CRYPTO_MANAGEMENT_H */
