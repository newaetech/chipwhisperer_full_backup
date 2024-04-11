/***************************************************************************//**
 * @file
 * @brief AES block cipher implementation optimized for Silicon Labs devices
 *        with a Radio AES peripheral.
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
#include "em_device.h"

#if defined(RADIOAES_PRESENT)
#include "radioaes.h"
#include "mbedtls/aes.h"
#include "em_core.h"

#include <stddef.h>
#include <stdbool.h>

#if defined( MBEDTLS_THREADING_C )
#include "mbedtls/threading.h"
static mbedtls_threading_mutex_t    radioaes_lock;
static volatile bool                radioaes_lock_initialized = false;
#endif

int radioaes_acquire ( void )
{
#if defined (_CMU_CLKEN0_MASK)
  CMU->CLKEN0 |= CMU_CLKEN0_RADIOAES;
#endif
  CMU->RADIOCLKCTRL = CMU_RADIOCLKCTRL_EN;
  if ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0U) {
    // IRQ: need to store & restore RADIOAES registers
    while (RADIOAES->STATUS & (AES_STATUS_FETCHERBSY | AES_STATUS_PUSHERBSY | AES_STATUS_SOFTRSTBSY));
    return 1;
  } else {
#if defined(MBEDTLS_THREADING_C)
    // Non-IRQ, RTOS available: take mutex
    /* Initialize mutex if that hasn't happened yet */
    CORE_DECLARE_IRQ_STATE;

    // Check flag first before going into a critical section, to avoid going into
    // a critical section on every single acquire() call. Since the _initialized
    // flag only transitions false -> true, we can in 99% of the calls avoid the
    // critical section.
    if ( !radioaes_lock_initialized ) {
      CORE_ENTER_CRITICAL();

      // Check the flag again after entering the critical section. Now that we're
      // in the critical section, we can be sure that we are the only ones looking
      // at the flag and no-one is interrupting us during its manipulation.
      if ( !radioaes_lock_initialized ) {
        mbedtls_mutex_init( &radioaes_lock );
        radioaes_lock_initialized = true;
      }

      CORE_EXIT_CRITICAL();
    }
    return mbedtls_mutex_lock(&radioaes_lock);
#else
    // Non-IRQ, no RTOS: busywait
    while (RADIOAES->STATUS & (AES_STATUS_FETCHERBSY | AES_STATUS_PUSHERBSY | AES_STATUS_SOFTRSTBSY));
    return 0;
#endif
  }
}

int radioaes_release ( void )
{
  // IRQ: nothing to do
  if ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0U) {
    return 0;
  }
#if defined(MBEDTLS_THREADING_C)
  // Non-IRQ, RTOS available: free mutex
  return mbedtls_mutex_unlock(&radioaes_lock);
#else
  // Non-IRQ, no RTOS: nothing to do.
  return 0;
#endif
}

int radioaes_saveState (radioaes_state_t *ctx)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  ctx->FETCHADDR = RADIOAES->FETCHADDR;
#if defined(_AES_FETCHDESCR_MASK)
  ctx->FETCHDESCR = RADIOAES->FETCHDESCR;
#endif
  ctx->FETCHLEN = RADIOAES->FETCHLEN;
  ctx->FETCHTAG = RADIOAES->FETCHTAG;
  ctx->PUSHADDR = RADIOAES->PUSHADDR;
#if defined(_AES_PUSHDESCR_MASK)
  ctx->PUSHDESCR = RADIOAES->PUSHDESCR;
#endif
  ctx->PUSHLEN = RADIOAES->PUSHLEN;
  ctx->CTRL = RADIOAES->CTRL;
  CORE_EXIT_CRITICAL();
  return 0;
}

int radioaes_restoreState (radioaes_state_t *ctx)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  RADIOAES->FETCHADDR = ctx->FETCHADDR;
#if defined(_AES_FETCHDESCR_MASK)
  RADIOAES->FETCHDESCR = ctx->FETCHDESCR;
#endif
  RADIOAES->FETCHLEN = ctx->FETCHLEN;
  RADIOAES->FETCHTAG = ctx->FETCHTAG;
  RADIOAES->PUSHADDR = ctx->PUSHADDR;
#if defined(_AES_PUSHDESCR_MASK)
  RADIOAES->PUSHDESCR = ctx->PUSHDESCR;
#endif
  RADIOAES->PUSHLEN = ctx->PUSHLEN;
  RADIOAES->CTRL = ctx->CTRL;
  CORE_EXIT_CRITICAL();
  return 0;
}

#endif //defined(RADIOAES_PRESENT)
