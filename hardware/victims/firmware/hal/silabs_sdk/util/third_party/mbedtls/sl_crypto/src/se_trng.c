/***************************************************************************//**
 * @file
 * @brief True Random Number Generator (TRNG) driver for Silicon Labs devices with
 *        Secure Element.
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

#include "em_device.h"
#include "em_se.h"
#include "se_management.h"

#if defined(SEMAILBOX_PRESENT)

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT) && defined(MBEDTLS_ENTROPY_C)

#include "mbedtls/entropy.h"

int mbedtls_hardware_poll( void *data,
                           unsigned char *output,
                           size_t len,
                           size_t *olen ) {
    SE_Response_t ret = SE_RESPONSE_OK;
    size_t surplus_bytes = 0;
    uint32_t surplus_word = 0;
    (void) data;

    surplus_bytes = len & 0x3;
    len &= ~0x3;

    if ( len > 0 ) {
      SE_Command_t command = SE_COMMAND_DEFAULT(SE_COMMAND_TRNG_GET_RANDOM);
      SE_DataTransfer_t data_out = SE_DATATRANSFER_DEFAULT(output, len);

      SE_addDataOutput(&command, &data_out);
      SE_addParameter(&command, len);

      int status = se_management_acquire();
      if (status != 0) {
          return status;
      }

      SE_executeCommand(&command);
      ret = SE_readCommandResponse();

      se_management_release();

      if ( ret == SE_RESPONSE_OK ) {
        *olen = len;
      }
    }

    if ( surplus_bytes != 0 ) {
      SE_Command_t command2 = SE_COMMAND_DEFAULT(SE_COMMAND_TRNG_GET_RANDOM);
      SE_DataTransfer_t word_out = SE_DATATRANSFER_DEFAULT(&surplus_word, 4);

      SE_addDataOutput(&command2, &word_out);
      SE_addParameter(&command2, 4);

      int status = se_management_acquire();
      if (status != 0) {
          return status;
      }

      SE_executeCommand(&command2);
      ret = SE_readCommandResponse();

      se_management_release();

      if ( ret == SE_RESPONSE_OK ) {
        for ( size_t i = 0; i < surplus_bytes; i++ ) {
          output[len+i] = (surplus_word >> (i*8)) & 0xFF;
        }

        *olen = len + surplus_bytes;
      }
    }

    if (ret == SE_RESPONSE_OK) {
      return 0;
    } else {
      return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    }
}

#endif /* MBEDTLS_ENTROPY_HARDWARE_ALT && MBEDTLS_ENTROPY_C */

#endif /* SEMAILBOX_PRESENT */
