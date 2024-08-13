/* seed-asm.S */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

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
/**
 * \file	seed-asm.S
 * \author	Daniel Otte 
 * \date	2007-06-1
 * \brief	SEED parts in assembler for AVR
 * \par License	
 * GPLv3 or later
 * 
 */
#include "avr-asm-macros.S"

/******************************************************************************/	
/*
#define M0 0xfc
#define M1 0xf3
#define M2 0xcf
#define M3 0x3f

#define X3 (((uint8_t*)(&x))[0])
#define X2 (((uint8_t*)(&x))[1])
#define X1 (((uint8_t*)(&x))[2])
#define X0 (((uint8_t*)(&x))[3])

#define Z3 (((uint8_t*)(&z))[0])
#define Z2 (((uint8_t*)(&z))[1])
#define Z1 (((uint8_t*)(&z))[2])
#define Z0 (((uint8_t*)(&z))[3])

uint32_t g_function(uint32_t x){
	uint32_t z;
	/ * sbox substitution * /
	X3 = pgm_read_byte(&(seed_sbox2[X3]));
	X2 = pgm_read_byte(&(seed_sbox1[X2]));
	X1 = pgm_read_byte(&(seed_sbox2[X1]));
	X0 = pgm_read_byte(&(seed_sbox1[X0]));
	/ * now the permutation * /
	Z0 = (X0 & M0) ^ (X1 & M1) ^ (X2 & M2) ^ (X3 & M3);
	Z1 = (X0 & M1) ^ (X1 & M2) ^ (X2 & M3) ^ (X3 & M0);
	Z2 = (X0 & M2) ^ (X1 & M3) ^ (X2 & M0) ^ (X3 & M1);
	Z3 = (X0 & M3) ^ (X1 & M0) ^ (X2 & M1) ^ (X3 & M2);
	return z;
}
*/
M0 = 0xfc
M1 = 0xf3
M2 = 0xcf
M3 = 0x3f
X0 = 18
X1 = 19
X2 = 20
X3 = 21
Z0 = 25
Z1 = 24
Z2 = 23
Z3 = 22
T0 = X0
T1 = 26
T2 = 27
T3 = X1
/*
 *  param x: r22:r25
 *  X0 = R25
 *  X1 = R24
 *  X2 = R23
 *  X3 = R22
 */    
seed_g_function:
	ldi r30, lo8(seed_sbox1)
	ldi r31, hi8(seed_sbox1)
 	movw r26, r30
	add r30, Z2
	adc r31, r1
	lpm X2, Z
	movw r30, r26
	add r30, Z0
	adc r31, r1
	lpm X0, Z
	inc r27 /* switch X to point to sbox2 */
	movw r30, r26
	add r30, Z3
	adc r31, r1
	lpm X3, Z
	movw r30, r26
	add r30, Z1
	adc r31, r1
	lpm X1, Z
	/* now the secound part */
	mov Z0, X0
	mov Z1, X0
	mov Z2, X0
	mov Z3, X0
	andi Z0, M0
	andi Z1, M1
	andi Z2, M2
	andi Z3, M3	
	mov T0, X1
	mov T1, X1
	mov T2, X1
	; mov T3, X1 /* T3 = X1 */
	andi T0, M1
	andi T1, M2
	andi T2, M3
	andi T3, M0
	eor Z0, T0
	eor Z1, T1
	eor Z2, T2
	eor Z3, T3
	mov T0, X2
	mov T1, X2
	mov T2, X2
	mov T3, X2
	andi T0, M2
	andi T1, M3
	andi T2, M0
	andi T3, M1
	eor Z0, T0
	eor Z1, T1
	eor Z2, T2
	eor Z3, T3
	mov T0, X3
	mov T1, X3
	mov T2, X3
	mov T3, X3
	andi T0, M3
	andi T1, M0
	andi T2, M1
	andi T3, M2
	eor Z0, T0
	eor Z1, T1
	eor Z2, T2
	eor Z3, T3
	ret

