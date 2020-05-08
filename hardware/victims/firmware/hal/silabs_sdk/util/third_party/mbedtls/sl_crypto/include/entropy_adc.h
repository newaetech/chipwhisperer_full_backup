/***************************************************************************//**
 * @file
 * @brief Collect entropy from the ADC on Silicon Labs devices.
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
#ifndef MBEDTLS_ENTROPY_ADC_H
#define MBEDTLS_ENTROPY_ADC_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "em_device.h"

#if defined(MBEDTLS_ENTROPY_ADC_C)
#if defined(ADC_PRESENT)
#if defined(_ADC_SINGLECTRLX_VREFSEL_VENTROPY)

#include "em_cmu.h"
#include "em_adc.h"
#include "mbedtls/entropy.h"

/***************************************************************************//**
 * \addtogroup rng_module
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_entropy_adc ADC Entropy Source Plugin
 * \brief Collect entropy from the ADC on Silicon Labs devices
 *
 * The ADC entropy module implements an entropy source plugin module
 * for mbedTLS that can be used in applications needing random numbers or
 * indirectly using mbedTLS modules that depend on the random number generation
 * interfaces of mbed TLS. The configuration #define @ref MBEDTLS_ENTROPY_ADC_C
 * will compile this module. Note that the IADC peripheral is not currently 
 * supported.
 *
 * \{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          ADC entropy context structure
 */
typedef struct
{
    /**  ADC register block pointer. */
    ADC_TypeDef       *adc;
    /** ADC clock */
    CMU_Clock_TypeDef  clk;
}
mbedtls_entropy_adc_context;

/**
 * \brief          Initialize an ADC entropy context
 *
 * \details        This function will initialize an ADC entropy context.
 *                 This function will not initialize the ADC hardware instance
 *                 as an entropy source. The user must call
 *                 @ref mbedtls_entropy_adc_set_instance in order to
 *                 initialize the ADC hardware to entropy source mode.
 *
 * \param ctx      ADC entropy context to be initialized
 */
 void mbedtls_entropy_adc_init( mbedtls_entropy_adc_context *ctx );

/**
 * \brief          Set and initialize an ADC hardware instance.
 *
 * \details        This function will initialize an ADC hardware instance as
 *                 an entropy source by starting the ADC clock
 *                 and initializing registers to prepare collecting entropy
 *                 in single sample mode.
 *                 This function assumes that the ADC clock mode is set to
 *                 SYNC, i.e. the HFPERCLK generates the ADC_CLK.
 *
 * \param ctx      ADC entropy context
 * \param adc_inst ADC instance number to use
 *
 * \return         0 for success or MBEDTLS_ERR_ENTROPY_SOURCE_FAILED if
 *                 adc_inst is invalid.
 */
int mbedtls_entropy_adc_set_instance( mbedtls_entropy_adc_context *ctx,
				       unsigned int adc_inst) ;

/**
 * \brief          Free ADC entropy context
 *
 * \details        This function will reset the ADC peripheral and stop
 *                 the ADC clock. If more than one ENTROPY_ADC contexts are
 *                 instantiated the user should be aware that one call to
 *                 mbedtls_entropy_adc_free with any of the ADC entropy
 *                 contexts will disable the ADC and effectively disable all
 *                 other calls in the ADC entropy API, except
 *                 @ref mbedtls_entropy_adc_init and 
 *                 @ref mbedtls_entropy_adc_set_instance which will enable
 *                 the ADC again. Normally the application will need only one
 *                 ADC entropy context.
 *
 * \param ctx      ADC entropy context to be released
 */
void mbedtls_entropy_adc_free( mbedtls_entropy_adc_context *ctx );

/**
 * \brief           Poll for entropy data
 *
 * \details         This function will return entropy data from the ADC and
 *                  place it into the output buffer. The len parameter
 *                  tells this function the maximum number of bytes to read.
 *
 *                  Note that the number of bytes read from the ADC might differ
 *                  from the number of bytes requested.
 *
 *                  The return value should be used to see if the operation was
 *                  successful of if an error was encountered while reading the
 *                  ADC. The content of the olen parameter can be used to check
 *                  how many bytes were actually read.
 *
 * \param ctx       ADC entropy context
 * \param output    Buffer to fill with entropy data from ADC
 * \param len       Maximum number of bytes to fill in output buffer.
 * \param olen      The actual amount of bytes put into the buffer (Can be 0)
 *
 * \return          Always 0 for success.
 */
int mbedtls_entropy_adc_poll( mbedtls_entropy_adc_context *ctx,
			      unsigned char *output, size_t len, size_t *olen );

#ifdef __cplusplus
}
#endif

#endif /* #if defined(_ADC_SINGLECTRLX_VREFSEL_VENTROPY) */
#endif /* #if defined(ADC_PRESENT) */
#endif /* #if defined(MBEDTLS_ENTROPY_ADC_C) */

/** \} (end addtogroup sl_crypto_entropy_adc) */
/** \} (end addtogroup rng_module) */

#endif /* MBEDTLS_ENTROPY_ADC_H */
