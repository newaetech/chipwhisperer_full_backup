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

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_TRNG_C)

#include "trng.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_common.h"
#include <string.h>

#if defined(TRNG_PRESENT)

#define FIFO_LEVEL_RETRY   (10000)
#define TEST_WORDS_MIN       (257)

#define TEST_VECTOR_CONDITIONING_KEY_SIZE  (4)
static const uint32_t
test_vector_conditioning_key[TEST_VECTOR_CONDITIONING_KEY_SIZE] =
  {0x16157E2B, 0xA6D2AE28, 0x8815F7AB, 0x3C4FCF09};

#define TEST_VECTOR_CONDITIONING_INPUT_SIZE (16)
static const uint32_t
test_vector_conditioning_input[TEST_VECTOR_CONDITIONING_INPUT_SIZE] =
  {0xE1BCC06B, 0x9199452A, 0x1A7434E1, 0x25199E7F,
   0x578A2DAE, 0x9CAC031E, 0xAC6FB79E, 0x518EAF45,
   0x461CC830, 0x11E45CA3, 0x19C1FBE5, 0xEF520A1A,
   0x45249FF6, 0x179B4FDF, 0x7B412BAD, 0x10376CE6};

#define TEST_VECTOR_CONDITIONING_OUTPUT_SIZE (4)
static const uint32_t
test_vector_conditioning_output[TEST_VECTOR_CONDITIONING_OUTPUT_SIZE] =
  {0xA1CAF13F, 0x09AC1F68, 0x30CA0E12, 0xA7E18675};

#define TRNG_STARTUP_TEST_WAIT_RETRY  (10000)

void mbedtls_trng_init( mbedtls_trng_context *ctx )
{
    int i;

    memset(ctx, 0, sizeof( mbedtls_trng_context ) );

    /* Enable the TRNG0 clock. */
    CMU_ClockEnable( cmuClock_TRNG0, true );

    /* Enable TRNG0. */
    ctx->trng = TRNG0;

    ctx->trng->CONTROL =
        TRNG_CONTROL_ENABLE |
        TRNG_CONTROL_REPCOUNTIEN |
        TRNG_CONTROL_APT64IEN |
        TRNG_CONTROL_APT4096IEN |
        TRNG_CONTROL_PREIEN |
        TRNG_CONTROL_ALMIEN;

    /* Apply software reset */
    mbedtls_trng_soft_reset(ctx);

    /* Wait for TRNG to complete startup tests and start filling the FIFO. */
    for (i=0; (TRNG0->FIFOLEVEL == 0) && (i<TRNG_STARTUP_TEST_WAIT_RETRY); i++);
    EFM_ASSERT(i<TRNG_STARTUP_TEST_WAIT_RETRY);
}

void mbedtls_trng_free( mbedtls_trng_context *ctx )
{
    /* Disable TRNG0. */
    ctx->trng->CONTROL = 0;

    /* Disable the TRNG0 clock. */
    CMU_ClockEnable( cmuClock_TRNG0, false );

    memset(ctx, 0, sizeof( mbedtls_trng_context ) );
}

void mbedtls_trng_soft_reset( mbedtls_trng_context *ctx )
{
    TRNG_TypeDef *trng = ctx->trng;
    uint32_t ctrl = trng->CONTROL;

    ctrl |= TRNG_CONTROL_SOFTRESET;

    trng->CONTROL = ctrl;

    ctrl &= ~TRNG_CONTROL_SOFTRESET;
    trng->CONTROL = ctrl;
}

static inline
void mbedtls_trng_write_test_data( mbedtls_trng_context *ctx, uint32_t data )
{
    TRNG_TypeDef *trng = ctx->trng;
    /* Wait for TESTDATA register to become ready for next word. */
    while (trng->STATUS & TRNG_STATUS_TESTDATABUSY);
    trng->TESTDATA = data;
}

static void mbedtls_trng_clear_fifo( mbedtls_trng_context *ctx )
{
  TRNG_TypeDef *trng = ctx->trng;
  volatile uint32_t val32;

  /* Empty FIFO */
  while ( trng->FIFOLEVEL )
  {
      val32 = trng->FIFO;
      (void)val32;
  }
}