seed_sbox1:
.byte   169,  133,  214,  211,   84,   29,  172,   37 
.byte    93,   67,   24,   30,   81,  252,  202,   99 
.byte    40,   68,   32,  157,  224,  226,  200,   23 
.byte   165,  143,    3,  123,  187,   19,  210,  238 
.byte   112,  140,   63,  168,   50,  221,  246,  116 
.byte   236,  149,   11,   87,   92,   91,  189,    1 
.byte    36,   28,  115,  152,   16,  204,  242,  217 
.byte    44,   231, 114,  131,  155,  209,  134,  201 
.byte    96,   80,  163,  235,   13,  182,  158,   79 
.byte   183,   90,  198,  120,  166,   18,  175,  213 
.byte    97,  195,  180,   65,   82,  125,  141,    8 
.byte    31,  153,    0,   25,    4,   83,  247,  225 
.byte   253,  118,   47,   39,  176,  139,   14,  171 
.byte   162,  110,  147,   77,  105,  124,    9,   10 
.byte   191,  239,  243,  197,  135,   20,  254,  100 
.byte   222,   46,   75,   26,    6,   33,  107,  102 
.byte     2,  245,  146,  138,   12,  179,  126,  208 
.byte   122,   71,  150,  229,   38,  128,  173,  223 
.byte   161,   48,   55,  174,   54,   21,   34,   56 
.byte   244,  167,   69,   76,  129,  233,  132,  151 
.byte    53,  203,  206,   60,  113,   17,  199,  137 
.byte   117,  251,  218,  248,  148,   89,  130,  196 
.byte   255,   73,   57,  103,  192,  207,  215,  184 
.byte    15,  142,   66,   35,  145,  108,  219,  164 
.byte    52,  241,   72,  194,  111,   61,   45,   64 
.byte   190,   62,  188,  193,  170,  186,   78,   85 
.byte    59,  220,  104,  127,  156,  216,   74,   86 
.byte   119,  160,  237,   70,  181,   43,  101,  250 
.byte   227,  185,  177,  159,   94,  249,  230,  178 
.byte    49,  234,  109,   95,  228,  240,  205,  136 
.byte    22,   58,   88,  212,   98,   41,    7,   51 
.byte   232,   27,    5,  121,  144,  106,   42,  154


seed_sbox2:
.byte    56,  232,   45,  166,  207,  222,  179,  184 
.byte   175,   96,   85,  199,   68,  111,  107,   91 
.byte   195,   98,   51,  181,   41,  160,  226,  167 
.byte   211,  145,   17,    6,   28,  188,   54,   75 
.byte   239,  136,  108,  168,   23,  196,   22,  244 
.byte   194,   69,  225,  214,   63,   61,  142,  152 
.byte    40,   78,  246,   62,  165,  249,   13,  223 
.byte   216,   43,  102,  122,   39,   47,  241,  114 
.byte    66,  212,   65,  192,  115,  103,  172,  139 
.byte   247,  173,  128,   31,  202,   44,  170,   52 
.byte   210,   11,  238,  233,   93,  148,   24,  248 
.byte    87,  174,    8,  197,   19,  205,  134,  185 
.byte   255,  125,  193,   49,  245,  138,  106,  177 
.byte   209,   32,  215,    2,   34,    4,  104,  113 
.byte     7,  219,  157,  153,   97,  190,  230,   89 
.byte   221,   81,  144,  220,  154,  163,  171,  208 
.byte   129,   15,   71,   26,  227,  236,  141,  191 
.byte   150,  123,   92,  162,  161,   99,   35,   77 
.byte   200,  158,  156,   58,   12,   46,  186,  110 
.byte   159,   90,  242,  146,  243,   73,  120,  204 
.byte    21,  251,  112,  117,  127,   53,   16,    3 
.byte   100,  109,  198,  116,  213,  180,  234,    9 
.byte   118,   25,  254,   64,   18,  224,  189,    5 
.byte   250,    1,  240,   42,   94,  169,   86,   67 
.byte   133,   20,  137,  155,  176,  229,   72,  121 
.byte   151,  252,   30,  130,   33,  140,   27,   95 
.byte   119,   84,  178,   29,   37,   79,    0,   70 
.byte   237,   88,   82,  235,  126,  218,  201,  253 
.byte    48,  149,  101,   60,  182,  228,  187,  124 
.byte    14,   80,   57,   38,   50,  132,  105,  147 
.byte    55,  231,   36,  164,  203,   83,   10,  135 
.byte   217,   76,  131,  143,  206,   59,   74,  183 

