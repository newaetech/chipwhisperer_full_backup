;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 3.6.0 #9615 (MINGW32)
;--------------------------------------------------------
	.module aes
	.optsdcc -mmcs51 --model-small
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _update_key
	.globl _mix_columns
	.globl _shift_rows
	.globl _sub_bytes
	.globl _add_round_key
	.globl _xtime
	.globl _aes_encrypt
;--------------------------------------------------------
; special function registers
;--------------------------------------------------------
	.area RSEG    (ABS,DATA)
	.org 0x0000
;--------------------------------------------------------
; special function bits
;--------------------------------------------------------
	.area RSEG    (ABS,DATA)
	.org 0x0000
;--------------------------------------------------------
; overlayable register banks
;--------------------------------------------------------
	.area REG_BANK_0	(REL,OVR,DATA)
	.ds 8
;--------------------------------------------------------
; internal ram data
;--------------------------------------------------------
	.area DSEG    (DATA)
;--------------------------------------------------------
; overlayable items in internal ram 
;--------------------------------------------------------
;--------------------------------------------------------
; indirectly addressable internal ram data
;--------------------------------------------------------
	.area ISEG    (DATA)
;--------------------------------------------------------
; absolute internal ram data
;--------------------------------------------------------
	.area IABS    (ABS,DATA)
	.area IABS    (ABS,DATA)
;--------------------------------------------------------
; bit data
;--------------------------------------------------------
	.area BSEG    (BIT)
;--------------------------------------------------------
; paged external ram data
;--------------------------------------------------------
	.area PSEG    (PAG,XDATA)
;--------------------------------------------------------
; external ram data
;--------------------------------------------------------
	.area XSEG    (XDATA)
;--------------------------------------------------------
; absolute external ram data
;--------------------------------------------------------
	.area XABS    (ABS,XDATA)
;--------------------------------------------------------
; external initialized ram data
;--------------------------------------------------------
	.area XISEG   (XDATA)
	.area HOME    (CODE)
	.area GSINIT0 (CODE)
	.area GSINIT1 (CODE)
	.area GSINIT2 (CODE)
	.area GSINIT3 (CODE)
	.area GSINIT4 (CODE)
	.area GSINIT5 (CODE)
	.area GSINIT  (CODE)
	.area GSFINAL (CODE)
	.area CSEG    (CODE)
;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
	.area HOME    (CODE)
	.area GSINIT  (CODE)
	.area GSFINAL (CODE)
	.area GSINIT  (CODE)
;--------------------------------------------------------
; Home
;--------------------------------------------------------
	.area HOME    (CODE)
	.area HOME    (CODE)
;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CSEG    (CODE)
;------------------------------------------------------------
;Allocation info for local variables in function 'xtime'
;------------------------------------------------------------
;x                         Allocated to registers r7 
;------------------------------------------------------------
;	aes.c:70: uint8_t xtime(uint8_t x)
;	-----------------------------------------
;	 function xtime
;	-----------------------------------------
_xtime:
	ar7 = 0x07
	ar6 = 0x06
	ar5 = 0x05
	ar4 = 0x04
	ar3 = 0x03
	ar2 = 0x02
	ar1 = 0x01
	ar0 = 0x00
;	aes.c:72: return ((x<<1) ^ (((x>>7) & 1) * 0x1b));
	mov	a,dpl
	mov	r7,a
	add	a,acc
	mov	r6,a
	mov	a,r7
	rl	a
	anl	a,#0x01
	mov	b,#0x1b
	mul	ab
	xrl	a,r6
	mov	dpl,a
	ret
;------------------------------------------------------------
;Allocation info for local variables in function 'add_round_key'
;------------------------------------------------------------
;w                         Allocated to stack - _bp -5
;state                     Allocated to stack - _bp +2
;i                         Allocated to stack - _bp +5
;sloc0                     Allocated to stack - _bp +1
;------------------------------------------------------------
;	aes.c:76: void add_round_key(uint8_t* state, uint8_t* w)
;	-----------------------------------------
;	 function add_round_key
;	-----------------------------------------
_add_round_key:
	push	_bp
	mov	a,sp
	mov	_bp,a
	add	a,#0x05
	mov	sp,a
	mov	r0,_bp
	inc	r0
	inc	r0
	mov	@r0,dpl
	inc	r0
	mov	@r0,dph
	inc	r0
	mov	@r0,b
;	aes.c:79: for(i = 0; i < KEYLEN; i++)
	mov	a,_bp
	add	a,#0x05
	mov	r0,a
	mov	@r0,#0x00
00102$:
;	aes.c:80: state[i] ^= w[i];
	mov	r0,_bp
	inc	r0
	inc	r0
	mov	a,_bp
	add	a,#0x05
	mov	r1,a
	mov	a,@r1
	add	a,@r0
	mov	r2,a
	clr	a
	inc	r0
	addc	a,@r0
	mov	r3,a
	inc	r0
	mov	ar7,@r0
	mov	dpl,r2
	mov	dph,r3
	mov	b,r7
	mov	r0,_bp
	inc	r0
	lcall	__gptrget
	mov	@r0,a
	mov	a,_bp
	add	a,#0xfb
	mov	r0,a
	mov	a,_bp
	add	a,#0x05
	mov	r1,a
	mov	a,@r1
	add	a,@r0
	mov	r4,a
	clr	a
	inc	r0
	addc	a,@r0
	mov	r5,a
	inc	r0
	mov	ar6,@r0
	mov	dpl,r4
	mov	dph,r5
	mov	b,r6
	lcall	__gptrget
	mov	r4,a
	mov	r0,_bp
	inc	r0
	mov	a,@r0
	xrl	ar4,a
	mov	dpl,r2
	mov	dph,r3
	mov	b,r7
	mov	a,r4
	lcall	__gptrput
;	aes.c:79: for(i = 0; i < KEYLEN; i++)
	mov	a,_bp
	add	a,#0x05
	mov	r0,a
	inc	@r0
	mov	a,_bp
	add	a,#0x05
	mov	r0,a
	cjne	@r0,#0x10,00110$
00110$:
	jc	00102$
	mov	sp,_bp
	pop	_bp
	ret
;------------------------------------------------------------
;Allocation info for local variables in function 'sub_bytes'
;------------------------------------------------------------
;state                     Allocated to registers r5 r6 r7 
;i                         Allocated to registers r4 
;------------------------------------------------------------
;	aes.c:83: void sub_bytes(uint8_t* state)
;	-----------------------------------------
;	 function sub_bytes
;	-----------------------------------------
_sub_bytes:
	mov	r5,dpl
	mov	r6,dph
	mov	r7,b
