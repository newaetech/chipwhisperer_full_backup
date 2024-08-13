/* camellia-asm.S */
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
/* 
 * File:        camellis-asm.S
 * Author:      Daniel Otte
 * Date:        2006-11-10
 * License:     GPLv3 or later
 * Description: Implementation of the camellia block cipher algorithm.
 * 
 */
 
.macro SWAP_R A, B
	eor \A, \B
	eor \B, \A
	eor \A, \B
.endm	 
 
.macro precall
	/* push r18 - r27, r30 - r31*/
	push r0
	push r1
	push r18
	push r19
	push r20
	push r21
	push r22
	push r23
	push r24
	push r25
	push r26
	push r27
	push r30
	push r31
	clr r1
.endm

.macro postcall
	pop r31
	pop r30
	pop r27
	pop r26
	pop r25
	pop r24
	pop r23
	pop r22
	pop r21
	pop r20
	pop r19
	pop r18
	pop r1
	pop r0
.endm


.macro hexdump length
	push r27
	push r26
	ldi r25, '\r'
	mov r24, r25
	call uart_putc
	ldi r25, '\n'
	mov r24, r25
	call uart_putc
	pop r26
	pop r27
	movw r24, r26
.if \length > 16
	ldi r22, lo8(16)
	ldi r23, hi8(16)
	push r27
	push r26
	call uart_hexdump
	pop r26
	pop r27
	adiw r26, 16
	hexdump \length-16
.else
	ldi r22, lo8(\length)
	ldi r23, hi8(\length)
	call uart_hexdump
.endif
.endm

/* X points to Block */
.macro dbg_hexdump length
	precall
	hexdump \length
	postcall
.endm
 
SPL = 0x3D
SPH = 0x3E
SREG = 0x3F
NULLr = 1
 
 
camellia_sbox:
.byte	112, 130,  44, 236, 179,  39, 192, 229, 228, 133,  87,  53, 234,  12, 174,  65
.byte	 35, 239, 107, 147,  69,  25, 165,  33, 237,  14,  79,  78,  29, 101, 146, 189
.byte	134, 184, 175, 143, 124, 235,  31, 206,  62,  48, 220,  95,  94, 197,  11,  26
.byte	166, 225,  57, 202, 213,  71,  93,  61, 217,   1,  90, 214,  81,  86, 108,  77
.byte	139,  13, 154, 102, 251, 204, 176,  45, 116,  18,  43,  32, 240, 177, 132, 153
.byte	223,  76, 203, 194,  52, 126, 118,   5, 109, 183, 169,  49, 209,  23,   4, 215
.byte	 20,  88,  58,  97, 222,  27,  17,  28,  50,  15, 156,  22,  83,  24, 242,  34
.byte	254,  68, 207, 178, 195, 181, 122, 145,  36,   8, 232, 168,  96, 252, 105,  80
.byte	170, 208, 160, 125, 161, 137,  98, 151,  84,  91,  30, 149, 224, 255, 100, 210
.byte	 16, 196,   0,  72, 163, 247, 117, 219, 138,   3, 230, 218,   9,  63, 221, 148
.byte	135,  92, 131,   2, 205,  74, 144,  51, 115, 103, 246, 243, 157, 127, 191, 226
.byte	 82, 155, 216,  38, 200,  55, 198,  59, 129, 150, 111,  75,  19, 190,  99,  46
.byte	233, 121, 167, 140, 159, 110, 188, 142,  41, 245, 249, 182,  47, 253, 180,  89
.byte	120, 152,   6, 106, 231,  70, 113, 186, 212,  37, 171,  66, 136, 162, 141, 250
.byte	114,   7, 185,  85, 248, 238, 172,  10,  54,  73,  42, 104,  60,  56, 241, 164
.byte	 64,  40, 211, 123, 187, 201,  67, 193,  21, 227, 173, 244, 119, 199, 128, 158

//.global camellia_sigma
/*
camellia_sigma:
.quad	0xA09E667F3BCC908B
.quad	0xB67AE8584CAA73B2
.quad	0xC6EF372FE94F82BE
.quad	0x54FF53A5F1D36F1C
.quad	0x10E527FADE682D1D
.quad	0xB05688C2B3E6C1FD	
*/



