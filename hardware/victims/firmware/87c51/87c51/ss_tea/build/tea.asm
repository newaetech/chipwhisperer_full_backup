;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 3.6.0 #9615 (MINGW32)
;--------------------------------------------------------
	.module tea
	.optsdcc -mmcs51 --model-small
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _tea_encrypt
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
;Allocation info for local variables in function 'tea_encrypt'
;------------------------------------------------------------
;k                         Allocated to stack - _bp -5
;v                         Allocated to stack - _bp +1
;vp                        Allocated to stack - _bp +22
;kp                        Allocated to stack - _bp +30
;sum                       Allocated to stack - _bp +46
;i                         Allocated to stack - _bp +50
;j                         Allocated to stack - _bp +54
;delta                     Allocated to registers 
;a                         Allocated to stack - _bp +58
;b                         Allocated to stack - _bp +62
;c                         Allocated to stack - _bp +66
;shift                     Allocated to stack - _bp +58
;sloc0                     Allocated to stack - _bp +4
;sloc1                     Allocated to stack - _bp +8
;sloc2                     Allocated to stack - _bp +12
;sloc3                     Allocated to stack - _bp +13
;sloc4                     Allocated to stack - _bp +14
;sloc5                     Allocated to stack - _bp +18
;------------------------------------------------------------
;	tea.c:8: void tea_encrypt(uint8_t* v, uint8_t* k)
;	-----------------------------------------
;	 function tea_encrypt
;	-----------------------------------------
_tea_encrypt:
	ar7 = 0x07
	ar6 = 0x06
	ar5 = 0x05
	ar4 = 0x04
	ar3 = 0x03
	ar2 = 0x02
	ar1 = 0x01
	ar0 = 0x00
	push	_bp
	mov	_bp,sp
	push	dpl
	push	dph
	push	b
	mov	a,sp
	add	a,#0x42
	mov	sp,a