/******************************************************************************/

/*
static
uint64_t f_function(const uint64_t* a, uint32_t k0, uint32_t k1){
	uint32_t c,d;

	c = *a & 0x00000000FFFFFFFFLL;
	d = (*a>>32) & 0x00000000FFFFFFFFLL;
	
	c ^= k0; d ^= k1;
	d ^= c;
	d = g_function(d);
	c = bigendian_sum32(c,d);
	c = g_function(c);
	d = bigendian_sum32(c,d);
	d = g_function(d);
	c = bigendian_sum32(c,d);	
	return ((uint64_t)d << 32) | c;
}
*/
/*
 * param a   r24:r25
 * param k0  r20:r23
 * param k1  r16:r19
 */
D0 = 10
D1 = 11
C0 = 12
C1 = 13
C2 = 14
C3 = 15
D2 = 16
D3 = 17
seed_f_function:
	push_range 10, 17
	movw r30, r24
	ld C0, Z+
	ld C1, Z+
	ld C2, Z+
	ld C3, Z+
	eor C0, r20
	eor C1, r21
	eor C2, r22
	eor C3, r23
	ld r22, Z+
	ld r23, Z+
	ld r24, Z+
	ld r25, Z+
	eor r22, r16
	eor r23, r17
	eor r24, r18
	eor r25, r19
	eor r22, C0
	eor r23, C1
	eor r24, C2
	eor r25, C3
	rcall seed_g_function
	mov D0, r22
	mov D1, r23
	mov D2, r24
	mov D3, r25

	add r25, C3
	adc r24, C2
	adc r23, C1
	adc r22, C0
	rcall seed_g_function
	mov C0, r22
	mov C1, r23
	mov C2, r24
	mov C3, r25

	add r25, D3
	adc r24, D2
	adc r23, D1
	adc r22, D0
	rcall seed_g_function
	mov D0, r22
	mov D1, r23
	mov D2, r24
	mov D3, r25

	add C3, r25
	adc C2, r24
	adc C1, r23
	adc C0, r22

	mov r18, C0
	mov r19, C1
	mov r20, C2
	mov r21, C3
	
	pop_range 10, 17
	ret

/******************************************************************************/
/*
void seed_init(uint8_t * key, seed_ctx_t * ctx){
	memcpy(ctx->k, key, 128/8);
}
*/

.global seed_init
seed_init:
	movw r26, r24
	movw r30, r22
	ldi r22, 16
1:
	ld r0, X+
	st Z+, r0
	dec r22
	brne 1b	
	ret
/******************************************************************************/
/*
typedef struct {
	uint32_t k0, k1;
} keypair_t;

keypair_t getnextkeys(uint32_t *keystate, uint8_t curround){
	keypair_t ret;
	if (curround>15){
		/ * ERROR * /
		ret.k0 = ret.k1 = 0;
	} else {
	/ *	ret.k0 = seed_g_function(keystate[0] + keystate[2] - pgm_read_dword(&(seed_kc[curround])));
		ret.k1 = seed_g_function(keystate[1] - keystate[3] + pgm_read_dword(&(seed_kc[curround]))); * /
		ret.k0 = bigendian_sum32(keystate[0], keystate[2]);
		ret.k0 = bigendian_sub32(ret.k0, pgm_read_dword(&(seed_kc[curround])));
		ret.k0 = seed_g_function(ret.k0);
		ret.k1 = bigendian_sub32(keystate[1], keystate[3]);
		ret.k1 = bigendian_sum32(ret.k1, pgm_read_dword(&(seed_kc[curround])));
		ret.k1 = seed_g_function(ret.k1);
		
		if (curround & 1){
			/ * odd round (1,3,5, ...) * /
			((uint64_t*)keystate)[1] = bigendian_rotl8_64( ((uint64_t*)keystate)[1] );
		} else {
			/ * even round (0,2,4, ...) * /
			((uint64_t*)keystate)[0] = bigendian_rotr8_64(((uint64_t*)keystate)[0]);
		}
	}
	return ret;
}
*/
/*
 * param keystate: r24:r25
 * param curround: r22
 */
