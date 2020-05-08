/***************************************************************************//**
 * @file mbedtls_ecode.h
 * @brief Silicon Labs specific mbedtls error code definitions.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef MBEDTLS_ECODE_H
#define MBEDTLS_ECODE_H

/**
 * This file defines the Silicon Labs specific mbedtls error code layout
 * and values.
 *
 * Error code layout.
 *
 * The standard mbedtls error code layout use the negative space of 16 bits
 * signed integers in order to support all architectures. However the standard
 * mbedtls error code layout is fixed by ARM and does not define a sub-space
 * for additional vendor specific error codes. Additionally the 16 bit
 * requirement is not relevant for the Silicon Labs 32bit MCUs. Therefore
 * Silicon Labs defines a 32 bit signed integer error code layout which does
 * not use the values of the standard mbedtls error code layout given that the
 * 16 most significant bits of standard mbedtls error codes are all ones (which
 * should be the case when running on 32-bit architectures and the application
 * does not strip off the 16 most significant bits):
 *
 *   bit 31:      Sign bit. 1 to signify that the value is part of the error
 *                code space. 0 for non error codes, in which case all 32 bits
 *                are 0 which means OK (i.e. operation succeded).
 *   bits 24-30:  sub-system id, for example mbedtls, emdrv, etc
 *   bits 16-23:  module id, for example UART, LCD,...
 *   bits 0-15:   error code, specific error code
 *
 * The sub-system id and module id are not allowed to be all ones.
 *
 * Below is the list of module ids for the mbedtls sub-system.
 * Module specific error codes are defined in the header file of
 * each respective module.
 */

#define MBEDTLS_ECODE_BASE  ( 0xF1000000 )  /*!< Base value for all error codes. */

#define MBEDTLS_ERR_TRNG_BASE        ( MBEDTLS_ECODE_BASE | 0x0000E000 ) /**< Base value of error codes for TRNG submodule. */
#define MBEDTLS_ERR_CRYPTOACC_BASE   ( MBEDTLS_ECODE_BASE | 0x00001000 ) /**< Base value of error codes for CRYPTOACC submodule. */

#endif /* #ifndef MBEDTLS_ECODE_H */
