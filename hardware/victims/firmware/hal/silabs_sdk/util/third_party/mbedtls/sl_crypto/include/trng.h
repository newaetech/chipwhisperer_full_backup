/***************************************************************************//**
 * @file
 * @brief True Random Number Generator (TRNG) driver for Silicon Labs devices
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
#ifndef MBEDTLS_TRNG_H
#define MBEDTLS_TRNG_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/entropy.h"
#include "em_device.h"

#if defined(MBEDTLS_TRNG_C) && defined(TRNG_PRESENT)

/***************************************************************************//**
 * \addtogroup rng_module
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_entropy_trng True Random Number Generator (TRNG) Plugin
 * \brief True Random Number Generator (TRNG) driver for Silicon Labs devices
 *
 * \details EFR32xG12/EFM32xG12 and newer Silicon Labs devices contains
 * a True Random Number Generator (TRNG) peripheral. The TRNG is a
 * non-deterministic random number generator based on a full hardware solution.
 * The TRNG contains a 64 x 32-bit FIFO for reading out random numbers.
 *
 * @note The TRNG peripheral may either be stand-alone or integrated in the SE
 * peripheral depending on the target device.
 *
 * The samples from entropy source within the TRNG are monitored permanently by
 * 4 built in tests that detect issues with the noise source. The tests are
 * specified in NIST-800-90B and AIS31. The tests that are always checking the
 * entropy source are "Repetition Count Test", "Adaptive Proportion Test
 * (64-sample window)", "Adaptive Proportion Test (4096-sample window)" and
 * the "AIS31 Online Test".
 *
 * In addition the TRNG has options for running startup tests. When these tests
 * are enabled, the TRNG FIFO will not contain any data before all the startup
 * tests have passed. There are 4 TRNG startup tests, 3 of the tests are
 * specified in NIST-800-90B. These are the "Repetition Count Test", "Adaptive
 * Proportion Test (64-sample window)" and "Adaptive Proportion Test
 * (4096-sample window)". The last startup test is the AIS31 startup test. By
 * default when using this driver all the startup tests are enabled.
 *
 * The TRNG module implements an entropy source plugin module for mbed TLS that
 * can be used in applications needing random numbers or indirectly using
 * mbed TLS modules that depend on the random number generation interfaces of
 * mbed TLS. The #define @ref MBEDTLS_TRNG_C will compile the TRNG module.
 * The TRNG functions are declared in 'mbedtls/sl_crypto/include/trng.h'.
 *
 * \{
 ******************************************************************************/

#include "em_device.h"
#include <stddef.h>
#include "mbedtls_ecode.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Conditioning test failed. */
#define MBEDTLS_ERR_TRNG_CONDITIONING_TEST_FAILED             ((int)MBEDTLS_ERR_TRNG_BASE | 0x00000001)

/** No data received in the TRNG FIFO. */
#define MBEDTLS_ERR_TRNG_NO_DATA                              ((int)MBEDTLS_ERR_TRNG_BASE | 0x00000002)

/** Repetition Count test failed. The repetition count test fails when the
 *  TRNG detects that the output become "stuck" on a single value for a long
 *  period of time. The repetition count test is described in NIST-800-90B.
 *
 *  If an application detects this error then the TRNG should be reset. The
 *  repetition count test is always enabled. */
#define MBEDTLS_ERR_TRNG_REPETITION_COUNT_TEST_FAILED         ((int)MBEDTLS_ERR_TRNG_BASE | 0x00000003)

/** Adaptive Proportion test over 64 samples failed. The adaptive proportion
 *  test is designed to detect a large loss of entropy that might occur as a
 *  result of some physical failure or environmental change affecting the
 *  TRNG.
 *
 *  The test will fail when a 2 bit sample from the TRNG is repeated an
 *  unusual amount of times within a window of 64 bits. The adaptive
 *  proportion test is further described in NIST-800-90B.
 *
 *  If an application detects this error then the TRNG should be reset. The
 *  adaptive proportion test over 64 samples is always enabled. */
#define MBEDTLS_ERR_TRNG_ADAPTIVE_PROPORTION_TEST_64_FAILED   ((int)MBEDTLS_ERR_TRNG_BASE | 0x00000004)