XRC0 = 10
XRC1 = 11
XRC2 = 12
XRC3 = 13
D0 = 14
D1 = 15
D2 = 16
D3 = 17

compute_keys:
	ldi r30, lo8(seed_kc)
	ldi r31, hi8(seed_kc)
	lsl r22
	lsl r22
	add r30, r22
	adc r31, r1
	lpm XRC0, Z+
	lpm XRC1, Z+
	lpm XRC2, Z+
	lpm XRC3, Z+
	movw r28, r24
	ldd r25, Y+0*4+3
	ldd r24, Y+0*4+2
	ldd r23, Y+0*4+1
	ldd r22, Y+0*4+0
	
	ldd r0, Y+2*4+3
	add r25, r0
	ldd r0, Y+2*4+2
	adc r24, r0
	ldd r0, Y+2*4+1
	adc r23, r0
	ldd r0, Y+2*4+0
	adc r22, r0

	sub r25, XRC3
	sbc r24, XRC2
	sbc r23, XRC1
	sbc r22, XRC0
	rcall seed_g_function
	mov D0, r22
	mov D1, r23
	mov D2, r24
	mov D3, r25
	

	ldd r25, Y+1*4+3
	ldd r24, Y+1*4+2
	ldd r23, Y+1*4+1
	ldd r22, Y+1*4+0

	ldd r0, Y+3*4+3
	sub r25, r0
	ldd r0, Y+3*4+2
	sbc r24, r0
	ldd r0, Y+3*4+1
	sbc r23, r0
	ldd r0, Y+3*4+0
	sbc r22, r0

	add r25, XRC3
	adc r24, XRC2
	adc r23, XRC1
	adc r22, XRC0
	rcall seed_g_function

	mov r21, D3
	mov r20, D2
	mov r19, D1
	mov r18, D0 
	ret

seed_getnextkeys:
	push_range 10, 17
	push r28
	push r29
;	andi r22, 0x0F
	bst r22,0
	rcall compute_keys		
	brtc even_round
odd_round:

	adiw r28, 8
	ld r26, Y
	ldd r0, Y+1
	std Y+0, r0
	ldd r0, Y+2
	std Y+1, r0
	ldd r0, Y+3
	std Y+2, r0
	ldd r0, Y+4
	std Y+3, r0
	ldd r0, Y+5
	std Y+4, r0
	ldd r0, Y+6
	std Y+5, r0
	ldd r0, Y+7
	std Y+6, r0
	std Y+7, r26	
/*
	movw r30, r28
	ld r26, Z+
	ldi r27, 7
1:
	ld r0, Z+
	st Y+, r0
	dec r27
	brne 1b
	st Y, r26
*/	
	rjmp 4f

even_round:

	ldd r26, Y+7
	ldd r0, Y+6
	std Y+7, r0
	ldd r0, Y+5
	std Y+6, r0
	ldd r0, Y+4
	std Y+5, r0
	ldd r0, Y+3
	std Y+4, r0
	ldd r0, Y+2
	std Y+3, r0
	ldd r0, Y+1
	std Y+2, r0
	ldd r0, Y+0
	std Y+1, r0
	std Y+0, r26
/*
	adiw r28, 7	
	ld r26, Y
	ldi r27, 7	
1:
	ld r0, -Y
	std Y+1, r0
	dec r27
	brne 1b
	st Y, r26
*/
4:	
	pop r29
	pop r28
	pop_range 10, 17
	ret

