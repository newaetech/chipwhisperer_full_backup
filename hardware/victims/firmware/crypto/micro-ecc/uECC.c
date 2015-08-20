/* Copyright 2014, Kenneth MacKay. Licensed under the BSD 2-clause license. */

#include "uECC.h"

#ifndef uECC_PLATFORM
    #if __AVR__
        #define uECC_PLATFORM uECC_avr
    #elif defined(__thumb2__) || defined(_M_ARMT) /* I think MSVC only supports Thumb-2 targets */
        #define uECC_PLATFORM uECC_arm_thumb2
    #elif defined(__thumb__)
        #define uECC_PLATFORM uECC_arm_thumb
    #elif defined(__arm__) || defined(_M_ARM)
        #define uECC_PLATFORM uECC_arm
    #elif defined(__i386__) || defined(_M_IX86) || defined(_X86_) || defined(__I86__)
        #define uECC_PLATFORM uECC_x86
    #elif defined(__amd64__) || defined(_M_X64)
        #define uECC_PLATFORM uECC_x86_64
    #else
        #define uECC_PLATFORM uECC_arch_other
    #endif
#endif

#ifndef uECC_WORD_SIZE
    #if uECC_PLATFORM == uECC_avr
        #define uECC_WORD_SIZE 1
    #elif (uECC_PLATFORM == uECC_x86_64)
        #define uECC_WORD_SIZE 8
    #else
        #define uECC_WORD_SIZE 4
    #endif
#endif

#if (uECC_CURVE == uECC_secp160r1) && (uECC_WORD_SIZE == 8)
    #undef uECC_WORD_SIZE
    #define uECC_WORD_SIZE 4
    #if (uECC_PLATFORM == uECC_x86_64)
        #undef uECC_PLATFORM
        #define uECC_PLATFORM uECC_x86
    #endif
#endif

#if (uECC_WORD_SIZE != 1) && (uECC_WORD_SIZE != 4) && (uECC_WORD_SIZE != 8)
    #error "Unsupported value for uECC_WORD_SIZE"
#endif

#if (uECC_ASM && (uECC_PLATFORM == uECC_avr) && (uECC_WORD_SIZE != 1))
    #pragma message ("uECC_WORD_SIZE must be 1 when using AVR asm")
    #undef uECC_WORD_SIZE
    #define uECC_WORD_SIZE 1
#endif

#if (uECC_ASM && (uECC_PLATFORM == uECC_arm || uECC_PLATFORM == uECC_arm_thumb) && (uECC_WORD_SIZE != 4))
    #pragma message ("uECC_WORD_SIZE must be 4 when using ARM asm")
    #undef uECC_WORD_SIZE
    #define uECC_WORD_SIZE 4
#endif

#if __STDC_VERSION__ >= 199901L
    #define RESTRICT restrict
#else
    #define RESTRICT
#endif

#if defined(__SIZEOF_INT128__) || ((__clang_major__ * 100 + __clang_minor__) >= 302)
    #define SUPPORTS_INT128 1
#else
    #define SUPPORTS_INT128 0
#endif

#define MAX_TRIES 64

#include <avr/io.h>
#include <util/delay.h>

#if (uECC_WORD_SIZE == 1)

typedef uint8_t uECC_word_t;
typedef uint16_t uECC_dword_t;
typedef uint8_t wordcount_t;
typedef int8_t swordcount_t;
typedef int16_t bitcount_t;
typedef int8_t cmpresult_t;

#define HIGH_BIT_SET 0x80
#define uECC_WORD_BITS 8
#define uECC_WORD_BITS_SHIFT 3
#define uECC_WORD_BITS_MASK 0x07

#define uECC_WORDS_1 20
#define uECC_WORDS_2 24
#define uECC_WORDS_3 32
#define uECC_WORDS_4 32

#define uECC_N_WORDS_1 21
#define uECC_N_WORDS_2 24
#define uECC_N_WORDS_3 32
#define uECC_N_WORDS_4 32

#define Curve_P_1 {0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, \
                   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
                   0xFF, 0xFF, 0xFF, 0xFF}
#define Curve_P_2 {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
                   0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
                   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
#define Curve_P_3 {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
                   0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, \
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                   0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF}
#define Curve_P_4 {0x2F, 0xFC, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, \
                   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
                   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
                   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}

#define Curve_B_1 {0x45, 0xFA, 0x65, 0xC5, 0xAD, 0xD4, 0xD4, 0x81, \
                   0x9F, 0xF8, 0xAC, 0x65, 0x8B, 0x7A, 0xBD, 0x54, \
                   0xFC, 0xBE, 0x97, 0x1C}
#define Curve_B_2 {0xB1, 0xB9, 0x46, 0xC1, 0xEC, 0xDE, 0xB8, 0xFE, \
                   0x49, 0x30, 0x24, 0x72, 0xAB, 0xE9, 0xA7, 0x0F, \
                   0xE7, 0x80, 0x9C, 0xE5, 0x19, 0x05, 0x21, 0x64}
#define Curve_B_3 {0x4B, 0x60, 0xD2, 0x27, 0x3E, 0x3C, 0xCE, 0x3B, \
                   0xF6, 0xB0, 0x53, 0xCC, 0xB0, 0x06, 0x1D, 0x65, \
                   0xBC, 0x86, 0x98, 0x76, 0x55, 0xBD, 0xEB, 0xB3, \
                   0xE7, 0x93, 0x3A, 0xAA, 0xD8, 0x35, 0xC6, 0x5A}
#define Curve_B_4 {0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define Curve_G_1 { \
    {0x82, 0xFC, 0xCB, 0x13, 0xB9, 0x8B, 0xC3, 0x68, \
        0x89, 0x69, 0x64, 0x46, 0x28, 0x73, 0xF5, 0x8E, \
        0x68, 0xB5, 0x96, 0x4A}, \
    {0x32, 0xFB, 0xC5, 0x7A, 0x37, 0x51, 0x23, 0x04, \
        0x12, 0xC9, 0xDC, 0x59, 0x7D, 0x94, 0x68, 0x31, \
        0x55, 0x28, 0xA6, 0x23}}

#define Curve_G_2 { \
    {0x12, 0x10, 0xFF, 0x82, 0xFD, 0x0A, 0xFF, 0xF4, \
        0x00, 0x88, 0xA1, 0x43, 0xEB, 0x20, 0xBF, 0x7C, \
        0xF6, 0x90, 0x30, 0xB0, 0x0E, 0xA8, 0x8D, 0x18}, \
    {0x11, 0x48, 0x79, 0x1E, 0xA1, 0x77, 0xF9, 0x73, \
        0xD5, 0xCD, 0x24, 0x6B, 0xED, 0x11, 0x10, 0x63, \
        0x78, 0xDA, 0xC8, 0xFF, 0x95, 0x2B, 0x19, 0x07}}

#define Curve_G_3 { \
    {0x96, 0xC2, 0x98, 0xD8, 0x45, 0x39, 0xA1, 0xF4, \
        0xA0, 0x33, 0xEB, 0x2D, 0x81, 0x7D, 0x03, 0x77, \
        0xF2, 0x40, 0xA4, 0x63, 0xE5, 0xE6, 0xBC, 0xF8, \
        0x47, 0x42, 0x2C, 0xE1, 0xF2, 0xD1, 0x17, 0x6B}, \
    {0xF5, 0x51, 0xBF, 0x37, 0x68, 0x40, 0xB6, 0xCB, \
        0xCE, 0x5E, 0x31, 0x6B, 0x57, 0x33, 0xCE, 0x2B, \
        0x16, 0x9E, 0x0F, 0x7C, 0x4A, 0xEB, 0xE7, 0x8E, \
        0x9B, 0x7F, 0x1A, 0xFE, 0xE2, 0x42, 0xE3, 0x4F}}

#define Curve_G_4 { \
    {0x98, 0x17, 0xF8, 0x16, 0x5B, 0x81, 0xF2, 0x59, \
        0xD9, 0x28, 0xCE, 0x2D, 0xDB, 0xFC, 0x9B, 0x02, \
        0x07, 0x0B, 0x87, 0xCE, 0x95, 0x62, 0xA0, 0x55, \
        0xAC, 0xBB, 0xDC, 0xF9, 0x7E, 0x66, 0xBE, 0x79}, \
    {0xB8, 0xD4, 0x10, 0xFB, 0x8F, 0xD0, 0x47, 0x9C, \
        0x19, 0x54, 0x85, 0xA6, 0x48, 0xB4, 0x17, 0xFD, \
        0xA8, 0x08, 0x11, 0x0E, 0xFC, 0xFB, 0xA4, 0x5D, \
        0x65, 0xC4, 0xA3, 0x26, 0x77, 0xDA, 0x3A, 0x48}}

#define Curve_N_1 {0x57, 0x22, 0x75, 0xCA, 0xD3, 0xAE, 0x27, 0xF9, \
                   0xC8, 0xF4, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, \
                   0x00, 0x00, 0x00, 0x00, 0x01}
#define Curve_N_2 {0x31, 0x28, 0xD2, 0xB4, 0xB1, 0xC9, 0x6B, 0x14, \
                   0x36, 0xF8, 0xDE, 0x99, 0xFF, 0xFF, 0xFF, 0xFF, \
                   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
#define Curve_N_3 {0x51, 0x25, 0x63, 0xFC, 0xC2, 0xCA, 0xB9, 0xF3, \
                   0x84, 0x9E, 0x17, 0xA7, 0xAD, 0xFA, 0xE6, 0xBC, \
                   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
                   0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF}
#define Curve_N_4 {0x41, 0x41, 0x36, 0xD0, 0x8C, 0x5E, 0xD2, 0xBF, \
                   0x3B, 0xA0, 0x48, 0xAF, 0xE6, 0xDC, 0xAE, 0xBA, \
                   0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
                   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}

#elif (uECC_WORD_SIZE == 4)

typedef uint32_t uECC_word_t;
typedef uint64_t uECC_dword_t;
typedef unsigned wordcount_t;
typedef int swordcount_t;
typedef int bitcount_t;
typedef int cmpresult_t;

#define HIGH_BIT_SET 0x80000000
#define uECC_WORD_BITS 32
#define uECC_WORD_BITS_SHIFT 5
#define uECC_WORD_BITS_MASK 0x01F

#define uECC_WORDS_1 5
#define uECC_WORDS_2 6
#define uECC_WORDS_3 8
#define uECC_WORDS_4 8

#define uECC_N_WORDS_1 6
#define uECC_N_WORDS_2 6
#define uECC_N_WORDS_3 8
#define uECC_N_WORDS_4 8

#define Curve_P_1 {0x7FFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
#define Curve_P_2 {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
#define Curve_P_3 {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000001, 0xFFFFFFFF}
#define Curve_P_4 {0xFFFFFC2F, 0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}

#define Curve_B_1 {0xC565FA45, 0x81D4D4AD, 0x65ACF89F, 0x54BD7A8B, 0x1C97BEFC}
#define Curve_B_2 {0xC146B9B1, 0xFEB8DEEC, 0x72243049, 0x0FA7E9AB, 0xE59C80E7, 0x64210519}
#define Curve_B_3 {0x27D2604B, 0x3BCE3C3E, 0xCC53B0F6, 0x651D06B0, 0x769886BC, 0xB3EBBD55, 0xAA3A93E7, 0x5AC635D8}
#define Curve_B_4 {0x00000007, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000}

#define Curve_G_1 { \
    {0x13CBFC82, 0x68C38BB9, 0x46646989, 0x8EF57328, 0x4A96B568}, \
    {0x7AC5FB32, 0x04235137, 0x59DCC912, 0x3168947D, 0x23A62855}}

#define Curve_G_2 { \
    {0x82FF1012, 0xF4FF0AFD, 0x43A18800, 0x7CBF20EB, 0xB03090F6, 0x188DA80E}, \
    {0x1E794811, 0x73F977A1, 0x6B24CDD5, 0x631011ED, 0xFFC8DA78, 0x07192B95}}
    
#define Curve_G_3 { \
    {0xD898C296, 0xF4A13945, 0x2DEB33A0, 0x77037D81, 0x63A440F2, 0xF8BCE6E5, 0xE12C4247, 0x6B17D1F2}, \
    {0x37BF51F5, 0xCBB64068, 0x6B315ECE, 0x2BCE3357, 0x7C0F9E16, 0x8EE7EB4A, 0xFE1A7F9B, 0x4FE342E2}}

#define Curve_G_4 { \
    {0x16F81798, 0x59F2815B, 0x2DCE28D9, 0x029BFCDB, 0xCE870B07, 0x55A06295, 0xF9DCBBAC, 0x79BE667E}, \
    {0xFB10D4B8, 0x9C47D08F, 0xA6855419, 0xFD17B448, 0x0E1108A8, 0x5DA4FBFC, 0x26A3C465, 0x483ADA77}}

#define Curve_N_1 {0xCA752257, 0xF927AED3, 0x0001F4C8, 0x00000000, 0x00000000, 0x00000001}
#define Curve_N_2 {0xB4D22831, 0x146BC9B1, 0x99DEF836, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
#define Curve_N_3 {0xFC632551, 0xF3B9CAC2, 0xA7179E84, 0xBCE6FAAD, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF}
#define Curve_N_4 {0xD0364141, 0xBFD25E8C, 0xAF48A03B, 0xBAAEDCE6, 0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}

