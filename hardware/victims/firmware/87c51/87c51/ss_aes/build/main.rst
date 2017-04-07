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
                                     12 	.globl _aes_encrypt
                                     13 	.globl _simpleserial_put
                                     14 	.globl _simpleserial_get
                                     15 	.globl _inituart
                                     16 	.globl _TF2
                                     17 	.globl _EXF2
                                     18 	.globl _RCLK
                                     19 	.globl _TCLK
                                     20 	.globl _EXEN2
                                     21 	.globl _TR2
                                     22 	.globl _C_T2
                                     23 	.globl _CP_RL2
                                     24 	.globl _T2CON_7
                                     25 	.globl _T2CON_6
                                     26 	.globl _T2CON_5
                                     27 	.globl _T2CON_4
                                     28 	.globl _T2CON_3
                                     29 	.globl _T2CON_2
                                     30 	.globl _T2CON_1
                                     31 	.globl _T2CON_0
                                     32 	.globl _PT2
                                     33 	.globl _ET2
                                     34 	.globl _CY
                                     35 	.globl _AC
                                     36 	.globl _F0
                                     37 	.globl _RS1
                                     38 	.globl _RS0
                                     39 	.globl _OV
                                     40 	.globl _F1
                                     41 	.globl _P
                                     42 	.globl _PS
                                     43 	.globl _PT1
                                     44 	.globl _PX1
                                     45 	.globl _PT0
                                     46 	.globl _PX0
                                     47 	.globl _RD
                                     48 	.globl _WR
                                     49 	.globl _T1
                                     50 	.globl _T0
                                     51 	.globl _INT1
                                     52 	.globl _INT0
                                     53 	.globl _TXD
                                     54 	.globl _RXD
                                     55 	.globl _P3_7
                                     56 	.globl _P3_6
                                     57 	.globl _P3_5
                                     58 	.globl _P3_4
                                     59 	.globl _P3_3
                                     60 	.globl _P3_2
                                     61 	.globl _P3_1
                                     62 	.globl _P3_0
                                     63 	.globl _EA
                                     64 	.globl _ES
                                     65 	.globl _ET1
                                     66 	.globl _EX1
                                     67 	.globl _ET0
                                     68 	.globl _EX0
                                     69 	.globl _P2_7
                                     70 	.globl _P2_6
                                     71 	.globl _P2_5
                                     72 	.globl _P2_4
                                     73 	.globl _P2_3
                                     74 	.globl _P2_2
                                     75 	.globl _P2_1
                                     76 	.globl _P2_0
                                     77 	.globl _SM0
                                     78 	.globl _SM1
                                     79 	.globl _SM2
                                     80 	.globl _REN
                                     81 	.globl _TB8
                                     82 	.globl _RB8
                                     83 	.globl _TI
                                     84 	.globl _RI
                                     85 	.globl _P1_7
                                     86 	.globl _P1_6
                                     87 	.globl _P1_5
                                     88 	.globl _P1_4
                                     89 	.globl _P1_3
                                     90 	.globl _P1_2
                                     91 	.globl _P1_1
                                     92 	.globl _P1_0
                                     93 	.globl _TF1
                                     94 	.globl _TR1
                                     95 	.globl _TF0
                                     96 	.globl _TR0
                                     97 	.globl _IE1
                                     98 	.globl _IT1
                                     99 	.globl _IE0
                                    100 	.globl _IT0
                                    101 	.globl _P0_7
                                    102 	.globl _P0_6
                                    103 	.globl _P0_5
                                    104 	.globl _P0_4
                                    105 	.globl _P0_3
                                    106 	.globl _P0_2
                                    107 	.globl _P0_1
                                    108 	.globl _P0_0
                                    109 	.globl _SIO_SM0
                                    110 	.globl _SIO_SM1
                                    111 	.globl _SIO_SM2
                                    112 	.globl _SIO_REN
                                    113 	.globl _SIO_TB8
                                    114 	.globl _SIO_RB8
                                    115 	.globl _SIO_TI
                                    116 	.globl _SIO_RI
                                    117 	.globl _SIO_TR1
                                    118 	.globl _TH2
                                    119 	.globl _TL2
                                    120 	.globl _RCAP2H
                                    121 	.globl _RCAP2L
                                    122 	.globl _T2CON
                                    123 	.globl _B
                                    124 	.globl _ACC
                                    125 	.globl _PSW
                                    126 	.globl _IP
                                    127 	.globl _P3
                                    128 	.globl _IE
                                    129 	.globl _P2
                                    130 	.globl _SBUF
                                    131 	.globl _SCON
                                    132 	.globl _P1
                                    133 	.globl _TH1
                                    134 	.globl _TH0
                                    135 	.globl _TL1
                                    136 	.globl _TL0
                                    137 	.globl _TMOD
                                    138 	.globl _TCON
                                    139 	.globl _PCON
                                    140 	.globl _DPH
                                    141 	.globl _DPL
                                    142 	.globl _SP
                                    143 	.globl _P0
                                    144 	.globl _SIO_SBUF
                                    145 	.globl _SIO_SCON
                                    146 	.globl _SIO_TL1
                                    147 	.globl _SIO_TH1
                                    148 	.globl _SIO_TMOD
                                    149 	.globl _SIO_PCON
                                    150 	.globl _putchar
                                    151 	.globl _getchar
                                    152 ;--------------------------------------------------------
                                    153 ; special function registers
                                    154 ;--------------------------------------------------------
                                    155 	.area RSEG    (ABS,DATA)
      000000                        156 	.org 0x0000
                           000087   157 _SIO_PCON	=	0x0087
                           000089   158 _SIO_TMOD	=	0x0089
                           00008D   159 _SIO_TH1	=	0x008d
                           00008B   160 _SIO_TL1	=	0x008b
                           000098   161 _SIO_SCON	=	0x0098
                           000099   162 _SIO_SBUF	=	0x0099
                           000080   163 _P0	=	0x0080
                           000081   164 _SP	=	0x0081
                           000082   165 _DPL	=	0x0082
                           000083   166 _DPH	=	0x0083
                           000087   167 _PCON	=	0x0087
                           000088   168 _TCON	=	0x0088
                           000089   169 _TMOD	=	0x0089
                           00008A   170 _TL0	=	0x008a
                           00008B   171 _TL1	=	0x008b
                           00008C   172 _TH0	=	0x008c
                           00008D   173 _TH1	=	0x008d
                           000090   174 _P1	=	0x0090
                           000098   175 _SCON	=	0x0098
                           000099   176 _SBUF	=	0x0099
                           0000A0   177 _P2	=	0x00a0
                           0000A8   178 _IE	=	0x00a8
                           0000B0   179 _P3	=	0x00b0
                           0000B8   180 _IP	=	0x00b8
                           0000D0   181 _PSW	=	0x00d0
                           0000E0   182 _ACC	=	0x00e0
                           0000F0   183 _B	=	0x00f0
                           0000C8   184 _T2CON	=	0x00c8
                           0000CA   185 _RCAP2L	=	0x00ca
                           0000CB   186 _RCAP2H	=	0x00cb
                           0000CC   187 _TL2	=	0x00cc
                           0000CD   188 _TH2	=	0x00cd
                                    189 ;--------------------------------------------------------
                                    190 ; special function bits
                                    191 ;--------------------------------------------------------
                                    192 	.area RSEG    (ABS,DATA)
      000000                        193 	.org 0x0000
                           00008E   194 _SIO_TR1	=	0x008e
                           000098   195 _SIO_RI	=	0x0098
                           000099   196 _SIO_TI	=	0x0099
                           00009A   197 _SIO_RB8	=	0x009a
                           00009B   198 _SIO_TB8	=	0x009b
                           00009C   199 _SIO_REN	=	0x009c
                           00009D   200 _SIO_SM2	=	0x009d
                           00009E   201 _SIO_SM1	=	0x009e
                           00009F   202 _SIO_SM0	=	0x009f
                           000080   203 _P0_0	=	0x0080
                           000081   204 _P0_1	=	0x0081
                           000082   205 _P0_2	=	0x0082
                           000083   206 _P0_3	=	0x0083
                           000084   207 _P0_4	=	0x0084
                           000085   208 _P0_5	=	0x0085
                           000086   209 _P0_6	=	0x0086
                           000087   210 _P0_7	=	0x0087
                           000088   211 _IT0	=	0x0088
                           000089   212 _IE0	=	0x0089
                           00008A   213 _IT1	=	0x008a
                           00008B   214 _IE1	=	0x008b
                           00008C   215 _TR0	=	0x008c
                           00008D   216 _TF0	=	0x008d
                           00008E   217 _TR1	=	0x008e
                           00008F   218 _TF1	=	0x008f
                           000090   219 _P1_0	=	0x0090
                           000091   220 _P1_1	=	0x0091
                           000092   221 _P1_2	=	0x0092
                           000093   222 _P1_3	=	0x0093
                           000094   223 _P1_4	=	0x0094
                           000095   224 _P1_5	=	0x0095
                           000096   225 _P1_6	=	0x0096
                           000097   226 _P1_7	=	0x0097
                           000098   227 _RI	=	0x0098
                           000099   228 _TI	=	0x0099
                           00009A   229 _RB8	=	0x009a
                           00009B   230 _TB8	=	0x009b
                           00009C   231 _REN	=	0x009c
                           00009D   232 _SM2	=	0x009d
                           00009E   233 _SM1	=	0x009e
                           00009F   234 _SM0	=	0x009f
                           0000A0   235 _P2_0	=	0x00a0
                           0000A1   236 _P2_1	=	0x00a1
                           0000A2   237 _P2_2	=	0x00a2
                           0000A3   238 _P2_3	=	0x00a3
                           0000A4   239 _P2_4	=	0x00a4
                           0000A5   240 _P2_5	=	0x00a5
                           0000A6   241 _P2_6	=	0x00a6
                           0000A7   242 _P2_7	=	0x00a7
                           0000A8   243 _EX0	=	0x00a8
                           0000A9   244 _ET0	=	0x00a9
                           0000AA   245 _EX1	=	0x00aa
                           0000AB   246 _ET1	=	0x00ab
                           0000AC   247 _ES	=	0x00ac
                           0000AF   248 _EA	=	0x00af
                           0000B0   249 _P3_0	=	0x00b0
                           0000B1   250 _P3_1	=	0x00b1
                           0000B2   251 _P3_2	=	0x00b2
                           0000B3   252 _P3_3	=	0x00b3
                           0000B4   253 _P3_4	=	0x00b4
                           0000B5   254 _P3_5	=	0x00b5
                           0000B6   255 _P3_6	=	0x00b6
                           0000B7   256 _P3_7	=	0x00b7
                           0000B0   257 _RXD	=	0x00b0
                           0000B1   258 _TXD	=	0x00b1
                           0000B2   259 _INT0	=	0x00b2
                           0000B3   260 _INT1	=	0x00b3
                           0000B4   261 _T0	=	0x00b4
                           0000B5   262 _T1	=	0x00b5
                           0000B6   263 _WR	=	0x00b6
                           0000B7   264 _RD	=	0x00b7
                           0000B8   265 _PX0	=	0x00b8
                           0000B9   266 _PT0	=	0x00b9
                           0000BA   267 _PX1	=	0x00ba
                           0000BB   268 _PT1	=	0x00bb
                           0000BC   269 _PS	=	0x00bc
                           0000D0   270 _P	=	0x00d0
                           0000D1   271 _F1	=	0x00d1
                           0000D2   272 _OV	=	0x00d2
                           0000D3   273 _RS0	=	0x00d3
                           0000D4   274 _RS1	=	0x00d4
                           0000D5   275 _F0	=	0x00d5
                           0000D6   276 _AC	=	0x00d6
                           0000D7   277 _CY	=	0x00d7
                           0000AD   278 _ET2	=	0x00ad
                           0000BD   279 _PT2	=	0x00bd
                           0000C8   280 _T2CON_0	=	0x00c8
                           0000C9   281 _T2CON_1	=	0x00c9
                           0000CA   282 _T2CON_2	=	0x00ca
                           0000CB   283 _T2CON_3	=	0x00cb
                           0000CC   284 _T2CON_4	=	0x00cc
                           0000CD   285 _T2CON_5	=	0x00cd
                           0000CE   286 _T2CON_6	=	0x00ce
                           0000CF   287 _T2CON_7	=	0x00cf
                           0000C8   288 _CP_RL2	=	0x00c8
                           0000C9   289 _C_T2	=	0x00c9
                           0000CA   290 _TR2	=	0x00ca
                           0000CB   291 _EXEN2	=	0x00cb
                           0000CC   292 _TCLK	=	0x00cc
                           0000CD   293 _RCLK	=	0x00cd
                           0000CE   294 _EXF2	=	0x00ce
                           0000CF   295 _TF2	=	0x00cf
                                    296 ;--------------------------------------------------------
                                    297 ; overlayable register banks
                                    298 ;--------------------------------------------------------
                                    299 	.area REG_BANK_0	(REL,OVR,DATA)
      000000                        300 	.ds 8
                                    301 ;--------------------------------------------------------
                                    302 ; internal ram data
                                    303 ;--------------------------------------------------------
                                    304 	.area DSEG    (DATA)
                                    305 ;--------------------------------------------------------
                                    306 ; overlayable items in internal ram 
                                    307 ;--------------------------------------------------------
                                    308 ;--------------------------------------------------------
                                    309 ; Stack segment in internal ram 
                                    310 ;--------------------------------------------------------
                                    311 	.area	SSEG
      000009                        312 __start__stack:
      000009                        313 	.ds	1
                                    314 
                                    315 ;--------------------------------------------------------
                                    316 ; indirectly addressable internal ram data
                                    317 ;--------------------------------------------------------
                                    318 	.area ISEG    (DATA)
                                    319 ;--------------------------------------------------------
                                    320 ; absolute internal ram data
                                    321 ;--------------------------------------------------------
                                    322 	.area IABS    (ABS,DATA)
                                    323 	.area IABS    (ABS,DATA)
                                    324 ;--------------------------------------------------------
                                    325 ; bit data
                                    326 ;--------------------------------------------------------
                                    327 	.area BSEG    (BIT)
                                    328 ;--------------------------------------------------------
                                    329 ; paged external ram data
                                    330 ;--------------------------------------------------------
                                    331 	.area PSEG    (PAG,XDATA)
                                    332 ;--------------------------------------------------------
                                    333 ; external ram data
                                    334 ;--------------------------------------------------------
                                    335 	.area XSEG    (XDATA)
                                    336 ;--------------------------------------------------------
                                    337 ; absolute external ram data
                                    338 ;--------------------------------------------------------
                                    339 	.area XABS    (ABS,XDATA)
                                    340 ;--------------------------------------------------------
                                    341 ; external initialized ram data
                                    342 ;--------------------------------------------------------
                                    343 	.area XISEG   (XDATA)
                                    344 	.area HOME    (CODE)
                                    345 	.area GSINIT0 (CODE)
                                    346 	.area GSINIT1 (CODE)
                                    347 	.area GSINIT2 (CODE)
                                    348 	.area GSINIT3 (CODE)
                                    349 	.area GSINIT4 (CODE)
                                    350 	.area GSINIT5 (CODE)
                                    351 	.area GSINIT  (CODE)
                                    352 	.area GSFINAL (CODE)
                                    353 	.area CSEG    (CODE)
                                    354 ;--------------------------------------------------------
                                    355 ; interrupt vector 
                                    356 ;--------------------------------------------------------
                                    357 	.area HOME    (CODE)
      000000                        358 __interrupt_vect:
      000000 02 00 06         [24]  359 	ljmp	__sdcc_gsinit_startup
                                    360 ;--------------------------------------------------------
                                    361 ; global & static initialisations
                                    362 ;--------------------------------------------------------
                                    363 	.area HOME    (CODE)
                                    364 	.area GSINIT  (CODE)
                                    365 	.area GSFINAL (CODE)
                                    366 	.area GSINIT  (CODE)
                                    367 	.globl __sdcc_gsinit_startup
                                    368 	.globl __sdcc_program_startup
                                    369 	.globl __start__stack
                                    370 	.globl __mcs51_genXINIT
                                    371 	.globl __mcs51_genXRAMCLEAR
                                    372 	.globl __mcs51_genRAMCLEAR
                                    373 	.area GSFINAL (CODE)
      00005F 02 00 03         [24]  374 	ljmp	__sdcc_program_startup
                                    375 ;--------------------------------------------------------
                                    376 ; Home
                                    377 ;--------------------------------------------------------
                                    378 	.area HOME    (CODE)
                                    379 	.area HOME    (CODE)
      000003                        380 __sdcc_program_startup:
      000003 02 00 B5         [24]  381 	ljmp	_main
                                    382 ;	return from main will return to caller
                                    383 ;--------------------------------------------------------
                                    384 ; code
                                    385 ;--------------------------------------------------------
                                    386 	.area CSEG    (CODE)
                                    387 ;------------------------------------------------------------
                                    388 ;Allocation info for local variables in function 'inituart'
                                    389 ;------------------------------------------------------------
                                    390 ;t1_reload                 Allocated to registers r7 
                                    391 ;------------------------------------------------------------
                                    392 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:50: void inituart (unsigned char t1_reload)
                                    393 ;	-----------------------------------------
                                    394 ;	 function inituart
                                    395 ;	-----------------------------------------
      000062                        396 _inituart:
                           000007   397 	ar7 = 0x07
                           000006   398 	ar6 = 0x06
                           000005   399 	ar5 = 0x05
                           000004   400 	ar4 = 0x04
                           000003   401 	ar3 = 0x03
                           000002   402 	ar2 = 0x02
                           000001   403 	ar1 = 0x01
                           000000   404 	ar0 = 0x00
      000062 AF 82            [24]  405 	mov	r7,dpl
                                    406 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:52: SIO_TR1=0;
      000064 C2 8E            [12]  407 	clr	_SIO_TR1
                                    408 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:53: SIO_TMOD=(SIO_TMOD&0x0f)|0x20;
      000066 74 0F            [12]  409 	mov	a,#0x0f
      000068 55 89            [12]  410 	anl	a,_SIO_TMOD
      00006A 44 20            [12]  411 	orl	a,#0x20
      00006C F5 89            [12]  412 	mov	_SIO_TMOD,a
                                    413 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:54: SIO_PCON|=0x80;
      00006E 43 87 80         [24]  414 	orl	_SIO_PCON,#0x80
                                    415 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:55: SIO_TH1=SIO_TL1=t1_reload;
      000071 8F 8B            [24]  416 	mov	_SIO_TL1,r7
      000073 8F 8D            [24]  417 	mov	_SIO_TH1,r7
                                    418 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:56: SIO_TR1=1;
      000075 D2 8E            [12]  419 	setb	_SIO_TR1
                                    420 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:57: SIO_SCON=0x52;
      000077 75 98 52         [24]  421 	mov	_SIO_SCON,#0x52
      00007A 22               [24]  422 	ret
                                    423 ;------------------------------------------------------------
                                    424 ;Allocation info for local variables in function 'putchar'
                                    425 ;------------------------------------------------------------
                                    426 ;c                         Allocated to registers r7 
                                    427 ;------------------------------------------------------------
                                    428 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:60: void putchar (char c)
                                    429 ;	-----------------------------------------
                                    430 ;	 function putchar
                                    431 ;	-----------------------------------------
      00007B                        432 _putchar:
      00007B AF 82            [24]  433 	mov	r7,dpl
                                    434 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:62: if((!SIO_SM0)&&(!SIO_SM1)) inituart(0xff);
      00007D 20 9F 0D         [24]  435 	jb	_SIO_SM0,00102$
      000080 20 9E 0A         [24]  436 	jb	_SIO_SM1,00102$
      000083 75 82 FF         [24]  437 	mov	dpl,#0xff
      000086 C0 07            [24]  438 	push	ar7
      000088 12 00 62         [24]  439 	lcall	_inituart
      00008B D0 07            [24]  440 	pop	ar7
      00008D                        441 00102$:
                                    442 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:63: if (c=='\n')
      00008D BF 0A 08         [24]  443 	cjne	r7,#0x0a,00109$
                                    444 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:65: while (!SIO_TI);
      000090                        445 00104$:
                                    446 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:66: SIO_TI=0;
      000090 10 99 02         [24]  447 	jbc	_SIO_TI,00136$
      000093 80 FB            [24]  448 	sjmp	00104$
      000095                        449 00136$:
                                    450 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:67: SIO_SBUF='\r';
      000095 75 99 0D         [24]  451 	mov	_SIO_SBUF,#0x0d
                                    452 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:69: while (!SIO_TI);
      000098                        453 00109$:
                                    454 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:70: SIO_TI=0;
      000098 10 99 02         [24]  455 	jbc	_SIO_TI,00137$
      00009B 80 FB            [24]  456 	sjmp	00109$
      00009D                        457 00137$:
                                    458 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:71: SIO_SBUF=c;
      00009D 8F 99            [24]  459 	mov	_SIO_SBUF,r7
      00009F 22               [24]  460 	ret
                                    461 ;------------------------------------------------------------
                                    462 ;Allocation info for local variables in function 'getchar'
                                    463 ;------------------------------------------------------------
                                    464 ;c                         Allocated to registers 
                                    465 ;------------------------------------------------------------
                                    466 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:74: char getchar (void)
                                    467 ;	-----------------------------------------
                                    468 ;	 function getchar
                                    469 ;	-----------------------------------------
      0000A0                        470 _getchar:
                                    471 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:78: if((!SIO_SM0)&&(!SIO_SM1)) inituart(0xff);
      0000A0 20 9F 09         [24]  472 	jb	_SIO_SM0,00104$
      0000A3 20 9E 06         [24]  473 	jb	_SIO_SM1,00104$
      0000A6 75 82 FF         [24]  474 	mov	dpl,#0xff
      0000A9 12 00 62         [24]  475 	lcall	_inituart
                                    476 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:80: while (!SIO_RI);
      0000AC                        477 00104$:
                                    478 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:81: SIO_RI=0;
      0000AC 10 98 02         [24]  479 	jbc	_SIO_RI,00123$
      0000AF 80 FB            [24]  480 	sjmp	00104$
      0000B1                        481 00123$:
                                    482 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:82: c=SIO_SBUF;
      0000B1 85 99 82         [24]  483 	mov	dpl,_SIO_SBUF
                                    484 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:83: return c;
      0000B4 22               [24]  485 	ret
                                    486 ;------------------------------------------------------------
                                    487 ;Allocation info for local variables in function 'main'
                                    488 ;------------------------------------------------------------
                                    489 ;input                     Allocated to stack - _bp +4
                                    490 ;key                       Allocated to stack - _bp +20
                                    491 ;sloc0                     Allocated to stack - _bp +1
                                    492 ;------------------------------------------------------------
                                    493 ;	main.c:20: void main(void) 
                                    494 ;	-----------------------------------------
                                    495 ;	 function main
                                    496 ;	-----------------------------------------
      0000B5                        497 _main:
      0000B5 C0 08            [24]  498 	push	_bp
      0000B7 E5 81            [12]  499 	mov	a,sp
      0000B9 F5 08            [12]  500 	mov	_bp,a
      0000BB 24 23            [12]  501 	add	a,#0x23
      0000BD F5 81            [12]  502 	mov	sp,a
                                    503 ;	main.c:27: inituart(248);	// 4800 baud
      0000BF 75 82 F8         [24]  504 	mov	dpl,#0xf8
      0000C2 12 00 62         [24]  505 	lcall	_inituart
                                    506 ;	main.c:28: trigger_low();
      0000C5 53 90 FE         [24]  507 	anl	_P1,#0xfe
                                    508 ;	main.c:31: putchar('\n');
      0000C8 75 82 0A         [24]  509 	mov	dpl,#0x0a
      0000CB 12 00 7B         [24]  510 	lcall	_putchar
                                    511 ;	main.c:34: while(1)
      0000CE                        512 00104$:
                                    513 ;	main.c:37: if(simpleserial_get(input, key))
      0000CE E5 08            [12]  514 	mov	a,_bp
      0000D0 24 14            [12]  515 	add	a,#0x14
      0000D2 FF               [12]  516 	mov	r7,a
      0000D3 A8 08            [24]  517 	mov	r0,_bp
      0000D5 08               [12]  518 	inc	r0
      0000D6 A6 07            [24]  519 	mov	@r0,ar7
      0000D8 08               [12]  520 	inc	r0
      0000D9 76 00            [12]  521 	mov	@r0,#0x00
      0000DB 08               [12]  522 	inc	r0
      0000DC 76 40            [12]  523 	mov	@r0,#0x40
      0000DE E5 08            [12]  524 	mov	a,_bp
      0000E0 24 04            [12]  525 	add	a,#0x04
      0000E2 FB               [12]  526 	mov	r3,a
      0000E3 FA               [12]  527 	mov	r2,a
      0000E4 7D 00            [12]  528 	mov	r5,#0x00
      0000E6 7E 40            [12]  529 	mov	r6,#0x40
      0000E8 C0 07            [24]  530 	push	ar7
      0000EA C0 03            [24]  531 	push	ar3
      0000EC A8 08            [24]  532 	mov	r0,_bp
      0000EE 08               [12]  533 	inc	r0
      0000EF E6               [12]  534 	mov	a,@r0
      0000F0 C0 E0            [24]  535 	push	acc
      0000F2 08               [12]  536 	inc	r0
      0000F3 E6               [12]  537 	mov	a,@r0
      0000F4 C0 E0            [24]  538 	push	acc
      0000F6 08               [12]  539 	inc	r0
      0000F7 E6               [12]  540 	mov	a,@r0
      0000F8 C0 E0            [24]  541 	push	acc
      0000FA 8A 82            [24]  542 	mov	dpl,r2
      0000FC 8D 83            [24]  543 	mov	dph,r5
      0000FE 8E F0            [24]  544 	mov	b,r6
      000100 12 0D EE         [24]  545 	lcall	_simpleserial_get
      000103 AD 82            [24]  546 	mov	r5,dpl
      000105 AE 83            [24]  547 	mov	r6,dph
      000107 15 81            [12]  548 	dec	sp
      000109 15 81            [12]  549 	dec	sp
      00010B 15 81            [12]  550 	dec	sp
      00010D D0 03            [24]  551 	pop	ar3
      00010F D0 07            [24]  552 	pop	ar7
      000111 ED               [12]  553 	mov	a,r5
      000112 4E               [12]  554 	orl	a,r6
      000113 60 B9            [24]  555 	jz	00104$
                                    556 ;	main.c:40: trigger_high();
      000115 43 90 01         [24]  557 	orl	_P1,#0x01
                                    558 ;	main.c:41: aes_encrypt(input, key);
      000118 A8 08            [24]  559 	mov	r0,_bp
      00011A 08               [12]  560 	inc	r0
      00011B A6 07            [24]  561 	mov	@r0,ar7
      00011D 08               [12]  562 	inc	r0
      00011E 76 00            [12]  563 	mov	@r0,#0x00
      000120 08               [12]  564 	inc	r0
      000121 76 40            [12]  565 	mov	@r0,#0x40
      000123 8B 04            [24]  566 	mov	ar4,r3
      000125 8C 02            [24]  567 	mov	ar2,r4
      000127 7C 00            [12]  568 	mov	r4,#0x00
      000129 7F 40            [12]  569 	mov	r7,#0x40
      00012B C0 03            [24]  570 	push	ar3
      00012D A8 08            [24]  571 	mov	r0,_bp
      00012F 08               [12]  572 	inc	r0
      000130 E6               [12]  573 	mov	a,@r0
      000131 C0 E0            [24]  574 	push	acc
      000133 08               [12]  575 	inc	r0
      000134 E6               [12]  576 	mov	a,@r0
      000135 C0 E0            [24]  577 	push	acc
      000137 08               [12]  578 	inc	r0
      000138 E6               [12]  579 	mov	a,@r0
      000139 C0 E0            [24]  580 	push	acc
      00013B 8A 82            [24]  581 	mov	dpl,r2
      00013D 8C 83            [24]  582 	mov	dph,r4
      00013F 8F F0            [24]  583 	mov	b,r7
      000141 12 09 DA         [24]  584 	lcall	_aes_encrypt
      000144 15 81            [12]  585 	dec	sp
      000146 15 81            [12]  586 	dec	sp
      000148 15 81            [12]  587 	dec	sp
      00014A D0 03            [24]  588 	pop	ar3
                                    589 ;	main.c:42: trigger_low();
      00014C 53 90 FE         [24]  590 	anl	_P1,#0xfe
                                    591 ;	main.c:43: simpleserial_put(input);
      00014F 7F 00            [12]  592 	mov	r7,#0x00
      000151 7E 40            [12]  593 	mov	r6,#0x40
      000153 8B 82            [24]  594 	mov	dpl,r3
      000155 8F 83            [24]  595 	mov	dph,r7
      000157 8E F0            [24]  596 	mov	b,r6
      000159 12 0E D7         [24]  597 	lcall	_simpleserial_put
      00015C 02 00 CE         [24]  598 	ljmp	00104$
      00015F 85 08 81         [24]  599 	mov	sp,_bp
      000162 D0 08            [24]  600 	pop	_bp
      000164 22               [24]  601 	ret
                                    602 	.area CSEG    (CODE)
                                    603 	.area CONST   (CODE)
                                    604 	.area XINIT   (CODE)
                                    605 	.area CABS    (ABS,CODE)
