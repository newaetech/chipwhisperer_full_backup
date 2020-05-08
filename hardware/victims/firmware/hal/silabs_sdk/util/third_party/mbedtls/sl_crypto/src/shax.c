/***************************************************************************//**
 * @file
 * @brief Generic SHA abstraction
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
 *  The SHA-1 standard was published by NIST in 1993.
 *
 *  http://www.itl.nist.gov/fipspubs/fip180-1.htm
 *
 *  The SHA-256 Secure Hash Standard was published by NIST in 2002.
 *
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"
#include "mbedtls/md.h"

#if (defined(MBEDTLS_SHA256_ALT) && defined(MBEDTLS_SHA256_C)) \
    || (defined(MBEDTLS_SHA1_ALT) && defined(MBEDTLS_SHA1_C)) \
    || (defined(MBEDTLS_SHA512_ALT) && defined(MBEDTLS_SHA512_C))

#include "shax.h"

/* import memcpy from string.h */
#include <string.h>

/* Define all init vectors. If we end up not needing a feature, there won't be a reference to the vecotr and it will get thrown out by the linker */
#if defined(MBEDTLS_SHA1_ALT) && defined(MBEDTLS_SHA1_C)
static const uint8_t init_state_sha1[32] =
{
    0x67, 0x45, 0x23, 0x01,
    0xEF, 0xCD, 0xAB, 0x89,
    0x98, 0xBA, 0xDC, 0xFE,
    0x10, 0x32, 0x54, 0x76,
    0xC3, 0xD2, 0xE1, 0xF0,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};
#endif

#if defined(MBEDTLS_SHA256_ALT) && defined(MBEDTLS_SHA256_C)
static const uint8_t init_state_sha224[32] =
{
    0xC1, 0x05, 0x9E, 0xD8,
    0x36, 0x7C, 0xD5, 0x07,
    0x30, 0x70, 0xDD, 0x17,
    0xF7, 0x0E, 0x59, 0x39,
    0xFF, 0xC0, 0x0B, 0x31,
    0x68, 0x58, 0x15, 0x11,
    0x64, 0xF9, 0x8F, 0xA7,
    0xBE, 0xFA, 0x4F, 0xA4
};

static const uint8_t init_state_sha256[32] =
{
    0x6A, 0x09, 0xE6, 0x67,
    0xBB, 0x67, 0xAE, 0x85,
    0x3C, 0x6E, 0xF3, 0x72,
    0xA5, 0x4F, 0xF5, 0x3A,
    0x51, 0x0E, 0x52, 0x7F,
    0x9B, 0x05, 0x68, 0x8C,
    0x1F, 0x83, 0xD9, 0xAB,
    0x5B, 0xE0, 0xCD, 0x19
};
#endif

#if defined(MBEDTLS_SHA512_ALT) && defined(MBEDTLS_SHA512_C)
static const uint8_t init_state_sha384[64] =
{
    0xCB, 0xBB, 0x9D, 0x5D, 0xC1, 0x05, 0x9E, 0xD8,
    0x62, 0x9A, 0x29, 0x2A, 0x36, 0x7C, 0xD5, 0x07,
    0x91, 0x59, 0x01, 0x5A, 0x30, 0x70, 0xDD, 0x17,
    0x15, 0x2F, 0xEC, 0xD8, 0xF7, 0x0E, 0x59, 0x39,
    0x67, 0x33, 0x26, 0x67, 0xFF, 0xC0, 0x0B, 0x31,
    0x8E, 0xB4, 0x4A, 0x87, 0x68, 0x58, 0x15, 0x11,
    0xDB, 0x0C, 0x2E, 0x0D, 0x64, 0xF9, 0x8F, 0xA7,
    0x47, 0xB5, 0x48, 0x1D, 0xBE, 0xFA, 0x4F, 0xA4
};

static const uint8_t init_state_sha512[64] =
{
    0x6A, 0x09, 0xE6, 0x67, 0xF3, 0xBC, 0xC9, 0x08,
    0xBB, 0x67, 0xAE, 0x85, 0x84, 0xCA, 0xA7, 0x3B,
    0x3C, 0x6E, 0xF3, 0x72, 0xFE, 0x94, 0xF8, 0x2B,
    0xA5, 0x4F, 0xF5, 0x3A, 0x5F, 0x1D, 0x36, 0xF1,
    0x51, 0x0E, 0x52, 0x7F, 0xAD, 0xE6, 0x82, 0xD1,
    0x9B, 0x05, 0x68, 0x8C, 0x2B, 0x3E, 0x6C, 0x1F,
    0x1F, 0x83, 0xD9, 0xAB, 0xFB, 0x41, 0xBD, 0x6B,
    0x5B, 0xE0, 0xCD, 0x19, 0x13, 0x7E, 0x21, 0x79
};
#endif

