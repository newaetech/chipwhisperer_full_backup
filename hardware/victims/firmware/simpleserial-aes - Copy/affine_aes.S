/* AES-128 Encryption first order masking
* v1.0: Affine version of AES (encryption and decryption), only 128 bit keys are supported.
* Two random bytes (rin and rout) are used during the subbyte operation (constant for an encryption).
* State2 is 16 different random bytes that are different for each round ShiftRows and MixColumns operations
;*---------------------------------------------------------------------------
;* HISTORY:
;*----------------------------------------------------------------------------
;* V 1.0 | 01/05/19 | Initial version of the AES library.
;*****************************************************************************
*/

/* Import the ASM abstraction layer */
#define ASM
#include "compiler_abstraction.h"
#include "affine_aes.h"

	DECLARE_GLOB_VAR(SEC_OK)
SET_GLOB_VAR(SEC_OK, 0)

/********* Read only data section ****************/
	RODATASECT

// Original AES Sbox
sbox MYLABEL
    MYBYTE    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76
    MYBYTE    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0
    MYBYTE    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15
    MYBYTE    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75
    MYBYTE    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84
    MYBYTE    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf
    MYBYTE    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8
    MYBYTE    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2
    MYBYTE    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73
    MYBYTE    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb
    MYBYTE    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79
    MYBYTE    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08
    MYBYTE    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a
    MYBYTE    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e
    MYBYTE    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf
    MYBYTE    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16

// Original AES inverse-Sbox
Invsbox MYLABEL
    MYBYTE    0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB
    MYBYTE    0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB
    MYBYTE    0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E
    MYBYTE    0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25
    MYBYTE    0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92
    MYBYTE    0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84
    MYBYTE    0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06
    MYBYTE    0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B
    MYBYTE    0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73
    MYBYTE    0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E
    MYBYTE    0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B
    MYBYTE    0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4
    MYBYTE    0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F
    MYBYTE    0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF
    MYBYTE    0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61
    MYBYTE    0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D

// table representing the ShiftRows transformation for encryption
t_shiftRows MYLABEL MYBYTE 0,13,10,7,4,1,14,11,8,5,2,15,12,9,6,3

// table representing the ShiftRows transformation for decryption
t_shiftRows_D MYLABEL MYBYTE 0,5,10,15,4,9,14,3,8,13,2,7,12,1,6,11

//Table used for key derivation
rcon MYLABEL
	MYBYTE    0x01,0x00,0x00,0x00
	MYBYTE    0x02,0x00,0x00,0x00
	MYBYTE    0x04,0x00,0x00,0x00
	MYBYTE    0x08,0x00,0x00,0x00
	MYBYTE    0x10,0x00,0x00,0x00
	MYBYTE    0x20,0x00,0x00,0x00
	MYBYTE    0x40,0x00,0x00,0x00
	MYBYTE    0x80,0x00,0x00,0x00
	MYBYTE    0x1b,0x00,0x00,0x00
	MYBYTE    0x36,0x00,0x00,0x00

// Table representing the field multiplication by 2 in GF(256)
xtime MYLABEL
	MYBYTE 0x00,0x02,0x04,0x06,0x08,0x0a,0x0c,0x0e,0x10,0x12,0x14,0x16,0x18,0x1a,0x1c,0x1e
	MYBYTE 0x20,0x22,0x24,0x26,0x28,0x2a,0x2c,0x2e,0x30,0x32,0x34,0x36,0x38,0x3a,0x3c,0x3e
	MYBYTE 0x40,0x42,0x44,0x46,0x48,0x4a,0x4c,0x4e,0x50,0x52,0x54,0x56,0x58,0x5a,0x5c,0x5e
	MYBYTE 0x60,0x62,0x64,0x66,0x68,0x6a,0x6c,0x6e,0x70,0x72,0x74,0x76,0x78,0x7a,0x7c,0x7e
	MYBYTE 0x80,0x82,0x84,0x86,0x88,0x8a,0x8c,0x8e,0x90,0x92,0x94,0x96,0x98,0x9a,0x9c,0x9e
	MYBYTE 0xa0,0xa2,0xa4,0xa6,0xa8,0xaa,0xac,0xae,0xb0,0xb2,0xb4,0xb6,0xb8,0xba,0xbc,0xbe
	MYBYTE 0xc0,0xc2,0xc4,0xc6,0xc8,0xca,0xcc,0xce,0xd0,0xd2,0xd4,0xd6,0xd8,0xda,0xdc,0xde
	MYBYTE 0xe0,0xe2,0xe4,0xe6,0xe8,0xea,0xec,0xee,0xf0,0xf2,0xf4,0xf6,0xf8,0xfa,0xfc,0xfe
	MYBYTE 0x1b,0x19,0x1f,0x1d,0x13,0x11,0x17,0x15,0x0b,0x09,0x0f,0x0d,0x03,0x01,0x07,0x05
	MYBYTE 0x3b,0x39,0x3f,0x3d,0x33,0x31,0x37,0x35,0x2b,0x29,0x2f,0x2d,0x23,0x21,0x27,0x25
	MYBYTE 0x5b,0x59,0x5f,0x5d,0x53,0x51,0x57,0x55,0x4b,0x49,0x4f,0x4d,0x43,0x41,0x47,0x45
	MYBYTE 0x7b,0x79,0x7f,0x7d,0x73,0x71,0x77,0x75,0x6b,0x69,0x6f,0x6d,0x63,0x61,0x67,0x65
	MYBYTE 0x9b,0x99,0x9f,0x9d,0x93,0x91,0x97,0x95,0x8b,0x89,0x8f,0x8d,0x83,0x81,0x87,0x85
	MYBYTE 0xbb,0xb9,0xbf,0xbd,0xb3,0xb1,0xb7,0xb5,0xab,0xa9,0xaf,0xad,0xa3,0xa1,0xa7,0xa5
	MYBYTE 0xdb,0xd9,0xdf,0xdd,0xd3,0xd1,0xd7,0xd5,0xcb,0xc9,0xcf,0xcd,0xc3,0xc1,0xc7,0xc5
	MYBYTE 0xfb,0xf9,0xff,0xfd,0xf3,0xf1,0xf7,0xf5,0xeb,0xe9,0xef,0xed,0xe3,0xe1,0xe7,0xe5

// Table representing the field multiplication by 4 in GF(256)
fourtime MYLABEL
	MYBYTE 0x0 , 0x4 , 0x8 , 0xc , 0x10 , 0x14 , 0x18 , 0x1c , 0x20 , 0x24 , 0x28 , 0x2c , 0x30 , 0x34 , 0x38 , 0x3c
	MYBYTE 0x40 , 0x44 , 0x48 , 0x4c , 0x50 , 0x54 , 0x58 , 0x5c , 0x60 , 0x64 , 0x68 , 0x6c , 0x70 , 0x74 , 0x78 , 0x7c
	MYBYTE 0x80 , 0x84 , 0x88 , 0x8c , 0x90 , 0x94 , 0x98 , 0x9c , 0xa0 , 0xa4 , 0xa8 , 0xac , 0xb0 , 0xb4 , 0xb8 , 0xbc
	MYBYTE 0xc0 , 0xc4 , 0xc8 , 0xcc , 0xd0 , 0xd4 , 0xd8 , 0xdc , 0xe0 , 0xe4 , 0xe8 , 0xec , 0xf0 , 0xf4 , 0xf8 , 0xfc
	MYBYTE 0x1b , 0x1f , 0x13 , 0x17 , 0xb , 0xf , 0x3 , 0x7 , 0x3b , 0x3f , 0x33 , 0x37 , 0x2b , 0x2f , 0x23 , 0x27
	MYBYTE 0x5b , 0x5f , 0x53 , 0x57 , 0x4b , 0x4f , 0x43 , 0x47 , 0x7b , 0x7f , 0x73 , 0x77 , 0x6b , 0x6f , 0x63 , 0x67
	MYBYTE 0x9b , 0x9f , 0x93 , 0x97 , 0x8b , 0x8f , 0x83 , 0x87 , 0xbb , 0xbf , 0xb3 , 0xb7 , 0xab , 0xaf , 0xa3 , 0xa7
	MYBYTE 0xdb , 0xdf , 0xd3 , 0xd7 , 0xcb , 0xcf , 0xc3 , 0xc7 , 0xfb , 0xff , 0xf3 , 0xf7 , 0xeb , 0xef , 0xe3 , 0xe7
	MYBYTE 0x36 , 0x32 , 0x3e , 0x3a , 0x26 , 0x22 , 0x2e , 0x2a , 0x16 , 0x12 , 0x1e , 0x1a , 0x6 , 0x2 , 0xe , 0xa
	MYBYTE 0x76 , 0x72 , 0x7e , 0x7a , 0x66 , 0x62 , 0x6e , 0x6a , 0x56 , 0x52 , 0x5e , 0x5a , 0x46 , 0x42 , 0x4e , 0x4a
	MYBYTE 0xb6 , 0xb2 , 0xbe , 0xba , 0xa6 , 0xa2 , 0xae , 0xaa , 0x96 , 0x92 , 0x9e , 0x9a , 0x86 , 0x82 , 0x8e , 0x8a
	MYBYTE 0xf6 , 0xf2 , 0xfe , 0xfa , 0xe6 , 0xe2 , 0xee , 0xea , 0xd6 , 0xd2 , 0xde , 0xda , 0xc6 , 0xc2 , 0xce , 0xca
	MYBYTE 0x2d , 0x29 , 0x25 , 0x21 , 0x3d , 0x39 , 0x35 , 0x31 , 0xd , 0x9 , 0x5 , 0x1 , 0x1d , 0x19 , 0x15 , 0x11
	MYBYTE 0x6d , 0x69 , 0x65 , 0x61 , 0x7d , 0x79 , 0x75 , 0x71 , 0x4d , 0x49 , 0x45 , 0x41 , 0x5d , 0x59 , 0x55 , 0x51
	MYBYTE 0xad , 0xa9 , 0xa5 , 0xa1 , 0xbd , 0xb9 , 0xb5 , 0xb1 , 0x8d , 0x89 , 0x85 , 0x81 , 0x9d , 0x99 , 0x95 , 0x91
	MYBYTE 0xed , 0xe9 , 0xe5 , 0xe1 , 0xfd , 0xf9 , 0xf5 , 0xf1 , 0xcd , 0xc9 , 0xc5 , 0xc1 , 0xdd , 0xd9 , 0xd5 , 0xd1

// Table representing the field multiplication by 8 in GF(256)
eighttime MYLABEL
	MYBYTE 0x0 , 0x8 , 0x10 , 0x18 , 0x20 , 0x28 , 0x30 , 0x38 , 0x40 , 0x48 , 0x50 , 0x58 , 0x60 , 0x68 , 0x70 , 0x78
	MYBYTE 0x80 , 0x88 , 0x90 , 0x98 , 0xa0 , 0xa8 , 0xb0 , 0xb8 , 0xc0 , 0xc8 , 0xd0 , 0xd8 , 0xe0 , 0xe8 , 0xf0 , 0xf8
	MYBYTE 0x1b , 0x13 , 0xb , 0x3 , 0x3b , 0x33 , 0x2b , 0x23 , 0x5b , 0x53 , 0x4b , 0x43 , 0x7b , 0x73 , 0x6b , 0x63
	MYBYTE 0x9b , 0x93 , 0x8b , 0x83 , 0xbb , 0xb3 , 0xab , 0xa3 , 0xdb , 0xd3 , 0xcb , 0xc3 , 0xfb , 0xf3 , 0xeb , 0xe3
	MYBYTE 0x36 , 0x3e , 0x26 , 0x2e , 0x16 , 0x1e , 0x6 , 0xe , 0x76 , 0x7e , 0x66 , 0x6e , 0x56 , 0x5e , 0x46 , 0x4e
	MYBYTE 0xb6 , 0xbe , 0xa6 , 0xae , 0x96 , 0x9e , 0x86 , 0x8e , 0xf6 , 0xfe , 0xe6 , 0xee , 0xd6 , 0xde , 0xc6 , 0xce
	MYBYTE 0x2d , 0x25 , 0x3d , 0x35 , 0xd , 0x5 , 0x1d , 0x15 , 0x6d , 0x65 , 0x7d , 0x75 , 0x4d , 0x45 , 0x5d , 0x55
	MYBYTE 0xad , 0xa5 , 0xbd , 0xb5 , 0x8d , 0x85 , 0x9d , 0x95 , 0xed , 0xe5 , 0xfd , 0xf5 , 0xcd , 0xc5 , 0xdd , 0xd5
	MYBYTE 0x6c , 0x64 , 0x7c , 0x74 , 0x4c , 0x44 , 0x5c , 0x54 , 0x2c , 0x24 , 0x3c , 0x34 , 0xc , 0x4 , 0x1c , 0x14
	MYBYTE 0xec , 0xe4 , 0xfc , 0xf4 , 0xcc , 0xc4 , 0xdc , 0xd4 , 0xac , 0xa4 , 0xbc , 0xb4 , 0x8c , 0x84 , 0x9c , 0x94
	MYBYTE 0x77 , 0x7f , 0x67 , 0x6f , 0x57 , 0x5f , 0x47 , 0x4f , 0x37 , 0x3f , 0x27 , 0x2f , 0x17 , 0x1f , 0x7 , 0xf
	MYBYTE 0xf7 , 0xff , 0xe7 , 0xef , 0xd7 , 0xdf , 0xc7 , 0xcf , 0xb7 , 0xbf , 0xa7 , 0xaf , 0x97 , 0x9f , 0x87 , 0x8f
	MYBYTE 0x5a , 0x52 , 0x4a , 0x42 , 0x7a , 0x72 , 0x6a , 0x62 , 0x1a , 0x12 , 0xa , 0x2 , 0x3a , 0x32 , 0x2a , 0x22
	MYBYTE 0xda , 0xd2 , 0xca , 0xc2 , 0xfa , 0xf2 , 0xea , 0xe2 , 0x9a , 0x92 , 0x8a , 0x82 , 0xba , 0xb2 , 0xaa , 0xa2
	MYBYTE 0x41 , 0x49 , 0x51 , 0x59 , 0x61 , 0x69 , 0x71 , 0x79 , 0x1 , 0x9 , 0x11 , 0x19 , 0x21 , 0x29 , 0x31 , 0x39
	MYBYTE 0xc1 , 0xc9 , 0xd1 , 0xd9 , 0xe1 , 0xe9 , 0xf1 , 0xf9 , 0x81 , 0x89 , 0x91 , 0x99 , 0xa1 , 0xa9 , 0xb1 , 0xb9

