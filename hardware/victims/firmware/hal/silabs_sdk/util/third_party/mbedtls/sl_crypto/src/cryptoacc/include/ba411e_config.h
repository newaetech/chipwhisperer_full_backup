/**
 * @file
 * @brief Defines macros to be used for the configuration of the BA411E AES
 * @copyright Copyright (c) 2016-2019 Silex Insight. All Rights reserved
 */


#ifndef BA411E_CONFIG_H
#define BA411E_CONFIG_H
#include <stdint.h>
#include <stdbool.h>

#include "em_device.h"
#include "sx_blk_cipher_common.h"
#include "sx_blk_cipher.h"

/** @brief BA411E Mode Register mask for hardware key 1 & 2*/
#define AES_MODEID_KEYX_MASK     0x000000C0

/* AES hardware configuration - register 1*/
#define AES_HW_CFG_ECB_SUPPORTED_LSB      0
#define AES_HW_CFG_ECB_SUPPORTED_MASK     (1L<<AES_HW_CFG_ECB_SUPPORTED_LSB)
#define AES_HW_CFG_CBC_SUPPORTED_LSB      1
#define AES_HW_CFG_CBC_SUPPORTED_MASK     (1L<<AES_HW_CFG_CBC_SUPPORTED_LSB)
#define AES_HW_CFG_CTR_SUPPORTED_LSB      2
#define AES_HW_CFG_CTR_SUPPORTED_MASK     (1L<<AES_HW_CFG_CTR_SUPPORTED_LSB)
#define AES_HW_CFG_CFB_SUPPORTED_LSB      3
#define AES_HW_CFG_CFB_SUPPORTED_MASK     (1L<<AES_HW_CFG_CFB_SUPPORTED_LSB)
#define AES_HW_CFG_OFB_SUPPORTED_LSB      4
#define AES_HW_CFG_OFB_SUPPORTED_MASK     (1L<<AES_HW_CFG_OFB_SUPPORTED_LSB)
#define AES_HW_CFG_CCM_SUPPORTED_LSB      5
#define AES_HW_CFG_CCM_SUPPORTED_MASK     (1L<<AES_HW_CFG_CCM_SUPPORTED_LSB)
#define AES_HW_CFG_GCM_SUPPORTED_LSB      6
#define AES_HW_CFG_GCM_SUPPORTED_MASK     (1L<<AES_HW_CFG_GCM_SUPPORTED_LSB)
#define AES_HW_CFG_XTS_SUPPORTED_LSB      7
#define AES_HW_CFG_XTS_SUPPORTED_MASK     (1L<<AES_HW_CFG_XTS_SUPPORTED_LSB)
#define AES_HW_CFG_CMAC_SUPPORTED_LSB     8
#define AES_HW_CFG_CMAC_SUPPORTED_MASK    (1L<<AES_HW_CFG_CMAC_SUPPORTED_LSB)
#define AES_HW_CFG_CS_EN_LSB              16 /* Ciphertext Stealing */
#define AES_HW_CFG_CS_EN_MASK             (1L<<AES_HW_CFG_CS_EN_LSB)
#define AES_HW_CFG_CM_EN_LSB              17 /* Countermeasures */
#define AES_HW_CFG_CM_EN_MASK             (1L<<AES_HW_CFG_CM_EN_LSB)
#define AES_HW_CFG_KEY_SIZE_LSB           24
#define AES_HW_CFG_KEY_SIZE_MASK          (0x7<<AES_HW_CFG_KEY_SIZE_LSB)
#define AES_HW_CFG_KEY_SIZE_128_SUPPORTED_LSB 24
#define AES_HW_CFG_KEY_SIZE_128_SUPPORTED_MASK (1L<<AES_HW_CFG_KEY_SIZE_128_SUPPORTED_LSB)
#define AES_HW_CFG_KEY_SIZE_192_SUPPORTED_LSB 25
#define AES_HW_CFG_KEY_SIZE_192_SUPPORTED_MASK (1L<<AES_HW_CFG_KEY_SIZE_192_SUPPORTED_LSB)
#define AES_HW_CFG_KEY_SIZE_256_SUPPORTED_LSB 26
#define AES_HW_CFG_KEY_SIZE_256_SUPPORTED_MASK (1L<<AES_HW_CFG_KEY_SIZE_256_SUPPORTED_LSB)

#define BA411E_HW_CFG_1 (*((const volatile uint32_t*) &CRYPTOACC->BA411E_HW_CFG_1))

/* AES hardware configuration - register 2*/
#define AES_HW_CFG_MAX_CTR_SIZE_LSB       0
#define AES_HW_CFG_MAX_CTR_SIZE_MASK      (0xFFFF<<AES_HW_CFG_MAX_CTR_SIZE_LSB)

#define BA411E_HW_CFG_2 (*((const volatile uint32_t*) &CRYPTOACC->BA411E_HW_CFG_2))

#endif