/* uint8_t camellia_s1(uint8_t b) */
.global camellia_s1
camellia_s1:
	ldi r30, lo8(camellia_sbox)
	ldi r31, hi8(camellia_sbox)
	add r30, r24
	adc r31, NULLr
	lpm r24, Z
	clr r25
	ret

.global camellia_s2
camellia_s2:
	ldi r30, lo8(camellia_sbox)
	ldi r31, hi8(camellia_sbox)
	add r30, r24
	adc r31, NULLr
	lpm r24, Z
	lsl r24
	adc r24, NULLr
	clr r25
	ret

.global camellia_s3
camellia_s3:
	ldi r30, lo8(camellia_sbox)
	ldi r31, hi8(camellia_sbox)
	add r30, r24
	adc r31, NULLr
	lpm r24, Z
	bst r24, 0
	lsr r24
	bld r24, 7
	clr r25
	ret

.global camellia_s4
camellia_s4:
	ldi r30, lo8(camellia_sbox)
	ldi r31, hi8(camellia_sbox)
	lsl r24
	adc r24, NULLr
	add r30, r24
	adc r31, NULLr
	lpm r24, Z
	clr r25
	ret

.global camellia_s
/* uint64_t camellia_s(uint64_t d){
	#define D ((uint8_t*)(&d))
	D[7] = camellia_s1(D[7]); // MSB
	D[6] = camellia_s2(D[6]);
	D[5] = camellia_s3(D[5]);
	D[4] = camellia_s4(D[4]);
	
	D[3] = camellia_s2(D[3]);
	D[2] = camellia_s3(D[2]);
	D[1] = camellia_s4(D[1]);
	D[0] = camellia_s1(D[0]); // LSB
	#undef D
	return d;
}*/
; parameters
; d: r18-r25 (r18 is LSB)
camellia_s:
	movw r26, r24	; backup r24,r25 -> X
	clr r25
	rcall camellia_s2
	mov r26, r24

	mov r24, r27
	rcall camellia_s1
	mov r27, r24
	 
	mov r24, r23
	rcall camellia_s3
	mov r23, r24
	
	mov r24, r22
	rcall camellia_s4
	mov r22, r24

	mov r24, r21
	rcall camellia_s2
	mov r21, r24
	
	mov r24, r20
	rcall camellia_s3
	mov r20, r24
	
	mov r24, r19
	rcall camellia_s4
	mov r19, r24


	mov r24, r18
	rcall camellia_s1
	mov r18, r24
		
	movw r24, r26
	ret
	
;##############################################################################
/* uint64_t camellia_p(uint64_t d) */
; param: r18-r25 (r18 is LSB)
z1 = 25
z2 = 24
z3 = 23
z4 = 22
z5 = 21
z6 = 20
z7 = 19
z8 = 18

.global camellia_p
camellia_p:
	eor z1, z6
	eor z2, z7
	eor z3, z8
	eor z4, z5
	eor z5, z3
	eor z6, z4
	eor z7, z1
	eor z8, z2
    ;---------
	eor z1, z8
	eor z2, z5
	eor z3, z6
	eor z4, z7
	eor z5, z4
	eor z6, z1
	eor z7, z2
	eor z8, z3
    ;---------
	movw r26, z8
	movw r30, z6 ; backup z5 bis z8
	movw z8, z4
	movw z6, z2
	movw z4, r26
	movw z2, r30
	ret
	

;##############################################################################

/* uint64_t camellia_f(uint64_t x, uint64_t k) */	
; param x: r18-r25
; param k: r10-r17
.global camellia_f
camellia_f:
	eor r18, r10
	eor r19, r11
	eor r20, r12
	eor r21, r13
	eor r22, r14
	eor r23, r15
	eor r24, r16
	eor r25, r17
	rcall camellia_s
	rcall camellia_p
	ret
	
;##############################################################################