// Two Tables to process a field multplication over GF(256): a*b = alog (log(a) + log(b) mod 255)
LogTable MYLABEL
	MYBYTE   0,   0,  25,   1,  50,   2,  26, 198,  75, 199,  27, 104,  51, 238, 223,   3
	MYBYTE 100,   4, 224,  14,  52, 141, 129, 239,  76, 113,   8, 200, 248, 105,  28, 193
	MYBYTE 125, 194,  29, 181, 249, 185,  39, 106,  77, 228, 166, 114, 154, 201,   9, 120
	MYBYTE 101,  47, 138,   5,  33,  15, 225,  36,  18, 240, 130,  69,  53, 147, 218, 142
	MYBYTE 150, 143, 219, 189,  54, 208, 206, 148,  19,  92, 210, 241,  64,  70, 131,  56
	MYBYTE 102, 221, 253,  48, 191,   6, 139,  98, 179,  37, 226, 152,  34, 136, 145,  16
	MYBYTE 126, 110,  72, 195, 163, 182,  30,  66,  58, 107,  40,  84, 250, 133,  61, 186
	MYBYTE  43, 121,  10,  21, 155, 159,  94, 202,  78, 212, 172, 229, 243, 115, 167,  87
	MYBYTE 175,  88, 168,  80, 244, 234, 214, 116,  79, 174, 233, 213, 231, 230, 173, 232
	MYBYTE  44, 215, 117, 122, 235,  22,  11, 245,  89, 203,  95, 176, 156, 169,  81, 160
	MYBYTE 127,  12, 246, 111,  23, 196,  73, 236, 216,  67,  31,  45, 164, 118, 123, 183
	MYBYTE 204, 187,  62,  90, 251,  96, 177, 134,  59,  82, 161, 108, 170,  85,  41, 157
	MYBYTE 151, 178, 135, 144,  97, 190, 220, 252, 188, 149, 207, 205,  55,  63,  91, 209
	MYBYTE  83,  57, 132,  60,  65, 162, 109,  71,  20,  42, 158,  93,  86, 242, 211, 171
	MYBYTE  68,  17, 146, 217,  35,  32,  46, 137, 180, 124, 184,  38, 119, 153, 227, 165
	MYBYTE 103,  74, 237, 222, 197,  49, 254,  24,  13,  99, 140, 128, 192, 247, 112,   7

ALogTable MYLABEL
	MYBYTE   1,   3,   5,  15,  17,  51,  85, 255,  26,  46, 114, 150, 161, 248,  19,  53
	MYBYTE  95, 225,  56,  72, 216, 115, 149, 164, 247,   2,   6,  10,  30,  34, 102, 170
	MYBYTE 229,  52,  92, 228,  55,  89, 235,  38, 106, 190, 217, 112, 144, 171, 230,  49
	MYBYTE  83, 245,   4,  12,  20,  60,  68, 204,  79, 209, 104, 184, 211, 110, 178, 205
	MYBYTE  76, 212, 103, 169, 224,  59,  77, 215,  98, 166, 241,   8,  24,  40, 120, 136
	MYBYTE 131, 158, 185, 208, 107, 189, 220, 127, 129, 152, 179, 206,  73, 219, 118, 154
	MYBYTE 181, 196,  87, 249,  16,  48,  80, 240,  11,  29,  39, 105, 187, 214,  97, 163
	MYBYTE 254,  25,  43, 125, 135, 146, 173, 236,  47, 113, 147, 174, 233,  32,  96, 160
	MYBYTE 251,  22,  58,  78, 210, 109, 183, 194,  93, 231,  50,  86, 250,  21,  63,  65
	MYBYTE 195,  94, 226,  61,  71, 201,  64, 192,  91, 237,  44, 116, 156, 191, 218, 117
	MYBYTE 159, 186, 213, 100, 172, 239,  42, 126, 130, 157, 188, 223, 122, 142, 137, 128
	MYBYTE 155, 182, 193,  88, 232,  35, 101, 175, 234,  37, 111, 177, 200,  67, 197,  84
	MYBYTE 252,  31,  33,  99, 165, 244,   7,   9,  27,  45, 119, 153, 176, 203,  70, 202
	MYBYTE  69, 207,  74, 222, 121, 139, 134, 145, 168, 227,  62,  66, 198,  81, 243,  14
	MYBYTE  18,  54,  90, 238,  41, 123, 141, 140, 143, 138, 133, 148, 167, 242,  13,  23
	MYBYTE  57,  75, 221, 124, 132, 151, 162, 253,  28,  36, 108, 180, 199,  82, 246,   1

// Permutation over GF(16) used to generate a shuffling permutation for the operations order
permGFSixteen MYLABEL
	MYBYTE 0x0C, 0x05, 0x06, 0x0B, 0x09, 0x00, 0x0A, 0X0D, 0x03, 0x0E, 0x0F, 0x08, 0x04, 0x07, 0x01, 0x02
	// FOR DEBUG
	//MYBYTE 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0X07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f

/********* Read/Write data section ****************/
	DATASECT

	/* Counter for loop handling (either fully unrolled or not) */
#ifdef USE_FULL_UNROLLING 
        DECLARE_UNLOOP_VAR(count)
#else
#define count  0
#define count_ 4
#endif

/********* Executable code section ****************/
	CODESECT

/** Exported functions ***/
	EXPORT aes_init_enc
	EXPORT aes_enc
	EXPORT aes_init_dec
	EXPORT aes_dec
	EXPORT aes_loadKey

/*******************************************************************************************************************
********************************************************************************************************************
*********************************           SUB-FUNCTIONS                          *********************************
*********************************-----------------------------------------------************************************
*********************************-----------------------------------------------************************************/

/*******************************************************************************************************************
* FUNC: Compute_GTab
* DESCRIPTION: 	It computes Gtab a look-up table in RAM for the field multiplication x * rmult for x in GF(256).
*
* INPUT: 		- byte rmult (stored in register r7)
* OUTPUT: 		- Table Gtab (@ in register r3)
* MODIFIED REGISTERS: r2, r3, r4, r5, r6, r7, r8, r10, r11
* PRECONDITIONS: r7 = rmult, r3 = @GTab
* POSTCONDITIONS: none.
*******************************************************************************************************************/
Compute_GTab MYLABEL
	ldr_addr r4, LogTable				// r4 = @LogTable
	ldr_addr r5, ALogTable				// r5 = @ALogTable
	ldrb r7, [r4, r7] 		    	    // r7 = LogTable[rmult] then r7 = log (rmult)
	mov  r6, r7							// r6 = log (rmult)
	lsl  r7, r7, #8						// r7 = log (rmult)|0x00
	eor  r6, r6, r7						// r6 = log (rmult)|log (rmult)
	lsl  r7, r7, #8						// r7 = log (rmult)|0x00|0x00
	eor  r6, r6, r7						// r6 = log (rmult)|log (rmult)|log (rmult)
	lsl  r7, r7, #8						// r7 = log (rmult)|0x00|0x00|0x00
	eor  r7, r6, r7						// r7 = log (rmult)|log (rmult)|log (rmult)|log (rmult)

	mov r2, #0x01010101					// r2 = 0x01010101
	mov r10, #0							// r10 = 0

	// loop for x=1 to x=255
#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 0x40, Compute_GTab_loop)
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 0x40, Compute_GTab_loop)
	SET_REPT_INC(count)
#endif
	ldr    r6, [r4], #0x04          // r6  = log (x[3]) | log (x[2]) | log (x[1]) | log (x[0])
	uadd8  r6, r6, r7		        // r6  = r6 + (log (rmult)|log (rmult)|log (rmult)|log (rmult))
	sel    r8, r2, r10              // get the addition carry flag in r8
	uadd8  r6, r6, r8               // add the carry flag modulo 256

	mov  r8, r6                     // put log(x[0]) + log(rmult) mod 255 in r8
	and  r8, #0xFF
	ldrb r8,[r5, r8]	 	        // r8 = x[0] * rmult
	mov  r11, r8                    // r11 = 00 | 00 | 00 | x[0] * rmult

	mov  r8, r6, LSR #8             // put log(x[1]) + log(rmult) mod 255 in r8
	and  r8, #0xFF
	ldrb r8,[r5, r8]    	        // r8 = x * rmult
	lsl  r8, r8, #8
	eor  r11, r8                    // r11 = 00 | 00 | x[1] * rmult | x[0] * rmult

	mov  r8, r6, LSR #16
	and  r8, #0xFF
	ldrb r8,[r5, r8]	    	    // r8 = x[2] * rmult
	lsl  r8, r8, #16
	eor  r11, r8

	mov  r8, r6, LSR #24
	and  r8, #0xFF
	ldrb r8,[r5, r8]		        // r8 = x * rmult
	lsl  r8, r8, #24
	eor  r11, r8
 	str  r11, [r3], #04		        // store x

	SET_REPT_CLOSE(Compute_GTab_loop)	

	sub  r3, #256                   // force the first byte of Gtab to 0
	mov  r6, #0					    // r6 = 0
	strb r6, [r3]   			    // r6 = GTab[0]; r3 = @GTab[1]

	bx LR

/*******************************************************************************************************************
* FUNC: Load_data 
* DESCRIPTION: Load the plaintext stored in  state and the masks table  stored in state2.
*
* INPUT: 	 	- Plaintext (@ in register r2)
*				- masksState 
* OUTPUT: 		- Initializes state, state2
* MODIFIED REGISTERS: r2, r3, r5, r6.
* PRECONDITIONS: r2 = @Plaintext.
* POSTCONDITIONS: None.
*******************************************************************************************************************/
Load_data MYLABEL
	ldr_struct_field_addr(r3, 0, aes_ctx_state)			// r3 = @state
	ldr_struct_field_addr(r5, 0, aes_ctx_masksState)	// r5 = @masksState
	ldr_struct_field_addr(r6, 0, aes_ctx_state2)  		// r6 = @state2
	// loop for i=0 to i=15
#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, Load_data)
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, Load_data)
	SET_REPT_INC(count)
#endif	
	ldrb r4, [r2], #1									// r4 = Plaintext[i]
	strb r4, [r3], #1									// Plaintext[i] stored in state[i]
	ldrb r4, [r5], #1									// r4 = masksState[i]
	strb r4, [r6], #1									// masksState[i] stored in state2[i]
	SET_REPT_CLOSE(Load_data)

	bx LR

/*******************************************************************************************************************
* FUNC: Load_masterKey 
* DESCRIPTION: 	Load the master key with affine masking 
*				For i from 0 to 15, key_maskedState[i] = GTab[masterkey[i]] ^ key_masksState[i]
*
* INPUT: 		None.
* OUTPUT: 		- 16 bytes table key_maskedState 
* MODIFIED REGISTERS: r1, r3, r4, r7, r8, r12
* PRECONDITIONS: None.
* POSTCONDITIONS: None.
*******************************************************************************************************************/
Load_masterKey MYLABEL
	ldr_struct_field_addr(r8, 0, key_ctx_maskedState)		// r8 = @maskedState
	ldr_struct_field_addr(r7, 0, key_ctx_masksState)    	// r7 = @masksState
	ldr_struct_field_addr(r12, 0, key_ctx_gtab)				// r12 = @Gtab
#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, Load_masterKey)
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, Load_masterKey)
	SET_REPT_INC(count)
#endif

	ldrb r4, [r7], #0x1			// r4 = key_masksState[i]
	ldrb r3, [r1], #0x1 	    // r3 = masterkey[i]
	ldrb r3, [r12, r3]			// r3 = GTab[masterkey[i]]
	eor  r4, r3, r4				// r4 = GTab[masterkey[i]] ^ key_mask[i]
	strb r4, [r8], #1			// key_maskedState[i] = GTab[masterkey[i]] ^ key_masksState[i]

	SET_REPT_CLOSE(Load_masterKey)

	bx LR

/*******************************************************************************************************************
* FUNC: Load_random_key 
* DESCRIPTION: Fill the following values from table T that has to contain 19 random bytes.
*
* INPUT: 		- 19 random bytes table T (@ in register r2)
* OUTPUT: 		- 16 bytes table key_masksState  
*				- 4 identique bytes equal to rin
*				- 4 identique bytes equal to rout 
*				- byte rmult
* MODIFIED REGISTERS: r2, r4, r5, r8
* PRECONDITIONS: r2 = @T
* POSTCONDITIONS: None.
*******************************************************************************************************************/
Load_random_key MYLABEL
	ldr_struct_field_addr(r8, 0, key_ctx_masksState)    // r8 = local variable which will contain @key_masksState
	// Load 16 random bytes in @key_mask_state
#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, Load_random_key)
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, Load_random_key)
	SET_REPT_INC(count)
