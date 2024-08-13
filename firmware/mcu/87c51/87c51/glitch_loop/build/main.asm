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
	.globl _glitch_loop
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
	.globl _SIO_SBUF
	.globl _SIO_SCON
	.globl _SIO_TL1
	.globl _SIO_TH1
	.globl _SIO_TMOD
	.globl _SIO_PCON
	.globl _putchar
	.globl _getchar
;--------------------------------------------------------
; special function registers
;--------------------------------------------------------
	.area RSEG    (ABS,DATA)
	.org 0x0000
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
;Allocation info for local variables in function 'main'
;------------------------------------------------------------
;	main.c:9: void main (void) 
;	-----------------------------------------
;	 function main
;	-----------------------------------------
_main:
;	main.c:13: inituart(248);
	mov	dpl,#0xf8
	lcall	_inituart
;	main.c:16: putchar('\n');	
	mov	dpl,#0x0a
	lcall	_putchar
;	main.c:19: glitch_loop();
	ljmp	_glitch_loop
	.area CSEG    (CODE)
	.area CONST   (CODE)
	.area XINIT   (CODE)
	.area CABS    (ABS,CODE)
