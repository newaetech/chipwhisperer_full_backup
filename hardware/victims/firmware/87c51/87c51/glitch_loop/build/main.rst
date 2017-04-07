                                      1 ;--------------------------------------------------------
                                      2 ; File Created by SDCC : free open source ANSI-C Compiler
                                      3 ; Version 3.6.0 #9615 (MINGW32)
                                      4 ;--------------------------------------------------------
                                      5 	.module main
                                      6 	.optsdcc -mmcs51 --model-small
                                      7 	
                                      8 ;--------------------------------------------------------
                                      9 ; Public variables in this module
                                     10 ;--------------------------------------------------------
                                     11 	.globl _main
                                     12 	.globl _glitch_loop
                                     13 	.globl _inituart
                                     14 	.globl _SIO_SM0
                                     15 	.globl _SIO_SM1
                                     16 	.globl _SIO_SM2
                                     17 	.globl _SIO_REN
                                     18 	.globl _SIO_TB8
                                     19 	.globl _SIO_RB8
                                     20 	.globl _SIO_TI
                                     21 	.globl _SIO_RI
                                     22 	.globl _SIO_TR1
                                     23 	.globl _SIO_SBUF
                                     24 	.globl _SIO_SCON
                                     25 	.globl _SIO_TL1
                                     26 	.globl _SIO_TH1
                                     27 	.globl _SIO_TMOD
                                     28 	.globl _SIO_PCON
                                     29 	.globl _putchar
                                     30 	.globl _getchar
                                     31 ;--------------------------------------------------------
                                     32 ; special function registers
                                     33 ;--------------------------------------------------------
                                     34 	.area RSEG    (ABS,DATA)
      000000                         35 	.org 0x0000
                           000087    36 _SIO_PCON	=	0x0087
                           000089    37 _SIO_TMOD	=	0x0089
                           00008D    38 _SIO_TH1	=	0x008d
                           00008B    39 _SIO_TL1	=	0x008b
                           000098    40 _SIO_SCON	=	0x0098
                           000099    41 _SIO_SBUF	=	0x0099
                                     42 ;--------------------------------------------------------
                                     43 ; special function bits
                                     44 ;--------------------------------------------------------
                                     45 	.area RSEG    (ABS,DATA)
      000000                         46 	.org 0x0000
                           00008E    47 _SIO_TR1	=	0x008e
                           000098    48 _SIO_RI	=	0x0098
                           000099    49 _SIO_TI	=	0x0099
                           00009A    50 _SIO_RB8	=	0x009a
                           00009B    51 _SIO_TB8	=	0x009b
                           00009C    52 _SIO_REN	=	0x009c
                           00009D    53 _SIO_SM2	=	0x009d
                           00009E    54 _SIO_SM1	=	0x009e
                           00009F    55 _SIO_SM0	=	0x009f
                                     56 ;--------------------------------------------------------
                                     57 ; overlayable register banks
                                     58 ;--------------------------------------------------------
                                     59 	.area REG_BANK_0	(REL,OVR,DATA)
      000000                         60 	.ds 8
                                     61 ;--------------------------------------------------------
                                     62 ; internal ram data
                                     63 ;--------------------------------------------------------
                                     64 	.area DSEG    (DATA)
                                     65 ;--------------------------------------------------------
                                     66 ; overlayable items in internal ram 
                                     67 ;--------------------------------------------------------
                                     68 ;--------------------------------------------------------
                                     69 ; Stack segment in internal ram 
                                     70 ;--------------------------------------------------------
                                     71 	.area	SSEG
      000030                         72 __start__stack:
      000030                         73 	.ds	1
                                     74 
                                     75 ;--------------------------------------------------------
                                     76 ; indirectly addressable internal ram data
                                     77 ;--------------------------------------------------------
                                     78 	.area ISEG    (DATA)
                                     79 ;--------------------------------------------------------
                                     80 ; absolute internal ram data
                                     81 ;--------------------------------------------------------
                                     82 	.area IABS    (ABS,DATA)
                                     83 	.area IABS    (ABS,DATA)
                                     84 ;--------------------------------------------------------
                                     85 ; bit data
                                     86 ;--------------------------------------------------------
                                     87 	.area BSEG    (BIT)
                                     88 ;--------------------------------------------------------
                                     89 ; paged external ram data
                                     90 ;--------------------------------------------------------
                                     91 	.area PSEG    (PAG,XDATA)
                                     92 ;--------------------------------------------------------
                                     93 ; external ram data
                                     94 ;--------------------------------------------------------
                                     95 	.area XSEG    (XDATA)
                                     96 ;--------------------------------------------------------
                                     97 ; absolute external ram data
                                     98 ;--------------------------------------------------------
                                     99 	.area XABS    (ABS,XDATA)
                                    100 ;--------------------------------------------------------
                                    101 ; external initialized ram data
                                    102 ;--------------------------------------------------------
                                    103 	.area XISEG   (XDATA)
                                    104 	.area HOME    (CODE)
                                    105 	.area GSINIT0 (CODE)
                                    106 	.area GSINIT1 (CODE)
                                    107 	.area GSINIT2 (CODE)
                                    108 	.area GSINIT3 (CODE)
                                    109 	.area GSINIT4 (CODE)
                                    110 	.area GSINIT5 (CODE)
                                    111 	.area GSINIT  (CODE)
                                    112 	.area GSFINAL (CODE)
                                    113 	.area CSEG    (CODE)
                                    114 ;--------------------------------------------------------
                                    115 ; interrupt vector 
                                    116 ;--------------------------------------------------------
                                    117 	.area HOME    (CODE)
      000000                        118 __interrupt_vect:
      000000 02 00 06         [24]  119 	ljmp	__sdcc_gsinit_startup
                                    120 ;--------------------------------------------------------
                                    121 ; global & static initialisations
                                    122 ;--------------------------------------------------------
                                    123 	.area HOME    (CODE)
                                    124 	.area GSINIT  (CODE)
                                    125 	.area GSFINAL (CODE)
                                    126 	.area GSINIT  (CODE)
                                    127 	.globl __sdcc_gsinit_startup
                                    128 	.globl __sdcc_program_startup
                                    129 	.globl __start__stack
                                    130 	.globl __mcs51_genXINIT
                                    131 	.globl __mcs51_genXRAMCLEAR
                                    132 	.globl __mcs51_genRAMCLEAR
                                    133 	.area GSFINAL (CODE)
      00005F 02 00 03         [24]  134 	ljmp	__sdcc_program_startup
                                    135 ;--------------------------------------------------------
                                    136 ; Home
                                    137 ;--------------------------------------------------------
                                    138 	.area HOME    (CODE)
                                    139 	.area HOME    (CODE)
      000003                        140 __sdcc_program_startup:
      000003 02 00 B5         [24]  141 	ljmp	_main
                                    142 ;	return from main will return to caller
                                    143 ;--------------------------------------------------------
                                    144 ; code
                                    145 ;--------------------------------------------------------
                                    146 	.area CSEG    (CODE)
                                    147 ;------------------------------------------------------------
                                    148 ;Allocation info for local variables in function 'inituart'
                                    149 ;------------------------------------------------------------
                                    150 ;t1_reload                 Allocated to registers r7 
                                    151 ;------------------------------------------------------------
                                    152 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:50: void inituart (unsigned char t1_reload)
                                    153 ;	-----------------------------------------
                                    154 ;	 function inituart
                                    155 ;	-----------------------------------------
      000062                        156 _inituart:
                           000007   157 	ar7 = 0x07
                           000006   158 	ar6 = 0x06
                           000005   159 	ar5 = 0x05
                           000004   160 	ar4 = 0x04
                           000003   161 	ar3 = 0x03
                           000002   162 	ar2 = 0x02
                           000001   163 	ar1 = 0x01
                           000000   164 	ar0 = 0x00
      000062 AF 82            [24]  165 	mov	r7,dpl
                                    166 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:52: SIO_TR1=0;
      000064 C2 8E            [12]  167 	clr	_SIO_TR1
                                    168 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:53: SIO_TMOD=(SIO_TMOD&0x0f)|0x20;
      000066 74 0F            [12]  169 	mov	a,#0x0f
      000068 55 89            [12]  170 	anl	a,_SIO_TMOD
      00006A 44 20            [12]  171 	orl	a,#0x20
      00006C F5 89            [12]  172 	mov	_SIO_TMOD,a
                                    173 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:54: SIO_PCON|=0x80;
      00006E 43 87 80         [24]  174 	orl	_SIO_PCON,#0x80
                                    175 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:55: SIO_TH1=SIO_TL1=t1_reload;
      000071 8F 8B            [24]  176 	mov	_SIO_TL1,r7
      000073 8F 8D            [24]  177 	mov	_SIO_TH1,r7
                                    178 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:56: SIO_TR1=1;
      000075 D2 8E            [12]  179 	setb	_SIO_TR1
                                    180 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:57: SIO_SCON=0x52;
      000077 75 98 52         [24]  181 	mov	_SIO_SCON,#0x52
      00007A 22               [24]  182 	ret
                                    183 ;------------------------------------------------------------
                                    184 ;Allocation info for local variables in function 'putchar'
                                    185 ;------------------------------------------------------------
                                    186 ;c                         Allocated to registers r7 
                                    187 ;------------------------------------------------------------
                                    188 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:60: void putchar (char c)
                                    189 ;	-----------------------------------------
                                    190 ;	 function putchar
                                    191 ;	-----------------------------------------
      00007B                        192 _putchar:
      00007B AF 82            [24]  193 	mov	r7,dpl
                                    194 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:62: if((!SIO_SM0)&&(!SIO_SM1)) inituart(0xff);
      00007D 20 9F 0D         [24]  195 	jb	_SIO_SM0,00102$
      000080 20 9E 0A         [24]  196 	jb	_SIO_SM1,00102$
      000083 75 82 FF         [24]  197 	mov	dpl,#0xff
      000086 C0 07            [24]  198 	push	ar7
      000088 12 00 62         [24]  199 	lcall	_inituart
      00008B D0 07            [24]  200 	pop	ar7
      00008D                        201 00102$:
                                    202 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:63: if (c=='\n')
      00008D BF 0A 08         [24]  203 	cjne	r7,#0x0a,00109$
                                    204 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:65: while (!SIO_TI);
      000090                        205 00104$:
                                    206 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:66: SIO_TI=0;
      000090 10 99 02         [24]  207 	jbc	_SIO_TI,00136$
      000093 80 FB            [24]  208 	sjmp	00104$
      000095                        209 00136$:
                                    210 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:67: SIO_SBUF='\r';
      000095 75 99 0D         [24]  211 	mov	_SIO_SBUF,#0x0d
                                    212 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:69: while (!SIO_TI);
      000098                        213 00109$:
                                    214 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:70: SIO_TI=0;
      000098 10 99 02         [24]  215 	jbc	_SIO_TI,00137$
      00009B 80 FB            [24]  216 	sjmp	00109$
      00009D                        217 00137$:
                                    218 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:71: SIO_SBUF=c;
      00009D 8F 99            [24]  219 	mov	_SIO_SBUF,r7
      00009F 22               [24]  220 	ret
                                    221 ;------------------------------------------------------------
                                    222 ;Allocation info for local variables in function 'getchar'
                                    223 ;------------------------------------------------------------
                                    224 ;c                         Allocated to registers 
                                    225 ;------------------------------------------------------------
                                    226 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:74: char getchar (void)
                                    227 ;	-----------------------------------------
                                    228 ;	 function getchar
                                    229 ;	-----------------------------------------
      0000A0                        230 _getchar:
                                    231 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:78: if((!SIO_SM0)&&(!SIO_SM1)) inituart(0xff);
      0000A0 20 9F 09         [24]  232 	jb	_SIO_SM0,00104$
      0000A3 20 9E 06         [24]  233 	jb	_SIO_SM1,00104$
      0000A6 75 82 FF         [24]  234 	mov	dpl,#0xff
      0000A9 12 00 62         [24]  235 	lcall	_inituart
                                    236 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:80: while (!SIO_RI);
      0000AC                        237 00104$:
                                    238 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:81: SIO_RI=0;
      0000AC 10 98 02         [24]  239 	jbc	_SIO_RI,00123$
      0000AF 80 FB            [24]  240 	sjmp	00104$
      0000B1                        241 00123$:
                                    242 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:82: c=SIO_SBUF;
      0000B1 85 99 82         [24]  243 	mov	dpl,_SIO_SBUF
                                    244 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:83: return c;
      0000B4 22               [24]  245 	ret
                                    246 ;------------------------------------------------------------
                                    247 ;Allocation info for local variables in function 'main'
                                    248 ;------------------------------------------------------------
                                    249 ;	main.c:9: void main (void) 
                                    250 ;	-----------------------------------------
                                    251 ;	 function main
                                    252 ;	-----------------------------------------
      0000B5                        253 _main:
                                    254 ;	main.c:13: inituart(248);
      0000B5 75 82 F8         [24]  255 	mov	dpl,#0xf8
      0000B8 12 00 62         [24]  256 	lcall	_inituart
                                    257 ;	main.c:16: putchar('\n');	
      0000BB 75 82 0A         [24]  258 	mov	dpl,#0x0a
      0000BE 12 00 7B         [24]  259 	lcall	_putchar
                                    260 ;	main.c:19: glitch_loop();
      0000C1 02 00 C4         [24]  261 	ljmp	_glitch_loop
                                    262 	.area CSEG    (CODE)
                                    263 	.area CONST   (CODE)
                                    264 	.area XINIT   (CODE)
                                    265 	.area CABS    (ABS,CODE)