#endif
	ldrb r4, [r2], #1			// r4 = T[i]
	strb r4, [r8], #1			// store r4 in @r8 (key_masksState[i] = T[i])                      
	SET_REPT_CLOSE(Load_random_key)
	
	ldr_struct_field_addr(r8, 0, key_ctx_rin)         // r8 = @rin
	ldrb r5, [r2], #1	        // r5 = T[16]
	strb r5, [r8,  #0x0]		// rin[0] = T[16]
	strb r5, [r8,  #0x1]		// rin[1] = T[16]
	strb r5, [r8,  #0x2]		// rin[2] = T[16]
	strb r5, [r8,  #0x3]		// rin[3] = T[16] then rin = T[16]|T[16]|T[16]|T[16]
	
	ldr_struct_field_addr(r8, 0, key_ctx_rout)        // r8 = @rout
	ldrb r5, [r2], #1 			// r5 = T[17]
	strb r5, [r8, #0x0]			// rout[0] = T[17]
	strb r5, [r8, #0x1]		    // rout[1] = T[17]
	strb r5, [r8, #0x2]			// rout[2] = T[17]
	strb r5, [r8, #0x3]			// rout[3] = T[17]
	
	ldr_struct_field_addr(r8, 0, key_ctx_rmult)       // r8 = @rmult
	ldrb r7, [r2] 				// r7 = T[18]
	strb r7, [r8]				// rmult = T[18]
	
	bx LR


/*******************************************************************************************************************
* FUNC: Load_random
* DESCRIPTION: 	Fill the following values from table T that has to contain 19 random bytes.
*
* INPUT: 		- 19 random bytes table T (@ in register r2)
* OUTPUT: 		- 16 bytes table aes_masksState  
*				- 4 identique bytes equal to rin
*				- 4 identique bytes equal to rout 
*				- byte rmult
* MODIFIED REGISTERS: r2, r3, r4, r5, r7.
* PRECONDITIONS:  r1 = @T
* POSTCONDITIONS: None.
*******************************************************************************************************************/
Load_random MYLABEL
	ldr_struct_field_addr(r3, 0, aes_ctx_masksState)	// r3 = @masksState
    // Load 16 random bytes in masksState
#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, Load_random)
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, Load_random)
	SET_REPT_INC(count)
#endif	
	ldrb r4, [r1], #1									// r4 = T[i]
	strb r4, [r3], #1									// store r4 in @r3 (masksState[i] = T[i])
	SET_REPT_CLOSE(Load_random)
	
	// Load rin
	ldr_struct_field_addr(r7, 0, aes_ctx_rin)			// r7 = @rin
	ldrb r5, [r1], #1		// r5 = T[16]
	strb r5, [r7, #0x0]	    // rin[0] = T[16]
	strb r5, [r7, #0x1]	    // rin[1] = T[16]
	strb r5, [r7, #0x2]	    // rin[2] = T[16]
	strb r5, [r7, #0x3]	    // rin[3] = T[16] then rin = T[16]|T[16]|T[16]|T[16]
	ldr  r5, [r7]

	// Load rout
	ldr_struct_field_addr(r7, 0, aes_ctx_rout)			// r7 = @rout 
	ldrb r5, [r1], #1	    // r5 = T[17]
	strb r5, [r7, #0x0]	    // rout[0] = T[17]
	strb r5, [r7, #0x1]	    // rout[1] = T[17]
	strb r5, [r7, #0x2]	    // rout[2] = T[17]
	strb r5, [r7, #0x3]	    // rout[3] = T[17]
	ldr  r5, [r7]

	// Load rmult
	ldr_struct_field_addr(r5, 0, aes_ctx_rmult)			// r7 = @rmult
	ldrb r7, [r1]		    // r7 = T[18]
	strb r7, [r5]		    // rmult = T[18]

	bx LR

/*******************************************************************************************************************
* FUNC: Compute_Affine_sboxMasked 
* DESCRIPTION: 	It computes the masked version of the sbox: sboxMasked(y) = box(y ^ rin)*rmult ^ rout
*               where rin and rout are two random bytes. 
*				box = sbox for aes encryption and box = invSbox for aes decryption.
*               
* INPUT: 		- byte rin (in register r1)
*				- byte rout (in register r8)
*				- Table box (@ in register r2)
*				- Multiplicative table G by rmult (@ in register r4)
* OUTPUT: 		- Table sboxMasked (@ in register r3)
* MODIFIED REGISTERS: r0, r1, r2, r3, r4, r6, r7, r8, r12.
* PRECONDITIONS: r2 = @box ; r3 = @maskedSBox; r4 = @G; r1 = rin; r8 = rout
* POSTCONDITIONS: none.
*******************************************************************************************************************/
Compute_Affine_sboxMasked MYLABEL
	mov	r12, r4 			// r12 = @G
#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 64, Compute_Affine_sboxMasked)
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 64, Compute_Affine_sboxMasked)
	SET_REPT_INC(count)
#endif

	ldr r6, [r4], #4		// r6 = (y[3+4i] * rmult)||...||(y[0+4i] * rmult)
	eor r6, r6, r1			// r6 = (y[3+4i] * rmult ^ rin)||...||(y[0+4i] * rmult ^ rin) = G(3+4i)||...|G(0+4i)|

	and  r0, r6, #0xff		// r0 = r6[0]
	ldrb r7, [r2], #1		// r7 = box(0+4i)
	ldrb r7, [r12, r7]		// r7 = box(0+4i)*rmult
	eor  r7, r8 			// r7 = (box(0+4i)*rmult ^ rout)
	strb r7, [r3, r0]		// sboxMasked [G(0+4i)] = box(0+4i)*rmult ^ rout

	lsr  r0, r6,#8			// r0 = r6 >> 8
	and  r0, r0, #0xff		// r0 = r0[0]
	ldrb r7, [r2], #1		// r7 = box(1+4i)
	ldrb r7, [r12, r7]		// r7 = box(1+4i)*rmult
	eor  r7, r8 			// r7 = (box(1+4i)*rmult ^ rout)
	strb r7, [r3, r0]		// sboxMasked [G(1+4i)] = box(1+4i)*rmult ^ rout

	lsr  r0, r6, #16
	and  r0, r0, #0xff 		// r0 =  G(2+4i)
	ldrb r7, [r2], #1		// r7 = box(2+4i)
	ldrb r7, [r12, r7]		// r7 = box(2+4i)*rmult
	eor  r7, r8 			// r7 = (box(2+4i)*rmult ^ rout)
	strb r7, [r3, r0]		// sboxMasked [G(2+4i)] = box(2+4i)*rmult ^ rout

	lsr  r0, r6, #24
	and  r0, r0, #0xff		// r0 = G(3+4i)
	ldrb r7, [r2], #1		// r7 = box(3+4i)
	ldrb r7, [r12, r7]		// r7 = box(3+4i)*rmult
	eor  r7, r8 			// r7 = (box(3+4i)*rmult ^ rout)
	strb r7, [r3, r0]		// sboxMasked [G(3+4i)] = box(3+4i)*rmult ^ rout

	SET_REPT_CLOSE(Compute_Affine_sboxMasked)

   bx LR


/*******************************************************************************************************************
* FUNC: Compute_permIndices_Tables_MC 
* DESCRIPTION: 	It computes a random permutation denoted permIndicesMC in {0,1,2,3}.
*               
* INPUT: 		- 4 bytes of randomness from state2 at position index (@ in register r4)
*				- index (in register r6)
*				
* OUTPUT: 		- 4-bytes table in permIndicesMC (in register r0)
* MODIFIED REGISTERS: r0, r2, r5, r6
* PRECONDITIONS: r0=@permIndicesMC, r4=@state2, r6 = index of randomness in state2
* POSTCONDITIONS: r4=@state2.
* NOTE: build a random permutation of {0,1,2,3} as described in Lubby-Rackoff paper.
*******************************************************************************************************************/
Compute_permIndices_Tables_MC MYLABEL
	ldrb  	r2, [r4, r6]					// r2 = Tin[r6]
	and		r2, r2, #0x3				    // extract the two lsb bits: r2 = rand = r1 | r0
	mov 	r5, #0						    // r5 = i = loop index
#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 4, Compute_permIndices_Tables_MC)
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 4, Compute_permIndices_Tables_MC)
	SET_REPT_INC(count)
#endif
	eor 	  r6,   r5, r2					// r6      = i ^ rand
	strb      r6, [r0], #1					// Tout[i] = i ^ rand 
	add       r5,   r5, #1                  // i++
	SET_REPT_CLOSE(Compute_permIndices_Tables_MC)
	bx LR

/*******************************************************************************************************************
* FUNC: Compute_permIndices_Tables 
* DESCRIPTION:	It computes two random permutations from permGFSixteen: permIndices and permIndicesBis.
* INPUT: 		- 4 random bytes from state2 (@ in register r4)
* OUTPUT: 		- permIndices (@ in register r2)
*				- permIndicesBis (@ in register r3)
* MODIFIED REGISTERS: r0, r1, r2, r3, r4, r5, r7, r8, r10, r11
* PRECONDITIONS: r4 =@state2; r2 = @permIndices; r3 = @permIndicesBis;
* POSTCONDITIONS: None.
*******************************************************************************************************************/
Compute_permIndices_Tables MYLABEL
	ldr_addr r5, permGFSixteen              // r5  = @permGF16 = p
	ldrb      r1,  [r4], #1					// r1  = rand1
	and		  r1, r1, #0x0f
	ldrb      r8,  [r4], #1					// r8  = rand2
	and		  r8, r8, #0x0f
	ldrb      r11,  [r4], #1                // r11  = rand3
	and		  r11, r11, #0x0f
	ldrb      r10, [r4], #1                 // r10 = rand4
	and		  r10, r10, #0x0f

	mov     r0, #0							// loop index i

#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, Compute_permIndices_Tables)
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, Compute_permIndices_Tables)
	SET_REPT_INC(count)
#endif
	eor  	r7, r0, r1						// r7  = i ^ rand1
	ldrb	r7, [r5, r7]					// r7  = p(i^rand1)
	eor		r7, r7, r8                     	// r7 ^= rand2
	ldrb	r7, [r5, r7]					// r7  = p(p(i^rand1)^rand2)
	eor		r7, r7, r11                     // r7 ^= rand3
	ldrb	r7, [r5, r7]        			// r7  = p(p(p(i^rand1)^rand2)^rand3)
	eor		r7, r7, r10                    	// r7  ^= ran4
	ldrb	r7, [r5, r7]					// r7  = p(p(p(p(i^rand1)^rand2)^rand3)^rand4)
	strb	r7, [r2], #1					// store r7 in @permIndices[i]
	eor		r7, r7, r1 						// r7 ^= rand1 
	ldrb	r7, [r5, r7]					// r7 = p(r7)
	eor		r7, r7, r8 						// r7 ^= rand2 
	ldrb	r7, [r5, r7]					// r7 = p(r7)
	eor		r7, r7, r11 					// r7^= rand3
	ldrb	r7, [r5, r7] 					// r7 = p(r7)
	eor		r7, r7, r10 					// r7 ^= rand4
	ldrb	r7, [r5, r7] 					// r7 = p(r7)
	strb	r7, [r3], #1					// store r7 in @permIndicesBis[i]
	add		r0, #0x1
 SET_REPT_CLOSE(Compute_permIndices_Tables)


	bx LR

/*******************************************************************************************************************
* FUNC: SubBytesWithMask 
* DESCRIPTION: 	It computes the SubBytes with the masked version of the sbox. Then each byte is masked with rout. 
*				Computation in random order specified by permIndices. Namely, for every in [0,..15], it computes
*  				sboxMasked[state[permIndices[i]] ^ rin ^ state2[permIndices[i]]]= 
*																	Sbox[unmaskedState[permIndices[i]]]*rmult ^ rout
* INPUT: 			- state (@ in register r3)
*					- state2 (@ in register r10)
* OUTPUT: 			- SubBytesWithMask(state[i] and state2[i]) ^ rout (@ in register r3)
* MODIFIED REGISTERS: r0, r3, r4, r5, r6, r7.
* PRECONDITIONS: r3 = @state, r10 = @state2, r7= @rout, r8= @sboxmasked, r5 = @permIndices, 
*				 r1 = @key_state[nb_round*16], r12 = @mask_key[nb_round*16] (unused)
* POSTCONDITIONS:r1 = @key_state[nb_round*16], r3 = @state, r10 = @state2,  r12 = @mask_key[nb_round*16]
*******************************************************************************************************************/
SubBytesWithMask MYLABEL

#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, SubBytesWithMask)
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, SubBytesWithMask)
	SET_REPT_INC(count)
#endif
	 ldrb r0, [r5], #1				// r0 = permIndices[i]
	 ldrb r4, [r3, r0]				// r4 = State[permIndices[i]]
	 ldrb r6, [r10, r0]				// r6 = State2[permIndices[i]]
	 eor  r4, r6 					// r4 = State[permIndices[i]] ^ State2[permIndices[i]]
	 ldrb r4, [r8, r4]				// r4 = sboxMasked[State[permIndices[i]] ^ State2[permIndices[i]]]
	 eor  r4, r6 					// r4 = sboxMasked[State[permIndices[i]] ^ State2[permIndices[i]]] ^ State2[permIndices[i]]
	 strb r4, [r3, r0]				// store the result r4 in State[permIndices[i]]
	
 SET_REPT_CLOSE(SubBytesWithMask)

    // Remove the mask rout from state (to just keep the masks State2[permIndices[i]])
	ldr r7, [r7]					// r7 = rout|rout|rout|rout = rout
	//it1
	ldr  r6, [r3]					// r6 = state[0;3]
	eor  r6, r7					    // r6 = state[0;3] ^ rout
	str  r6, [r3] 					// save r6 in @state[0;3] and go to the next state word

	//it2
	ldr  r6, [r3, #0x4]				// r6 = state[4;7]
	eor  r6, r7					    // r6 = state[4;7] ^ rout
	str  r6, [r3, #0x4]				// save r6 in @state[4;7] and go to the next state word

	//it3
	ldr  r6, [r3, #0x8]				// r6 = state[8;11]
	eor  r6, r7					    // r6 = state[8;11] ^ rout
	str  r6, [r3, #0x8]				// save r6 in @state[8;11] and go to the next state word

	//it4
	ldr  r6, [r3, #0xC]				// r6 = state[12;15]
	eor  r6, r7					    // r6 = state[12;15] ^ rout
	str  r6, [r3, #0xC]				// save r6 in @state[12;15] and go to the next state word

	bx LR

/*******************************************************************************************************************
* FUNC: ShiftRowsWithMask 
* DESCRIPTION:	It computes ShiftRows of state and ShiftRows of state2 in random orders.
*
* INPUT: 		- state (@ in register r3)
*				- state2 (@ in register r10)
* OUTPUT: 		- ShiftRows of state (@ in register r3)
*				- ShiftRows of state2 (@ in register r10)
* MODIFIED REGISTERS: r5, r6, r8
* PRECONDITIONS: r3 = @state, r4=@t_shiftRows, r7 = @t_temp, r10 = @state2.
* POSTCONDITIONS:
*******************************************************************************************************************/
ShiftRowsWithMasked MYLABEL
	// Process ShiftRows of state in a random order specified in aes_ctx_permIndices
	ldr_struct_field_addr(r5, 0, aes_ctx_permIndices)       // r5 = @permIndices
#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, ShiftRowsWithMasked0)
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, ShiftRowsWithMasked0)
	SET_REPT_INC(count)
#endif
	ldrb r6, [r5], #1				// get aes_ctx_permIndices[i]
	ldrb r8, [r3, r6]				// r8 = state[aes_ctx_permIndices[i]]
	ldrb r6, [r4, r6]				// r6 = t_shiftRows[aes_ctx_permIndices[i]]
	strb r8, [r7, r6]               // t_temp[t_shiftRows[aes_ctx_permIndices[i]]]=state[aes_ctx_permIndices[i]]

	SET_REPT_CLOSE(ShiftRowsWithMasked0)

	ldr_struct_field_addr(r5, 0, aes_ctx_permIndices)       // r5 = @permIndices

#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, ShiftRowsWithMasked1)
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, ShiftRowsWithMasked1)
	SET_REPT_INC(count)
#endif
	ldrb r6, [r5], #1				// get permIndices[i]
	ldrb r8, [r7, r6]				// r8 = t_temp[permIndices[i]]
	strb r8, [r3, r6]               // state[permIndices[i]] = t_temp[permIndices[i]]

	SET_REPT_CLOSE(ShiftRowsWithMasked1)

	// Process ShiftRows of state2 in a random order specified in permIndicesBis
	ldr_struct_field_addr(r5, 0, aes_ctx_permIndicesBis)       // r5 = @permIndicesBis
	// fom i = 0 to 15
#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, ShiftRowsWithMasked2)
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, ShiftRowsWithMasked2)
	SET_REPT_INC(count)
#endif
	ldrb r6, [r5], #1				// get aes_ctx_permIndicesBis[i]
	ldrb r8, [r10, r6]				// r8 = state2[permIndicesBis[i]]
	ldrb r6, [r4, r6]				// r6 = t_shiftRows[permIndicesBis[i]]
	strb r8, [r7, r6]               // t_temp[t_shiftRows[permIndicesBis[i]]] = state[permIndicesBis[i]]

	SET_REPT_CLOSE(ShiftRowsWithMasked2)

// copy t_temp in state2
	ldr_struct_field_addr(r5, 0, aes_ctx_permIndicesBis)       // r5 = @permIndicesBis
// from i = 0 to 15
#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, ShiftRowsWithMasked3)		
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, ShiftRowsWithMasked3)
	SET_REPT_INC(count)
#endif
	ldrb r6, [r5], #1				// get aes_ctx_permIndicesBis[i]
	ldrb r8, [r7, r6]				// r8 = t_temp[permIndicesBis[i]]
	strb r8, [r10, r6]              // state2[permIndicesBis[i]] = t_temp[permIndicesBis[i]]
	
	SET_REPT_CLOSE(ShiftRowsWithMasked3)

	bx LR


/*******************************************************************************************************************
* FUNC: MixColumns 
* DESCRIPTION: It computes MixColumns of a 16 bytes table T.
*        		|0x2 0x3 0x1 0x1| |w0|
*		 		|0x1 0x2 0x3 0x1| |w1|
*	T[count] = 	|0x1 0x1 0x2 0x3|*|w2|
*		 		|0x3 0x1 0x1 0x2| |w3| for count tfrom 0 to 3.
*
* INPUT: 		- Table T of 16-bytes (@ in register r10)
*        		- Table of a 4 bytes random permutation {0,1,2,3} (@ in register r0)
* OUTPUT: 		- MixColumns of T (in register r3)
* MODIFIED REGISTERS: r0, r2, r3, r4, r5, r7, r8, r11.
* PRECONDITIONS:r0 = permIndicesMC/permIndicesBisMC, r6 = @xtime, r10 = @T.
* POSTCONDITIONS: 
*******************************************************************************************************************/
MixColumns MYLABEL

#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 4, MixColumns)
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 4, MixColumns)
	SET_REPT_INC(count)
#endif
	ldrb r8, [r0], #1               // random index permIndicesMC[count] of the column to treat
	lsl  r8, r8, #2                 // r8 = 4*permIndicesMC[count]
	add  r3, r10, r8               	// r3 = T[4*permIndicesMC[count]] =  count-th column of T[count] = [w0 w1 w2 w3]**t

	ldrb r8, [r3]					// r8 = w0
	ldrb r11, [r3, #0x1] 			// r11 = w1
	eor  r4, r8, r11				// r4 = w0 ^ w1
	ldrb r2, [r3, #0x2]			    // r2 = w2
	eor  r4, r4, r2					// r4 = w0 ^ w1 ^ w2
	ldrb r5, [r3, #0x3]				// r5 = w3
	eor  r4, r4, r5					// r4 = w0 ^ w1 ^ w2 ^ w3 = t

	eor r7, r8, r11					// r7 = w0+w1
	ldrb r7, [r6, r7]				// r7 = xtime[w0+w1] = 2(w0+w1)

	eor r7, r8						// r7 = 2*(w0+w1)+w0
	eor r7, r4						// r7 = 2*(w0+w1)+w0+t
	strb r7, [r3], #0x1				// store in state
	eor r7, r11, r2 				// r7 = w1+w2

	ldrb r7, [r6, r7]				// r7 = xtime[w1+w2] = 2(w1+w2)
	eor r7, r11						// r7 = 2*(w1+w2)+w1
	eor r7, r4						// r7 = 2*(w1+w2)+w1+t
	strb r7, [r3], #0x1				// store in state

	eor r7, r2, r5					// r7 = w2+w3

	ldrb r7, [r7, r6]				// r7 = xtime[w2+w3] = 2(w2+w3)

	eor r7, r2						// r7 = 2*(w2+w3)+w2
	eor r7, r4						// r7 = 2*(w2+w3)+w2+t
	strb r7, [r3], #0x1

	eor r7, r8, r5					// r7 = w0+w3

	ldrb r7, [r6, r7]				// r7 = xtime[w0+w3] = 2*(w0+w3)

	eor r7, r5						// r7 = 2*(w0+a4)+w3
	eor r7, r4						// r7 = 2*(w0+a4)+w3+t
	strb r7, [r3]

	SET_REPT_CLOSE(MixColumns)

	bx LR


/*******************************************************************************************************************
* FUNC: InvMixColumns_part2 (ok)
* DESCRIPTION: 	It computes the second part of InvMixColumns on w0w1w2w3 for column 0 to 3 (count) and store 
* 				the result in t_temp.
* 				|0xC 0x8 0xC 0x8| |w0|		t_temp[count][0]
*				|0x8 0xC 0x8 0xC| |w1|		t_temp[count][1]
*				|0xC 0x8 0xC 0x8|*|w2| = 	t_temp[count][2]
*				|0x8 0xC 0x8 0xC| |w3|		t_temp[count][3] where count = 0 to 3.
* INPUT: 		- Table of 16 bytes T (@ in register r10)
*				- Table of a 4 bytes random permutation {0,1,2,3} (register r0)
* OUTPUT: 		- Table of 16 bytes t_temp[i][j] (@ in register r2)
* MODIFIED REGISTERS: r0, r2, r3, r4, r5, r8, r11.
* PRECONDITIONS: r0 = @permIndicesMC/permIndicesBisMC, r10 = @T.
* POSTCONDITIONS: None.
*******************************************************************************************************************/
InvMixColumns_part2 MYLABEL

// For count = 0 to 3 
#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 4, InvMixColumns_part2)
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 4, InvMixColumns_part2)
	SET_REPT_INC(count)
#endif
	ldr_struct_field_addr(r2, 0, aes_ctx_t_temp)	// r2 = @t_temp 
	ldrb r8, [r0], #1               // random index permIndicesMC[count] of the column to treat
	lsl  r8, r8, #2                 // r8 = 4*permIndicesMC[[count]]
	add  r3, r10, r8               	// r3 = @T[4*permIndicesMC[[count]]] = count-th column of T = [w0 w1 w2 w3]**t
	add  r2, r2, r8					// r2 = @t_temp[4*permIndicesMC[[count]]]

	ldrb r8, [r3]					// r8 = w0
	ldrb r11, [r3, #0x1] 			// r11 = w1
	eor r4, r8, r11					// r4 = w0 ^ w1
	ldrb r6, [r3, #0x2]			    // r6 = w2
	eor r4, r4, r6					// r4 = w0 ^ w1 ^ w2
	ldrb r5, [r3, #0x3]				// r5 = w3
	eor r8, r8, r6					// r8 = w0 ^ w2

	eor r4, r4, r5					// r4 = w0 ^ w1 ^ w2 ^ w3
	ldr_addr r6, eighttime			// r6 = @eighttime
	ldrb r4, [r6, r4]				// r4 = eighttime[w0^w1^w2^w3]

	ldr_addr r6, fourtime			// r4 = @fourtime
	ldrb r8, [r6, r8]				// r8 = fourtime[w0 + w2]
	eor r8, r8, r4 					// r8 = fourtime[w0 + w2] + t

	strb r8, [r2], #0x2				// store in t_temp[count][0]
	strb r8, [r2]					// store in t_temp[count][2]
	sub r2, r2, #0x1				// come back to index 1

	eor r8, r11, r5					// r8 = w1 + w3
	ldrb r8, [r6, r8]				// r8 = fourtime[w1 + w3]
	eor r8, r8, r4					// r8 = fourtime[w1 + w3] + t

	strb r8, [r2], #0x2				// store in t_temp[count][1]
	strb r8, [r2]					// store in t_temp[count][3]

	SET_REPT_CLOSE(InvMixColumns_part2)

	bx LR

/*******************************************************************************************************************
* FUNC: KeyExpansion_masked 
* DESCRIPTION:	It performs the masked key schedule: for each round r, it generates a masked derivated key and  
*				it updates the corresponding mask masksState[16*(r+1)].
*
* INPUT: 		- maskedState (@ in register r1)
*				- masksState (@ in register r7)
* OUTPUT: 		- masksState[16*(r+1)] of masksState (@ in register r7)
*
* MODIFIED REGISTERS: r0, r1, r4, r5, r6, r8, r10, r11 r12
* PRECONDITIONS: r1 = @maskedState[r*16]; r2 = @rcon_mult; r3 = @permIndicesMC;
* 				 r5 = @maskedSBox; r7 = @masksState; r10 = rout; r11 = rin.
* POSTCONDITIONS:r3 = @permIndicesMC.
*******************************************************************************************************************/
KeyExpansion_masked MYLABEL				// @r6 contains the i-th key ki = w0|w1|w2|w3
	//RotWord() for maskedState
	ldr r8, [r1,#0xC]					// r8 = w(i-1) (last word of ki) = w(i-1)[3]|w(i-1)[2]|w(i-1)[1]|w(i-1)[0]
	ror r8, #0x8						// r8 = w(i-1)[0]|w(i-1)[3]|w(i-1)[2]|w(i-1)[1] (right rotation of 8 bits) = temp

	str r8, [r1, #0x10]!				// r1 = @maskedState[rnd*16*8] +16*8 =@maskedState[(rnd+1)*16*8] Go to the next @key in key state
										// store r8 (four bytes of the derivated key) in @wi

	// RotWord() for mask key state (wm : "w mask")
	ldr r8, [r7, #0xC]					// r8 = wm(i-1) (last word of mask_key_i) = wm(i-1)[3]|wm(i-1)[2]|wm(i-1)[1]|wm(i-1)[0]
	ror r8, #0x8						// r8 = wm(i-1)[0]|wm(i-1)[3]|wm(i-1)[2]|wm(i-1)[1] (rotation right 8 bits) = temp_mask
	mov r6, r7
	add r6, r6, #0x10					// r6 = @maskedState[round*16] @derivated key = @wmi
	str r8, [r6]						// store r8 (four bytes of the derivated maskedState) in @wmi

	// SubWord()
	// temp = w(i-1) = temp[0]temp[1]temp[2]temp[3]
	// temp_mask = wm(i-1) = temp_mask[0] temp_mask[1] temp_mask[2] temp_mask[3]
	// j position in key word temp j in {0, 1, 2, 3}

#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 4, KeyExpansion_masked)
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 4, KeyExpansion_masked)
	SET_REPT_INC(count)
#endif
	ldrb r4, [r3], #1 				// r4 = permIndicesMC[j]
	ldrb r12, [r1, r4]				// r12 = temp[permIndicesMC[j]] (first bytes stored in keystate[i*4])
	eor r12, r12, r11				// r12 = temp[permIndicesMC[j]] ^ rin = temp[permIndicesMC[j]]
 	ldrb r0, [r6, r4]				// r0  = temp_mask[permIndicesMC[j]]
	eor r12, r12, r0				// r12 = temp[permIndicesMC[j]] ^ temp_mask[permIndicesMC[j]] = temp[permIndicesMC[j]]
	ldrb r12, [r5, r12]				// r12 = sboxMasked[temp[permIndicesMC[j]]]= temp[permIndicesMC[j]]
 	eor r12, r12, r0				// r12 = temp[permIndicesMC[j]] ^ temp_mask[permIndicesMC[j]]
 	eor r12, r12, r10				// r12 = temp[permIndicesMC[j]] ^ rout = temp[permIndicesMC[j]]
	strb r12, [r1, r4]	 			// Store temp[permIndicesMC[j]] in maskedState[rnd*16*8+j*8]

	SET_REPT_CLOSE(KeyExpansion_masked)

	// Process for maskedState (computation of wi)
	mov r12, r1						// r12 = @maskedState[round*16] @derivated key = @temp (@wi)
	ldr r4, [r12]					// r4 = temp
	ldr r8, [r2],#0x4				// r8 = load the four bytes of rcon_rnd
	eor r4, r8						// r4 = temp ^ rcon_rnd
	sub r8, r12, #0x10				// r8 = @w(i-4)
	ldr r8, [r8]					// r8 = w(i-4)
	eor r4, r8						// r4 = w(i-4) ^ temp = temp
	sub r6, r12, #0xc				// r6 = @w(i-3) (three columns before)
	str r4, [r12],#0x4				// store r4 in @wi; r12 = @maskedState [round* 16*8 * rnd]
	
	// computation of w(i+1)
	ldr r8, [r6], #0x4           	// r8 = w(i-3) ; r7 = @w(i-3)
	eor r4, r8						// r4 = temp ^ w(i-3) = temp
	str r4, [r12],#0x4				// store temp in @maskedState [round* 16*8 * rnd + 8]
	
	// computation of w(i+2)
	ldr r8, [r6], #0x4           	// r8 = w(i-2) ; r7 = @w(i-2)
	eor r4, r8						// r4 = temp ^ w(i-2) = temp
	str r4, [r12], #0x4				// store temp in @maskedState [round* 16*8 * rnd + 8*2]
	
	// computation of w(i+3)
	ldr r8, [r6], #0x4           	// r8 = w(i-1) ; r7 = @w(i-1)
	eor r4, r8						// r4 = temp ^ w(i-1) = temp
	str r4, [r12]					// store temp in @maskedState [round* 16*8 * rnd + 8*3]
	
	// Process for mask_key (computation of wmi) 
	ldr r4, [r7, #0x10]				// r4 = temp load current key word;
	ldr r8, [r7]					// r8 = @temp_mask
	eor r4, r8						// r4 = wm(i-4) ^ temp_mask = temp_mask
	str r4, [r7, #0x10]				// store r4 in @wmi; r12 = @masksState[round* 16*8 * rnd]
	
 	//computation of wm(i+1)
	ldr r8, [r7, #0x4]     			// r8 = wm(i-3)
	eor r4, r8						// r4 = temp_mask ^ wm(i-3) = temp_mask
	str r4, [r7, #0x14]				// store temp_mask in @masksState[round* 16*8 * rnd + 8*1]

	//computation of wm(i+2)
	ldr r8, [r7, #0x8]				// r8 = wm(i-2) ; r7 = @wm(i-2)
	eor r4, r8						// r4 = temp_mask ^ w(i-2) = temp_mask
	str r4, [r7, #0x18]				// store temp_mask in @masksState[round* 16*8 * rnd + 8*2]
	
	//computation of wm(i+3)
	ldr r8, [r7, #0xC]				// r8 = wm(i-1) ; r7 = @wm(i-1)
	eor r4, r8						// r4 = temp_mask ^ w(i-1) = temp_mask
	str r4, [r7, #0x1C]				// store temp_mask in @masksState[round* 16*8 * rnd + 8*3]
	
	sub r3, r3, #4

	bx LR
	
/*******************************************************************************************************************
* FUNC: field_division
* DESCRIPTION: 	It computes x/y in GF(256) for AES field representation, assuming all elements in GF(256) take the          
*				form z**i for i in [0..254], we have x / y = z**{a-b mod 255} for a = log_z(x) and b = log_z(y).  
*
* INPUT: 		- z**b (@ in register r2)
*				- z**a (@ in regsiter r3)
* OUTPUT: 		- x/y in GF(256) (@ in register r7)
* MODIFIED REGISTERS: r0, r1, r2, r3, r10, r7
* PRECONDITIONS: r2 = @b, r3 = @a, x and y are non-zero.
* POSTCONDITIONS: r7 = x/y
*******************************************************************************************************************/
field_division MYLABEL
	ldr_addr r0, LogTable				                           // r0 = @LogTable
   	ldr_addr r1, ALogTable				                           // r1 = @ALogTable
	
	ldrb r2, [r2]					                           	   // r2 = z**b
	ldrb r2, [r0,r2]				                               // r2 = LogTable[z**b]=b
	eor  r2, r2, #0xFF				                               // r2 = 255 - b
	
	ldrb r3, [r3]		   			                               // r3 = z**a
	ldrb r10, [r0, r3]	   			                               // r10 = LogTable[z**a] = a  
	
	add  r10, r10, r2		   		                               // r10 = a - b
	lsr  r2, r10, #8        			                           // r2 = 1 if r10>255 and 0 otherwise; r2 = carry
	add  r10, r2            			                           // r10 = a - b + carry
	and  r10, r10, #0xFF          			                       // r10 = a - b mod (255)
	ldrb r7, [r1, r10]			   	                               // r7 = Alog[a - b mod (255)] = z**{a-b mod 255}

	bx LR


/*******************************************************************************************************************
* FUNC: Xor_states 
* DESCRIPTION: 	Xor between two tables of 16 bytes (state_a and state_b) stored in table state_a.
*				
* INPUT: 		- state_a (@ in register r3)
*				- state_b (@ in register r7)
* OUTPUT: 		- state_a (@ in register r3)
* MODIFIED REGISTERS: r3, r6, r7, r8
* PRECONDITIONS: r3 = @state_a; r7 = @state_b
* POSTCONDITIONS: r3 = @state_a = @(state_a ^ state_b)
* Note: ldr/str, ldrm/strm are not used to be deal with misalignement issues.
*******************************************************************************************************************/
Xor_states MYLABEL
// For i = 0 to i = 15
#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, Xor_states)
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, Xor_states)
	SET_REPT_INC(count)
#endif
	ldrb  r6, [r3]				// r6 = state_a[i]
	ldrb  r8, [r7], #1			// r8 = state_b[i]
	eor   r6, r6, r8 			// r6 = state_a[i] ^ state_b[i]
	strb  r6, [r3], #1			// store r6 in state_a[i]
	SET_REPT_CLOSE(Xor_states)
	
	sub r3, r3, #16				// restore the  @ of state_a 
	bx LR

/*******************************************************************************************************************
* FUNC: State_copy 
* DESCRIPTION: It copies the content of a 16-bytes table T_in into T_out.
* INPUT: 		- Tin (@ in register r3)
*				- Tout (@ in register r7)
* OUTPUT: 		- Tout (@ in register r7)
* MODIFIED REGISTERS: r5, r3, r7
* PRECONDITIONS: r3 = @T_in, r7 = @T_out
* POSTCONDITIONS: None.
*******************************************************************************************************************/
State_copy MYLABEL
 	// Unroll: save_result_Loop // for j = 0 to j = 15
#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, save_result_Loop)
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, save_result_Loop)
	SET_REPT_INC(count)
#endif
	// save state value in @result
	ldrb r5, [r3], #1				// r5 = T_in[i]
	strb r5, [r7], #1				// store r5 in @T_out[i]
	SET_REPT_CLOSE(save_result_Loop)
	bx LR
	
/*******************************************************************************************************************
* FUNC: Map_in_G
* DESCRIPTION: 	For a 16 bytes table state it computes state[i] = GTab[state[i]].
* INPUT: 		- state (@ in register r3)
* OUTPUT: 		- state = Gtab(state) (@ in register r3)
* MODIFIED REGISTERS: r3, r6
* PRECONDITIONS: r12 = @GTab, r3 = @state.
* POSTCONDITIONS: None.
*******************************************************************************************************************/
Map_in_G MYLABEL

#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, Map_in_G)
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, Map_in_G)
	SET_REPT_INC(count)
#endif
	ldrb r6, [r3] 					// r6 = state[i]
	ldrb r6, [r12, r6]				// r6 = Gtab[state[i]]
	strb r6, [r3], #0x1				// store r6 in @state[i]

	SET_REPT_CLOSE(Map_in_G)

	bx LR


/*******************************************************************************************************************
* FUNC: Multiplicative_unmasking 
* DESCRIPTION: 	It computes  T = z^{-a} * T for an array T and a field element z^{-a}.
* INPUT: 		- Table T (@ in register r7) 
*				- Value -a (@ in register r2)
* OUTPUT: 		- Computed Table T (@ in register r7) 
* MODIFIED REGISTERS: r4, r6, r7, r10, r11
* PRECONDITIONS: r0 = LogTable, r1 = ALogTable, r7 = @T, r2 = @-a
* POSTCONDITIONS: None.
*******************************************************************************************************************/
Multiplicative_unmasking MYLABEL
#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, Multiplicative_unmasking)
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, Multiplicative_unmasking)
	SET_REPT_INC(count)
#endif
	ldrb r11, [r7]		   			// r11 = T[j] = z**b
	ldrb r10, [r11, r0]	   			// r10 = LogTable[T[j]] = b 
	add  r10, r10, r2		   		// r10 = b - a
	lsr  r4, r10, #8        		// r4 = 1 if r5>255 and 0 otherwise; r11 = carry
	add  r10, r4            		// r10 = b - a + carry
	and  r10,r10,r12          		// r10 = b - a mod (255)
	ldrb r6,[r10, r1]			   	// r6 = Alog[b - a mod (255)] = z**{b - a mod (255)}

	// we suppose that a!=0
	cmp r11, #0x0					// >254
	it gt
	movgt r10, #0xFF
	it eq
	moveq r6, #0x0
	it lt
	movlt r10, #0x00
	strb r6, [r7], #0x1				// store r6 in T[j]
	SET_REPT_CLOSE(Multiplicative_unmasking)

	bx LR

/*******************************************************************************************************************
* FUNC: Map_in_G_rcon_key 
* DESCRIPTION : It computes rcon table multiplied bu  rcon_mult[i]= Gtab(rcon[i])
* INPUT: 		-rcon_mult (non computed) (@ in register r12)
* OUTPUT: 		-Table rcon_mult computed (@ in register r12)
* MODIFIED REGISTERS: r4, r6, r8.
* PRECONDITIONS: r12 = @Gtab
* POSTCONDITIONS: None.
*******************************************************************************************************************/
Map_in_G_rcon_key MYLABEL
	ldr_addr r4, rcon  								// r4 = @rcon
	ldr_struct_field_addr(r8, 0, key_ctx_rcon_mult)	// r8 = rcon_mult

	// Loop for i in [0..9]
#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 10, Map_in_G_rcon_key)
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 10, Map_in_G_rcon_key)
	SET_REPT_INC(count)
#endif
	ldrb r6, [r4], #0x4								// r6 = rcon[i] 
	ldrb r6, [r12, r6]								// r6 = Gtab[rcon[i]]
	str  r6, [r8], #0x4							    // store r6 in @rcon_mult[i]

	SET_REPT_CLOSE(Map_in_G_rcon_key)

	bx LR


/*******************************************************************************************************************
* FUNC: Add_roundkey_masked 
* DESCRIPTION:	Xor between state and the derivated key multiplied by the byte key_rmult. I computes state bytes xi   
*             	such that GTab(state_unmasked[i]) = aes_rmult * state_unmasked[i] ^ aes_rin.
* INPUT: 		- aes_state = state (@ in register r3), 
*				- state_key (@ in register r8)
*				- key_masksState = mask_key (@ in register r12)
*				- permIndices (@ in register r5)
* OUTPUT: 		- state
* MODIFIED REGISTERS: r4, r5, r6, r11 
* PRECONDITIONS: r0 = GTab_key, r3 = @state, r5 = permIndices, r8 = @state_key, r12 = key_masksState			 
* POSTCONDITIONS: r12 = @key_masksState
*******************************************************************************************************************/
Add_roundkey_masked MYLABEL
	ldr_struct_field_addr(r2, 0, aes_ctx_state2)		// r2 = @aes_state2
// Loop for i in [0..15]
#ifdef USE_FULL_UNROLLING
SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, Add_roundkey_masked)
SET_REPT_INC(count)
#else
	SET_REPT_START(count, 1)
        SET_REPT_END(count, 16, Add_roundkey_masked)
	SET_REPT_INC(count)
#endif
	ldrb r4,  [r5], #1				// r4  = permIndices[i]
	ldrb r11, [r2, r4]				// r11 = aes_state2[permIndices[i]]
	ldrb r6, [r12, r4]				// r6  = key_masksState[permIndices[i]]
	ldrb r6, [r0, r6]				// r6  = GTab_key[key_masksState[permGFSixteen[i]]] = (aes_rmult/key_rmult) * key_masksState[permIndices[i]] 
	eor  r11, r11, r6				// r11 = (aes_rmult/key_rmult) * key_masksState[permIndices[i]] ^ aes_state2[permIndices[i]]
	strb r11, [r2, r4]				// store r11 in @aes_state2[permIndices[i]]

	ldrb r11, [r3, r4]				// r11 = state[permIndices[i]]
	ldrb r6,  [r8, r4]				// load the permIndices(i)-th byte ki of key_state
	ldrb r6, [r0, r6]				// r6 = aes_rmult * round_key[permIndices[i]] ^ (aes_rmult/key_rmult) * key_masksState[permIndices[i]]
	eor  r11, r11, r6				// r11 = state[permIndices[i]] ^ aes_rmult * round_key[permIndices[i]]
	strb r11, [r3, r4]              // store r11 in @state[permIndices[i]]


	SET_REPT_CLOSE(Add_roundkey_masked)

	bx LR


/*******************************************************************************************************************
* FUNC: Xor_Word 
* DESCRIPTION: 	Xor between wordB|wordB|wordB|wordB and state where wordB is a 4 bytes table.
*
* INPUT: 		- state (@ in register r3)
*				- wordB (@ in register r7)
* OUTPUT: 		- state ^ [wordB|wordB|wordB|wordB]
* MODIFIED REGISTERS: r3, r5, r6.
* PRECONDITIONS: r1= @key_state[nb_round*16] ,r3 = @state, r7 = @wordB.
* POSTCONDITIONS: r1= @key_state[nb_round*16], r3 = @state
*******************************************************************************************************************/
Xor_Word MYLABEL
	ldr r5, [r7]				// r5 = wordB

	//it1
	ldr  r6, [r3]				// r6 = state[0:3]
	eor  r6, r5					// r6 = state[0:3] ^ WordB
	str  r6, [r3]				// save r6 in @state[0:3] and go to the next state word

	//it2
	ldr  r6, [r3,#0x4]			// r6 = state[4:7]
	eor  r6, r5					// r6 = state[4:7] ^ WordB
	str  r6, [r3, #0x4]			// save r6 in @state[4:7] and go to the next state word

	//it3
	ldr  r6, [r3, #0x8]			// r6 = state[8:11]
	eor  r6, r5					// r6 = state[8:11] ^ WordB
	str  r6, [r3, #0x8]			// save r6 in @state[8:11] and go to the next state word

	//it4
	ldr  r6, [r3, #0x0C]		// r6 = state[12:15]
	eor  r6, r5					// r6 = state[12:15] ^ WordB
	str  r6, [r3, #0xC]			// save r6 in @state[12:15]
	bx LR


/*******************************************************************************************************************
********************************************************************************************************************
*********************************           EXPORTED MAIN FUNCTIONS             *********************************
*********************************-----------------------------------------------************************************
*********************************-----------------------------------------------************************************/


/*******************************************************************************************************************
* FUNC: aes_loadKey
* DESCRIPTION: 	aes_loadKey masks the master key (the input key) with an affine representation. 
* 				Then the round keys are derivated from the masked master key and are themselves masked with an affine 
*				representation.   
* INPUT: 		-Context STRUCT_KEY_CONTEXT (address in register r0)
*				-key (address in register r1)
*				-T_rand: Table of 19 random bytes  (address in register r2)
* OUTPUT: 
*				-Master key and the 10 masked subkeys with an affine representation in key_maskedState 
*				-The mask state in key_masksState
* 				-Load the concatenation of a random byte 4 times for boolean input masking for SubByte operation in key_rin 
* 				-Load the concatenation of a random byte 4 times for boolean output masking for SubByte operation in key_rout 
* 				-Load a random byte as a multiplicative byte in key_rmult
*				-Computes the multiplicative table by key_rmult in  key_gtab
*				-Computes the the masked AES SBox in key_maskedSBox[16*16]
* 				-Computes the permutation key_permIndicesMC for shuffling
*				-Computes the rcon table multiplied by key_rmult in key_rcon_mult
*				-Return #SEC_OK if it succeeded garbage otherwise (last value of register r0).
* USED REGISTERS: r0-r8; r10-r12
* UNSED REGISTERS: r9
* PRECONDITIONS: The input T_rand has to contain 19 random bytes where T_rand[18] has to be different from 0. This byte is the 
*				 multipicative factor of the affine representation.
* POSTCONDITIONS: None
*******************************************************************************************************************/
aes_loadKey MYLABEL
	// Push the context pointer on the stack
	push.w {LR}
	push.w {r1-r12}
	push.w {r0} 												// Push the key context pointer on the stack

	// Load random[0:15] in key_masksState, random[16] in key_rin, random[17] in key_rout and random[18] in key_rmult.
	// ------------------------------------------------------
	bl Load_random_key

	// Compute the look-up table key_gtab s.t.
	// For all x in {0,.,255} key_gtab[x]= x * key_rmult
	// ------------------------------------------------------
	ldr_struct_field_addr(r3, 0, key_ctx_gtab)    				// r3 = @key_ctx_gtab
	ldr_struct_field_addr(r7, 0, key_ctx_rmult)					// r7 = @key_ctx_rmult
	ldrb r7, [r7]												// r7 = key_ctx_rmult
	bl Compute_GTab

	// Compute the key_rcon_mult table s.t. 
	// For all i in {0,.,39}, key_rcon_mult[i] = key_ctx_rmult * rcon[i] 
	// ------------------------------------------------------
	ldr_struct_field_addr(r12, 0, key_ctx_gtab)					// r12 = @key_ctx_gtab
	bl Map_in_G_rcon_key


	// Load the the random affine representation of the key in key_maskedState s.t.
	// For all i in {0,.,15}, key_maskedState[i] = key_maskedState[i]*key_ctx_rmult ^ key_masksState[i]
	// ------------------------------------------------------
	bl Load_masterKey

	// Computation of key_permIndicesMC permutation (4 elements perm)
	// ------------------------------------------------------
	ldr_struct_field_addr(r4, 0, key_ctx_masksState)
	ldr_struct_field_addr(r0, 0, key_ctx_permIndicesMC)
	mov 	  r6, #0x0											// r6 = Index of random byte in state2
	bl Compute_permIndices_Tables_MC

	// Computation of the masked Sbox in key_maskedSBox
	// ------------------------------------------------------
	ldr_addr r2, sbox											// r2 = @sbox
	ldr_struct_field_addr(r3, 0, key_ctx_maskedSBox)			// r3 = @key_ctx_maskedSBox
	ldr_struct_field_addr(r4, 0, key_ctx_gtab)					// r4 = @key_ctx_gtab
	ldr_struct_field_addr(r8, 0, key_ctx_rin)					// r8 = @key_ctx_rin
	ldr  r1, [r8]												// r1 = key_rin
	ldr_struct_field_addr(r6, 0, key_ctx_rout)					// r6 = @key_ctx_rout
	ldrb r8, [r6]												// r8 = key_ctx_rout
	bl Compute_Affine_sboxMasked

	// Computation of the round keys in key_maskedState
	// ------------------------------------------------------
	ldr_struct_field_addr(r1, 0, key_ctx_maskedState)			// r1 = @key_ctx_maskedState
	ldr_struct_field_addr(r7, 0, key_ctx_masksState)			// r7 = @key_ctx_masksState
	ldr_struct_field_addr(r5, 0, key_ctx_maskedSBox)			// r5 = @key_ctx_maskedSBox
	ldr_struct_field_addr(r2, 0, key_ctx_rcon_mult)				// r2 = @key_ctx_rcon_mult
	ldr_struct_field_addr(r3, 0, key_ctx_permIndicesMC)			// r3 = @permIndicesMC
#ifdef USE_FULL_UNROLLING
SET_REPT_START(count_, 1)
        SET_REPT_END(count_, 10, EntireKeyExpansion)
SET_REPT_INC(count_)
#else
	SET_REPT_START(count_, 1)
        SET_REPT_END(count_, 10, EntireKeyExpansion)
	SET_REPT_INC(count_)
#endif
	ldr_struct_field_addr(r11, 0, key_ctx_rin)					// r11 = @key_ctx_rin
	ldrb r11,[r11]												// r11 = key_ctx_rin
	ldr_struct_field_addr(r10, 0, key_ctx_rout)					// r10 = @key_ctx_rout
	ldrb r10,[r10]												// r10 = key_ctx_rout
	bl KeyExpansion_masked
	add r7, r7, #0x10											// r7 = key_maskedState
	SET_REPT_CLOSE(EntireKeyExpansion)

	pop {r0}
	mov r0, #SEC_OK												// Returns SEC_OK if it succeeds.
	pop {r1-r12}												// Restore Registers R1-R12 from the stack
	pop {PC}
	BX  LR

/*********************************************************************************************************************
* FUNC: aes_init_enc
* DESCRIPTION : aes_init_enc initilizes the aes_context structure for encryption. It computes the masked Sbox.	
* INPUT: 		- AES context structure 
*			 	- T_rand: Table of 19 random bytes.
* OUTPUT:
*				- load T_rand[0:15]  in @aes_ctx_masksState
*           	- load T_rand[16]    in @aes_ctx_rin
*           	- load T_rand[17]    in @aes_ctx_rout
*           	- load T_rand[18]    in @aes_ctx_rmult
* 				- preprocess the LUT for the function x in GF(256) -> rmult * x and store it at address @aes_ctx_GTab
*           	- preprocess the LUTs for two random permutations in {0,1,2,3} and store them at @aes_ctx_permIndicesMC and
*             	@aes_ctx_permIndicesBisMC respctively. T_rand[0] and T_rand[1] are respectively used to seed the two permutations.
*           	- preprocess the LUTs for two random permutations in {0,1,...,15} and store them at @aes_ctx_permIndices and
*             	@aes_ctx_permIndicesBis respctively. T_rand[0], T_rand[1], T_rand[2] and T_rand[4] are used to seed the generation.
* 				- preprocess the LUT for the function x -> aes_ctx_rmult * Sbox(x^aes_ctx_rin) ^ aes_ctx_rout and store it 
*             	at address @aes_ctx_sboxMasked.
*
* USED REGISTERS: r0-r8; r10-r12
* UNSED REGISTERS: r9
* PRECONDITIONS: The input T_rand has to contain 19 random bytes where T_rand[18] has to be different from 0. This byte is the 
*				 multipicative factor of the affine representation.
* POSTCONDITIONS: None.
************************************************************************************************************************/
aes_init_enc MYLABEL
	push.w {LR}
	push.w {r2-r12}  	
	push.w {r1}		// T_rand address pushed on the stack
	push.w {r0}		// aes_context pointer pushed on the stack
 
	// Load T_rand[0:15] in aes_masksState, T_rand[16] in aes_rin, T_rand[17] in aes_rout and T_rand[18] in aes_rmult.
	// ------------------------------------------------------
	bl Load_random

	// Compute the look-up table key_gtab s.t.
	// For all x in {0,.,255} key_gtab[x]= x * aes_rmult
	// ------------------------------------------------------
	ldr_struct_field_addr(r3, 0, aes_ctx_GTab)    			  	// r3 = @aes_ctx_GTab
	ldr_struct_field_addr(r7, 0, aes_ctx_rmult)   				// r7 = @aes_ctx_rmult
	ldrb r7, [r7]                                 				// r7 = aes_ctx_rmult
	bl Compute_GTab

	// Computation of aes_permIndicesMC permutation (4 elements perm)
	// ------------------------------------------------------
	ldr_struct_field_addr(r4, 0, aes_ctx_masksState)          	// r4 = @aes_ctx_masksState
	ldr_struct_field_addr(r0, 0, aes_ctx_permIndicesMC)      	// r0 = @aes_ctx_permIndicesMC
	mov r6, #0x0							                  	// r6 = Index of random byte in state2
	bl Compute_permIndices_Tables_MC

	// Computation of aes_permIndicesBisMC permutation (4 elements perm)
	// ------------------------------------------------------
	ldr_struct_field_addr(r0, 0, aes_ctx_permIndicesBisMC) 		// r0 = @aes_ctx_permIndicesBisMC
	mov r6, #0x1												// r6 = Index of random byte in state2
	bl Compute_permIndices_Tables_MC

	// Computation of the aes_permIndices and aes_permIndicesBis permutations
	// ------------------------------------------------------
	ldr_struct_field_addr(r2, 0, aes_ctx_permIndices) 			// r2  = @aes_ctx_permIndices
	ldr_struct_field_addr(r3, 0, aes_ctx_permIndicesBis) 		// r3  = @aes_ctx_permIndicesBis
	bl Compute_permIndices_Tables								

	// Computation of the masked Sbox stored in aes_ctx_sboxMasked
	// ------------------------------------------------------
	ldr_addr r2, sbox 											// r2 = @sbox
	ldr_struct_field_addr(r3, 0, aes_ctx_sboxMasked) 			// r3  = @aes_ctx_sboxMasked
	ldr_struct_field_addr(r4, 0, aes_ctx_GTab) 					// r4 = @aes_ctx_GTab
	mov r12, r4													// r12 = @aes_ctx_GTab
	ldr_struct_field_addr(r8, 0, aes_ctx_rin) 					// r8 = @aes_ctx_rin
	ldr r1, [r8]												// r1 = aes_ctx_rin
	ldr_struct_field_addr(r6, 0, aes_ctx_rout) 					// r6 = @aes_ctx_rout
	ldrb r8, [r6]												// r8 = aes_ctx_rout
	bl Compute_Affine_sboxMasked								

	pop {r0}
	pop {r1}
	pop {r2-r12}												// Restore Registers r0-r12 from the stack
        mov r0, #SEC_OK											// Returns 0 if it succeeds.
	pop {PC}
        BX  LR

/*********************************************************************************************************************
* FUNC: aes_init_dec
* DESCRIPTION : aes_init_dec initilizes the aes_context structure for decryption. It computes the masked InvSbox.
* INPUT: 		- AES context structure 
*				- T_rand: Table of 19 random bytes.
* OUTPUT:
*           	- load T_rand[16]    in @aes_ctx_rin
*           	- load T_rand[17]    in @aes_ctx_rout
*           	- load T_rand[18]    in @aes_ctx_rmult
*           	- load T_rand[0..15] in @aes_ctx_masksState
* 				- preprocess the LUT for the function x in GF(256) -> rmult * x and store it at address @aes_ctx_GTab
*           	- preprocess the LUTs for two random permutations in {0,1,2,3} and store them at @aes_ctx_permIndicesMC and
*             	@aes_ctx_permIndicesBisMC respctively. T_rand[0] and T_rand[1] are respectively used to seed the two permutations.
*           	- preprocess the LUTs for two random permutations in {0,1,...,15} and store them at @aes_ctx_permIndices and
*             	@aes_ctx_permIndicesBis respctively. T_rand[0], T_rand[1], T_rand[2] and T_rand[4] are used to seed the generation.
* 				- preprocess the LUT for the function x -> aes_ctx_rmult * INVSBOX(x^aes_ctx_rin) ^ aes_ctx_rout and store it 
*             	at address @aes_ctx_sboxMasked
*
* USED REGISTERS: r0-r8; r10-r12
* UNSED REGISTERS: r9
* PRECONDITIONS: The input T_rand has to contain 19 random bytes where T_rand[18] has to be different from 0. This byte is the 
*				 multipicative factor of the affine representation.
* POSTCONDITIONS:
************************************************************************************************************************/
aes_init_dec MYLABEL
	push.w {LR}
	push.w {r2-r12}  	
	push.w {r1}													//  T_rand address pusehd on the stack
	push.w {r0}													//  aes_context pointer pushed on the stack

	// Load T_rand[0:15] in aes_masksState, T_rand[16] in aes_rin, T_rand[17] in aes_rout and T_rand[18] in aes_rmult.
	// ------------------------------------------------------	
	bl Load_random

	// Compute the look-up table key_gtab s.t.
	// For all x in {0,.,255} key_gtab[x]= x * aes_rmult
	// ------------------------------------------------------
	ldr_struct_field_addr(r3, 0, aes_ctx_GTab)    			  	// r3 = @aes_ctx_GTab
	ldr_struct_field_addr(r7, 0, aes_ctx_rmult)   				// r7 = @aes_ctx_rmult
	ldrb r7, [r7]                                 				// r7 = aes_ctx_rmult
	bl Compute_GTab
	
	// Computation of aes_permIndicesMC permutation (4 elements perm)
	// ------------------------------------------------------
	ldr_struct_field_addr(r4, 0, aes_ctx_masksState)          	// r4 = @aes_ctx_masksState
	ldr_struct_field_addr(r0, 0, aes_ctx_permIndicesMC)      	// r0 = @aes_ctx_permIndicesMC
	mov r6, #0x0							                  	// r6 = Index of random byte in state2
	bl Compute_permIndices_Tables_MC
	
	// Computation of aes_permIndicesBisMC permutation (4 elements perm)
	// ------------------------------------------------------
	ldr_struct_field_addr(r0, 0, aes_ctx_permIndicesBisMC) 		// r0 = @aes_ctx_permIndicesBisMC
	mov r6, #0x1												// r6 = Index of random byte in state2
	bl Compute_permIndices_Tables_MC
	
	// Computation of the aes_permIndices and aes_permIndicesBis permutations
	// ------------------------------------------------------
	ldr_struct_field_addr(r2, 0, aes_ctx_permIndices) 			// r2  = @aes_ctx_permIndices
	ldr_struct_field_addr(r3, 0, aes_ctx_permIndicesBis) 		// r3  = @aes_ctx_permIndicesBis
	bl Compute_permIndices_Tables								

	// Computation of the masked Sbox stored in aes_ctx_sboxMasked
	// ------------------------------------------------------
	ldr_addr r2, Invsbox										// r2 = @Invsbox
	ldr_struct_field_addr(r3, 0, aes_ctx_sboxMasked) 			// r3  = @aes_ctx_sboxMasked
	ldr_struct_field_addr(r4, 0, aes_ctx_GTab) 					// r4 = @aes_ctx_GTab
	mov r12, r4													// r12 = @aes_ctx_GTab
	ldr_struct_field_addr(r8, 0, aes_ctx_rin) 					// r8 = @aes_ctx_rin
	ldr r1, [r8]												// r1 = aes_ctx_rin
	ldr_struct_field_addr(r6, 0, aes_ctx_rout) 					// r6 = @aes_ctx_rout
	ldrb r8, [r6]												// r8 = aes_ctx_rout
	bl Compute_Affine_sboxMasked								

	pop {r0}
	pop {r1}
	pop {r2-r12}												// Restores registers r0-r12 from the stack
        mov r0, #SEC_OK											// Returns SEC_OK if it succeeds.
	pop {PC}
        BX  LR


/************************************************************************************************************************
* FUNC: aes_enc
* DESCRIPTION: 	aes_enc performs an 128-AES encryption. Computation is performed with an affine masking of the state and 
*				the key (different affine masks) and operations performed in a random order.
* INPUT: 		- AES context 
* 				- Key context
*				- input: plaintext
*				- output: result
* OUTPUT: 		- result
*				- Returns 0 if it succeeds.
* USED REGISTERS: r0-r8; r10-r12
* UNSED REGISTERS: r9
* PRECONDITIONS: The input T_rand has to contain 19 random bytes where T_rand[18] has to be different from 0. This byte is the 
*				 multipicative factor of the affine representation.
* POSTCONDITIONS: None. 
************************************************************************************************************************/
aes_enc MYLABEL
	push.w {LR}
	push.w {r2}
	push.w {r4-r12}
	push.w {r3}											// push @result (address of the aes output)
	push.w {r1}											// Push the address of T-rand on the stack
	push.w {r0}											// Push the aes context pointer on the stack

	// NOTE: Key context will be in [sp+4] while AES context is on [sp]

	// Load message in state and aes_ctx_masksState in aes_ctx_state2
	// ------------------------------------------------------
	bl Load_data					

	// Computation of r7 = aes_rmult/key_rmult in GF(256)
	// ------------------------------------------------------
	ldr_struct_field_addr(r2, 4, key_ctx_rmult) 		// r2 = @key_ctx_rmult		   
	ldr_struct_field_addr(r3, 0, aes_ctx_rmult) 		// r3 = aes_ctx_rmult		   
	bl field_division
	
	// Compute the look-up table key_gtab s.t.
	// For all x in {0,.,255} key_gtab[x]= x * aes_rmult
	// ------------------------------------------------------	
	ldr_struct_field_addr(r3, 0, aes_ctx_GTab_key)		// r3 = @aes_ctx_GTab_key
	bl Compute_GTab

	// Computation of Gtab(state)
	// where aes_ctx_state = GTab[aes_ctx_state]
	// ------------------------------------------------------
	ldr_struct_field_addr(r3, 0, aes_ctx_state)			// r3 = @aes_ctx_state
	ldr_struct_field_addr(r12, 0, aes_ctx_GTab)         // r12 = @aes_ctx_GTab
	bl Map_in_G					                        

    // State masking
    // aes_ctx_state = aes_ctx_state ^ aes_ctx_state2
	// ------------------------------------------------------
	ldr_struct_field_addr(r3, 0, aes_ctx_state)			// r3 = @aes_ctx_state
	ldr_struct_field_addr(r7, 0, aes_ctx_state2)		// r7 = @aes_ctx_state2
	bl Xor_states					                    

	// Masked AddRoundKey where 
	// aes_ctx_state = aes_ctx_state ^ aes_rmult*round_key
	// ------------------------------------------------------
	ldr_struct_field_addr(r0, 0, aes_ctx_GTab_key)      // r0  = @aes_ctx_GTab_key
	ldr_struct_field_addr(r1, 4, key_ctx_maskedState)   // r1  = @key_ctx_maskedState
	mov r8, r1						                    // r8  = @key_ctx_maskedState
	ldr_struct_field_addr(r12, 4, key_ctx_masksState)   // r12 = @key_ctx_masksState
	ldr_struct_field_addr(r3, 0, aes_ctx_state)			// r3  = @aes_ctx_state
	ldr_struct_field_addr(r5, 0, aes_ctx_permIndices)   // r5  = @aes_ctx_permIndices
	bl Add_roundkey_masked	                             


//////////////////////////////////////BEGIN 9 FIRST ROUNDS /////////////////////////////////////////////////////////////////
#ifdef USE_FULL_UNROLLING
SET_REPT_START(count_, 1)
        SET_REPT_END(count_, 9, aes_enc)
SET_REPT_INC(count_)
#else
	SET_REPT_START(count_, 1)
        SET_REPT_END(count_, 9, aes_enc)
	SET_REPT_INC(count_)
#endif
	
	// Add SBox input mask to state where 
	// aes_ctx_state = aes_ctx_state ^ aes_ctx_rin
	// ------------------------------------------------------
	ldr_struct_field_addr(r7, 0, aes_ctx_rin)             	// r7    = @aes_ctx_rin
	ldr_struct_field_addr(r3, 0, aes_ctx_state)           	// r3    = @aes_ctx_state
	bl Xor_Word					                          	


	// Masked SubBytes operation where 
	// aes_ctx_state = aes_ctxt_rmult * Sbox(aes_ctx_state) ^ aes_ctx_state2
	// ------------------------------------------------------
	ldr_struct_field_addr(r5, 0, aes_ctx_permIndices)     	// r5    = @aes_ctx_permIndices
	ldr_struct_field_addr(r8, 0, aes_ctx_sboxMasked)      	// r8    = @aes_ctx_sboxMasked
	ldr_struct_field_addr(r10, 0, aes_ctx_state2)         	// r10   = @aes_ctx_state2
	ldr_struct_field_addr(r7, 0, aes_ctx_rout)            	// r7    = @aes_ctx_rout
	bl SubBytesWithMask 			                      	

	// Masked ShiftRows operation
	// state = ShiftRowsWithMasked(state) and state2 = ShiftRowsWithMasked(state2)
	// ------------------------------------------------------
	ldr_addr r4, t_shiftRows 								// r4 = @t_shiftRows
	ldr_struct_field_addr(r7, 0, aes_ctx_t_temp)          	// r7 = @aes_ctx_t_temp
	bl ShiftRowsWithMasked 			                      	
	
	// Masked MixColumns of aes_ctx_state2 	
	// ------------------------------------------------------
	ldr_struct_field_addr(r0, 0, aes_ctx_permIndicesMC)
	ldr_addr r6, xtime									  	// Store in r6 the @xtime (multiplication by 2)
	bl MixColumns										  	


	// MixColumns(state)
	ldr_struct_field_addr(r0, 0, aes_ctx_permIndicesBisMC)	// r0 = aes_ctx_permIndicesBisMC
	ldr_struct_field_addr(r10, 0, aes_ctx_state)			// r10 = aes_ctx_state
	bl MixColumns										  	

	// Masked AddRoundKey operation where 	
	// aes_ctx_state = aes_ctx_state ^ round_key
	// ------------------------------------------------------
	add r1, r1, #0x10 										// r1 = @key_state[(rnd+1)*16*8]
	add r12, r12, #0x10									  	// r12 = mask_key
	mov r8, r1											  	// r8 = @key_state[(rnd+1)*16*8]
	ldr_struct_field_addr(r3, 0, aes_ctx_state)			  	// r3 = @state
	ldr_struct_field_addr(r5, 0, aes_ctx_permIndices) 	  	// r5 = @permIndices
	ldr_struct_field_addr(r0, 0, aes_ctx_GTab_key)			// r0 = aes_ctx_GTab_key
	bl Add_roundkey_masked								  	

	SET_REPT_CLOSE(aes_enc)
//////////////////////////////////////END 9 FIRST ROUNDS /////////////////////////////////////////////////////////////////

//////////////////////////////////////BEGIN LAST ROUND   /////////////////////////////////////////////////////////////////
	
	// Xor operation where 
	// aes_ctx_state = aes_ctx_state ^ (aes_ctx_rin|...|aes_ctx_rin)
	// ------------------------------------------------------
	ldr_struct_field_addr(r7, 0, aes_ctx_rin)    		// r7    = @aes_ctx_rin
	ldr_struct_field_addr(r3, 0, aes_ctx_state)  		// r3    = @state
	bl Xor_Word					                        

	// Masked SubBytes operation where
	// state = aes_ctx_rmult * Sbox(aes_ctx_state) ^ aes_ctx_state2
	// ------------------------------------------------------
	ldr_struct_field_addr(r5, 0, aes_ctx_permIndices)   // r5    = @aes_ctx_permIndices
	ldr_struct_field_addr(r8, 0, aes_ctx_sboxMasked)    // r8    = @aes_ctx_sboxMasked
	ldr_struct_field_addr(r10, 0, aes_ctx_state2)       // r10   = @aes_ctx_state2
	ldr_struct_field_addr(r7, 0, aes_ctx_rout)          // r7    = @aes_ctx_rout
	bl SubBytesWithMask 			                    

	// Masked ShiftRows operation where 
	// aes_ctx_state = ShiftRowsWithMasked(aes_ctx_state) and aes_ctx_state2 = ShiftRowsWithMasked(aes_ctx_state2) 
	// ------------------------------------------------------
	ldr_addr r4, t_shiftRows 							// r4 = @t_shiftRows
	ldr_struct_field_addr(r7, 0, aes_ctx_t_temp)        // r7 = @aes_ctx_t_temp
	bl ShiftRowsWithMasked 			                   

	// Masked AddRoundKey operation	where
	// aes_ctx_state = aes_ctx_state ^ round_key
	// ------------------------------------------------------
	add r1, r1, #0x10  									// r1 = @key_state[(rnd+1)*16*8]
	add r12, r12, #0x10									// r12 = mask_key
	mov r8, r1											// r8 = @key_state[(rnd+1)*16*8]
	ldr_struct_field_addr(r5, 0, aes_ctx_permIndices) 	// r5 = @aes_ctx_permIndices
	ldr_struct_field_addr(r0, 0, aes_ctx_GTab_key)		// r0 = @aes_ctx_GTab_key 
	bl Add_roundkey_masked	


/////////////////////////////////END LAST ROUND/////////////////////////////////////////////////////////////////////////////////////////////////
		
	// Remove the Additive Masking	where
	// aes_ctx_state = aes_ctx_state ^ aes_ctx_state2
	// ------------------------------------------------------	
	ldr_struct_field_addr(r7, 0, aes_ctx_state2)		// r7 = aes_ctx_state2
	ldr_struct_field_addr(r3, 0, aes_ctx_state)			// r3 = aes_ctx_state
	bl Xor_states										


	// Remove the Multiplicative Masking where 
	// aes_ctx_state = 1/aes_ctx_rmult * aes_ctx_state	
	// ------------------------------------------------------
	ldr_addr r0, LogTable								// r0 = @LogTable
   	ldr_addr r1, ALogTable								// r1 = @ALogTable
	ldr_struct_field_addr(r4, 0, aes_ctx_rmult)			// r4 = @aes_ctx_rmult
	ldrb r2, [r4]										// r2 = aes_ctx_rmult = z**a
	ldrb r2, [r0,r2]									// r2 = LogTable[z**a] = a
	mov r12, #0xFF										// r12 = 255
	sub r2, r12, r2										// r2 = - a mod 255
	mov r7, r3											// r7 = @aes_ctx_state
	bl  Multiplicative_unmasking						

	// Outputs the result and restores the context	
	// ------------------------------------------------------
	ldr r7, [sp, #8]									// r7 = @result
	bl  State_copy										// copy of state in result
	pop {r0}
	mov r0, #SEC_OK										// It returns 0 if it succeeds
	pop {r1}
	pop {r3}										// r7 = @result
	pop {r4-r12}										// Restore Registers r4-r11 from the stack
	pop {r2}
	pop {PC}
	BX  LR


/************************************************************************************************************************
* FUNC: aes_dec
* DESCRIPTION: 	aes_dec performs an 128-AES decryption. Computation is performed with an affine masking of the state and 
*				the key (different affine masks) and operations performed in a random order.
* INPUT: 		- input: ciphertext
*				- Key
*			 	- T_rand: Table of 19 random bytes.
*				- result: plaintext
* OUTPUT: 		- result
*				- Returns 0 if it succeeds.
* USED REGISTERS: r0-r8; r10-r12
* UNSED REGISTERS: r9
* PRECONDITIONS: The input T_rand has to contain 19 random bytes where T_rand[18] has to be different from 0. This byte is the 
*				 multipicative factor of the affine representation.
* POSTCONDITIONS: None. 
************************************************************************************************************************/
aes_dec MYLABEL
	push.w {LR}
	push.w {r2}
	push.w {r4-r12}
	push.w {r3}
	push.w {r1}												// key_context pointer pushed on the stack
	push.w {r0}												// aes_context pointer pushed on the stack

	// NOTE: Key context will be in [sp+4] while AES context is on [sp]

	// Load message in state and aes_ctx_masksState in aes_ctx_state2
	// ------------------------------------------------------
	bl Load_data				

	// Computation of r7 = aes_ctx_rmult/key_ctx_rmult in GF(256)
	// ------------------------------------------------------
	ldr_struct_field_addr(r2, 4, key_ctx_rmult) 			// r2 = @key_ctx_rmult 			   
	ldr_struct_field_addr(r3, 0, aes_ctx_rmult) 			// r3 = @aes_ctx_rmult			   
	bl field_division


	// Compute the look-up table key_gtab s.t.
	// For all x in {0,.,255} key_gtab[x]= x * aes_rmult
	// ------------------------------------------------------	
	ldr_struct_field_addr(r3, 0, aes_ctx_GTab_key)			// r3 = @aes_ctx_GTab_key
	bl Compute_GTab

	// Computation of Gtab(aes_ctx_state) where
	// aes_ctx_state = GTab[aes_ctx_state]
	// ------------------------------------------------------
	ldr_struct_field_addr(r3, 0, aes_ctx_state)				// r3 = @aes_ctx_state
	ldr_struct_field_addr(r12, 0, aes_ctx_GTab)
	bl Map_in_G												

	// State masking where 
	// aes_ctx_state = aes_ctx_state ^ aes_ctx_state2
	// ------------------------------------------------------
	ldr_struct_field_addr(r3, 0, aes_ctx_state)				// r3 = @aes_ctx_state
	ldr_struct_field_addr(r7, 0, aes_ctx_state2)			// r7 = @aes_ctx_state2
	bl Xor_states											

	// AddRoundKey where
	// aes_ctx_state = aes_ctx_state ^ (aes_ctx_rmult * key_state)
	// ------------------------------------------------------
	ldr_struct_field_addr(r0, 0, aes_ctx_GTab_key)			// r0 = @aes_ctx_GTab_key
	ldr_struct_field_addr(r1, 4, key_ctx_maskedState)		// r1 = @key_ctx_maskedState
	add r1, r1, #0xa0										// r1 = @key_ctx_maskedState[16*10-1; 16*11-1] (last subkey @)
	ldr_struct_field_addr(r12, 4, key_ctx_masksState)		// r12 = @key_ctx_masksState
	add r12, r12, #0xa0										// r12 = @key_ctx_masksState[16*10-1; 16*11-1] (last submask_key @)
	ldr_struct_field_addr(r3, 0, aes_ctx_state)				// r3 = @aes_ctx_state
	ldr_struct_field_addr(r5, 0, aes_ctx_permIndices) 		// r5 = @aes_ctx_permIndices
	mov r8, r1												// r8 = @key_ctx_maskedState
	bl Add_roundkey_masked	


//////////////////////////////////////BEGIN 9 FIRST ROUNDS /////////////////////////////////////////////////////////////////
#ifdef USE_FULL_UNROLLING
SET_REPT_START(count_, 1)
        SET_REPT_END(count_, 9, aes_dec)
SET_REPT_INC(count_)
#else
	SET_REPT_START(count_, 1)
        SET_REPT_END(count_, 9, aes_dec)
	SET_REPT_INC(count_)
#endif

	// Masked ShiftRows operation where 
	// aes_ctx_state = ShiftRowsWithMasked(aes_ctx_state) and aes_ctx_state2 = ShiftRowsWithMasked(aes_ctx_state2)
	// ------------------------------------------------------
	ldr_struct_field_addr(r3, 0, aes_ctx_state)				// r3 = @aes_ctx_state
	ldr_addr r4, t_shiftRows_D								// r4 = @t_shiftRows_D
	ldr_struct_field_addr(r7, 0, aes_ctx_t_temp)			// r7 = @aes_ctx_t_temp
	ldr_struct_field_addr(r10, 0, aes_ctx_state2)			// r10 = @aes_ctx_state2
	bl ShiftRowsWithMasked 									

	// Xor operation where 
	// aes_ctx_state = aes_ctx_state ^ (aes_ctx_rin|...|aes_ctx_rin)
	// ------------------------------------------------------
	ldr_struct_field_addr(r7, 0, aes_ctx_rin)				// r7 = @aes_ctx_rin
	bl Xor_Word												

	// Masked SubBytes operation where 
	// aes_ctx_state = (key_ctx_rmult * InvSbox(aes_ctx_state)) ^ aes_ctx_state2
	// ------------------------------------------------------
	ldr_struct_field_addr(r10, 0, aes_ctx_state2)			// r10 = @aes_ctx_state2
	ldr_struct_field_addr(r8, 0, aes_ctx_sboxMasked)		// r8 = @aes_ctx_sboxMasked
	ldr_struct_field_addr(r5, 0, aes_ctx_permIndices)		// r5 = @aes_ctx_permIndices
	ldr_struct_field_addr(r7, 0, aes_ctx_rout)              // r7 = @aes_ctx_rout
	bl SubBytesWithMask 			                       

	// Masked AddRoundKey where
	// aes_ctx_state = aes_ctx_state ^ round_key
	// ------------------------------------------------------
	sub r12, r12, #0x10				                        // r12 = key_state
	sub r1, r1, #0x10 										// r1 = @key_state[(rnd+1)*16*8]
	mov r8, r1						                        // r8 = @key_state[(rnd+1)*16*8]
	ldr_struct_field_addr(r5, 0, aes_ctx_permIndices)       // r5 = @aes_ctx_permIndices
	ldr_struct_field_addr(r0, 0, aes_ctx_GTab_key)			// r0 = @aes_ctx_GTab_key
	bl Add_roundkey_masked		


	// ------------------ BEGIN InvMixColumns
	// ------------------------------------------------------
	/* DESCRIPTION: Performs InvMixcolumns on state and state2
	*        |0xe 0xb 0xd 0x9| |w0|   |0x2 0x3 0x1 0x1| |w0|   |0xC 0x8 0xC 0x8| |w0|
	*		 |0x9 0xe 0xb 0xd| |w1|   |0x1 0x2 0x3 0x1| |w1|   |0x8 0xC 0x8 0xC| |w1|
	*		 |0xd 0x9 0xe 0xb|*|w2|	= |0x1 0x1 0x2 0x3|*|w2| + |0xC 0x8 0xC 0x8|*|w2|
	*		 |0xb 0xd 0x9 0xe| |w3|	  |0x3 0x1 0x1 0x2| |w3|   |0x8 0xC 0x8 0xC| |w3|
	*   where the wi's are 4 bytes row vectors.     
	*								<=>
	* InvMixColumns(state)	= MixColumn(state)   + InvMixColumns_part2(state)
	*/
	// aes_ctx_t_temp <- InvMixColumns_part2(aes_ctx_state2)
	// ------------------------------------------------------
	ldr_struct_field_addr(r10, 0, aes_ctx_state2)			// r10 = @aes_ctx_state2
	ldr_struct_field_addr(r0, 0, aes_ctx_permIndicesBisMC)	// r0 = @aes_ctx_permIndicesBisMC
	bl InvMixColumns_part2									
							
	// aes_ctx_state2 <- MixColumns(aes_ctx_state2) 
	// ------------------------------------------------------
	ldr_struct_field_addr(r10, 0, aes_ctx_state2)			// r10 = @aes_ctx_state2
	ldr_struct_field_addr(r0, 0, aes_ctx_permIndicesBisMC)	// r0 = @aes_ctx_permIndicesBisMC
	ldr_addr r6, xtime										// r6 = @xtime (multiplication by 2)
	bl MixColumns											

	// Xor operation: aes_ctx_state2 <- aes_ctx_state2 ^ aes_ctx_t_temp
	// ------------------------------------------------------
	ldr_struct_field_addr(r7, 0, aes_ctx_t_temp)			// r7 = aes_ctx_t_temp
	ldr_struct_field_addr(r3, 0, aes_ctx_state2)			// r3 = aes_ctx_state2
	bl Xor_states


	// aes_ctx_t_temp <- InvMixColumns_part2(aes_ctx_state)
	// ------------------------------------------------------
	ldr_struct_field_addr(r10, 0, aes_ctx_state)			// r10 = @aes_ctx_state
	ldr_struct_field_addr(r0, 0, aes_ctx_permIndicesMC)		// r0 = @aes_ctx_permIndicesMC
	bl InvMixColumns_part2									

	// aes_ctx_state <- MixColumns(aes_ctx_state) 
	//------------------------------------------------------
	ldr_struct_field_addr(r10, 0, aes_ctx_state)			// r10 = @aes_ctx_state
	ldr_addr r6, xtime										// r6 = @xtime table (multiplication by 2)
	ldr_struct_field_addr(r0, 0, aes_ctx_permIndicesMC)		// r0 = @aes_ctx_permIndicesMC
	bl MixColumns											

	//  Xor operation: aes_ctx_state <- aes_ctx_state ^ aes_ctx_t_temp
	// ------------------------------------------------------
	ldr_struct_field_addr(r7, 0, aes_ctx_t_temp)			// r7 = @aes_ctx_t_temp
	ldr_struct_field_addr(r3, 0, aes_ctx_state)				// r3 = @aes_ctx_state
	bl Xor_states

	// ------------------ END InvMixColumns
	// ------------------------------------------------------

	SET_REPT_CLOSE(aes_dec)
	//////////////////////////////////////END 9 FIRST ROUNDS /////////////////////////////////////////////////////////////////

	//////////////////////////////////////BEGIN LAST ROUND   /////////////////////////////////////////////////////////////////
	// Masked ShiftRows operation where 
	// aes_ctx_state = ShiftRowsWithMasked(aes_ctx_state) and aes_ctx_state2 = ShiftRowsWithMasked(aes_ctx_state2)
	// ------------------------------------------------------
	ldr_struct_field_addr(r3, 0, aes_ctx_state)				// r3 = @aes_ctx_state
	ldr_addr r4, t_shiftRows_D								// r4 = @t_shiftRows_D
	ldr_struct_field_addr(r7, 0, aes_ctx_t_temp)			// r7 = @aes_ctx_t_temp
	ldr_struct_field_addr(r10, 0, aes_ctx_state2)			// r10 = aes_ctx_state2
	bl ShiftRowsWithMasked 									 

	// Xor operation where
	// aes_ctx_state = aes_ctx_state ^ (aes_ctx_rin|...|aes_ctx_rin) 
	// ------------------------------------------------------
	ldr_struct_field_addr(r7, 0, aes_ctx_rin)				// r7 = @aes_ctx_rin
	bl Xor_Word												

	// Masked SubBytes operation where 
	// aes_ctx_state = (aes_ctx_rmult * InvSbox(aes_ctx_state)) ^ aes_ctx_state2
	// ------------------------------------------------------
	ldr_struct_field_addr(r10, 0, aes_ctx_state2)			// r10 = @aes_ctx_state2
	ldr_struct_field_addr(r8, 0, aes_ctx_sboxMasked)		// r8 = @aes_ctx_sboxMasked
	ldr_struct_field_addr(r5, 0, aes_ctx_permIndices)		// r5 = @aes_ctx_permIndices
	ldr_struct_field_addr(r7, 0, aes_ctx_rout)       		// r7 = @aes_ctx_rout
	bl SubBytesWithMask 									
	
	// Add_roundkey where 
	// aes_ctx_state = aes_ctx_state ^ round_key
	// ------------------------------------------------------
	sub r12, r12, #0x10										// r12 = key_state
	sub r1, r1, #0x10 										// r1 = @key_state[(rnd+1)*16*8]
	mov r8, r1												// r8 = @key_state[(rnd+1)*16*8]
	ldr_struct_field_addr(r5, 0, aes_ctx_permIndices) 		// r5 = @aes_ctx_permIndices
	ldr_struct_field_addr(r0, 0, aes_ctx_GTab_key)			// r0 = @aes_ctx_GTab_key
	bl Add_roundkey_masked	




/////////////////////////////////END LAST ROUND////////////////////////////////////////////////////////////////////////////////

	// Remove the Additive Masking	where
	// aes_ctx_state = aes_ctx_state ^ aes_ctx_state2
	// ------------------------------------------------------	
	ldr_struct_field_addr(r7, 0, aes_ctx_state2) 			// r7 = @aes_ctx_state2	
	bl Xor_states											

	// Remove Multiplicative Masking
	// aes_ctx_state = 1/aes_ctx_rmult * aes_ctx_state	
	// ------------------------------------------------------	
	ldr_addr r0, LogTable									// r0 = @LogTable
   	ldr_addr r1, ALogTable									// r1 = @ALogTable
	ldr_struct_field_addr(r4, 0, aes_ctx_rmult)				// r4 = @aes_ctx_rmult
	ldrb r2, [r4]											// r2 = aes_ctx_rmult = z**a
	ldrb r2, [r0,r2]										// r2 = a loaded at @LogTable[table z**a]
	mov r12, #0xFF											// r12 = 255
	sub r2, r12, r2											// r2 = 255 - a
	mov r7, r3												// r7 = @aes_ctx_state
	bl  Multiplicative_unmasking							

	// Output the result and restores the context	
	// ------------------------------------------------------
	ldr r7, [sp, #8]									// r7 = @result
	bl  State_copy										// copy of state in result
	pop {r0}
	mov r0, #SEC_OK										// It returns 0 if it succeeds
	pop {r1}
	pop {r3}										// r7 = @result
	pop {r4-r12}										// Restore Registers r4-r11 from the stack
	pop {r2}
	pop {PC}
	BX  LR
	END
