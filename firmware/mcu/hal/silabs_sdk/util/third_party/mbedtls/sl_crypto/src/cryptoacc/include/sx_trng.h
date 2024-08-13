/**
 * @file
 * @brief Defines the procedures to make operations with
 *          the BA431 (N)DRNG
 *          (see BA431_NDRNG_DataSheet.pdf)
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#ifndef SX_TRNG_H
#define SX_TRNG_H

#include <stdint.h>
#include <stdbool.h>
#include "compiler_extentions.h"
#include "cryptolib_types.h"

typedef enum sx_trng_test {
   TRNG_REP_TEST,      /**< Repetition count test. */
   TRNG_PROP_TEST,     /**< Adaptive proportion test. */
   TRNG_PREALM_TEST,   /**< AIS31 preliminary noise alarm */
   TRNG_ALM_TEST,      /**< AIS31 noise alarm. */
} sx_trng_test_t;

/** @brief structure used in save/restore TRNG state. Should be considered opaque */
struct sx_trng_internal_state
{
   uint32_t control;
   uint32_t key[4];
   uint32_t wakeup_lvl;
   uint32_t swoff_timer;
   uint32_t clk_divider;
   uint32_t init_wait_cnt;
};

/**
* @brief TRNG initialization
* @param cond_test_en When not zero, conditioning test is executed first
* @return CRYPTOLIB_SUCCESS or CRYPTOLIB_CRYPTO_ERR
*/
uint32_t sx_trng_init(uint32_t cond_test_en) CHECK_RESULT;

/**
* @brief First part of TRNG initialization
* For boot time optimization, the TRNG init can be split into 2 parts. The first
* part executes the conditioning test, makes some configurations and switches on
* the oscillator rings. After that it returns, allowing to call other functions
* while the TRNG FIFO is filling up in the background.
* @param cond_test_en When not zero, conditioning test is executed
* @return CRYPTOLIB_SUCCESS or CRYPTOLIB_CRYPTO_ERR
*/
uint32_t sx_trng_init_part1(uint32_t cond_test_en) CHECK_RESULT;

/**
* @brief Second part of TRNG initialization
* For boot time optimization, the TRNG init can be split into 2 parts. The
* second part checks that startup tests were successful, once the TRNG FIFO is
* fully filled. Then it programs a random key for the conditioning function,
* resets the TRNG and returns. Note that the FIFO should be fully filled again
* and startup tests successful before reading the first piece of random data.
* This can be assured by using ::sx_trng_set_startup_chk_flag after TRNG init.
* @return CRYPTOLIB_SUCCESS or CRYPTOLIB_CRYPTO_ERR
*/
uint32_t sx_trng_init_part2(void) CHECK_RESULT;

/**
* @brief Indicate whether conditioning or startup test failed during init.
* Only valid during bootloader and diagnostic mode!
* If false is returned, true randomness can't be warranted and user must react
* (i.e. raising tamper)
* @return boolean
*/
bool sx_trng_startup_failed(void);

/**
* @brief Sets a flag that tells the TRNG software module to wait for TRNG
* startup tests to be finished successfully before reading random data from the
* TRNG FIFO.This needs to be done after explicitly calling the ba431_softreset()
* in interrupt context and after TRNG initialization.
*
* The TRNG startup test will wait until the TRNG FIFO is filled with data and
* then check that the data is sufficiently random. If it is, the startup test is
* passed. The BA431 datasheet specifies that we shouldn't use data from the TRNG
* FIFO before it has been confirmed to be sufficiently random.
*/
void sx_trng_set_startup_chk_flag(void);

/**
 * @brief Indicates whether a NIST-800-90B repetition count test fails,
 *  a adaptative proportion test fails or (if enabled) a AIS31 noise alarm
 *  is triggered.
 * @return true if one error described above is raised
 */
bool sx_trng_critical_raised(void);

/**
* @brief Applies a soft reset on the random generator and sets a flag that tells
* the TRNG software module to wait for TRNG startup tests to be finished
* successfully before reading random data from the TRNG FIFO.
*
* The TRNG startup test will wait until the TRNG FIFO is filled with data and
* then check that the data is sufficiently random. If it is, the startup test is
* passed. The BA431 datasheet specifies that we shouldn't use data from the TRNG
* FIFO before it has been confirmed to be sufficiently random.
*/

void sx_trng_apply_soft_reset(void);

/**
* @brief Generate random data (block_t)
* @param dest Output location for the generated random data.
*/
void sx_trng_get_rand_blk(block_t dest);

/**
* @brief Get an array of random values
* @param dst Pointer to the array where the random values are stored
* @param size Number of random bytes to be generate
*/
void sx_trng_get_rand(uint8_t *dst, uint32_t size);

/**
* @brief Returns a random 32-bit word
* @return 32-bit random value
*/
uint32_t sx_trng_get_word(void);

/**
* @brief Generate a random block of data by the TRNG. Defined to match the
* prototype of the struct sx_rng get_rand_blk()
* @param param Should be set to NULL
* @param result block_t for the generated values
*/
void sx_trng_fill_blk(void *param, block_t result);

#ifdef UNITTESTS
uint32_t sx_trng_conditioning_test(void);
uint32_t sx_trng_wait_startup(void);
#endif

/**
 * @brief Return if the TRNG module is  facing an error state
 *
 * This error could be due to start-up tests (NIST-800-90B Start-up Test or
 * AIS31 Start-up Test) or due to online test (Adaptive Proportion Test or
 * AIS31 Online  Test)
 */

bool sx_trng_is_error_detected(void);

/**
* @brief Cause a TRNG to fail (on health tests)
* For test failure confirmation the TRNG interrupt needs to be checked.
*
* \note This test will wait until a ::BA431_STAT_MASK_PROP_FAIL even if other
*       kind of BA431 failure is generated before.
*/
void sx_trng_trigger_failure(void);

/**
* @brief Cause a TRNG self test to fail for a specified test
* For test failure confirmation the TRNG status and/or interrupt needs to be
* checked.
* @param test Chosen TRNG self test
* @return CRYPTOLIB_SUCCESS if the TRNG was successfully configured for causing
*                          the test failure
*/
uint32_t sx_trng_trigger_test_failure(sx_trng_test_t test);

/**
 * @brief Save a local copy of the TRNG internal configuration state
 * @param copy is structure which will contain the internal state required to
 *        reconfigure the BA431 with specifics parameters
 */
void sx_trng_save_state(struct sx_trng_internal_state *copy);

/**
 * @brief Restore to the TRNG the internal configuration state
 * @param source is a structure containing the config to restore to the TRNG
 */
void sx_trng_restore_state(struct sx_trng_internal_state *source);

#endif /* SX_TRNG_H */