;	aes.c:86: for(i = 0; i < KEYLEN; i++)
	mov	r4,#0x00
00102$:
;	aes.c:87: state[i] = s_box[state[i]];
	mov	a,r4
	add	a,r5
	mov	r1,a
	clr	a
	addc	a,r6
	mov	r2,a
	mov	ar3,r7
	mov	dpl,r1
	mov	dph,r2
	mov	b,r3
	lcall	__gptrget
	mov	dptr,#_s_box
	movc	a,@a+dptr
	mov	r0,a
	mov	dpl,r1
	mov	dph,r2
	mov	b,r3
	lcall	__gptrput
;	aes.c:86: for(i = 0; i < KEYLEN; i++)
	inc	r4
	cjne	r4,#0x10,00110$
00110$:
	jc	00102$
	ret
;------------------------------------------------------------
;Allocation info for local variables in function 'shift_rows'
;------------------------------------------------------------
;state                     Allocated to stack - _bp +1
;temp                      Allocated to stack - _bp +7
;sloc0                     Allocated to stack - _bp +4
;------------------------------------------------------------
;	aes.c:90: void shift_rows(uint8_t* state)
;	-----------------------------------------
;	 function shift_rows
;	-----------------------------------------
_shift_rows:
	push	_bp
	mov	_bp,sp
	push	dpl
	push	dph
	push	b
	mov	a,sp
	add	a,#0x04
	mov	sp,a
;	aes.c:93: temp = state[1];
	mov	r0,_bp
	inc	r0
	mov	a,#0x01
	add	a,@r0
	mov	r2,a
	clr	a
	inc	r0
	addc	a,@r0
	mov	r3,a
	inc	r0
	mov	ar4,@r0
	mov	dpl,r2
	mov	dph,r3
	mov	b,r4
	mov	a,_bp
	add	a,#0x07
	mov	r0,a
	lcall	__gptrget
	mov	@r0,a
;	aes.c:94: state[1] = state[5];
	mov	r0,_bp
	inc	r0
	mov	a,_bp
	add	a,#0x04
	mov	r1,a
	mov	a,#0x05
	add	a,@r0
	mov	@r1,a
	clr	a
	inc	r0
	addc	a,@r0
	inc	r1
	mov	@r1,a
	inc	r0
	inc	r1
	mov	a,@r0
	mov	@r1,a
	mov	a,_bp
	add	a,#0x04
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	__gptrget
	mov	dpl,r2
	mov	dph,r3
	mov	b,r4
	lcall	__gptrput
;	aes.c:95: state[5] = state[9];
	mov	r0,_bp
	inc	r0
	mov	a,#0x09
	add	a,@r0
	mov	r5,a
	clr	a
	inc	r0
	addc	a,@r0
	mov	r6,a
	inc	r0
	mov	ar7,@r0
	mov	dpl,r5
	mov	dph,r6
	mov	b,r7
	lcall	__gptrget
	mov	r4,a
	mov	a,_bp
	add	a,#0x04
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	mov	a,r4
	lcall	__gptrput
;	aes.c:96: state[9] = state[13];
	mov	r0,_bp
	inc	r0
	mov	a,_bp
	add	a,#0x04
	mov	r1,a
	mov	a,#0x0d
	add	a,@r0
	mov	@r1,a
	clr	a
	inc	r0
	addc	a,@r0
	inc	r1
	mov	@r1,a
	inc	r0
	inc	r1
	mov	a,@r0
	mov	@r1,a
	mov	a,_bp
	add	a,#0x04
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	__gptrget
	mov	dpl,r5
	mov	dph,r6
	mov	b,r7
	lcall	__gptrput
;	aes.c:97: state[13] = temp;
	mov	a,_bp
	add	a,#0x04
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	mov	a,_bp
	add	a,#0x07
	mov	r1,a
	mov	a,@r1
	lcall	__gptrput
;	aes.c:99: temp = state[2];
	mov	r0,_bp
	inc	r0
	mov	a,#0x02
	add	a,@r0
	mov	r5,a
	clr	a
	inc	r0
	addc	a,@r0
	mov	r6,a
	inc	r0
	mov	ar7,@r0
	mov	dpl,r5
	mov	dph,r6
	mov	b,r7
	mov	a,_bp
	add	a,#0x07
	mov	r0,a
	lcall	__gptrget
	mov	@r0,a
;	aes.c:100: state[2] = state[10];
	mov	r0,_bp
	inc	r0
	mov	a,_bp
	add	a,#0x04
	mov	r1,a
	mov	a,#0x0a
	add	a,@r0
	mov	@r1,a
	clr	a
	inc	r0
	addc	a,@r0
	inc	r1
	mov	@r1,a
	inc	r0
	inc	r1
	mov	a,@r0
	mov	@r1,a
	mov	a,_bp
	add	a,#0x04
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	__gptrget
	mov	dpl,r5
	mov	dph,r6
	mov	b,r7
	lcall	__gptrput
;	aes.c:101: state[10] = temp;
	mov	a,_bp
	add	a,#0x04
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	mov	a,_bp
	add	a,#0x07
	mov	r1,a
	mov	a,@r1
	lcall	__gptrput
;	aes.c:103: temp = state[6];
	mov	r0,_bp
	inc	r0
	mov	a,#0x06
	add	a,@r0
	mov	r5,a
	clr	a
	inc	r0
	addc	a,@r0
	mov	r6,a
	inc	r0
	mov	ar7,@r0
	mov	dpl,r5
	mov	dph,r6
	mov	b,r7
	mov	a,_bp
	add	a,#0x07
	mov	r0,a
	lcall	__gptrget
	mov	@r0,a
;	aes.c:104: state[6] = state[14];
	mov	r0,_bp
	inc	r0
	mov	a,_bp
	add	a,#0x04
	mov	r1,a
	mov	a,#0x0e
	add	a,@r0
	mov	@r1,a
	clr	a
	inc	r0
	addc	a,@r0
	inc	r1
	mov	@r1,a
	inc	r0
	inc	r1
	mov	a,@r0
	mov	@r1,a
	mov	a,_bp
	add	a,#0x04
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	__gptrget
	mov	dpl,r5
	mov	dph,r6
	mov	b,r7
	lcall	__gptrput