/* Define padding as largest padding we might need */
#if defined(MBEDTLS_SHA512_ALT) && defined(MBEDTLS_SHA512_C)
static const unsigned char shax_padding[128] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
#elif (defined(MBEDTLS_SHA256_ALT) && defined(MBEDTLS_SHA256_C)) || (defined(MBEDTLS_SHA1_ALT) && defined(MBEDTLS_SHA1_C))
static const unsigned char shax_padding[64] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
#endif

int sha_x_update(SHA_Type_t algo, const unsigned char *data, size_t data_len, uint8_t* state, unsigned char *buffer, uint32_t* counter)
{
    size_t blocksize, countersize, blocks, fill, left;
    uint8_t* state_in = NULL;

    switch(algo) {
#if defined(MBEDTLS_SHA1_ALT) && defined(MBEDTLS_SHA1_C)
        case SHA1:
            blocksize = 64;
            countersize = 64/32;
            break;
#endif
#if defined(MBEDTLS_SHA256_ALT) && defined(MBEDTLS_SHA256_C)
        case SHA224:
        case SHA256:
            blocksize = 64;
            countersize = 64/32;
            break;
#endif
#if defined(MBEDTLS_SHA512_ALT) && defined(MBEDTLS_SHA512_C)
        case SHA384:
        case SHA512:
            blocksize = 128;
            countersize = 128/32;
            break;
#endif
        default:
            return MBEDTLS_ERR_MD_FEATURE_UNAVAILABLE;
    }

    if( data_len == 0 )
        return 0;

    if(counter[0] < blocksize) {
        for(size_t i = 1; i < countersize; i++) {
            if(counter[i] != 0) {
                state_in = state;
                break;
            }
        }
        if(state_in == NULL) {
            switch(algo) {
#if defined(MBEDTLS_SHA1_ALT) && defined(MBEDTLS_SHA1_C)
                case SHA1:
                    state_in = (uint8_t*)init_state_sha1;
                    break;
#endif
#if defined(MBEDTLS_SHA256_ALT) && defined(MBEDTLS_SHA256_C)
                case SHA224:
                    state_in = (uint8_t*)init_state_sha224;
                    break;
                case SHA256:
                    state_in = (uint8_t*)init_state_sha256;
                    break;
#endif
#if defined(MBEDTLS_SHA512_ALT) && defined(MBEDTLS_SHA512_C)
                case SHA384:
                    state_in = (uint8_t*)init_state_sha384;
                    break;
                case SHA512:
                    state_in = (uint8_t*)init_state_sha512;
                    break;
#endif
                default:
                    return MBEDTLS_ERR_MD_FEATURE_UNAVAILABLE;
            }
        }
    } else {
        state_in = state;
    }

    left = (counter[0] & (blocksize - 1));
    fill = blocksize - left;

    counter[0] += data_len;

    /* ripple counter */
    if( counter[0] < data_len ) {
        counter[1] += 1;
        for(size_t i = 1; i < (countersize - 1); i++) {
            if( counter[i] == 0 )
                counter[i+1]++;
        }
    }

    if( ( left > 0 ) && ( data_len >= fill ) )
    {
        memcpy( (void *) (buffer + left), data, fill );
        sha_x_process(algo, state_in, buffer, state, 1);
        state_in = state;
        data += fill;
        data_len -= fill;
        left = 0;
    }

    if( data_len >= blocksize )
    {
        blocks = data_len / blocksize;
        sha_x_process(algo, state_in, data, state, blocks);
        state_in = state;
        data += blocksize * blocks;
        data_len -= blocksize * blocks;
    }

    if( data_len > 0 ) {
        memcpy( (void *) (buffer + left), data, data_len );
    }

    return 0;
}