/* uint64_t camellia_fl(uint64_t x, uint64_t k) */
; param x: r18-r25		xl: r22-r25, xr: r18-r21
; param k: r10-r17		kl: r14-r17, kr: r10-r13
kl1 = 14
kl2 = 15
kl3 = 16
kl4 = 17
kr1 = 10
kr2 = 11
kr3 = 12
kr4 = 13
xr1 = 18
xr2 = 19
xr3 = 20
xr4 = 21
xl1 = 22
xl2 = 23
xl3 = 24
xl4 = 25
.global camellia_fl
camellia_fl:
	and kl1, xl1
	and kl2, xl2
	and kl3, xl3
	and kl4, xl4
	mov r26, kl4
	rol r26
	rol kl1
	rol kl2
	rol kl3
	rol kl4
	eor xr1, kl1
	eor xr2, kl2
	eor xr3, kl3
	eor xr4, kl4
	// that was part one
	or kr1, xr1
	or kr2, xr2
	or kr3, xr3
	or kr4, xr4	
	eor xl1, kr1
	eor xl2, kr2
	eor xl3, kr3
	eor xl4, kr4
	ret
	
;##############################################################################

/* uint64_t camellia_fl_inv(uint64_t y, uint64_t k) */
; param y: r18-r25		yl: r22-r25, yr: r18-r21
; param k: r10-r17		kl: r14-r17, kr: r10-r13
kl1 = 14
kl2 = 15
kl3 = 16
kl4 = 17
kr1 = 10
kr2 = 11
kr3 = 12
kr4 = 13
yr1 = 18
yr2 = 19
yr3 = 20
yr4 = 21
yl1 = 22
yl2 = 23
yl3 = 24
yl4 = 25
.global camellia_fl_inv
camellia_fl_inv:
	or kr1, yr1
	or kr2, yr2
	or kr3, yr3
	or kr4, yr4
	eor yl1, kr1
	eor yl2, kr2
	eor yl3, kr3
	eor yl4, kr4
	// the first one is done
	and kl1, yl1
	and kl2, yl2
	and kl3, yl3
	and kl4, yl4	
	mov r26, kl4
	rol r26
	rol kl1
	rol kl2
	rol kl3
	rol kl4
	eor yr1, kl1
	eor yr2, kl2
	eor yr3, kl3
	eor yr4, kl4
	ret

;##############################################################################
; param s: r24-r25
; param q: r22
B1 = 18
B2 = 19
.global camellia128_keyop_rot15
camellia128_keyop_rot15:
	movw r30, r24 ; Z points at LSB of kl            ;-- 0
	ldi r22, 2		
2:	adiw r30, 15                                     ;-- 15
	ld  r21, Z
	ld  r20, -Z                                      ;-- 14
	movw B1, r20 ; store Backup of the 2 MSB of kl
	ror r20

	ldi r21, 14
1:	ld r20, -Z                                       ;-- 13..0
	ror r20
	std Z+2, r20                                     ;-- (15..2)
	dec r21
	brne 1b
	
	ror B2
	ror B1
	st Z+, B1                                        ;-- 1
	st Z, B2
	adiw r30, 15                                     ;-- 16
	
	dec r22
	brne 2b
	ret

;##############################################################################
; param s: r24-r25
; param q: r22
.global camellia128_keyop_rot17
camellia128_keyop_rot17:
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	clt
	movw r30, r24
	clr r27
2:	ldi r26, 8
	mov r1, r26
	lsl r1	; r1=16
	;push r1
	; load 128bit value
	ldd r0, Z+15
	rol r0
1:	ld r0, Z+
	rol r0
	st X+, r0
	dec r1
	brne 1b
	
	st -Z, 21
	st -Z, 20
	st -Z, 19
	st -Z, 18
	st -Z, 17
	st -Z, 16
	st -Z, 15
	st -Z, 14 ;--
	st -Z, 13
	st -Z, 12
	st -Z, 11
	st -Z, 10
	st -Z, 9
	st -Z, 8
	st -Z, 23
	st -Z, 22
	
	brts 2f
	set
	adiw r30, 16
	rjmp 2b
2:	
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8 
	ret

;##############################################################################
; param s: r24-r25
; param q: r22
.global camellia128_keyop
camellia128_keyop:
	cpi r22, 1
	breq camellia128_keyop_rot17
	rjmp camellia128_keyop_rot15
	
