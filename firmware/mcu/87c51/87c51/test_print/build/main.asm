;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 3.6.0 #9615 (MINGW32)
;--------------------------------------------------------
	.module main
	.optsdcc -mmcs51 --model-small
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _main
	.globl _func
	.globl _printf_tiny
	.globl _putchar
	.globl _inituart
	.globl _SIO_SM0
	.globl _SIO_SM1
	.globl _SIO_SM2
	.globl _SIO_REN
	.globl _SIO_TB8
	.globl _SIO_RB8
	.globl _SIO_TI
	.globl _SIO_RI
	.globl _SIO_TR1
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
	.globl _SIO_SBUF
	.globl _SIO_SCON
	.globl _SIO_TL1
	.globl _SIO_TH1
	.globl _SIO_TMOD
	.globl _SIO_PCON
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
_SIO_PCON	=	0x0087
_SIO_TMOD	=	0x0089
_SIO_TH1	=	0x008d
_SIO_TL1	=	0x008b
_SIO_SCON	=	0x0098
_SIO_SBUF	=	0x0099
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
_SIO_TR1	=	0x008e
_SIO_RI	=	0x0098
_SIO_TI	=	0x0099
_SIO_RB8	=	0x009a
_SIO_TB8	=	0x009b
_SIO_REN	=	0x009c
_SIO_SM2	=	0x009d
_SIO_SM1	=	0x009e
_SIO_SM0	=	0x009f
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
; Stack segment in internal ram 
;--------------------------------------------------------
	.area	SSEG
__start__stack:
	.ds	1

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
; interrupt vector 
;--------------------------------------------------------
	.area HOME    (CODE)
__interrupt_vect:
	ljmp	__sdcc_gsinit_startup
;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
	.area HOME    (CODE)
	.area GSINIT  (CODE)
	.area GSFINAL (CODE)
	.area GSINIT  (CODE)
	.globl __sdcc_gsinit_startup
	.globl __sdcc_program_startup
	.globl __start__stack
	.globl __mcs51_genXINIT
	.globl __mcs51_genXRAMCLEAR
	.globl __mcs51_genRAMCLEAR
	.area GSFINAL (CODE)
	ljmp	__sdcc_program_startup
;--------------------------------------------------------
; Home
;--------------------------------------------------------
	.area HOME    (CODE)
	.area HOME    (CODE)
__sdcc_program_startup:
	ljmp	_main
;	return from main will return to caller
;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CSEG    (CODE)
;------------------------------------------------------------
;Allocation info for local variables in function 'inituart'
;------------------------------------------------------------
;t1_reload                 Allocated to registers r7 
;------------------------------------------------------------
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:50: void inituart (unsigned char t1_reload)
;	-----------------------------------------
;	 function inituart
;	-----------------------------------------
_inituart:
	ar7 = 0x07
	ar6 = 0x06
	ar5 = 0x05
	ar4 = 0x04
	ar3 = 0x03
	ar2 = 0x02
	ar1 = 0x01
	ar0 = 0x00
	mov	r7,dpl
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:52: SIO_TR1=0;
	clr	_SIO_TR1
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:53: SIO_TMOD=(SIO_TMOD&0x0f)|0x20;
	mov	a,#0x0f
	anl	a,_SIO_TMOD
	orl	a,#0x20
	mov	_SIO_TMOD,a
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:54: SIO_PCON|=0x80;
	orl	_SIO_PCON,#0x80
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:55: SIO_TH1=SIO_TL1=t1_reload;
	mov	_SIO_TL1,r7
	mov	_SIO_TH1,r7
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:56: SIO_TR1=1;
	setb	_SIO_TR1
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:57: SIO_SCON=0x52;
	mov	_SIO_SCON,#0x52
	ret
;------------------------------------------------------------
;Allocation info for local variables in function 'putchar'
;------------------------------------------------------------
;c                         Allocated to registers r7 
;------------------------------------------------------------
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:60: void putchar (char c)
;	-----------------------------------------
;	 function putchar
;	-----------------------------------------
_putchar:
	mov	r7,dpl
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:62: if((!SIO_SM0)&&(!SIO_SM1)) inituart(0xff);
	jb	_SIO_SM0,00102$
	jb	_SIO_SM1,00102$
	mov	dpl,#0xff
	push	ar7
	lcall	_inituart
	pop	ar7
00102$:
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:63: if (c=='\n')
	cjne	r7,#0x0a,00109$
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:65: while (!SIO_TI);
00104$:
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:66: SIO_TI=0;
	jbc	_SIO_TI,00136$
	sjmp	00104$
00136$:
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:67: SIO_SBUF='\r';
	mov	_SIO_SBUF,#0x0d
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:69: while (!SIO_TI);
00109$:
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:70: SIO_TI=0;
	jbc	_SIO_TI,00137$
	sjmp	00109$
00137$:
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:71: SIO_SBUF=c;
	mov	_SIO_SBUF,r7
	ret
;------------------------------------------------------------
;Allocation info for local variables in function 'getchar'
;------------------------------------------------------------
;c                         Allocated to registers 
;------------------------------------------------------------
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:74: char getchar (void)
;	-----------------------------------------
;	 function getchar
;	-----------------------------------------
_getchar:
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:78: if((!SIO_SM0)&&(!SIO_SM1)) inituart(0xff);
	jb	_SIO_SM0,00104$
	jb	_SIO_SM1,00104$
	mov	dpl,#0xff
	lcall	_inituart
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:80: while (!SIO_RI);
00104$:
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:81: SIO_RI=0;
	jbc	_SIO_RI,00123$
	sjmp	00104$
00123$:
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:82: c=SIO_SBUF;
	mov	dpl,_SIO_SBUF
;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:83: return c;
	ret
;------------------------------------------------------------
;Allocation info for local variables in function 'func'
;------------------------------------------------------------
;x                         Allocated to registers 
;y                         Allocated to registers 
;------------------------------------------------------------
;	main.c:9: void func()
;	-----------------------------------------
;	 function func
;	-----------------------------------------
_func:
;	main.c:13: x += 2;
	ret
;------------------------------------------------------------
;Allocation info for local variables in function 'main'
;------------------------------------------------------------
;x                         Allocated to registers r6 r7 
;------------------------------------------------------------
;	main.c:16: void main (void) 
;	-----------------------------------------
;	 function main
;	-----------------------------------------
_main:
;	main.c:20: int x = 0;
	mov	r6,#0x00
	mov	r7,#0x00
;	main.c:23: inituart(248);
	mov	dpl,#0xf8
	push	ar7
	push	ar6
	lcall	_inituart
;	main.c:26: putchar('\n');
	mov	dpl,#0x0a
	lcall	_putchar
	pop	ar6
	pop	ar7
;	main.c:29: while(1)
00102$:
;	main.c:31: printf_tiny("Testing %d\n", x);
	push	ar7
	push	ar6
	push	ar6
	push	ar7
	mov	a,#___str_0
	push	acc
	mov	a,#(___str_0 >> 8)
	push	acc
	lcall	_printf_tiny
	mov	a,sp
	add	a,#0xfc
	mov	sp,a
	pop	ar6
	pop	ar7
;	main.c:32: x += 1;
	inc	r6
	cjne	r6,#0x00,00102$
	inc	r7
	sjmp	00102$
	.area CSEG    (CODE)
	.area CONST   (CODE)
___str_0:
	.ascii "Testing %d"
	.db 0x0a
	.db 0x00
	.area XINIT   (CODE)
	.area CABS    (ABS,CODE)
