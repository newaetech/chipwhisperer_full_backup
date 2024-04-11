/***************************************************************************//**
 * @brief SE-accelerated elliptic curve (Diffie-Helman) operations
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
/*
 * This file includes an alternative implementation of ECDH using the
 * CRYPTOACC incorporated in MCU devices from Silicon Laboratories.
 */

#include "em_device.h"

#if defined(CRYPTOACC_PRESENT)

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ECP_C)

#if defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT) \
    || defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT) \
    || defined(MBEDTLS_ECDSA_GENKEY_ALT) \
    || defined(MBEDTLS_ECDSA_VERIFY_ALT) \
    || defined(MBEDTLS_ECDSA_SIGN_ALT)
#include "cryptoacc_management.h"
#include "ba414ep_config.h"
#include "sx_ecc_keygen_alg.h"
#include "sx_dh_alg.h"
#include "sx_ecc_curves.h"
#include "sx_errors.h"
#include "sx_trng.h"
#include "sx_ecdsa_alg.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/ecdsa.h"

#define CRYPTOACC_ECP_MAX_BYTES ((((MBEDTLS_ECP_MAX_BYTES) + 3) / 4) * 4)

#if defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT) \
    || defined(MBEDTLS_ECDSA_GENKEY_ALT)
static int ecc_keygen(mbedtls_ecp_group *grp, mbedtls_mpi *d, mbedtls_ecp_point *Q)
{
    int status;
    uint32_t res;
    uint32_t keylen;
    uint32_t curve_flags;
    block_t  _domain;
    block_t  _priv;
    block_t  _pub;
    uint32_t pub[CRYPTOACC_ECP_MAX_BYTES*2/sizeof(uint32_t)] = { 0 };
    uint32_t priv[CRYPTOACC_ECP_MAX_BYTES/sizeof(uint32_t)] = { 0 };
    struct sx_rng trng = { NULL, sx_trng_fill_blk };

    if ( grp == NULL || d == NULL || Q == NULL ) {
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    }

    switch(grp->id) {
        case MBEDTLS_ECP_DP_SECP192R1:
            keylen       = sx_ecc_curve_p192.bytesize;
            curve_flags  = sx_ecc_curve_p192.pk_flags;
            _domain = block_t_convert(sx_ecc_curve_p192.params.addr, 6*keylen);
            break;

        case MBEDTLS_ECP_DP_SECP224R1:
            keylen       = sx_ecc_curve_p224.bytesize;
            curve_flags  = sx_ecc_curve_p224.pk_flags;
            _domain = block_t_convert(sx_ecc_curve_p224.params.addr, 6*keylen);
            break;

        case MBEDTLS_ECP_DP_SECP256R1:
            keylen       = sx_ecc_curve_p256.bytesize;
            curve_flags  = sx_ecc_curve_p256.pk_flags;
            _domain = block_t_convert(sx_ecc_curve_p256.params.addr, 6*keylen);
            break;

        default:
            return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    if (keylen > (MBEDTLS_ECP_MAX_BITS+7)/8) {
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    status = cryptoacc_trng_initialize();
    if (status != 0) {
        return status;
    }

    _priv = block_t_convert(priv, keylen);
    _pub = block_t_convert(pub, keylen * 2);

    status = cryptoacc_management_acquire();
    if (status != 0) {
        return status;
    }
    res = ecc_generate_keypair(_domain, _pub, _priv, keylen, curve_flags, trng);
    cryptoacc_management_release();

    if ( res == CRYPTOLIB_SUCCESS ) {
        mbedtls_mpi_read_binary(d, _priv.addr, keylen);
        mbedtls_mpi_read_binary(&Q->X, _pub.addr, keylen);
        mbedtls_mpi_read_binary(&Q->Y, &(_pub.addr[keylen]), keylen);
        mbedtls_mpi_lset(&Q->Z, 1);
        return 0;
    } else {
        return MBEDTLS_ERR_ECP_HW_ACCEL_FAILED;
    }
}
#endif /* #if defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT)
          || defined(MBEDTLS_ECDSA_GENKEY_ALT) */

#if defined(MBEDTLS_ECDSA_GENKEY_ALT)
/*
 * Generate key pair
 */
int mbedtls_ecdsa_genkey( mbedtls_ecdsa_context *ctx, mbedtls_ecp_group_id gid,
                  int (*f_rng)(void *, unsigned char *, size_t), void *p_rng )
{
    /* CRYPTOACC uses internal entropy */
    (void)f_rng;
    (void)p_rng;

    mbedtls_ecp_group_load( &ctx->grp, gid );

    return ecc_keygen(&ctx->grp, &ctx->d, &ctx->Q);
}
#endif /* MBEDTLS_ECDSA_GENKEY_ALT */

#if defined(MBEDTLS_ECDSA_SIGN_ALT)
int mbedtls_ecdsa_sign( mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                int (*f_rng)(void *, unsigned char *, size_t), void *p_rng )
{
    /* CRYPTOACC uses internal entropy */
    (void)f_rng;
    (void)p_rng;

    if ( d == NULL || buf == NULL || grp == NULL || s == NULL || r == NULL) {
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    }
    int status;
    uint32_t res;
    uint32_t keylen = 0;

    struct sx_rng trng = { NULL, sx_trng_fill_blk };

    uint32_t priv[CRYPTOACC_ECP_MAX_BYTES/sizeof(uint32_t)] = { 0 };
    uint32_t signature[CRYPTOACC_ECP_MAX_BYTES*2/sizeof(uint32_t)] = { 0 };

    block_t _priv = { 0 };
    block_t _digest = { 0 };
    block_t _signature = { 0 };

    /* Make sure d is in range 1..n-1 */
    if( mbedtls_mpi_cmp_int( d, 1 ) < 0 || mbedtls_mpi_cmp_mpi( d, &grp->N ) >= 0 )
        return( MBEDTLS_ERR_ECP_INVALID_KEY );

    switch(grp->id) {
        case MBEDTLS_ECP_DP_SECP192R1:
            keylen       = 24;
            break;

        case MBEDTLS_ECP_DP_SECP224R1:
            keylen       = 28;
            break;

        case MBEDTLS_ECP_DP_SECP256R1:
            keylen       = 32;
            break;

        default:
            return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    if (keylen > CRYPTOACC_ECP_MAX_BYTES) {
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    mbedtls_mpi_write_binary(d, ((uint8_t*)priv), keylen);

    _priv = block_t_convert(priv, keylen);
    _digest = block_t_convert(buf, blen);
    _signature = block_t_convert(signature, keylen*2);

    status = cryptoacc_management_acquire();
    if (status != 0) {
        return status;
    }

    switch(keylen) {
        case 24:
            res = ecdsa_generate_signature_digest(&sx_ecc_curve_p192, _digest, _priv, _signature, trng);
            break;

        case 28:
            res = ecdsa_generate_signature_digest(&sx_ecc_curve_p224, _digest, _priv, _signature, trng);
            break;

        case 32:
            res = ecdsa_generate_signature_digest(&sx_ecc_curve_p256, _digest, _priv, _signature, trng);
            break;

        default:
            cryptoacc_management_release();
            return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }
    cryptoacc_management_release();

    if (res == CRYPTOLIB_SUCCESS) {
        mbedtls_mpi_read_binary(r, _signature.addr, keylen);
        mbedtls_mpi_read_binary(s, &(_signature.addr[keylen]), keylen);
        return 0;
    } else {
        return MBEDTLS_ERR_ECP_HW_ACCEL_FAILED;
    }
}
#endif /* MBEDTLS_ECDSA_SIGN_ALT */

#if defined(MBEDTLS_ECDSA_VERIFY_ALT)
int mbedtls_ecdsa_verify( mbedtls_ecp_group *grp,
                  const unsigned char *buf, size_t blen,
                  const mbedtls_ecp_point *Q, const mbedtls_mpi *r, const mbedtls_mpi *s)
{
    if ( buf == NULL || grp == NULL || s == NULL || r == NULL || Q == NULL ) {
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    }

    /* Make sure r and s is in range 1..n-1 */
    if( mbedtls_mpi_cmp_int( r, 1 ) < 0 || mbedtls_mpi_cmp_mpi( r, &grp->N ) >= 0 )
        return( MBEDTLS_ERR_ECP_VERIFY_FAILED );

    if( mbedtls_mpi_cmp_int( s, 1 ) < 0 || mbedtls_mpi_cmp_mpi( s, &grp->N ) >= 0 )
        return( MBEDTLS_ERR_ECP_VERIFY_FAILED );

    int status;
    uint32_t res;
    uint32_t pub[CRYPTOACC_ECP_MAX_BYTES*2/sizeof(uint32_t)] = { 0 };
    uint32_t signature[CRYPTOACC_ECP_MAX_BYTES*2/sizeof(uint32_t)] = { 0 };

    block_t  _pub = { 0 };
    block_t  _signature = { 0 };
    block_t  _digest = { 0 };

    uint32_t keylen = 0;

    switch(grp->id) {
        case MBEDTLS_ECP_DP_SECP192R1:
            keylen       = 24;
            break;

        case MBEDTLS_ECP_DP_SECP224R1:
            keylen       = 28;
            break;

        case MBEDTLS_ECP_DP_SECP256R1:
            keylen       = 32;
            break;

        default:
            return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    if (keylen > CRYPTOACC_ECP_MAX_BYTES) {
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    /* pull out signature info from mbedtls structures */
    mbedtls_mpi_write_binary(r, ((uint8_t*)signature), keylen);
    mbedtls_mpi_write_binary(s, ((uint8_t*)signature)+keylen, keylen);

    mbedtls_mpi_write_binary(&Q->X, ((uint8_t*)pub), keylen);
    mbedtls_mpi_write_binary(&Q->Y, ((uint8_t*)&pub)+keylen, keylen);

    _pub = block_t_convert(pub, keylen * 2);
    _signature = block_t_convert(signature, keylen * 2);
    _digest = block_t_convert(buf, blen);

    status = cryptoacc_management_acquire();
    if (status != 0) {
        return status;
    }
    switch(keylen) {
        case 24:
            res = ecdsa_verify_signature_digest(&sx_ecc_curve_p192, _digest, _pub, _signature);
            break;

        case 28:
            res = ecdsa_verify_signature_digest(&sx_ecc_curve_p224, _digest, _pub, _signature);
            break;

        case 32:
            res = ecdsa_verify_signature_digest(&sx_ecc_curve_p256, _digest, _pub, _signature);
            break;

        default:
            cryptoacc_management_release();
            return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }
    cryptoacc_management_release();

    if (res == CRYPTOLIB_SUCCESS) {
        return 0;
    } else if (res == CRYPTOLIB_INVALID_SIGN_ERR) {
        return MBEDTLS_ERR_ECP_VERIFY_FAILED;
    } else {
        return MBEDTLS_ERR_ECP_HW_ACCEL_FAILED;
    }
}
#endif /* MBEDTLS_ECDSA_VERIFY_ALT */

#if defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT)
int mbedtls_ecdh_gen_public( mbedtls_ecp_group *grp, mbedtls_mpi *d, mbedtls_ecp_point *Q,
                     int (*f_rng)(void *, unsigned char *, size_t),
                     void *p_rng )
{
    /* CRYPTOACC uses internal entropy */
    (void)f_rng;
    (void)p_rng;

    return ecc_keygen(grp, d, Q);
}
#endif /* #if defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT) */

#if defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT)
int mbedtls_ecdh_compute_shared( mbedtls_ecp_group *grp, mbedtls_mpi *z,
                         const mbedtls_ecp_point *Q, const mbedtls_mpi *d,
                         int (*f_rng)(void *, unsigned char *, size_t),
                         void *p_rng )
{
    int      status;
    uint32_t res;
    uint32_t keylen;
    uint32_t curve_flags;
    uint32_t pub[CRYPTOACC_ECP_MAX_BYTES*2/sizeof(uint32_t)] = {0};
    uint32_t priv[CRYPTOACC_ECP_MAX_BYTES/sizeof(uint32_t)] = {0};
    uint32_t zbuf[CRYPTOACC_ECP_MAX_BYTES*2/sizeof(uint32_t)] = {0};
    block_t  _domain;
    block_t  _priv;
    block_t  _pub;
    block_t  _z;

    (void)p_rng;
    if (f_rng) {
        /* CRYPTOACC on EFR32XG22 does not support randomization of
           projective coordinates (counter-measure against timing attacks). */
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    }

    if ( grp == NULL || z == NULL || d == NULL || Q == NULL ) {
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    }

    switch(grp->id) {
        case MBEDTLS_ECP_DP_SECP192R1:
            keylen       = sx_ecc_curve_p192.bytesize;
            curve_flags  = sx_ecc_curve_p192.pk_flags;
            _domain = block_t_convert(sx_ecc_curve_p192.params.addr, 6*keylen);
            break;

        case MBEDTLS_ECP_DP_SECP224R1:
            keylen       = sx_ecc_curve_p224.bytesize;
            curve_flags  = sx_ecc_curve_p224.pk_flags;
            _domain = block_t_convert(sx_ecc_curve_p224.params.addr, 6*keylen);
            break;

        case MBEDTLS_ECP_DP_SECP256R1:
            keylen       = sx_ecc_curve_p256.bytesize;
            curve_flags  = sx_ecc_curve_p256.pk_flags;
            _domain = block_t_convert(sx_ecc_curve_p256.params.addr, 6*keylen);
            break;

        default:
            return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    if (keylen > (MBEDTLS_ECP_MAX_BITS+7)/8) {
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    /* pull out key info from mbedtls structures */
    mbedtls_mpi_write_binary(d, ((uint8_t*)priv), keylen);
    mbedtls_mpi_write_binary(&Q->X, ((uint8_t*)pub), keylen);
    mbedtls_mpi_write_binary(&Q->Y, ((uint8_t*)pub)+keylen, keylen);

    _priv = block_t_convert(priv, keylen);
    _pub = block_t_convert(pub, keylen * 2);
    _z = block_t_convert(zbuf, keylen * 2);

    status = cryptoacc_management_acquire();
    if (status != 0) {
        return status;
    }
    res = dh_shared_key_ecdh(_domain, _priv, _pub, _z, keylen, curve_flags);
    cryptoacc_management_release();

    if ( res == CRYPTOLIB_SUCCESS ) {
        mbedtls_mpi_read_binary(z, _z.addr, keylen);
        return 0;
    } else {
        return MBEDTLS_ERR_ECP_HW_ACCEL_FAILED;
    }
}
#endif /* #if defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT) */

#endif /* #if defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT) || defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT) */

#endif // #if defined(MBEDTLS_ECP_C)

#endif /* #if defined(CRYPTOACC_PRESENT) */