int sha_x_finish(SHA_Type_t algo, uint8_t* state, unsigned char *buffer, uint32_t* counter, unsigned char *output)
{
    size_t last_data_byte, num_pad_bytes, blocksize, countersize, outputsize;
    uint8_t msglen[16];

    switch(algo) {
#if defined(MBEDTLS_SHA1_ALT) && defined(MBEDTLS_SHA1_C)
        case SHA1:
            blocksize = 64;
            outputsize = 20;
            countersize = 64/32;
            break;
#endif
#if defined(MBEDTLS_SHA256_C) && defined(MBEDTLS_SHA256_ALT)
        case SHA224:
            blocksize = 64;
            outputsize = 28;
            countersize = 64/32;
            break;
        case SHA256:
            blocksize = 64;
            outputsize = 32;
            countersize = 64/32;
            break;
#endif
#if defined(MBEDTLS_SHA512_C) && defined(MBEDTLS_SHA512_ALT)
        case SHA384:
            blocksize = 128;
            outputsize = 48;
            countersize = 128/32;
            break;
        case SHA512:
            blocksize = 128;
            outputsize = 64;
            countersize = 128/32;
            break;
#endif
        default:
            return MBEDTLS_ERR_MD_FEATURE_UNAVAILABLE;
    }

    /* Convert counter value to bits, and put in big-endian array */
    uint8_t residual = 0;
    for(size_t i = 0; i < countersize; i++) {
        size_t msglen_index = ( (countersize - i) * sizeof(uint32_t) )- 1;

        msglen[msglen_index - 0] = ((counter[i] << 3) + residual) & 0xFF;
        msglen[msglen_index - 1] = (counter[i] >> 5) & 0xFF;
        msglen[msglen_index - 2] = (counter[i] >> 13) & 0xFF;
        msglen[msglen_index - 3] = (counter[i] >> 21) & 0xFF;

        residual = (counter[i] >> 29) & 0xFF;
    }

    last_data_byte = ( counter[0] & (blocksize - 1) );
    num_pad_bytes = ( last_data_byte < (blocksize - (countersize*4)) ) ?
                      ( (blocksize - (countersize*4)) - last_data_byte ) :
                      ( ((2*blocksize) - (countersize*4)) - last_data_byte );

    sha_x_update(algo, (uint8_t*)shax_padding, num_pad_bytes, state, buffer, counter);
    sha_x_update(algo, msglen, (countersize * 4), state, buffer, counter);

    memcpy(output, state, outputsize);

    return 0;
}

#if defined(MBEDTLS_SHA512_ALT) && defined(MBEDTLS_SHA512_C)

void mbedtls_sha512_init( mbedtls_sha512_context *ctx )
{
    if( ctx == NULL ) {
        return;
    }

    memset( ctx, 0, sizeof( mbedtls_sha512_context ) );
}

void mbedtls_sha512_free( mbedtls_sha512_context *ctx )
{
    if( ctx == NULL ) {
        return;
    }

    memset( ctx, 0, sizeof( mbedtls_sha512_context ) );
}

void mbedtls_sha512_clone( mbedtls_sha512_context *dst,
                           const mbedtls_sha512_context *src )
{
    if( (dst == NULL) || (src == NULL) ) {
        return;
    }

    *dst = *src;
}

int mbedtls_sha512_starts_ret( mbedtls_sha512_context *ctx, int is384 )
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;
    ctx->is384 = (is384 ? true : false);
    return 0;
}

int mbedtls_sha512_update_ret( mbedtls_sha512_context *ctx, const unsigned char *input,
                    size_t ilen )
{
    return sha_x_update((ctx->is384 ? SHA384 : SHA512),
                        input,
                        ilen,
                        (uint8_t*)ctx->state,
                        ctx->buffer,
                        (uint32_t*)ctx->total);
}

int mbedtls_internal_sha512_process( mbedtls_sha512_context *ctx, const unsigned char data[128] )
{
    return mbedtls_sha512_update_ret(ctx, data, 128);
}

int mbedtls_sha512_finish_ret( mbedtls_sha512_context *ctx, unsigned char output[64] )
{
    return sha_x_finish((ctx->is384 ? SHA384 : SHA512),
                        (uint8_t*)ctx->state,
                        ctx->buffer,
                        (uint32_t*)ctx->total,
                        output);
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha512_starts( mbedtls_sha512_context *ctx,
                            int is384 )
{
    mbedtls_sha512_starts_ret( ctx, is384 );
}

void mbedtls_sha512_process( mbedtls_sha512_context *ctx, const unsigned char data[128] )
{
    mbedtls_sha512_update_ret(ctx, data, 128);
}

void mbedtls_sha512_update( mbedtls_sha512_context *ctx,
                            const unsigned char *input,
                            size_t ilen )
{
    mbedtls_sha512_update_ret( ctx, input, ilen );
}

void mbedtls_sha512_finish( mbedtls_sha512_context *ctx,
                            unsigned char output[64] )
{
    mbedtls_sha512_finish_ret( ctx, output );
}
#endif /* !DEPRECATED_REMOVED */
#endif /* SHA512 acceleration active */

#if defined(MBEDTLS_SHA256_ALT) && defined(MBEDTLS_SHA256_C)
void mbedtls_sha256_init( mbedtls_sha256_context *ctx )
{
    if( ctx == NULL ) {
        return;
    }

    memset( ctx, 0, sizeof( mbedtls_sha256_context ) );
}

void mbedtls_sha256_free( mbedtls_sha256_context *ctx )
{
    if( ctx == NULL ) {
        return;
    }

    memset( ctx, 0, sizeof( mbedtls_sha256_context ) );
}

void mbedtls_sha256_clone( mbedtls_sha256_context *dst,
                           const mbedtls_sha256_context *src )
{
    if ( dst != NULL && src != NULL ) {
        *dst = *src;
    }
}

int mbedtls_sha256_starts_ret( mbedtls_sha256_context *ctx, int is224 )
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;
    ctx->is224 = (is224 > 0 ? true : false);

    return 0;
}

