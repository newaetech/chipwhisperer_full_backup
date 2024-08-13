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
                                     12 	.globl _func
                                     13 	.globl _printf_tiny
                                     14 	.globl _putchar
                                     15 	.globl _inituart
                                     16 	.globl _SIO_SM0
                                     17 	.globl _SIO_SM1
                                     18 	.globl _SIO_SM2
                                     19 	.globl _SIO_REN
                                     20 	.globl _SIO_TB8
                                     21 	.globl _SIO_RB8
                                     22 	.globl _SIO_TI
                                     23 	.globl _SIO_RI
                                     24 	.globl _SIO_TR1
                                     25 	.globl _TF2
                                     26 	.globl _EXF2
                                     27 	.globl _RCLK
                                     28 	.globl _TCLK
                                     29 	.globl _EXEN2
                                     30 	.globl _TR2
                                     31 	.globl _C_T2
                                     32 	.globl _CP_RL2
                                     33 	.globl _T2CON_7
                                     34 	.globl _T2CON_6
                                     35 	.globl _T2CON_5
                                     36 	.globl _T2CON_4
                                     37 	.globl _T2CON_3
                                     38 	.globl _T2CON_2
                                     39 	.globl _T2CON_1
                                     40 	.globl _T2CON_0
                                     41 	.globl _PT2
                                     42 	.globl _ET2
                                     43 	.globl _CY
                                     44 	.globl _AC
                                     45 	.globl _F0
                                     46 	.globl _RS1
                                     47 	.globl _RS0
                                     48 	.globl _OV
                                     49 	.globl _F1
                                     50 	.globl _P
                                     51 	.globl _PS
                                     52 	.globl _PT1
                                     53 	.globl _PX1
                                     54 	.globl _PT0
                                     55 	.globl _PX0
                                     56 	.globl _RD
                                     57 	.globl _WR
                                     58 	.globl _T1
                                     59 	.globl _T0
                                     60 	.globl _INT1
                                     61 	.globl _INT0
                                     62 	.globl _TXD
                                     63 	.globl _RXD
                                     64 	.globl _P3_7
                                     65 	.globl _P3_6
                                     66 	.globl _P3_5
                                     67 	.globl _P3_4
                                     68 	.globl _P3_3
                                     69 	.globl _P3_2
                                     70 	.globl _P3_1
                                     71 	.globl _P3_0
                                     72 	.globl _EA
                                     73 	.globl _ES
                                     74 	.globl _ET1
                                     75 	.globl _EX1
                                     76 	.globl _ET0
                                     77 	.globl _EX0
                                     78 	.globl _P2_7
                                     79 	.globl _P2_6
                                     80 	.globl _P2_5
                                     81 	.globl _P2_4
                                     82 	.globl _P2_3
                                     83 	.globl _P2_2
                                     84 	.globl _P2_1
                                     85 	.globl _P2_0
                                     86 	.globl _SM0
                                     87 	.globl _SM1
                                     88 	.globl _SM2
                                     89 	.globl _REN
                                     90 	.globl _TB8
                                     91 	.globl _RB8
                                     92 	.globl _TI
                                     93 	.globl _RI
                                     94 	.globl _P1_7
                                     95 	.globl _P1_6
                                     96 	.globl _P1_5
                                     97 	.globl _P1_4
                                     98 	.globl _P1_3
                                     99 	.globl _P1_2
                                    100 	.globl _P1_1
                                    101 	.globl _P1_0
                                    102 	.globl _TF1
                                    103 	.globl _TR1
                                    104 	.globl _TF0
                                    105 	.globl _TR0
                                    106 	.globl _IE1
                                    107 	.globl _IT1
                                    108 	.globl _IE0
                                    109 	.globl _IT0
                                    110 	.globl _P0_7
                                    111 	.globl _P0_6
                                    112 	.globl _P0_5
                                    113 	.globl _P0_4
                                    114 	.globl _P0_3
                                    115 	.globl _P0_2
                                    116 	.globl _P0_1
                                    117 	.globl _P0_0
                                    118 	.globl _SIO_SBUF
                                    119 	.globl _SIO_SCON
                                    120 	.globl _SIO_TL1
                                    121 	.globl _SIO_TH1
                                    122 	.globl _SIO_TMOD
                                    123 	.globl _SIO_PCON
                                    124 	.globl _TH2
                                    125 	.globl _TL2
                                    126 	.globl _RCAP2H
                                    127 	.globl _RCAP2L
                                    128 	.globl _T2CON
                                    129 	.globl _B
                                    130 	.globl _ACC
                                    131 	.globl _PSW
                                    132 	.globl _IP
                                    133 	.globl _P3
                                    134 	.globl _IE
                                    135 	.globl _P2
                                    136 	.globl _SBUF
                                    137 	.globl _SCON
                                    138 	.globl _P1
                                    139 	.globl _TH1
                                    140 	.globl _TH0
                                    141 	.globl _TL1
                                    142 	.globl _TL0
                                    143 	.globl _TMOD
                                    144 	.globl _TCON
                                    145 	.globl _PCON
                                    146 	.globl _DPH
                                    147 	.globl _DPL
                                    148 	.globl _SP
                                    149 	.globl _P0
                                    150 ;--------------------------------------------------------
                                    151 ; special function registers
                                    152 ;--------------------------------------------------------
                                    153 	.area RSEG    (ABS,DATA)
      000000                        154 	.org 0x0000
                           000080   155 _P0	=	0x0080
                           000081   156 _SP	=	0x0081
                           000082   157 _DPL	=	0x0082
                           000083   158 _DPH	=	0x0083
                           000087   159 _PCON	=	0x0087
                           000088   160 _TCON	=	0x0088
                           000089   161 _TMOD	=	0x0089
                           00008A   162 _TL0	=	0x008a
                           00008B   163 _TL1	=	0x008b
                           00008C   164 _TH0	=	0x008c
                           00008D   165 _TH1	=	0x008d
                           000090   166 _P1	=	0x0090
                           000098   167 _SCON	=	0x0098
                           000099   168 _SBUF	=	0x0099
                           0000A0   169 _P2	=	0x00a0
                           0000A8   170 _IE	=	0x00a8
                           0000B0   171 _P3	=	0x00b0
                           0000B8   172 _IP	=	0x00b8
                           0000D0   173 _PSW	=	0x00d0
                           0000E0   174 _ACC	=	0x00e0
                           0000F0   175 _B	=	0x00f0
                           0000C8   176 _T2CON	=	0x00c8
                           0000CA   177 _RCAP2L	=	0x00ca
                           0000CB   178 _RCAP2H	=	0x00cb
                           0000CC   179 _TL2	=	0x00cc
                           0000CD   180 _TH2	=	0x00cd
                           000087   181 _SIO_PCON	=	0x0087
                           000089   182 _SIO_TMOD	=	0x0089
                           00008D   183 _SIO_TH1	=	0x008d
                           00008B   184 _SIO_TL1	=	0x008b
                           000098   185 _SIO_SCON	=	0x0098
                           000099   186 _SIO_SBUF	=	0x0099
                                    187 ;--------------------------------------------------------
                                    188 ; special function bits
                                    189 ;--------------------------------------------------------
                                    190 	.area RSEG    (ABS,DATA)
      000000                        191 	.org 0x0000
                           000080   192 _P0_0	=	0x0080
                           000081   193 _P0_1	=	0x0081
                           000082   194 _P0_2	=	0x0082
                           000083   195 _P0_3	=	0x0083
                           000084   196 _P0_4	=	0x0084
                           000085   197 _P0_5	=	0x0085
                           000086   198 _P0_6	=	0x0086
                           000087   199 _P0_7	=	0x0087
                           000088   200 _IT0	=	0x0088
                           000089   201 _IE0	=	0x0089
                           00008A   202 _IT1	=	0x008a
                           00008B   203 _IE1	=	0x008b
                           00008C   204 _TR0	=	0x008c
                           00008D   205 _TF0	=	0x008d
                           00008E   206 _TR1	=	0x008e
                           00008F   207 _TF1	=	0x008f
                           000090   208 _P1_0	=	0x0090
                           000091   209 _P1_1	=	0x0091
                           000092   210 _P1_2	=	0x0092
                           000093   211 _P1_3	=	0x0093
                           000094   212 _P1_4	=	0x0094
                           000095   213 _P1_5	=	0x0095
                           000096   214 _P1_6	=	0x0096
                           000097   215 _P1_7	=	0x0097
                           000098   216 _RI	=	0x0098
                           000099   217 _TI	=	0x0099
                           00009A   218 _RB8	=	0x009a
                           00009B   219 _TB8	=	0x009b
                           00009C   220 _REN	=	0x009c
                           00009D   221 _SM2	=	0x009d
                           00009E   222 _SM1	=	0x009e
                           00009F   223 _SM0	=	0x009f
                           0000A0   224 _P2_0	=	0x00a0
                           0000A1   225 _P2_1	=	0x00a1
                           0000A2   226 _P2_2	=	0x00a2
                           0000A3   227 _P2_3	=	0x00a3
                           0000A4   228 _P2_4	=	0x00a4
                           0000A5   229 _P2_5	=	0x00a5
                           0000A6   230 _P2_6	=	0x00a6
                           0000A7   231 _P2_7	=	0x00a7
                           0000A8   232 _EX0	=	0x00a8
                           0000A9   233 _ET0	=	0x00a9
                           0000AA   234 _EX1	=	0x00aa
                           0000AB   235 _ET1	=	0x00ab
                           0000AC   236 _ES	=	0x00ac
                           0000AF   237 _EA	=	0x00af
                           0000B0   238 _P3_0	=	0x00b0
                           0000B1   239 _P3_1	=	0x00b1
                           0000B2   240 _P3_2	=	0x00b2
                           0000B3   241 _P3_3	=	0x00b3
                           0000B4   242 _P3_4	=	0x00b4
                           0000B5   243 _P3_5	=	0x00b5
                           0000B6   244 _P3_6	=	0x00b6
                           0000B7   245 _P3_7	=	0x00b7
                           0000B0   246 _RXD	=	0x00b0
                           0000B1   247 _TXD	=	0x00b1
                           0000B2   248 _INT0	=	0x00b2
                           0000B3   249 _INT1	=	0x00b3
                           0000B4   250 _T0	=	0x00b4
                           0000B5   251 _T1	=	0x00b5
                           0000B6   252 _WR	=	0x00b6
                           0000B7   253 _RD	=	0x00b7
                           0000B8   254 _PX0	=	0x00b8
                           0000B9   255 _PT0	=	0x00b9
                           0000BA   256 _PX1	=	0x00ba
                           0000BB   257 _PT1	=	0x00bb
                           0000BC   258 _PS	=	0x00bc
                           0000D0   259 _P	=	0x00d0
                           0000D1   260 _F1	=	0x00d1
                           0000D2   261 _OV	=	0x00d2
                           0000D3   262 _RS0	=	0x00d3
                           0000D4   263 _RS1	=	0x00d4
                           0000D5   264 _F0	=	0x00d5
                           0000D6   265 _AC	=	0x00d6
                           0000D7   266 _CY	=	0x00d7
                           0000AD   267 _ET2	=	0x00ad
                           0000BD   268 _PT2	=	0x00bd
                           0000C8   269 _T2CON_0	=	0x00c8
                           0000C9   270 _T2CON_1	=	0x00c9
                           0000CA   271 _T2CON_2	=	0x00ca
                           0000CB   272 _T2CON_3	=	0x00cb
                           0000CC   273 _T2CON_4	=	0x00cc
                           0000CD   274 _T2CON_5	=	0x00cd
                           0000CE   275 _T2CON_6	=	0x00ce
                           0000CF   276 _T2CON_7	=	0x00cf
                           0000C8   277 _CP_RL2	=	0x00c8
                           0000C9   278 _C_T2	=	0x00c9
                           0000CA   279 _TR2	=	0x00ca
                           0000CB   280 _EXEN2	=	0x00cb
                           0000CC   281 _TCLK	=	0x00cc
                           0000CD   282 _RCLK	=	0x00cd
                           0000CE   283 _EXF2	=	0x00ce
                           0000CF   284 _TF2	=	0x00cf
                           00008E   285 _SIO_TR1	=	0x008e
                           000098   286 _SIO_RI	=	0x0098
                           000099   287 _SIO_TI	=	0x0099
                           00009A   288 _SIO_RB8	=	0x009a
                           00009B   289 _SIO_TB8	=	0x009b
                           00009C   290 _SIO_REN	=	0x009c
                           00009D   291 _SIO_SM2	=	0x009d
                           00009E   292 _SIO_SM1	=	0x009e
                           00009F   293 _SIO_SM0	=	0x009f
                                    294 ;--------------------------------------------------------
                                    295 ; overlayable register banks
                                    296 ;--------------------------------------------------------
                                    297 	.area REG_BANK_0	(REL,OVR,DATA)
      000000                        298 	.ds 8
                                    299 ;--------------------------------------------------------
                                    300 ; internal ram data
                                    301 ;--------------------------------------------------------
                                    302 	.area DSEG    (DATA)
                                    303 ;--------------------------------------------------------
                                    304 ; overlayable items in internal ram 
                                    305 ;--------------------------------------------------------
                                    306 ;--------------------------------------------------------
                                    307 ; Stack segment in internal ram 
                                    308 ;--------------------------------------------------------
                                    309 	.area	SSEG
      000030                        310 __start__stack:
      000030                        311 	.ds	1
                                    312 
                                    313 ;--------------------------------------------------------
                                    314 ; indirectly addressable internal ram data
                                    315 ;--------------------------------------------------------
                                    316 	.area ISEG    (DATA)
                                    317 ;--------------------------------------------------------
                                    318 ; absolute internal ram data
                                    319 ;--------------------------------------------------------
                                    320 	.area IABS    (ABS,DATA)
                                    321 	.area IABS    (ABS,DATA)
                                    322 ;--------------------------------------------------------
                                    323 ; bit data
                                    324 ;--------------------------------------------------------
                                    325 	.area BSEG    (BIT)
                                    326 ;--------------------------------------------------------
                                    327 ; paged external ram data
                                    328 ;--------------------------------------------------------
                                    329 	.area PSEG    (PAG,XDATA)
                                    330 ;--------------------------------------------------------
                                    331 ; external ram data
                                    332 ;--------------------------------------------------------
                                    333 	.area XSEG    (XDATA)
                                    334 ;--------------------------------------------------------
                                    335 ; absolute external ram data
                                    336 ;--------------------------------------------------------
                                    337 	.area XABS    (ABS,XDATA)
                                    338 ;--------------------------------------------------------
                                    339 ; external initialized ram data
                                    340 ;--------------------------------------------------------
                                    341 	.area XISEG   (XDATA)
                                    342 	.area HOME    (CODE)
                                    343 	.area GSINIT0 (CODE)
                                    344 	.area GSINIT1 (CODE)
                                    345 	.area GSINIT2 (CODE)
                                    346 	.area GSINIT3 (CODE)
                                    347 	.area GSINIT4 (CODE)
                                    348 	.area GSINIT5 (CODE)
                                    349 	.area GSINIT  (CODE)
                                    350 	.area GSFINAL (CODE)
                                    351 	.area CSEG    (CODE)
                                    352 ;--------------------------------------------------------
                                    353 ; interrupt vector 
                                    354 ;--------------------------------------------------------
                                    355 	.area HOME    (CODE)
      000000                        356 __interrupt_vect:
      000000 02 00 06         [24]  357 	ljmp	__sdcc_gsinit_startup
                                    358 ;--------------------------------------------------------
                                    359 ; global & static initialisations
                                    360 ;--------------------------------------------------------
                                    361 	.area HOME    (CODE)
                                    362 	.area GSINIT  (CODE)
                                    363 	.area GSFINAL (CODE)
                                    364 	.area GSINIT  (CODE)
                                    365 	.globl __sdcc_gsinit_startup
                                    366 	.globl __sdcc_program_startup
                                    367 	.globl __start__stack
                                    368 	.globl __mcs51_genXINIT
                                    369 	.globl __mcs51_genXRAMCLEAR
                                    370 	.globl __mcs51_genRAMCLEAR
                                    371 	.area GSFINAL (CODE)
      00005F 02 00 03         [24]  372 	ljmp	__sdcc_program_startup
                                    373 ;--------------------------------------------------------
                                    374 ; Home
                                    375 ;--------------------------------------------------------
                                    376 	.area HOME    (CODE)
                                    377 	.area HOME    (CODE)
      000003                        378 __sdcc_program_startup:
      000003 02 00 B6         [24]  379 	ljmp	_main
                                    380 ;	return from main will return to caller
                                    381 ;--------------------------------------------------------
                                    382 ; code
                                    383 ;--------------------------------------------------------
                                    384 	.area CSEG    (CODE)
                                    385 ;------------------------------------------------------------
                                    386 ;Allocation info for local variables in function 'inituart'
                                    387 ;------------------------------------------------------------
                                    388 ;t1_reload                 Allocated to registers r7 
                                    389 ;------------------------------------------------------------
                                    390 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:50: void inituart (unsigned char t1_reload)
                                    391 ;	-----------------------------------------
                                    392 ;	 function inituart
                                    393 ;	-----------------------------------------
      000062                        394 _inituart:
                           000007   395 	ar7 = 0x07
                           000006   396 	ar6 = 0x06
                           000005   397 	ar5 = 0x05
                           000004   398 	ar4 = 0x04
                           000003   399 	ar3 = 0x03
                           000002   400 	ar2 = 0x02
                           000001   401 	ar1 = 0x01
                           000000   402 	ar0 = 0x00
      000062 AF 82            [24]  403 	mov	r7,dpl
                                    404 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:52: SIO_TR1=0;
      000064 C2 8E            [12]  405 	clr	_SIO_TR1
                                    406 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:53: SIO_TMOD=(SIO_TMOD&0x0f)|0x20;
      000066 74 0F            [12]  407 	mov	a,#0x0f
      000068 55 89            [12]  408 	anl	a,_SIO_TMOD
      00006A 44 20            [12]  409 	orl	a,#0x20
      00006C F5 89            [12]  410 	mov	_SIO_TMOD,a
                                    411 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:54: SIO_PCON|=0x80;
      00006E 43 87 80         [24]  412 	orl	_SIO_PCON,#0x80
                                    413 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:55: SIO_TH1=SIO_TL1=t1_reload;
      000071 8F 8B            [24]  414 	mov	_SIO_TL1,r7
      000073 8F 8D            [24]  415 	mov	_SIO_TH1,r7
                                    416 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:56: SIO_TR1=1;
      000075 D2 8E            [12]  417 	setb	_SIO_TR1
                                    418 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:57: SIO_SCON=0x52;
      000077 75 98 52         [24]  419 	mov	_SIO_SCON,#0x52
      00007A 22               [24]  420 	ret
                                    421 ;------------------------------------------------------------
                                    422 ;Allocation info for local variables in function 'putchar'
                                    423 ;------------------------------------------------------------
                                    424 ;c                         Allocated to registers r7 
                                    425 ;------------------------------------------------------------
                                    426 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:60: void putchar (char c)
                                    427 ;	-----------------------------------------
                                    428 ;	 function putchar
                                    429 ;	-----------------------------------------
      00007B                        430 _putchar:
      00007B AF 82            [24]  431 	mov	r7,dpl
                                    432 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:62: if((!SIO_SM0)&&(!SIO_SM1)) inituart(0xff);
      00007D 20 9F 0D         [24]  433 	jb	_SIO_SM0,00102$
      000080 20 9E 0A         [24]  434 	jb	_SIO_SM1,00102$
      000083 75 82 FF         [24]  435 	mov	dpl,#0xff
      000086 C0 07            [24]  436 	push	ar7
      000088 12 00 62         [24]  437 	lcall	_inituart
      00008B D0 07            [24]  438 	pop	ar7
      00008D                        439 00102$:
                                    440 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:63: if (c=='\n')
      00008D BF 0A 08         [24]  441 	cjne	r7,#0x0a,00109$
                                    442 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:65: while (!SIO_TI);
      000090                        443 00104$:
                                    444 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:66: SIO_TI=0;
      000090 10 99 02         [24]  445 	jbc	_SIO_TI,00136$
      000093 80 FB            [24]  446 	sjmp	00104$
      000095                        447 00136$:
                                    448 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:67: SIO_SBUF='\r';
      000095 75 99 0D         [24]  449 	mov	_SIO_SBUF,#0x0d
                                    450 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:69: while (!SIO_TI);
      000098                        451 00109$:
                                    452 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:70: SIO_TI=0;
      000098 10 99 02         [24]  453 	jbc	_SIO_TI,00137$
      00009B 80 FB            [24]  454 	sjmp	00109$
      00009D                        455 00137$:
                                    456 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:71: SIO_SBUF=c;
      00009D 8F 99            [24]  457 	mov	_SIO_SBUF,r7
      00009F 22               [24]  458 	ret
                                    459 ;------------------------------------------------------------
                                    460 ;Allocation info for local variables in function 'getchar'
                                    461 ;------------------------------------------------------------
                                    462 ;c                         Allocated to registers 
                                    463 ;------------------------------------------------------------
                                    464 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:74: char getchar (void)
                                    465 ;	-----------------------------------------
                                    466 ;	 function getchar
                                    467 ;	-----------------------------------------
      0000A0                        468 _getchar:
                                    469 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:78: if((!SIO_SM0)&&(!SIO_SM1)) inituart(0xff);
      0000A0 20 9F 09         [24]  470 	jb	_SIO_SM0,00104$
      0000A3 20 9E 06         [24]  471 	jb	_SIO_SM1,00104$
      0000A6 75 82 FF         [24]  472 	mov	dpl,#0xff
      0000A9 12 00 62         [24]  473 	lcall	_inituart
                                    474 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:80: while (!SIO_RI);
      0000AC                        475 00104$:
                                    476 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:81: SIO_RI=0;
      0000AC 10 98 02         [24]  477 	jbc	_SIO_RI,00123$
      0000AF 80 FB            [24]  478 	sjmp	00104$
      0000B1                        479 00123$:
                                    480 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:82: c=SIO_SBUF;
      0000B1 85 99 82         [24]  481 	mov	dpl,_SIO_SBUF
                                    482 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:83: return c;
      0000B4 22               [24]  483 	ret
                                    484 ;------------------------------------------------------------
                                    485 ;Allocation info for local variables in function 'func'
                                    486 ;------------------------------------------------------------
                                    487 ;x                         Allocated to registers 
                                    488 ;y                         Allocated to registers 
                                    489 ;------------------------------------------------------------
                                    490 ;	main.c:9: void func()
                                    491 ;	-----------------------------------------
                                    492 ;	 function func
                                    493 ;	-----------------------------------------
      0000B5                        494 _func:
                                    495 ;	main.c:13: x += 2;
      0000B5 22               [24]  496 	ret
                                    497 ;------------------------------------------------------------
                                    498 ;Allocation info for local variables in function 'main'
                                    499 ;------------------------------------------------------------
                                    500 ;x                         Allocated to registers r6 r7 
                                    501 ;------------------------------------------------------------
                                    502 ;	main.c:16: void main (void) 
                                    503 ;	-----------------------------------------
                                    504 ;	 function main
                                    505 ;	-----------------------------------------
      0000B6                        506 _main:
                                    507 ;	main.c:20: int x = 0;
      0000B6 7E 00            [12]  508 	mov	r6,#0x00
      0000B8 7F 00            [12]  509 	mov	r7,#0x00
                                    510 ;	main.c:23: inituart(248);
      0000BA 75 82 F8         [24]  511 	mov	dpl,#0xf8
      0000BD C0 07            [24]  512 	push	ar7
      0000BF C0 06            [24]  513 	push	ar6
      0000C1 12 00 62         [24]  514 	lcall	_inituart
                                    515 ;	main.c:26: putchar('\n');
      0000C4 75 82 0A         [24]  516 	mov	dpl,#0x0a
      0000C7 12 00 7B         [24]  517 	lcall	_putchar
      0000CA D0 06            [24]  518 	pop	ar6
      0000CC D0 07            [24]  519 	pop	ar7
                                    520 ;	main.c:29: while(1)
      0000CE                        521 00102$:
                                    522 ;	main.c:31: printf_tiny("Testing %d\n", x);
      0000CE C0 07            [24]  523 	push	ar7
      0000D0 C0 06            [24]  524 	push	ar6
      0000D2 C0 06            [24]  525 	push	ar6
      0000D4 C0 07            [24]  526 	push	ar7
      0000D6 74 1B            [12]  527 	mov	a,#___str_0
      0000D8 C0 E0            [24]  528 	push	acc
      0000DA 74 02            [12]  529 	mov	a,#(___str_0 >> 8)
      0000DC C0 E0            [24]  530 	push	acc
      0000DE 12 00 F2         [24]  531 	lcall	_printf_tiny
      0000E1 E5 81            [12]  532 	mov	a,sp
      0000E3 24 FC            [12]  533 	add	a,#0xfc
      0000E5 F5 81            [12]  534 	mov	sp,a
      0000E7 D0 06            [24]  535 	pop	ar6
      0000E9 D0 07            [24]  536 	pop	ar7
                                    537 ;	main.c:32: x += 1;
      0000EB 0E               [12]  538 	inc	r6
      0000EC BE 00 DF         [24]  539 	cjne	r6,#0x00,00102$
      0000EF 0F               [12]  540 	inc	r7
      0000F0 80 DC            [24]  541 	sjmp	00102$
                                    542 	.area CSEG    (CODE)
                                    543 	.area CONST   (CODE)
      00021B                        544 ___str_0:
      00021B 54 65 73 74 69 6E 67   545 	.ascii "Testing %d"
             20 25 64
      000225 0A                     546 	.db 0x0a
      000226 00                     547 	.db 0x00
                                    548 	.area XINIT   (CODE)
                                    549 	.area CABS    (ABS,CODE)