/** Adaptive Proportion test over 4096 samples failed. The adaptive proportion
 *  test is designed to detect a large loss of entropy that might occur as a
 *  result of some physical failure or environmental change affecting the
 *  TRNG.
 *
 *  The test will fail when a 2 bit sample from the TRNG is repeated an
 *  unusual amount of times within a window of 4096 bits. The adaptive
 *  proportion test is further described in NIST-800-90B.
 *
 *  If an application detects this error then the TRNG should be reset. The
 *  adaptive proportion test over 4096 samples is always enabled. */
#define MBEDTLS_ERR_TRNG_ADAPTIVE_PROPORTION_TEST_4096_FAILED ((int)MBEDTLS_ERR_TRNG_BASE | 0x00000005)

/** AIS31 test noise alarm. The AIS31 test is designed to monitor and verify
 *  the statistical distribution of the random numbers from the TRNG. The test
 *  performs 512 consecutive 128 bit X^2 calculations with 4 bit words. The
 *  details of the AIS31 test can be found in the AIS31 specification.
 *
 *  The test will fail when an unusual statistical distribution of the TRNG
 *  output is found.
 *
 *  If an application detects this error then the TRNG should be reset. The
 *  AIS31 test is always enabled. */
#define MBEDTLS_ERR_TRNG_NOISE_ALARM                          ((int)MBEDTLS_ERR_TRNG_BASE | 0x00000006)

/** AIS31 test Preliminary Noise alarm. The preliminary noise alarms generated
 *  from the same AIS31 test that generates \ref MBEDTLS_ERR_TRNG_NOISE_ALARM.
 *  The difference between a preliminary noise alarm and a noise alarm is the
 *  severity and the expected frequency. A preliminary noise alarm will happen
 *  more frequently than a noise alarm, and a preliminary noise alarm is not
 *  considered critical. The preliminary noise alarm is not uncommon and should
 *  be expected from time to time when reading data from the TRNG.
 *
 *  If an application detects a preliminary noise alarm then the recommended
 *  action is to flush the TRNG FIFO, or reset the TRNG. */
#define MBEDTLS_ERR_TRNG_PRELIMINARY_NOISE_ALARM              ((int)MBEDTLS_ERR_TRNG_BASE | 0x00000007)

/**
 * \brief          TRNG context structure
 */
typedef struct
{
    /**  TRNG register block pointer. */
    TRNG_TypeDef  *trng;

    /** User defined callback to be called when an internal TRNG test fails. */
    void (*test_error_callback)( void* user_arg, int error_code );

    /** User defined argument to TRNG test error callback. */
    void*  test_error_callback_user_arg;
}
mbedtls_trng_context;

/**
 * \brief          Initialize TRNG context
 *
 * \details        This function will enable the TRNG0 peripheral by starting
 *                 the TRNG0 clock, initializing the control register, perform
 *                 soft reset and wait until data is available in the FIFO.
 *                 Normally the application will need only one TRNG context.
 *                 If more than one TRNG contexts are instantiated the user
 *                 should be aware that mbedtls_trng_free will disable the TRNG0
 *                 by stopping the clock, requiring mbedtls_trng_init to be
 *                 called again before any TRNG API calls can be used.
 *                 When the TRNG module is used as a standard mbedtls entropy
 *                 source the context passed to
 *                 void mbedtls_entropy_init( mbedtls_entropy_context *ctx )
 *                 will include one TRNG context and initialize it. The
 *                 application should not want to keep more than one
 *                 mbedtls_entropy_context structures as it will consume
 *                 memory.
 *
 * \param ctx      TRNG context to be initialized
 */
void mbedtls_trng_init( mbedtls_trng_context *ctx );

/**
 * \brief          Free TRNG context
 *
 * \details        This function will disable the TRNG0 peripheral by stopping
 *                 the TRNG0 clock. If more than one TRNG contexts are
 *                 instantiated the user should be aware that one call to
 *                 mbedtls_trng_free with any of the trng contexts will disable
 *                 the TRNG0 and effectively disable all other calls in the
 *                 TRNG API, except mbedtls_trng_init which will enable the
 *                 TRNG0 again. Normally the application will need only one
 *                 TRNG context.
 *
 * \param ctx      TRNG context to be released
 */
void mbedtls_trng_free( mbedtls_trng_context *ctx );

/**
 * \brief          Check the TRNG conditioning function
 *
 * \param ctx      TRNG context
 * \param key      128-bit AES key
 *
 * \return
 *   0 if success. Error code if failure.
 */
