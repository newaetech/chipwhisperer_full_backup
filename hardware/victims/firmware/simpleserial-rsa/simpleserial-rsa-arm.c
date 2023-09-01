/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2016-2017 NewAE Technology Inc.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "hal.h"
#include "simpleserial.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#if defined(__arm__) || defined(__riscv__) || defined(__riscv)

#include "mbedtls/rsa.h"
#include "mbedtls/sha256.h"
#include "mbedtls/oid.h"
uint8_t sig_chunk_1(uint8_t *pt, uint8_t len);
uint8_t sig_chunk_2(uint8_t *pt, uint8_t len);
#define mbedtls_calloc calloc
#define mbedtls_free free

// MWC random number implementation - https://en.wikipedia.org/wiki/Multiply-with-carry_pseudorandom_number_generator
#define PHI 0x9e3779b9

static uint32_t Q[4096], c = 362436;

void init_rand(uint32_t x)
{
    int i;

    Q[0] = x;
    Q[1] = x + PHI;
    Q[2] = x + PHI + PHI;

    for (i = 3; i < 4096; i++)
            Q[i] = Q[i - 3] ^ Q[i - 2] ^ PHI ^ i;
}

uint32_t rand_cmwc(void)
{
    uint64_t t, a = 18782LL;
    static uint32_t i = 4095;
    uint32_t x, r = 0xfffffffe;
    i = (i + 1) & 4095;
    t = a * Q[i] + c;
    c = (t >> 32);
    x = t + c;
    if (x < c) {
            x++;
            c++;
    }
    return (Q[i] = r - x);
}

static int myrand( void *rng_state, unsigned char *output, size_t len )
{
     size_t i;

     if( rng_state != NULL )
          rng_state  = NULL;

     for( i = 0; i < len; ++i )
          output[i] = rand_cmwc();

     return( 0 );
}

/*
 * Example RSA-1024 keypair, for test purposes
 */
#define RSA_KEY_LEN 128

#define RSA_N   "9292758453063D803DD603D5E777D788" \
                "8ED1D5BF35786190FA2F23EBC0848AEA" \
                "DDA92CA6C3D80B32C4D109BE0F36D6AE" \
                "7130B9CED7ACDF54CFC7555AC14EEBAB" \
                "93A89813FBF3C4F8066D2D800F7C38A8" \
                "1AE31942917403FF4946B0A83D3D3E05" \
                "EE57C6F5F5606FB5D4BC6CD34EE0801A" \
                "5E94BB77B07507233A0BC7BAC8F90F79"

#define RSA_E   "10001"

#define RSA_D   "24BF6185468786FDD303083D25E64EFC" \
                "66CA472BC44D253102F8B4A9D3BFA750" \
                "91386C0077937FE33FA3252D28855837" \
                "AE1B484A8A9A45F7EE8C0C634F99E8CD" \
                "DF79C5CE07EE72C7F123142198164234" \
                "CABB724CF78B8173B9F880FC86322407" \
                "AF1FEDFDDE2BEB674CA15F3E81A1521E" \
                "071513A1E85B5DFA031F21ECAE91A34D"

#define RSA_P   "C36D0EB7FCD285223CFB5AABA5BDA3D8" \
                "2C01CAD19EA484A87EA4377637E75500" \
                "FCB2005C5C7DD6EC4AC023CDA285D796" \
                "C3D9E75E1EFC42488BB4F1D13AC30A57"

#define RSA_Q   "C000DF51A7C77AE8D7C7370C1FF55B69" \
                "E211C2B9E5DB1ED0BF61D0D9899620F4" \
                "910E4168387E3C30AA1E00C339A79508" \
                "8452DD96A9A5EA5D9DCA68DA636032AF"

#define RSA_DP  "C1ACF567564274FB07A0BBAD5D26E298" \
                "3C94D22288ACD763FD8E5600ED4A702D" \
                "F84198A5F06C2E72236AE490C93F07F8" \
                "3CC559CD27BC2D1CA488811730BB5725"

#define RSA_DQ  "4959CBF6F8FEF750AEE6977C155579C7" \
                "D8AAEA56749EA28623272E4F7D0592AF" \
                "7C1F1313CAC9471B5C523BFE592F517B" \
                "407A1BD76C164B93DA2D32A383E58357"

#define RSA_QP  "9AE7FBC99546432DF71896FC239EADAE" \
                "F38D18D2B2F0E2DD275AA977E2BF4411" \
                "F5A3B2A5D33605AEBBCCBA7FEB9F2D2F" \
                "A74206CEC169D74BF5A8C50D6F48EA08"

#define PT_LEN  24
#define RSA_PT  "\xAA\xBB\xCC\x03\x02\x01\x00\xFF\xFF\xFF\xFF\xFF" \
                "\x11\x22\x33\x0A\x0B\x0C\xCC\xDD\xDD\xDD\xDD\xDD"

const char MESSAGE[] =  "Hello World!";


mbedtls_rsa_context rsa_ctx;
unsigned char rsa_plaintext[PT_LEN];
unsigned char rsa_decrypted[PT_LEN];
unsigned char rsa_ciphertext[RSA_KEY_LEN];