int mbedtls_sha256_update_ret( mbedtls_sha256_context *ctx, const unsigned char *input,
                    size_t ilen )
{
    return sha_x_update((ctx->is224 ? SHA224 : SHA256),
                        input,
                        ilen,
                        (uint8_t*)ctx->state,
                        ctx->buffer,
                        ctx->total);
}

int mbedtls_internal_sha256_process( mbedtls_sha256_context *ctx, const unsigned char data[64] )
{
    return mbedtls_sha256_update_ret(ctx, data, 64);
}

int mbedtls_sha256_finish_ret( mbedtls_sha256_context *ctx, unsigned char output[32] )
{
    return sha_x_finish((ctx->is224 ? SHA224 : SHA256),
                        (uint8_t*)ctx->state,
                        ctx->buffer,
                        ctx->total,
                        output);
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha256_starts( mbedtls_sha256_context *ctx,
                            int is224 )
{
    mbedtls_sha256_starts_ret( ctx, is224 );
}

void mbedtls_sha256_process( mbedtls_sha256_context *ctx, const unsigned char data[64] )
{
    mbedtls_sha256_update_ret(ctx, data, 64);
}

void mbedtls_sha256_update( mbedtls_sha256_context *ctx,
                            const unsigned char *input,
                            size_t ilen )
{
    mbedtls_sha256_update_ret( ctx, input, ilen );
}

void mbedtls_sha256_finish( mbedtls_sha256_context *ctx,
                            unsigned char output[32] )
{
    mbedtls_sha256_finish_ret( ctx, output );
}
#endif /* !DEPRECATED_REMOVED */
#endif /* SHA256 acceleration active */

#if defined(MBEDTLS_SHA1_ALT) && defined(MBEDTLS_SHA1_C)

void mbedtls_sha1_init( mbedtls_sha1_context *ctx )
{
    if( ctx == NULL ) {
        return;
    }
    memset( ctx, 0, sizeof( mbedtls_sha1_context ) );
}

void mbedtls_sha1_free( mbedtls_sha1_context *ctx )
{
    if( ctx == NULL ) {
        return;
    }
    memset( ctx, 0, sizeof( mbedtls_sha1_context ) );
}

void mbedtls_sha1_clone( mbedtls_sha1_context *dst,
                         const mbedtls_sha1_context *src )
{
    if ( dst != NULL && src != NULL ) {
        *dst = *src;
    }
}

int mbedtls_sha1_starts_ret( mbedtls_sha1_context *ctx )
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;
    return 0;
}

int mbedtls_sha1_update_ret( mbedtls_sha1_context *ctx, const unsigned char *input, size_t ilen )
{
    return sha_x_update(SHA1,
                        input,
                        ilen,
                        (uint8_t *)ctx->state,
                        ctx->buffer,
                        ctx->total);
}

int mbedtls_internal_sha1_process( mbedtls_sha1_context *ctx, const unsigned char data[64] )
{
    return mbedtls_sha1_update_ret(ctx, data, 64);
}

int mbedtls_sha1_finish_ret( mbedtls_sha1_context *ctx, unsigned char output[20] )
{
    return sha_x_finish(SHA1,
                        (uint8_t *)ctx->state,
                        ctx->buffer,
                        ctx->total,
                        output);
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha1_starts( mbedtls_sha1_context *ctx )
{
    mbedtls_sha1_starts_ret( ctx );
}

void mbedtls_sha1_process( mbedtls_sha1_context *ctx, const unsigned char data[64] )
{
    mbedtls_sha1_update_ret(ctx, data, 64);
}

void mbedtls_sha1_update( mbedtls_sha1_context *ctx,
                          const unsigned char *input,
                          size_t ilen )
{
    mbedtls_sha1_update_ret( ctx, input, ilen );
}

void mbedtls_sha1_finish( mbedtls_sha1_context *ctx,
                          unsigned char output[20] )
{
    mbedtls_sha1_finish_ret( ctx, output );
}
#endif /* !DEPRECATED_REMOVED */
#endif /* SHA1 acceleration active */

#endif /* (SHA1 or SHA256 or SHA512) acceleration active */
