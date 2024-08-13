/***************************************************************************//**
 * @file
 * @brief Silicon Labs CRYPTOACC device management interface.
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

#include "cryptoacc_management.h"
#include "em_core.h"

#if defined(CRYPTOACC_PRESENT)

#include "sx_trng.h"
#include "sx_errors.h"

#include "cryptolib_def.h"
#include "mbedtls/threading.h"
#if defined( MBEDTLS_THREADING_C )
static mbedtls_threading_mutex_t    cryptoacc_lock;
static volatile bool                cryptoacc_lock_initialized = false;
#endif /* MBEDTLS_THREADING_C */

#define DO_TRNG_COND_TEST  (1)

// Dummy function pointers to make cryptolib compile without sm4 and aria source files.
uint32_t (*sx_sm4_set_hw_config_for_key)(block_t *key, uint32_t *config) = NULL;
uint32_t (*sx_aria_set_hw_config_for_key)(block_t *key, uint32_t *config) = NULL;

static bool trng_initialized = false;

/* Get ownership of an available CRYPTOACC device */
int cryptoacc_management_acquire( void )
{
    int retVal;
#if defined( MBEDTLS_THREADING_C )
    if ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0U) {
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }

    /* Initialize mutex if that hasn't happened yet */
    CORE_DECLARE_IRQ_STATE;

    // Check flag first before going into a critical section, to avoid going into
    // a critical section on every single acquire() call. Since the _initialized
    // flag only transitions false -> true, we can in 99% of the calls avoid the
    // critical section.
    if ( !cryptoacc_lock_initialized ) {
        CORE_ENTER_CRITICAL();

        // Check the flag again after entering the critical section. Now that we're
        // in the critical section, we can be sure that we are the only ones looking
        // at the flag and no-one is interrupting us during its manipulation.
        if ( !cryptoacc_lock_initialized ) {
            mbedtls_mutex_init( &cryptoacc_lock );
            cryptoacc_lock_initialized = true;
        }

        CORE_EXIT_CRITICAL( );
    }

    retVal = mbedtls_mutex_lock( &cryptoacc_lock );
#else //!MBEDTLS_THREADING_C
    retVal = 0;
#endif
    if (retVal == 0) {
        CMU->CLKEN1 |= CMU_CLKEN1_CRYPTOACC;
        CMU->CRYPTOACCCLKCTRL |= CMU_CRYPTOACCCLKCTRL_PKEN;
        CMU->CRYPTOACCCLKCTRL |= CMU_CRYPTOACCCLKCTRL_AESEN;
    }
    return retVal;
}

/* Release ownership of an available CRYPTOACC device */
int cryptoacc_management_release( void )
{
    CMU->CLKEN1 = (CMU->CLKEN1 & ~CMU_CLKEN1_CRYPTOACC);
    CMU->CRYPTOACCCLKCTRL = (CMU->CRYPTOACCCLKCTRL & ~CMU_CRYPTOACCCLKCTRL_PKEN);
    CMU->CRYPTOACCCLKCTRL = (CMU->CRYPTOACCCLKCTRL & ~CMU_CRYPTOACCCLKCTRL_AESEN);
#if defined( MBEDTLS_THREADING_C )
    return mbedtls_mutex_unlock(&cryptoacc_lock);
#else //!MBEDTLS_THREADING_C
    return 0;
#endif
}

int cryptoacc_trng_initialize( void )
{
    int status;
    if(!trng_initialized) {
        status = cryptoacc_management_acquire();
        if (status != 0) {
            return status;
        }
        status = sx_trng_init(DO_TRNG_COND_TEST);
        cryptoacc_management_release();
        if (status != CRYPTOLIB_SUCCESS) {
            return MBEDTLS_ERR_CRYPTOACC_TRNG_FAILED;
        }
        trng_initialized = true;
    }
    return 0;
}

#endif /* CRYPTOACC_PRESENT */
