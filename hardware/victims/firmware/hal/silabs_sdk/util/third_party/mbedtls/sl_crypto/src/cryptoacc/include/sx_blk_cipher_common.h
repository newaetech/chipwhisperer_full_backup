/**
 * @file
 * @brief Defines common macros used by block ciphers for configure input/output
 *        data, used in AES, SM4 and ARIA
 * @copyright Copyright (c) 2019 Silex Insight. All Rights reserved
 */

#ifndef BLK_CIPHER_COMMON_H
#define BLK_CIPHER_COMMON_H

/** @brief Block data size for block cipher */
#define BLK_CIPHER_BLOCK_SIZE        16
/** @brief Size for block cipher IV in GCM mode */
#define BLK_CIPHER_IV_GCM_SIZE       12
/** @brief Size for block cipher IV in all modes except GCM */
#define BLK_CIPHER_IV_SIZE           16
/** @brief Size for block cipher Context in GCM and CCM modes */
#define BLK_CIPHER_CTX_xCM_SIZE      32
/** @brief Size for block cipher Context in all modes except GCM and CCM */
#define BLK_CIPHER_CTX_SIZE          16
/** @brief Size of a GCM/GMAC/CMAC block cipher message authentification
 *         code (MAC) or maximum size of a CCM MAC */
#define BLK_CIPHER_MAC_SIZE          16

#endif
