/***************************************************************************//**
 * @file
 * @brief Silicon Labs CRYPTO device management interface.
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

#include "se_management.h"
#include "em_core.h"

#if defined( SEMAILBOX_PRESENT )

#include "mbedtls/threading.h"
#if defined( MBEDTLS_THREADING_C )
static mbedtls_threading_mutex_t    se_lock;
static volatile bool                se_lock_initialized = false;
#endif /* MBEDTLS_THREADING_C */

/* Get ownership of an available SE mailbox */
int se_management_acquire( void )
{
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
    if ( !se_lock_initialized ) {
        CORE_ENTER_CRITICAL();

        // Check the flag again after entering the critical section. Now that we're
        // in the critical section, we can be sure that we are the only ones looking
        // at the flag and no-one is interrupting us during its manipulation.
        if ( !se_lock_initialized ) {
            mbedtls_mutex_init( &se_lock );
            se_lock_initialized = true;
        }

        CORE_EXIT_CRITICAL( );
    }

    mbedtls_mutex_lock( &se_lock );

    return 0;
#else //!MBEDTLS_THREADING_C
#if !defined(USE_SE_IN_IRQ) || USE_SE_IN_IRQ == 0
    // If USE_SE_IN_IRQ is not explicilty defined, then calling SE-based functionality
    // from IRQ context is not supported, since there's no way to tell whether the
    // SEMAILBOX is currently processing a command or is idling.
    if ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0U) {
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }
#endif
    return 0;
#endif
}

/* Release ownership of an available crypto device */
int se_management_release( void )
{
#if defined( MBEDTLS_THREADING_C )
    return mbedtls_mutex_unlock(&se_lock);
#else //!MBEDTLS_THREADING_C
    return 0;
#endif
}

#endif /* SEMAILBOX_PRESENT */