/******************************************************************************/
/*
keypair_t getprevkeys(uint32_t *keystate, uint8_t curround){
	keypair_t ret;
	if (curround>15){
		/ * ERROR * /
		ret.k0 = ret.k1 = 0;
	} else {
		if (curround & 1){
			/ * odd round (1,3,5, ..., 15) * /
			((uint64_t*)keystate)[1] = bigendian_rotr8_64( ((uint64_t*)keystate)[1] );
		} else {
			/ * even round (0,2,4, ..., 14) * /
			((uint64_t*)keystate)[0] = bigendian_rotl8_64(((uint64_t*)keystate)[0]);
		}
	/ *	ret.k0 = seed_g_function(keystate[0] + keystate[2] - pgm_read_dword(&(seed_kc[curround])));
		ret.k1 = seed_g_function(keystate[1] - keystate[3] + pgm_read_dword(&(seed_kc[curround]))); * /
		ret.k0 = bigendian_sum32(keystate[0], keystate[2]);
		ret.k0 = bigendian_sub32(ret.k0, pgm_read_dword(&(seed_kc[curround])));
		ret.k0 = seed_g_function(ret.k0);
		ret.k1 = bigendian_sub32(keystate[1], keystate[3]);
		ret.k1 = bigendian_sum32(ret.k1, pgm_read_dword(&(seed_kc[curround])));
		ret.k1 = seed_g_function(ret.k1);
		}
	return ret;
}
*/
/*
 * param keystate: r24:r25
 * param curround: r22
 */

seed_getprevkeys:
	push_range 10, 17
	push r28
	push r29
	movw r28, r24	
;	andi r22, 0x0F
	bst r22, 0
	brts r_odd_round
r_even_round:
	ldd r26, Y+0
	ldd r0, Y+1
	std Y+0, r0
	ldd r0, Y+2
	std Y+1, r0
	ldd r0, Y+3
	std Y+2, r0
	ldd r0, Y+4
	std Y+3, r0
	ldd r0, Y+5
	std Y+4, r0
	ldd r0, Y+6
	std Y+5, r0
	ldd r0, Y+7
	std Y+6, r0
	std Y+7, r26	
/*
	movw r30, r28
	ld r26, Z+
	ldi r27, 7
1:
	ld r0, Z+
	st Y+, r0
	dec r27
	brne 1b
	st Y, r26
*/	

	rjmp 4f
r_odd_round:
	ldd r26, Y+8+7
	ldd r0, Y+8+6
	std Y+8+7, r0
	ldd r0, Y+8+5
	std Y+8+6, r0
	ldd r0, Y+8+4
	std Y+8+5, r0
	ldd r0, Y+8+3
	std Y+8+4, r0
	ldd r0, Y+8+2
	std Y+8+3, r0
	ldd r0, Y+8+1
	std Y+8+2, r0
	ldd r0, Y+8+0
	std Y+8+1, r0
	std Y+8+0, r26
/*
	adiw r28, 7	
	ld r26, Y
	ldi r27, 7	
1:
	ld r0, -Y
	std Y+1, r0
	dec r27
	brne 1b
	st Y, r26
*/
4:
	rcall compute_keys	

	pop r29
	pop r28
	pop_range 10, 17
	ret

/******************************************************************************/

seed_kc:
.long   0xb979379e 
.long   0x73f36e3c
.long   0xe6e6dd78 
.long   0xcccdbbf1 
.long   0x999b77e3 
.long   0x3337efc6 
.long   0x676ede8d 
.long   0xcfdcbc1b 
.long   0x9eb97937
.long   0x3c73f36e	
.long   0x78e6e6dd
.long   0xf1cccdbb
.long   0xe3999b77
.long   0xc63337ef
.long   0x8d676ede
.long   0x1bcfdcbc