#elif (uECC_WORD_SIZE == 8)

typedef uint64_t uECC_word_t;
#if SUPPORTS_INT128
typedef unsigned __int128 uECC_dword_t;
#endif
typedef unsigned wordcount_t;
typedef int swordcount_t;
typedef int bitcount_t;
typedef int cmpresult_t;

#define HIGH_BIT_SET 0x8000000000000000ull
#define uECC_WORD_BITS 64
#define uECC_WORD_BITS_SHIFT 6
#define uECC_WORD_BITS_MASK 0x03F

#define uECC_WORDS_1 3
#define uECC_WORDS_2 3
#define uECC_WORDS_3 4
#define uECC_WORDS_4 4

#define uECC_N_WORDS_1 3
#define uECC_N_WORDS_2 3
#define uECC_N_WORDS_3 4
#define uECC_N_WORDS_4 4

#define Curve_P_1 {0xFFFFFFFF7FFFFFFFull, 0xFFFFFFFFFFFFFFFFull, 0x00000000FFFFFFFFull}
#define Curve_P_2 {0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFFFFFFFEull, 0xFFFFFFFFFFFFFFFFull}
#define Curve_P_3 {0xFFFFFFFFFFFFFFFFull, 0x00000000FFFFFFFFull, 0x0000000000000000ull, 0xFFFFFFFF00000001ull}
#define Curve_P_4 {0xFFFFFFFEFFFFFC2Full, 0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFFFFFFFFull}

#define Curve_B_1 {0x81D4D4ADC565FA45ull, 0x54BD7A8B65ACF89Full, 0x000000001C97BEFCull}
#define Curve_B_2 {0xFEB8DEECC146B9B1ull, 0x0FA7E9AB72243049ull, 0x64210519E59C80E7ull}
#define Curve_B_3 {0x3BCE3C3E27D2604Bull, 0x651D06B0CC53B0F6ull, 0xB3EBBD55769886BCull, 0x5AC635D8AA3A93E7ull}
#define Curve_B_4 {0x0000000000000007ull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull}

#define Curve_G_1 { \
    {0x68C38BB913CBFC82ull, 0x8EF5732846646989ull, 0x000000004A96B568ull}, \
    {0x042351377AC5FB32ull, 0x3168947D59DCC912ull, 0x0000000023A62855ull}}

#define Curve_G_2 { \
    {0xF4FF0AFD82FF1012ull, 0x7CBF20EB43A18800ull, 0x188DA80EB03090F6ull}, \
    {0x73F977A11E794811ull, 0x631011ED6B24CDD5ull, 0x07192B95FFC8DA78ull}}
    
#define Curve_G_3 { \
    {0xF4A13945D898C296ull, 0x77037D812DEB33A0ull, 0xF8BCE6E563A440F2ull, 0x6B17D1F2E12C4247ull}, \
    {0xCBB6406837BF51F5ull, 0x2BCE33576B315ECEull, 0x8EE7EB4A7C0F9E16ull, 0x4FE342E2FE1A7F9Bull}}

#define Curve_G_4 { \
    {0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC}, \
    {0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465}}

#define Curve_N_1 {0xF927AED3CA752257ull, 0x000000000001F4C8ull, 0x0000000100000000ull}
#define Curve_N_2 {0x146BC9B1B4D22831ull, 0xFFFFFFFF99DEF836ull, 0xFFFFFFFFFFFFFFFFull}
#define Curve_N_3 {0xF3B9CAC2FC632551ull, 0xBCE6FAADA7179E84ull, 0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFF00000000ull}
#define Curve_N_4 {0xBFD25E8CD0364141, 0xBAAEDCE6AF48A03B, 0xFFFFFFFFFFFFFFFE, 0xFFFFFFFFFFFFFFFF}

#endif /* (uECC_WORD_SIZE == 8) */

#define uECC_WORDS uECC_CONCAT(uECC_WORDS_, uECC_CURVE)
#define uECC_N_WORDS uECC_CONCAT(uECC_N_WORDS_, uECC_CURVE)

typedef struct EccPoint
{
    uECC_word_t x[uECC_WORDS];
    uECC_word_t y[uECC_WORDS];
} EccPoint;

static uECC_word_t curve_p[uECC_WORDS] = uECC_CONCAT(Curve_P_, uECC_CURVE);
static uECC_word_t curve_b[uECC_WORDS] = uECC_CONCAT(Curve_B_, uECC_CURVE);
static EccPoint curve_G = uECC_CONCAT(Curve_G_, uECC_CURVE);
static uECC_word_t curve_n[uECC_N_WORDS] = uECC_CONCAT(Curve_N_, uECC_CURVE);

static void vli_clear(uECC_word_t *p_vli);
static uECC_word_t vli_isZero(const uECC_word_t *p_vli);
static uECC_word_t vli_testBit(const uECC_word_t *p_vli, bitcount_t p_bit);
static bitcount_t vli_numBits(const uECC_word_t *p_vli, wordcount_t p_maxWords);
static void vli_set(uECC_word_t *p_dest, const uECC_word_t *p_src);
static cmpresult_t vli_cmp(uECC_word_t *p_left, uECC_word_t *p_right);
static cmpresult_t vli_equal(uECC_word_t *p_left, uECC_word_t *p_right);
static void vli_rshift1(uECC_word_t *p_vli);
static uECC_word_t vli_add(uECC_word_t *p_result, uECC_word_t *p_left, uECC_word_t *p_right);
static uECC_word_t vli_sub(uECC_word_t *p_result, uECC_word_t *p_left, uECC_word_t *p_right);
static void vli_mult(uECC_word_t *p_result, uECC_word_t *p_left, uECC_word_t *p_right);
static void vli_modAdd(uECC_word_t *p_result, uECC_word_t *p_left, uECC_word_t *p_right, uECC_word_t *p_mod);
static void vli_modSub(uECC_word_t *p_result, uECC_word_t *p_left, uECC_word_t *p_right, uECC_word_t *p_mod);
static void vli_mmod_fast(uECC_word_t *RESTRICT p_result, uECC_word_t *RESTRICT p_product);
static void vli_modMult_fast(uECC_word_t *p_result, uECC_word_t *p_left, uECC_word_t *p_right);
static void vli_modInv(uECC_word_t *p_result, uECC_word_t *p_input, uECC_word_t *p_mod);
#if uECC_SQUARE_FUNC
static void vli_square(uECC_word_t *p_result, uECC_word_t *p_left);
static void vli_modSquare_fast(uECC_word_t *p_result, uECC_word_t *p_left);
#endif

#if (defined(_WIN32) || defined(_WIN64))
/* Windows */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincrypt.h>