int mbedtls_trng_set_key( mbedtls_trng_context *ctx, const unsigned char *key )
{
    TRNG_TypeDef *trng = ctx->trng;
    uint32_t _key[4];

    memcpy(_key, key, sizeof(_key));

    mbedtls_trng_clear_fifo(ctx);

    /* Program key in KEY registers of the TRNG. */
    trng->KEY0 = _key[0];
    trng->KEY1 = _key[1];
    trng->KEY2 = _key[2];
    trng->KEY3 = _key[3];

    return 0;
}

int mbedtls_trng_check_conditioning( mbedtls_trng_context *ctx )
{
    TRNG_TypeDef *trng = ctx->trng;
    uint32_t val32;
    int i, ret=0;
    uint32_t ctrl = trng->CONTROL;

    /* Setup control register */
    trng->CONTROL = TRNG_CONTROL_ENABLE | TRNG_CONTROL_TESTEN |
        TRNG_CONTROL_BYPNIST | TRNG_CONTROL_BYPAIS31;

    /* Apply software reset */
    mbedtls_trng_soft_reset(ctx);

    /* Write test vector to the key register. */
    mbedtls_trng_set_key(ctx,
                         (const unsigned char*)test_vector_conditioning_key);

    /* Write test vector to the TESTDATA register */
    for (i=0; i<TEST_VECTOR_CONDITIONING_INPUT_SIZE; i++)
    {
      mbedtls_trng_write_test_data(ctx, test_vector_conditioning_input[i]);
    }

    for (i=0; i<TEST_VECTOR_CONDITIONING_OUTPUT_SIZE; i++)
    {
        /* Wait for data to become available in the FIFO. */
        while ( 0 == trng->FIFOLEVEL );
        /* Read output from the conditioning function */
        val32 = trng->FIFO;
        /* Compare with expected test vector. */
        if (val32 != test_vector_conditioning_output[i])
        {
            ret = MBEDTLS_ERR_TRNG_CONDITIONING_TEST_FAILED;
        }
    }

    /* Restore initial value of control register */
    trng->CONTROL = ctrl;

    return ret;
}

static int trng_check_status( mbedtls_trng_context *ctx )
{
    TRNG_TypeDef *trng = ctx->trng;
    uint32_t status = trng->STATUS;

    if ( (status & (TRNG_STATUS_PREIF
                    | TRNG_STATUS_REPCOUNTIF
                    | TRNG_STATUS_APT64IF
                    | TRNG_STATUS_APT4096IF
                    | TRNG_STATUS_ALMIF)) == 0 )
    {
        /* No errors */
        return 0;
    }

    if ( status & TRNG_STATUS_PREIF )
    {
        /* On a preliminary noise alarm we clear the FIFO and clear
         * the alarm. The preliminary noise alarm is not critical. */
        status &= ~TRNG_STATUS_PREIF;
        trng->STATUS = status;
        mbedtls_trng_clear_fifo(ctx);
        return MBEDTLS_ERR_TRNG_PRELIMINARY_NOISE_ALARM;
    }
    else
    {
        /* Clear alarm conditions by doing a TRNG soft reset. */
        mbedtls_trng_soft_reset( ctx );
        if ( status & TRNG_STATUS_REPCOUNTIF )
        {
            return MBEDTLS_ERR_TRNG_REPETITION_COUNT_TEST_FAILED;
        }
        if ( status & TRNG_STATUS_APT64IF )
        {
            return MBEDTLS_ERR_TRNG_ADAPTIVE_PROPORTION_TEST_64_FAILED;
        }
        if ( status & TRNG_STATUS_APT4096IF )
        {
            return MBEDTLS_ERR_TRNG_ADAPTIVE_PROPORTION_TEST_4096_FAILED;
        }
        if ( status & TRNG_STATUS_ALMIF )
        {
            return MBEDTLS_ERR_TRNG_NOISE_ALARM;
        }
    }

    return 0;
}

