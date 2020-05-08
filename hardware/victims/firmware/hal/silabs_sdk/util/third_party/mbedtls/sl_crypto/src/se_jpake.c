/***************************************************************************//**
 * @file
 * @brief ECC J-PAKE accelerated implementation
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
 * This file includes an alternative implementation of the standard
 * mbedtls/libary/ecjpake.c using the secure element incorporated in (W)MCU
 * devices from Silicon Laboratories.
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/ecjpake.h"

#if defined(MBEDTLS_ECJPAKE_ALT) && defined(MBEDTLS_ECJPAKE_C)

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT)
#include "em_se.h"
#include "se_management.h"
#include <string.h>

static const char * const ecjpake_id[] = {
    "client",
    "server"
};

static int parse_tls_point(const uint8_t **ibuf, size_t *ilen, size_t *rlen,
                           uint8_t **obuf, size_t *olen)
{
    if (ilen == NULL || ibuf == NULL || obuf == NULL || olen == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (*ilen == 0 || *ibuf == NULL || *obuf == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    // consume first byte, length of what follows
    size_t field_length = **ibuf;
    *ibuf += 1;
    *ilen -= 1;
    if (rlen != NULL) {
        *rlen += 1;
    }

    if (field_length > *ilen) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    // consume second byte, point type
    uint8_t point_type = **ibuf;
    size_t point_length = field_length - 1;
    *ibuf += 1;
    *ilen -= 1;

    switch (point_type) {
        case 0x0:
            // Why would we ever get a zero-point?
            return MBEDTLS_ERR_ECP_INVALID_KEY;
        case 0x04:
            break;
        case 0x05:
            // We don't support compressed points...
            return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
        default:
            return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    // copy out binary point
    if (point_length > *olen) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    memcpy(*obuf, *ibuf, point_length);
    *ibuf += point_length;
    *ilen -= point_length;
    *obuf += point_length;
    *olen -= point_length;
    if (rlen != NULL) {
        *rlen += field_length;
    }

    return 0;
}

static int parse_tls_zkp(const uint8_t **ibuf, size_t *ilen, size_t *rlen,
                         uint8_t **obuf, size_t *olen)
{
    if (ilen == NULL || ibuf == NULL || obuf == NULL || olen == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (*ilen == 0 || *ibuf == NULL || *obuf == NULL || *olen < 96) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    int ret = parse_tls_point(ibuf, ilen, rlen, obuf, olen);
    if (ret != 0) {
        return ret;
    }

    if (*ilen < 1) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    // consume first byte, length of what follows
    size_t field_length = **ibuf;
    *ibuf += 1;
    *ilen -= 1;
    if (rlen != NULL) {
        *rlen += 1;
    }

    if (field_length > *ilen || field_length > *olen) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (field_length == 0) {
        // scalar cannot be zero
        return MBEDTLS_ERR_ECP_INVALID_KEY;
    }

    // right-adjust
    size_t adjust_length = 32 - field_length;
    memset(*obuf, 0, adjust_length);
    *obuf += adjust_length;
    *olen -= adjust_length;

    // Consume field
    memcpy(*obuf, *ibuf, field_length);
    *obuf += field_length;
    *olen -= field_length;
    *ibuf += field_length;
    *ilen -= field_length;

    if (rlen != NULL) {
        *rlen += field_length;
    }

    return 0;
}

static int write_tls_point(uint8_t **obuf, size_t *olen, size_t *wlen,
                           const uint8_t **ibuf, size_t *ilen, size_t point_length)
{
    if (ibuf == NULL || obuf == NULL || olen == NULL || ilen == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (*obuf == NULL || *ibuf == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    // We can only output uncompressed points here
    if (*olen < point_length + 2) {
        return MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;
    }

    if (*ilen < point_length) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    **obuf = point_length + 1;
    *obuf += 1;
    *olen -= 1;

    **obuf = 0x04;
    *obuf += 1;
    *olen -= 1;

    memcpy(*obuf, *ibuf, point_length);

    *obuf += point_length;
    *olen -= point_length;
    *ibuf += point_length;
    *ilen -= point_length;

    if (wlen != NULL) {
        *wlen += point_length + 2;
    }

    return 0;
}

static int write_tls_zkp(uint8_t **obuf, size_t *olen, size_t *wlen,
                         const uint8_t **ibuf, size_t *ilen, size_t point_length)
{
    int ret = 0;

    if (ibuf == NULL || obuf == NULL || olen == NULL || ilen == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (*obuf == NULL || *ibuf == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    ret = write_tls_point(obuf, olen, wlen, ibuf, ilen, point_length);

    if (ret != 0) {
        return ret;
    }

    size_t zkp_length = 32;

    if (*olen < zkp_length + 1 || *ilen < zkp_length) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    **obuf = zkp_length;
    *obuf += 1;
    *olen -= 1;

    memcpy(*obuf, *ibuf, zkp_length);

    *obuf += zkp_length;
    *olen -= zkp_length;
    *ibuf += zkp_length;
    *ilen -= zkp_length;

    if (wlen != NULL) {
        *wlen += zkp_length + 1;
    }

    return 0;
}

void mbedtls_ecjpake_init( mbedtls_ecjpake_context *ctx )
{
    if (ctx == NULL) {
        return;
    }

    memset(ctx, 0, sizeof(*ctx));
}

int mbedtls_ecjpake_setup( mbedtls_ecjpake_context *ctx,
                           mbedtls_ecjpake_role role,
                           mbedtls_md_type_t hash,
                           mbedtls_ecp_group_id curve,
                           const unsigned char *secret,
                           size_t len )
{
    if (ctx == NULL || (len > 0 && secret == NULL)) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    // SE only supports passphrases of maximum 32 bytes
    if (len > 32) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    // SE currently only supports SHA256 as JPAKE hashing mechanism
    if (hash != MBEDTLS_MD_SHA256) {
        return MBEDTLS_ERR_MD_FEATURE_UNAVAILABLE;
    }

    // SE currently only supports ECDSA secp256r1 as curve
    if (curve != MBEDTLS_ECP_DP_SECP256R1) {
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    ctx->curve_flags = 0x8000001FUL;
    ctx->role = role;
    ctx->pwd_len = len;
    memcpy(ctx->pwd, secret, len);

    return 0;
}

int mbedtls_ecjpake_check( const mbedtls_ecjpake_context *ctx )
{
    if (ctx == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (ctx->curve_flags == 0 || (ctx->pwd_len > 0 && ctx->pwd == NULL)) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    return 0;
}

int mbedtls_ecjpake_write_round_one( mbedtls_ecjpake_context *ctx,
                            unsigned char *buf, size_t len, size_t *olen,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng )
{
    // SE has internal RNG
    (void)f_rng;
    (void)p_rng;

    int ret = 0;

    if (ctx == NULL || buf == NULL || olen == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    // local storage for ZKPs
    uint8_t zkp1[32+64];
    uint8_t zkp2[32+64];

    *olen = 0;

    // SE command structures
    SE_Command_t command = SE_COMMAND_DEFAULT(SE_COMMAND_JPAKE_R1_GENERATE);
    SE_DataTransfer_t domain_in = SE_DATATRANSFER_DEFAULT(NULL, 0);
    SE_DataTransfer_t userid = SE_DATATRANSFER_DEFAULT((void*)ecjpake_id[ctx->role], strlen(ecjpake_id[ctx->role]));
    SE_DataTransfer_t r_out = SE_DATATRANSFER_DEFAULT(ctx->r, 32);
    SE_DataTransfer_t Xm1_out = SE_DATATRANSFER_DEFAULT(ctx->Xm1, 64);
    SE_DataTransfer_t zkp1_out = SE_DATATRANSFER_DEFAULT(zkp1, sizeof(zkp1));
    SE_DataTransfer_t Xm2_out = SE_DATATRANSFER_DEFAULT(ctx->Xm2, 64);
    SE_DataTransfer_t zkp2_out = SE_DATATRANSFER_DEFAULT(zkp2, sizeof(zkp2));

    SE_addDataInput(&command, &domain_in);
    SE_addDataInput(&command, &userid);
    SE_addDataOutput(&command, &r_out);
    SE_addDataOutput(&command, &Xm1_out);
    SE_addDataOutput(&command, &zkp1_out);
    SE_addDataOutput(&command, &Xm2_out);
    SE_addDataOutput(&command, &zkp2_out);

    SE_addParameter(&command, ctx->curve_flags);
    SE_addParameter(&command, strlen(ecjpake_id[ctx->role]));

    int status = se_management_acquire();
    if (status != 0) {
        return status;
    }

    SE_executeCommand(&command);
    SE_Response_t res = SE_readCommandResponse();

    se_management_release();

    if ( res == SE_RESPONSE_OK ) {
        // To write TLS structures of ECJ-PAKE, we need to write:
        // * Xm1
        // * zkp1
        // * Xm2
        // * zkp2
        uint8_t *obuf = buf;
        const uint8_t *ibuf = ctx->Xm1;
        size_t ilen = 64;

        ret = write_tls_point(&obuf, &len, olen, &ibuf, &ilen, 64);
        if (ret != 0) {
            return ret;
        }

        ibuf = zkp1;
        ilen = 96;
        ret = write_tls_zkp(&obuf, &len, olen, &ibuf, &ilen, 64);
        if (ret != 0) {
            return ret;
        }

        ibuf = ctx->Xm2;
        ilen = 64;
        ret = write_tls_point(&obuf, &len, olen, &ibuf, &ilen, 64);
        if (ret != 0) {
            return ret;
        }

        ibuf = zkp2;
        ilen = 96;
        ret = write_tls_zkp(&obuf, &len, olen, &ibuf, &ilen, 64);
        if (ret != 0) {
            return ret;
        }

        return 0;
    } else {
        return MBEDTLS_ERR_ECP_HW_ACCEL_FAILED;
    }
}

int mbedtls_ecjpake_read_round_one( mbedtls_ecjpake_context *ctx,
                                    const unsigned char *buf,
                                    size_t len )
{
    int ret = 0;

    if (ctx == NULL || buf == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    // Should receive 2 binary points and 2 ZKPs

    // local storage for ZKPs
    uint8_t zkp1[32+64] = {0};
    uint8_t zkp2[32+64] = {0};

    uint8_t *obuf = ctx->Xp1;
    size_t olen = 64;

    // Parse structures
    ret = parse_tls_point(&buf, &len, NULL, &obuf, &olen);
    if (ret != 0) {
        return ret;
    }

    obuf = zkp1;
    olen = 96;
    ret = parse_tls_zkp(&buf, &len, NULL, &obuf, &olen);
    if (ret != 0) {
        return ret;
    }

    obuf = ctx->Xp2;
    olen = 64;
    ret = parse_tls_point(&buf, &len, NULL, &obuf, &olen);
    if (ret != 0) {
        return ret;
    }

    obuf = zkp2;
    olen = 96;
    ret = parse_tls_zkp(&buf, &len, NULL, &obuf, &olen);
    if (ret != 0) {
        return ret;
    }

    if (len > 0) {
        // Too much input
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    // SE command structures
    SE_Command_t command = SE_COMMAND_DEFAULT(SE_COMMAND_JPAKE_R1_VERIFY);
    SE_DataTransfer_t domain_in = SE_DATATRANSFER_DEFAULT(NULL, 0);
    SE_DataTransfer_t userid_mine = SE_DATATRANSFER_DEFAULT((void*)ecjpake_id[ctx->role], strlen(ecjpake_id[ctx->role]));
    SE_DataTransfer_t userid_peer = SE_DATATRANSFER_DEFAULT((void*)ecjpake_id[1-ctx->role], strlen(ecjpake_id[1-ctx->role]));

    SE_DataTransfer_t Xp1_in = SE_DATATRANSFER_DEFAULT(ctx->Xp1, 64);
    SE_DataTransfer_t zkp1_in = SE_DATATRANSFER_DEFAULT(zkp1, sizeof(zkp1));
    SE_DataTransfer_t Xp2_in = SE_DATATRANSFER_DEFAULT(ctx->Xp2, 64);
    SE_DataTransfer_t zkp2_in = SE_DATATRANSFER_DEFAULT(zkp2, sizeof(zkp2));

    SE_addDataInput(&command, &domain_in);
    SE_addDataInput(&command, &userid_mine);
    SE_addDataInput(&command, &userid_peer);

    SE_addDataInput(&command, &Xp1_in);
    SE_addDataInput(&command, &zkp1_in);
    SE_addDataInput(&command, &Xp2_in);
    SE_addDataInput(&command, &zkp2_in);

    SE_addParameter(&command, ctx->curve_flags);
    SE_addParameter(&command, strlen(ecjpake_id[ctx->role]));
    SE_addParameter(&command, strlen(ecjpake_id[1-ctx->role]));

    int status = se_management_acquire();
    if (status != 0) {
        return status;
    }

    SE_executeCommand(&command);
    SE_Response_t res = SE_readCommandResponse();

    se_management_release();

    if ( res == SE_RESPONSE_OK ) {
        return 0;
    } else {
        return MBEDTLS_ERR_ECP_VERIFY_FAILED;
    }
}

int mbedtls_ecjpake_write_round_two( mbedtls_ecjpake_context *ctx,
                            unsigned char *buf, size_t len, size_t *olen,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng )
{
    // SE has internal RNG
    (void)f_rng;
    (void)p_rng;

    int ret = 0;

    if (ctx == NULL || buf == NULL || olen == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    *olen = 0;

    uint8_t zkpA[32+64];
    uint8_t xA[64];

    // SE command structures
    SE_Command_t command = SE_COMMAND_DEFAULT(SE_COMMAND_JPAKE_R2_GENERATE);
    SE_DataTransfer_t domain_in = SE_DATATRANSFER_DEFAULT(NULL, 0);
    SE_DataTransfer_t pwd_in = SE_DATATRANSFER_DEFAULT(ctx->pwd, ctx->pwd_len);
    SE_DataTransfer_t userid = SE_DATATRANSFER_DEFAULT((void*)ecjpake_id[ctx->role], strlen(ecjpake_id[ctx->role]));
    SE_DataTransfer_t r_in = SE_DATATRANSFER_DEFAULT(ctx->r, 32);
    SE_DataTransfer_t Xm1_in = SE_DATATRANSFER_DEFAULT(ctx->Xm1, 64);
    SE_DataTransfer_t Xp1_in = SE_DATATRANSFER_DEFAULT(ctx->Xp1, 64);
    SE_DataTransfer_t Xp2_in = SE_DATATRANSFER_DEFAULT(ctx->Xp2, 64);

    SE_DataTransfer_t xA_out = SE_DATATRANSFER_DEFAULT(xA, sizeof(xA));
    SE_DataTransfer_t zkpA_out = SE_DATATRANSFER_DEFAULT(zkpA, sizeof(zkpA));

    SE_addDataInput(&command, &domain_in);
    SE_addDataInput(&command, &pwd_in);
    SE_addDataInput(&command, &userid);
    SE_addDataInput(&command, &r_in);
    SE_addDataInput(&command, &Xm1_in);
    SE_addDataInput(&command, &Xp1_in);
    SE_addDataInput(&command, &Xp2_in);

    SE_addDataOutput(&command, &xA_out);
    SE_addDataOutput(&command, &zkpA_out);

    SE_addParameter(&command, ctx->curve_flags);
    SE_addParameter(&command, ctx->pwd_len);
    SE_addParameter(&command, strlen(ecjpake_id[ctx->role]));

    int status = se_management_acquire();
    if (status != 0) {
        return status;
    }

    SE_executeCommand(&command);
    SE_Response_t res = SE_readCommandResponse();

    se_management_release();

    if ( res == SE_RESPONSE_OK ) {
        // If we are the server, we need to write out the ECParams
        if( ctx->role == MBEDTLS_ECJPAKE_SERVER )
        {
            if( len < 3 + 66 + 66 + 33)
            {
                return MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;
            }
            const mbedtls_ecp_curve_info *curve_info;

            if( ( curve_info = mbedtls_ecp_curve_info_from_grp_id( MBEDTLS_ECP_DP_SECP256R1 ) ) == NULL ) {
                return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
            }

            // First byte is curve_type, always named_curve
            *(buf++) = MBEDTLS_ECP_TLS_NAMED_CURVE;

            // Next two bytes are the namedcurve value
            *(buf++) = curve_info->tls_id >> 8;
            *(buf++) = curve_info->tls_id & 0xFF;

            *olen += 3;
            len -= 3;
        }

        // To write TLS structures of ECJ-PAKE, we need to write:
        // * XA in uncompressed form
        // * zkpA in uncompressed form
        uint8_t *obuf = buf;
        const uint8_t *ibuf = xA;
        size_t ilen = 64;

        ret = write_tls_point(&obuf, &len, olen, &ibuf, &ilen, 64);
        if (ret != 0) {
            return ret;
        }

        ibuf = zkpA;
        ilen = 96;
        ret = write_tls_zkp(&obuf, &len, olen, &ibuf, &ilen, 64);
        if (ret != 0) {
            return ret;
        }

        return 0;
    } else {
        return MBEDTLS_ERR_ECP_HW_ACCEL_FAILED;
    }
}

int mbedtls_ecjpake_read_round_two( mbedtls_ecjpake_context *ctx,
                                    const unsigned char *buf,
                                    size_t len )
{
    if (ctx == NULL || buf == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    int ret = 0;

    // local storage for ZKP
    uint8_t zkpB[32+64];

    if( ctx->role == MBEDTLS_ECJPAKE_CLIENT )
    {
        const mbedtls_ecp_curve_info *curve_info;
        uint16_t tls_id;

        if( len < 3 ) {
            return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
        }

        // First byte is curve_type; only named_curve is handled
        if( *(buf++) != MBEDTLS_ECP_TLS_NAMED_CURVE ) {
            return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
        }

        // Next two bytes are the namedcurve value
        tls_id = *(buf++);
        tls_id <<= 8;
        tls_id |= *(buf++);

        if( ( curve_info = mbedtls_ecp_curve_info_from_tls_id( tls_id ) ) == NULL ) {
            return( MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE );
        }

        if (curve_info->grp_id != MBEDTLS_ECP_DP_SECP256R1) {
            return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
        }

        len -= 3;
    }

    // Should receive 1 binary point and 1 ZKP
    uint8_t *obuf = ctx->Xp;
    size_t olen = 64;

    // Parse structures
    ret = parse_tls_point(&buf, &len, NULL, &obuf, &olen);
    if (ret != 0) {
        return ret;
    }

    obuf = zkpB;
    olen = sizeof(zkpB);
    ret = parse_tls_zkp(&buf, &len, NULL, &obuf, &olen);
    if (ret != 0) {
        return ret;
    }

    if (len > 0) {
        // Too much input
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    // SE command structures
    SE_Command_t command = SE_COMMAND_DEFAULT(SE_COMMAND_JPAKE_R2_VERIFY);
    SE_DataTransfer_t domain_in = SE_DATATRANSFER_DEFAULT(NULL, 0);
    SE_DataTransfer_t userid_peer = SE_DATATRANSFER_DEFAULT((void*)ecjpake_id[1-ctx->role], strlen(ecjpake_id[1-ctx->role]));

    SE_DataTransfer_t Xm1_in = SE_DATATRANSFER_DEFAULT(ctx->Xm1, 64);
    SE_DataTransfer_t Xm2_in = SE_DATATRANSFER_DEFAULT(ctx->Xm2, 64);
    SE_DataTransfer_t Xp1_in = SE_DATATRANSFER_DEFAULT(ctx->Xp1, 64);
    SE_DataTransfer_t Xp_in = SE_DATATRANSFER_DEFAULT(ctx->Xp, 64);
    SE_DataTransfer_t zkpB_in = SE_DATATRANSFER_DEFAULT(zkpB, sizeof(zkpB));

    SE_addDataInput(&command, &domain_in);
    SE_addDataInput(&command, &userid_peer);

    SE_addDataInput(&command, &Xm1_in);
    SE_addDataInput(&command, &Xm2_in);
    SE_addDataInput(&command, &Xp1_in);
    SE_addDataInput(&command, &Xp_in);
    SE_addDataInput(&command, &zkpB_in);

    SE_addParameter(&command, ctx->curve_flags);
    SE_addParameter(&command, strlen(ecjpake_id[1-ctx->role]));

    int status = se_management_acquire();
    if (status != 0) {
        return status;
    }

    SE_executeCommand(&command);
    SE_Response_t res = SE_readCommandResponse();

    se_management_release();

    if ( res == SE_RESPONSE_OK ) {
        return 0;
    } else {
        return MBEDTLS_ERR_ECP_VERIFY_FAILED;
    }
}

int mbedtls_ecjpake_derive_secret( mbedtls_ecjpake_context *ctx,
                            unsigned char *buf, size_t len, size_t *olen,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng )
{
    // SE has internal RNG
    (void)f_rng;
    (void)p_rng;

    if (len < 32) {
        return MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;
    }

    // Generated session key needs to come out unprotected
    uint32_t gen_key_flags = 32;

    // SE command structures
    SE_Command_t command = SE_COMMAND_DEFAULT(SE_COMMAND_JPAKE_GEN_SESSIONKEY | SE_COMMAND_OPTION_HASH_SHA256);
    SE_DataTransfer_t domain_in = SE_DATATRANSFER_DEFAULT(NULL, 0);
    SE_DataTransfer_t pwd_in = SE_DATATRANSFER_DEFAULT(ctx->pwd, ctx->pwd_len);

    SE_DataTransfer_t r_in = SE_DATATRANSFER_DEFAULT(ctx->r, 32);
    SE_DataTransfer_t Xp2_in = SE_DATATRANSFER_DEFAULT(ctx->Xp2, 64);
    SE_DataTransfer_t Xp_in = SE_DATATRANSFER_DEFAULT(ctx->Xp, 64);
    SE_DataTransfer_t key_out = SE_DATATRANSFER_DEFAULT(buf, 32);

    SE_addDataInput(&command, &domain_in);
    SE_addDataInput(&command, &pwd_in);

    SE_addDataInput(&command, &r_in);
    SE_addDataInput(&command, &Xp2_in);
    SE_addDataInput(&command, &Xp_in);

    SE_addDataOutput(&command, &key_out);

    SE_addParameter(&command, ctx->curve_flags);
    SE_addParameter(&command, ctx->pwd_len);
    SE_addParameter(&command, gen_key_flags);

    int status = se_management_acquire();
    if (status != 0) {
        return status;
    }

    SE_executeCommand(&command);
    SE_Response_t res = SE_readCommandResponse();

    se_management_release();

    if ( res == SE_RESPONSE_OK ) {
        *olen = 32;
        return 0;
    } else {
        *olen = 0;
        return MBEDTLS_ERR_ECP_VERIFY_FAILED;
    }
}

void mbedtls_ecjpake_free( mbedtls_ecjpake_context *ctx )
{
    if (ctx == NULL) {
        return;
    }

    memset(ctx, 0, sizeof(*ctx));
}

#endif /* #if defined(SEMAILBOX_PRESENT) */

#endif /* #if defined(MBEDTLS_ECJPAKE_ALT) && defined(MBEDTLS_ECJPAKE_C) */