/*
 * Do an RSA private key operation
 */
static int simpleserial_mbedtls_rsa_private( mbedtls_rsa_context *ctx,
                 int (*f_rng)(void *, unsigned char *, size_t),
                 void *p_rng,
                 const unsigned char *input,
                 unsigned char *output )
{
    int ret;
    size_t olen;
    mbedtls_mpi T, T1, T2;
    mbedtls_mpi P1, Q1, R;
    mbedtls_mpi *DP = &ctx->DP;
    mbedtls_mpi *DQ = &ctx->DQ;

    /* Make sure we have private key info, prevent possible misuse */
    if( ctx->P.p == NULL || ctx->Q.p == NULL || ctx->D.p == NULL )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    mbedtls_mpi_init( &T ); mbedtls_mpi_init( &T1 ); mbedtls_mpi_init( &T2 );
    mbedtls_mpi_init( &P1 ); mbedtls_mpi_init( &Q1 ); mbedtls_mpi_init( &R );

    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &T, input, ctx->len ) );
    if( mbedtls_mpi_cmp_mpi( &T, &ctx->N ) >= 0 )
    {
        ret = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
        goto cleanup;
    }

    /*
     * Faster decryption using the CRT
     *
     * T1 = input ^ dP mod P
     * T2 = input ^ dQ mod Q
     */
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &T1, &T, DP, &ctx->P, &ctx->RP ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &T2, &T, DQ, &ctx->Q, &ctx->RQ ) );

    /*
     * T = (T1 - T2) * (Q^-1 mod P) mod P
     */
    MBEDTLS_MPI_CHK( mbedtls_mpi_sub_mpi( &T, &T1, &T2 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &T1, &T, &ctx->QP ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &T, &T1, &ctx->P ) );

    /*
     * T = T2 + T * Q
     */
    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &T1, &T, &ctx->Q ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_add_mpi( &T, &T2, &T1 ) );

    olen = ctx->len;
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &T, output, olen ) );

cleanup:
    mbedtls_mpi_free( &T ); mbedtls_mpi_free( &T1 ); mbedtls_mpi_free( &T2 );
    mbedtls_mpi_free( &P1 ); mbedtls_mpi_free( &Q1 ); mbedtls_mpi_free( &R );

    if( ret != 0 )
        return( MBEDTLS_ERR_RSA_PRIVATE_FAILED + ret );

    return( 0 );
}

