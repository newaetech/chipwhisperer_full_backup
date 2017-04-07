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
	.globl _simpleserial_put
	.globl _simpleserial_get
	.globl _TF2
	.globl _EXF2
	.globl _RCLK
	.globl _TCLK
	.globl _EXEN2
	.globl _TR2
	.globl _C_T2
	.globl _CP_RL2
	.globl _T2CON_7
	.globl _T2CON_6
	.globl _T2CON_5
	.globl _T2CON_4
	.globl _T2CON_3
	.globl _T2CON_2
	.globl _T2CON_1
	.globl _T2CON_0
	.globl _PT2
	.globl _ET2
	.globl _CY
	.globl _AC
	.globl _F0
	.globl _RS1
	.globl _RS0
	.globl _OV
	.globl _F1
	.globl _P
	.globl _PS
	.globl _PT1
	.globl _PX1
	.globl _PT0
	.globl _PX0
	.globl _RD
	.globl _WR
	.globl _T1
	.globl _T0
	.globl _INT1
	.globl _INT0
	.globl _TXD
	.globl _RXD
	.globl _P3_7
	.globl _P3_6
	.globl _P3_5
	.globl _P3_4
	.globl _P3_3
	.globl _P3_2
	.globl _P3_1
	.globl _P3_0
	.globl _EA
	.globl _ES
	.globl _ET1
	.globl _EX1
	.globl _ET0
	.globl _EX0
	.globl _P2_7
	.globl _P2_6
	.globl _P2_5
	.globl _P2_4
	.globl _P2_3
	.globl _P2_2
	.globl _P2_1
	.globl _P2_0
	.globl _SM0
	.globl _SM1
	.globl _SM2
	.globl _REN
	.globl _TB8
	.globl _RB8
	.globl _TI
	.globl _RI
	.globl _P1_7
	.globl _P1_6
	.globl _P1_5
	.globl _P1_4
	.globl _P1_3
	.globl _P1_2
	.globl _P1_1
	.globl _P1_0
	.globl _TF1
	.globl _TR1
	.globl _TF0
	.globl _TR0
	.globl _IE1
	.globl _IT1
	.globl _IE0
	.globl _IT0
	.globl _P0_7
	.globl _P0_6
	.globl _P0_5
	.globl _P0_4
	.globl _P0_3
	.globl _P0_2
	.globl _P0_1
	.globl _P0_0
	.globl _TH2
	.globl _TL2
	.globl _RCAP2H
	.globl _RCAP2L
	.globl _T2CON
	.globl _B
	.globl _ACC
	.globl _PSW
	.globl _IP
	.globl _P3
	.globl _IE
	.globl _P2
	.globl _SBUF
	.globl _SCON
	.globl _P1
	.globl _TH1
	.globl _TH0
	.globl _TL1
	.globl _TL0
	.globl _TMOD
	.globl _TCON
	.globl _PCON
	.globl _DPH
	.globl _DPL
	.globl _SP
	.globl _P0
	.globl _main_tea
;--------------------------------------------------------
; special function registers
;--------------------------------------------------------
	.area RSEG    (ABS,DATA)
	.org 0x0000
_P0	=	0x0080
_SP	=	0x0081
_DPL	=	0x0082
_DPH	=	0x0083
_PCON	=	0x0087
_TCON	=	0x0088
_TMOD	=	0x0089
_TL0	=	0x008a
_TL1	=	0x008b
_TH0	=	0x008c
_TH1	=	0x008d
_P1	=	0x0090
_SCON	=	0x0098
_SBUF	=	0x0099
_P2	=	0x00a0
_IE	=	0x00a8
_P3	=	0x00b0
_IP	=	0x00b8
_PSW	=	0x00d0
_ACC	=	0x00e0
_B	=	0x00f0
_T2CON	=	0x00c8
_RCAP2L	=	0x00ca
_RCAP2H	=	0x00cb
_TL2	=	0x00cc
_TH2	=	0x00cd
;--------------------------------------------------------
; special function bits
;--------------------------------------------------------
	.area RSEG    (ABS,DATA)
	.org 0x0000