;	aes.c:105: state[14] = temp;
	mov	a,_bp
	add	a,#0x04
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	mov	a,_bp
	add	a,#0x07
	mov	r1,a
	mov	a,@r1
	lcall	__gptrput
;	aes.c:107: temp = state[3];
	mov	r0,_bp
	inc	r0
	mov	a,#0x03
	add	a,@r0
	mov	r5,a
	clr	a
	inc	r0
	addc	a,@r0
	mov	r6,a
	inc	r0
	mov	ar7,@r0
	mov	dpl,r5
	mov	dph,r6
	mov	b,r7
	mov	a,_bp
	add	a,#0x07
	mov	r0,a
	lcall	__gptrget
	mov	@r0,a
;	aes.c:108: state[3]  = state[15];
	mov	r0,_bp
	inc	r0
	mov	a,_bp
	add	a,#0x04
	mov	r1,a
	mov	a,#0x0f
	add	a,@r0
	mov	@r1,a
	clr	a
	inc	r0
	addc	a,@r0
	inc	r1
	mov	@r1,a
	inc	r0
	inc	r1
	mov	a,@r0
	mov	@r1,a
	mov	a,_bp
	add	a,#0x04
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	__gptrget
	mov	dpl,r5
	mov	dph,r6
	mov	b,r7
	lcall	__gptrput
;	aes.c:109: state[15] = state[11];
	mov	r0,_bp
	inc	r0
	mov	a,#0x0b
	add	a,@r0
	mov	r5,a
	clr	a
	inc	r0
	addc	a,@r0
	mov	r6,a
	inc	r0
	mov	ar7,@r0
	mov	dpl,r5
	mov	dph,r6
	mov	b,r7
	lcall	__gptrget
	mov	r4,a
	mov	a,_bp
	add	a,#0x04
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	mov	a,r4
	lcall	__gptrput
;	aes.c:110: state[11] = state[7];
	mov	r0,_bp
	inc	r0
	mov	a,_bp
	add	a,#0x04
	mov	r1,a
	mov	a,#0x07
	add	a,@r0
	mov	@r1,a
	clr	a
	inc	r0
	addc	a,@r0
	inc	r1
	mov	@r1,a
	inc	r0
	inc	r1
	mov	a,@r0
	mov	@r1,a
	mov	a,_bp
	add	a,#0x04
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	__gptrget
	mov	dpl,r5
	mov	dph,r6
	mov	b,r7
	lcall	__gptrput
;	aes.c:111: state[7]  = temp;
	mov	a,_bp
	add	a,#0x04
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	mov	a,_bp
	add	a,#0x07
	mov	r1,a
	mov	a,@r1
	lcall	__gptrput
	mov	sp,_bp
	pop	_bp
	ret
;------------------------------------------------------------
;Allocation info for local variables in function 'mix_columns'
;------------------------------------------------------------
;state                     Allocated to stack - _bp +1
;col                       Allocated to stack - _bp +18
;Tmp                       Allocated to stack - _bp +19
;Tm                        Allocated to registers r5 
;t                         Allocated to stack - _bp +20
;sloc0                     Allocated to stack - _bp +4
;sloc1                     Allocated to stack - _bp +5
;sloc2                     Allocated to stack - _bp +8
;sloc3                     Allocated to stack - _bp +9
;sloc4                     Allocated to stack - _bp +11
;sloc5                     Allocated to stack - _bp +14
;sloc6                     Allocated to stack - _bp +15
;------------------------------------------------------------
;	aes.c:114: void mix_columns(uint8_t* state)
;	-----------------------------------------
;	 function mix_columns
;	-----------------------------------------
_mix_columns:
	push	_bp
	mov	_bp,sp
	push	dpl
	push	dph
	push	b
	mov	a,sp
	add	a,#0x11
	mov	sp,a
;	aes.c:117: for(col = 0; col < 4; col++)
	mov	a,_bp
	add	a,#0x12
	mov	r0,a
	mov	@r0,#0x00
00102$:
;	aes.c:120: Tmp = state[4*col+0] ^ state[4*col+1] ^ state[4*col+2] ^ state[4*col+3];
	mov	a,_bp
	add	a,#0x12
	mov	r0,a
	mov	a,_bp
	add	a,#0x09
	mov	r1,a
	mov	a,@r0
	mov	b,#0x04
	mul	ab
	mov	@r1,a
	inc	r1
	mov	@r1,b
	mov	r0,_bp
	inc	r0
	mov	a,_bp
	add	a,#0x09
	mov	r1,a
	mov	a,@r1
	add	a,@r0
	push	acc
	inc	r1
	mov	a,@r1
	inc	r0
	addc	a,@r0
	push	acc
	inc	r0
	mov	a,@r0
	push	acc
	mov	a,_bp
	add	a,#0x07
	mov	r0,a
	pop	acc
	mov	@r0,a
	dec	r0
	pop	acc
	mov	@r0,a
	dec	r0
	pop	acc
	mov	@r0,a
	mov	a,_bp
	add	a,#0x05
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	mov	a,_bp
	add	a,#0x04
	mov	r1,a
	lcall	__gptrget
	mov	@r1,a
	mov	a,_bp
	add	a,#0x09
	mov	r0,a
	mov	a,#0x01
	add	a,@r0
	mov	r5,a
	clr	a
	inc	r0
	addc	a,@r0
	mov	r7,a
	mov	r0,_bp
	inc	r0
	mov	a,_bp
	add	a,#0x0b
	mov	r1,a
	mov	a,r5
	add	a,@r0
	mov	@r1,a
	mov	a,r7
	inc	r0
	addc	a,@r0
	inc	r1
	mov	@r1,a
	inc	r0
	inc	r1
	mov	a,@r0
	mov	@r1,a
	mov	a,_bp
	add	a,#0x0b
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	__gptrget
	mov	r4,a
	mov	a,_bp
	add	a,#0x04
	mov	r0,a
	mov	a,_bp
	add	a,#0x08
	mov	r1,a
	mov	a,r4
	xrl	a,@r0
	mov	@r1,a
	mov	a,_bp
	add	a,#0x09
	mov	r0,a
	mov	a,#0x02
	add	a,@r0
	mov	r3,a
	clr	a
	inc	r0
	addc	a,@r0
	mov	r4,a
	mov	r0,_bp
	inc	r0
	mov	a,r3
	add	a,@r0
	mov	r3,a
	mov	a,r4
	inc	r0
	addc	a,@r0
	mov	r4,a
	inc	r0
	mov	ar2,@r0
	mov	dpl,r3
	mov	dph,r4
	mov	b,r2
	lcall	__gptrget
	mov	r7,a
	mov	a,_bp
	add	a,#0x08
	mov	r0,a
	mov	a,_bp
	add	a,#0x0e
	mov	r1,a
	mov	a,r7
	xrl	a,@r0
	mov	@r1,a
	mov	a,_bp
	add	a,#0x09
	mov	r0,a
	mov	a,#0x03
	add	a,@r0
	mov	r5,a
	clr	a
	inc	r0
	addc	a,@r0
	mov	r6,a
	mov	r0,_bp
	inc	r0
	mov	a,_bp
	add	a,#0x0f
	mov	r1,a
	mov	a,r5
	add	a,@r0
	mov	@r1,a
	mov	a,r6
	inc	r0
	addc	a,@r0
	inc	r1
	mov	@r1,a
	inc	r0
	inc	r1
	mov	a,@r0
	mov	@r1,a
	mov	a,_bp
	add	a,#0x0f
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	__gptrget
	mov	r7,a
	mov	a,_bp
	add	a,#0x0e
	mov	r0,a
	mov	a,@r0
	xrl	ar7,a
	mov	a,_bp
	add	a,#0x13
	mov	r0,a
	mov	@r0,ar7