;##############################################################################
; param s: r24-r25
; param q: r22
B1 = 18
B2 = 19
.global camellia128_keyop_inv_rot15
camellia128_keyop_inv_rot15:
	movw r30, r24 ; Z points at LSB of kl                ;-- 0
	movw r26, r24 ; X also
	ldi r22, 2		
2:                                                           ;-- 0
	ld  r20, Z+                                          ;-- 0/1
	ld  r21, Z+                                          ;-- 1/2
	movw B1, r20 ; store Backup of the 2 LSB of kl
	rol r21

	ldi r20, 14
1:	ld r21, Z+                                           ;-- 2/14..3/16
	rol r21
	st X+, r21                                           ;-- (0..13)/(1..14)
	dec r20
	brne 1b
	
	rol B1
	rol B2
	st X+, B1                                            ;-- 14/15
	st X+, B2                                            ;-- 15/16
	
	dec r22
	brne 2b
	ret
	
;##############################################################################
; param s: r24-r25
; param q: r22
.global camellia128_keyop_inv_rot17
camellia128_keyop_inv_rot17:
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	clt
	movw r30, r24
	clr r27
2:	ldi r26, 8
	mov r1, r26
	lsl r1	; r1=16
	; load 128bit value
	
	ld  r0, Z
	adiw r30, 16
	ror r0
1:	ld r0, -Z
	ror r0
	st X+, r0
	dec r1
	brne 1b
	
	st Z+, 21
	st Z+, 20
	st Z+, 19
	st Z+, 18
	st Z+, 17
	st Z+, 16
	st Z+, 15
	st Z+, 14 ;--
	st Z+, 13
	st Z+, 12
	st Z+, 11
	st Z+, 10
	st Z+, 9
	st Z+, 8
	st Z+, 23
	st Z+, 22
	
	brts 2f
	set
;	adiw r30, 16
	rjmp 2b
2:	
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8 
	ret

;##############################################################################
; param s: r24-r25
; param q: r22
.global camellia128_keyop_inv
camellia128_keyop_inv:
	cpi r22, 1
	breq camellia128_keyop_inv_rot17
	rjmp camellia128_keyop_inv_rot15
	
;##############################################################################
; param p: r24-r25	pointer to data
; param l: r22		length of word
.global change_endian
change_endian:
	movw r26, r24
	movw r30, r24
	add r30, r22
	adc	r31, r1
	lsr r22
1:
	ld r20,  X
	ld r21, -Z	
	st X+, r21
	st Z,  r20
	dec r22
	brne 1b
	ret

;##############################################################################
	
#define SEL_KA 1
#define SEL_KL 0
#define KEY_POSTC1		0x00
#define KEY_POSTC2		0x01
#define KEY_INC2		0x02
#define KEY_DIR			0x04
#define KEY_DIR_NORM	0x00
#define KEY_DIR_INV		0x04
#define KEY_AMMOUNT		0x08 
#define KEY_ROL17		0x08
#define KEY_ROL15		0x00
/*
void camellia_6rounds(camellia128_ctx_t* s, uint64_t* bl, uint64_t* br, uint8_t roundop, uint8_t keychoice){
	uint8_t i;
	uint64_t* k[4];
	k[0] = &(s->kll);
	k[1] = &(s->klr);
	k[2] = &(s->kal);
	k[3] = &(s->kar);
	for(i=0; i<3; ++i){ / * each cycle * /
		br[0] ^= camellia_f(bl[0],*(k[(keychoice&1)*2+((roundop&KEY_DIR)?1:0)]));
		keychoice >>= 1;
		
		if((i == 1) && (roundop&KEY_INC2)){
			((roundop&KEY_DIR)?camellia128_keyop_inv:camellia128_keyop)(s,(roundop&KEY_AMMOUNT)?1:-1);
		}
		
		bl[0] ^= camellia_f(br[0],*(k[(keychoice&1)*2+((roundop&KEY_DIR)?0:1)]));
		keychoice >>= 1;
		
		/ * check if we should do some keyop * /
		if((i == (roundop&1)) && (!(roundop&KEY_INC2)) ){
			((roundop&KEY_DIR)?camellia128_keyop_inv:camellia128_keyop)(s,(roundop&KEY_AMMOUNT)?1:-1);
			/ * isn't it fuckin nice what we can do in C?! * /
		}
	}
}
*/
; param s:  r24-r25
; param bl: r22-r23
; param br: r20-r21
; param roundop: r18
; param keychoice: r16	
s1 = 24
s2 = 25
bl1 = 22
bl2 = 23
br1 = 20
br2 = 22
xro = 18
kc = 16	
xro_sec = 17
br1_sec = 10
br2_sec = 11
bl1_sec = 12
bl2_sec = 13
s1_sec = 14
t = 9
loop_cnt = 8
keyop_time = 7