/******************************************************************************/
/*
#define L (((uint64_t*)buffer)[0])
#define R (((uint64_t*)buffer)[1])

void seed_enc(void * buffer, seed_ctx_t * ctx){
	uint8_t r;
	keypair_t k;
	for(r=0; r<8; ++r){
			k = seed_getnextkeys(ctx->k, 2*r);
/ *
	DEBUG_S("\r\n\tDBG ka,0: "); uart_hexdump(&k.k0, 4);
	DEBUG_S("\r\n\tDBG ka,1: "); uart_hexdump(&k.k1, 4);
	DEBUG_S("\r\n\t DBG L:   "); uart_hexdump((uint8_t*)buffer+0, 8);
	DEBUG_S("\r\n\t DBG R:   "); uart_hexdump((uint8_t*)buffer+8, 8);
* /
			L ^= seed_f_function(&R,k.k0,k.k1);
			
			k = seed_getnextkeys(ctx->k, 2*r+1);
/ *
	DEBUG_S("\r\n\tDBG kb,0: "); uart_hexdump(&k.k0, 4);
	DEBUG_S("\r\n\tDBG kb,1: "); uart_hexdump(&k.k1, 4);
	DEBUG_S("\r\n\t DBG L:   "); uart_hexdump((uint8_t*)buffer+8, 8);
	DEBUG_S("\r\n\t DBG R:   "); uart_hexdump((uint8_t*)buffer+0, 8);
* /
			R ^= seed_f_function(&L,k.k0,k.k1);
	}
	/ * just an exchange without temp. variable * /
	L ^= R;
	R ^= L;
	L ^= R;
}
*/
/*
 * param buffer: r24:r25
 * param ctx:    r22:r23
 */
CTR  = 9
xLPTR = 10
xRPTR = 12
CPTR = 14

.global seed_enc
seed_enc:
	push_range 9, 17
	push r28
	push r29	
	clr CTR
	movw xLPTR, r24
	adiw r24, 8
	movw xRPTR, r24		
	movw CPTR, r22
1:
	movw r28, xLPTR
	movw r24, CPTR
	mov r22, CTR
	lsl r22
	rcall seed_getnextkeys	

	/* use pen & paper to understand the following permutation */
	movw r16, r22
	movw r22, r18
	movw r18, r24
	movw r24, r20
	movw r20, r22
	movw r22, r24
	movw r24, xRPTR	

	rcall seed_f_function

	ld r0, Y
	eor r0, r18
	st Y+, r0
	ld r0, Y
	eor r0, r19
	st Y+, r0
	ld r0, Y
	eor r0, r20
	st Y+, r0
	ld r0, Y
	eor r0, r21
	st Y+, r0
	ld r0, Y
	eor r0, r22
	st Y+, r0
	ld r0, Y
	eor r0, r23
	st Y+, r0
	ld r0, Y
	eor r0, r24
	st Y+, r0
	ld r0, Y
	eor r0, r25
	st Y+, r0
	/* secound half */
	movw r24, CPTR
	mov r22, CTR
	lsl r22
	inc r22
	rcall seed_getnextkeys	

	movw r16, r22
	movw r22, r18
	movw r18, r24
	movw r24, r20
	movw r20, r22
	movw r22, r24
	movw r24, xLPTR	
	
	rcall seed_f_function

	ld r0, Y
	eor r0, r18
	st Y+, r0
	ld r0, Y
	eor r0, r19
	st Y+, r0
	ld r0, Y
	eor r0, r20
	st Y+, r0
	ld r0, Y
	eor r0, r21
	st Y+, r0
	ld r0, Y
	eor r0, r22
	st Y+, r0
	ld r0, Y
	eor r0, r23
	st Y+, r0
	ld r0, Y
	eor r0, r24
	st Y+, r0
	ld r0, Y
	eor r0, r25
	st Y+, r0
	
	inc CTR
	bst CTR, 3
	brts 3f
	rjmp 1b
3:
	movw r28, xLPTR
	movw r30, xRPTR
	ldi r17, 8
4:
	ld r10, Y
	ld r11, Z
	st Z+, r10
	st Y+, r11
	dec r17
	brne 4b
5:
	pop r29
	pop r28
	pop_range 9, 17
	ret

