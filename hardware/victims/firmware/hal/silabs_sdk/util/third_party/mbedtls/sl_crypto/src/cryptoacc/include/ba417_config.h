/**
 * @file
 * @brief Defines the procedures to make operations with
 *          the BA417 ChaCha20 Poly1305
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#ifndef BA417_CONFIG_H
#define BA417_CONFIG_H

#include <stdint.h>


struct ba417_regs_s {
   volatile uint32_t config;
   volatile uint8_t  key[32];
   volatile uint8_t  dummy[4];
   volatile uint8_t  iv[4];
   volatile uint8_t  nonce[12];
};


/** @brief BA417 Config Register value for ChaCha20 + Poly1305 mode of operation */
#define BA417_CONFIG_CHACHA_POLY  0x00000000
/** @brief BA417 Config Register value for ChaCha20 mode of operation */
#define BA417_CONFIG_CHACHA_ONLY  0x00000001
/** @brief BA417 Config Register value for Poly1305 (internal key) mode of operation */
#define BA417_CONFIG_POLY_CHACHAKEY  0x00000002
/** @brief BA417 Config Register value for Poly1305 (concatenated key) mode of operation */
#define BA417_CONFIG_POLY_ONLY  0x00000003
/** @brief BA411E Config Register value for encryption mode */
#define BA417_CONFIG_ENCRYPT 0x00000000
/** @brief BA411E Config Register value for decryption mode */
#define BA417_CONFIG_DECRYPT 0x00000004

#endif
