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
                                     12 	.globl _main_tea
                                     13 	.globl _main_aes
                                     14 	.globl _main_xor
                                     15 	.globl _main_glitchloop
                                     16 	.globl _main_passcheck
                                     17 	.globl _main_print
                                     18 	.globl _putchar
                                     19 	.globl _getchar
                                     20 	.globl _inituart
                                     21 	.globl _TF2
                                     22 	.globl _EXF2
                                     23 	.globl _RCLK
                                     24 	.globl _TCLK
                                     25 	.globl _EXEN2
                                     26 	.globl _TR2
                                     27 	.globl _C_T2
                                     28 	.globl _CP_RL2
                                     29 	.globl _T2CON_7
                                     30 	.globl _T2CON_6
                                     31 	.globl _T2CON_5
                                     32 	.globl _T2CON_4
                                     33 	.globl _T2CON_3
                                     34 	.globl _T2CON_2
                                     35 	.globl _T2CON_1
                                     36 	.globl _T2CON_0
                                     37 	.globl _PT2
                                     38 	.globl _ET2
                                     39 	.globl _CY
                                     40 	.globl _AC
                                     41 	.globl _F0
                                     42 	.globl _RS1
                                     43 	.globl _RS0
                                     44 	.globl _OV
                                     45 	.globl _F1
                                     46 	.globl _P
                                     47 	.globl _PS
                                     48 	.globl _PT1
                                     49 	.globl _PX1
                                     50 	.globl _PT0
                                     51 	.globl _PX0
                                     52 	.globl _RD
                                     53 	.globl _WR
                                     54 	.globl _T1
                                     55 	.globl _T0
                                     56 	.globl _INT1
                                     57 	.globl _INT0
                                     58 	.globl _TXD
                                     59 	.globl _RXD
                                     60 	.globl _P3_7
                                     61 	.globl _P3_6
                                     62 	.globl _P3_5
                                     63 	.globl _P3_4
                                     64 	.globl _P3_3
                                     65 	.globl _P3_2
                                     66 	.globl _P3_1
                                     67 	.globl _P3_0
                                     68 	.globl _EA
                                     69 	.globl _ES
                                     70 	.globl _ET1
                                     71 	.globl _EX1
                                     72 	.globl _ET0
                                     73 	.globl _EX0
                                     74 	.globl _P2_7
                                     75 	.globl _P2_6
                                     76 	.globl _P2_5
                                     77 	.globl _P2_4
                                     78 	.globl _P2_3
                                     79 	.globl _P2_2
                                     80 	.globl _P2_1
                                     81 	.globl _P2_0
                                     82 	.globl _SM0
                                     83 	.globl _SM1
                                     84 	.globl _SM2
                                     85 	.globl _REN
                                     86 	.globl _TB8
                                     87 	.globl _RB8
                                     88 	.globl _TI
                                     89 	.globl _RI
                                     90 	.globl _P1_7
                                     91 	.globl _P1_6
                                     92 	.globl _P1_5
                                     93 	.globl _P1_4
                                     94 	.globl _P1_3
                                     95 	.globl _P1_2
                                     96 	.globl _P1_1
                                     97 	.globl _P1_0
                                     98 	.globl _TF1
                                     99 	.globl _TR1
                                    100 	.globl _TF0
                                    101 	.globl _TR0
                                    102 	.globl _IE1
                                    103 	.globl _IT1
                                    104 	.globl _IE0
                                    105 	.globl _IT0
                                    106 	.globl _P0_7
                                    107 	.globl _P0_6
                                    108 	.globl _P0_5
                                    109 	.globl _P0_4
                                    110 	.globl _P0_3
                                    111 	.globl _P0_2
                                    112 	.globl _P0_1
                                    113 	.globl _P0_0
                                    114 	.globl _SIO_SM0
                                    115 	.globl _SIO_SM1
                                    116 	.globl _SIO_SM2
                                    117 	.globl _SIO_REN
                                    118 	.globl _SIO_TB8
                                    119 	.globl _SIO_RB8
                                    120 	.globl _SIO_TI
                                    121 	.globl _SIO_RI
                                    122 	.globl _SIO_TR1
                                    123 	.globl _TH2
                                    124 	.globl _TL2
                                    125 	.globl _RCAP2H
                                    126 	.globl _RCAP2L
                                    127 	.globl _T2CON
                                    128 	.globl _B
                                    129 	.globl _ACC
                                    130 	.globl _PSW
                                    131 	.globl _IP
                                    132 	.globl _P3
                                    133 	.globl _IE
                                    134 	.globl _P2
                                    135 	.globl _SBUF
                                    136 	.globl _SCON
                                    137 	.globl _P1
                                    138 	.globl _TH1
                                    139 	.globl _TH0
                                    140 	.globl _TL1
                                    141 	.globl _TL0
                                    142 	.globl _TMOD
                                    143 	.globl _TCON
                                    144 	.globl _PCON
                                    145 	.globl _DPH
                                    146 	.globl _DPL
                                    147 	.globl _SP
                                    148 	.globl _P0
                                    149 	.globl _SIO_SBUF
                                    150 	.globl _SIO_SCON
                                    151 	.globl _SIO_TL1
                                    152 	.globl _SIO_TH1
                                    153 	.globl _SIO_TMOD
                                    154 	.globl _SIO_PCON
                                    155 ;--------------------------------------------------------
                                    156 ; special function registers
                                    157 ;--------------------------------------------------------
                                    158 	.area RSEG    (ABS,DATA)
      000000                        159 	.org 0x0000
                           000087   160 _SIO_PCON	=	0x0087
                           000089   161 _SIO_TMOD	=	0x0089
                           00008D   162 _SIO_TH1	=	0x008d
                           00008B   163 _SIO_TL1	=	0x008b
                           000098   164 _SIO_SCON	=	0x0098
                           000099   165 _SIO_SBUF	=	0x0099
                           000080   166 _P0	=	0x0080
                           000081   167 _SP	=	0x0081
                           000082   168 _DPL	=	0x0082
                           000083   169 _DPH	=	0x0083
                           000087   170 _PCON	=	0x0087
                           000088   171 _TCON	=	0x0088
                           000089   172 _TMOD	=	0x0089
                           00008A   173 _TL0	=	0x008a
                           00008B   174 _TL1	=	0x008b
                           00008C   175 _TH0	=	0x008c
                           00008D   176 _TH1	=	0x008d
                           000090   177 _P1	=	0x0090
                           000098   178 _SCON	=	0x0098
                           000099   179 _SBUF	=	0x0099
                           0000A0   180 _P2	=	0x00a0
                           0000A8   181 _IE	=	0x00a8
                           0000B0   182 _P3	=	0x00b0
                           0000B8   183 _IP	=	0x00b8
                           0000D0   184 _PSW	=	0x00d0
                           0000E0   185 _ACC	=	0x00e0
                           0000F0   186 _B	=	0x00f0
                           0000C8   187 _T2CON	=	0x00c8
                           0000CA   188 _RCAP2L	=	0x00ca
                           0000CB   189 _RCAP2H	=	0x00cb
                           0000CC   190 _TL2	=	0x00cc
                           0000CD   191 _TH2	=	0x00cd
                                    192 ;--------------------------------------------------------
                                    193 ; special function bits
                                    194 ;--------------------------------------------------------
                                    195 	.area RSEG    (ABS,DATA)
      000000                        196 	.org 0x0000
                           00008E   197 _SIO_TR1	=	0x008e
                           000098   198 _SIO_RI	=	0x0098
                           000099   199 _SIO_TI	=	0x0099
                           00009A   200 _SIO_RB8	=	0x009a
                           00009B   201 _SIO_TB8	=	0x009b
                           00009C   202 _SIO_REN	=	0x009c
                           00009D   203 _SIO_SM2	=	0x009d
                           00009E   204 _SIO_SM1	=	0x009e
                           00009F   205 _SIO_SM0	=	0x009f
                           000080   206 _P0_0	=	0x0080
                           000081   207 _P0_1	=	0x0081
                           000082   208 _P0_2	=	0x0082
                           000083   209 _P0_3	=	0x0083
                           000084   210 _P0_4	=	0x0084
                           000085   211 _P0_5	=	0x0085
                           000086   212 _P0_6	=	0x0086
                           000087   213 _P0_7	=	0x0087
                           000088   214 _IT0	=	0x0088
                           000089   215 _IE0	=	0x0089
                           00008A   216 _IT1	=	0x008a
                           00008B   217 _IE1	=	0x008b
                           00008C   218 _TR0	=	0x008c
                           00008D   219 _TF0	=	0x008d
                           00008E   220 _TR1	=	0x008e
                           00008F   221 _TF1	=	0x008f
                           000090   222 _P1_0	=	0x0090
                           000091   223 _P1_1	=	0x0091
                           000092   224 _P1_2	=	0x0092
                           000093   225 _P1_3	=	0x0093
                           000094   226 _P1_4	=	0x0094
                           000095   227 _P1_5	=	0x0095
                           000096   228 _P1_6	=	0x0096
                           000097   229 _P1_7	=	0x0097
                           000098   230 _RI	=	0x0098
                           000099   231 _TI	=	0x0099
                           00009A   232 _RB8	=	0x009a
                           00009B   233 _TB8	=	0x009b
                           00009C   234 _REN	=	0x009c
                           00009D   235 _SM2	=	0x009d
                           00009E   236 _SM1	=	0x009e
                           00009F   237 _SM0	=	0x009f
                           0000A0   238 _P2_0	=	0x00a0
                           0000A1   239 _P2_1	=	0x00a1
                           0000A2   240 _P2_2	=	0x00a2
                           0000A3   241 _P2_3	=	0x00a3
                           0000A4   242 _P2_4	=	0x00a4
                           0000A5   243 _P2_5	=	0x00a5
                           0000A6   244 _P2_6	=	0x00a6
                           0000A7   245 _P2_7	=	0x00a7
                           0000A8   246 _EX0	=	0x00a8
                           0000A9   247 _ET0	=	0x00a9
                           0000AA   248 _EX1	=	0x00aa
                           0000AB   249 _ET1	=	0x00ab
                           0000AC   250 _ES	=	0x00ac
                           0000AF   251 _EA	=	0x00af
                           0000B0   252 _P3_0	=	0x00b0
                           0000B1   253 _P3_1	=	0x00b1
                           0000B2   254 _P3_2	=	0x00b2
                           0000B3   255 _P3_3	=	0x00b3
                           0000B4   256 _P3_4	=	0x00b4
                           0000B5   257 _P3_5	=	0x00b5
                           0000B6   258 _P3_6	=	0x00b6
                           0000B7   259 _P3_7	=	0x00b7
                           0000B0   260 _RXD	=	0x00b0
                           0000B1   261 _TXD	=	0x00b1
                           0000B2   262 _INT0	=	0x00b2
                           0000B3   263 _INT1	=	0x00b3
                           0000B4   264 _T0	=	0x00b4
                           0000B5   265 _T1	=	0x00b5
                           0000B6   266 _WR	=	0x00b6
                           0000B7   267 _RD	=	0x00b7
                           0000B8   268 _PX0	=	0x00b8
                           0000B9   269 _PT0	=	0x00b9
                           0000BA   270 _PX1	=	0x00ba
                           0000BB   271 _PT1	=	0x00bb
                           0000BC   272 _PS	=	0x00bc
                           0000D0   273 _P	=	0x00d0
                           0000D1   274 _F1	=	0x00d1
                           0000D2   275 _OV	=	0x00d2
                           0000D3   276 _RS0	=	0x00d3
                           0000D4   277 _RS1	=	0x00d4
                           0000D5   278 _F0	=	0x00d5
                           0000D6   279 _AC	=	0x00d6
                           0000D7   280 _CY	=	0x00d7
                           0000AD   281 _ET2	=	0x00ad
                           0000BD   282 _PT2	=	0x00bd
                           0000C8   283 _T2CON_0	=	0x00c8
                           0000C9   284 _T2CON_1	=	0x00c9
                           0000CA   285 _T2CON_2	=	0x00ca
                           0000CB   286 _T2CON_3	=	0x00cb
                           0000CC   287 _T2CON_4	=	0x00cc
                           0000CD   288 _T2CON_5	=	0x00cd
                           0000CE   289 _T2CON_6	=	0x00ce
                           0000CF   290 _T2CON_7	=	0x00cf
                           0000C8   291 _CP_RL2	=	0x00c8
                           0000C9   292 _C_T2	=	0x00c9
                           0000CA   293 _TR2	=	0x00ca
                           0000CB   294 _EXEN2	=	0x00cb
                           0000CC   295 _TCLK	=	0x00cc
                           0000CD   296 _RCLK	=	0x00cd
                           0000CE   297 _EXF2	=	0x00ce
                           0000CF   298 _TF2	=	0x00cf
                                    299 ;--------------------------------------------------------
                                    300 ; overlayable register banks
                                    301 ;--------------------------------------------------------
                                    302 	.area REG_BANK_0	(REL,OVR,DATA)
      000000                        303 	.ds 8
                                    304 ;--------------------------------------------------------
                                    305 ; internal ram data
                                    306 ;--------------------------------------------------------
                                    307 	.area DSEG    (DATA)
                                    308 ;--------------------------------------------------------
                                    309 ; overlayable items in internal ram 
                                    310 ;--------------------------------------------------------
                                    311 ;--------------------------------------------------------
                                    312 ; Stack segment in internal ram 
                                    313 ;--------------------------------------------------------
                                    314 	.area	SSEG
      000009                        315 __start__stack:
      000009                        316 	.ds	1
                                    317 
                                    318 ;--------------------------------------------------------
                                    319 ; indirectly addressable internal ram data
                                    320 ;--------------------------------------------------------
                                    321 	.area ISEG    (DATA)
                                    322 ;--------------------------------------------------------
                                    323 ; absolute internal ram data
                                    324 ;--------------------------------------------------------
                                    325 	.area IABS    (ABS,DATA)
                                    326 	.area IABS    (ABS,DATA)
                                    327 ;--------------------------------------------------------
                                    328 ; bit data
                                    329 ;--------------------------------------------------------
                                    330 	.area BSEG    (BIT)
                                    331 ;--------------------------------------------------------
                                    332 ; paged external ram data
                                    333 ;--------------------------------------------------------
                                    334 	.area PSEG    (PAG,XDATA)
                                    335 ;--------------------------------------------------------
                                    336 ; external ram data
                                    337 ;--------------------------------------------------------
                                    338 	.area XSEG    (XDATA)
                                    339 ;--------------------------------------------------------
                                    340 ; absolute external ram data
                                    341 ;--------------------------------------------------------
                                    342 	.area XABS    (ABS,XDATA)
                                    343 ;--------------------------------------------------------
                                    344 ; external initialized ram data
                                    345 ;--------------------------------------------------------
                                    346 	.area XISEG   (XDATA)
                                    347 	.area HOME    (CODE)
                                    348 	.area GSINIT0 (CODE)
                                    349 	.area GSINIT1 (CODE)
                                    350 	.area GSINIT2 (CODE)
                                    351 	.area GSINIT3 (CODE)
                                    352 	.area GSINIT4 (CODE)
                                    353 	.area GSINIT5 (CODE)
                                    354 	.area GSINIT  (CODE)
                                    355 	.area GSFINAL (CODE)
                                    356 	.area CSEG    (CODE)
                                    357 ;--------------------------------------------------------
                                    358 ; interrupt vector 
                                    359 ;--------------------------------------------------------
                                    360 	.area HOME    (CODE)
      000000                        361 __interrupt_vect:
      000000 02 00 06         [24]  362 	ljmp	__sdcc_gsinit_startup
                                    363 ;--------------------------------------------------------
                                    364 ; global & static initialisations
                                    365 ;--------------------------------------------------------
                                    366 	.area HOME    (CODE)
                                    367 	.area GSINIT  (CODE)
                                    368 	.area GSFINAL (CODE)
                                    369 	.area GSINIT  (CODE)
                                    370 	.globl __sdcc_gsinit_startup
                                    371 	.globl __sdcc_program_startup
                                    372 	.globl __start__stack
                                    373 	.globl __mcs51_genXINIT
                                    374 	.globl __mcs51_genXRAMCLEAR
                                    375 	.globl __mcs51_genRAMCLEAR
                                    376 	.area GSFINAL (CODE)
      00005F 02 00 03         [24]  377 	ljmp	__sdcc_program_startup
                                    378 ;--------------------------------------------------------
                                    379 ; Home
                                    380 ;--------------------------------------------------------
                                    381 	.area HOME    (CODE)
                                    382 	.area HOME    (CODE)
      000003                        383 __sdcc_program_startup:
      000003 02 00 B5         [24]  384 	ljmp	_main
                                    385 ;	return from main will return to caller
                                    386 ;--------------------------------------------------------
                                    387 ; code
                                    388 ;--------------------------------------------------------
                                    389 	.area CSEG    (CODE)
                                    390 ;------------------------------------------------------------
                                    391 ;Allocation info for local variables in function 'inituart'
                                    392 ;------------------------------------------------------------
                                    393 ;t1_reload                 Allocated to registers r7 
                                    394 ;------------------------------------------------------------
                                    395 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:50: void inituart (unsigned char t1_reload)
                                    396 ;	-----------------------------------------
                                    397 ;	 function inituart
                                    398 ;	-----------------------------------------
      000062                        399 _inituart:
                           000007   400 	ar7 = 0x07
                           000006   401 	ar6 = 0x06
                           000005   402 	ar5 = 0x05
                           000004   403 	ar4 = 0x04
                           000003   404 	ar3 = 0x03
                           000002   405 	ar2 = 0x02
                           000001   406 	ar1 = 0x01
                           000000   407 	ar0 = 0x00
      000062 AF 82            [24]  408 	mov	r7,dpl
                                    409 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:52: SIO_TR1=0;
      000064 C2 8E            [12]  410 	clr	_SIO_TR1
                                    411 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:53: SIO_TMOD=(SIO_TMOD&0x0f)|0x20;
      000066 74 0F            [12]  412 	mov	a,#0x0f
      000068 55 89            [12]  413 	anl	a,_SIO_TMOD
      00006A 44 20            [12]  414 	orl	a,#0x20
      00006C F5 89            [12]  415 	mov	_SIO_TMOD,a
                                    416 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:54: SIO_PCON|=0x80;
      00006E 43 87 80         [24]  417 	orl	_SIO_PCON,#0x80
                                    418 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:55: SIO_TH1=SIO_TL1=t1_reload;
      000071 8F 8B            [24]  419 	mov	_SIO_TL1,r7
      000073 8F 8D            [24]  420 	mov	_SIO_TH1,r7
                                    421 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:56: SIO_TR1=1;
      000075 D2 8E            [12]  422 	setb	_SIO_TR1
                                    423 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:57: SIO_SCON=0x52;
      000077 75 98 52         [24]  424 	mov	_SIO_SCON,#0x52
      00007A 22               [24]  425 	ret
                                    426 ;------------------------------------------------------------
                                    427 ;Allocation info for local variables in function 'putchar'
                                    428 ;------------------------------------------------------------
                                    429 ;c                         Allocated to registers r7 
                                    430 ;------------------------------------------------------------
                                    431 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:60: void putchar (char c)
                                    432 ;	-----------------------------------------
                                    433 ;	 function putchar
                                    434 ;	-----------------------------------------
      00007B                        435 _putchar:
      00007B AF 82            [24]  436 	mov	r7,dpl
                                    437 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:62: if((!SIO_SM0)&&(!SIO_SM1)) inituart(0xff);
      00007D 20 9F 0D         [24]  438 	jb	_SIO_SM0,00102$
      000080 20 9E 0A         [24]  439 	jb	_SIO_SM1,00102$
      000083 75 82 FF         [24]  440 	mov	dpl,#0xff
      000086 C0 07            [24]  441 	push	ar7
      000088 12 00 62         [24]  442 	lcall	_inituart
      00008B D0 07            [24]  443 	pop	ar7
      00008D                        444 00102$:
                                    445 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:63: if (c=='\n')
      00008D BF 0A 08         [24]  446 	cjne	r7,#0x0a,00109$
                                    447 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:65: while (!SIO_TI);
      000090                        448 00104$:
                                    449 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:66: SIO_TI=0;
      000090 10 99 02         [24]  450 	jbc	_SIO_TI,00136$
      000093 80 FB            [24]  451 	sjmp	00104$
      000095                        452 00136$:
                                    453 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:67: SIO_SBUF='\r';
      000095 75 99 0D         [24]  454 	mov	_SIO_SBUF,#0x0d
                                    455 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:69: while (!SIO_TI);
      000098                        456 00109$:
                                    457 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:70: SIO_TI=0;
      000098 10 99 02         [24]  458 	jbc	_SIO_TI,00137$
      00009B 80 FB            [24]  459 	sjmp	00109$
      00009D                        460 00137$:
                                    461 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:71: SIO_SBUF=c;
      00009D 8F 99            [24]  462 	mov	_SIO_SBUF,r7
      00009F 22               [24]  463 	ret
                                    464 ;------------------------------------------------------------
                                    465 ;Allocation info for local variables in function 'getchar'
                                    466 ;------------------------------------------------------------
                                    467 ;c                         Allocated to registers 
                                    468 ;------------------------------------------------------------
                                    469 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:74: char getchar (void)
                                    470 ;	-----------------------------------------
                                    471 ;	 function getchar
                                    472 ;	-----------------------------------------
      0000A0                        473 _getchar:
                                    474 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:78: if((!SIO_SM0)&&(!SIO_SM1)) inituart(0xff);
      0000A0 20 9F 09         [24]  475 	jb	_SIO_SM0,00104$
      0000A3 20 9E 06         [24]  476 	jb	_SIO_SM1,00104$
      0000A6 75 82 FF         [24]  477 	mov	dpl,#0xff
      0000A9 12 00 62         [24]  478 	lcall	_inituart
                                    479 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:80: while (!SIO_RI);
      0000AC                        480 00104$:
                                    481 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:81: SIO_RI=0;
      0000AC 10 98 02         [24]  482 	jbc	_SIO_RI,00123$
      0000AF 80 FB            [24]  483 	sjmp	00104$
      0000B1                        484 00123$:
                                    485 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:82: c=SIO_SBUF;
      0000B1 85 99 82         [24]  486 	mov	dpl,_SIO_SBUF
                                    487 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:83: return c;
      0000B4 22               [24]  488 	ret
                                    489 ;------------------------------------------------------------
                                    490 ;Allocation info for local variables in function 'main'
                                    491 ;------------------------------------------------------------
                                    492 ;mode                      Allocated to registers r7 
                                    493 ;------------------------------------------------------------
                                    494 ;	main.c:34: void main(void) 
                                    495 ;	-----------------------------------------
                                    496 ;	 function main
                                    497 ;	-----------------------------------------
      0000B5                        498 _main:
                                    499 ;	main.c:40: inituart(248);	// 4800 baud with 7.37 MHz clock
      0000B5 75 82 F8         [24]  500 	mov	dpl,#0xf8
      0000B8 12 00 62         [24]  501 	lcall	_inituart
                                    502 ;	main.c:41: trigger_low();
      0000BB 53 90 FE         [24]  503 	anl	_P1,#0xfe
                                    504 ;	main.c:44: putchar('\n');
      0000BE 75 82 0A         [24]  505 	mov	dpl,#0x0a
      0000C1 12 00 7B         [24]  506 	lcall	_putchar
                                    507 ;	main.c:46: mode = get_mode();
      0000C4 E5 B0            [12]  508 	mov	a,_P3
      0000C6 C4               [12]  509 	swap	a
      0000C7 23               [12]  510 	rl	a
                                    511 ;	main.c:49: switch(mode)
      0000C8 54 07            [12]  512 	anl	a,#(0x1f&0x07)
      0000CA FF               [12]  513 	mov	r7,a
      0000CB 24 FA            [12]  514 	add	a,#0xff - 0x05
      0000CD 40 24            [24]  515 	jc	00107$
      0000CF EF               [12]  516 	mov	a,r7
      0000D0 2F               [12]  517 	add	a,r7
                                    518 ;	main.c:51: case MODE_PRINT:
      0000D1 90 00 D5         [24]  519 	mov	dptr,#00120$
      0000D4 73               [24]  520 	jmp	@a+dptr
      0000D5                        521 00120$:
      0000D5 80 0A            [24]  522 	sjmp	00101$
      0000D7 80 0B            [24]  523 	sjmp	00102$
      0000D9 80 0C            [24]  524 	sjmp	00103$
      0000DB 80 0D            [24]  525 	sjmp	00104$
      0000DD 80 0E            [24]  526 	sjmp	00105$
      0000DF 80 0F            [24]  527 	sjmp	00106$
      0000E1                        528 00101$:
                                    529 ;	main.c:52: main_print();
      0000E1 12 0D 73         [24]  530 	lcall	_main_print
                                    531 ;	main.c:54: case MODE_PASSCHECK:
      0000E4                        532 00102$:
                                    533 ;	main.c:55: main_passcheck();
      0000E4 12 0C 96         [24]  534 	lcall	_main_passcheck
                                    535 ;	main.c:57: case MODE_GLITCHLOOP:
      0000E7                        536 00103$:
                                    537 ;	main.c:58: main_glitchloop();
      0000E7 12 0B 72         [24]  538 	lcall	_main_glitchloop
                                    539 ;	main.c:60: case MODE_XOR:
      0000EA                        540 00104$:
                                    541 ;	main.c:61: main_xor();
      0000EA 12 1A D0         [24]  542 	lcall	_main_xor
                                    543 ;	main.c:63: case MODE_AES:
      0000ED                        544 00105$:
                                    545 ;	main.c:64: main_aes();
      0000ED 12 00 F8         [24]  546 	lcall	_main_aes
                                    547 ;	main.c:66: case MODE_TEA:
      0000F0                        548 00106$:
                                    549 ;	main.c:67: main_tea();
      0000F0 12 19 A5         [24]  550 	lcall	_main_tea
                                    551 ;	main.c:71: }
      0000F3                        552 00107$:
                                    553 ;	main.c:77: getchar();
      0000F3 12 00 A0         [24]  554 	lcall	_getchar
                                    555 ;	main.c:80: while(1);
      0000F6                        556 00109$:
      0000F6 80 FE            [24]  557 	sjmp	00109$
                                    558 	.area CSEG    (CODE)
                                    559 	.area CONST   (CODE)
                                    560 	.area XINIT   (CODE)
                                    561 	.area CABS    (ABS,CODE)
