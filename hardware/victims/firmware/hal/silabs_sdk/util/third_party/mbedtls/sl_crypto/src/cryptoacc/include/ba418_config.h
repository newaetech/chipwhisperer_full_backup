/**
 * @file
 * @brief Defines the procedures to make operations with
 *          the BA418 hash function
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#ifndef BA418_CONFIG_H
#define BA418_CONFIG_H

#include <stdint.h>
/** @brief Value for ::ba418_regs_s.config, set mode to SHA3 224 */
#define BA418_CONF_MODE_SHA3_224 0x06
/** @brief Value for ::ba418_regs_s.config, set mode to SHA3 256 */
#define BA418_CONF_MODE_SHA3_256 0x07
/** @brief Value for ::ba418_regs_s.config, set mode to SHA3 384 */
#define BA418_CONF_MODE_SHA3_384 0x0B
/** @brief Value for ::ba418_regs_s.config, set mode to SHA3 512 */
#define BA418_CONF_MODE_SHA3_512 0x0F
/** @brief Value for ::ba418_regs_s.config, set mode to SHAKE 128 */
#define BA418_CONF_MODE_SHAKE_128 0x13
/** @brief Value for ::ba418_regs_s.config, set mode to SHAKE 256 */
#define BA418_CONF_MODE_SHAKE_256 0x17
/** @brief Value for ::ba418_regs_s.config, sets the output as a state not as digest */
#define BA418_CONF_OUTPUT_STATE	0x40

struct ba418_regs_s {
   volatile uint32_t config;
};

/**
* @brief Enumeration of the supported SHA3 Hash operations
*/
typedef enum sx_sha3_hash_op_e
{
   e_SHA3_OP_WHOLE  = 0x00,
   e_SHA3_OP_BEGIN  = 0x01,
   e_SHA3_OP_UPDATE = 0x02,
   e_SHA3_OP_FINAL  = 0x03
} sx_sha3_op_t;

/* Hardware configuration register */
#define BA418_HW_CFG_CONTEXT_ENABLED_LSB   0
#define BA418_HW_CFG_CONTEXT_ENABLED_MASK (1<<BA418_HW_CFG_CONTEXT_ENABLED_LSB)

#define BA418_HW_CFG (*((const volatile uint32_t*) &CRYPTOACC->BA418_HW_CFG))
#endif
