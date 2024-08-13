;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 3.6.0 #9615 (MINGW32)
;--------------------------------------------------------
	.module passcheck
	.optsdcc -mmcs51 --model-small
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _get_password
	.globl _printf_tiny
	.globl _getchar
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
	.globl _main_passcheck
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
;Allocation info for local variables in function 'get_password'
;------------------------------------------------------------
;str                       Allocated to registers r5 r6 r7 
;n                         Allocated to registers r3 r4 
;sloc0                     Allocated to stack - _bp +1
;------------------------------------------------------------
;	passcheck.c:13: int get_password(char* str)
;	-----------------------------------------
;	 function get_password
;	-----------------------------------------
_get_password:
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
	inc	sp
	inc	sp
	inc	sp
	mov	r5,dpl
	mov	r6,dph
	mov	r7,b
;	passcheck.c:16: while(1)
	mov	r3,#0x00
	mov	r4,#0x00
00104$:
;	passcheck.c:18: str[n] = getchar();
	mov	r0,_bp
	inc	r0
	mov	a,r3
	add	a,r5
	mov	@r0,a
	mov	a,r4
	addc	a,r6
	inc	r0
	mov	@r0,a
	inc	r0
	mov	@r0,ar7
	push	ar5
	push	ar6
	push	ar7
	push	ar6
	push	ar5
	push	ar4
	push	ar3
	lcall	_getchar
	mov	r7,dpl
	pop	ar3
	pop	ar4
	pop	ar5
	pop	ar6
	mov	r0,_bp
	inc	r0
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	mov	a,r7
	lcall	__gptrput
;	passcheck.c:19: if(str[n] == '\r')
	cjne	r7,#0x0d,00116$
	sjmp	00117$
00116$:
	pop	ar7
	pop	ar6
	pop	ar5
	sjmp	00102$
00117$:
	pop	ar7
	pop	ar6
	pop	ar5
;	passcheck.c:21: str[n] = '\0';
	mov	r0,_bp
	inc	r0
	mov	dpl,@r0
	inc	r0
	mov	dph,@r0
	inc	r0
	mov	b,@r0
	clr	a
	lcall	__gptrput
;	passcheck.c:22: break;
	sjmp	00105$
00102$:
;	passcheck.c:24: n++;
	inc	r3
	cjne	r3,#0x00,00104$
	inc	r4
	sjmp	00104$
00105$:
;	passcheck.c:26: return n;
	mov	dpl,r3
	mov	dph,r4
	mov	sp,_bp
	pop	_bp
	ret
;------------------------------------------------------------
;Allocation info for local variables in function 'main_passcheck'
;------------------------------------------------------------
;typed_password            Allocated to stack - _bp +1
;i                         Allocated to registers r3 r4 
;success                   Allocated to registers r5 r6 
;------------------------------------------------------------
;	passcheck.c:29: void main_passcheck()
;	-----------------------------------------
;	 function main_passcheck
;	-----------------------------------------
_main_passcheck:
	push	_bp
	mov	a,sp
	mov	_bp,a
	add	a,#0x20
	mov	sp,a
;	passcheck.c:37: printf_tiny("Welcome to your 87C51 login, [username]\n");
	mov	a,#___str_0
	push	acc
	mov	a,#(___str_0 >> 8)
	push	acc
	lcall	_printf_tiny
	dec	sp
	dec	sp
;	passcheck.c:38: printf_tiny("Please enter high-entropy password:\n");
	mov	a,#___str_1
	push	acc
	mov	a,#(___str_1 >> 8)
	push	acc
	lcall	_printf_tiny
	dec	sp
	dec	sp
;	passcheck.c:41: get_password(typed_password);
	mov	r7,_bp
	inc	r7
	mov	ar4,r7
	mov	r5,#0x00
	mov	r6,#0x40
	mov	dpl,r4
	mov	dph,r5
	mov	b,r6
	push	ar7
	lcall	_get_password
	pop	ar7
;	passcheck.c:44: trigger_high();
	orl	_P1,#0x01
