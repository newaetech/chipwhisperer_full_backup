/***************************************************************************//**
 * @file
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
 * This file includes an alternative implementation of ECDH using the secure
 * element incorporated in MCU devices from Silicon Laboratories.
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/ecdh.h"
#include "mbedtls/ecdsa.h"

#if defined(MBEDTLS_ECP_C)

#if defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT) \
    || defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT) \
    || defined(MBEDTLS_ECDSA_GENKEY_ALT) \
    || defined(MBEDTLS_ECDSA_VERIFY_ALT) \
    || defined(MBEDTLS_ECDSA_SIGN_ALT)

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT)
#include "em_se.h"
#include "se_management.h"

#define SE_ECP_MAX_BYTES ((((MBEDTLS_ECP_MAX_BYTES) + 3) / 4) * 4)

#if defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT) \
    || defined(MBEDTLS_ECDSA_GENKEY_ALT)
static int ecc_keygen(mbedtls_ecp_group *grp, mbedtls_mpi *d, mbedtls_ecp_point *Q)
{
    uint32_t pub[SE_ECP_MAX_BYTES*2/sizeof(uint32_t)] = {0};
    uint32_t priv[SE_ECP_MAX_BYTES/sizeof(uint32_t)] = {0};

    uint32_t keyspec = (1<<14) /* Generate private key */
                     | (1<<13); /* Generate public key */
    uint32_t keylen = 0;
    uint32_t offset = 0;
    bool montgomery = false;

    switch(grp->id) {
        case MBEDTLS_ECP_DP_SECP192R1:
            keyspec |= 0x80000000; /* Prime curve */
            keyspec |= 23UL; /* curve byte size - 1 */
            keylen = 24;
            break;
        case MBEDTLS_ECP_DP_SECP256R1:
            keyspec |= 0x80000000; /* Prime curve */
            keyspec |= 31UL; /* curve byte size - 1 */
            keylen = 32;
            break;
        case MBEDTLS_ECP_DP_SECP384R1:
            keyspec |= 0x80000000; /* Prime curve */
            keyspec |= 47UL; /* curve byte size - 1 */
            keylen = 48;
            break;
        case MBEDTLS_ECP_DP_SECP521R1:
            keyspec |= 0x80000000; /* Prime curve */
            keyspec |= 65UL; /* curve byte size - 1 */
            keylen = 68;
            offset = 2;
            break;
        case MBEDTLS_ECP_DP_CURVE25519:
            keyspec |= 0xB0000000; /* Montgomery curve */
            keyspec |= 31UL; /* curve byte size - 1 */
            keylen = 32;
            montgomery = true;
            break;
        default:
            return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    if (keylen-offset > (MBEDTLS_ECP_MAX_BITS+7)/8) {
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    /* SE command structures */
    SE_Command_t command = SE_COMMAND_DEFAULT(SE_COMMAND_CREATE_KEY);
    SE_DataTransfer_t domain_in = SE_DATATRANSFER_DEFAULT(NULL, 0);
    SE_DataTransfer_t pubkey_out = SE_DATATRANSFER_DEFAULT(pub, (montgomery ? keylen : 2*keylen));
    SE_DataTransfer_t privkey_out = SE_DATATRANSFER_DEFAULT(priv, keylen);

    SE_addDataInput(&command, &domain_in);
    SE_addDataOutput(&command, &pubkey_out);
    SE_addDataOutput(&command, &privkey_out);

    SE_addParameter(&command, keyspec);

    int status = se_management_acquire();
    if (status != 0) {
        return status;
    }

    SE_executeCommand(&command);
    SE_Response_t res = SE_readCommandResponse();

    se_management_release();

    if ( res == SE_RESPONSE_OK ) {
        mbedtls_mpi_read_binary(d, ((uint8_t*)priv)+offset, keylen-offset);
        mbedtls_mpi_read_binary(&Q->X, ((uint8_t*)pub)+offset, keylen-offset);
        if (!montgomery) {
            mbedtls_mpi_read_binary(&Q->Y, ((uint8_t*)&pub[keylen/sizeof(uint32_t)])+offset, keylen-offset);
        }
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
    /* SE uses internal entropy */
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
    /* SE uses internal entropy */
    (void)f_rng;
    (void)p_rng;

    uint32_t priv[SE_ECP_MAX_BYTES/sizeof(uint32_t)] = {0};
    uint32_t signature[SE_ECP_MAX_BYTES*2/sizeof(uint32_t)] = {0};

    uint32_t keyspec = (1<<14) | (1 << 10); /* Supply private signing key */
    uint32_t keylen = 0;
    uint32_t offset = 0;

    /* Make sure d is in range 1..n-1 */
    if( mbedtls_mpi_cmp_int( d, 1 ) < 0 || mbedtls_mpi_cmp_mpi( d, &grp->N ) >= 0 )
        return( MBEDTLS_ERR_ECP_INVALID_KEY );

    switch(grp->id) {
        case MBEDTLS_ECP_DP_SECP192R1:
            keyspec |= 0x80000000; /* Prime curve */
            keyspec |= 23UL; /* curve byte size - 1 */
            keylen = 24;
            break;
        case MBEDTLS_ECP_DP_SECP256R1:
            keyspec |= 0x80000000; /* Prime curve */
            keyspec |= 31UL; /* curve byte size - 1 */
            keylen = 32;
            break;
        case MBEDTLS_ECP_DP_SECP384R1:
            keyspec |= 0x80000000; /* Prime curve */
            keyspec |= 47UL; /* curve byte size - 1 */
            keylen = 48;
            break;
        case MBEDTLS_ECP_DP_SECP521R1:
            keyspec |= 0x80000000; /* Prime curve */
            keyspec |= 65UL; /* curve byte size - 1 */
            keylen = 68;
            offset = 2;
            break;
        default:
            return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    /* pull out key info from mbedtls structures */
    mbedtls_mpi_write_binary(d, ((uint8_t*)priv), keylen);

    /* SE command structures */
    SE_Command_t command = SE_COMMAND_DEFAULT(SE_COMMAND_SIGNATURE_SIGN);
    SE_DataTransfer_t privkey_in = SE_DATATRANSFER_DEFAULT(priv, keylen);
    SE_DataTransfer_t hash_in = SE_DATATRANSFER_DEFAULT((void*)buf, blen);
    SE_DataTransfer_t signature_out = SE_DATATRANSFER_DEFAULT(signature, keylen*2);

    SE_addDataInput(&command, &privkey_in);
    SE_addDataInput(&command, &hash_in);
    SE_addDataOutput(&command, &signature_out);

    SE_addParameter(&command, keyspec);
    SE_addParameter(&command, blen);

    int status = se_management_acquire();
    if (status != 0) {
        return status;
    }

    SE_executeCommand(&command);
    SE_Response_t res = SE_readCommandResponse();

    se_management_release();

    if ( res == SE_RESPONSE_OK ) {
        mbedtls_mpi_read_binary(r, ((uint8_t*)signature)+offset, keylen-offset);
        mbedtls_mpi_read_binary(s, ((uint8_t*)&signature[keylen/sizeof(uint32_t)])+offset, keylen-offset);
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
    uint32_t pub[SE_ECP_MAX_BYTES*2/sizeof(uint32_t)] = {0};
    uint32_t signature[SE_ECP_MAX_BYTES*2/sizeof(uint32_t)] = {0};

    uint32_t keyspec = (1<<13) | (1 << 10); /* Supply public signing key */
    uint32_t keylen = 0;

    switch(grp->id) {
        case MBEDTLS_ECP_DP_SECP192R1:
            keyspec |= 0x80000000; /* Prime curve */
            keyspec |= 23UL; /* curve byte size - 1 */
            keylen = 24;
            break;
        case MBEDTLS_ECP_DP_SECP256R1:
            keyspec |= 0x80000000; /* Prime curve */
            keyspec |= 31UL; /* curve byte size - 1 */
            keylen = 32;
            break;
        case MBEDTLS_ECP_DP_SECP384R1:
            keyspec |= 0x80000000; /* Prime curve */
            keyspec |= 47UL; /* curve byte size - 1 */
            keylen = 48;
            break;
        case MBEDTLS_ECP_DP_SECP521R1:
            keyspec |= 0x80000000; /* Prime curve */
            keyspec |= 65UL; /* curve byte size - 1 */
            keylen = 68;
            break;
        default:
            return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    /* pull out signature info from mbedtls structures */
    mbedtls_mpi_write_binary(r, ((uint8_t*)signature), keylen);
    mbedtls_mpi_write_binary(s, ((uint8_t*)&signature[keylen/sizeof(uint32_t)]), keylen);

    mbedtls_mpi_write_binary(&Q->X, ((uint8_t*)pub), keylen);
    mbedtls_mpi_write_binary(&Q->Y, ((uint8_t*)&pub[keylen/sizeof(uint32_t)]), keylen);

    /* SE command structures */
    SE_Command_t command = SE_COMMAND_DEFAULT(SE_COMMAND_SIGNATURE_VERIFY);
    SE_DataTransfer_t pubkey_in = SE_DATATRANSFER_DEFAULT(pub, keylen*2);
    SE_DataTransfer_t signature_in = SE_DATATRANSFER_DEFAULT(signature, keylen*2);
    SE_DataTransfer_t hash_in = SE_DATATRANSFER_DEFAULT((void*)buf, blen);

    SE_addDataInput(&command, &pubkey_in);
    SE_addDataInput(&command, &hash_in);
    SE_addDataInput(&command, &signature_in);

    SE_addParameter(&command, keyspec);
    SE_addParameter(&command, blen);

    int status = se_management_acquire();
    if (status != 0) {
        return status;
    }

    SE_executeCommand(&command);
    SE_Response_t res = SE_readCommandResponse();

    se_management_release();

    if ( res == SE_RESPONSE_OK ) {
        return 0;
    } else if (res == SE_RESPONSE_INVALID_SIGNATURE) {
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
    /* SE uses internal entropy */
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
    /* ECDH key generation through regular keygen command */

    /* SE uses internal entropy */
    (void)f_rng;
    (void)p_rng;

    uint32_t pub[SE_ECP_MAX_BYTES*2/sizeof(uint32_t)] = {0};
    uint32_t priv[SE_ECP_MAX_BYTES/sizeof(uint32_t)] = {0};

    int ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    uint32_t keyspec = (1<<14); /* Use private key */
    uint32_t keylen = 0;
    uint32_t offset = 0;
    bool montgomery = false;

    switch(grp->id) {
        case MBEDTLS_ECP_DP_SECP192R1:
            keyspec |= 0x80000000UL; /* Prime curve */
            keyspec |= 23UL; /* curve byte size - 1 */
            keylen = 24;
            break;
        case MBEDTLS_ECP_DP_SECP256R1:
            keyspec |= 0x80000000UL; /* Prime curve */
            keyspec |= 31UL; /* curve byte size - 1 */
            keylen = 32;
            break;
        case MBEDTLS_ECP_DP_SECP384R1:
            keyspec |= 0x80000000; /* Prime curve */
            keyspec |= 47UL; /* curve byte size - 1 */
            keylen = 48;
            break;
        case MBEDTLS_ECP_DP_SECP521R1:
            keyspec |= 0x80000000; /* Prime curve */
            keyspec |= 65UL; /* curve byte size - 1 */
            keylen = 68;
            offset = 2;
            break;
        case MBEDTLS_ECP_DP_CURVE25519:
            keyspec |= 0xB0000000; /* Montgomery curve */
            keyspec |= 31UL; /* curve byte size - 1 */
            keylen = 32;
            montgomery = true;
            break;
        default:
            return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    if (keylen-offset > (MBEDTLS_ECP_MAX_BITS+7)/8) {
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    ret = mbedtls_ecp_check_pubkey( grp, Q );
    if (ret != 0) {
        return ret;
    }

    /* pull out key info from mbedtls structures */
    mbedtls_mpi_write_binary(d, ((uint8_t*)priv), keylen);
    mbedtls_mpi_write_binary(&Q->X, ((uint8_t*)pub), keylen);
    mbedtls_mpi_write_binary(&Q->Y, ((uint8_t*)pub)+keylen, keylen);

    /* SE command structures */
    SE_Command_t command = SE_COMMAND_DEFAULT(SE_COMMAND_DH);
    SE_DataTransfer_t privkey_in = SE_DATATRANSFER_DEFAULT(priv, keylen);
    SE_DataTransfer_t pubkey_in = SE_DATATRANSFER_DEFAULT(pub, (montgomery ? keylen : 2*keylen));
    SE_DataTransfer_t privkey_out = SE_DATATRANSFER_DEFAULT(pub, (montgomery ? keylen : 2*keylen));

    SE_addDataInput(&command, &privkey_in);
    SE_addDataInput(&command, &pubkey_in);
    SE_addDataOutput(&command, &privkey_out);

    SE_addParameter(&command, keyspec);
    SE_addParameter(&command, (montgomery ? keylen : 2*keylen)); /* DH key output type must be RAW */

    int status = se_management_acquire();
    if (status != 0) {
        return status;
    }

    SE_executeCommand(&command);
    SE_Response_t res = SE_readCommandResponse();

    se_management_release();

    if ( res == SE_RESPONSE_OK ) {
        mbedtls_mpi_read_binary(z, ((uint8_t*)pub)+offset, keylen-offset);
        return 0;
    } else {
        return MBEDTLS_ERR_ECP_HW_ACCEL_FAILED;
    }
}
#endif /* #if defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT) */

#endif /* #if defined(SEMAILBOX_PRESENT) */

#endif /* #if defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT) || defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT) */

#endif /* #if defined(MBEDTLS_ECP_C) */