;	tea.c:17: for(i = 0; i < 2; i++){
	mov	a,_bp
	add	a,#0x16
	mov	r1,a
	mov	a,_bp
	add	a,#0x32
	mov	r0,a
	clr	a
	mov	@r0,a
	inc	r0
	mov	@r0,a
	inc	r0
	mov	@r0,a
	inc	r0
	mov	@r0,a
00109$:
;	tea.c:18: vp[i] = 0;
	mov	a,_bp
	add	a,#0x32
	mov	r0,a
	mov	a,@r0
	add	a,acc
	mov	r2,a
	inc	r0
	mov	a,@r0
	rlc	a
	mov	r3,a
	inc	r0
	mov	a,@r0
	rlc	a
	mov	r4,a
	inc	r0
	mov	a,@r0
	rlc	a
	mov	r7,a
	mov	a,r2
	add	a,r2
	mov	r2,a
	mov	a,r3
	rlc	a
	mov	r3,a
	mov	a,r4
	rlc	a
	mov	r4,a
	mov	a,r7
	rlc	a
	mov	r7,a
	mov	a,r2
	add	a,r1
	mov	r0,a
	mov	@r0,#0x00
	inc	r0
	mov	@r0,#0x00
	inc	r0
	mov	@r0,#0x00
	inc	r0
	mov	@r0,#0x00
;	tea.c:19: for(j = 0; j < 4; j++)
	mov	a,_bp
	add	a,#0x04
	mov	r0,a
	mov	@r0,ar2
	inc	r0
	mov	@r0,ar3
	inc	r0
	mov	@r0,ar4
	inc	r0
	mov	@r0,ar7
	mov	a,_bp
	add	a,#0x36
	mov	r0,a
	clr	a
	mov	@r0,a
	inc	r0
	mov	@r0,a
	inc	r0
	mov	@r0,a
	inc	r0
	mov	@r0,a
;	tea.c:48: for(i = 0; i < 8; i++)
;	tea.c:19: for(j = 0; j < 4; j++)
00107$:
;	tea.c:20: vp[i] = (vp[i] << 8) | v[4*i + j]; 
	mov	a,_bp
	add	a,#0x04
	mov	r0,a
	mov	a,@r0
	add	a,r1
	mov	r0,a
	push	ar1
	mov	ar2,@r0
	inc	r0
	mov	ar3,@r0
	inc	r0
	mov	ar4,@r0
	inc	r0
	mov	ar7,@r0
	dec	r0
	dec	r0
	dec	r0
	push	ar0
	mov	a,_bp
	add	a,#0x08
	mov	r0,a
	inc	r0
	inc	r0
	inc	r0
	mov	@r0,ar4
	dec	r0
	mov	@r0,ar3
	dec	r0
	mov	@r0,ar2
	dec	r0
	mov	@r0,#0x00
	mov	a,_bp
	add	a,#0x04
	mov	r0,a
	push	ar1
	mov	a,_bp
	add	a,#0x36
	mov	r1,a
	mov	a,@r1
	add	a,@r0
	mov	r4,a
	inc	r1
	mov	a,@r1
	inc	r0
	addc	a,@r0
	mov	r5,a
	inc	r1
	mov	a,@r1
	inc	r0
	addc	a,@r0
	inc	r1
	mov	a,@r1
	inc	r0
	addc	a,@r0
	pop	ar1
	mov	r0,_bp
	inc	r0
	mov	a,r4
	add	a,@r0
	mov	r4,a
	mov	a,r5
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
	clr	a
	mov	r7,a
	mov	r6,a
	mov	r5,a
	mov	a,_bp
	add	a,#0x08
	mov	r0,a
	mov	a,@r0
	orl	ar4,a
	inc	r0
	mov	a,@r0
	orl	ar7,a
	inc	r0
	mov	a,@r0
	orl	ar6,a
	inc	r0
	mov	a,@r0
	orl	ar5,a
	pop	ar0
	mov	@r0,ar4
	inc	r0
	mov	@r0,ar7
	inc	r0
	mov	@r0,ar6
	inc	r0
	mov	@r0,ar5
	dec	r0
	dec	r0
	dec	r0
;	tea.c:19: for(j = 0; j < 4; j++)
	mov	a,_bp
	add	a,#0x36
	mov	r0,a
	inc	@r0
	cjne	@r0,#0x00,00161$
	inc	r0
	inc	@r0
	cjne	@r0,#0x00,00161$
	inc	r0
	inc	@r0
	cjne	@r0,#0x00,00161$
	inc	r0
	inc	@r0
00161$:
	mov	a,_bp
	add	a,#0x36
	mov	r0,a
	clr	c
	mov	a,@r0
	subb	a,#0x04
	inc	r0
	mov	a,@r0
	subb	a,#0x00
	inc	r0
	mov	a,@r0
	subb	a,#0x00
	inc	r0
	mov	a,@r0
	subb	a,#0x00
	pop	ar1
	jnc	00162$
	ljmp	00107$
00162$:
;	tea.c:17: for(i = 0; i < 2; i++){
	mov	a,_bp
	add	a,#0x32
	mov	r0,a
	inc	@r0
	cjne	@r0,#0x00,00163$
	inc	r0
	inc	@r0
	cjne	@r0,#0x00,00163$
	inc	r0
	inc	@r0
	cjne	@r0,#0x00,00163$
	inc	r0
	inc	@r0
00163$:
	mov	a,_bp
	add	a,#0x32
	mov	r0,a
	clr	c
	mov	a,@r0
	subb	a,#0x02
	inc	r0
	mov	a,@r0
	subb	a,#0x00
	inc	r0
	mov	a,@r0
	subb	a,#0x00
	inc	r0
	mov	a,@r0
	subb	a,#0x00
	jnc	00164$
	ljmp	00109$
00164$:
;	tea.c:23: for(i = 0; i < 4; i++){
	mov	a,_bp
	add	a,#0x1e
	mov	r0,a
	push	ar0
	mov	a,_bp
	add	a,#0x32
	mov	r0,a
	clr	a
	mov	@r0,a
	inc	r0
	mov	@r0,a
	inc	r0
	mov	@r0,a
	inc	r0
	mov	@r0,a
	pop	ar0
00113$:
;	tea.c:24: kp[i] = 0;
	push	ar1
	push	ar0
	mov	a,_bp
	add	a,#0x32
	mov	r0,a
	mov	a,@r0
	add	a,acc
	mov	r2,a
	inc	r0
	mov	a,@r0
	rlc	a
	mov	r3,a
	inc	r0
	mov	a,@r0
	rlc	a
	mov	r6,a
	inc	r0
	mov	a,@r0
	rlc	a
	mov	r7,a
	mov	a,r2
	add	a,r2
	mov	r2,a
	mov	a,r3
	rlc	a
	mov	r3,a
	mov	a,r6
	rlc	a
	mov	r6,a
	mov	a,r7
	rlc	a
	mov	r7,a
	pop	ar0
	mov	a,r2
	add	a,r0
	push	ar0
	mov	r0,a
	mov	@r0,#0x00
	inc	r0
	mov	@r0,#0x00
	inc	r0
	mov	@r0,#0x00
	inc	r0
	mov	@r0,#0x00
;	tea.c:25: for(j = 0; j < 4; j++)
	mov	a,_bp
	add	a,#0x08
	mov	r0,a
	mov	@r0,ar2
	inc	r0
	mov	@r0,ar3
	inc	r0
	mov	@r0,ar6
	inc	r0
	mov	@r0,ar7
	mov	a,_bp
	add	a,#0x36
	mov	r0,a
	clr	a
	mov	@r0,a
	inc	r0
	mov	@r0,a
	inc	r0
	mov	@r0,a
	inc	r0
	mov	@r0,a
	pop	ar0
;	tea.c:48: for(i = 0; i < 8; i++)
	pop	ar1
;	tea.c:25: for(j = 0; j < 4; j++)
00111$:
;	tea.c:26: kp[i] = (kp[i] << 8) | k[4*i + j]; 
	push	ar1
	push	ar1
	mov	a,_bp
	add	a,#0x08
	mov	r1,a
	mov	a,@r1
	add	a,r0
	mov	r7,a
	pop	ar1
	push	ar0
	mov	r0,ar7
	mov	ar3,@r0
	inc	r0
	mov	ar4,@r0
	inc	r0
	mov	ar5,@r0
	inc	r0
	mov	ar6,@r0
	mov	a,_bp
	add	a,#0x04
	mov	r0,a
	inc	r0
	inc	r0
	inc	r0
	mov	@r0,ar5
	dec	r0
	mov	@r0,ar4
	dec	r0
	mov	@r0,ar3
	dec	r0
	mov	@r0,#0x00
	mov	a,_bp
	add	a,#0x08
	mov	r0,a
	push	ar1
	mov	a,_bp
	add	a,#0x36
	mov	r1,a
	mov	a,@r1
	add	a,@r0
	mov	r2,a
	inc	r1
	mov	a,@r1
	inc	r0
	addc	a,@r0
	mov	r4,a
	inc	r1
	mov	a,@r1
	inc	r0
	addc	a,@r0
	inc	r1
	mov	a,@r1
	inc	r0
	addc	a,@r0
	pop	ar1
	mov	a,_bp
	add	a,#0xfb
	mov	r0,a
	mov	a,r2
	add	a,@r0
	mov	r2,a
	mov	a,r4
	inc	r0
	addc	a,@r0
	mov	r4,a
	inc	r0
	mov	ar5,@r0
	mov	dpl,r2
	mov	dph,r4
	mov	b,r5
	lcall	__gptrget
	mov	r2,a
	clr	a
	mov	r6,a
	mov	r5,a
	mov	r4,a
	mov	a,_bp
	add	a,#0x04
	mov	r0,a
	mov	a,@r0
	orl	ar2,a
	inc	r0
	mov	a,@r0
	orl	ar6,a
	inc	r0
	mov	a,@r0
	orl	ar5,a
	inc	r0
	mov	a,@r0
	orl	ar4,a
	mov	r0,ar7
	mov	@r0,ar2
	inc	r0
	mov	@r0,ar6
	inc	r0
	mov	@r0,ar5
	inc	r0
	mov	@r0,ar4
;	tea.c:25: for(j = 0; j < 4; j++)
	mov	a,_bp
	add	a,#0x36
	mov	r0,a
	inc	@r0
	cjne	@r0,#0x00,00165$
	inc	r0
	inc	@r0
	cjne	@r0,#0x00,00165$
	inc	r0
	inc	@r0
	cjne	@r0,#0x00,00165$
	inc	r0
	inc	@r0
00165$:
	mov	a,_bp
	add	a,#0x36
	mov	r0,a
	clr	c
	mov	a,@r0
	subb	a,#0x04
	inc	r0
	mov	a,@r0
	subb	a,#0x00
	inc	r0
	mov	a,@r0
	subb	a,#0x00
	inc	r0
	mov	a,@r0
	subb	a,#0x00
	pop	ar0
	pop	ar1
	jnc	00166$
	ljmp	00111$
00166$:
;	tea.c:23: for(i = 0; i < 4; i++){
	push	ar0
	mov	a,_bp
	add	a,#0x32
	mov	r0,a
	inc	@r0
	cjne	@r0,#0x00,00167$
	inc	r0
	inc	@r0
	cjne	@r0,#0x00,00167$
	inc	r0
	inc	@r0
	cjne	@r0,#0x00,00167$
	inc	r0
	inc	@r0
00167$:
	mov	a,_bp
	add	a,#0x32
	mov	r0,a
	clr	c
	mov	a,@r0
	subb	a,#0x04
	inc	r0
	mov	a,@r0
	subb	a,#0x00
	inc	r0
	mov	a,@r0
	subb	a,#0x00
	inc	r0
	mov	a,@r0
	subb	a,#0x00
	pop	ar0
	jnc	00168$
	ljmp	00113$
00168$:
;	tea.c:29: for (i=0; i < 32; i++) {                       /* basic cycle start */
	push	ar1
	mov	a,_bp
	add	a,#0x0c
	mov	r1,a
	mov	a,#0x04
	add	a,r0
	mov	@r1,a
	mov	a,_bp
	add	a,#0x04
	mov	r1,a
	mov	a,#0x08
	add	a,r0
	mov	@r1,a
	mov	a,_bp
	add	a,#0x08
	mov	r1,a
	mov	a,#0x0c
	add	a,r0
	mov	@r1,a
	pop	ar1
	push	ar0
	mov	a,_bp
	add	a,#0x2e
	mov	r0,a
	clr	a
	mov	@r0,a
	inc	r0
	mov	@r0,a
	inc	r0
	mov	@r0,a
	inc	r0
	mov	@r0,a
	mov	a,_bp
	add	a,#0x32
	mov	r0,a
	mov	@r0,#0x20
	clr	a
	inc	r0
	mov	@r0,a
	inc	r0
	mov	@r0,a
	inc	r0
	mov	@r0,a
	pop	ar0
00117$:
;	tea.c:32: sum += delta;
	push	ar0
	mov	a,_bp
	add	a,#0x2e
	mov	r0,a
	mov	a,#0xb9
	add	a,@r0
	mov	@r0,a
	mov	a,#0x79
	inc	r0
	addc	a,@r0
	mov	@r0,a
	mov	a,#0x37
	inc	r0
	addc	a,@r0
	mov	@r0,a
	mov	a,#0x9e
	inc	r0
	addc	a,@r0
	mov	@r0,a
;	tea.c:35: a = (vp[1] << 4) + kp[0];
	mov	a,_bp
	add	a,#0x0d
	mov	r0,a
	mov	a,#0x04
	add	a,r1
	mov	@r0,a
	mov	a,_bp
	add	a,#0x0d
	mov	r0,a
	push	ar1
	mov	a,_bp
	add	a,#0x0e
	mov	r1,a
	mov	ar0,@r0
	mov	a,@r0
	mov	@r1,a
	inc	r0
	mov	a,@r0
	inc	r1
	mov	@r1,a
	inc	r0
	mov	a,@r0
	inc	r1
	mov	@r1,a
	inc	r0
	mov	a,@r0
	inc	r1
	mov	@r1,a
	dec	r0
	dec	r0
	dec	r0
	mov	a,_bp
	add	a,#0x0e
	mov	r0,a
	mov	a,_bp
	add	a,#0x12
	mov	r1,a
	inc	r0
	inc	r0
	mov	a,@r0
	inc	r1
	inc	r1
	mov	@r1,a
	inc	r0
	mov	a,@r0
	swap	a
	anl	a,#0xf0
	xch	a,@r1
	swap	a
	xch	a,@r1
	xrl	a,@r1
	xch	a,@r1
	anl	a,#0xf0
	xch	a,@r1
	xrl	a,@r1
	inc	r1
	mov	@r1,a
	dec	r0
	dec	r0
	mov	a,@r0
	swap	a
	anl	a,#0x0f
	dec	r1
	orl	a,@r1
	mov	@r1,a
	dec	r0
	mov	a,@r0
	dec	r1
	dec	r1
	mov	@r1,a
	inc	r0
	mov	a,@r0
	swap	a
	anl	a,#0xf0
	xch	a,@r1
	swap	a
	xch	a,@r1
	xrl	a,@r1
	xch	a,@r1
	anl	a,#0xf0
	xch	a,@r1
	xrl	a,@r1
	inc	r1
	mov	@r1,a
	pop	ar1
	pop	ar0
	mov	ar2,@r0
	inc	r0
	mov	ar3,@r0
	inc	r0
	mov	ar6,@r0
	inc	r0
	mov	ar7,@r0
	dec	r0
	dec	r0
	dec	r0
	push	ar0
	mov	a,_bp
	add	a,#0x12
	mov	r0,a
	mov	a,r2
	add	a,@r0
	mov	r2,a
	mov	a,r3
	inc	r0
	addc	a,@r0
	mov	r3,a
	mov	a,r6
	inc	r0
	addc	a,@r0
	mov	r6,a
	mov	a,r7
	inc	r0
	addc	a,@r0
	mov	r7,a
	mov	a,_bp
	add	a,#0x3a
	mov	r0,a
	mov	@r0,ar2
	inc	r0
	mov	@r0,ar3
	inc	r0
	mov	@r0,ar6
	inc	r0
	mov	@r0,ar7
;	tea.c:36: b = (vp[1] + sum);
	mov	a,_bp
	add	a,#0x0e
	mov	r0,a
	push	ar1
	mov	a,_bp
	add	a,#0x2e
	mov	r1,a
	mov	a,@r1
	add	a,@r0
	mov	r4,a
	inc	r1
	mov	a,@r1
	inc	r0
	addc	a,@r0
	mov	r5,a
	inc	r1
	mov	a,@r1
	inc	r0
	addc	a,@r0
	mov	r6,a
	inc	r1
	mov	a,@r1
	inc	r0
	addc	a,@r0
	mov	r7,a
	pop	ar1
	pop	ar0
	push	ar0
	push	ar0
	mov	a,_bp
	add	a,#0x3e
	mov	r0,a
	mov	@r0,ar4
	inc	r0
	mov	@r0,ar5
	inc	r0
	mov	@r0,ar6
	inc	r0
	mov	@r0,ar7
;	tea.c:37: c = (vp[1] >> 5) + kp[1];
	mov	a,_bp
	add	a,#0x0e
	mov	r0,a
	push	ar1
	mov	a,_bp
	add	a,#0x12
	mov	r1,a
	mov	a,@r0
	mov	@r1,a
	inc	r0
	mov	a,@r0
	swap	a
	rr	a
	xch	a,@r1
	swap	a
	rr	a
	anl	a,#0x07
	xrl	a,@r1
	xch	a,@r1
	anl	a,#0x07
	xch	a,@r1
	xrl	a,@r1
	xch	a,@r1
	inc	r1
	mov	@r1,a
	inc	r0
	mov	a,@r0
	swap	a
	rr	a
	anl	a,#0xf8
	orl	a,@r1
	mov	@r1,a
	mov	a,@r0
	inc	r1
	mov	@r1,a
	inc	r0
	mov	a,@r0
	swap	a
	rr	a
	xch	a,@r1
	swap	a
	rr	a
	anl	a,#0x07
	xrl	a,@r1
	xch	a,@r1
	anl	a,#0x07
	xch	a,@r1
	xrl	a,@r1
	xch	a,@r1
	inc	r1
	mov	@r1,a
	pop	ar1
	mov	a,_bp
	add	a,#0x0c
	mov	r0,a
	mov	ar0,@r0
	mov	ar4,@r0
	inc	r0
	mov	ar5,@r0
	inc	r0
	mov	ar6,@r0
	inc	r0
	mov	ar7,@r0
	dec	r0
	dec	r0
	dec	r0
	mov	a,_bp
	add	a,#0x12
	mov	r0,a
	mov	a,r4
	add	a,@r0
	mov	r4,a
	mov	a,r5
	inc	r0
	addc	a,@r0
	mov	r5,a
	mov	a,r6
	inc	r0
	addc	a,@r0
	mov	r6,a
	mov	a,r7
	inc	r0
	addc	a,@r0
	mov	r7,a
	mov	a,_bp
	add	a,#0x42
	mov	r0,a
	mov	@r0,ar4
	inc	r0
	mov	@r0,ar5
	inc	r0
	mov	@r0,ar6
	inc	r0
	mov	@r0,ar7
;	tea.c:38: vp[0] += (a ^ b ^ c);
	mov	a,_bp
	add	a,#0x12
	mov	r0,a
	mov	a,@r1
	mov	@r0,a
	inc	r1
	mov	a,@r1
	inc	r0
	mov	@r0,a
	inc	r1
	mov	a,@r1
	inc	r0
	mov	@r0,a
	inc	r1
	mov	a,@r1
	inc	r0
	mov	@r0,a
	dec	r1
	dec	r1
	dec	r1
	mov	a,_bp
	add	a,#0x3a
	mov	r0,a
	push	ar1
	mov	a,_bp
	add	a,#0x3e
	mov	r1,a
	mov	a,@r1
	xrl	a,@r0
	mov	r4,a
	inc	r1
	mov	a,@r1
	inc	r0
	xrl	a,@r0
	mov	r5,a
	inc	r1
	mov	a,@r1
	inc	r0
	xrl	a,@r0
	mov	r6,a
	inc	r1
	mov	a,@r1
	inc	r0
	xrl	a,@r0
	mov	r7,a
	pop	ar1
	mov	a,_bp
	add	a,#0x42
	mov	r0,a
	mov	a,@r0
	xrl	ar4,a
	inc	r0
	mov	a,@r0
	xrl	ar5,a
	inc	r0
	mov	a,@r0
	xrl	ar6,a
	inc	r0
	mov	a,@r0
	xrl	ar7,a
	mov	a,_bp
	add	a,#0x12
	mov	r0,a
	mov	a,r4
	add	a,@r0
	mov	@r0,a
	mov	a,r5
	inc	r0
	addc	a,@r0
	mov	@r0,a
	mov	a,r6
	inc	r0
	addc	a,@r0
	mov	@r0,a
	mov	a,r7
	inc	r0
	addc	a,@r0
	mov	@r0,a
	mov	a,_bp
	add	a,#0x12
	mov	r0,a
	mov	a,@r0
	mov	@r1,a
	inc	r1
	inc	r0
	mov	a,@r0
	mov	@r1,a
	inc	r1
	inc	r0
	mov	a,@r0
	mov	@r1,a
	inc	r1
	inc	r0
	mov	a,@r0
	mov	@r1,a
	dec	r1
	dec	r1
	dec	r1
;	tea.c:41: a = (vp[0] << 4) + kp[2];
	mov	a,_bp
	add	a,#0x12
	mov	r0,a
	push	ar1
	mov	a,_bp
	add	a,#0x0e
	mov	r1,a
	inc	r0
	inc	r0
	mov	a,@r0
	inc	r1
	inc	r1
	mov	@r1,a
	inc	r0
	mov	a,@r0
	swap	a
	anl	a,#0xf0
	xch	a,@r1
	swap	a
	xch	a,@r1
	xrl	a,@r1
	xch	a,@r1
	anl	a,#0xf0
	xch	a,@r1
	xrl	a,@r1
	inc	r1
	mov	@r1,a
	dec	r0
	dec	r0
	mov	a,@r0
	swap	a
	anl	a,#0x0f
	dec	r1
	orl	a,@r1
	mov	@r1,a
	dec	r0
	mov	a,@r0
	dec	r1
	dec	r1
	mov	@r1,a
	inc	r0
	mov	a,@r0
	swap	a
	anl	a,#0xf0
	xch	a,@r1
	swap	a
	xch	a,@r1
	xrl	a,@r1
	xch	a,@r1
	anl	a,#0xf0
	xch	a,@r1
	xrl	a,@r1
	inc	r1
	mov	@r1,a
	mov	a,_bp
	add	a,#0x04
	mov	r0,a
	mov	ar0,@r0
	mov	ar4,@r0
	inc	r0
	mov	ar5,@r0
	inc	r0
	mov	ar6,@r0
	inc	r0
	mov	ar7,@r0
	dec	r0
	dec	r0
	dec	r0
	mov	a,_bp
	add	a,#0x0e
	mov	r0,a
	mov	a,r4
	add	a,@r0
	mov	r4,a
	mov	a,r5
	inc	r0
	addc	a,@r0
	mov	r5,a
	mov	a,r6
	inc	r0
	addc	a,@r0
	mov	r6,a
	mov	a,r7
	inc	r0
	addc	a,@r0
	mov	r7,a
	mov	a,_bp
	add	a,#0x3a
	mov	r0,a
	mov	@r0,ar4
	inc	r0
	mov	@r0,ar5
	inc	r0
	mov	@r0,ar6
	inc	r0
	mov	@r0,ar7
;	tea.c:42: b = (vp[0] + sum);
	mov	a,_bp
	add	a,#0x12
	mov	r0,a
	mov	a,_bp
	add	a,#0x2e
	mov	r1,a
	mov	a,@r1
	add	a,@r0
	mov	r4,a
	inc	r1
	mov	a,@r1
	inc	r0
	addc	a,@r0
	mov	r5,a
	inc	r1
	mov	a,@r1
	inc	r0
	addc	a,@r0
	mov	r6,a
	inc	r1
	mov	a,@r1
	inc	r0
	addc	a,@r0
	mov	r7,a
	mov	a,_bp
	add	a,#0x3e
	mov	r0,a
	mov	@r0,ar4
	inc	r0
	mov	@r0,ar5
	inc	r0
	mov	@r0,ar6
	inc	r0
	mov	@r0,ar7
;	tea.c:43: c = (vp[0] >> 5) + kp[3];
	mov	a,_bp
	add	a,#0x12
	mov	r0,a
	inc	r0
	mov	a,@r0
	dec	r0
	swap	a
	rr	a
	xch	a,@r0
	swap	a
	rr	a
	anl	a,#0x07
	xrl	a,@r0
	xch	a,@r0
	anl	a,#0x07
	xch	a,@r0
	xrl	a,@r0
	xch	a,@r0
	inc	r0
	mov	@r0,a
	inc	r0
	mov	a,@r0
	swap	a
	rr	a
	anl	a,#0xf8
	dec	r0
	orl	a,@r0
	mov	@r0,a
	inc	r0
	inc	r0
	mov	a,@r0
	dec	r0
	swap	a
	rr	a
	xch	a,@r0
	swap	a
	rr	a
	anl	a,#0x07
	xrl	a,@r0
	xch	a,@r0
	anl	a,#0x07
	xch	a,@r0
	xrl	a,@r0
	xch	a,@r0
	inc	r0
	mov	@r0,a
	mov	a,_bp
	add	a,#0x08
	mov	r0,a
	mov	ar0,@r0
	mov	ar2,@r0
	inc	r0
	mov	ar3,@r0
	inc	r0
	mov	ar6,@r0
	inc	r0
	mov	ar7,@r0
	dec	r0
	dec	r0
	dec	r0
	mov	a,_bp
	add	a,#0x12
	mov	r0,a
	mov	a,r2
	add	a,@r0
	mov	r2,a
	mov	a,r3
	inc	r0
	addc	a,@r0
	mov	r3,a
	mov	a,r6
	inc	r0
	addc	a,@r0
	mov	r6,a
	mov	a,r7
	inc	r0
	addc	a,@r0
	mov	r7,a
	mov	a,_bp
	add	a,#0x42
	mov	r0,a
	mov	@r0,ar2
	inc	r0
	mov	@r0,ar3
	inc	r0
	mov	@r0,ar6
	inc	r0
	mov	@r0,ar7
;	tea.c:44: vp[1] += (a ^ b ^ c);
	mov	a,_bp
	add	a,#0x0d
	mov	r0,a
	mov	a,_bp
	add	a,#0x12
	mov	r1,a
	mov	ar0,@r0
	mov	a,@r0
	mov	@r1,a
	inc	r0
	mov	a,@r0
	inc	r1
	mov	@r1,a
	inc	r0
	mov	a,@r0
	inc	r1
	mov	@r1,a
	inc	r0
	mov	a,@r0
	inc	r1
	mov	@r1,a
	dec	r0
	dec	r0
	dec	r0
	mov	a,_bp
	add	a,#0x3a
	mov	r0,a
	mov	a,_bp
	add	a,#0x3e
	mov	r1,a
	mov	a,@r1
	xrl	a,@r0
	mov	r2,a
	inc	r1
	mov	a,@r1
	inc	r0
	xrl	a,@r0
	mov	r3,a
	inc	r1
	mov	a,@r1
	inc	r0
	xrl	a,@r0
	mov	r6,a
	inc	r1
	mov	a,@r1
	inc	r0
	xrl	a,@r0
	mov	r7,a
	pop	ar1
	mov	a,_bp
	add	a,#0x42
	mov	r0,a
	mov	a,@r0
	xrl	ar2,a
	inc	r0
	mov	a,@r0
	xrl	ar3,a
	inc	r0
	mov	a,@r0
	xrl	ar6,a
	inc	r0
	mov	a,@r0
	xrl	ar7,a
	mov	a,_bp
	add	a,#0x12
	mov	r0,a
	mov	a,r2
	add	a,@r0
	mov	r2,a
	mov	a,r3
	inc	r0
	addc	a,@r0
	mov	r3,a
	mov	a,r6
	inc	r0
	addc	a,@r0
	mov	r6,a
	mov	a,r7
	inc	r0
	addc	a,@r0
	mov	r7,a
	mov	a,_bp
	add	a,#0x0d
	mov	r0,a
	mov	ar0,@r0
	mov	@r0,ar2
	inc	r0
	mov	@r0,ar3
	inc	r0
	mov	@r0,ar6
	inc	r0
	mov	@r0,ar7
	dec	r0
	dec	r0
	dec	r0
	mov	a,_bp
	add	a,#0x32
	mov	r0,a
	dec	@r0
	cjne	@r0,#0xff,00169$
	inc	r0
	dec	@r0
	cjne	@r0,#0xff,00169$
	inc	r0
	dec	@r0
	cjne	@r0,#0xff,00169$
	inc	r0
	dec	@r0
00169$:
	pop	ar0
;	tea.c:29: for (i=0; i < 32; i++) {                       /* basic cycle start */
	mov	a,_bp
	add	a,#0x32
	mov	r0,a
	mov	a,@r0
	inc	r0
	orl	a,@r0
	inc	r0
	orl	a,@r0
	inc	r0
	orl	a,@r0
	pop	ar0
	jz	00170$
	ljmp	00117$
00170$:
;	tea.c:48: for(i = 0; i < 8; i++)
	mov	a,_bp
	add	a,#0x32
	mov	r0,a
	clr	a
	mov	@r0,a
	inc	r0
	mov	@r0,a
	inc	r0
	mov	@r0,a
	inc	r0
	mov	@r0,a
00118$:
;	tea.c:50: uint8_t shift = 24 - 8 * (i%4);
	mov	a,_bp
	add	a,#0x32
	mov	r0,a
	mov	a,#0x03
	anl	a,@r0
	mov	r2,a
	clr	a
	mov	a,r2
	swap	a
	rr	a
	anl	a,#0xf8
	mov	r2,a
	mov	a,_bp
	add	a,#0x3a
	mov	r0,a
	mov	a,#0x18
	clr	c
	subb	a,r2
	mov	@r0,a
;	tea.c:51: v[i] = (vp[i/4] >> shift) & 0xff;
	mov	r0,_bp
	inc	r0
	push	ar1
	mov	a,_bp
	add	a,#0x32
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
	add	a,#0x14
	mov	r0,a
	pop	acc
	mov	@r0,a
	dec	r0
	pop	acc
	mov	@r0,a
	dec	r0
	pop	acc
	mov	@r0,a
	pop	ar1
	mov	a,_bp
	add	a,#0x32
	mov	r0,a
	inc	r0
	inc	r0
	inc	r0
	mov	a,@r0
	clr	c
	rrc	a
	mov	r7,a
	dec	r0
	mov	a,@r0
	rrc	a
	mov	r4,a
	dec	r0
	mov	a,@r0
	rrc	a
	mov	r3,a
	dec	r0
	mov	a,@r0
	rrc	a
	mov	r2,a
	mov	a,r7
	clr	c
	rrc	a
	mov	r7,a
	mov	a,r4
	rrc	a
	mov	r4,a
	mov	a,r3
	rrc	a
	mov	r3,a
	mov	a,r2
	rrc	a
	add	a,acc
	mov	r2,a
	mov	a,r3
	rlc	a
	mov	r3,a
	mov	a,r4
	rlc	a
	mov	r4,a
	mov	a,r7
	rlc	a
	mov	r7,a
	mov	a,r2
	add	a,r2
	mov	r2,a
	mov	a,r3
	rlc	a
	mov	r3,a
	mov	a,r4
	rlc	a
	mov	a,r7
	rlc	a
	mov	a,r2
	add	a,r1
	mov	r7,a
	mov	r0,ar7
	mov	ar4,@r0
	inc	r0
	mov	ar5,@r0
	inc	r0
	mov	ar6,@r0
	inc	r0
	mov	ar7,@r0
	mov	a,_bp
	add	a,#0x3a
	mov	r0,a
	mov	b,@r0
	inc	b
	sjmp	00172$
00171$:
	clr	c
	mov	a,r7
	rrc	a
	mov	r7,a
	mov	a,r6
	rrc	a
	mov	r6,a
	mov	a,r5
	rrc	a
	mov	r5,a
	mov	a,r4
	rrc	a
	mov	r4,a
00172$:
	djnz	b,00171$
	clr	a
	mov	r5,a
	mov	r6,a
	mov	r7,a
	mov	a,_bp
	add	a,#0x12
	mov	r0,a
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	mov	a,r4
	lcall	__gptrput
;	tea.c:48: for(i = 0; i < 8; i++)
	mov	a,_bp
	add	a,#0x32
	mov	r0,a
	inc	@r0
	cjne	@r0,#0x00,00173$
	inc	r0
	inc	@r0
	cjne	@r0,#0x00,00173$
	inc	r0
	inc	@r0
	cjne	@r0,#0x00,00173$
	inc	r0
	inc	@r0
00173$:
	mov	a,_bp
	add	a,#0x32
	mov	r0,a
	clr	c
	mov	a,@r0
	subb	a,#0x08
	inc	r0
	mov	a,@r0
	subb	a,#0x00
	inc	r0
	mov	a,@r0
	subb	a,#0x00
	inc	r0
	mov	a,@r0
	subb	a,#0x00
	jnc	00174$
	ljmp	00118$
00174$:
	mov	sp,_bp
	pop	_bp
	ret
	.area CSEG    (CODE)
	.area CONST   (CODE)
	.area XINIT   (CODE)
	.area CABS    (ABS,CODE)
