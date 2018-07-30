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

#if defined(__arm__)

#include "mbedtls/rsa.h"

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


mbedtls_rsa_context rsa_ctx;
unsigned char rsa_plaintext[PT_LEN];
unsigned char rsa_decrypted[PT_LEN];
unsigned char rsa_ciphertext[RSA_KEY_LEN];

static int myrand( void *rng_state, unsigned char *output, size_t len )
{
    size_t i;

    if( rng_state != NULL )
        rng_state  = NULL;

    for( i = 0; i < len; ++i )
        output[i] = rand();

    return( 0 );
}

void rsa_init(void)
{
    mbedtls_rsa_init( &rsa_ctx, MBEDTLS_RSA_PKCS_V15, 0 );

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


/* Perform a real RSA decryption, be aware this is very slow (~5 seconds). Takes 30124165 cycles on STM32F3. */
uint8_t real_dec(uint8_t * pt)
{
    int ret;
    size_t len;

    trigger_high();
    //Second 'NULL' can be set to 'myrand' which causes blinding to happen. This slows down the algorithm even more.
    ret = mbedtls_rsa_pkcs1_decrypt( &rsa_ctx, NULL, NULL, MBEDTLS_RSA_PRIVATE, &len,
                           rsa_ciphertext, rsa_decrypted, sizeof(rsa_decrypted) );
    trigger_low();
    
    switch(ret){
        case MBEDTLS_ERR_RSA_BAD_INPUT_DATA:
            return 0x10;
        case MBEDTLS_ERR_RSA_INVALID_PADDING:
            return 0x11;
        case MBEDTLS_ERR_RSA_KEY_GEN_FAILED:
            return 0x12;
        case MBEDTLS_ERR_RSA_KEY_CHECK_FAILED:
            return 0x13;
        case MBEDTLS_ERR_RSA_PUBLIC_FAILED:
            return 0x14;
        case MBEDTLS_ERR_RSA_PRIVATE_FAILED:
            return 0x15;
        case MBEDTLS_ERR_RSA_VERIFY_FAILED:
            return 0x16;
        case MBEDTLS_ERR_RSA_OUTPUT_TOO_LARGE:
            return 0x17;
        case MBEDTLS_ERR_RSA_RNG_FAILED:
            return 0x18;
    }
    
    return ret;
}

/* Performs PART of a RSA decryption using only 16 bytes of keying material, where the "key" is
   actually the 16-byte input plaintext (sent with 'p' command). or previously sent key with 'k' 
   command. This is used to give you an easier
   target to perform SPA on rather than the full (very slow) RSA algorithm. */
uint8_t get_pt(uint8_t * pt)
{
    //Part of function mbedtls_rsa_private()
    int ret = 0;
    mbedtls_mpi T, T1, T2;
    mbedtls_mpi P1, Q1, R;
    mbedtls_mpi *DP = &rsa_ctx.DP;
    mbedtls_mpi *DQ = &rsa_ctx.DQ;
    
    mbedtls_mpi fake_DP;
    mbedtls_mpi_init( &fake_DP );
    uint8_t fake_key[64];
    
    mbedtls_mpi_init( &T ); mbedtls_mpi_init( &T1 ); mbedtls_mpi_init( &T2 );
    mbedtls_mpi_init( &P1 ); mbedtls_mpi_init( &Q1 ); mbedtls_mpi_init( &R );
    
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &T, rsa_ciphertext, rsa_ctx.len ) );
    if( mbedtls_mpi_cmp_mpi( &T, &rsa_ctx.N ) >= 0 )
    {
        ret = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
        goto cleanup;
    }


    //mbedtls_mpi_set_bit( mbedtls_mpi *X, size_t pos, unsigned char val )
    memset(fake_key, 0, sizeof(fake_key));
    memcpy(fake_key, pt, 16);
    mbedtls_mpi_read_binary(&fake_DP, fake_key, sizeof(fake_key));

    /*
     * Faster decryption using the CRT
     *
     * T1 = input ^ dP mod P
     * T2 = input ^ dQ mod Q
     */
    trigger_high();
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &T1, &T, &fake_DP, &rsa_ctx.P, &rsa_ctx.RP ) );
    trigger_low();
    
    //The full CRT is below - no need to run it since we are just trying to demo an example, which is above
    /*
    // T1 = T^DP mod P
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &T1, &T, DP, &rsa_ctx.P, &rsa_ctx.RP ) );
    
    // T2 = T^DQ mod Q
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &T2, &T, DQ, &rsa_ctx.Q, &rsa_ctx.RQ ) );

    //T = (T1 - T2) * (Q^-1 mod P) mod P
    MBEDTLS_MPI_CHK( mbedtls_mpi_sub_mpi( &T, &T1, &T2 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &T1, &T, &rsa_ctx.QP ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &T, &T1, &rsa_ctx.P ) );

    //T = T2 + T * Q
    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &T1, &T, &rsa_ctx.Q ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_add_mpi( &T, &T2, &T1 ) );

    */
    

cleanup:
    mbedtls_mpi_free( &T ); mbedtls_mpi_free( &T1 ); mbedtls_mpi_free( &T2 );
    mbedtls_mpi_free( &P1 ); mbedtls_mpi_free( &Q1 ); mbedtls_mpi_free( &R );
    mbedtls_mpi_free( &fake_DP );

    switch(ret){
        case MBEDTLS_ERR_RSA_BAD_INPUT_DATA:
            return 0x10;
        case MBEDTLS_ERR_RSA_INVALID_PADDING:
            return 0x11;
        case MBEDTLS_ERR_RSA_KEY_GEN_FAILED:
            return 0x12;
        case MBEDTLS_ERR_RSA_KEY_CHECK_FAILED:
            return 0x13;
        case MBEDTLS_ERR_RSA_PUBLIC_FAILED:
            return 0x14;
        case MBEDTLS_ERR_RSA_PRIVATE_FAILED:
            return 0x15;
        case MBEDTLS_ERR_RSA_VERIFY_FAILED:
            return 0x16;
        case MBEDTLS_ERR_RSA_OUTPUT_TOO_LARGE:
            return 0x17;
        case MBEDTLS_ERR_RSA_RNG_FAILED:
            return 0x18;
    }
    
    return ret;
}
	
#endif
    