.global camellia_6rounds
camellia_6rounds:
	push r17
	push r16
	push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	push r7
	
	ldi r17, 6
	mov loop_cnt, r17
	mov xro_sec,  xro
	movw br1_sec, br1
	movw bl1_sec, bl1
	movw s1_sec, s1
	clr keyop_time
	inc keyop_time
	sec
	rol keyop_time // keyop_time == 3
	SBRC xro, 1 // KEY_INC2
	 rjmp 1f
	SBRS xro, 0 // KEY_POSTC1
	 inc keyop_time
	SBRS xro, 0 // KEY_POSTC1
	 inc keyop_time
	rjmp 2f
1:	inc keyop_time
2:
main_loop:
	/* now we load the key to r18-r25 */
	movw r26, s1_sec
	SBRC kc, 0		/* select between KA and KL */
	 adiw r26, 16
	SBRC xro_sec, 2 // KEY_DIR
	 rjmp 2f
	SBRS loop_cnt, 0 /* enc */
	 adiw r26,  8
	rjmp 3f 
2:	SBRC loop_cnt, 0  /* dec */
	 adiw r26,  8
	rjmp 3f 
3:
	lsr kc
	ld r18, X+
	ld r19, X+
	ld r20, X+
	ld r21, X+
	ld r22, X+
	ld r23, X+
	ld r24, X+
	ld r25, X+
	/* now we xor bl in */
	movw r26, bl1_sec
	ld r0, X+
	eor r18, r0
	ld r0, X+
	eor r19, r0
	ld r0, X+
	eor r20, r0
	ld r0, X+
	eor r21, r0
	ld r0, X+
	eor r22, r0
	ld r0, X+
	eor r23, r0
	ld r0, X+
	eor r24, r0
	ld r0, X+
	eor r25, r0
	/* f(x,k) = p(s(x xor k)) ; xor is done */
	call camellia_s;
	call camellia_p;
	
//	in r26, SPL
//	in r27, SPH
//	sbiw r26, 9
//	dbg_hexdump 10 
	/* now we have to xor the result into br */
	clr r31
	ldi r30, 18
	movw r26, br1_sec
;	ldi r1, 8 ;-- this won't work
	clr r1
	sec
	ror r1
	swap r1
1:	 ld r0, X
	 ld t, Z+
	 eor r0, t
	 st X+, r0
	dec r1
	brne 1b
	
	/* check for keyop */
	cp loop_cnt, keyop_time
	brne 3f
	movw s1, s1_sec
	ldi r22, 1
	SBRS xro_sec, 3 // KEY_ROL17
	 neg r22
	SBRS xro_sec, 2 // KEY_DIR
	 rjmp 2f
	rcall camellia128_keyop_inv
	rjmp 3f 
2:	rcall camellia128_keyop
3:	/* loop back */
	SWAP_R br1_sec, bl1_sec
	SWAP_R br2_sec, bl2_sec
	dec loop_cnt
	breq 2f
	rjmp main_loop
2:
	pop r7
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15
	pop r16
	pop r17
	ret
	