int mbedtls_trng_check_entropy( mbedtls_trng_context *ctx )
{
    TRNG_TypeDef *trng = ctx->trng;
    volatile uint32_t val32;
    int i, ret = 0;
    uint32_t ctrl = trng->CONTROL;

    /* Setup control register */
    trng->CONTROL =
        TRNG_CONTROL_ENABLE |
        TRNG_CONTROL_REPCOUNTIEN |
        TRNG_CONTROL_APT64IEN |
        TRNG_CONTROL_APT4096IEN |
        TRNG_CONTROL_PREIEN |
        TRNG_CONTROL_ALMIEN;

    /* Apply software reset */
    mbedtls_trng_soft_reset(ctx);

    /* Check FIFO level is non-zero . */
    for (i=0; i<FIFO_LEVEL_RETRY; i++)
    {
        if ( trng->FIFOLEVEL )
        {
            break;
        }
    }
    /* Check for no data within timeout (max retry count) */
    if (i>=FIFO_LEVEL_RETRY)
    {
        ret = MBEDTLS_ERR_TRNG_NO_DATA;
    }
    else
    {
        /* Read at least 4097x2 bits (~257 x 32 bits) in order for the longest
           test to complete (adaptive proportion test of 4096 samples). */
        for (i=0; i<TEST_WORDS_MIN; i++)
        {
            val32 = trng->FIFO;
            (void)val32;
        }

        /* Check in status register for errors. */
        ret = trng_check_status( ctx );
    }

    /* Restore initial value of control register */
    trng->CONTROL = ctrl;

    return ret;
}

static void mbedtls_trng_read_chunk( mbedtls_trng_context *ctx,
                                     unsigned char *output, size_t len )
{
    TRNG_TypeDef *trng = ctx->trng;
    uint32_t * out32 = (uint32_t *) output;
    uint32_t tmp;

    /* Read known good available data. */
    while ( len >= 4 )
    {
        *out32++ = trng->FIFO;
        len -= 4;
    }    
    if (len == 0) return;

    /* Handle the case where len is not a multiple of 4 
       and FIFO data is available. */
    tmp = trng->FIFO;
    memcpy((uint8_t *)out32, (const uint8_t *) &tmp, len);
}

int mbedtls_trng_poll( mbedtls_trng_context *ctx,
                       unsigned char *output, size_t len, size_t *olen )
{
    TRNG_TypeDef *trng = ctx->trng;
    unsigned char *outptr = output;
    size_t output_len = 0;
    size_t count = 0;
    size_t available;
    int ret = 0;

    while (len > 0)
    {
        available = trng->FIFOLEVEL * 4;
        if (available == 0)
        {
            break;
        }

        /* Check status for current data in FIFO
         * and handle any error conditions. */
        ret = trng_check_status( ctx );

        /* The TRNG output data did not pass the internal TRNG random tests.
         * If the user has registered a callback function for TRNG test errors
         * call it to notify the user.
         * Discard the bad data by setting output length to zero.
         * Return 0 (OK) in order to keep the entropy accumulator of mbedtls
         * running. */

        if (ret != 0)
        {
            if (ctx->test_error_callback)
            {
                ctx->test_error_callback(ctx->test_error_callback_user_arg,
                                         ret);
            }
            memset(output, 0, output_len);
            output_len = 0;
            ret = 0;
            break;
        }

        count = SL_MIN(len, available);
        mbedtls_trng_read_chunk(ctx, outptr, count);
        outptr += count;
        output_len += count;
        len -= count;
    }

    *olen = output_len;
    return ret;
}

void mbedtls_trng_test_error_callback_set( mbedtls_trng_context *ctx,
					   void (*callback)( void*, int ),
					   void *user_arg )
{
    ctx->test_error_callback = callback;
    ctx->test_error_callback_user_arg = user_arg;
}

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT) && defined(MBEDTLS_ENTROPY_C)

static bool initialized = false;
static mbedtls_trng_context trng_ctx;

#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_89)
#include "em_system.h"
#endif
int mbedtls_hardware_poll( void *data,
                           unsigned char *output,
                           size_t len,
                           size_t *olen ) {
#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_89)
    SYSTEM_ChipRevision_TypeDef rev;
    SYSTEM_ChipRevisionGet(&rev);
    // TRNG entropy on xG13 is only supported for >= Rev A3
    if ((rev.major == 1) && (rev.minor < 3)) {
        while (true) {
            EFM_ASSERT(false);
        }
    }
#endif
    (void)data;
    if(!initialized) {
        mbedtls_trng_init(&trng_ctx);
        initialized = true;
    }

    return mbedtls_trng_poll(&trng_ctx, output, len, olen);
}

#endif // MBEDTLS_ENTROPY_HARDWARE_ALT && MBEDTLS_ENTROPY_C
#endif // TRNG_PRESENT
#endif // MBEDTLS_TRNG_C