/******************************************************************************/
/*
#define L (((uint64_t*)buffer)[0])
#define R (((uint64_t*)buffer)[1])

void seed_dec(void * buffer, seed_ctx_t * ctx){
	int8_t r;
	keypair_t k;
	for(r=7; r>=0; --r){
			k = seed_getprevkeys(ctx->k, 2*r+1);
/ *
	DEBUG_S("\r\n\tDBG ka,0: "); uart_hexdump(&k.k0, 4);
	DEBUG_S("\r\n\tDBG ka,1: "); uart_hexdump(&k.k1, 4);
	DEBUG_S("\r\n\t DBG L: "); uart_hexdump((uint8_t*)buffer+0, 8);
	DEBUG_S("\r\n\t DBG R: "); uart_hexdump((uint8_t*)buffer+8, 8);
* /
			L ^= seed_f_function(&R,k.k0,k.k1);
			
			k = seed_getprevkeys(ctx->k, 2*r+0);
/ *
	DEBUG_S("\r\n\tDBG kb,0: "); uart_hexdump(&k.k0, 4);
	DEBUG_S("\r\n\tDBG kb,1: "); uart_hexdump(&k.k1, 4);
	DEBUG_S("\r\n\t DBG L: "); uart_hexdump((uint8_t*)buffer+8, 8);
	DEBUG_S("\r\n\t DBG R: "); uart_hexdump((uint8_t*)buffer+0, 8);
* /
			R ^= seed_f_function(&L,k.k0,k.k1);
	}
	/ * just an exchange without temp. variable * /
	L ^= R;
	R ^= L;
	L ^= R;
}
*/
/*
 * param buffer: r24:r25
 * param ctx:    r22:r23
 */
CTR  = 9
xLPTR = 10
xRPTR = 12
CPTR = 14

.global seed_dec
seed_dec:
	push_range 9, 17
	push r28
	push r29	
	ldi r16, 7
	mov CTR, r16
	movw xLPTR, r24
	adiw r24, 8
	movw xRPTR, r24		
	movw CPTR, r22
1:
	movw r28, xLPTR
	movw r24, CPTR
	mov r22, CTR
	lsl r22
	inc r22
	rcall seed_getprevkeys	

	/* use pen & paper to understand the following permutation */
	movw r16, r22
	movw r22, r18
	movw r18, r24
	movw r24, r20
	movw r20, r22
	movw r22, r24
	movw r24, xRPTR	

	rcall seed_f_function

	ld r0, Y
	eor r0, r18
	st Y+, r0
	ld r0, Y
	eor r0, r19
	st Y+, r0
	ld r0, Y
	eor r0, r20
	st Y+, r0
	ld r0, Y
	eor r0, r21
	st Y+, r0
	ld r0, Y
	eor r0, r22
	st Y+, r0
	ld r0, Y
	eor r0, r23
	st Y+, r0
	ld r0, Y
	eor r0, r24
	st Y+, r0
	ld r0, Y
	eor r0, r25
	st Y+, r0
	/* secound half */
	movw r24, CPTR
	mov r22, CTR
	lsl r22
	rcall seed_getprevkeys	

	movw r16, r22
	movw r22, r18
	movw r18, r24
	movw r24, r20
	movw r20, r22
	movw r22, r24
	movw r24, xLPTR	
	
	rcall seed_f_function

	ld r0, Y
	eor r0, r18
	st Y+, r0
	ld r0, Y
	eor r0, r19
	st Y+, r0
	ld r0, Y
	eor r0, r20
	st Y+, r0
	ld r0, Y
	eor r0, r21
	st Y+, r0
	ld r0, Y
	eor r0, r22
	st Y+, r0
	ld r0, Y
	eor r0, r23
	st Y+, r0
	ld r0, Y
	eor r0, r24
	st Y+, r0
	ld r0, Y
	eor r0, r25
	st Y+, r0
	
	dec CTR
	brmi 3f
	rjmp 1b
3:
	movw r28, xLPTR
	movw r30, xRPTR
	ldi r17, 8
4:
	ld r10, Y
	ld r11, Z
	st Z+, r10
	st Y+, r11
	dec r17
	brne 4b
5:
	pop r29
	pop r28
	pop_range 9, 17
	ret

