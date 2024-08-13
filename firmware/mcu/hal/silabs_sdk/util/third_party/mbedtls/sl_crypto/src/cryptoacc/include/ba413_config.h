/**
 * @file
 * @brief Defines the procedures to make operations with
 *          the BA413 hash function
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#ifndef BA413_CONFIG_H
#define BA413_CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include "sx_hw_cfg.h"

struct ba413_regs_s {
   volatile uint32_t config;
};


/** @brief Value for ::ba413_regs_s.config, set mode to MD5 */
#define BA413_CONF_MODE_MD5      0x00000001
/** @brief Value for ::ba413_regs_s.config, set mode to SHA1 */
#define BA413_CONF_MODE_SHA1     0x00000002
/** @brief Value for ::ba413_regs_s.config, set mode to SHA224 */
#define BA413_CONF_MODE_SHA224   0x00000004
/** @brief Value for ::ba413_regs_s.config, set mode to SHA256 */
#define BA413_CONF_MODE_SHA256   0x00000008
/** @brief Value for ::ba413_regs_s.config, set mode to SHA384 */
#define BA413_CONF_MODE_SHA384   0X00000010
/** @brief Value for ::ba413_regs_s.config, set mode to SHA512 */
#define BA413_CONF_MODE_SHA512   0x00000020
/** @brief Value for ::ba413_regs_s.config, set mode to SM3 */
#define BA413_CONF_MODE_SM3      0x00000040

/** @brief Value for ::ba413_regs_s.config, enable HMAC */
#define BA413_CONF_HMAC    0x00000100
/** @brief Value for ::ba413_regs_s.config, enable padding in hardware  */
#define BA413_CONF_HWPAD   0x00000200
/** @brief Value for ::ba413_regs_s.config, set as final (return digest and not state) */
#define BA413_CONF_FINAL   0x00000400

/* Hardware configuration register. */
#define BA413_HW_CFG_MD5_SUPPORTED_LSB            0
#define BA413_HW_CFG_MD5_SUPPORTED_MASK           (1L<<BA413_HW_CFG_MD5_SUPPORTED_LSB)
#define BA413_HW_CFG_SHA1_SUPPORTED_LSB           1
#define BA413_HW_CFG_SHA1_SUPPORTED_MASK          (1L<<BA413_HW_CFG_SHA1_SUPPORTED_LSB)
#define BA413_HW_CFG_SHA224_SUPPORTED_LSB         2
#define BA413_HW_CFG_SHA224_SUPPORTED_MASK        (1L<<BA413_HW_CFG_SHA224_SUPPORTED_LSB)
#define BA413_HW_CFG_SHA256_SUPPORTED_LSB         3
#define BA413_HW_CFG_SHA256_SUPPORTED_MASK        (1L<<BA413_HW_CFG_SHA256_SUPPORTED_LSB)
#define BA413_HW_CFG_SHA384_SUPPORTED_LSB         4
#define BA413_HW_CFG_SHA384_SUPPORTED_MASK        (1L<<BA413_HW_CFG_SHA384_SUPPORTED_LSB)
#define BA413_HW_CFG_SHA512_SUPPORTED_LSB         5
#define BA413_HW_CFG_SHA512_SUPPORTED_MASK        (1L<<BA413_HW_CFG_SHA512_SUPPORTED_LSB)
#define BA413_HW_CFG_SM3_SUPPORTED_LSB            6
#define BA413_HW_CFG_SM3_SUPPORTED_MASK           (1L<<BA413_HW_CFG_SM3_SUPPORTED_LSB)
#define BA413_HW_CFG_PADDING_SUPPORTED_LSB        16
#define BA413_HW_CFG_PADDING_SUPPORTED_MASK       (1L<<BA413_HW_CFG_PADDING_SUPPORTED_LSB)
#define BA413_HW_CFG_HMAC_SUPPORTED_LSB           17
#define BA413_HW_CFG_HMAC_SUPPORTED_MASK          (1L<<BA413_HW_CFG_HMAC_SUPPORTED_LSB)
#define BA413_HW_CFG_VERIFY_DIGEST_SUPPORTED_LSB  18
#define BA413_HW_CFG_VERIFY_DIGEST_SUPPORTED_MASK (1L<<BA413_HW_CFG_VERIFY_DIGEST_SUPPORTED_LSB)

#define BA413_HW_CFG (*((const volatile uint32_t*) &CRYPTOACC->BA413_HW_CFG))

#endif