;	passcheck.c:45: success = 1;
	mov	r5,#0x01
	mov	r6,#0x00
;	passcheck.c:46: for(i = 0; i < MAX_PASS_LENGTH; i++)
	mov	r3,#0x00
	mov	r4,#0x00
00112$:
;	passcheck.c:48: if(stored_password[i] == '\0')
	mov	a,r3
	add	a,#_stored_password
	mov	dpl,a
	mov	a,r4
	addc	a,#(_stored_password >> 8)
	mov	dph,a
	clr	a
	movc	a,@a+dptr
	mov	r2,a
	jz	00105$
;	passcheck.c:50: if(typed_password[i] != stored_password[i])
	push	ar5
	push	ar6
	mov	a,r3
	add	a,r7
	mov	r1,a
	mov	a,@r1
	mov	r6,a
	cjne	a,ar2,00132$
	pop	ar6
	pop	ar5
	sjmp	00113$
00132$:
	pop	ar6
	pop	ar5
;	passcheck.c:52: success = 0;
	mov	r5,#0x00
	mov	r6,#0x00
;	passcheck.c:53: break;
	sjmp	00105$
00113$:
;	passcheck.c:46: for(i = 0; i < MAX_PASS_LENGTH; i++)
	inc	r3
	cjne	r3,#0x00,00133$
	inc	r4
00133$:
	clr	c
	mov	a,r3
	subb	a,#0x20
	mov	a,r4
	xrl	a,#0x80
	subb	a,#0x80
	jc	00112$
00105$:
;	passcheck.c:56: trigger_low();
	anl	_P1,#0xfe
;	passcheck.c:59: if(success)
	mov	a,r5
	orl	a,r6
	jz	00107$
;	passcheck.c:61: printf_tiny("Password check OK\n");
	mov	a,#___str_2
	push	acc
	mov	a,#(___str_2 >> 8)
	push	acc
	lcall	_printf_tiny
	dec	sp
	dec	sp
;	passcheck.c:62: printf_tiny("Last login: [time]\n");
	mov	a,#___str_3
	push	acc
	mov	a,#(___str_3 >> 8)
	push	acc
	lcall	_printf_tiny
	dec	sp
	dec	sp
;	passcheck.c:63: led1_high();
	orl	_P2,#0x20
;	passcheck.c:64: led2_low();
	anl	_P2,#0xef
	sjmp	00110$
00107$:
;	passcheck.c:69: printf_tiny("Password check FAILED\n");
	mov	a,#___str_4
	push	acc
	mov	a,#(___str_4 >> 8)
	push	acc
	lcall	_printf_tiny
	dec	sp
	dec	sp
;	passcheck.c:70: printf_tiny("Reporting incident to police...\n");
	mov	a,#___str_5
	push	acc
	mov	a,#(___str_5 >> 8)
	push	acc
	lcall	_printf_tiny
	dec	sp
	dec	sp
;	passcheck.c:71: led1_low();
	anl	_P2,#0xdf
;	passcheck.c:72: led2_high();
	orl	_P2,#0x10
;	passcheck.c:77: while(1);
00110$:
	sjmp	00110$
	mov	sp,_bp
	pop	_bp
	ret
	.area CSEG    (CODE)
	.area CONST   (CODE)
_stored_password:
	.ascii "Tr0ub4dor&3"
	.db 0x00
___str_0:
	.ascii "Welcome to your 87C51 login, [username]"
	.db 0x0a
	.db 0x00
___str_1:
	.ascii "Please enter high-entropy password:"
	.db 0x0a
	.db 0x00
___str_2:
	.ascii "Password check OK"
	.db 0x0a
	.db 0x00
___str_3:
	.ascii "Last login: [time]"
	.db 0x0a
	.db 0x00
___str_4:
	.ascii "Password check FAILED"
	.db 0x0a
	.db 0x00
___str_5:
	.ascii "Reporting incident to police..."
	.db 0x0a
	.db 0x00
	.area XINIT   (CODE)
	.area CABS    (ABS,CODE)
