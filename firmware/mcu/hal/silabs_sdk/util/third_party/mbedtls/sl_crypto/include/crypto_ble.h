/***************************************************************************//**
 * @file
 * @brief BLE-specific cipher implementations optimized for Silicon Labs devices
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
#ifndef CRYPTO_BLE_H
#define CRYPTO_BLE_H

/***************************************************************************//**
 * \addtogroup sl_crypto_internal
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_crypto_ble Accelerated Bluetooth Primitives
 * \brief Accelerated Bluetooth cryptographic primitives using the CRYPTO peripheral
 *
 * \{
 ******************************************************************************/

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "mbedtls/ccm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          CCM buffer encryption optimized for BLE
 *
 * \param data     Input/output buffer of payload data of BLE packet
 * \param length   length of input data
 * \param iv       nonce (initialization vector)
 *                 must be 13 bytes
 * \param header   header of BLE packet (1 byte)
 * \param tag      buffer where the BLE packet tag (4 bytes) will be written
 *
 * \return         0 if successful
 */
int mbedtls_ccm_encrypt_and_tag_ble( unsigned char       *data,
                                     size_t               length,
                                     const unsigned char *key,
                                     const unsigned char *iv,
                                     unsigned char        header,
                                     unsigned char       *tag );

/**
 * \brief          CCM buffer authenticated decryption optimized for BLE
 *
 * \param data     Input/output buffer of payload data of BLE packet
 * \param length   length of input data
 * \param iv       nonce (initialization vector)
 *                 must be 13 bytes
 * \param header   header of BLE packet (1 byte)
 * \param tag      authentication tag of BLE packet (4 bytes)
 *
 * \return         0 if successful and authenticated,
 *                 MBEDTLS_ERR_CCM_AUTH_FAILED if tag does not match payload
 */
int mbedtls_ccm_auth_decrypt_ble( unsigned char       *data,
                                  size_t               length,
                                  const unsigned char *key,
                                  const unsigned char *iv,
                                  unsigned char        header,
                                  unsigned char       *tag );

/**
 * \brief          Process a table of BLE RPA device keys and look for a
 *                 match against the supplied hash
 *
 * \param keytable Pointer to an array of AES-128 keys, corresponding to the
 *                 per-device key in the BLE RPA process
 * \param keymask  Bitmask indicating with key indices in keytable are valid
 * \param prand    24-bit BLE nonce to encrypt with each key and match against hash
 * \param hash     BLE RPA hash to match against (last 24 bits of AES result)
 *
 * \return         0-based index of matching key if a match is found, -1 for no match.
 */
int mbedtls_process_ble_rpa(  const unsigned char   keytable[],
                              uint32_t              keymask,
                              uint32_t              prand,
                              uint32_t              hash );

#ifdef __cplusplus
}
#endif

/** \} (end addtogroup sl_crypto_ble) */
/** \} (end addtogroup sl_crypto) */

#endif /* CRYPTO_BLE_H */