;	aes.c:121: t  = state[4*col];
	mov	a,_bp
	add	a,#0x04
	mov	r0,a
	mov	ar6,@r0
	mov	a,_bp
	add	a,#0x14
	mov	r0,a
	mov	@r0,ar6
;	aes.c:122: Tm = state[4*col+0] ^ state[4*col+1]; Tm = xtime(Tm); state[4*col+0] ^= (Tm ^ Tmp);
	mov	a,_bp
	add	a,#0x08
	mov	r0,a
	mov	ar5,@r0
	mov	dpl,r5
	push	ar4
	push	ar3
	push	ar2
	lcall	_xtime
	mov	r6,dpl
	pop	ar2
	pop	ar3
	pop	ar4
	mov	ar5,r6
	mov	a,_bp
	add	a,#0x05
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	__gptrget
	mov	r6,a
	mov	a,_bp
	add	a,#0x13
	mov	r0,a
	mov	a,@r0
	xrl	a,r5
	xrl	ar6,a
	mov	a,_bp
	add	a,#0x05
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	mov	a,r6
	lcall	__gptrput
;	aes.c:123: Tm = state[4*col+1] ^ state[4*col+2]; Tm = xtime(Tm); state[4*col+1] ^= (Tm ^ Tmp);
	mov	a,_bp
	add	a,#0x0b
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	__gptrget
	mov	r7,a
	mov	dpl,r3
	mov	dph,r4
	mov	b,r2
	lcall	__gptrget
	xrl	ar7,a
	mov	ar5,r7
	mov	dpl,r5
	push	ar4
	push	ar3
	push	ar2
	lcall	_xtime
	mov	r7,dpl
	pop	ar2
	pop	ar3
	pop	ar4
	mov	ar5,r7
	mov	a,_bp
	add	a,#0x0b
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	__gptrget
	mov	r7,a
	mov	a,_bp
	add	a,#0x13
	mov	r0,a
	mov	a,@r0
	xrl	a,r5
	xrl	ar7,a
	mov	a,_bp
	add	a,#0x0b
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	mov	a,r7
	lcall	__gptrput
;	aes.c:124: Tm = state[4*col+2] ^ state[4*col+3]; Tm = xtime(Tm); state[4*col+2] ^= (Tm ^ Tmp);
	mov	dpl,r3
	mov	dph,r4
	mov	b,r2
	lcall	__gptrget
	mov	r7,a
	mov	a,_bp
	add	a,#0x0f
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	__gptrget
	xrl	ar7,a
	mov	ar5,r7
	mov	dpl,r5
	push	ar4
	push	ar3
	push	ar2
	lcall	_xtime
	mov	r7,dpl
	pop	ar2
	pop	ar3
	pop	ar4
	mov	ar5,r7
	mov	dpl,r3
	mov	dph,r4
	mov	b,r2
	lcall	__gptrget
	mov	r7,a
	mov	a,_bp
	add	a,#0x13
	mov	r0,a
	mov	a,@r0
	xrl	a,r5
	xrl	ar7,a
	mov	dpl,r3
	mov	dph,r4
	mov	b,r2
	mov	a,r7
	lcall	__gptrput
;	aes.c:125: Tm = state[4*col+3] ^ t;              Tm = xtime(Tm); state[4*col+3] ^= (Tm ^ Tmp);
	mov	a,_bp
	add	a,#0x0f
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	__gptrget
	mov	r7,a
	mov	a,_bp
	add	a,#0x14
	mov	r0,a
	mov	a,@r0
	xrl	ar7,a
	mov	ar5,r7
	mov	dpl,r5
	lcall	_xtime
	mov	r7,dpl
	mov	ar5,r7
	mov	a,_bp
	add	a,#0x0f
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	__gptrget
	mov	r7,a
	mov	a,_bp
	add	a,#0x13
	mov	r0,a
	mov	a,@r0
	xrl	ar5,a
	mov	a,r5
	xrl	ar7,a
	mov	a,_bp
	add	a,#0x0f
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	mov	a,r7
	lcall	__gptrput
;	aes.c:117: for(col = 0; col < 4; col++)
	mov	a,_bp
	add	a,#0x12
	mov	r0,a
	inc	@r0
	mov	a,_bp
	add	a,#0x12
	mov	r0,a
	cjne	@r0,#0x04,00110$
00110$:
	jnc	00111$
	ljmp	00102$
00111$:
	mov	sp,_bp
	pop	_bp
	ret
;------------------------------------------------------------
;Allocation info for local variables in function 'update_key'
;------------------------------------------------------------
;round                     Allocated to stack - _bp -3
;w                         Allocated to stack - _bp +1
;temp                      Allocated to stack - _bp +18
;i                         Allocated to stack - _bp +22
;sloc0                     Allocated to stack - _bp +4
;sloc1                     Allocated to stack - _bp +6
;sloc2                     Allocated to stack - _bp +9
;sloc3                     Allocated to stack - _bp +12
;sloc4                     Allocated to stack - _bp +15
;sloc5                     Allocated to stack - _bp +16
;sloc6                     Allocated to stack - _bp +17
;------------------------------------------------------------
;	aes.c:129: void update_key(uint8_t* w, uint8_t round)
;	-----------------------------------------
;	 function update_key
;	-----------------------------------------
_update_key:
	push	_bp
	mov	_bp,sp
	push	dpl
	push	dph
	push	b
	mov	a,sp
	add	a,#0x13
	mov	sp,a