int mbedtls_trng_set_key( mbedtls_trng_context *ctx, const unsigned char *key );

/**
 * \brief          Check the TRNG conditioning function
 *
 * \param ctx      TRNG context
 *
 * \return
 *   0 if success. \ref MBEDTLS_ERR_TRNG_CONDITIONING_TEST_FAILED on failure.
 */
int mbedtls_trng_check_conditioning( mbedtls_trng_context *ctx );

/**
 * \brief          Check the TRNG entropy source is producing random data
 *
 * \param ctx      TRNG context
 *
 * \return
 *   0 if success. Error code if failure. Note that this function can return
 *                 a \ref MBEDTLS_ERR_TRNG_PRELIMINARY_NOISE_ALARM on some occasions.
 */
int mbedtls_trng_check_entropy( mbedtls_trng_context *ctx );

/**
 * \brief           Poll for entropy data
 *
 * \details         This function will read available random data from the TRNG
 *                  FIFO and place it into the output buffer. The len parameter
 *                  tells this function the maximum number of bytes to read.
 *
 *                  Note that the number of bytes read from the TRNG might differ
 *                  from the number of bytes requested. If any alarms are signaled
 *                  or the TRNG FIFO is empty then this function will return early.
 *
 *                  The return value should be used to see if the operation was
 *                  successful of if an alarm was encountered while reading the
 *                  FIFO. The content of the olen parameter can be used to check
 *                  how many bytes were actually read.
 *
 * \note            On EFR32 Series 1 devices, the TRNG will have a higher than usual
 *                  noise alarm rate if the HFPER clock tree is configured to run
 *                  at HFCLK/2 or faster. For true random numbers, please configure
 *                  HFPER to HFCLK/4 or slower, if at all possible.
 *
 * \param ctx       TRNG context
 * \param output    Buffer to fill with data from the TRNG
 * \param len       Maximum number of bytes to fill in output buffer.
 * \param olen      The actual amount of bytes put into the buffer (Can be 0)
 *
 * \return          \li 0 if no critical failures occurred,
 *                  \li MBEDTLS_ERR_TRNG_PRELIMINARY_NOISE_ALARM if a AIS31
 *                  preliminary noise alarm was detected while reading the FIFO,
 *                  \li MBEDTLS_ERR_TRNG_NOISE_ALARM if an AIS31 noise alarm
 *                  was detected.
 *                  \li MBEDTLS_ERR_TRNG_REPETITION_COUNT_TEST_FAILED if the
 *                  repetition count test failed while reading the FIFO.
 *                  \li MBEDTLS_ERR_TRNG_ADAPTIVE_PROPORTION_TEST_64_FAILED if the
 *                  adaptive proportion test over 64 samples failed while reading
 *                  the FIFO.
 *                  \li MBEDTLS_ERR_TRNG_ADAPTIVE_PROPORTION_TEST_4096_FAILED if
 *                  the adaptive proportion test over 4096 samples failed while
 *                  reading from the FIFO.
 */
int mbedtls_trng_poll( mbedtls_trng_context *ctx,
                       unsigned char *output, size_t len, size_t *olen );

/**
 * \brief           Execute TRNG soft reset
 *
 * \details         This function performs a TRNG soft reset. The TRNG soft
 *                  reset can be used to clear error conditions such as Noise
 *                  Alarms, etc.
 *
 * \param ctx       TRNG context
 */
void mbedtls_trng_soft_reset( mbedtls_trng_context *ctx );

/**
 * \brief           Set TRNG test error callback
 *
 * \details         This function registers a user defined callback function
 *                  to be called when an internal TRNG test fails.
 *
 * \param ctx       TRNG context
 * \param callback  Pointer to callback function which takes a user defined
 *                  argument as first parameter and the error code as the
 *                  second parameter.
 * \param user_arg  User defined argument to be passed to callback function.
 */
void mbedtls_trng_test_error_callback_set( mbedtls_trng_context *ctx,
                                           void (*callback)( void*, int ),
                                           void *user_arg );

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_TRNG_C && TRNG_PRESENT */

/** \} (end addtogroup sl_crypto_trng) */
/** \} (end addtogroup sl_crypto) */

#endif /* MBEDTLS_TRNG_H */