static int default_RNG(uint8_t *p_dest, unsigned p_size)
{
    HCRYPTPROV l_prov;
    if(!CryptAcquireContext(&l_prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
    {
        return 0;
    }

    CryptGenRandom(l_prov, p_size, (BYTE *)p_dest);
    CryptReleaseContext(l_prov, 0);
    
    return 1;
}

#elif defined(unix) || defined(__linux__) || defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__)) || defined(uECC_POSIX)

/* Some POSIX-like system with /dev/urandom or /dev/random. */
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef O_CLOEXEC
    #define O_CLOEXEC 0
#endif

static int default_RNG(uint8_t *p_dest, unsigned p_size)
{
    int l_fd = open("/dev/urandom", O_RDONLY | O_CLOEXEC);
    if(l_fd == -1)
    {
        l_fd = open("/dev/random", O_RDONLY | O_CLOEXEC);
        if(l_fd == -1)
        {
            return 0;
        }
    }
    
    char *l_ptr = (char *)p_dest;
    size_t l_left = p_size;
    while(l_left > 0)
    {
        int l_read = read(l_fd, l_ptr, l_left);
        if(l_read <= 0)
        { // read failed
            close(l_fd);
            return 0;
        }
        l_left -= l_read;
        l_ptr += l_read;
    }
    
    close(l_fd);
    return 1;
}

#else /* Some other platform */

static int default_RNG(uint8_t *p_dest, unsigned p_size)
{
    return 0;
}

#endif

static uECC_RNG_Function g_rng = &default_RNG;

void uECC_set_rng(uECC_RNG_Function p_rng)
{
    g_rng = p_rng;
}

#ifdef __GNUC__ /* Only support GCC inline asm for now */
    #if (uECC_ASM && (uECC_PLATFORM == uECC_avr))
        #include "asm_avr.inc"
    #endif

    #if (uECC_ASM && (uECC_PLATFORM == uECC_arm || uECC_PLATFORM == uECC_arm_thumb || uECC_PLATFORM == uECC_arm_thumb2))
        #include "asm_arm.inc"
    #endif
#endif

#if !asm_clear
static void vli_clear(uECC_word_t *p_vli)
{
    wordcount_t i;
    for(i = 0; i < uECC_WORDS; ++i)
    {
        p_vli[i] = 0;
    }
}
#endif

/* Returns 1 if p_vli == 0, 0 otherwise. */
#if !asm_isZero
static uECC_word_t vli_isZero(const uECC_word_t *p_vli)
{
    wordcount_t i;
    for(i = 0; i < uECC_WORDS; ++i)
    {
        if(p_vli[i])
        {
            return 0;
        }
    }
    return 1;
}
#endif

/* Returns nonzero if bit p_bit of p_vli is set. */
#if !asm_testBit
static uECC_word_t vli_testBit(const uECC_word_t *p_vli, bitcount_t p_bit)
{
    return (p_vli[p_bit >> uECC_WORD_BITS_SHIFT] & ((uECC_word_t)1 << (p_bit & uECC_WORD_BITS_MASK)));
}
#endif

/* Counts the number of words in p_vli. */
#if !asm_numBits
static wordcount_t vli_numDigits(const uECC_word_t *p_vli, wordcount_t p_maxWords)
{
    swordcount_t i;
    /* Search from the end until we find a non-zero digit.
       We do it in reverse because we expect that most digits will be nonzero. */
    for(i = p_maxWords-1; i >= 0 && p_vli[i] == 0; --i)
    {
    }

    return (i + 1);
}

/* Counts the number of bits required to represent p_vli. */
static bitcount_t vli_numBits(const uECC_word_t *p_vli, wordcount_t p_maxWords)
{
    uECC_word_t i;
    uECC_word_t l_digit;
    
    wordcount_t l_numDigits = vli_numDigits(p_vli, p_maxWords);
    if(l_numDigits == 0)
    {
        return 0;
    }

    l_digit = p_vli[l_numDigits - 1];
    for(i = 0; l_digit; ++i)
    {
        l_digit >>= 1;
    }
    
    return (((bitcount_t)(l_numDigits - 1) << uECC_WORD_BITS_SHIFT) + i);
}
#endif /* !asm_numBits */

/* Sets p_dest = p_src. */
#if !asm_set
static void vli_set(uECC_word_t *p_dest, const uECC_word_t *p_src)
{
    wordcount_t i;
    for(i=0; i<uECC_WORDS; ++i)
    {
        p_dest[i] = p_src[i];
    }
}
#endif

/* Returns sign of p_left - p_right. */
#if !asm_cmp
static cmpresult_t vli_cmp(uECC_word_t *p_left, uECC_word_t *p_right)
{
    swordcount_t i;
    for(i = uECC_WORDS-1; i >= 0; --i)
    {
        if(p_left[i] > p_right[i])
        {
            return 1;
        }
        else if(p_left[i] < p_right[i])
        {
            return -1;
        }
    }
    return 0;
}
#endif

static cmpresult_t vli_equal(uECC_word_t *p_left, uECC_word_t *p_right)
{
    uECC_word_t l_result = 0;
    
    swordcount_t i;
    for(i = uECC_WORDS-1; i >= 0; --i)
    {
        l_result |= (p_left[i] ^ p_right[i]);
    }
    
    return (l_result == 0);
}

/* Computes p_vli = p_vli >> 1. */
#if !asm_rshift1
static void vli_rshift1(uECC_word_t *p_vli)
{
    uECC_word_t *l_end = p_vli;
    uECC_word_t l_carry = 0;
    
    p_vli += uECC_WORDS;
    while(p_vli-- > l_end)
    {
        uECC_word_t l_temp = *p_vli;
        *p_vli = (l_temp >> 1) | l_carry;
        l_carry = l_temp << (uECC_WORD_BITS - 1);
    }
}
#endif

/* Computes p_result = p_left + p_right, returning carry. Can modify in place. */
#if !asm_add
static uECC_word_t vli_add(uECC_word_t *p_result, uECC_word_t *p_left, uECC_word_t *p_right)
{
    uECC_word_t l_carry = 0;
    wordcount_t i;
    for(i = 0; i < uECC_WORDS; ++i)
    {
        uECC_word_t l_sum = p_left[i] + p_right[i] + l_carry;
        if(l_sum != p_left[i])
        {
            l_carry = (l_sum < p_left[i]);
        }
        p_result[i] = l_sum;
    }
    return l_carry;
}
#endif

/* Computes p_result = p_left - p_right, returning borrow. Can modify in place. */
#if !asm_sub
static uECC_word_t vli_sub(uECC_word_t *p_result, uECC_word_t *p_left, uECC_word_t *p_right)
{
    uECC_word_t l_borrow = 0;
    wordcount_t i;
    for(i = 0; i < uECC_WORDS; ++i)
    {
        uECC_word_t l_diff = p_left[i] - p_right[i] - l_borrow;
        if(l_diff != p_left[i])
        {
            l_borrow = (l_diff > p_left[i]);
        }
        p_result[i] = l_diff;
    }
    return l_borrow;
}
#endif

#if (!asm_mult || !asm_square || uECC_CURVE == uECC_secp256k1)
static void muladd(uECC_word_t a, uECC_word_t b, uECC_word_t *r0, uECC_word_t *r1, uECC_word_t *r2)
{
#if uECC_WORD_SIZE == 8 && !SUPPORTS_INT128
    uint64_t a0 = a & 0xffffffffull;
    uint64_t a1 = a >> 32;
    uint64_t b0 = b & 0xffffffffull;
    uint64_t b1 = b >> 32;
    
    uint64_t i0 = a0 * b0;
    uint64_t i1 = a0 * b1;
    uint64_t i2 = a1 * b0;
    uint64_t i3 = a1 * b1;
    
    uint64_t p0, p1;
    
    i2 += (i0 >> 32);
    i2 += i1;
    if(i2 < i1)
    { // overflow
        i3 += 0x100000000ull;
    }
    
    p0 = (i0 & 0xffffffffull) | (i2 << 32);
    p1 = i3 + (i2 >> 32);
    
    *r0 += p0;
    *r1 += (p1 + (*r0 < p0));
    *r2 += ((*r1 < p1) || (*r1 == p1 && *r0 < p0));
#else
    uECC_dword_t p = (uECC_dword_t)a * b;
    uECC_dword_t r01 = ((uECC_dword_t)(*r1) << uECC_WORD_BITS) | *r0;
    r01 += p;
    *r2 += (r01 < p);
    *r1 = r01 >> uECC_WORD_BITS;
    *r0 = (uECC_word_t)r01;
#endif
}
#define muladd_exists 1
#endif

#if !asm_mult
static void vli_mult(uECC_word_t *p_result, uECC_word_t *p_left, uECC_word_t *p_right)
{
    uECC_word_t r0 = 0;
    uECC_word_t r1 = 0;
    uECC_word_t r2 = 0;
    
    wordcount_t i, k;
    
    /* Compute each digit of p_result in sequence, maintaining the carries. */
    for(k = 0; k < uECC_WORDS; ++k)
    {
        for(i = 0; i <= k; ++i)
        {
            muladd(p_left[i], p_right[k-i], &r0, &r1, &r2);
        }
        p_result[k] = r0;
        r0 = r1;
        r1 = r2;
        r2 = 0;
    }
    for(k = uECC_WORDS; k < uECC_WORDS*2 - 1; ++k)
    {
        for(i = (k + 1) - uECC_WORDS; i<uECC_WORDS; ++i)
        {
            muladd(p_left[i], p_right[k-i], &r0, &r1, &r2);
        }
        p_result[k] = r0;
        r0 = r1;
        r1 = r2;
        r2 = 0;
    }
    
    p_result[uECC_WORDS*2 - 1] = r0;
}
#endif

#if uECC_SQUARE_FUNC

#if !asm_square
static void mul2add(uECC_word_t a, uECC_word_t b, uECC_word_t *r0, uECC_word_t *r1, uECC_word_t *r2)
{
#if uECC_WORD_SIZE == 8 && !SUPPORTS_INT128
    uint64_t a0 = a & 0xffffffffull;
    uint64_t a1 = a >> 32;
    uint64_t b0 = b & 0xffffffffull;
    uint64_t b1 = b >> 32;
    
    uint64_t i0 = a0 * b0;
    uint64_t i1 = a0 * b1;
    uint64_t i2 = a1 * b0;
    uint64_t i3 = a1 * b1;
    
    uint64_t p0, p1;
    
    i2 += (i0 >> 32);
    i2 += i1;
    if(i2 < i1)
    { // overflow
        i3 += 0x100000000ull;
    }
    
    p0 = (i0 & 0xffffffffull) | (i2 << 32);
    p1 = i3 + (i2 >> 32);
    
    *r2 += (p1 >> 63);
    p1 = (p1 << 1) | (p0 >> 63);
    p0 <<= 1;
    
    *r0 += p0;
    *r1 += (p1 + (*r0 < p0));
    *r2 += ((*r1 < p1) || (*r1 == p1 && *r0 < p0));
#else
    uECC_dword_t p = (uECC_dword_t)a * b;
    uECC_dword_t r01 = ((uECC_dword_t)(*r1) << uECC_WORD_BITS) | *r0;
    *r2 += (p >> (uECC_WORD_BITS * 2 - 1));
    p *= 2;
    r01 += p;
    *r2 += (r01 < p);
    *r1 = r01 >> uECC_WORD_BITS;
    *r0 = (uECC_word_t)r01;
#endif
}

static void vli_square(uECC_word_t *p_result, uECC_word_t *p_left)
{
    uECC_word_t r0 = 0;
    uECC_word_t r1 = 0;
    uECC_word_t r2 = 0;
    
    wordcount_t i, k;
    
    for(k = 0; k < uECC_WORDS*2 - 1; ++k)
    {
        uECC_word_t l_min = (k < uECC_WORDS ? 0 : (k + 1) - uECC_WORDS);
        for(i = l_min; i<=k && i<=k-i; ++i)
        {
            if(i < k-i)
            {
                mul2add(p_left[i], p_left[k-i], &r0, &r1, &r2);
            }
            else
            {
                muladd(p_left[i], p_left[k-i], &r0, &r1, &r2);
            }
        }
        p_result[k] = r0;
        r0 = r1;
        r1 = r2;
        r2 = 0;
    }
    
    p_result[uECC_WORDS*2 - 1] = r0;
}
#endif

#else /* uECC_SQUARE_FUNC */

#define vli_square(result, left, size) vli_mult((result), (left), (left), (size))
    
#endif /* uECC_SQUARE_FUNC */


/* Computes p_result = (p_left + p_right) % p_mod.
   Assumes that p_left < p_mod and p_right < p_mod, p_result != p_mod. */
#if !asm_modAdd
static void vli_modAdd(uECC_word_t *p_result, uECC_word_t *p_left, uECC_word_t *p_right, uECC_word_t *p_mod)
{
    uECC_word_t l_carry = vli_add(p_result, p_left, p_right);
    if(l_carry || vli_cmp(p_result, p_mod) >= 0)
    { /* p_result > p_mod (p_result = p_mod + remainder), so subtract p_mod to get remainder. */
        vli_sub(p_result, p_result, p_mod);
    }
}
#endif

/* Computes p_result = (p_left - p_right) % p_mod.
   Assumes that p_left < p_mod and p_right < p_mod, p_result != p_mod. */
#if !asm_modSub
static void vli_modSub(uECC_word_t *p_result, uECC_word_t *p_left, uECC_word_t *p_right, uECC_word_t *p_mod)
{
    uECC_word_t l_borrow = vli_sub(p_result, p_left, p_right);
    if(l_borrow)
    { /* In this case, p_result == -diff == (max int) - diff.
         Since -x % d == d - x, we can get the correct result from p_result + p_mod (with overflow). */
        vli_add(p_result, p_result, p_mod);
    }
}
#endif

#if !asm_modSub_fast
    #define vli_modSub_fast(result, left, right) vli_modSub((result), (left), (right), curve_p)
#endif

#if !asm_mmod_fast

#if (uECC_CURVE == uECC_secp160r1 || uECC_CURVE == uECC_secp256k1)
/* omega_mult() is defined farther below for the different curves / word sizes */
static void omega_mult(uECC_word_t * RESTRICT p_result, uECC_word_t * RESTRICT p_right);

/* Computes p_result = p_product % curve_p
    see http://www.isys.uni-klu.ac.at/PDF/2001-0126-MT.pdf page 354
    
    Note that this only works if log2(omega) < log2(p)/2 */
static void vli_mmod_fast(uECC_word_t *RESTRICT p_result, uECC_word_t *RESTRICT p_product)
{
    uECC_word_t l_tmp[2*uECC_WORDS];
    uECC_word_t l_carry;
    
    vli_clear(l_tmp);
    vli_clear(l_tmp + uECC_WORDS);
    
    omega_mult(l_tmp, p_product + uECC_WORDS); /* (Rq, q) = q * c */
    
    l_carry = vli_add(p_result, p_product, l_tmp); /* (C, r) = r + q       */
    vli_clear(p_product);
    omega_mult(p_product, l_tmp + uECC_WORDS); /* Rq*c */
    l_carry += vli_add(p_result, p_result, p_product); /* (C1, r) = r + Rq*c */
    
    while(l_carry > 0)
    {
        --l_carry;
        vli_sub(p_result, p_result, curve_p);
    }
    
    if(vli_cmp(p_result, curve_p) > 0)
    {
        vli_sub(p_result, p_result, curve_p);
    }
}

#endif

#if uECC_CURVE == uECC_secp160r1

#if uECC_WORD_SIZE == 1
static void omega_mult(uint8_t * RESTRICT p_result, uint8_t * RESTRICT p_right)
{
    uint8_t l_carry;
    uint8_t i;
    
    /* Multiply by (2^31 + 1). */
    vli_set(p_result + 4, p_right); /* 2^32 */
    vli_rshift1(p_result + 4); /* 2^31 */
    p_result[3] = p_right[0] << 7; /* get last bit from shift */
    
    l_carry = vli_add(p_result, p_result, p_right); /* 2^31 + 1 */
    for(i = uECC_WORDS; l_carry; ++i)
    {
        uint16_t l_sum = (uint16_t)p_result[i] + l_carry;
        p_result[i] = (uint8_t)l_sum;
        l_carry = l_sum >> 8;
    }
}
#elif uECC_WORD_SIZE == 4
static void omega_mult(uint32_t * RESTRICT p_result, uint32_t * RESTRICT p_right)
{
    uint32_t l_carry;
    unsigned i;
    
    /* Multiply by (2^31 + 1). */
    vli_set(p_result + 1, p_right); /* 2^32 */
    vli_rshift1(p_result + 1); /* 2^31 */
    p_result[0] = p_right[0] << 31; /* get last bit from shift */
    
    l_carry = vli_add(p_result, p_result, p_right); /* 2^31 + 1 */
    for(i = uECC_WORDS; l_carry; ++i)
    {
        uint64_t l_sum = (uint64_t)p_result[i] + l_carry;
        p_result[i] = (uint32_t)l_sum;
        l_carry = l_sum >> 32;
    }
}
#endif /* uECC_WORD_SIZE */

#elif uECC_CURVE == uECC_secp192r1

/* Computes p_result = p_product % curve_p.
   See algorithm 5 and 6 from http://www.isys.uni-klu.ac.at/PDF/2001-0126-MT.pdf */
#if uECC_WORD_SIZE == 1
static void vli_mmod_fast(uint8_t *RESTRICT p_result, uint8_t *RESTRICT p_product)
{
    uint8_t l_tmp[uECC_WORDS];
    uint8_t l_carry;
    
    vli_set(p_result, p_product);
    
    vli_set(l_tmp, &p_product[24]);
    l_carry = vli_add(p_result, p_result, l_tmp);
    
    l_tmp[0] = l_tmp[1] = l_tmp[2] = l_tmp[3] = l_tmp[4] = l_tmp[5] = l_tmp[6] = l_tmp[7] = 0;
    l_tmp[8] = p_product[24]; l_tmp[9] = p_product[25]; l_tmp[10] = p_product[26]; l_tmp[11] = p_product[27];
    l_tmp[12] = p_product[28]; l_tmp[13] = p_product[29]; l_tmp[14] = p_product[30]; l_tmp[15] = p_product[31];
    l_tmp[16] = p_product[32]; l_tmp[17] = p_product[33]; l_tmp[18] = p_product[34]; l_tmp[19] = p_product[35];
    l_tmp[20] = p_product[36]; l_tmp[21] = p_product[37]; l_tmp[22] = p_product[38]; l_tmp[23] = p_product[39];
    l_carry += vli_add(p_result, p_result, l_tmp);
    
    l_tmp[0] = l_tmp[8] = p_product[40];
    l_tmp[1] = l_tmp[9] = p_product[41];
    l_tmp[2] = l_tmp[10] = p_product[42];
    l_tmp[3] = l_tmp[11] = p_product[43];
    l_tmp[4] = l_tmp[12] = p_product[44];
    l_tmp[5] = l_tmp[13] = p_product[45];
    l_tmp[6] = l_tmp[14] = p_product[46];
    l_tmp[7] = l_tmp[15] = p_product[47];
    l_tmp[16] = l_tmp[17] = l_tmp[18] = l_tmp[19] = l_tmp[20] = l_tmp[21] = l_tmp[22] = l_tmp[23] = 0;
    l_carry += vli_add(p_result, p_result, l_tmp);
    
    while(l_carry || vli_cmp(curve_p, p_result) != 1)
    {
        l_carry -= vli_sub(p_result, p_result, curve_p);
    }
}
#elif uECC_WORD_SIZE == 4
static void vli_mmod_fast(uint32_t *RESTRICT p_result, uint32_t *RESTRICT p_product)
{
    uint32_t l_tmp[uECC_WORDS];
    int l_carry;
    
    vli_set(p_result, p_product);
    
    vli_set(l_tmp, &p_product[6]);
    l_carry = vli_add(p_result, p_result, l_tmp);
    
    l_tmp[0] = l_tmp[1] = 0;
    l_tmp[2] = p_product[6];
    l_tmp[3] = p_product[7];
    l_tmp[4] = p_product[8];
    l_tmp[5] = p_product[9];
    l_carry += vli_add(p_result, p_result, l_tmp);
    
    l_tmp[0] = l_tmp[2] = p_product[10];
    l_tmp[1] = l_tmp[3] = p_product[11];
    l_tmp[4] = l_tmp[5] = 0;
    l_carry += vli_add(p_result, p_result, l_tmp);
    
    while(l_carry || vli_cmp(curve_p, p_result) != 1)
    {
        l_carry -= vli_sub(p_result, p_result, curve_p);
    }
}
#else
static void vli_mmod_fast(uint64_t *RESTRICT p_result, uint64_t *RESTRICT p_product)
{
    uint64_t l_tmp[uECC_WORDS];
    int l_carry;
    
    vli_set(p_result, p_product);
    
    vli_set(l_tmp, &p_product[3]);
    l_carry = vli_add(p_result, p_result, l_tmp);
    
    l_tmp[0] = 0;
    l_tmp[1] = p_product[3];
    l_tmp[2] = p_product[4];
    l_carry += vli_add(p_result, p_result, l_tmp);
    
    l_tmp[0] = l_tmp[1] = p_product[5];
    l_tmp[2] = 0;
    l_carry += vli_add(p_result, p_result, l_tmp);
    
    while(l_carry || vli_cmp(curve_p, p_result) != 1)
    {
        l_carry -= vli_sub(p_result, p_result, curve_p);
    }
}
#endif /* uECC_WORD_SIZE */

#elif uECC_CURVE == uECC_secp256r1

/* Computes p_result = p_product % curve_p
   from http://www.nsa.gov/ia/_files/nist-routines.pdf */
#if uECC_WORD_SIZE == 1
static void vli_mmod_fast(uint8_t *RESTRICT p_result, uint8_t *RESTRICT p_product)
{
    uint8_t l_tmp[uECC_BYTES];
    int8_t l_carry;
    
    /* t */
    vli_set(p_result, p_product);
    
    /* s1 */
    l_tmp[0] = l_tmp[1] = l_tmp[2] = l_tmp[3] = 0;
    l_tmp[4] = l_tmp[5] = l_tmp[6] = l_tmp[7] = 0;
    l_tmp[8] = l_tmp[9] = l_tmp[10] = l_tmp[11] = 0;
    l_tmp[12] = p_product[44]; l_tmp[13] = p_product[45]; l_tmp[14] = p_product[46]; l_tmp[15] = p_product[47];
    l_tmp[16] = p_product[48]; l_tmp[17] = p_product[49]; l_tmp[18] = p_product[50]; l_tmp[19] = p_product[51];
    l_tmp[20] = p_product[52]; l_tmp[21] = p_product[53]; l_tmp[22] = p_product[54]; l_tmp[23] = p_product[55];
    l_tmp[24] = p_product[56]; l_tmp[25] = p_product[57]; l_tmp[26] = p_product[58]; l_tmp[27] = p_product[59];
    l_tmp[28] = p_product[60]; l_tmp[29] = p_product[61]; l_tmp[30] = p_product[62]; l_tmp[31] = p_product[63];
    l_carry = vli_add(l_tmp, l_tmp, l_tmp);
    l_carry += vli_add(p_result, p_result, l_tmp);
    
    /* s2 */
    l_tmp[12] = p_product[48]; l_tmp[13] = p_product[49]; l_tmp[14] = p_product[50]; l_tmp[15] = p_product[51];
    l_tmp[16] = p_product[52]; l_tmp[17] = p_product[53]; l_tmp[18] = p_product[54]; l_tmp[19] = p_product[55];
    l_tmp[20] = p_product[56]; l_tmp[21] = p_product[57]; l_tmp[22] = p_product[58]; l_tmp[23] = p_product[59];
    l_tmp[24] = p_product[60]; l_tmp[25] = p_product[61]; l_tmp[26] = p_product[62]; l_tmp[27] = p_product[63];
    l_tmp[28] = l_tmp[29] = l_tmp[30] = l_tmp[31] = 0;
    l_carry += vli_add(l_tmp, l_tmp, l_tmp);
    l_carry += vli_add(p_result, p_result, l_tmp);
    
    /* s3 */
    l_tmp[0] = p_product[32]; l_tmp[1] = p_product[33]; l_tmp[2] = p_product[34]; l_tmp[3] = p_product[35];
    l_tmp[4] = p_product[36]; l_tmp[5] = p_product[37]; l_tmp[6] = p_product[38]; l_tmp[7] = p_product[39];
    l_tmp[8] = p_product[40]; l_tmp[9] = p_product[41]; l_tmp[10] = p_product[42]; l_tmp[11] = p_product[43];
    l_tmp[12] = l_tmp[13] = l_tmp[14] = l_tmp[15] = 0;
    l_tmp[16] = l_tmp[17] = l_tmp[18] = l_tmp[19] = 0;
    l_tmp[20] = l_tmp[21] = l_tmp[22] = l_tmp[23] = 0;
    l_tmp[24] = p_product[56]; l_tmp[25] = p_product[57]; l_tmp[26] = p_product[58]; l_tmp[27] = p_product[59];
    l_tmp[28] = p_product[60]; l_tmp[29] = p_product[61]; l_tmp[30] = p_product[62]; l_tmp[31] = p_product[63];
    l_carry += vli_add(p_result, p_result, l_tmp);
    
    /* s4 */
    l_tmp[0] = p_product[36]; l_tmp[1] = p_product[37]; l_tmp[2] = p_product[38]; l_tmp[3] = p_product[39];
    l_tmp[4] = p_product[40]; l_tmp[5] = p_product[41]; l_tmp[6] = p_product[42]; l_tmp[7] = p_product[43];
    l_tmp[8] = p_product[44]; l_tmp[9] = p_product[45]; l_tmp[10] = p_product[46]; l_tmp[11] = p_product[47];
    l_tmp[12] = p_product[52]; l_tmp[13] = p_product[53]; l_tmp[14] = p_product[54]; l_tmp[15] = p_product[55];
    l_tmp[16] = p_product[56]; l_tmp[17] = p_product[57]; l_tmp[18] = p_product[58]; l_tmp[19] = p_product[59];
    l_tmp[20] = p_product[60]; l_tmp[21] = p_product[61]; l_tmp[22] = p_product[62]; l_tmp[23] = p_product[63];
    l_tmp[24] = p_product[52]; l_tmp[25] = p_product[53]; l_tmp[26] = p_product[54]; l_tmp[27] = p_product[55];
    l_tmp[28] = p_product[32]; l_tmp[29] = p_product[33]; l_tmp[30] = p_product[34]; l_tmp[31] = p_product[35];
    l_carry += vli_add(p_result, p_result, l_tmp);
    
    /* d1 */
    l_tmp[0] = p_product[44]; l_tmp[1] = p_product[45]; l_tmp[2] = p_product[46]; l_tmp[3] = p_product[47];
    l_tmp[4] = p_product[48]; l_tmp[5] = p_product[49]; l_tmp[6] = p_product[50]; l_tmp[7] = p_product[51];
    l_tmp[8] = p_product[52]; l_tmp[9] = p_product[53]; l_tmp[10] = p_product[54]; l_tmp[11] = p_product[55];
    l_tmp[12] = l_tmp[13] = l_tmp[14] = l_tmp[15] = 0;
    l_tmp[16] = l_tmp[17] = l_tmp[18] = l_tmp[19] = 0;
    l_tmp[20] = l_tmp[21] = l_tmp[22] = l_tmp[23] = 0;
    l_tmp[24] = p_product[32]; l_tmp[25] = p_product[33]; l_tmp[26] = p_product[34]; l_tmp[27] = p_product[35];
    l_tmp[28] = p_product[40]; l_tmp[29] = p_product[41]; l_tmp[30] = p_product[42]; l_tmp[31] = p_product[43];
    l_carry -= vli_sub(p_result, p_result, l_tmp);
    
    /* d2 */
    l_tmp[0] = p_product[48]; l_tmp[1] = p_product[49]; l_tmp[2] = p_product[50]; l_tmp[3] = p_product[51];
    l_tmp[4] = p_product[52]; l_tmp[5] = p_product[53]; l_tmp[6] = p_product[54]; l_tmp[7] = p_product[55];
    l_tmp[8] = p_product[56]; l_tmp[9] = p_product[57]; l_tmp[10] = p_product[58]; l_tmp[11] = p_product[59];
    l_tmp[12] = p_product[60]; l_tmp[13] = p_product[61]; l_tmp[14] = p_product[62]; l_tmp[15] = p_product[63];
    l_tmp[16] = l_tmp[17] = l_tmp[18] = l_tmp[19] = 0;
    l_tmp[20] = l_tmp[21] = l_tmp[22] = l_tmp[23] = 0;
    l_tmp[24] = p_product[36]; l_tmp[25] = p_product[37]; l_tmp[26] = p_product[38]; l_tmp[27] = p_product[39];
    l_tmp[28] = p_product[44]; l_tmp[29] = p_product[45]; l_tmp[30] = p_product[46]; l_tmp[31] = p_product[47];
    l_carry -= vli_sub(p_result, p_result, l_tmp);
    
    /* d3 */
    l_tmp[0] = p_product[52]; l_tmp[1] = p_product[53]; l_tmp[2] = p_product[54]; l_tmp[3] = p_product[55];
    l_tmp[4] = p_product[56]; l_tmp[5] = p_product[57]; l_tmp[6] = p_product[58]; l_tmp[7] = p_product[59];
    l_tmp[8] = p_product[60]; l_tmp[9] = p_product[61]; l_tmp[10] = p_product[62]; l_tmp[11] = p_product[63];
    l_tmp[12] = p_product[32]; l_tmp[13] = p_product[33]; l_tmp[14] = p_product[34]; l_tmp[15] = p_product[35];
    l_tmp[16] = p_product[36]; l_tmp[17] = p_product[37]; l_tmp[18] = p_product[38]; l_tmp[19] = p_product[39];
    l_tmp[20] = p_product[40]; l_tmp[21] = p_product[41]; l_tmp[22] = p_product[42]; l_tmp[23] = p_product[43];
    l_tmp[24] = l_tmp[25] = l_tmp[26] = l_tmp[27] = 0;
    l_tmp[28] = p_product[48]; l_tmp[29] = p_product[49]; l_tmp[30] = p_product[50]; l_tmp[31] = p_product[51];
    l_carry -= vli_sub(p_result, p_result, l_tmp);
    
    /* d4 */
    l_tmp[0] = p_product[56]; l_tmp[1] = p_product[57]; l_tmp[2] = p_product[58]; l_tmp[3] = p_product[59];
    l_tmp[4] = p_product[60]; l_tmp[5] = p_product[61]; l_tmp[6] = p_product[62]; l_tmp[7] = p_product[63];
    l_tmp[8] = l_tmp[9] = l_tmp[10] = l_tmp[11] = 0;
    l_tmp[12] = p_product[36]; l_tmp[13] = p_product[37]; l_tmp[14] = p_product[38]; l_tmp[15] = p_product[39];
    l_tmp[16] = p_product[40]; l_tmp[17] = p_product[41]; l_tmp[18] = p_product[42]; l_tmp[19] = p_product[43];
    l_tmp[20] = p_product[44]; l_tmp[21] = p_product[45]; l_tmp[22] = p_product[46]; l_tmp[23] = p_product[47];
    l_tmp[24] = l_tmp[25] = l_tmp[26] = l_tmp[27] = 0;
    l_tmp[28] = p_product[52]; l_tmp[29] = p_product[53]; l_tmp[30] = p_product[54]; l_tmp[31] = p_product[55];
    l_carry -= vli_sub(p_result, p_result, l_tmp);
    
    if(l_carry < 0)
    {
        do
        {
            l_carry += vli_add(p_result, p_result, curve_p);
        } while(l_carry < 0);
    }
    else
    {
        while(l_carry || vli_cmp(curve_p, p_result) != 1)
        {
            l_carry -= vli_sub(p_result, p_result, curve_p);
        }
    }
}
#elif uECC_WORD_SIZE == 4
static void vli_mmod_fast(uint32_t *RESTRICT p_result, uint32_t *RESTRICT p_product)
{
    uint32_t l_tmp[uECC_WORDS];
    int l_carry;
    
    /* t */
    vli_set(p_result, p_product);
    
    /* s1 */
    l_tmp[0] = l_tmp[1] = l_tmp[2] = 0;
    l_tmp[3] = p_product[11];
    l_tmp[4] = p_product[12];
    l_tmp[5] = p_product[13];
    l_tmp[6] = p_product[14];
    l_tmp[7] = p_product[15];
    l_carry = vli_add(l_tmp, l_tmp, l_tmp);
    l_carry += vli_add(p_result, p_result, l_tmp);
    
    /* s2 */
    l_tmp[3] = p_product[12];
    l_tmp[4] = p_product[13];
    l_tmp[5] = p_product[14];
    l_tmp[6] = p_product[15];
    l_tmp[7] = 0;
    l_carry += vli_add(l_tmp, l_tmp, l_tmp);
    l_carry += vli_add(p_result, p_result, l_tmp);
    
    /* s3 */
    l_tmp[0] = p_product[8];
    l_tmp[1] = p_product[9];
    l_tmp[2] = p_product[10];
    l_tmp[3] = l_tmp[4] = l_tmp[5] = 0;
    l_tmp[6] = p_product[14];
    l_tmp[7] = p_product[15];
    l_carry += vli_add(p_result, p_result, l_tmp);
    
    /* s4 */
    l_tmp[0] = p_product[9];
    l_tmp[1] = p_product[10];
    l_tmp[2] = p_product[11];
    l_tmp[3] = p_product[13];
    l_tmp[4] = p_product[14];
    l_tmp[5] = p_product[15];
    l_tmp[6] = p_product[13];
    l_tmp[7] = p_product[8];
    l_carry += vli_add(p_result, p_result, l_tmp);
    
    /* d1 */
    l_tmp[0] = p_product[11];
    l_tmp[1] = p_product[12];
    l_tmp[2] = p_product[13];
    l_tmp[3] = l_tmp[4] = l_tmp[5] = 0;
    l_tmp[6] = p_product[8];
    l_tmp[7] = p_product[10];
    l_carry -= vli_sub(p_result, p_result, l_tmp);
    
    /* d2 */
    l_tmp[0] = p_product[12];
    l_tmp[1] = p_product[13];
    l_tmp[2] = p_product[14];
    l_tmp[3] = p_product[15];
    l_tmp[4] = l_tmp[5] = 0;
    l_tmp[6] = p_product[9];
    l_tmp[7] = p_product[11];
    l_carry -= vli_sub(p_result, p_result, l_tmp);
    
    /* d3 */
    l_tmp[0] = p_product[13];
    l_tmp[1] = p_product[14];
    l_tmp[2] = p_product[15];
    l_tmp[3] = p_product[8];
    l_tmp[4] = p_product[9];
    l_tmp[5] = p_product[10];
    l_tmp[6] = 0;
    l_tmp[7] = p_product[12];
    l_carry -= vli_sub(p_result, p_result, l_tmp);
    
    /* d4 */
    l_tmp[0] = p_product[14];
    l_tmp[1] = p_product[15];
    l_tmp[2] = 0;
    l_tmp[3] = p_product[9];
    l_tmp[4] = p_product[10];
    l_tmp[5] = p_product[11];
    l_tmp[6] = 0;
    l_tmp[7] = p_product[13];
    l_carry -= vli_sub(p_result, p_result, l_tmp);
    
    if(l_carry < 0)
    {
        do
        {
            l_carry += vli_add(p_result, p_result, curve_p);
        } while(l_carry < 0);
    }
    else
    {
        while(l_carry || vli_cmp(curve_p, p_result) != 1)
        {
            l_carry -= vli_sub(p_result, p_result, curve_p);
        }
    }
}
#else
static void vli_mmod_fast(uint64_t *RESTRICT p_result, uint64_t *RESTRICT p_product)
{
    uint64_t l_tmp[uECC_WORDS];
    int l_carry;
    
    /* t */
    vli_set(p_result, p_product);
    
    /* s1 */
    l_tmp[0] = 0;
    l_tmp[1] = p_product[5] & 0xffffffff00000000ull;
    l_tmp[2] = p_product[6];
    l_tmp[3] = p_product[7];
    l_carry = vli_add(l_tmp, l_tmp, l_tmp);
    l_carry += vli_add(p_result, p_result, l_tmp);
    
    /* s2 */
    l_tmp[1] = p_product[6] << 32;
    l_tmp[2] = (p_product[6] >> 32) | (p_product[7] << 32);
    l_tmp[3] = p_product[7] >> 32;
    l_carry += vli_add(l_tmp, l_tmp, l_tmp);
    l_carry += vli_add(p_result, p_result, l_tmp);
    
    /* s3 */
    l_tmp[0] = p_product[4];
    l_tmp[1] = p_product[5] & 0xffffffff;
    l_tmp[2] = 0;
    l_tmp[3] = p_product[7];
    l_carry += vli_add(p_result, p_result, l_tmp);
    
    /* s4 */
    l_tmp[0] = (p_product[4] >> 32) | (p_product[5] << 32);
    l_tmp[1] = (p_product[5] >> 32) | (p_product[6] & 0xffffffff00000000ull);
    l_tmp[2] = p_product[7];
    l_tmp[3] = (p_product[6] >> 32) | (p_product[4] << 32);
    l_carry += vli_add(p_result, p_result, l_tmp);
    
    /* d1 */
    l_tmp[0] = (p_product[5] >> 32) | (p_product[6] << 32);
    l_tmp[1] = (p_product[6] >> 32);
    l_tmp[2] = 0;
    l_tmp[3] = (p_product[4] & 0xffffffff) | (p_product[5] << 32);
    l_carry -= vli_sub(p_result, p_result, l_tmp);
    
    /* d2 */
    l_tmp[0] = p_product[6];
    l_tmp[1] = p_product[7];
    l_tmp[2] = 0;
    l_tmp[3] = (p_product[4] >> 32) | (p_product[5] & 0xffffffff00000000ull);
    l_carry -= vli_sub(p_result, p_result, l_tmp);
    
    /* d3 */
    l_tmp[0] = (p_product[6] >> 32) | (p_product[7] << 32);
    l_tmp[1] = (p_product[7] >> 32) | (p_product[4] << 32);
    l_tmp[2] = (p_product[4] >> 32) | (p_product[5] << 32);
    l_tmp[3] = (p_product[6] << 32);
    l_carry -= vli_sub(p_result, p_result, l_tmp);
    
    /* d4 */
    l_tmp[0] = p_product[7];
    l_tmp[1] = p_product[4] & 0xffffffff00000000ull;
    l_tmp[2] = p_product[5];
    l_tmp[3] = p_product[6] & 0xffffffff00000000ull;
    l_carry -= vli_sub(p_result, p_result, l_tmp);
    
    if(l_carry < 0)
    {
        do
        {
            l_carry += vli_add(p_result, p_result, curve_p);
        } while(l_carry < 0);
    }
    else
    {
        while(l_carry || vli_cmp(curve_p, p_result) != 1)
        {
            l_carry -= vli_sub(p_result, p_result, curve_p);
        }
    }
}
#endif /* uECC_WORD_SIZE */

#elif uECC_CURVE == uECC_secp256k1

#if uECC_WORD_SIZE == 1
static void omega_mult(uint8_t * RESTRICT p_result, uint8_t * RESTRICT p_right)
{
    /* Multiply by (2^32 + 2^9 + 2^8 + 2^7 + 2^6 + 2^4 + 1). */
    uECC_word_t r0 = 0;
    uECC_word_t r1 = 0;
    uECC_word_t r2 = 0;
    
    wordcount_t k;
    
    /* Multiply by (2^9 + 2^8 + 2^7 + 2^6 + 2^4 + 1). */
    muladd(0xD1, p_right[0], &r0, &r1, &r2);
    p_result[0] = r0;
    r0 = r1;
    r1 = r2;
    /* r2 is still 0 */
    
    for(k = 1; k < uECC_WORDS; ++k)
    {
        muladd(0x03, p_right[k-1], &r0, &r1, &r2);
        muladd(0xD1, p_right[k], &r0, &r1, &r2);
        p_result[k] = r0;
        r0 = r1;
        r1 = r2;
        r2 = 0;
    }
    
    muladd(0x03, p_right[uECC_WORDS-1], &r0, &r1, &r2);
    p_result[uECC_WORDS] = r0;
    p_result[uECC_WORDS + 1] = r1;
    
    p_result[4 + uECC_WORDS] = vli_add(p_result + 4, p_result + 4, p_right); /* add the 2^32 multiple */
}
#elif uECC_WORD_SIZE == 4
static void omega_mult(uint32_t * RESTRICT p_result, uint32_t * RESTRICT p_right)
{
    /* Multiply by (2^9 + 2^8 + 2^7 + 2^6 + 2^4 + 1). */
    uint32_t l_carry = 0;
    wordcount_t k;
    
    for(k = 0; k < uECC_WORDS; ++k)
    {
        uint64_t p = (uint64_t)0x3D1 * p_right[k] + l_carry;
        p_result[k] = (p & 0xffffffff);
        l_carry = p >> 32;
    }
    p_result[uECC_WORDS] = l_carry;
    
    p_result[1 + uECC_WORDS] = vli_add(p_result + 1, p_result + 1, p_right); /* add the 2^32 multiple */
}
#else
static void omega_mult(uint64_t * RESTRICT p_result, uint64_t * RESTRICT p_right)
{
    uECC_word_t r0 = 0;
    uECC_word_t r1 = 0;
    uECC_word_t r2 = 0;
    
    wordcount_t k;
    
    /* Multiply by (2^32 + 2^9 + 2^8 + 2^7 + 2^6 + 2^4 + 1). */
    for(k = 0; k < uECC_WORDS; ++k)
    {
        muladd(0x1000003D1ull, p_right[k], &r0, &r1, &r2);
        p_result[k] = r0;
        r0 = r1;
        r1 = r2;
        r2 = 0;
    }
    
    p_result[uECC_WORDS] = r0;
}
#endif /* uECC_WORD_SIZE */

#endif /* uECC_CURVE */
#endif /* !asm_mmod_fast */

/* Computes p_result = (p_left * p_right) % curve_p. */
static void vli_modMult_fast(uECC_word_t *p_result, uECC_word_t *p_left, uECC_word_t *p_right)
{
    uECC_word_t l_product[2 * uECC_WORDS];
    vli_mult(l_product, p_left, p_right);
    vli_mmod_fast(p_result, l_product);
}

#if uECC_SQUARE_FUNC

/* Computes p_result = p_left^2 % curve_p. */
static void vli_modSquare_fast(uECC_word_t *p_result, uECC_word_t *p_left)
{
    uECC_word_t l_product[2 * uECC_WORDS];
    vli_square(l_product, p_left);
    vli_mmod_fast(p_result, l_product);
}

#else /* uECC_SQUARE_FUNC */

#define vli_modSquare_fast(result, left) vli_modMult_fast((result), (left), (left))
    
#endif /* uECC_SQUARE_FUNC */


#define EVEN(vli) (!(vli[0] & 1))
/* Computes p_result = (1 / p_input) % p_mod. All VLIs are the same size.
   See "From Euclid's GCD to Montgomery Multiplication to the Great Divide"
   https://labs.oracle.com/techrep/2001/smli_tr-2001-95.pdf */
#if !asm_modInv
static void vli_modInv(uECC_word_t *p_result, uECC_word_t *p_input, uECC_word_t *p_mod)
{
    uECC_word_t a[uECC_WORDS], b[uECC_WORDS], u[uECC_WORDS], v[uECC_WORDS];
    uECC_word_t l_carry;
    cmpresult_t l_cmpResult;
    
    if(vli_isZero(p_input))
    {
        vli_clear(p_result);
        return;
    }

    vli_set(a, p_input);
    vli_set(b, p_mod);
    vli_clear(u);
    u[0] = 1;
    vli_clear(v);
    while((l_cmpResult = vli_cmp(a, b)) != 0)
    {
        l_carry = 0;
        if(EVEN(a))
        {
            vli_rshift1(a);
            if(!EVEN(u))
            {
                l_carry = vli_add(u, u, p_mod);
            }
            vli_rshift1(u);
            if(l_carry)
            {
                u[uECC_WORDS-1] |= HIGH_BIT_SET;
            }
        }
        else if(EVEN(b))
        {
            vli_rshift1(b);
            if(!EVEN(v))
            {
                l_carry = vli_add(v, v, p_mod);
            }
            vli_rshift1(v);
            if(l_carry)
            {
                v[uECC_WORDS-1] |= HIGH_BIT_SET;
            }
        }
        else if(l_cmpResult > 0)
        {
            vli_sub(a, a, b);
            vli_rshift1(a);
            if(vli_cmp(u, v) < 0)
            {
                vli_add(u, u, p_mod);
            }
            vli_sub(u, u, v);
            if(!EVEN(u))
            {
                l_carry = vli_add(u, u, p_mod);
            }
            vli_rshift1(u);
            if(l_carry)
            {
                u[uECC_WORDS-1] |= HIGH_BIT_SET;
            }
        }
        else
        {
            vli_sub(b, b, a);
            vli_rshift1(b);
            if(vli_cmp(v, u) < 0)
            {
                vli_add(v, v, p_mod);
            }
            vli_sub(v, v, u);
            if(!EVEN(v))
            {
                l_carry = vli_add(v, v, p_mod);
            }
            vli_rshift1(v);
            if(l_carry)
            {
                v[uECC_WORDS-1] |= HIGH_BIT_SET;
            }
        }
    }
    
    vli_set(p_result, u);
}
#endif /* !asm_modInv */

/* ------ Point operations ------ */

/* Returns 1 if p_point is the point at infinity, 0 otherwise. */
static cmpresult_t EccPoint_isZero(EccPoint *p_point)
{
    return (vli_isZero(p_point->x) && vli_isZero(p_point->y));
}

/* Point multiplication algorithm using Montgomery's ladder with co-Z coordinates.
From http://eprint.iacr.org/2011/338.pdf
*/

/* Double in place */
#if (uECC_CURVE == uECC_secp256k1)
static void EccPoint_double_jacobian(uECC_word_t * RESTRICT X1, uECC_word_t * RESTRICT Y1, uECC_word_t * RESTRICT Z1)
{
    /* t1 = X, t2 = Y, t3 = Z */
    uECC_word_t t4[uECC_WORDS];
    uECC_word_t t5[uECC_WORDS];
    
    if(vli_isZero(Z1))
    {
        return;
    }
    
    vli_modSquare_fast(t5, Y1);   /* t5 = y1^2 */
    vli_modMult_fast(t4, X1, t5); /* t4 = x1*y1^2 = A */
    vli_modSquare_fast(X1, X1);   /* t1 = x1^2 */
    vli_modSquare_fast(t5, t5);   /* t5 = y1^4 */
    vli_modMult_fast(Z1, Y1, Z1); /* t3 = y1*z1 = z3 */
    
    vli_modAdd(Y1, X1, X1, curve_p); /* t2 = 2*x1^2 */
    vli_modAdd(Y1, Y1, X1, curve_p); /* t2 = 3*x1^2 */
    if(vli_testBit(Y1, 0))
    {
        uECC_word_t l_carry = vli_add(Y1, Y1, curve_p);
        vli_rshift1(Y1);
        Y1[uECC_WORDS-1] |= l_carry << (uECC_WORD_BITS - 1);
    }
    else
    {
        vli_rshift1(Y1);
    }
    /* t2 = 3/2*(x1^2) = B */
    
    vli_modSquare_fast(X1, Y1);   /* t1 = B^2 */
    vli_modSub(X1, X1, t4, curve_p); /* t1 = B^2 - A */
    vli_modSub(X1, X1, t4, curve_p); /* t1 = B^2 - 2A = x3 */
    
    vli_modSub(t4, t4, X1, curve_p); /* t4 = A - x3 */
    vli_modMult_fast(Y1, Y1, t4);    /* t2 = B * (A - x3) */
    vli_modSub(Y1, Y1, t5, curve_p); /* t2 = B * (A - x3) - y1^4 = y3 */
}
#else
static void EccPoint_double_jacobian(uECC_word_t * RESTRICT X1, uECC_word_t * RESTRICT Y1, uECC_word_t * RESTRICT Z1)
{
    /* t1 = X, t2 = Y, t3 = Z */
    uECC_word_t t4[uECC_WORDS];
    uECC_word_t t5[uECC_WORDS];
    
    if(vli_isZero(Z1))
    {
        return;
    }
    
    vli_modSquare_fast(t4, Y1);   /* t4 = y1^2 */
    vli_modMult_fast(t5, X1, t4); /* t5 = x1*y1^2 = A */
    vli_modSquare_fast(t4, t4);   /* t4 = y1^4 */
    vli_modMult_fast(Y1, Y1, Z1); /* t2 = y1*z1 = z3 */
    vli_modSquare_fast(Z1, Z1);   /* t3 = z1^2 */
    
    vli_modAdd(X1, X1, Z1, curve_p); /* t1 = x1 + z1^2 */
    vli_modAdd(Z1, Z1, Z1, curve_p); /* t3 = 2*z1^2 */
    vli_modSub_fast(Z1, X1, Z1); /* t3 = x1 - z1^2 */
    vli_modMult_fast(X1, X1, Z1);    /* t1 = x1^2 - z1^4 */
    
    vli_modAdd(Z1, X1, X1, curve_p); /* t3 = 2*(x1^2 - z1^4) */
    vli_modAdd(X1, X1, Z1, curve_p); /* t1 = 3*(x1^2 - z1^4) */
    if(vli_testBit(X1, 0))
    {
        uECC_word_t l_carry = vli_add(X1, X1, curve_p);
        vli_rshift1(X1);
        X1[uECC_WORDS-1] |= l_carry << (uECC_WORD_BITS - 1);
    }
    else
    {
        vli_rshift1(X1);
    }
    /* t1 = 3/2*(x1^2 - z1^4) = B */
    
    vli_modSquare_fast(Z1, X1);      /* t3 = B^2 */
    vli_modSub_fast(Z1, Z1, t5); /* t3 = B^2 - A */
    vli_modSub_fast(Z1, Z1, t5); /* t3 = B^2 - 2A = x3 */
    vli_modSub_fast(t5, t5, Z1); /* t5 = A - x3 */
    vli_modMult_fast(X1, X1, t5);    /* t1 = B * (A - x3) */
    vli_modSub_fast(t4, X1, t4); /* t4 = B * (A - x3) - y1^4 = y3 */
    
    vli_set(X1, Z1);
    vli_set(Z1, Y1);
    vli_set(Y1, t4);
}
#endif

/* Modify (x1, y1) => (x1 * z^2, y1 * z^3) */
static void apply_z(uECC_word_t * RESTRICT X1, uECC_word_t * RESTRICT Y1, uECC_word_t * RESTRICT Z)
{
    uECC_word_t t1[uECC_WORDS];

    vli_modSquare_fast(t1, Z);    /* z^2 */
    vli_modMult_fast(X1, X1, t1); /* x1 * z^2 */
    vli_modMult_fast(t1, t1, Z);  /* z^3 */
    vli_modMult_fast(Y1, Y1, t1); /* y1 * z^3 */
}

/* P = (x1, y1) => 2P, (x2, y2) => P' */
static void XYcZ_initial_double(uECC_word_t * RESTRICT X1, uECC_word_t * RESTRICT Y1,
    uECC_word_t * RESTRICT X2, uECC_word_t * RESTRICT Y2, const uECC_word_t * RESTRICT p_initialZ)
{
    uECC_word_t z[uECC_WORDS];
    
    vli_set(X2, X1);
    vli_set(Y2, Y1);
    
    vli_clear(z);
    z[0] = 1;
    if(p_initialZ)
    {
        vli_set(z, p_initialZ);
    }

    apply_z(X1, Y1, z);
    
    EccPoint_double_jacobian(X1, Y1, z);
    
    apply_z(X2, Y2, z);
}

/* Input P = (x1, y1, Z), Q = (x2, y2, Z)
   Output P' = (x1', y1', Z3), P + Q = (x3, y3, Z3)
   or P => P', Q => P + Q
*/
static void XYcZ_add(uECC_word_t * RESTRICT X1, uECC_word_t * RESTRICT Y1, uECC_word_t * RESTRICT X2, uECC_word_t * RESTRICT Y2)
{
    /* t1 = X1, t2 = Y1, t3 = X2, t4 = Y2 */
    uECC_word_t t5[uECC_WORDS];
    
    vli_modSub_fast(t5, X2, X1); /* t5 = x2 - x1 */
    vli_modSquare_fast(t5, t5);      /* t5 = (x2 - x1)^2 = A */
    vli_modMult_fast(X1, X1, t5);    /* t1 = x1*A = B */
    vli_modMult_fast(X2, X2, t5);    /* t3 = x2*A = C */
    vli_modSub_fast(Y2, Y2, Y1); /* t4 = y2 - y1 */
    vli_modSquare_fast(t5, Y2);      /* t5 = (y2 - y1)^2 = D */
    
    vli_modSub_fast(t5, t5, X1); /* t5 = D - B */
    vli_modSub_fast(t5, t5, X2); /* t5 = D - B - C = x3 */
    vli_modSub_fast(X2, X2, X1); /* t3 = C - B */
    vli_modMult_fast(Y1, Y1, X2);    /* t2 = y1*(C - B) */
    vli_modSub_fast(X2, X1, t5); /* t3 = B - x3 */
    vli_modMult_fast(Y2, Y2, X2);    /* t4 = (y2 - y1)*(B - x3) */
    vli_modSub_fast(Y2, Y2, Y1); /* t4 = y3 */
    
    vli_set(X2, t5);
}

/* Input P = (x1, y1, Z), Q = (x2, y2, Z)
   Output P + Q = (x3, y3, Z3), P - Q = (x3', y3', Z3)
   or P => P - Q, Q => P + Q
*/
static void XYcZ_addC(uECC_word_t * RESTRICT X1, uECC_word_t * RESTRICT Y1, uECC_word_t * RESTRICT X2, uECC_word_t * RESTRICT Y2)
{
    /* t1 = X1, t2 = Y1, t3 = X2, t4 = Y2 */
    uECC_word_t t5[uECC_WORDS];
    uECC_word_t t6[uECC_WORDS];
    uECC_word_t t7[uECC_WORDS];
    
    vli_modSub_fast(t5, X2, X1); /* t5 = x2 - x1 */    
    vli_modSquare_fast(t5, t5);      /* t5 = (x2 - x1)^2 = A */
    vli_modMult_fast(X1, X1, t5);    /* t1 = x1*A = B */
    vli_modMult_fast(X2, X2, t5);    /* t3 = x2*A = C */
    vli_modAdd(t5, Y2, Y1, curve_p); /* t4 = y2 + y1 */
    vli_modSub_fast(Y2, Y2, Y1); /* t4 = y2 - y1 */

    vli_modSub_fast(t6, X2, X1); /* t6 = C - B */
    vli_modMult_fast(Y1, Y1, t6);    /* t2 = y1 * (C - B) */
    vli_modAdd(t6, X1, X2, curve_p); /* t6 = B + C */
    vli_modSquare_fast(X2, Y2);      /* t3 = (y2 - y1)^2 */
    vli_modSub_fast(X2, X2, t6); /* t3 = x3 */
    
    vli_modSub_fast(t7, X1, X2); /* t7 = B - x3 */
    vli_modMult_fast(Y2, Y2, t7);    /* t4 = (y2 - y1)*(B - x3) */
    vli_modSub_fast(Y2, Y2, Y1); /* t4 = y3 */
    
    vli_modSquare_fast(t7, t5);      /* t7 = (y2 + y1)^2 = F */
    vli_modSub_fast(t7, t7, t6); /* t7 = x3' */
    vli_modSub_fast(t6, t7, X1); /* t6 = x3' - B */
    vli_modMult_fast(t6, t6, t5);    /* t6 = (y2 + y1)*(x3' - B) */
    vli_modSub_fast(Y1, t6, Y1); /* t2 = y3' */
    
    vli_set(X1, t7);
}

static void EccPoint_mult(EccPoint * RESTRICT p_result, EccPoint * RESTRICT p_point,
    const uECC_word_t * RESTRICT p_scalar, const uECC_word_t * RESTRICT p_initialZ, bitcount_t p_numBits)
{
    /* R0 and R1 */
    uECC_word_t Rx[2][uECC_WORDS];
    uECC_word_t Ry[2][uECC_WORDS];
    uECC_word_t z[uECC_WORDS];
    
    bitcount_t i;
    uECC_word_t nb;
    
    vli_set(Rx[1], p_point->x);
    vli_set(Ry[1], p_point->y);

    XYcZ_initial_double(Rx[1], Ry[1], Rx[0], Ry[0], p_initialZ);

    
    for(i = p_numBits - 2; i > 0; --i)
    {        
        PORTA.OUT = PIN0_bm;
        nb = !vli_testBit(p_scalar, i);        
        XYcZ_addC(Rx[1-nb], Ry[1-nb], Rx[nb], Ry[nb]);
        PORTA.OUT = 0;
        XYcZ_add(Rx[nb], Ry[nb], Rx[1-nb], Ry[1-nb]);
        
    }
    PORTA.OUT = 0;
    
    printf("\n");
    for(i = p_numBits - 2; i > 0; --i)
    {
        if(vli_testBit(p_scalar, i)){
            printf("1");
        } else {
            printf("0");
        }
    }
    printf("\n\n");

    nb = !vli_testBit(p_scalar, 0);
    XYcZ_addC(Rx[1-nb], Ry[1-nb], Rx[nb], Ry[nb]);
    
    /* Find final 1/Z value. */
    vli_modSub_fast(z, Rx[1], Rx[0]); /* X1 - X0 */
    vli_modMult_fast(z, z, Ry[1-nb]);     /* Yb * (X1 - X0) */
    vli_modMult_fast(z, z, p_point->x);   /* xP * Yb * (X1 - X0) */
    vli_modInv(z, z, curve_p);            /* 1 / (xP * Yb * (X1 - X0)) */
    vli_modMult_fast(z, z, p_point->y);   /* yP / (xP * Yb * (X1 - X0)) */
    vli_modMult_fast(z, z, Rx[1-nb]);     /* Xb * yP / (xP * Yb * (X1 - X0)) */
    /* End 1/Z calculation */

    XYcZ_add(Rx[nb], Ry[nb], Rx[1-nb], Ry[1-nb]);
    
    apply_z(Rx[0], Ry[0], z);
    
    vli_set(p_result->x, Rx[0]);
    vli_set(p_result->y, Ry[0]);
}

/* Compute a = sqrt(a) (mod curve_p). */
static void mod_sqrt(uECC_word_t *a)
{
    bitcount_t i;
    uECC_word_t p1[uECC_WORDS] = {1};
    uECC_word_t l_result[uECC_WORDS] = {1};
    
    /* Since curve_p == 3 (mod 4) for all supported curves, we can
       compute sqrt(a) = a^((curve_p + 1) / 4) (mod curve_p). */
    vli_add(p1, curve_p, p1); /* p1 = curve_p + 1 */
    for(i = vli_numBits(p1, uECC_WORDS) - 1; i > 1; --i)
    {
        vli_modSquare_fast(l_result, l_result);
        if(vli_testBit(p1, i))
        {
            vli_modMult_fast(l_result, l_result, a);
        }
    }
    vli_set(a, l_result);
}

#if uECC_WORD_SIZE == 1

static void vli_nativeToBytes(uint8_t * RESTRICT p_dest, const uint8_t * RESTRICT p_src)
{
    uint8_t i;
    for(i=0; i<uECC_BYTES; ++i)
    {
        p_dest[i] = p_src[(uECC_BYTES - 1) - i];
    }
}

#define vli_bytesToNative(dest, src) vli_nativeToBytes((dest), (src))

#elif uECC_WORD_SIZE == 4

static void vli_nativeToBytes(uint8_t *p_bytes, const uint32_t *p_native)
{
    unsigned i;
    for(i=0; i<uECC_WORDS; ++i)
    {
        uint8_t *p_digit = p_bytes + 4 * (uECC_WORDS - 1 - i);
        p_digit[0] = p_native[i] >> 24;
        p_digit[1] = p_native[i] >> 16;
        p_digit[2] = p_native[i] >> 8;
        p_digit[3] = p_native[i];
    }
}

static void vli_bytesToNative(uint32_t *p_native, const uint8_t *p_bytes)
{
    unsigned i;
    for(i=0; i<uECC_WORDS; ++i)
    {
        const uint8_t *p_digit = p_bytes + 4 * (uECC_WORDS - 1 - i);
        p_native[i] = ((uint32_t)p_digit[0] << 24) | ((uint32_t)p_digit[1] << 16) | ((uint32_t)p_digit[2] << 8) | (uint32_t)p_digit[3];
    }
}

#else

static void vli_nativeToBytes(uint8_t *p_bytes, const uint64_t *p_native)
{
    unsigned i;
    for(i=0; i<uECC_WORDS; ++i)
    {
        uint8_t *p_digit = p_bytes + 8 * (uECC_WORDS - 1 - i);
        p_digit[0] = p_native[i] >> 56;
        p_digit[1] = p_native[i] >> 48;
        p_digit[2] = p_native[i] >> 40;
        p_digit[3] = p_native[i] >> 32;
        p_digit[4] = p_native[i] >> 24;
        p_digit[5] = p_native[i] >> 16;
        p_digit[6] = p_native[i] >> 8;
        p_digit[7] = p_native[i];
    }
}

static void vli_bytesToNative(uint64_t *p_native, const uint8_t *p_bytes)
{
    unsigned i;
    for(i=0; i<uECC_WORDS; ++i)
    {
        const uint8_t *p_digit = p_bytes + 8 * (uECC_WORDS - 1 - i);
        p_native[i] = ((uint64_t)p_digit[0] << 56) | ((uint64_t)p_digit[1] << 48) | ((uint64_t)p_digit[2] << 40) | ((uint64_t)p_digit[3] << 32) |
            ((uint64_t)p_digit[4] << 24) | ((uint64_t)p_digit[5] << 16) | ((uint64_t)p_digit[6] << 8) | (uint64_t)p_digit[7];
    }
}

#endif /* uECC_WORD_SIZE */

int uECC_make_key(uint8_t p_publicKey[uECC_BYTES*2], uint8_t p_privateKey[uECC_BYTES])
{
    EccPoint l_public;
    uECC_word_t l_private[uECC_WORDS];
    uECC_word_t l_tries = 0;
    
    do
    {
    repeat:
        if(!g_rng((uint8_t *)l_private, sizeof(l_private)) || (l_tries++ >= MAX_TRIES))
        {
            return 0;
        }
        if(vli_isZero(l_private))
        {
            goto repeat;
        }
    
        /* Make sure the private key is in the range [1, n-1]. */
    #if uECC_CURVE != uECC_secp160r1
        if(vli_cmp(curve_n, l_private) != 1)
        {
            goto repeat;
        }
    #endif

        EccPoint_mult(&l_public, &curve_G, l_private, 0, vli_numBits(l_private, uECC_WORDS));
    } while(EccPoint_isZero(&l_public));
    
    vli_nativeToBytes(p_privateKey, l_private);
    vli_nativeToBytes(p_publicKey, l_public.x);
    vli_nativeToBytes(p_publicKey + uECC_BYTES, l_public.y);
    return 1;
}

int uECC_shared_secret(const uint8_t p_publicKey[uECC_BYTES*2], const uint8_t p_privateKey[uECC_BYTES], uint8_t p_secret[uECC_BYTES])
{
    EccPoint l_public;
    uECC_word_t l_private[uECC_WORDS];
    uECC_word_t l_random[uECC_WORDS];
    
    g_rng((uint8_t *)l_random, sizeof(l_random));
    
    vli_bytesToNative(l_private, p_privateKey);
    vli_bytesToNative(l_public.x, p_publicKey);
    vli_bytesToNative(l_public.y, p_publicKey + uECC_BYTES);
    
    EccPoint l_product;
    EccPoint_mult(&l_product, &l_public, l_private, (vli_isZero(l_random) ? 0: l_random), vli_numBits(l_private, uECC_WORDS));
    
    vli_nativeToBytes(p_secret, l_product.x);
    
    return !EccPoint_isZero(&l_product);
}

void uECC_compress(const uint8_t p_publicKey[uECC_BYTES*2], uint8_t p_compressed[uECC_BYTES+1])
{
    wordcount_t i;
    for(i=0; i<uECC_BYTES; ++i)
    {
        p_compressed[i+1] = p_publicKey[i];
    }
    p_compressed[0] = 2 + (p_publicKey[uECC_BYTES * 2 - 1] & 0x01);
}

void uECC_decompress(const uint8_t p_compressed[uECC_BYTES+1], uint8_t p_publicKey[uECC_BYTES*2])
{
    EccPoint l_point;
    vli_bytesToNative(l_point.x, p_compressed + 1);
    
#if (uECC_CURVE == uECC_secp256k1)
    vli_modSquare_fast(l_point.y, l_point.x); /* r = x^2 */
    vli_modMult_fast(l_point.y, l_point.y, l_point.x); /* r = x^3 */
    vli_modAdd(l_point.y, l_point.y, curve_b, curve_p); /* r = x^3 + b */
#else
    uECC_word_t _3[uECC_WORDS] = {3}; /* -a = 3 */
    
    vli_modSquare_fast(l_point.y, l_point.x); /* y = x^2 */
    vli_modSub_fast(l_point.y, l_point.y, _3); /* y = x^2 - 3 */
    vli_modMult_fast(l_point.y, l_point.y, l_point.x); /* y = x^3 - 3x */
    vli_modAdd(l_point.y, l_point.y, curve_b, curve_p); /* y = x^3 - 3x + b */
#endif
    
    mod_sqrt(l_point.y);
    
    if((l_point.y[0] & 0x01) != (p_compressed[0] & 0x01))
    {
        vli_sub(l_point.y, curve_p, l_point.y);
    }
    
    vli_nativeToBytes(p_publicKey, l_point.x);
    vli_nativeToBytes(p_publicKey + uECC_BYTES, l_point.y);
}

/* -------- ECDSA code -------- */

#if (uECC_CURVE == uECC_secp160r1)
static void vli_clear_n(uECC_word_t *p_vli)
{
    vli_clear(p_vli);
    p_vli[uECC_N_WORDS - 1] = 0;
}

static uECC_word_t vli_isZero_n(const uECC_word_t *p_vli)
{
    if(p_vli[uECC_N_WORDS - 1])
    {
        return 0;
    }
    return vli_isZero(p_vli);
}

static void vli_set_n(uECC_word_t *p_dest, const uECC_word_t *p_src)
{
    vli_set(p_dest, p_src);
    p_dest[uECC_N_WORDS-1] = p_src[uECC_N_WORDS-1];
}

static cmpresult_t vli_cmp_n(uECC_word_t *p_left, uECC_word_t *p_right)
{
    if(p_left[uECC_N_WORDS-1] > p_right[uECC_N_WORDS-1])
    {
        return 1;
    }
    else if(p_left[uECC_N_WORDS-1] < p_right[uECC_N_WORDS-1])
    {
        return -1;
    }
    return vli_cmp(p_left, p_right);
}

static void vli_rshift1_n(uECC_word_t *p_vli)
{
    vli_rshift1(p_vli);
    p_vli[uECC_N_WORDS-2] |= p_vli[uECC_N_WORDS-1] << (uECC_WORD_BITS - 1);
    p_vli[uECC_N_WORDS-1] = p_vli[uECC_N_WORDS-1] >> 1;
}

static uECC_word_t vli_add_n(uECC_word_t *p_result, uECC_word_t *p_left, uECC_word_t *p_right)
{
    uECC_word_t l_carry = vli_add(p_result, p_left, p_right);
    uECC_word_t l_sum = p_left[uECC_N_WORDS-1] + p_right[uECC_N_WORDS-1] + l_carry;
    if(l_sum != p_left[uECC_N_WORDS-1])
    {
        l_carry = (l_sum < p_left[uECC_N_WORDS-1]);
    }
    p_result[uECC_N_WORDS-1] = l_sum;
    return l_carry;
}

static uECC_word_t vli_sub_n(uECC_word_t *p_result, uECC_word_t *p_left, uECC_word_t *p_right)
{
    uECC_word_t l_borrow = vli_sub(p_result, p_left, p_right);
    uECC_word_t l_diff = p_left[uECC_N_WORDS-1] - p_right[uECC_N_WORDS-1] - l_borrow;
    if(l_diff != p_left[uECC_N_WORDS-1])
    {
        l_borrow = (l_diff > p_left[uECC_N_WORDS-1]);
    }
    p_result[uECC_N_WORDS-1] = l_diff;
    return l_borrow;
}

#if !muladd_exists
static void muladd(uECC_word_t a, uECC_word_t b, uECC_word_t *r0, uECC_word_t *r1, uECC_word_t *r2)
{
    uECC_dword_t p = (uECC_dword_t)a * b;
    uECC_dword_t r01 = ((uECC_dword_t)(*r1) << uECC_WORD_BITS) | *r0;
    r01 += p;
    *r2 += (r01 < p);
    *r1 = r01 >> uECC_WORD_BITS;
    *r0 = (uECC_word_t)r01;
}
#define muladd_exists 1
#endif

static void vli_mult_n(uECC_word_t *p_result, uECC_word_t *p_left, uECC_word_t *p_right)
{
    uECC_word_t r0 = 0;
    uECC_word_t r1 = 0;
    uECC_word_t r2 = 0;
    
    wordcount_t i, k;
    for(k = 0; k < uECC_N_WORDS*2 - 1; ++k)
    {
        wordcount_t l_min = (k < uECC_N_WORDS ? 0 : (k + 1) - uECC_N_WORDS);
        wordcount_t l_max = (k < uECC_N_WORDS ? k : uECC_N_WORDS-1);
        for(i = l_min; i <= l_max; ++i)
        {
            muladd(p_left[i], p_right[k-i], &r0, &r1, &r2);
        }
        p_result[k] = r0;
        r0 = r1;
        r1 = r2;
        r2 = 0;
    }
    
    p_result[uECC_N_WORDS*2 - 1] = r0;
}

static void vli_modAdd_n(uECC_word_t *p_result, uECC_word_t *p_left, uECC_word_t *p_right, uECC_word_t *p_mod)
{
    uECC_word_t l_carry = vli_add_n(p_result, p_left, p_right);
    if(l_carry || vli_cmp_n(p_result, p_mod) >= 0)
    {
        vli_sub_n(p_result, p_result, p_mod);
    }
}

static void vli_modInv_n(uECC_word_t *p_result, uECC_word_t *p_input, uECC_word_t *p_mod)
{
    uECC_word_t a[uECC_N_WORDS], b[uECC_N_WORDS], u[uECC_N_WORDS], v[uECC_N_WORDS];
    uECC_word_t l_carry;
    cmpresult_t l_cmpResult;
    
    if(vli_isZero_n(p_input))
    {
        vli_clear_n(p_result);
        return;
    }

    vli_set_n(a, p_input);
    vli_set_n(b, p_mod);
    vli_clear_n(u);
    u[0] = 1;
    vli_clear_n(v);
    while((l_cmpResult = vli_cmp_n(a, b)) != 0)
    {
        l_carry = 0;
        if(EVEN(a))
        {
            vli_rshift1_n(a);
            if(!EVEN(u)) l_carry = vli_add_n(u, u, p_mod);
            vli_rshift1_n(u);
            if(l_carry) u[uECC_N_WORDS-1] |= HIGH_BIT_SET;
        }
        else if(EVEN(b))
        {
            vli_rshift1_n(b);
            if(!EVEN(v)) l_carry = vli_add_n(v, v, p_mod);
            vli_rshift1_n(v);
            if(l_carry) v[uECC_N_WORDS-1] |= HIGH_BIT_SET;
        }
        else if(l_cmpResult > 0)
        {
            vli_sub_n(a, a, b);
            vli_rshift1_n(a);
            if(vli_cmp_n(u, v) < 0) vli_add_n(u, u, p_mod);
            vli_sub_n(u, u, v);
            if(!EVEN(u)) l_carry = vli_add_n(u, u, p_mod);
            vli_rshift1_n(u);
            if(l_carry) u[uECC_N_WORDS-1] |= HIGH_BIT_SET;
        }
        else
        {
            vli_sub_n(b, b, a);
            vli_rshift1_n(b);
            if(vli_cmp_n(v, u) < 0) vli_add_n(v, v, p_mod);
            vli_sub_n(v, v, u);
            if(!EVEN(v)) l_carry = vli_add_n(v, v, p_mod);
            vli_rshift1_n(v);
            if(l_carry) v[uECC_N_WORDS-1] |= HIGH_BIT_SET;
        }
    }
    
    vli_set_n(p_result, u);
}

static void vli2_rshift1_n(uECC_word_t *p_vli)
{
    vli_rshift1_n(p_vli);
    p_vli[uECC_N_WORDS-1] |= p_vli[uECC_N_WORDS] << (uECC_WORD_BITS - 1);
    vli_rshift1_n(p_vli + uECC_N_WORDS);
}

static uECC_word_t vli2_sub_n(uECC_word_t *p_result, uECC_word_t *p_left, uECC_word_t *p_right)
{
    uECC_word_t l_borrow = 0;
    wordcount_t i;
    for(i=0; i<uECC_N_WORDS*2; ++i)
    {
        uECC_word_t l_diff = p_left[i] - p_right[i] - l_borrow;
        if(l_diff != p_left[i])
        {
            l_borrow = (l_diff > p_left[i]);
        }
        p_result[i] = l_diff;
    }
    return l_borrow;
}

/* Computes p_result = (p_left * p_right) % curve_n. */
static void vli_modMult_n(uECC_word_t *p_result, uECC_word_t *p_left, uECC_word_t *p_right)
{
    uECC_word_t l_product[2 * uECC_N_WORDS];
    uECC_word_t l_modMultiple[2 * uECC_N_WORDS];
    uECC_word_t l_tmp[2 * uECC_N_WORDS];
    uECC_word_t *v[2] = {l_tmp, l_product};
    
    vli_mult_n(l_product, p_left, p_right);
    vli_clear_n(l_modMultiple);
    vli_set(l_modMultiple + uECC_N_WORDS + 1, curve_n);
    vli_rshift1(l_modMultiple + uECC_N_WORDS + 1);
    l_modMultiple[2 * uECC_N_WORDS - 1] |= HIGH_BIT_SET;
    l_modMultiple[uECC_N_WORDS] = HIGH_BIT_SET;
    
    bitcount_t i;
    uECC_word_t l_index = 1;
    for(i=0; i<=((((bitcount_t)uECC_N_WORDS) << uECC_WORD_BITS_SHIFT) + (uECC_WORD_BITS - 1)); ++i)
    {
        uECC_word_t l_borrow = vli2_sub_n(v[1-l_index], v[l_index], l_modMultiple);
        l_index = !(l_index ^ l_borrow); /* Swap the index if there was no borrow */
        vli2_rshift1_n(l_modMultiple);
    }

    vli_set_n(p_result, v[l_index]);
}

#else

#define vli_modInv_n vli_modInv
#define vli_modAdd_n vli_modAdd

static void vli2_rshift1(uECC_word_t *p_vli)
{
    vli_rshift1(p_vli);
    p_vli[uECC_WORDS-1] |= p_vli[uECC_WORDS] << (uECC_WORD_BITS - 1);
    vli_rshift1(p_vli + uECC_WORDS);
}

static uECC_word_t vli2_sub(uECC_word_t *p_result, uECC_word_t *p_left, uECC_word_t *p_right)
{
    uECC_word_t l_borrow = 0;
    wordcount_t i;
    for(i=0; i<uECC_WORDS*2; ++i)
    {
        uECC_word_t l_diff = p_left[i] - p_right[i] - l_borrow;
        if(l_diff != p_left[i])
        {
            l_borrow = (l_diff > p_left[i]);
        }
        p_result[i] = l_diff;
    }
    return l_borrow;
}

/* Computes p_result = (p_left * p_right) % curve_n. */
static void vli_modMult_n(uECC_word_t *p_result, uECC_word_t *p_left, uECC_word_t *p_right)
{
    uECC_word_t l_product[2 * uECC_WORDS];
    uECC_word_t l_modMultiple[2 * uECC_WORDS];
    uECC_word_t l_tmp[2 * uECC_WORDS];
    uECC_word_t *v[2] = {l_tmp, l_product};
    
    vli_mult(l_product, p_left, p_right);
    vli_set(l_modMultiple + uECC_WORDS, curve_n); /* works if curve_n has its highest bit set */
    vli_clear(l_modMultiple);
    
    bitcount_t i;
    uECC_word_t l_index = 1;
    for(i=0; i<=uECC_BYTES * 8; ++i)
    {
        uECC_word_t l_borrow = vli2_sub(v[1-l_index], v[l_index], l_modMultiple);
        l_index = !(l_index ^ l_borrow); /* Swap the index if there was no borrow */
        vli2_rshift1(l_modMultiple);
    }

    vli_set(p_result, v[l_index]);
}
#endif /* (uECC_CURVE != uECC_secp160r1) */

int uECC_sign(const uint8_t p_privateKey[uECC_BYTES], const uint8_t p_hash[uECC_BYTES], uint8_t p_signature[uECC_BYTES*2])
{
    uECC_word_t k[uECC_N_WORDS];
    uECC_word_t l_tmp[uECC_N_WORDS];
    uECC_word_t s[uECC_N_WORDS];
    uECC_word_t *k2[2] = {l_tmp, s};
    EccPoint p;
    uECC_word_t l_tries = 0;
    
    do
    {
    repeat:
        if(!g_rng((uint8_t *)k, sizeof(k)) || (l_tries++ >= MAX_TRIES))
        {
            return 0;
        }
        
        if(vli_isZero(k))
        {
            goto repeat;
        }
        
    #if (uECC_CURVE == uECC_secp160r1)
        k[uECC_WORDS] &= 0x01;
        if(vli_cmp_n(curve_n, k) != 1)
        {
            goto repeat;
        }
        
        /* make sure that we don't leak timing information about k. See http://eprint.iacr.org/2011/232.pdf */
        vli_add_n(l_tmp, k, curve_n);
        uECC_word_t l_carry = (l_tmp[uECC_WORDS] & 0x02);
        vli_add_n(s, l_tmp, curve_n);
    
        /* p = k * G */
        EccPoint_mult(&p, &curve_G, k2[!l_carry], 0, (uECC_BYTES * 8) + 2);
    #else
        if(vli_cmp(curve_n, k) != 1)
        {
            goto repeat;
        }
        
        /* make sure that we don't leak timing information about k. See http://eprint.iacr.org/2011/232.pdf */
        uECC_word_t l_carry = vli_add(l_tmp, k, curve_n);
        vli_add(s, l_tmp, curve_n);
    
        /* p = k * G */
        EccPoint_mult(&p, &curve_G, k2[!l_carry], 0, (uECC_BYTES * 8) + 1);
    
        /* r = x1 (mod n) */
        if(vli_cmp(curve_n, p.x) != 1)
        {
            vli_sub(p.x, p.x, curve_n);
        }
    #endif
    } while(vli_isZero(p.x));
    
    l_tries = 0;
    do
    {
        if(!g_rng((uint8_t *)l_tmp, sizeof(l_tmp)) || (l_tries++ >= MAX_TRIES))
        {
            return 0;
        }
    } while(vli_isZero(l_tmp));
    
    /* Prevent side channel analysis of vli_modInv() to determine
       bits of k / the private key by premultiplying by a random number */
    vli_modMult_n(k, k, l_tmp); /* k' = rand * k */
    vli_modInv_n(k, k, curve_n); /* k = 1 / k' */
    vli_modMult_n(k, k, l_tmp); /* k = 1 / k */
    
    vli_nativeToBytes(p_signature, p.x); /* store r */
    
    l_tmp[uECC_N_WORDS-1] = 0;
    vli_bytesToNative(l_tmp, p_privateKey); /* tmp = d */
    s[uECC_N_WORDS-1] = 0;
    vli_set(s, p.x);
    vli_modMult_n(s, l_tmp, s); /* s = r*d */

    vli_bytesToNative(l_tmp, p_hash);
    vli_modAdd_n(s, l_tmp, s, curve_n); /* s = e + r*d */
    vli_modMult_n(s, s, k); /* s = (e + r*d) / k */
#if (uECC_CURVE == uECC_secp160r1)
    if(s[uECC_N_WORDS-1])
    {
        goto repeat;
    }
#endif
    vli_nativeToBytes(p_signature + uECC_BYTES, s);
    
    return 1;
}

static bitcount_t smax(bitcount_t a, bitcount_t b)
{
    return (a > b ? a : b);
}

int uECC_verify(const uint8_t p_publicKey[uECC_BYTES*2], const uint8_t p_hash[uECC_BYTES], const uint8_t p_signature[uECC_BYTES*2])
{
    uECC_word_t u1[uECC_N_WORDS], u2[uECC_N_WORDS];
    uECC_word_t z[uECC_N_WORDS];
    EccPoint l_public, l_sum;
    uECC_word_t rx[uECC_WORDS];
    uECC_word_t ry[uECC_WORDS];
    uECC_word_t tx[uECC_WORDS];
    uECC_word_t ty[uECC_WORDS];
    uECC_word_t tz[uECC_WORDS];
    
    uECC_word_t r[uECC_N_WORDS], s[uECC_N_WORDS];
    r[uECC_N_WORDS-1] = 0;
    s[uECC_N_WORDS-1] = 0;
    
    vli_bytesToNative(l_public.x, p_publicKey);
    vli_bytesToNative(l_public.y, p_publicKey + uECC_BYTES);
    vli_bytesToNative(r, p_signature);
    vli_bytesToNative(s, p_signature + uECC_BYTES);
    
    if(vli_isZero(r) || vli_isZero(s))
    { /* r, s must not be 0. */
        return 0;
    }
    
#if (uECC_CURVE != uECC_secp160r1)
    if(vli_cmp(curve_n, r) != 1 || vli_cmp(curve_n, s) != 1)
    { /* r, s must be < n. */
        return 0;
    }
#endif

    /* Calculate u1 and u2. */
    vli_modInv_n(z, s, curve_n); /* Z = s^-1 */
    u1[uECC_N_WORDS-1] = 0;
    vli_bytesToNative(u1, p_hash);
    vli_modMult_n(u1, u1, z); /* u1 = e/s */
    vli_modMult_n(u2, r, z); /* u2 = r/s */
    
    /* Calculate l_sum = G + Q. */
    vli_set(l_sum.x, l_public.x);
    vli_set(l_sum.y, l_public.y);
    vli_set(tx, curve_G.x);
    vli_set(ty, curve_G.y);
    vli_modSub_fast(z, l_sum.x, tx); /* Z = x2 - x1 */
    XYcZ_add(tx, ty, l_sum.x, l_sum.y);
    vli_modInv(z, z, curve_p); /* Z = 1/Z */
    apply_z(l_sum.x, l_sum.y, z);
    
    /* Use Shamir's trick to calculate u1*G + u2*Q */
    EccPoint *l_points[4] = {0, &curve_G, &l_public, &l_sum};
    bitcount_t l_numBits = smax(vli_numBits(u1, uECC_N_WORDS), vli_numBits(u2, uECC_N_WORDS));
    
    EccPoint *l_point = l_points[(!!vli_testBit(u1, l_numBits-1)) | ((!!vli_testBit(u2, l_numBits-1)) << 1)];
    vli_set(rx, l_point->x);
    vli_set(ry, l_point->y);
    vli_clear(z);
    z[0] = 1;

    bitcount_t i;
    for(i = l_numBits - 2; i >= 0; --i)
    {
        EccPoint_double_jacobian(rx, ry, z);
        
        uECC_word_t l_index = (!!vli_testBit(u1, i)) | ((!!vli_testBit(u2, i)) << 1);
        l_point = l_points[l_index];
        if(l_point)
        {
            vli_set(tx, l_point->x);
            vli_set(ty, l_point->y);
            apply_z(tx, ty, z);
            vli_modSub_fast(tz, rx, tx); /* Z = x2 - x1 */
            XYcZ_add(tx, ty, rx, ry);
            vli_modMult_fast(z, z, tz);
        }
    }

    vli_modInv(z, z, curve_p); /* Z = 1/Z */
    apply_z(rx, ry, z);
    
    /* v = x1 (mod n) */
#if (uECC_CURVE != uECC_secp160r1)
    if(vli_cmp(curve_n, rx) != 1)
    {
        vli_sub(rx, rx, curve_n);
    }
#endif

    /* Accept only if v == r. */
    return vli_equal(rx, r);
}

int uECC_bytes(void)
{
    return uECC_BYTES;
}

int uECC_curve(void)
{
    return uECC_CURVE;
}