;	aes.c:134: for(i = 0; i < 4; i++)
	mov	r0,_bp
	inc	r0
	mov	a,#0x0d
	add	a,@r0
	mov	r2,a
	clr	a
	inc	r0
	addc	a,@r0
	mov	r3,a
	inc	r0
	mov	ar4,@r0
	mov	a,_bp
	add	a,#0xfd
	mov	r0,a
	mov	a,_bp
	add	a,#0x04
	mov	r1,a
	mov	a,@r0
	add	a,#_rcon
	mov	@r1,a
	clr	a
	addc	a,#(_rcon >> 8)
	inc	r1
	mov	@r1,a
	mov	r0,_bp
	inc	r0
	mov	a,_bp
	add	a,#0x06
	mov	r1,a
	mov	a,#0x0e
	add	a,@r0
	mov	@r1,a
	clr	a
	inc	r0
	addc	a,@r0
	inc	r1
	mov	@r1,a
	inc	r0
	inc	r1
	mov	a,@r0
	mov	@r1,a
	mov	r0,_bp
	inc	r0
	mov	a,_bp
	add	a,#0x09
	mov	r1,a
	mov	a,#0x0f
	add	a,@r0
	mov	@r1,a
	clr	a
	inc	r0
	addc	a,@r0
	inc	r1
	mov	@r1,a
	inc	r0
	inc	r1
	mov	a,@r0
	mov	@r1,a
	mov	r0,_bp
	inc	r0
	mov	a,_bp
	add	a,#0x0c
	mov	r1,a
	mov	a,#0x0c
	add	a,@r0
	mov	@r1,a
	clr	a
	inc	r0
	addc	a,@r0
	inc	r1
	mov	@r1,a
	inc	r0
	inc	r1
	mov	a,@r0
	mov	@r1,a
	mov	a,_bp
	add	a,#0x12
	mov	r1,a
	inc	a
	mov	r0,a
	push	ar0
	mov	a,_bp
	add	a,#0x0f
	mov	r0,a
	mov	a,#0x02
	add	a,r1
	mov	@r0,a
	mov	a,_bp
	add	a,#0x10
	mov	r0,a
	mov	a,#0x03
	add	a,r1
	mov	@r0,a
	mov	a,_bp
	add	a,#0x16
	mov	r0,a
	mov	@r0,#0x00
	pop	ar0
00105$:
;	aes.c:136: if(i == 0)
	push	ar0
	mov	a,_bp
	add	a,#0x16
	mov	r0,a
	mov	a,@r0
	pop	ar0
	jz	00116$
	ljmp	00102$
00116$:
;	aes.c:138: temp[0] = s_box[w[13]] ^ rcon[round];
	mov	dpl,r2
	mov	dph,r3
	mov	b,r4
	lcall	__gptrget
	mov	dptr,#_s_box
	movc	a,@a+dptr
	mov	r6,a
	push	ar0
	mov	a,_bp
	add	a,#0x04
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	clr	a
	movc	a,@a+dptr
	mov	r5,a
	xrl	a,r6
	mov	@r1,a
;	aes.c:139: temp[1] = s_box[w[14]];
	mov	a,_bp
	add	a,#0x06
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	__gptrget
	mov	r6,a
	pop	ar0
	mov	a,r6
	mov	dptr,#_s_box
	movc	a,@a+dptr
	mov	@r0,a
;	aes.c:140: temp[2] = s_box[w[15]];
	push	ar0
	mov	a,_bp
	add	a,#0x09
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	__gptrget
	mov	dptr,#_s_box
	movc	a,@a+dptr
	mov	r6,a
	mov	a,_bp
	add	a,#0x0f
	mov	r0,a
	mov	ar0,@r0
	mov	@r0,ar6
;	aes.c:141: temp[3] = s_box[w[12]];
	mov	a,_bp
	add	a,#0x0c
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	__gptrget
	mov	dptr,#_s_box
	movc	a,@a+dptr
	mov	r6,a
	mov	a,_bp
	add	a,#0x10
	mov	r0,a
	mov	ar0,@r0
	mov	@r0,ar6
	pop	ar0
	ljmp	00103$
00102$:
;	aes.c:145: temp[0] = w[4*(i-1) + 0];
	push	ar2
	push	ar3
	push	ar4
	push	ar0
	mov	a,_bp
	add	a,#0x16
	mov	r0,a
	mov	ar5,@r0
	mov	r6,#0x00
	pop	ar0
	dec	r5
	cjne	r5,#0xff,00117$
	dec	r6
00117$:
	mov	a,r6
	xch	a,r5
	add	a,acc
	xch	a,r5
	rlc	a
	xch	a,r5
	add	a,acc
	xch	a,r5
	rlc	a
	mov	r6,a
	push	ar0
	mov	r0,_bp
	inc	r0
	mov	a,r5
	add	a,@r0
	mov	r2,a
	mov	a,r6
	inc	r0
	addc	a,@r0
	mov	r3,a
	inc	r0
	mov	ar4,@r0
	mov	dpl,r2
	mov	dph,r3
	mov	b,r4
	lcall	__gptrget
	mov	@r1,a
;	aes.c:146: temp[1] = w[4*(i-1) + 1];
	mov	a,#0x01
	add	a,r5
	mov	r3,a
	clr	a
	addc	a,r6
	mov	r4,a
	mov	r0,_bp
	inc	r0
	mov	a,r3
	add	a,@r0
	mov	r3,a
	mov	a,r4
	inc	r0
	addc	a,@r0
	mov	r4,a
	inc	r0
	mov	ar2,@r0
	pop	ar0
	mov	dpl,r3
	mov	dph,r4
	mov	b,r2
	lcall	__gptrget
	mov	@r0,a
;	aes.c:147: temp[2] = w[4*(i-1) + 2];
	mov	a,#0x02
	add	a,r5
	mov	r3,a
	clr	a
	addc	a,r6
	mov	r4,a
	push	ar0
	mov	r0,_bp
	inc	r0
	mov	a,r3
	add	a,@r0
	mov	r3,a
	mov	a,r4
	inc	r0
	addc	a,@r0
	mov	r4,a
	inc	r0
	mov	ar2,@r0
	mov	dpl,r3
	mov	dph,r4
	mov	b,r2
	lcall	__gptrget
	mov	r3,a
	mov	a,_bp
	add	a,#0x0f
	mov	r0,a
	mov	ar0,@r0
	mov	@r0,ar3