_P0_0	=	0x0080
_P0_1	=	0x0081
_P0_2	=	0x0082
_P0_3	=	0x0083
_P0_4	=	0x0084
_P0_5	=	0x0085
_P0_6	=	0x0086
_P0_7	=	0x0087
_IT0	=	0x0088
_IE0	=	0x0089
_IT1	=	0x008a
_IE1	=	0x008b
_TR0	=	0x008c
_TF0	=	0x008d
_TR1	=	0x008e
_TF1	=	0x008f
_P1_0	=	0x0090
_P1_1	=	0x0091
_P1_2	=	0x0092
_P1_3	=	0x0093
_P1_4	=	0x0094
_P1_5	=	0x0095
_P1_6	=	0x0096
_P1_7	=	0x0097
_RI	=	0x0098
_TI	=	0x0099
_RB8	=	0x009a
_TB8	=	0x009b
_REN	=	0x009c
_SM2	=	0x009d
_SM1	=	0x009e
_SM0	=	0x009f
_P2_0	=	0x00a0
_P2_1	=	0x00a1
_P2_2	=	0x00a2
_P2_3	=	0x00a3
_P2_4	=	0x00a4
_P2_5	=	0x00a5
_P2_6	=	0x00a6
_P2_7	=	0x00a7
_EX0	=	0x00a8
_ET0	=	0x00a9
_EX1	=	0x00aa
_ET1	=	0x00ab
_ES	=	0x00ac
_EA	=	0x00af
_P3_0	=	0x00b0
_P3_1	=	0x00b1
_P3_2	=	0x00b2
_P3_3	=	0x00b3
_P3_4	=	0x00b4
_P3_5	=	0x00b5
_P3_6	=	0x00b6
_P3_7	=	0x00b7
_RXD	=	0x00b0
_TXD	=	0x00b1
_INT0	=	0x00b2
_INT1	=	0x00b3
_T0	=	0x00b4
_T1	=	0x00b5
_WR	=	0x00b6
_RD	=	0x00b7
_PX0	=	0x00b8
_PT0	=	0x00b9
_PX1	=	0x00ba
_PT1	=	0x00bb
_PS	=	0x00bc
_P	=	0x00d0
_F1	=	0x00d1
_OV	=	0x00d2
_RS0	=	0x00d3
_RS1	=	0x00d4
_F0	=	0x00d5
_AC	=	0x00d6
_CY	=	0x00d7
_ET2	=	0x00ad
_PT2	=	0x00bd
_T2CON_0	=	0x00c8
_T2CON_1	=	0x00c9
_T2CON_2	=	0x00ca
_T2CON_3	=	0x00cb
_T2CON_4	=	0x00cc
_T2CON_5	=	0x00cd
_T2CON_6	=	0x00ce
_T2CON_7	=	0x00cf
_CP_RL2	=	0x00c8
_C_T2	=	0x00c9
_TR2	=	0x00ca
_EXEN2	=	0x00cb
_TCLK	=	0x00cc
_RCLK	=	0x00cd
_EXF2	=	0x00ce
_TF2	=	0x00cf
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
;	tea.c:21: void tea_encrypt(uint8_t* v, uint8_t* k)
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
;	tea.c:30: for(i = 0; i < 2; i++){
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
;	tea.c:31: vp[i] = 0;
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
;	tea.c:32: for(j = 0; j < 4; j++)
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
;	tea.c:61: for(i = 0; i < 8; i++)
;	tea.c:32: for(j = 0; j < 4; j++)
00107$:
;	tea.c:33: vp[i] = (vp[i] << 8) | v[4*i + j]; 
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
;	tea.c:32: for(j = 0; j < 4; j++)
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
;	tea.c:30: for(i = 0; i < 2; i++){
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
;	tea.c:36: for(i = 0; i < 4; i++){
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
;	tea.c:37: kp[i] = 0;
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
;	tea.c:38: for(j = 0; j < 4; j++)
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
;	tea.c:61: for(i = 0; i < 8; i++)
	pop	ar1
;	tea.c:38: for(j = 0; j < 4; j++)
00111$:
;	tea.c:39: kp[i] = (kp[i] << 8) | k[4*i + j]; 
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
;	tea.c:38: for(j = 0; j < 4; j++)
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
;	tea.c:36: for(i = 0; i < 4; i++){
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
;	tea.c:42: for (i=0; i < 32; i++) {                       /* basic cycle start */
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
;	tea.c:45: sum += delta;
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
;	tea.c:48: a = (vp[1] << 4) + kp[0];
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
;	tea.c:49: b = (vp[1] + sum);
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
;	tea.c:50: c = (vp[1] >> 5) + kp[1];
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
;	tea.c:51: vp[0] += (a ^ b ^ c);
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
;	tea.c:54: a = (vp[0] << 4) + kp[2];
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
;	tea.c:55: b = (vp[0] + sum);
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
;	tea.c:56: c = (vp[0] >> 5) + kp[3];
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
;	tea.c:57: vp[1] += (a ^ b ^ c);
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
;	tea.c:42: for (i=0; i < 32; i++) {                       /* basic cycle start */
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
;	tea.c:61: for(i = 0; i < 8; i++)
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
;	tea.c:63: uint8_t shift = 24 - 8 * (i%4);
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
;	tea.c:64: v[i] = (vp[i/4] >> shift) & 0xff;
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
;	tea.c:61: for(i = 0; i < 8; i++)
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
;------------------------------------------------------------
;Allocation info for local variables in function 'main_tea'
;------------------------------------------------------------
;input                     Allocated to stack - _bp +4
;key                       Allocated to stack - _bp +12
;sloc0                     Allocated to stack - _bp +1
;------------------------------------------------------------
;	tea.c:68: void main_tea(void) 
;	-----------------------------------------
;	 function main_tea
;	-----------------------------------------
_main_tea:
	push	_bp
	mov	a,sp
	mov	_bp,a
	add	a,#0x1b
	mov	sp,a
;	tea.c:75: while(1)
00104$:
;	tea.c:78: if(simpleserial_get(input, key, TEA_BLOCK_SIZE, TEA_KEY_SIZE))
	mov	a,_bp
	add	a,#0x0c
	mov	r7,a
	mov	r0,_bp
	inc	r0
	mov	@r0,ar7
	inc	r0
	mov	@r0,#0x00
	inc	r0
	mov	@r0,#0x40
	mov	a,_bp
	add	a,#0x04
	mov	r3,a
	mov	r2,a
	mov	r5,#0x00
	mov	r6,#0x40
	push	ar7
	push	ar3
	mov	a,#0x10
	push	acc
	rr	a
	push	acc
	mov	r0,_bp
	inc	r0
	mov	a,@r0
	push	acc
	inc	r0
	mov	a,@r0
	push	acc
	inc	r0
	mov	a,@r0
	push	acc
	mov	dpl,r2
	mov	dph,r5
	mov	b,r6
	lcall	_simpleserial_get
	mov	r5,dpl
	mov	r6,dph
	mov	a,sp
	add	a,#0xfb
	mov	sp,a
	pop	ar3
	pop	ar7
	mov	a,r5
	orl	a,r6
	jz	00104$
;	tea.c:81: trigger_high();
	orl	_P1,#0x01
;	tea.c:82: tea_encrypt(input, key);
	mov	r0,_bp
	inc	r0
	mov	@r0,ar7
	inc	r0
	mov	@r0,#0x00
	inc	r0
	mov	@r0,#0x40
	mov	ar4,r3
	mov	ar2,r4
	mov	r4,#0x00
	mov	r7,#0x40
	push	ar3
	mov	r0,_bp
	inc	r0
	mov	a,@r0
	push	acc
	inc	r0
	mov	a,@r0
	push	acc
	inc	r0
	mov	a,@r0
	push	acc
	mov	dpl,r2
	mov	dph,r4
	mov	b,r7
	lcall	_tea_encrypt
	dec	sp
	dec	sp
	dec	sp
	pop	ar3
;	tea.c:83: trigger_low();
	anl	_P1,#0xfe
;	tea.c:84: simpleserial_put(input, TEA_BLOCK_SIZE);
	mov	r7,#0x00
	mov	r6,#0x40
	mov	a,#0x08
	push	acc
	mov	dpl,r3
	mov	dph,r7
	mov	b,r6
	lcall	_simpleserial_put
	dec	sp
	ljmp	00104$
	mov	sp,_bp
	pop	_bp
	ret
	.area CSEG    (CODE)
	.area CONST   (CODE)
	.area XINIT   (CODE)
	.area CABS    (ABS,CODE)