;##############################################################################
/*	
void camellia128_init(camellia128_ctx_t* s, uint8_t* key){
	uint8_t i;
	s->kll = 0; //((uint64_t*)key)[0];
	
	/ * load the key, endian-adjusted, to kll,klr * /
	for(i=0; i<8; ++i){
		s->kll <<= 8;
		s->kll |= *key++;
	}
	for(i=0; i<8; ++i){
		s->klr <<= 8;
		s->klr |= *key++;
	}
	
	s->kal = s->kll;
	s->kar = s->klr;
	
	s->kar ^= camellia_f(s->kal, camellia_sigma[0]);
	s->kal ^= camellia_f(s->kar, camellia_sigma[1]);
	
	s->kal ^= s->kll;
	s->kar ^= s->klr;
	
	s->kar ^= camellia_f(s->kal, camellia_sigma[2]);
	s->kal ^= camellia_f(s->kar, camellia_sigma[3]);
	/ * * /
//	uart_putstr("\n\r----------------init finished--------------------");
}	
*/	
/*
X64_xor_in:
	ld r0, X+ 
	eor r18, r0
	ld r0, X+ 
	eor r19, r0
	ld r0, X+ 
	eor r20, r0
	ld r0, X+ 
	eor r21, r0
	ld r0, X+ 
	eor r22, r0
	ld r0, X+ 
	eor r23, r0
	ld r0, X+ 
	eor r24, r0
	ld r0, X+ 
	eor r25, r0
	ret

X64_load:
	ld r18, X+
	ld r19, X+
	ld r20, X+
	ld r21, X+
	ld r22, X+
	ld r23, X+
	ld r24, X+
	ld r25, X+
	ret

Y64_load_xor_store:
	ld r0, Y
	eor r18, r0
	st Y+, r18
	ld r0, Y
	eor r19, r0
	st Y+, r19
	ld r0, Y
	eor r20, r0
	st Y+, r20
	ld r0, Y
	eor r21, r0
	st Y+, r21
	ld r0, Y
	eor r22, r0
	st Y+, r22
	ld r0, Y
	eor r23, r0
	st Y+, r23
	ld r0, Y
	eor r24, r0
	st Y+, r24
	ld r0, Y
	eor r25, r0
	st Y+, r25
	ret	
	
; param s:  r24-r25
; param *k: r22-r23
//.global camellia128_init
camellia128_init:	
	push r29
	push r28
	movw r30, r24 ; Z is statepointer
	movw r26, r22 ; X is keypointer
	clr r29
	ldi r28, 18
//	/ * load key into kl, ka and kal to r18:r25 * /
	adiw r26, 128/8 ;-- 16
	ldi r16, (128/8)-1
1:	ld r17, -X
	std Z+(128/8), r17
	st Z+, r17
	sbrs r16, 3
	st Y+, r17		; this should only be done the last 8 rounds 0<=r16<=7
	dec r16
	brpl 1b
//	/ * step 1 * /
	ldi r26, lo8(camellia_sigma)
	ldi r27, hi8(camellia_sigma)
	rcall X64_xor_in
	rcall camellia_s
	rcall camellia_p	// / * f(x,k) is done * /
	sbiw r30, 128/8
	movw r28, r30 ; Z&Y point on kar now
	call Y64_load_xor_store

//	/ * step 2 now * /
	rcall X64_xor_in
	rcall camellia_s
	rcall camellia_p	// / * f(x,k) is done * /
	rcall Y64_load_xor_store
	
//	/ * now the xor part (kl and kr) * /
	sbiw r30, 128/8 	; Z points to klr
	ldi r16, 128/8
1:	ld  r0, Z+
	ldd r1, Z+(128/8)-1
	eor r0, r1
	std Z+(128/8)-1, r0 
	dec r16
	brne 1b
	
//	/ * now s->kar ^= camellia_f(s->kal, camellia_sigma[2]); * /	
	rcall X64_load 		; load sigma[2]
	movw r26, r28		; X&Y point at kal
	rcall X64_xor_in
	rcall camellia_s
	rcall camellia_p
	sbiw r28, 128/8/2	; Y points at kar
	rcall Y64_load_xor_store
	
//	/ * now s->kal ^= camellia_f(s->kar, camellia_sigma[3]); * /
	sbiw r26, 128/8		;
	rcall X64_load 		; load kar
	ldi r26, lo8(camellia_sigma+3*8)
	ldi r27, hi8(camellia_sigma+3*8)
	rcall X64_xor_in		; xor sigma[3] in
	rcall camellia_s
	rcall camellia_p
	rcall Y64_load_xor_store
	
	pop r28
	pop r29
	ret
	
//*/	
	
	
	






	