;	aes.c:148: temp[3] = w[4*(i-1) + 3];
	mov	a,#0x03
	add	a,r5
	mov	r5,a
	clr	a
	addc	a,r6
	mov	r6,a
	mov	r0,_bp
	inc	r0
	mov	a,r5
	add	a,@r0
	mov	r5,a
	mov	a,r6
	inc	r0
	addc	a,@r0
	mov	r6,a
	inc	r0
	mov	ar4,@r0
	mov	dpl,r5
	mov	dph,r6
	mov	b,r4
	lcall	__gptrget
	mov	r5,a
	mov	a,_bp
	add	a,#0x10
	mov	r0,a
	mov	ar0,@r0
	mov	@r0,ar5
	pop	ar0
;	aes.c:134: for(i = 0; i < 4; i++)
	pop	ar4
	pop	ar3
	pop	ar2
;	aes.c:148: temp[3] = w[4*(i-1) + 3];
00103$:
;	aes.c:151: w[4*i+0] ^= temp[0];
	push	ar2
	push	ar3
	push	ar4
	push	ar0
	mov	a,_bp
	add	a,#0x16
	mov	r0,a
	mov	a,@r0
	mov	b,#0x04
	mul	ab
	mov	r5,a
	mov	r6,b
	mov	r0,_bp
	inc	r0
	mov	a,r5
	add	a,@r0
	mov	r2,a
	mov	a,r6
	inc	r0
	addc	a,@r0
	mov	r3,a
	inc	r0
	mov	ar4,@r0
	mov	dpl,r2
	mov	dph,r3
	mov	b,r4
	mov	a,_bp
	add	a,#0x11
	mov	r0,a
	lcall	__gptrget
	mov	@r0,a
	mov	ar7,@r1
	mov	a,_bp
	add	a,#0x11
	mov	r0,a
	mov	a,@r0
	xrl	ar7,a
	mov	dpl,r2
	mov	dph,r3
	mov	b,r4
	mov	a,r7
	lcall	__gptrput
;	aes.c:152: w[4*i+1] ^= temp[1];
	mov	a,#0x01
	add	a,r5
	mov	r4,a
	clr	a
	addc	a,r6
	mov	r7,a
	mov	r0,_bp
	inc	r0
	mov	a,r4
	add	a,@r0
	mov	r4,a
	mov	a,r7
	inc	r0
	addc	a,@r0
	mov	r7,a
	inc	r0
	mov	ar3,@r0
	mov	dpl,r4
	mov	dph,r7
	mov	b,r3
	mov	a,_bp
	add	a,#0x11
	mov	r0,a
	lcall	__gptrget
	mov	@r0,a
	pop	ar0
	mov	ar2,@r0
	push	ar0
	mov	a,_bp
	add	a,#0x11
	mov	r0,a
	mov	a,@r0
	xrl	ar2,a
	mov	dpl,r4
	mov	dph,r7
	mov	b,r3
	mov	a,r2
	lcall	__gptrput
;	aes.c:153: w[4*i+2] ^= temp[2];
	mov	a,#0x02
	add	a,r5
	mov	r4,a
	clr	a
	addc	a,r6
	mov	r7,a
	mov	r0,_bp
	inc	r0
	mov	a,r4
	add	a,@r0
	mov	r4,a
	mov	a,r7
	inc	r0
	addc	a,@r0
	mov	r7,a
	inc	r0
	mov	ar3,@r0
	mov	dpl,r4
	mov	dph,r7
	mov	b,r3
	mov	a,_bp
	add	a,#0x11
	mov	r0,a
	lcall	__gptrget
	mov	@r0,a
	mov	a,_bp
	add	a,#0x0f
	mov	r0,a
	mov	ar0,@r0
	mov	ar2,@r0
	mov	a,_bp
	add	a,#0x11
	mov	r0,a
	mov	a,@r0
	xrl	ar2,a
	mov	dpl,r4
	mov	dph,r7
	mov	b,r3
	mov	a,r2
	lcall	__gptrput
;	aes.c:154: w[4*i+3] ^= temp[3];
	mov	a,#0x03
	add	a,r5
	mov	r5,a
	clr	a
	addc	a,r6
	mov	r6,a
	mov	r0,_bp
	inc	r0
	mov	a,r5
	add	a,@r0
	mov	r5,a
	mov	a,r6
	inc	r0
	addc	a,@r0
	mov	r6,a
	inc	r0
	mov	ar7,@r0
	mov	dpl,r5
	mov	dph,r6
	mov	b,r7
	lcall	__gptrget
	mov	r4,a
	mov	a,_bp
	add	a,#0x10
	mov	r0,a
	mov	ar0,@r0
	mov	a,@r0
	mov	r3,a
	xrl	ar4,a
	mov	dpl,r5
	mov	dph,r6
	mov	b,r7
	mov	a,r4
	lcall	__gptrput
;	aes.c:134: for(i = 0; i < 4; i++)
	mov	a,_bp
	add	a,#0x16
	mov	r0,a
	inc	@r0
	mov	a,_bp
	add	a,#0x16
	mov	r0,a
	cjne	@r0,#0x04,00118$
00118$:
	pop	ar0
	pop	ar4
	pop	ar3
	pop	ar2
	jnc	00119$
	ljmp	00105$
00119$:
	mov	sp,_bp
	pop	_bp
	ret
;------------------------------------------------------------
;Allocation info for local variables in function 'aes_encrypt'
;------------------------------------------------------------
;key                       Allocated to stack - _bp -5
;state                     Allocated to stack - _bp +1
;i                         Allocated to registers r3 
;w                         Allocated to stack - _bp +4
;------------------------------------------------------------
;	aes.c:158: void aes_encrypt(uint8_t* state, uint8_t* key)
;	-----------------------------------------
;	 function aes_encrypt
;	-----------------------------------------
_aes_encrypt:
	push	_bp
	mov	_bp,sp
	push	dpl
	push	dph
	push	b
	mov	a,sp
	add	a,#0x10
	mov	sp,a
;	aes.c:162: for(i = 0; i < KEYLEN; i++)
	mov	a,_bp
	add	a,#0x04
	mov	r4,a
	mov	r3,#0x00