static int simpleserial_mbedtls_rsa_rsassa_pkcs1_v15_sign( mbedtls_rsa_context *ctx,
                               int (*f_rng)(void *, unsigned char *, size_t),
                               void *p_rng,
                               int mode,
                               mbedtls_md_type_t md_alg,
                               unsigned int hashlen,
                               const unsigned char *hash,
                               unsigned char *sig )
{
    size_t nb_pad, olen, oid_size = 0;
    unsigned char *p = sig;
    const char *oid = NULL;
    unsigned char *sig_try = NULL, *verif = NULL;
    size_t i;
    unsigned char diff;
    volatile unsigned char diff_no_optimize;
    int ret;

    if( mode == MBEDTLS_RSA_PRIVATE && ctx->padding != MBEDTLS_RSA_PKCS_V15 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    olen = ctx->len;
    nb_pad = olen - 3;

    if( md_alg != MBEDTLS_MD_NONE )
    {
        const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type( md_alg );
        if( md_info == NULL )
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

        if( mbedtls_oid_get_oid_by_md( md_alg, &oid, &oid_size ) != 0 )
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

        nb_pad -= 10 + oid_size;

        hashlen = mbedtls_md_get_size( md_info );
    }

    nb_pad -= hashlen;

    if( ( nb_pad < 8 ) || ( nb_pad > olen ) )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    *p++ = 0;
    *p++ = MBEDTLS_RSA_SIGN;
    memset( p, 0xFF, nb_pad );
    p += nb_pad;
    *p++ = 0;

    if( md_alg == MBEDTLS_MD_NONE )
    {
        memcpy( p, hash, hashlen );
    }
    else
    {
        /*
         * DigestInfo ::= SEQUENCE {
         *   digestAlgorithm DigestAlgorithmIdentifier,
         *   digest Digest }
         *
         * DigestAlgorithmIdentifier ::= AlgorithmIdentifier
         *
         * Digest ::= OCTET STRING
         */
        *p++ = MBEDTLS_ASN1_SEQUENCE | MBEDTLS_ASN1_CONSTRUCTED;
        *p++ = (unsigned char) ( 0x08 + oid_size + hashlen );
        *p++ = MBEDTLS_ASN1_SEQUENCE | MBEDTLS_ASN1_CONSTRUCTED;
        *p++ = (unsigned char) ( 0x04 + oid_size );
        *p++ = MBEDTLS_ASN1_OID;
        *p++ = oid_size & 0xFF;
        memcpy( p, oid, oid_size );
        p += oid_size;
        *p++ = MBEDTLS_ASN1_NULL;
        *p++ = 0x00;
        *p++ = MBEDTLS_ASN1_OCTET_STRING;
        *p++ = hashlen;
        memcpy( p, hash, hashlen );
    }

    if( mode == MBEDTLS_RSA_PUBLIC )
        return( mbedtls_rsa_public(  ctx, sig, sig ) );

    /*
     * In order to prevent Lenstra's attack, make the signature in a
     * temporary buffer and check it before returning it.
     */
    sig_try = mbedtls_calloc( 1, ctx->len );
    if( sig_try == NULL )
        return( MBEDTLS_ERR_MPI_ALLOC_FAILED );

    verif   = mbedtls_calloc( 1, ctx->len );
    if( verif == NULL )
    {
        mbedtls_free( sig_try );
        return( MBEDTLS_ERR_MPI_ALLOC_FAILED );
    }

    MBEDTLS_MPI_CHK( simpleserial_mbedtls_rsa_private( ctx, f_rng, p_rng, sig, sig_try ) );
    //make things easier
    MBEDTLS_MPI_CHK( mbedtls_rsa_public( ctx, sig_try, verif ) );

    /* Compare in constant time just in case */
    /* for( diff = 0, i = 0; i < ctx->len; i++ ) */
    /*     diff |= verif[i] ^ sig[i]; */
    /* diff_no_optimize = diff; */

    /* if( diff_no_optimize != 0 ) */
    /* { */
    /*     ret = MBEDTLS_ERR_RSA_PRIVATE_FAILED; */
    /*     goto cleanup; */
    /* } */

    memcpy( sig, sig_try, ctx->len );

cleanup:
    mbedtls_free( sig_try );
    mbedtls_free( verif );

    return( ret );
}


void rsa_init(void)
{
    init_rand(0);
    mbedtls_rsa_init( &rsa_ctx, MBEDTLS_RSA_PKCS_V15, 0 );
    simpleserial_addcmd('1', 0, sig_chunk_1);
    simpleserial_addcmd('2', 0, sig_chunk_2);

    rsa_ctx.len = RSA_KEY_LEN;
    mbedtls_mpi_read_string( &rsa_ctx.N , 16, RSA_N  ) ;
    mbedtls_mpi_read_string( &rsa_ctx.E , 16, RSA_E  ) ;
    mbedtls_mpi_read_string( &rsa_ctx.D , 16, RSA_D  ) ;
    mbedtls_mpi_read_string( &rsa_ctx.P , 16, RSA_P  ) ;
    mbedtls_mpi_read_string( &rsa_ctx.Q , 16, RSA_Q  ) ;
    mbedtls_mpi_read_string( &rsa_ctx.DP, 16, RSA_DP ) ;
    mbedtls_mpi_read_string( &rsa_ctx.DQ, 16, RSA_DQ ) ;
    mbedtls_mpi_read_string( &rsa_ctx.QP, 16, RSA_QP ) ;

    //Make valid data first, otherwise system barfs
    memcpy( rsa_plaintext, RSA_PT, PT_LEN );
    mbedtls_rsa_pkcs1_encrypt( &rsa_ctx, myrand, NULL, MBEDTLS_RSA_PUBLIC, PT_LEN,
                           rsa_plaintext, rsa_ciphertext );

}




/*
  Performs an RSA-CRT PKCS1 signing. This can be broken by injecting a fault into one of the two parts of the
  signature calulation, at which point the secrets q and p can be recovered from the received faulty signature.

  int mbedtls_rsa_pkcs1_sign( mbedtls_rsa_context *ctx,
                              int (*f_rng)(void *, unsigned char *, size_t),
                              void *p_rng,
                              int mode,
                              mbedtls_md_type_t md_alg,
                              unsigned int hashlen,
                              const unsigned char *hash,
                              unsigned char *sig );
 */
uint8_t buf[128];
uint8_t hash[32];
#if SS_VER == SS_VER_2_1
uint8_t real_dec(uint8_t cmd, uint8_t scmd, uint8_t len, uint8_t *pt)
#else
uint8_t real_dec(uint8_t *pt, uint8_t len)
#endif
{
    int ret = 0;

    //first need to hash our message
    memset(buf, 0, 128);
    mbedtls_sha256(MESSAGE, 12, hash, 0);

    trigger_high();
    ret = simpleserial_mbedtls_rsa_rsassa_pkcs1_v15_sign(&rsa_ctx, NULL, NULL, MBEDTLS_RSA_PRIVATE, MBEDTLS_MD_SHA256, 32, hash, buf);
    trigger_low();

    //send back first 48 bytes
#if SS_VER == SS_VER_2_1
    simpleserial_put('r', 128, buf);
#else
    simpleserial_put('r', 48, buf);
#endif
    return ret;
}

uint8_t sig_chunk_1(uint8_t *pt, uint8_t len)
{
     simpleserial_put('r', 48, buf + 48);
     return 0x00;
}

uint8_t sig_chunk_2(uint8_t *pt, uint8_t len)
{
     simpleserial_put('r', 128 - 48 * 2, buf + 48*2);
     return 0x00;
}


uint8_t get_pt(uint8_t *pt, uint8_t len)
{
}

#endif