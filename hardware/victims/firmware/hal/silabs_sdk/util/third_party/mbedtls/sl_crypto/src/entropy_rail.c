/***************************************************************************//**
 * @file
 * @brief Collect entropy from the RAIL API on Silicon Labs radios.
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

#if defined(MBEDTLS_ENTROPY_RAIL_C)

#include "em_device.h"

#if defined(_EFR_DEVICE)

#include "entropy_rail.h"
#include "rail.h"
#include <string.h>

void mbedtls_entropy_rail_init( mbedtls_entropy_rail_context *ctx )
{
    memset(ctx, 0, sizeof(mbedtls_entropy_rail_context));
}

void mbedtls_entropy_rail_free( mbedtls_entropy_rail_context *ctx )
{
    memset(ctx, 0, sizeof(mbedtls_entropy_rail_context));
}

int mbedtls_entropy_rail_poll( mbedtls_entropy_rail_context *ctx,
			      unsigned char *output, size_t len, size_t *olen )
{
    (void) ctx;

    if ( len > UINT16_MAX )
    {
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    }

    *olen = (size_t) RAIL_GetRadioEntropy(RAIL_EFR32_HANDLE,
					  (uint8_t*) output,
					  (uint16_t) len);
    return 0;
}

#if defined (MBEDTLS_ENTROPY_C) && \
  defined(MBEDTLS_ENTROPY_HARDWARE_ALT) && \
  defined(MBEDTLS_ENTROPY_HARDWARE_ALT_RAIL)

int mbedtls_hardware_poll( void *data,
                           unsigned char *output,
                           size_t len,
                           size_t *olen )
{
    return mbedtls_entropy_rail_poll((mbedtls_entropy_rail_context*)data,
             output, len, olen);
}

#endif /* #if defined(MBEDTLS_ENTROPY_C) && ... */


#endif /* #if defined(_EFR_DEVICE) */

#endif /* #if defined(MBEDTLS_ENTROPY_RAIL_C) */