00103$:
;	aes.c:163: w[i] = key[i];
	mov	a,r3
	add	a,r4
	mov	r1,a
	mov	a,_bp
	add	a,#0xfb
	mov	r0,a
	mov	a,r3
	add	a,@r0
	mov	r2,a
	clr	a
	inc	r0
	addc	a,@r0
	mov	r6,a
	inc	r0
	mov	ar7,@r0
	mov	dpl,r2
	mov	dph,r6
	mov	b,r7
	lcall	__gptrget
	mov	r2,a
	mov	@r1,a
;	aes.c:162: for(i = 0; i < KEYLEN; i++)
	inc	r3
	cjne	r3,#0x10,00123$
00123$:
	jc	00103$
;	aes.c:165: add_round_key(state, w);
	mov	ar3,r4
	mov	ar2,r3
	mov	r3,#0x00
	mov	r7,#0x40
	push	ar4
	push	ar2
	push	ar3
	push	ar7
	mov	r0,_bp
	inc	r0
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	_add_round_key
	dec	sp
	dec	sp
	dec	sp
	pop	ar4
;	aes.c:166: for(i = 1; i < Nr; i++)
	mov	ar7,r4
	mov	ar6,r4
	mov	r5,#0x01
00105$:
;	aes.c:168: sub_bytes(state);
	push	ar4
	mov	r0,_bp
	inc	r0
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	push	ar7
	push	ar6
	push	ar5
	push	ar4
	lcall	_sub_bytes
;	aes.c:169: shift_rows(state);
	mov	r0,_bp
	inc	r0
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	_shift_rows
;	aes.c:170: mix_columns(state);
	mov	r0,_bp
	inc	r0
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	_mix_columns
	pop	ar4
	pop	ar5
	pop	ar6
	pop	ar7
;	aes.c:171: update_key(w, i);	
	mov	ar2,r7
	mov	r3,#0x00
	mov	r4,#0x40
	push	ar7
	push	ar6
	push	ar5
	push	ar4
	push	ar5
	mov	dpl,r2
	mov	dph,r3
	mov	b,r4
	lcall	_update_key
	dec	sp
	pop	ar4
	pop	ar5
	pop	ar6
;	aes.c:172: add_round_key(state, w);
	mov	ar2,r6
	mov	r3,#0x00
	mov	r4,#0x40
	push	ar6
	push	ar5
	push	ar4
	push	ar2
	push	ar3
	push	ar4
	mov	r0,_bp
	inc	r0
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	_add_round_key
	dec	sp
	dec	sp
	dec	sp
	pop	ar4
	pop	ar5
	pop	ar6
	pop	ar7
;	aes.c:166: for(i = 1; i < Nr; i++)
	inc	r5
	cjne	r5,#0x0a,00125$
00125$:
	pop	ar4
	jnc	00126$
	ljmp	00105$
00126$:
;	aes.c:175: sub_bytes(state);
	mov	r0,_bp
	inc	r0
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	push	ar5
	push	ar4
	lcall	_sub_bytes
;	aes.c:176: shift_rows(state);
	mov	r0,_bp
	inc	r0
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	_shift_rows
	pop	ar4
	pop	ar5
;	aes.c:177: update_key(w, i);
	mov	ar7,r4
	mov	r6,#0x00
	mov	r3,#0x40
	push	ar4
	push	ar5
	mov	dpl,r7
	mov	dph,r6
	mov	b,r3
	lcall	_update_key
	dec	sp
;	aes.c:178: add_round_key(state, w);
	mov	r7,#0x00
	mov	r6,#0x40
	push	ar7
	push	ar6
	mov	r0,_bp
	inc	r0
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	lcall	_add_round_key
	dec	sp
	dec	sp
	dec	sp
	mov	sp,_bp
	pop	_bp
	ret
	.area CSEG    (CODE)
	.area CONST   (CODE)
_s_box:
	.db #0x63	; 99	'c'
	.db #0x7c	; 124
	.db #0x77	; 119	'w'
	.db #0x7b	; 123
	.db #0xf2	; 242
	.db #0x6b	; 107	'k'
	.db #0x6f	; 111	'o'
	.db #0xc5	; 197
	.db #0x30	; 48	'0'
	.db #0x01	; 1
	.db #0x67	; 103	'g'
	.db #0x2b	; 43
	.db #0xfe	; 254
	.db #0xd7	; 215
	.db #0xab	; 171
	.db #0x76	; 118	'v'
	.db #0xca	; 202
	.db #0x82	; 130
	.db #0xc9	; 201
	.db #0x7d	; 125
	.db #0xfa	; 250
	.db #0x59	; 89	'Y'
	.db #0x47	; 71	'G'
	.db #0xf0	; 240
	.db #0xad	; 173
	.db #0xd4	; 212
	.db #0xa2	; 162
	.db #0xaf	; 175
	.db #0x9c	; 156
	.db #0xa4	; 164
	.db #0x72	; 114	'r'
	.db #0xc0	; 192
	.db #0xb7	; 183
	.db #0xfd	; 253
	.db #0x93	; 147
	.db #0x26	; 38
	.db #0x36	; 54	'6'
	.db #0x3f	; 63
	.db #0xf7	; 247
	.db #0xcc	; 204
	.db #0x34	; 52	'4'
	.db #0xa5	; 165
	.db #0xe5	; 229
	.db #0xf1	; 241
	.db #0x71	; 113	'q'
	.db #0xd8	; 216
	.db #0x31	; 49	'1'
	.db #0x15	; 21
	.db #0x04	; 4
	.db #0xc7	; 199
	.db #0x23	; 35
	.db #0xc3	; 195
	.db #0x18	; 24
	.db #0x96	; 150
	.db #0x05	; 5
	.db #0x9a	; 154
	.db #0x07	; 7
	.db #0x12	; 18
	.db #0x80	; 128
	.db #0xe2	; 226
	.db #0xeb	; 235
	.db #0x27	; 39
	.db #0xb2	; 178
	.db #0x75	; 117	'u'
	.db #0x09	; 9
	.db #0x83	; 131
	.db #0x2c	; 44
	.db #0x1a	; 26
	.db #0x1b	; 27
	.db #0x6e	; 110	'n'
	.db #0x5a	; 90	'Z'
	.db #0xa0	; 160
	.db #0x52	; 82	'R'
	.db #0x3b	; 59
	.db #0xd6	; 214
	.db #0xb3	; 179
	.db #0x29	; 41
	.db #0xe3	; 227
	.db #0x2f	; 47
	.db #0x84	; 132
	.db #0x53	; 83	'S'
	.db #0xd1	; 209
	.db #0x00	; 0
	.db #0xed	; 237
	.db #0x20	; 32
	.db #0xfc	; 252
	.db #0xb1	; 177
	.db #0x5b	; 91
	.db #0x6a	; 106	'j'
	.db #0xcb	; 203
	.db #0xbe	; 190
	.db #0x39	; 57	'9'
	.db #0x4a	; 74	'J'
	.db #0x4c	; 76	'L'
	.db #0x58	; 88	'X'
	.db #0xcf	; 207
	.db #0xd0	; 208
	.db #0xef	; 239
	.db #0xaa	; 170
	.db #0xfb	; 251
	.db #0x43	; 67	'C'
	.db #0x4d	; 77	'M'
	.db #0x33	; 51	'3'
	.db #0x85	; 133
	.db #0x45	; 69	'E'
	.db #0xf9	; 249
	.db #0x02	; 2
	.db #0x7f	; 127
	.db #0x50	; 80	'P'
	.db #0x3c	; 60
	.db #0x9f	; 159
	.db #0xa8	; 168
	.db #0x51	; 81	'Q'
	.db #0xa3	; 163
	.db #0x40	; 64
	.db #0x8f	; 143
	.db #0x92	; 146
	.db #0x9d	; 157
	.db #0x38	; 56	'8'
	.db #0xf5	; 245
	.db #0xbc	; 188
	.db #0xb6	; 182
	.db #0xda	; 218
	.db #0x21	; 33
	.db #0x10	; 16
	.db #0xff	; 255
	.db #0xf3	; 243
	.db #0xd2	; 210
	.db #0xcd	; 205
	.db #0x0c	; 12
	.db #0x13	; 19
	.db #0xec	; 236
	.db #0x5f	; 95
	.db #0x97	; 151
	.db #0x44	; 68	'D'
	.db #0x17	; 23
	.db #0xc4	; 196
	.db #0xa7	; 167
	.db #0x7e	; 126
	.db #0x3d	; 61
	.db #0x64	; 100	'd'
	.db #0x5d	; 93
	.db #0x19	; 25
	.db #0x73	; 115	's'
	.db #0x60	; 96
	.db #0x81	; 129
	.db #0x4f	; 79	'O'
	.db #0xdc	; 220
	.db #0x22	; 34
	.db #0x2a	; 42
	.db #0x90	; 144
	.db #0x88	; 136
	.db #0x46	; 70	'F'
	.db #0xee	; 238
	.db #0xb8	; 184
	.db #0x14	; 20
	.db #0xde	; 222
	.db #0x5e	; 94
	.db #0x0b	; 11
	.db #0xdb	; 219
	.db #0xe0	; 224
	.db #0x32	; 50	'2'
	.db #0x3a	; 58
	.db #0x0a	; 10
	.db #0x49	; 73	'I'
	.db #0x06	; 6
	.db #0x24	; 36
	.db #0x5c	; 92
	.db #0xc2	; 194
	.db #0xd3	; 211
	.db #0xac	; 172
	.db #0x62	; 98	'b'
	.db #0x91	; 145
	.db #0x95	; 149
	.db #0xe4	; 228
	.db #0x79	; 121	'y'
	.db #0xe7	; 231
	.db #0xc8	; 200
	.db #0x37	; 55	'7'
	.db #0x6d	; 109	'm'
	.db #0x8d	; 141
	.db #0xd5	; 213
	.db #0x4e	; 78	'N'
	.db #0xa9	; 169
	.db #0x6c	; 108	'l'
	.db #0x56	; 86	'V'
	.db #0xf4	; 244
	.db #0xea	; 234
	.db #0x65	; 101	'e'
	.db #0x7a	; 122	'z'
	.db #0xae	; 174
	.db #0x08	; 8
	.db #0xba	; 186
	.db #0x78	; 120	'x'
	.db #0x25	; 37
	.db #0x2e	; 46
	.db #0x1c	; 28
	.db #0xa6	; 166
	.db #0xb4	; 180
	.db #0xc6	; 198
	.db #0xe8	; 232
	.db #0xdd	; 221
	.db #0x74	; 116	't'
	.db #0x1f	; 31
	.db #0x4b	; 75	'K'
	.db #0xbd	; 189
	.db #0x8b	; 139
	.db #0x8a	; 138
	.db #0x70	; 112	'p'
	.db #0x3e	; 62
	.db #0xb5	; 181
	.db #0x66	; 102	'f'
	.db #0x48	; 72	'H'
	.db #0x03	; 3
	.db #0xf6	; 246
	.db #0x0e	; 14
	.db #0x61	; 97	'a'
	.db #0x35	; 53	'5'
	.db #0x57	; 87	'W'
	.db #0xb9	; 185
	.db #0x86	; 134
	.db #0xc1	; 193
	.db #0x1d	; 29
	.db #0x9e	; 158
	.db #0xe1	; 225
	.db #0xf8	; 248
	.db #0x98	; 152
	.db #0x11	; 17
	.db #0x69	; 105	'i'
	.db #0xd9	; 217
	.db #0x8e	; 142
	.db #0x94	; 148
	.db #0x9b	; 155
	.db #0x1e	; 30
	.db #0x87	; 135
	.db #0xe9	; 233
	.db #0xce	; 206
	.db #0x55	; 85	'U'
	.db #0x28	; 40
	.db #0xdf	; 223
	.db #0x8c	; 140
	.db #0xa1	; 161
	.db #0x89	; 137
	.db #0x0d	; 13
	.db #0xbf	; 191
	.db #0xe6	; 230
	.db #0x42	; 66	'B'
	.db #0x68	; 104	'h'
	.db #0x41	; 65	'A'
	.db #0x99	; 153
	.db #0x2d	; 45
	.db #0x0f	; 15
	.db #0xb0	; 176
	.db #0x54	; 84	'T'
	.db #0xbb	; 187
	.db #0x16	; 22
_rcon:
	.db #0x00	; 0
	.db #0x01	; 1
	.db #0x02	; 2
	.db #0x04	; 4
	.db #0x08	; 8
	.db #0x10	; 16
	.db #0x20	; 32
	.db #0x40	; 64
	.db #0x80	; 128
	.db #0x1b	; 27
	.db #0x36	; 54	'6'
	.area XINIT   (CODE)
	.area CABS    (ABS,CODE)
