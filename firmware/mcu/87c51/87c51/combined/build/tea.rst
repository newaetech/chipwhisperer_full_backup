                                      1 ;--------------------------------------------------------
                                      2 ; File Created by SDCC : free open source ANSI-C Compiler
                                      3 ; Version 3.6.0 #9615 (MINGW32)
                                      4 ;--------------------------------------------------------
                                      5 	.module tea
                                      6 	.optsdcc -mmcs51 --model-small
                                      7 	
                                      8 ;--------------------------------------------------------
                                      9 ; Public variables in this module
                                     10 ;--------------------------------------------------------
                                     11 	.globl _tea_encrypt
                                     12 	.globl _simpleserial_put
                                     13 	.globl _simpleserial_get
                                     14 	.globl _TF2
                                     15 	.globl _EXF2
                                     16 	.globl _RCLK
                                     17 	.globl _TCLK
                                     18 	.globl _EXEN2
                                     19 	.globl _TR2
                                     20 	.globl _C_T2
                                     21 	.globl _CP_RL2
                                     22 	.globl _T2CON_7
                                     23 	.globl _T2CON_6
                                     24 	.globl _T2CON_5
                                     25 	.globl _T2CON_4
                                     26 	.globl _T2CON_3
                                     27 	.globl _T2CON_2
                                     28 	.globl _T2CON_1
                                     29 	.globl _T2CON_0
                                     30 	.globl _PT2
                                     31 	.globl _ET2
                                     32 	.globl _CY
                                     33 	.globl _AC
                                     34 	.globl _F0
                                     35 	.globl _RS1
                                     36 	.globl _RS0
                                     37 	.globl _OV
                                     38 	.globl _F1
                                     39 	.globl _P
                                     40 	.globl _PS
                                     41 	.globl _PT1
                                     42 	.globl _PX1
                                     43 	.globl _PT0
                                     44 	.globl _PX0
                                     45 	.globl _RD
                                     46 	.globl _WR
                                     47 	.globl _T1
                                     48 	.globl _T0
                                     49 	.globl _INT1
                                     50 	.globl _INT0
                                     51 	.globl _TXD
                                     52 	.globl _RXD
                                     53 	.globl _P3_7
                                     54 	.globl _P3_6
                                     55 	.globl _P3_5
                                     56 	.globl _P3_4
                                     57 	.globl _P3_3
                                     58 	.globl _P3_2
                                     59 	.globl _P3_1
                                     60 	.globl _P3_0
                                     61 	.globl _EA
                                     62 	.globl _ES
                                     63 	.globl _ET1
                                     64 	.globl _EX1
                                     65 	.globl _ET0
                                     66 	.globl _EX0
                                     67 	.globl _P2_7
                                     68 	.globl _P2_6
                                     69 	.globl _P2_5
                                     70 	.globl _P2_4
                                     71 	.globl _P2_3
                                     72 	.globl _P2_2
                                     73 	.globl _P2_1
                                     74 	.globl _P2_0
                                     75 	.globl _SM0
                                     76 	.globl _SM1
                                     77 	.globl _SM2
                                     78 	.globl _REN
                                     79 	.globl _TB8
                                     80 	.globl _RB8
                                     81 	.globl _TI
                                     82 	.globl _RI
                                     83 	.globl _P1_7
                                     84 	.globl _P1_6
                                     85 	.globl _P1_5
                                     86 	.globl _P1_4
                                     87 	.globl _P1_3
                                     88 	.globl _P1_2
                                     89 	.globl _P1_1
                                     90 	.globl _P1_0
                                     91 	.globl _TF1
                                     92 	.globl _TR1
                                     93 	.globl _TF0
                                     94 	.globl _TR0
                                     95 	.globl _IE1
                                     96 	.globl _IT1
                                     97 	.globl _IE0
                                     98 	.globl _IT0
                                     99 	.globl _P0_7
                                    100 	.globl _P0_6
                                    101 	.globl _P0_5
                                    102 	.globl _P0_4
                                    103 	.globl _P0_3
                                    104 	.globl _P0_2
                                    105 	.globl _P0_1
                                    106 	.globl _P0_0
                                    107 	.globl _TH2
                                    108 	.globl _TL2
                                    109 	.globl _RCAP2H
                                    110 	.globl _RCAP2L
                                    111 	.globl _T2CON
                                    112 	.globl _B
                                    113 	.globl _ACC
                                    114 	.globl _PSW
                                    115 	.globl _IP
                                    116 	.globl _P3
                                    117 	.globl _IE
                                    118 	.globl _P2
                                    119 	.globl _SBUF
                                    120 	.globl _SCON
                                    121 	.globl _P1
                                    122 	.globl _TH1
                                    123 	.globl _TH0
                                    124 	.globl _TL1
                                    125 	.globl _TL0
                                    126 	.globl _TMOD
                                    127 	.globl _TCON
                                    128 	.globl _PCON
                                    129 	.globl _DPH
                                    130 	.globl _DPL
                                    131 	.globl _SP
                                    132 	.globl _P0
                                    133 	.globl _main_tea
                                    134 ;--------------------------------------------------------
                                    135 ; special function registers
                                    136 ;--------------------------------------------------------
                                    137 	.area RSEG    (ABS,DATA)
      000000                        138 	.org 0x0000
                           000080   139 _P0	=	0x0080
                           000081   140 _SP	=	0x0081
                           000082   141 _DPL	=	0x0082
                           000083   142 _DPH	=	0x0083
                           000087   143 _PCON	=	0x0087
                           000088   144 _TCON	=	0x0088
                           000089   145 _TMOD	=	0x0089
                           00008A   146 _TL0	=	0x008a
                           00008B   147 _TL1	=	0x008b
                           00008C   148 _TH0	=	0x008c
                           00008D   149 _TH1	=	0x008d
                           000090   150 _P1	=	0x0090
                           000098   151 _SCON	=	0x0098
                           000099   152 _SBUF	=	0x0099
                           0000A0   153 _P2	=	0x00a0
                           0000A8   154 _IE	=	0x00a8
                           0000B0   155 _P3	=	0x00b0
                           0000B8   156 _IP	=	0x00b8
                           0000D0   157 _PSW	=	0x00d0
                           0000E0   158 _ACC	=	0x00e0
                           0000F0   159 _B	=	0x00f0
                           0000C8   160 _T2CON	=	0x00c8
                           0000CA   161 _RCAP2L	=	0x00ca
                           0000CB   162 _RCAP2H	=	0x00cb
                           0000CC   163 _TL2	=	0x00cc
                           0000CD   164 _TH2	=	0x00cd
                                    165 ;--------------------------------------------------------
                                    166 ; special function bits
                                    167 ;--------------------------------------------------------
                                    168 	.area RSEG    (ABS,DATA)
      000000                        169 	.org 0x0000
                           000080   170 _P0_0	=	0x0080
                           000081   171 _P0_1	=	0x0081
                           000082   172 _P0_2	=	0x0082
                           000083   173 _P0_3	=	0x0083
                           000084   174 _P0_4	=	0x0084
                           000085   175 _P0_5	=	0x0085
                           000086   176 _P0_6	=	0x0086
                           000087   177 _P0_7	=	0x0087
                           000088   178 _IT0	=	0x0088
                           000089   179 _IE0	=	0x0089
                           00008A   180 _IT1	=	0x008a
                           00008B   181 _IE1	=	0x008b
                           00008C   182 _TR0	=	0x008c
                           00008D   183 _TF0	=	0x008d
                           00008E   184 _TR1	=	0x008e
                           00008F   185 _TF1	=	0x008f
                           000090   186 _P1_0	=	0x0090
                           000091   187 _P1_1	=	0x0091
                           000092   188 _P1_2	=	0x0092
                           000093   189 _P1_3	=	0x0093
                           000094   190 _P1_4	=	0x0094
                           000095   191 _P1_5	=	0x0095
                           000096   192 _P1_6	=	0x0096
                           000097   193 _P1_7	=	0x0097
                           000098   194 _RI	=	0x0098
                           000099   195 _TI	=	0x0099
                           00009A   196 _RB8	=	0x009a
                           00009B   197 _TB8	=	0x009b
                           00009C   198 _REN	=	0x009c
                           00009D   199 _SM2	=	0x009d
                           00009E   200 _SM1	=	0x009e
                           00009F   201 _SM0	=	0x009f
                           0000A0   202 _P2_0	=	0x00a0
                           0000A1   203 _P2_1	=	0x00a1
                           0000A2   204 _P2_2	=	0x00a2
                           0000A3   205 _P2_3	=	0x00a3
                           0000A4   206 _P2_4	=	0x00a4
                           0000A5   207 _P2_5	=	0x00a5
                           0000A6   208 _P2_6	=	0x00a6
                           0000A7   209 _P2_7	=	0x00a7
                           0000A8   210 _EX0	=	0x00a8
                           0000A9   211 _ET0	=	0x00a9
                           0000AA   212 _EX1	=	0x00aa
                           0000AB   213 _ET1	=	0x00ab
                           0000AC   214 _ES	=	0x00ac
                           0000AF   215 _EA	=	0x00af
                           0000B0   216 _P3_0	=	0x00b0
                           0000B1   217 _P3_1	=	0x00b1
                           0000B2   218 _P3_2	=	0x00b2
                           0000B3   219 _P3_3	=	0x00b3
                           0000B4   220 _P3_4	=	0x00b4
                           0000B5   221 _P3_5	=	0x00b5
                           0000B6   222 _P3_6	=	0x00b6
                           0000B7   223 _P3_7	=	0x00b7
                           0000B0   224 _RXD	=	0x00b0
                           0000B1   225 _TXD	=	0x00b1
                           0000B2   226 _INT0	=	0x00b2
                           0000B3   227 _INT1	=	0x00b3
                           0000B4   228 _T0	=	0x00b4
                           0000B5   229 _T1	=	0x00b5
                           0000B6   230 _WR	=	0x00b6
                           0000B7   231 _RD	=	0x00b7
                           0000B8   232 _PX0	=	0x00b8
                           0000B9   233 _PT0	=	0x00b9
                           0000BA   234 _PX1	=	0x00ba
                           0000BB   235 _PT1	=	0x00bb
                           0000BC   236 _PS	=	0x00bc
                           0000D0   237 _P	=	0x00d0
                           0000D1   238 _F1	=	0x00d1
                           0000D2   239 _OV	=	0x00d2
                           0000D3   240 _RS0	=	0x00d3
                           0000D4   241 _RS1	=	0x00d4
                           0000D5   242 _F0	=	0x00d5
                           0000D6   243 _AC	=	0x00d6
                           0000D7   244 _CY	=	0x00d7
                           0000AD   245 _ET2	=	0x00ad
                           0000BD   246 _PT2	=	0x00bd
                           0000C8   247 _T2CON_0	=	0x00c8
                           0000C9   248 _T2CON_1	=	0x00c9
                           0000CA   249 _T2CON_2	=	0x00ca
                           0000CB   250 _T2CON_3	=	0x00cb
                           0000CC   251 _T2CON_4	=	0x00cc
                           0000CD   252 _T2CON_5	=	0x00cd
                           0000CE   253 _T2CON_6	=	0x00ce
                           0000CF   254 _T2CON_7	=	0x00cf
                           0000C8   255 _CP_RL2	=	0x00c8
                           0000C9   256 _C_T2	=	0x00c9
                           0000CA   257 _TR2	=	0x00ca
                           0000CB   258 _EXEN2	=	0x00cb
                           0000CC   259 _TCLK	=	0x00cc
                           0000CD   260 _RCLK	=	0x00cd
                           0000CE   261 _EXF2	=	0x00ce
                           0000CF   262 _TF2	=	0x00cf
                                    263 ;--------------------------------------------------------
                                    264 ; overlayable register banks
                                    265 ;--------------------------------------------------------
                                    266 	.area REG_BANK_0	(REL,OVR,DATA)
      000000                        267 	.ds 8
                                    268 ;--------------------------------------------------------
                                    269 ; internal ram data
                                    270 ;--------------------------------------------------------
                                    271 	.area DSEG    (DATA)
                                    272 ;--------------------------------------------------------
                                    273 ; overlayable items in internal ram 
                                    274 ;--------------------------------------------------------
                                    275 ;--------------------------------------------------------
                                    276 ; indirectly addressable internal ram data
                                    277 ;--------------------------------------------------------
                                    278 	.area ISEG    (DATA)
                                    279 ;--------------------------------------------------------
                                    280 ; absolute internal ram data
                                    281 ;--------------------------------------------------------
                                    282 	.area IABS    (ABS,DATA)
                                    283 	.area IABS    (ABS,DATA)
                                    284 ;--------------------------------------------------------
                                    285 ; bit data
                                    286 ;--------------------------------------------------------
                                    287 	.area BSEG    (BIT)
                                    288 ;--------------------------------------------------------
                                    289 ; paged external ram data
                                    290 ;--------------------------------------------------------
                                    291 	.area PSEG    (PAG,XDATA)
                                    292 ;--------------------------------------------------------
                                    293 ; external ram data
                                    294 ;--------------------------------------------------------
                                    295 	.area XSEG    (XDATA)
                                    296 ;--------------------------------------------------------
                                    297 ; absolute external ram data
                                    298 ;--------------------------------------------------------
                                    299 	.area XABS    (ABS,XDATA)
                                    300 ;--------------------------------------------------------
                                    301 ; external initialized ram data
                                    302 ;--------------------------------------------------------
                                    303 	.area XISEG   (XDATA)
                                    304 	.area HOME    (CODE)
                                    305 	.area GSINIT0 (CODE)
                                    306 	.area GSINIT1 (CODE)
                                    307 	.area GSINIT2 (CODE)
                                    308 	.area GSINIT3 (CODE)
                                    309 	.area GSINIT4 (CODE)
                                    310 	.area GSINIT5 (CODE)
                                    311 	.area GSINIT  (CODE)
                                    312 	.area GSFINAL (CODE)
                                    313 	.area CSEG    (CODE)
                                    314 ;--------------------------------------------------------
                                    315 ; global & static initialisations
                                    316 ;--------------------------------------------------------
                                    317 	.area HOME    (CODE)
                                    318 	.area GSINIT  (CODE)
                                    319 	.area GSFINAL (CODE)
                                    320 	.area GSINIT  (CODE)
                                    321 ;--------------------------------------------------------
                                    322 ; Home
                                    323 ;--------------------------------------------------------
                                    324 	.area HOME    (CODE)
                                    325 	.area HOME    (CODE)
                                    326 ;--------------------------------------------------------
                                    327 ; code
                                    328 ;--------------------------------------------------------
                                    329 	.area CSEG    (CODE)
                                    330 ;------------------------------------------------------------
                                    331 ;Allocation info for local variables in function 'tea_encrypt'
                                    332 ;------------------------------------------------------------
                                    333 ;k                         Allocated to stack - _bp -5
                                    334 ;v                         Allocated to stack - _bp +1
                                    335 ;vp                        Allocated to stack - _bp +22
                                    336 ;kp                        Allocated to stack - _bp +30
                                    337 ;sum                       Allocated to stack - _bp +46
                                    338 ;i                         Allocated to stack - _bp +50
                                    339 ;j                         Allocated to stack - _bp +54
                                    340 ;delta                     Allocated to registers 
                                    341 ;a                         Allocated to stack - _bp +58
                                    342 ;b                         Allocated to stack - _bp +62
                                    343 ;c                         Allocated to stack - _bp +66
                                    344 ;shift                     Allocated to stack - _bp +58
                                    345 ;sloc0                     Allocated to stack - _bp +4
                                    346 ;sloc1                     Allocated to stack - _bp +8
                                    347 ;sloc2                     Allocated to stack - _bp +12
                                    348 ;sloc3                     Allocated to stack - _bp +13
                                    349 ;sloc4                     Allocated to stack - _bp +14
                                    350 ;sloc5                     Allocated to stack - _bp +18
                                    351 ;------------------------------------------------------------
                                    352 ;	tea.c:21: void tea_encrypt(uint8_t* v, uint8_t* k)
                                    353 ;	-----------------------------------------
                                    354 ;	 function tea_encrypt
                                    355 ;	-----------------------------------------
      00121F                        356 _tea_encrypt:
                           000007   357 	ar7 = 0x07
                           000006   358 	ar6 = 0x06
                           000005   359 	ar5 = 0x05
                           000004   360 	ar4 = 0x04
                           000003   361 	ar3 = 0x03
                           000002   362 	ar2 = 0x02
                           000001   363 	ar1 = 0x01
                           000000   364 	ar0 = 0x00
      00121F C0 08            [24]  365 	push	_bp
      001221 85 81 08         [24]  366 	mov	_bp,sp
      001224 C0 82            [24]  367 	push	dpl
      001226 C0 83            [24]  368 	push	dph
      001228 C0 F0            [24]  369 	push	b
      00122A E5 81            [12]  370 	mov	a,sp
      00122C 24 42            [12]  371 	add	a,#0x42
      00122E F5 81            [12]  372 	mov	sp,a
                                    373 ;	tea.c:30: for(i = 0; i < 2; i++){
      001230 E5 08            [12]  374 	mov	a,_bp
      001232 24 16            [12]  375 	add	a,#0x16
      001234 F9               [12]  376 	mov	r1,a
      001235 E5 08            [12]  377 	mov	a,_bp
      001237 24 32            [12]  378 	add	a,#0x32
      001239 F8               [12]  379 	mov	r0,a
      00123A E4               [12]  380 	clr	a
      00123B F6               [12]  381 	mov	@r0,a
      00123C 08               [12]  382 	inc	r0
      00123D F6               [12]  383 	mov	@r0,a
      00123E 08               [12]  384 	inc	r0
      00123F F6               [12]  385 	mov	@r0,a
      001240 08               [12]  386 	inc	r0
      001241 F6               [12]  387 	mov	@r0,a
      001242                        388 00109$:
                                    389 ;	tea.c:31: vp[i] = 0;
      001242 E5 08            [12]  390 	mov	a,_bp
      001244 24 32            [12]  391 	add	a,#0x32
      001246 F8               [12]  392 	mov	r0,a
      001247 E6               [12]  393 	mov	a,@r0
      001248 25 E0            [12]  394 	add	a,acc
      00124A FA               [12]  395 	mov	r2,a
      00124B 08               [12]  396 	inc	r0
      00124C E6               [12]  397 	mov	a,@r0
      00124D 33               [12]  398 	rlc	a
      00124E FB               [12]  399 	mov	r3,a
      00124F 08               [12]  400 	inc	r0
      001250 E6               [12]  401 	mov	a,@r0
      001251 33               [12]  402 	rlc	a
      001252 FC               [12]  403 	mov	r4,a
      001253 08               [12]  404 	inc	r0
      001254 E6               [12]  405 	mov	a,@r0
      001255 33               [12]  406 	rlc	a
      001256 FF               [12]  407 	mov	r7,a
      001257 EA               [12]  408 	mov	a,r2
      001258 2A               [12]  409 	add	a,r2
      001259 FA               [12]  410 	mov	r2,a
      00125A EB               [12]  411 	mov	a,r3
      00125B 33               [12]  412 	rlc	a
      00125C FB               [12]  413 	mov	r3,a
      00125D EC               [12]  414 	mov	a,r4
      00125E 33               [12]  415 	rlc	a
      00125F FC               [12]  416 	mov	r4,a
      001260 EF               [12]  417 	mov	a,r7
      001261 33               [12]  418 	rlc	a
      001262 FF               [12]  419 	mov	r7,a
      001263 EA               [12]  420 	mov	a,r2
      001264 29               [12]  421 	add	a,r1
      001265 F8               [12]  422 	mov	r0,a
      001266 76 00            [12]  423 	mov	@r0,#0x00
      001268 08               [12]  424 	inc	r0
      001269 76 00            [12]  425 	mov	@r0,#0x00
      00126B 08               [12]  426 	inc	r0
      00126C 76 00            [12]  427 	mov	@r0,#0x00
      00126E 08               [12]  428 	inc	r0
      00126F 76 00            [12]  429 	mov	@r0,#0x00
                                    430 ;	tea.c:32: for(j = 0; j < 4; j++)
      001271 E5 08            [12]  431 	mov	a,_bp
      001273 24 04            [12]  432 	add	a,#0x04
      001275 F8               [12]  433 	mov	r0,a
      001276 A6 02            [24]  434 	mov	@r0,ar2
      001278 08               [12]  435 	inc	r0
      001279 A6 03            [24]  436 	mov	@r0,ar3
      00127B 08               [12]  437 	inc	r0
      00127C A6 04            [24]  438 	mov	@r0,ar4
      00127E 08               [12]  439 	inc	r0
      00127F A6 07            [24]  440 	mov	@r0,ar7
      001281 E5 08            [12]  441 	mov	a,_bp
      001283 24 36            [12]  442 	add	a,#0x36
      001285 F8               [12]  443 	mov	r0,a
      001286 E4               [12]  444 	clr	a
      001287 F6               [12]  445 	mov	@r0,a
      001288 08               [12]  446 	inc	r0
      001289 F6               [12]  447 	mov	@r0,a
      00128A 08               [12]  448 	inc	r0
      00128B F6               [12]  449 	mov	@r0,a
      00128C 08               [12]  450 	inc	r0
      00128D F6               [12]  451 	mov	@r0,a
                                    452 ;	tea.c:61: for(i = 0; i < 8; i++)
                                    453 ;	tea.c:32: for(j = 0; j < 4; j++)
      00128E                        454 00107$:
                                    455 ;	tea.c:33: vp[i] = (vp[i] << 8) | v[4*i + j]; 
      00128E E5 08            [12]  456 	mov	a,_bp
      001290 24 04            [12]  457 	add	a,#0x04
      001292 F8               [12]  458 	mov	r0,a
      001293 E6               [12]  459 	mov	a,@r0
      001294 29               [12]  460 	add	a,r1
      001295 F8               [12]  461 	mov	r0,a
      001296 C0 01            [24]  462 	push	ar1
      001298 86 02            [24]  463 	mov	ar2,@r0
      00129A 08               [12]  464 	inc	r0
      00129B 86 03            [24]  465 	mov	ar3,@r0
      00129D 08               [12]  466 	inc	r0
      00129E 86 04            [24]  467 	mov	ar4,@r0
      0012A0 08               [12]  468 	inc	r0
      0012A1 86 07            [24]  469 	mov	ar7,@r0
      0012A3 18               [12]  470 	dec	r0
      0012A4 18               [12]  471 	dec	r0
      0012A5 18               [12]  472 	dec	r0
      0012A6 C0 00            [24]  473 	push	ar0
      0012A8 E5 08            [12]  474 	mov	a,_bp
      0012AA 24 08            [12]  475 	add	a,#0x08
      0012AC F8               [12]  476 	mov	r0,a
      0012AD 08               [12]  477 	inc	r0
      0012AE 08               [12]  478 	inc	r0
      0012AF 08               [12]  479 	inc	r0
      0012B0 A6 04            [24]  480 	mov	@r0,ar4
      0012B2 18               [12]  481 	dec	r0
      0012B3 A6 03            [24]  482 	mov	@r0,ar3
      0012B5 18               [12]  483 	dec	r0
      0012B6 A6 02            [24]  484 	mov	@r0,ar2
      0012B8 18               [12]  485 	dec	r0
      0012B9 76 00            [12]  486 	mov	@r0,#0x00
      0012BB E5 08            [12]  487 	mov	a,_bp
      0012BD 24 04            [12]  488 	add	a,#0x04
      0012BF F8               [12]  489 	mov	r0,a
      0012C0 C0 01            [24]  490 	push	ar1
      0012C2 E5 08            [12]  491 	mov	a,_bp
      0012C4 24 36            [12]  492 	add	a,#0x36
      0012C6 F9               [12]  493 	mov	r1,a
      0012C7 E7               [12]  494 	mov	a,@r1
      0012C8 26               [12]  495 	add	a,@r0
      0012C9 FC               [12]  496 	mov	r4,a
      0012CA 09               [12]  497 	inc	r1
      0012CB E7               [12]  498 	mov	a,@r1
      0012CC 08               [12]  499 	inc	r0
      0012CD 36               [12]  500 	addc	a,@r0
      0012CE FD               [12]  501 	mov	r5,a
      0012CF 09               [12]  502 	inc	r1
      0012D0 E7               [12]  503 	mov	a,@r1
      0012D1 08               [12]  504 	inc	r0
      0012D2 36               [12]  505 	addc	a,@r0
      0012D3 09               [12]  506 	inc	r1
      0012D4 E7               [12]  507 	mov	a,@r1
      0012D5 08               [12]  508 	inc	r0
      0012D6 36               [12]  509 	addc	a,@r0
      0012D7 D0 01            [24]  510 	pop	ar1
      0012D9 A8 08            [24]  511 	mov	r0,_bp
      0012DB 08               [12]  512 	inc	r0
      0012DC EC               [12]  513 	mov	a,r4
      0012DD 26               [12]  514 	add	a,@r0
      0012DE FC               [12]  515 	mov	r4,a
      0012DF ED               [12]  516 	mov	a,r5
      0012E0 08               [12]  517 	inc	r0
      0012E1 36               [12]  518 	addc	a,@r0
      0012E2 FD               [12]  519 	mov	r5,a
      0012E3 08               [12]  520 	inc	r0
      0012E4 86 06            [24]  521 	mov	ar6,@r0
      0012E6 8C 82            [24]  522 	mov	dpl,r4
      0012E8 8D 83            [24]  523 	mov	dph,r5
      0012EA 8E F0            [24]  524 	mov	b,r6
      0012EC 12 1C A1         [24]  525 	lcall	__gptrget
      0012EF FC               [12]  526 	mov	r4,a
      0012F0 E4               [12]  527 	clr	a
      0012F1 FF               [12]  528 	mov	r7,a
      0012F2 FE               [12]  529 	mov	r6,a
      0012F3 FD               [12]  530 	mov	r5,a
      0012F4 E5 08            [12]  531 	mov	a,_bp
      0012F6 24 08            [12]  532 	add	a,#0x08
      0012F8 F8               [12]  533 	mov	r0,a
      0012F9 E6               [12]  534 	mov	a,@r0
      0012FA 42 04            [12]  535 	orl	ar4,a
      0012FC 08               [12]  536 	inc	r0
      0012FD E6               [12]  537 	mov	a,@r0
      0012FE 42 07            [12]  538 	orl	ar7,a
      001300 08               [12]  539 	inc	r0
      001301 E6               [12]  540 	mov	a,@r0
      001302 42 06            [12]  541 	orl	ar6,a
      001304 08               [12]  542 	inc	r0
      001305 E6               [12]  543 	mov	a,@r0
      001306 42 05            [12]  544 	orl	ar5,a
      001308 D0 00            [24]  545 	pop	ar0
      00130A A6 04            [24]  546 	mov	@r0,ar4
      00130C 08               [12]  547 	inc	r0
      00130D A6 07            [24]  548 	mov	@r0,ar7
      00130F 08               [12]  549 	inc	r0
      001310 A6 06            [24]  550 	mov	@r0,ar6
      001312 08               [12]  551 	inc	r0
      001313 A6 05            [24]  552 	mov	@r0,ar5
      001315 18               [12]  553 	dec	r0
      001316 18               [12]  554 	dec	r0
      001317 18               [12]  555 	dec	r0
                                    556 ;	tea.c:32: for(j = 0; j < 4; j++)
      001318 E5 08            [12]  557 	mov	a,_bp
      00131A 24 36            [12]  558 	add	a,#0x36
      00131C F8               [12]  559 	mov	r0,a
      00131D 06               [12]  560 	inc	@r0
      00131E B6 00 0C         [24]  561 	cjne	@r0,#0x00,00161$
      001321 08               [12]  562 	inc	r0
      001322 06               [12]  563 	inc	@r0
      001323 B6 00 07         [24]  564 	cjne	@r0,#0x00,00161$
      001326 08               [12]  565 	inc	r0
      001327 06               [12]  566 	inc	@r0
      001328 B6 00 02         [24]  567 	cjne	@r0,#0x00,00161$
      00132B 08               [12]  568 	inc	r0
      00132C 06               [12]  569 	inc	@r0
      00132D                        570 00161$:
      00132D E5 08            [12]  571 	mov	a,_bp
      00132F 24 36            [12]  572 	add	a,#0x36
      001331 F8               [12]  573 	mov	r0,a
      001332 C3               [12]  574 	clr	c
      001333 E6               [12]  575 	mov	a,@r0
      001334 94 04            [12]  576 	subb	a,#0x04
      001336 08               [12]  577 	inc	r0
      001337 E6               [12]  578 	mov	a,@r0
      001338 94 00            [12]  579 	subb	a,#0x00
      00133A 08               [12]  580 	inc	r0
      00133B E6               [12]  581 	mov	a,@r0
      00133C 94 00            [12]  582 	subb	a,#0x00
      00133E 08               [12]  583 	inc	r0
      00133F E6               [12]  584 	mov	a,@r0
      001340 94 00            [12]  585 	subb	a,#0x00
      001342 D0 01            [24]  586 	pop	ar1
      001344 50 03            [24]  587 	jnc	00162$
      001346 02 12 8E         [24]  588 	ljmp	00107$
      001349                        589 00162$:
                                    590 ;	tea.c:30: for(i = 0; i < 2; i++){
      001349 E5 08            [12]  591 	mov	a,_bp
      00134B 24 32            [12]  592 	add	a,#0x32
      00134D F8               [12]  593 	mov	r0,a
      00134E 06               [12]  594 	inc	@r0
      00134F B6 00 0C         [24]  595 	cjne	@r0,#0x00,00163$
      001352 08               [12]  596 	inc	r0
      001353 06               [12]  597 	inc	@r0
      001354 B6 00 07         [24]  598 	cjne	@r0,#0x00,00163$
      001357 08               [12]  599 	inc	r0
      001358 06               [12]  600 	inc	@r0
      001359 B6 00 02         [24]  601 	cjne	@r0,#0x00,00163$
      00135C 08               [12]  602 	inc	r0
      00135D 06               [12]  603 	inc	@r0
      00135E                        604 00163$:
      00135E E5 08            [12]  605 	mov	a,_bp
      001360 24 32            [12]  606 	add	a,#0x32
      001362 F8               [12]  607 	mov	r0,a
      001363 C3               [12]  608 	clr	c
      001364 E6               [12]  609 	mov	a,@r0
      001365 94 02            [12]  610 	subb	a,#0x02
      001367 08               [12]  611 	inc	r0
      001368 E6               [12]  612 	mov	a,@r0
      001369 94 00            [12]  613 	subb	a,#0x00
      00136B 08               [12]  614 	inc	r0
      00136C E6               [12]  615 	mov	a,@r0
      00136D 94 00            [12]  616 	subb	a,#0x00
      00136F 08               [12]  617 	inc	r0
      001370 E6               [12]  618 	mov	a,@r0
      001371 94 00            [12]  619 	subb	a,#0x00
      001373 50 03            [24]  620 	jnc	00164$
      001375 02 12 42         [24]  621 	ljmp	00109$
      001378                        622 00164$:
                                    623 ;	tea.c:36: for(i = 0; i < 4; i++){
      001378 E5 08            [12]  624 	mov	a,_bp
      00137A 24 1E            [12]  625 	add	a,#0x1e
      00137C F8               [12]  626 	mov	r0,a
      00137D C0 00            [24]  627 	push	ar0
      00137F E5 08            [12]  628 	mov	a,_bp
      001381 24 32            [12]  629 	add	a,#0x32
      001383 F8               [12]  630 	mov	r0,a
      001384 E4               [12]  631 	clr	a
      001385 F6               [12]  632 	mov	@r0,a
      001386 08               [12]  633 	inc	r0
      001387 F6               [12]  634 	mov	@r0,a
      001388 08               [12]  635 	inc	r0
      001389 F6               [12]  636 	mov	@r0,a
      00138A 08               [12]  637 	inc	r0
      00138B F6               [12]  638 	mov	@r0,a
      00138C D0 00            [24]  639 	pop	ar0
      00138E                        640 00113$:
                                    641 ;	tea.c:37: kp[i] = 0;
      00138E C0 01            [24]  642 	push	ar1
      001390 C0 00            [24]  643 	push	ar0
      001392 E5 08            [12]  644 	mov	a,_bp
      001394 24 32            [12]  645 	add	a,#0x32
      001396 F8               [12]  646 	mov	r0,a
      001397 E6               [12]  647 	mov	a,@r0
      001398 25 E0            [12]  648 	add	a,acc
      00139A FA               [12]  649 	mov	r2,a
      00139B 08               [12]  650 	inc	r0
      00139C E6               [12]  651 	mov	a,@r0
      00139D 33               [12]  652 	rlc	a
      00139E FB               [12]  653 	mov	r3,a
      00139F 08               [12]  654 	inc	r0
      0013A0 E6               [12]  655 	mov	a,@r0
      0013A1 33               [12]  656 	rlc	a
      0013A2 FE               [12]  657 	mov	r6,a
      0013A3 08               [12]  658 	inc	r0
      0013A4 E6               [12]  659 	mov	a,@r0
      0013A5 33               [12]  660 	rlc	a
      0013A6 FF               [12]  661 	mov	r7,a
      0013A7 EA               [12]  662 	mov	a,r2
      0013A8 2A               [12]  663 	add	a,r2
      0013A9 FA               [12]  664 	mov	r2,a
      0013AA EB               [12]  665 	mov	a,r3
      0013AB 33               [12]  666 	rlc	a
      0013AC FB               [12]  667 	mov	r3,a
      0013AD EE               [12]  668 	mov	a,r6
      0013AE 33               [12]  669 	rlc	a
      0013AF FE               [12]  670 	mov	r6,a
      0013B0 EF               [12]  671 	mov	a,r7
      0013B1 33               [12]  672 	rlc	a
      0013B2 FF               [12]  673 	mov	r7,a
      0013B3 D0 00            [24]  674 	pop	ar0
      0013B5 EA               [12]  675 	mov	a,r2
      0013B6 28               [12]  676 	add	a,r0
      0013B7 C0 00            [24]  677 	push	ar0
      0013B9 F8               [12]  678 	mov	r0,a
      0013BA 76 00            [12]  679 	mov	@r0,#0x00
      0013BC 08               [12]  680 	inc	r0
      0013BD 76 00            [12]  681 	mov	@r0,#0x00
      0013BF 08               [12]  682 	inc	r0
      0013C0 76 00            [12]  683 	mov	@r0,#0x00
      0013C2 08               [12]  684 	inc	r0
      0013C3 76 00            [12]  685 	mov	@r0,#0x00
                                    686 ;	tea.c:38: for(j = 0; j < 4; j++)
      0013C5 E5 08            [12]  687 	mov	a,_bp
      0013C7 24 08            [12]  688 	add	a,#0x08
      0013C9 F8               [12]  689 	mov	r0,a
      0013CA A6 02            [24]  690 	mov	@r0,ar2
      0013CC 08               [12]  691 	inc	r0
      0013CD A6 03            [24]  692 	mov	@r0,ar3
      0013CF 08               [12]  693 	inc	r0
      0013D0 A6 06            [24]  694 	mov	@r0,ar6
      0013D2 08               [12]  695 	inc	r0
      0013D3 A6 07            [24]  696 	mov	@r0,ar7
      0013D5 E5 08            [12]  697 	mov	a,_bp
      0013D7 24 36            [12]  698 	add	a,#0x36
      0013D9 F8               [12]  699 	mov	r0,a
      0013DA E4               [12]  700 	clr	a
      0013DB F6               [12]  701 	mov	@r0,a
      0013DC 08               [12]  702 	inc	r0
      0013DD F6               [12]  703 	mov	@r0,a
      0013DE 08               [12]  704 	inc	r0
      0013DF F6               [12]  705 	mov	@r0,a
      0013E0 08               [12]  706 	inc	r0
      0013E1 F6               [12]  707 	mov	@r0,a
      0013E2 D0 00            [24]  708 	pop	ar0
                                    709 ;	tea.c:61: for(i = 0; i < 8; i++)
      0013E4 D0 01            [24]  710 	pop	ar1
                                    711 ;	tea.c:38: for(j = 0; j < 4; j++)
      0013E6                        712 00111$:
                                    713 ;	tea.c:39: kp[i] = (kp[i] << 8) | k[4*i + j]; 
      0013E6 C0 01            [24]  714 	push	ar1
      0013E8 C0 01            [24]  715 	push	ar1
      0013EA E5 08            [12]  716 	mov	a,_bp
      0013EC 24 08            [12]  717 	add	a,#0x08
      0013EE F9               [12]  718 	mov	r1,a
      0013EF E7               [12]  719 	mov	a,@r1
      0013F0 28               [12]  720 	add	a,r0
      0013F1 FF               [12]  721 	mov	r7,a
      0013F2 D0 01            [24]  722 	pop	ar1
      0013F4 C0 00            [24]  723 	push	ar0
      0013F6 A8 07            [24]  724 	mov	r0,ar7
      0013F8 86 03            [24]  725 	mov	ar3,@r0
      0013FA 08               [12]  726 	inc	r0
      0013FB 86 04            [24]  727 	mov	ar4,@r0
      0013FD 08               [12]  728 	inc	r0
      0013FE 86 05            [24]  729 	mov	ar5,@r0
      001400 08               [12]  730 	inc	r0
      001401 86 06            [24]  731 	mov	ar6,@r0
      001403 E5 08            [12]  732 	mov	a,_bp
      001405 24 04            [12]  733 	add	a,#0x04
      001407 F8               [12]  734 	mov	r0,a
      001408 08               [12]  735 	inc	r0
      001409 08               [12]  736 	inc	r0
      00140A 08               [12]  737 	inc	r0
      00140B A6 05            [24]  738 	mov	@r0,ar5
      00140D 18               [12]  739 	dec	r0
      00140E A6 04            [24]  740 	mov	@r0,ar4
      001410 18               [12]  741 	dec	r0
      001411 A6 03            [24]  742 	mov	@r0,ar3
      001413 18               [12]  743 	dec	r0
      001414 76 00            [12]  744 	mov	@r0,#0x00
      001416 E5 08            [12]  745 	mov	a,_bp
      001418 24 08            [12]  746 	add	a,#0x08
      00141A F8               [12]  747 	mov	r0,a
      00141B C0 01            [24]  748 	push	ar1
      00141D E5 08            [12]  749 	mov	a,_bp
      00141F 24 36            [12]  750 	add	a,#0x36
      001421 F9               [12]  751 	mov	r1,a
      001422 E7               [12]  752 	mov	a,@r1
      001423 26               [12]  753 	add	a,@r0
      001424 FA               [12]  754 	mov	r2,a
      001425 09               [12]  755 	inc	r1
      001426 E7               [12]  756 	mov	a,@r1
      001427 08               [12]  757 	inc	r0
      001428 36               [12]  758 	addc	a,@r0
      001429 FC               [12]  759 	mov	r4,a
      00142A 09               [12]  760 	inc	r1
      00142B E7               [12]  761 	mov	a,@r1
      00142C 08               [12]  762 	inc	r0
      00142D 36               [12]  763 	addc	a,@r0
      00142E 09               [12]  764 	inc	r1
      00142F E7               [12]  765 	mov	a,@r1
      001430 08               [12]  766 	inc	r0
      001431 36               [12]  767 	addc	a,@r0
      001432 D0 01            [24]  768 	pop	ar1
      001434 E5 08            [12]  769 	mov	a,_bp
      001436 24 FB            [12]  770 	add	a,#0xfb
      001438 F8               [12]  771 	mov	r0,a
      001439 EA               [12]  772 	mov	a,r2
      00143A 26               [12]  773 	add	a,@r0
      00143B FA               [12]  774 	mov	r2,a
      00143C EC               [12]  775 	mov	a,r4
      00143D 08               [12]  776 	inc	r0
      00143E 36               [12]  777 	addc	a,@r0
      00143F FC               [12]  778 	mov	r4,a
      001440 08               [12]  779 	inc	r0
      001441 86 05            [24]  780 	mov	ar5,@r0
      001443 8A 82            [24]  781 	mov	dpl,r2
      001445 8C 83            [24]  782 	mov	dph,r4
      001447 8D F0            [24]  783 	mov	b,r5
      001449 12 1C A1         [24]  784 	lcall	__gptrget
      00144C FA               [12]  785 	mov	r2,a
      00144D E4               [12]  786 	clr	a
      00144E FE               [12]  787 	mov	r6,a
      00144F FD               [12]  788 	mov	r5,a
      001450 FC               [12]  789 	mov	r4,a
      001451 E5 08            [12]  790 	mov	a,_bp
      001453 24 04            [12]  791 	add	a,#0x04
      001455 F8               [12]  792 	mov	r0,a
      001456 E6               [12]  793 	mov	a,@r0
      001457 42 02            [12]  794 	orl	ar2,a
      001459 08               [12]  795 	inc	r0
      00145A E6               [12]  796 	mov	a,@r0
      00145B 42 06            [12]  797 	orl	ar6,a
      00145D 08               [12]  798 	inc	r0
      00145E E6               [12]  799 	mov	a,@r0
      00145F 42 05            [12]  800 	orl	ar5,a
      001461 08               [12]  801 	inc	r0
      001462 E6               [12]  802 	mov	a,@r0
      001463 42 04            [12]  803 	orl	ar4,a
      001465 A8 07            [24]  804 	mov	r0,ar7
      001467 A6 02            [24]  805 	mov	@r0,ar2
      001469 08               [12]  806 	inc	r0
      00146A A6 06            [24]  807 	mov	@r0,ar6
      00146C 08               [12]  808 	inc	r0
      00146D A6 05            [24]  809 	mov	@r0,ar5
      00146F 08               [12]  810 	inc	r0
      001470 A6 04            [24]  811 	mov	@r0,ar4
                                    812 ;	tea.c:38: for(j = 0; j < 4; j++)
      001472 E5 08            [12]  813 	mov	a,_bp
      001474 24 36            [12]  814 	add	a,#0x36
      001476 F8               [12]  815 	mov	r0,a
      001477 06               [12]  816 	inc	@r0
      001478 B6 00 0C         [24]  817 	cjne	@r0,#0x00,00165$
      00147B 08               [12]  818 	inc	r0
      00147C 06               [12]  819 	inc	@r0
      00147D B6 00 07         [24]  820 	cjne	@r0,#0x00,00165$
      001480 08               [12]  821 	inc	r0
      001481 06               [12]  822 	inc	@r0
      001482 B6 00 02         [24]  823 	cjne	@r0,#0x00,00165$
      001485 08               [12]  824 	inc	r0
      001486 06               [12]  825 	inc	@r0
      001487                        826 00165$:
      001487 E5 08            [12]  827 	mov	a,_bp
      001489 24 36            [12]  828 	add	a,#0x36
      00148B F8               [12]  829 	mov	r0,a
      00148C C3               [12]  830 	clr	c
      00148D E6               [12]  831 	mov	a,@r0
      00148E 94 04            [12]  832 	subb	a,#0x04
      001490 08               [12]  833 	inc	r0
      001491 E6               [12]  834 	mov	a,@r0
      001492 94 00            [12]  835 	subb	a,#0x00
      001494 08               [12]  836 	inc	r0
      001495 E6               [12]  837 	mov	a,@r0
      001496 94 00            [12]  838 	subb	a,#0x00
      001498 08               [12]  839 	inc	r0
      001499 E6               [12]  840 	mov	a,@r0
      00149A 94 00            [12]  841 	subb	a,#0x00
      00149C D0 00            [24]  842 	pop	ar0
      00149E D0 01            [24]  843 	pop	ar1
      0014A0 50 03            [24]  844 	jnc	00166$
      0014A2 02 13 E6         [24]  845 	ljmp	00111$
      0014A5                        846 00166$:
                                    847 ;	tea.c:36: for(i = 0; i < 4; i++){
      0014A5 C0 00            [24]  848 	push	ar0
      0014A7 E5 08            [12]  849 	mov	a,_bp
      0014A9 24 32            [12]  850 	add	a,#0x32
      0014AB F8               [12]  851 	mov	r0,a
      0014AC 06               [12]  852 	inc	@r0
      0014AD B6 00 0C         [24]  853 	cjne	@r0,#0x00,00167$
      0014B0 08               [12]  854 	inc	r0
      0014B1 06               [12]  855 	inc	@r0
      0014B2 B6 00 07         [24]  856 	cjne	@r0,#0x00,00167$
      0014B5 08               [12]  857 	inc	r0
      0014B6 06               [12]  858 	inc	@r0
      0014B7 B6 00 02         [24]  859 	cjne	@r0,#0x00,00167$
      0014BA 08               [12]  860 	inc	r0
      0014BB 06               [12]  861 	inc	@r0
      0014BC                        862 00167$:
      0014BC E5 08            [12]  863 	mov	a,_bp
      0014BE 24 32            [12]  864 	add	a,#0x32
      0014C0 F8               [12]  865 	mov	r0,a
      0014C1 C3               [12]  866 	clr	c
      0014C2 E6               [12]  867 	mov	a,@r0
      0014C3 94 04            [12]  868 	subb	a,#0x04
      0014C5 08               [12]  869 	inc	r0
      0014C6 E6               [12]  870 	mov	a,@r0
      0014C7 94 00            [12]  871 	subb	a,#0x00
      0014C9 08               [12]  872 	inc	r0
      0014CA E6               [12]  873 	mov	a,@r0
      0014CB 94 00            [12]  874 	subb	a,#0x00
      0014CD 08               [12]  875 	inc	r0
      0014CE E6               [12]  876 	mov	a,@r0
      0014CF 94 00            [12]  877 	subb	a,#0x00
      0014D1 D0 00            [24]  878 	pop	ar0
      0014D3 50 03            [24]  879 	jnc	00168$
      0014D5 02 13 8E         [24]  880 	ljmp	00113$
      0014D8                        881 00168$:
                                    882 ;	tea.c:42: for (i=0; i < 32; i++) {                       /* basic cycle start */
      0014D8 C0 01            [24]  883 	push	ar1
      0014DA E5 08            [12]  884 	mov	a,_bp
      0014DC 24 0C            [12]  885 	add	a,#0x0c
      0014DE F9               [12]  886 	mov	r1,a
      0014DF 74 04            [12]  887 	mov	a,#0x04
      0014E1 28               [12]  888 	add	a,r0
      0014E2 F7               [12]  889 	mov	@r1,a
      0014E3 E5 08            [12]  890 	mov	a,_bp
      0014E5 24 04            [12]  891 	add	a,#0x04
      0014E7 F9               [12]  892 	mov	r1,a
      0014E8 74 08            [12]  893 	mov	a,#0x08
      0014EA 28               [12]  894 	add	a,r0
      0014EB F7               [12]  895 	mov	@r1,a
      0014EC E5 08            [12]  896 	mov	a,_bp
      0014EE 24 08            [12]  897 	add	a,#0x08
      0014F0 F9               [12]  898 	mov	r1,a
      0014F1 74 0C            [12]  899 	mov	a,#0x0c
      0014F3 28               [12]  900 	add	a,r0
      0014F4 F7               [12]  901 	mov	@r1,a
      0014F5 D0 01            [24]  902 	pop	ar1
      0014F7 C0 00            [24]  903 	push	ar0
      0014F9 E5 08            [12]  904 	mov	a,_bp
      0014FB 24 2E            [12]  905 	add	a,#0x2e
      0014FD F8               [12]  906 	mov	r0,a
      0014FE E4               [12]  907 	clr	a
      0014FF F6               [12]  908 	mov	@r0,a
      001500 08               [12]  909 	inc	r0
      001501 F6               [12]  910 	mov	@r0,a
      001502 08               [12]  911 	inc	r0
      001503 F6               [12]  912 	mov	@r0,a
      001504 08               [12]  913 	inc	r0
      001505 F6               [12]  914 	mov	@r0,a
      001506 E5 08            [12]  915 	mov	a,_bp
      001508 24 32            [12]  916 	add	a,#0x32
      00150A F8               [12]  917 	mov	r0,a
      00150B 76 20            [12]  918 	mov	@r0,#0x20
      00150D E4               [12]  919 	clr	a
      00150E 08               [12]  920 	inc	r0
      00150F F6               [12]  921 	mov	@r0,a
      001510 08               [12]  922 	inc	r0
      001511 F6               [12]  923 	mov	@r0,a
      001512 08               [12]  924 	inc	r0
      001513 F6               [12]  925 	mov	@r0,a
      001514 D0 00            [24]  926 	pop	ar0
      001516                        927 00117$:
                                    928 ;	tea.c:45: sum += delta;
      001516 C0 00            [24]  929 	push	ar0
      001518 E5 08            [12]  930 	mov	a,_bp
      00151A 24 2E            [12]  931 	add	a,#0x2e
      00151C F8               [12]  932 	mov	r0,a
      00151D 74 B9            [12]  933 	mov	a,#0xb9
      00151F 26               [12]  934 	add	a,@r0
      001520 F6               [12]  935 	mov	@r0,a
      001521 74 79            [12]  936 	mov	a,#0x79
      001523 08               [12]  937 	inc	r0
      001524 36               [12]  938 	addc	a,@r0
      001525 F6               [12]  939 	mov	@r0,a
      001526 74 37            [12]  940 	mov	a,#0x37
      001528 08               [12]  941 	inc	r0
      001529 36               [12]  942 	addc	a,@r0
      00152A F6               [12]  943 	mov	@r0,a
      00152B 74 9E            [12]  944 	mov	a,#0x9e
      00152D 08               [12]  945 	inc	r0
      00152E 36               [12]  946 	addc	a,@r0
      00152F F6               [12]  947 	mov	@r0,a
                                    948 ;	tea.c:48: a = (vp[1] << 4) + kp[0];
      001530 E5 08            [12]  949 	mov	a,_bp
      001532 24 0D            [12]  950 	add	a,#0x0d
      001534 F8               [12]  951 	mov	r0,a
      001535 74 04            [12]  952 	mov	a,#0x04
      001537 29               [12]  953 	add	a,r1
      001538 F6               [12]  954 	mov	@r0,a
      001539 E5 08            [12]  955 	mov	a,_bp
      00153B 24 0D            [12]  956 	add	a,#0x0d
      00153D F8               [12]  957 	mov	r0,a
      00153E C0 01            [24]  958 	push	ar1
      001540 E5 08            [12]  959 	mov	a,_bp
      001542 24 0E            [12]  960 	add	a,#0x0e
      001544 F9               [12]  961 	mov	r1,a
      001545 86 00            [24]  962 	mov	ar0,@r0
      001547 E6               [12]  963 	mov	a,@r0
      001548 F7               [12]  964 	mov	@r1,a
      001549 08               [12]  965 	inc	r0
      00154A E6               [12]  966 	mov	a,@r0
      00154B 09               [12]  967 	inc	r1
      00154C F7               [12]  968 	mov	@r1,a
      00154D 08               [12]  969 	inc	r0
      00154E E6               [12]  970 	mov	a,@r0
      00154F 09               [12]  971 	inc	r1
      001550 F7               [12]  972 	mov	@r1,a
      001551 08               [12]  973 	inc	r0
      001552 E6               [12]  974 	mov	a,@r0
      001553 09               [12]  975 	inc	r1
      001554 F7               [12]  976 	mov	@r1,a
      001555 18               [12]  977 	dec	r0
      001556 18               [12]  978 	dec	r0
      001557 18               [12]  979 	dec	r0
      001558 E5 08            [12]  980 	mov	a,_bp
      00155A 24 0E            [12]  981 	add	a,#0x0e
      00155C F8               [12]  982 	mov	r0,a
      00155D E5 08            [12]  983 	mov	a,_bp
      00155F 24 12            [12]  984 	add	a,#0x12
      001561 F9               [12]  985 	mov	r1,a
      001562 08               [12]  986 	inc	r0
      001563 08               [12]  987 	inc	r0
      001564 E6               [12]  988 	mov	a,@r0
      001565 09               [12]  989 	inc	r1
      001566 09               [12]  990 	inc	r1
      001567 F7               [12]  991 	mov	@r1,a
      001568 08               [12]  992 	inc	r0
      001569 E6               [12]  993 	mov	a,@r0
      00156A C4               [12]  994 	swap	a
      00156B 54 F0            [12]  995 	anl	a,#0xf0
      00156D C7               [12]  996 	xch	a,@r1
      00156E C4               [12]  997 	swap	a
      00156F C7               [12]  998 	xch	a,@r1
      001570 67               [12]  999 	xrl	a,@r1
      001571 C7               [12] 1000 	xch	a,@r1
      001572 54 F0            [12] 1001 	anl	a,#0xf0
      001574 C7               [12] 1002 	xch	a,@r1
      001575 67               [12] 1003 	xrl	a,@r1
      001576 09               [12] 1004 	inc	r1
      001577 F7               [12] 1005 	mov	@r1,a
      001578 18               [12] 1006 	dec	r0
      001579 18               [12] 1007 	dec	r0
      00157A E6               [12] 1008 	mov	a,@r0
      00157B C4               [12] 1009 	swap	a
      00157C 54 0F            [12] 1010 	anl	a,#0x0f
      00157E 19               [12] 1011 	dec	r1
      00157F 47               [12] 1012 	orl	a,@r1
      001580 F7               [12] 1013 	mov	@r1,a
      001581 18               [12] 1014 	dec	r0
      001582 E6               [12] 1015 	mov	a,@r0
      001583 19               [12] 1016 	dec	r1
      001584 19               [12] 1017 	dec	r1
      001585 F7               [12] 1018 	mov	@r1,a
      001586 08               [12] 1019 	inc	r0
      001587 E6               [12] 1020 	mov	a,@r0
      001588 C4               [12] 1021 	swap	a
      001589 54 F0            [12] 1022 	anl	a,#0xf0
      00158B C7               [12] 1023 	xch	a,@r1
      00158C C4               [12] 1024 	swap	a
      00158D C7               [12] 1025 	xch	a,@r1
      00158E 67               [12] 1026 	xrl	a,@r1
      00158F C7               [12] 1027 	xch	a,@r1
      001590 54 F0            [12] 1028 	anl	a,#0xf0
      001592 C7               [12] 1029 	xch	a,@r1
      001593 67               [12] 1030 	xrl	a,@r1
      001594 09               [12] 1031 	inc	r1
      001595 F7               [12] 1032 	mov	@r1,a
      001596 D0 01            [24] 1033 	pop	ar1
      001598 D0 00            [24] 1034 	pop	ar0
      00159A 86 02            [24] 1035 	mov	ar2,@r0
      00159C 08               [12] 1036 	inc	r0
      00159D 86 03            [24] 1037 	mov	ar3,@r0
      00159F 08               [12] 1038 	inc	r0
      0015A0 86 06            [24] 1039 	mov	ar6,@r0
      0015A2 08               [12] 1040 	inc	r0
      0015A3 86 07            [24] 1041 	mov	ar7,@r0
      0015A5 18               [12] 1042 	dec	r0
      0015A6 18               [12] 1043 	dec	r0
      0015A7 18               [12] 1044 	dec	r0
      0015A8 C0 00            [24] 1045 	push	ar0
      0015AA E5 08            [12] 1046 	mov	a,_bp
      0015AC 24 12            [12] 1047 	add	a,#0x12
      0015AE F8               [12] 1048 	mov	r0,a
      0015AF EA               [12] 1049 	mov	a,r2
      0015B0 26               [12] 1050 	add	a,@r0
      0015B1 FA               [12] 1051 	mov	r2,a
      0015B2 EB               [12] 1052 	mov	a,r3
      0015B3 08               [12] 1053 	inc	r0
      0015B4 36               [12] 1054 	addc	a,@r0
      0015B5 FB               [12] 1055 	mov	r3,a
      0015B6 EE               [12] 1056 	mov	a,r6
      0015B7 08               [12] 1057 	inc	r0
      0015B8 36               [12] 1058 	addc	a,@r0
      0015B9 FE               [12] 1059 	mov	r6,a
      0015BA EF               [12] 1060 	mov	a,r7
      0015BB 08               [12] 1061 	inc	r0
      0015BC 36               [12] 1062 	addc	a,@r0
      0015BD FF               [12] 1063 	mov	r7,a
      0015BE E5 08            [12] 1064 	mov	a,_bp
      0015C0 24 3A            [12] 1065 	add	a,#0x3a
      0015C2 F8               [12] 1066 	mov	r0,a
      0015C3 A6 02            [24] 1067 	mov	@r0,ar2
      0015C5 08               [12] 1068 	inc	r0
      0015C6 A6 03            [24] 1069 	mov	@r0,ar3
      0015C8 08               [12] 1070 	inc	r0
      0015C9 A6 06            [24] 1071 	mov	@r0,ar6
      0015CB 08               [12] 1072 	inc	r0
      0015CC A6 07            [24] 1073 	mov	@r0,ar7
                                   1074 ;	tea.c:49: b = (vp[1] + sum);
      0015CE E5 08            [12] 1075 	mov	a,_bp
      0015D0 24 0E            [12] 1076 	add	a,#0x0e
      0015D2 F8               [12] 1077 	mov	r0,a
      0015D3 C0 01            [24] 1078 	push	ar1
      0015D5 E5 08            [12] 1079 	mov	a,_bp
      0015D7 24 2E            [12] 1080 	add	a,#0x2e
      0015D9 F9               [12] 1081 	mov	r1,a
      0015DA E7               [12] 1082 	mov	a,@r1
      0015DB 26               [12] 1083 	add	a,@r0
      0015DC FC               [12] 1084 	mov	r4,a
      0015DD 09               [12] 1085 	inc	r1
      0015DE E7               [12] 1086 	mov	a,@r1
      0015DF 08               [12] 1087 	inc	r0
      0015E0 36               [12] 1088 	addc	a,@r0
      0015E1 FD               [12] 1089 	mov	r5,a
      0015E2 09               [12] 1090 	inc	r1
      0015E3 E7               [12] 1091 	mov	a,@r1
      0015E4 08               [12] 1092 	inc	r0
      0015E5 36               [12] 1093 	addc	a,@r0
      0015E6 FE               [12] 1094 	mov	r6,a
      0015E7 09               [12] 1095 	inc	r1
      0015E8 E7               [12] 1096 	mov	a,@r1
      0015E9 08               [12] 1097 	inc	r0
      0015EA 36               [12] 1098 	addc	a,@r0
      0015EB FF               [12] 1099 	mov	r7,a
      0015EC D0 01            [24] 1100 	pop	ar1
      0015EE D0 00            [24] 1101 	pop	ar0
      0015F0 C0 00            [24] 1102 	push	ar0
      0015F2 C0 00            [24] 1103 	push	ar0
      0015F4 E5 08            [12] 1104 	mov	a,_bp
      0015F6 24 3E            [12] 1105 	add	a,#0x3e
      0015F8 F8               [12] 1106 	mov	r0,a
      0015F9 A6 04            [24] 1107 	mov	@r0,ar4
      0015FB 08               [12] 1108 	inc	r0
      0015FC A6 05            [24] 1109 	mov	@r0,ar5
      0015FE 08               [12] 1110 	inc	r0
      0015FF A6 06            [24] 1111 	mov	@r0,ar6
      001601 08               [12] 1112 	inc	r0
      001602 A6 07            [24] 1113 	mov	@r0,ar7
                                   1114 ;	tea.c:50: c = (vp[1] >> 5) + kp[1];
      001604 E5 08            [12] 1115 	mov	a,_bp
      001606 24 0E            [12] 1116 	add	a,#0x0e
      001608 F8               [12] 1117 	mov	r0,a
      001609 C0 01            [24] 1118 	push	ar1
      00160B E5 08            [12] 1119 	mov	a,_bp
      00160D 24 12            [12] 1120 	add	a,#0x12
      00160F F9               [12] 1121 	mov	r1,a
      001610 E6               [12] 1122 	mov	a,@r0
      001611 F7               [12] 1123 	mov	@r1,a
      001612 08               [12] 1124 	inc	r0
      001613 E6               [12] 1125 	mov	a,@r0
      001614 C4               [12] 1126 	swap	a
      001615 03               [12] 1127 	rr	a
      001616 C7               [12] 1128 	xch	a,@r1
      001617 C4               [12] 1129 	swap	a
      001618 03               [12] 1130 	rr	a
      001619 54 07            [12] 1131 	anl	a,#0x07
      00161B 67               [12] 1132 	xrl	a,@r1
      00161C C7               [12] 1133 	xch	a,@r1
      00161D 54 07            [12] 1134 	anl	a,#0x07
      00161F C7               [12] 1135 	xch	a,@r1
      001620 67               [12] 1136 	xrl	a,@r1
      001621 C7               [12] 1137 	xch	a,@r1
      001622 09               [12] 1138 	inc	r1
      001623 F7               [12] 1139 	mov	@r1,a
      001624 08               [12] 1140 	inc	r0
      001625 E6               [12] 1141 	mov	a,@r0
      001626 C4               [12] 1142 	swap	a
      001627 03               [12] 1143 	rr	a
      001628 54 F8            [12] 1144 	anl	a,#0xf8
      00162A 47               [12] 1145 	orl	a,@r1
      00162B F7               [12] 1146 	mov	@r1,a
      00162C E6               [12] 1147 	mov	a,@r0
      00162D 09               [12] 1148 	inc	r1
      00162E F7               [12] 1149 	mov	@r1,a
      00162F 08               [12] 1150 	inc	r0
      001630 E6               [12] 1151 	mov	a,@r0
      001631 C4               [12] 1152 	swap	a
      001632 03               [12] 1153 	rr	a
      001633 C7               [12] 1154 	xch	a,@r1
      001634 C4               [12] 1155 	swap	a
      001635 03               [12] 1156 	rr	a
      001636 54 07            [12] 1157 	anl	a,#0x07
      001638 67               [12] 1158 	xrl	a,@r1
      001639 C7               [12] 1159 	xch	a,@r1
      00163A 54 07            [12] 1160 	anl	a,#0x07
      00163C C7               [12] 1161 	xch	a,@r1
      00163D 67               [12] 1162 	xrl	a,@r1
      00163E C7               [12] 1163 	xch	a,@r1
      00163F 09               [12] 1164 	inc	r1
      001640 F7               [12] 1165 	mov	@r1,a
      001641 D0 01            [24] 1166 	pop	ar1
      001643 E5 08            [12] 1167 	mov	a,_bp
      001645 24 0C            [12] 1168 	add	a,#0x0c
      001647 F8               [12] 1169 	mov	r0,a
      001648 86 00            [24] 1170 	mov	ar0,@r0
      00164A 86 04            [24] 1171 	mov	ar4,@r0
      00164C 08               [12] 1172 	inc	r0
      00164D 86 05            [24] 1173 	mov	ar5,@r0
      00164F 08               [12] 1174 	inc	r0
      001650 86 06            [24] 1175 	mov	ar6,@r0
      001652 08               [12] 1176 	inc	r0
      001653 86 07            [24] 1177 	mov	ar7,@r0
      001655 18               [12] 1178 	dec	r0
      001656 18               [12] 1179 	dec	r0
      001657 18               [12] 1180 	dec	r0
      001658 E5 08            [12] 1181 	mov	a,_bp
      00165A 24 12            [12] 1182 	add	a,#0x12
      00165C F8               [12] 1183 	mov	r0,a
      00165D EC               [12] 1184 	mov	a,r4
      00165E 26               [12] 1185 	add	a,@r0
      00165F FC               [12] 1186 	mov	r4,a
      001660 ED               [12] 1187 	mov	a,r5
      001661 08               [12] 1188 	inc	r0
      001662 36               [12] 1189 	addc	a,@r0
      001663 FD               [12] 1190 	mov	r5,a
      001664 EE               [12] 1191 	mov	a,r6
      001665 08               [12] 1192 	inc	r0
      001666 36               [12] 1193 	addc	a,@r0
      001667 FE               [12] 1194 	mov	r6,a
      001668 EF               [12] 1195 	mov	a,r7
      001669 08               [12] 1196 	inc	r0
      00166A 36               [12] 1197 	addc	a,@r0
      00166B FF               [12] 1198 	mov	r7,a
      00166C E5 08            [12] 1199 	mov	a,_bp
      00166E 24 42            [12] 1200 	add	a,#0x42
      001670 F8               [12] 1201 	mov	r0,a
      001671 A6 04            [24] 1202 	mov	@r0,ar4
      001673 08               [12] 1203 	inc	r0
      001674 A6 05            [24] 1204 	mov	@r0,ar5
      001676 08               [12] 1205 	inc	r0
      001677 A6 06            [24] 1206 	mov	@r0,ar6
      001679 08               [12] 1207 	inc	r0
      00167A A6 07            [24] 1208 	mov	@r0,ar7
                                   1209 ;	tea.c:51: vp[0] += (a ^ b ^ c);
      00167C E5 08            [12] 1210 	mov	a,_bp
      00167E 24 12            [12] 1211 	add	a,#0x12
      001680 F8               [12] 1212 	mov	r0,a
      001681 E7               [12] 1213 	mov	a,@r1
      001682 F6               [12] 1214 	mov	@r0,a
      001683 09               [12] 1215 	inc	r1
      001684 E7               [12] 1216 	mov	a,@r1
      001685 08               [12] 1217 	inc	r0
      001686 F6               [12] 1218 	mov	@r0,a
      001687 09               [12] 1219 	inc	r1
      001688 E7               [12] 1220 	mov	a,@r1
      001689 08               [12] 1221 	inc	r0
      00168A F6               [12] 1222 	mov	@r0,a
      00168B 09               [12] 1223 	inc	r1
      00168C E7               [12] 1224 	mov	a,@r1
      00168D 08               [12] 1225 	inc	r0
      00168E F6               [12] 1226 	mov	@r0,a
      00168F 19               [12] 1227 	dec	r1
      001690 19               [12] 1228 	dec	r1
      001691 19               [12] 1229 	dec	r1
      001692 E5 08            [12] 1230 	mov	a,_bp
      001694 24 3A            [12] 1231 	add	a,#0x3a
      001696 F8               [12] 1232 	mov	r0,a
      001697 C0 01            [24] 1233 	push	ar1
      001699 E5 08            [12] 1234 	mov	a,_bp
      00169B 24 3E            [12] 1235 	add	a,#0x3e
      00169D F9               [12] 1236 	mov	r1,a
      00169E E7               [12] 1237 	mov	a,@r1
      00169F 66               [12] 1238 	xrl	a,@r0
      0016A0 FC               [12] 1239 	mov	r4,a
      0016A1 09               [12] 1240 	inc	r1
      0016A2 E7               [12] 1241 	mov	a,@r1
      0016A3 08               [12] 1242 	inc	r0
      0016A4 66               [12] 1243 	xrl	a,@r0
      0016A5 FD               [12] 1244 	mov	r5,a
      0016A6 09               [12] 1245 	inc	r1
      0016A7 E7               [12] 1246 	mov	a,@r1
      0016A8 08               [12] 1247 	inc	r0
      0016A9 66               [12] 1248 	xrl	a,@r0
      0016AA FE               [12] 1249 	mov	r6,a
      0016AB 09               [12] 1250 	inc	r1
      0016AC E7               [12] 1251 	mov	a,@r1
      0016AD 08               [12] 1252 	inc	r0
      0016AE 66               [12] 1253 	xrl	a,@r0
      0016AF FF               [12] 1254 	mov	r7,a
      0016B0 D0 01            [24] 1255 	pop	ar1
      0016B2 E5 08            [12] 1256 	mov	a,_bp
      0016B4 24 42            [12] 1257 	add	a,#0x42
      0016B6 F8               [12] 1258 	mov	r0,a
      0016B7 E6               [12] 1259 	mov	a,@r0
      0016B8 62 04            [12] 1260 	xrl	ar4,a
      0016BA 08               [12] 1261 	inc	r0
      0016BB E6               [12] 1262 	mov	a,@r0
      0016BC 62 05            [12] 1263 	xrl	ar5,a
      0016BE 08               [12] 1264 	inc	r0
      0016BF E6               [12] 1265 	mov	a,@r0
      0016C0 62 06            [12] 1266 	xrl	ar6,a
      0016C2 08               [12] 1267 	inc	r0
      0016C3 E6               [12] 1268 	mov	a,@r0
      0016C4 62 07            [12] 1269 	xrl	ar7,a
      0016C6 E5 08            [12] 1270 	mov	a,_bp
      0016C8 24 12            [12] 1271 	add	a,#0x12
      0016CA F8               [12] 1272 	mov	r0,a
      0016CB EC               [12] 1273 	mov	a,r4
      0016CC 26               [12] 1274 	add	a,@r0
      0016CD F6               [12] 1275 	mov	@r0,a
      0016CE ED               [12] 1276 	mov	a,r5
      0016CF 08               [12] 1277 	inc	r0
      0016D0 36               [12] 1278 	addc	a,@r0
      0016D1 F6               [12] 1279 	mov	@r0,a
      0016D2 EE               [12] 1280 	mov	a,r6
      0016D3 08               [12] 1281 	inc	r0
      0016D4 36               [12] 1282 	addc	a,@r0
      0016D5 F6               [12] 1283 	mov	@r0,a
      0016D6 EF               [12] 1284 	mov	a,r7
      0016D7 08               [12] 1285 	inc	r0
      0016D8 36               [12] 1286 	addc	a,@r0
      0016D9 F6               [12] 1287 	mov	@r0,a
      0016DA E5 08            [12] 1288 	mov	a,_bp
      0016DC 24 12            [12] 1289 	add	a,#0x12
      0016DE F8               [12] 1290 	mov	r0,a
      0016DF E6               [12] 1291 	mov	a,@r0
      0016E0 F7               [12] 1292 	mov	@r1,a
      0016E1 09               [12] 1293 	inc	r1
      0016E2 08               [12] 1294 	inc	r0
      0016E3 E6               [12] 1295 	mov	a,@r0
      0016E4 F7               [12] 1296 	mov	@r1,a
      0016E5 09               [12] 1297 	inc	r1
      0016E6 08               [12] 1298 	inc	r0
      0016E7 E6               [12] 1299 	mov	a,@r0
      0016E8 F7               [12] 1300 	mov	@r1,a
      0016E9 09               [12] 1301 	inc	r1
      0016EA 08               [12] 1302 	inc	r0
      0016EB E6               [12] 1303 	mov	a,@r0
      0016EC F7               [12] 1304 	mov	@r1,a
      0016ED 19               [12] 1305 	dec	r1
      0016EE 19               [12] 1306 	dec	r1
      0016EF 19               [12] 1307 	dec	r1
                                   1308 ;	tea.c:54: a = (vp[0] << 4) + kp[2];
      0016F0 E5 08            [12] 1309 	mov	a,_bp
      0016F2 24 12            [12] 1310 	add	a,#0x12
      0016F4 F8               [12] 1311 	mov	r0,a
      0016F5 C0 01            [24] 1312 	push	ar1
      0016F7 E5 08            [12] 1313 	mov	a,_bp
      0016F9 24 0E            [12] 1314 	add	a,#0x0e
      0016FB F9               [12] 1315 	mov	r1,a
      0016FC 08               [12] 1316 	inc	r0
      0016FD 08               [12] 1317 	inc	r0
      0016FE E6               [12] 1318 	mov	a,@r0
      0016FF 09               [12] 1319 	inc	r1
      001700 09               [12] 1320 	inc	r1
      001701 F7               [12] 1321 	mov	@r1,a
      001702 08               [12] 1322 	inc	r0
      001703 E6               [12] 1323 	mov	a,@r0
      001704 C4               [12] 1324 	swap	a
      001705 54 F0            [12] 1325 	anl	a,#0xf0
      001707 C7               [12] 1326 	xch	a,@r1
      001708 C4               [12] 1327 	swap	a
      001709 C7               [12] 1328 	xch	a,@r1
      00170A 67               [12] 1329 	xrl	a,@r1
      00170B C7               [12] 1330 	xch	a,@r1
      00170C 54 F0            [12] 1331 	anl	a,#0xf0
      00170E C7               [12] 1332 	xch	a,@r1
      00170F 67               [12] 1333 	xrl	a,@r1
      001710 09               [12] 1334 	inc	r1
      001711 F7               [12] 1335 	mov	@r1,a
      001712 18               [12] 1336 	dec	r0
      001713 18               [12] 1337 	dec	r0
      001714 E6               [12] 1338 	mov	a,@r0
      001715 C4               [12] 1339 	swap	a
      001716 54 0F            [12] 1340 	anl	a,#0x0f
      001718 19               [12] 1341 	dec	r1
      001719 47               [12] 1342 	orl	a,@r1
      00171A F7               [12] 1343 	mov	@r1,a
      00171B 18               [12] 1344 	dec	r0
      00171C E6               [12] 1345 	mov	a,@r0
      00171D 19               [12] 1346 	dec	r1
      00171E 19               [12] 1347 	dec	r1
      00171F F7               [12] 1348 	mov	@r1,a
      001720 08               [12] 1349 	inc	r0
      001721 E6               [12] 1350 	mov	a,@r0
      001722 C4               [12] 1351 	swap	a
      001723 54 F0            [12] 1352 	anl	a,#0xf0
      001725 C7               [12] 1353 	xch	a,@r1
      001726 C4               [12] 1354 	swap	a
      001727 C7               [12] 1355 	xch	a,@r1
      001728 67               [12] 1356 	xrl	a,@r1
      001729 C7               [12] 1357 	xch	a,@r1
      00172A 54 F0            [12] 1358 	anl	a,#0xf0
      00172C C7               [12] 1359 	xch	a,@r1
      00172D 67               [12] 1360 	xrl	a,@r1
      00172E 09               [12] 1361 	inc	r1
      00172F F7               [12] 1362 	mov	@r1,a
      001730 E5 08            [12] 1363 	mov	a,_bp
      001732 24 04            [12] 1364 	add	a,#0x04
      001734 F8               [12] 1365 	mov	r0,a
      001735 86 00            [24] 1366 	mov	ar0,@r0
      001737 86 04            [24] 1367 	mov	ar4,@r0
      001739 08               [12] 1368 	inc	r0
      00173A 86 05            [24] 1369 	mov	ar5,@r0
      00173C 08               [12] 1370 	inc	r0
      00173D 86 06            [24] 1371 	mov	ar6,@r0
      00173F 08               [12] 1372 	inc	r0
      001740 86 07            [24] 1373 	mov	ar7,@r0
      001742 18               [12] 1374 	dec	r0
      001743 18               [12] 1375 	dec	r0
      001744 18               [12] 1376 	dec	r0
      001745 E5 08            [12] 1377 	mov	a,_bp
      001747 24 0E            [12] 1378 	add	a,#0x0e
      001749 F8               [12] 1379 	mov	r0,a
      00174A EC               [12] 1380 	mov	a,r4
      00174B 26               [12] 1381 	add	a,@r0
      00174C FC               [12] 1382 	mov	r4,a
      00174D ED               [12] 1383 	mov	a,r5
      00174E 08               [12] 1384 	inc	r0
      00174F 36               [12] 1385 	addc	a,@r0
      001750 FD               [12] 1386 	mov	r5,a
      001751 EE               [12] 1387 	mov	a,r6
      001752 08               [12] 1388 	inc	r0
      001753 36               [12] 1389 	addc	a,@r0
      001754 FE               [12] 1390 	mov	r6,a
      001755 EF               [12] 1391 	mov	a,r7
      001756 08               [12] 1392 	inc	r0
      001757 36               [12] 1393 	addc	a,@r0
      001758 FF               [12] 1394 	mov	r7,a
      001759 E5 08            [12] 1395 	mov	a,_bp
      00175B 24 3A            [12] 1396 	add	a,#0x3a
      00175D F8               [12] 1397 	mov	r0,a
      00175E A6 04            [24] 1398 	mov	@r0,ar4
      001760 08               [12] 1399 	inc	r0
      001761 A6 05            [24] 1400 	mov	@r0,ar5
      001763 08               [12] 1401 	inc	r0
      001764 A6 06            [24] 1402 	mov	@r0,ar6
      001766 08               [12] 1403 	inc	r0
      001767 A6 07            [24] 1404 	mov	@r0,ar7
                                   1405 ;	tea.c:55: b = (vp[0] + sum);
      001769 E5 08            [12] 1406 	mov	a,_bp
      00176B 24 12            [12] 1407 	add	a,#0x12
      00176D F8               [12] 1408 	mov	r0,a
      00176E E5 08            [12] 1409 	mov	a,_bp
      001770 24 2E            [12] 1410 	add	a,#0x2e
      001772 F9               [12] 1411 	mov	r1,a
      001773 E7               [12] 1412 	mov	a,@r1
      001774 26               [12] 1413 	add	a,@r0
      001775 FC               [12] 1414 	mov	r4,a
      001776 09               [12] 1415 	inc	r1
      001777 E7               [12] 1416 	mov	a,@r1
      001778 08               [12] 1417 	inc	r0
      001779 36               [12] 1418 	addc	a,@r0
      00177A FD               [12] 1419 	mov	r5,a
      00177B 09               [12] 1420 	inc	r1
      00177C E7               [12] 1421 	mov	a,@r1
      00177D 08               [12] 1422 	inc	r0
      00177E 36               [12] 1423 	addc	a,@r0
      00177F FE               [12] 1424 	mov	r6,a
      001780 09               [12] 1425 	inc	r1
      001781 E7               [12] 1426 	mov	a,@r1
      001782 08               [12] 1427 	inc	r0
      001783 36               [12] 1428 	addc	a,@r0
      001784 FF               [12] 1429 	mov	r7,a
      001785 E5 08            [12] 1430 	mov	a,_bp
      001787 24 3E            [12] 1431 	add	a,#0x3e
      001789 F8               [12] 1432 	mov	r0,a
      00178A A6 04            [24] 1433 	mov	@r0,ar4
      00178C 08               [12] 1434 	inc	r0
      00178D A6 05            [24] 1435 	mov	@r0,ar5
      00178F 08               [12] 1436 	inc	r0
      001790 A6 06            [24] 1437 	mov	@r0,ar6
      001792 08               [12] 1438 	inc	r0
      001793 A6 07            [24] 1439 	mov	@r0,ar7
                                   1440 ;	tea.c:56: c = (vp[0] >> 5) + kp[3];
      001795 E5 08            [12] 1441 	mov	a,_bp
      001797 24 12            [12] 1442 	add	a,#0x12
      001799 F8               [12] 1443 	mov	r0,a
      00179A 08               [12] 1444 	inc	r0
      00179B E6               [12] 1445 	mov	a,@r0
      00179C 18               [12] 1446 	dec	r0
      00179D C4               [12] 1447 	swap	a
      00179E 03               [12] 1448 	rr	a
      00179F C6               [12] 1449 	xch	a,@r0
      0017A0 C4               [12] 1450 	swap	a
      0017A1 03               [12] 1451 	rr	a
      0017A2 54 07            [12] 1452 	anl	a,#0x07
      0017A4 66               [12] 1453 	xrl	a,@r0
      0017A5 C6               [12] 1454 	xch	a,@r0
      0017A6 54 07            [12] 1455 	anl	a,#0x07
      0017A8 C6               [12] 1456 	xch	a,@r0
      0017A9 66               [12] 1457 	xrl	a,@r0
      0017AA C6               [12] 1458 	xch	a,@r0
      0017AB 08               [12] 1459 	inc	r0
      0017AC F6               [12] 1460 	mov	@r0,a
      0017AD 08               [12] 1461 	inc	r0
      0017AE E6               [12] 1462 	mov	a,@r0
      0017AF C4               [12] 1463 	swap	a
      0017B0 03               [12] 1464 	rr	a
      0017B1 54 F8            [12] 1465 	anl	a,#0xf8
      0017B3 18               [12] 1466 	dec	r0
      0017B4 46               [12] 1467 	orl	a,@r0
      0017B5 F6               [12] 1468 	mov	@r0,a
      0017B6 08               [12] 1469 	inc	r0
      0017B7 08               [12] 1470 	inc	r0
      0017B8 E6               [12] 1471 	mov	a,@r0
      0017B9 18               [12] 1472 	dec	r0
      0017BA C4               [12] 1473 	swap	a
      0017BB 03               [12] 1474 	rr	a
      0017BC C6               [12] 1475 	xch	a,@r0
      0017BD C4               [12] 1476 	swap	a
      0017BE 03               [12] 1477 	rr	a
      0017BF 54 07            [12] 1478 	anl	a,#0x07
      0017C1 66               [12] 1479 	xrl	a,@r0
      0017C2 C6               [12] 1480 	xch	a,@r0
      0017C3 54 07            [12] 1481 	anl	a,#0x07
      0017C5 C6               [12] 1482 	xch	a,@r0
      0017C6 66               [12] 1483 	xrl	a,@r0
      0017C7 C6               [12] 1484 	xch	a,@r0
      0017C8 08               [12] 1485 	inc	r0
      0017C9 F6               [12] 1486 	mov	@r0,a
      0017CA E5 08            [12] 1487 	mov	a,_bp
      0017CC 24 08            [12] 1488 	add	a,#0x08
      0017CE F8               [12] 1489 	mov	r0,a
      0017CF 86 00            [24] 1490 	mov	ar0,@r0
      0017D1 86 02            [24] 1491 	mov	ar2,@r0
      0017D3 08               [12] 1492 	inc	r0
      0017D4 86 03            [24] 1493 	mov	ar3,@r0
      0017D6 08               [12] 1494 	inc	r0
      0017D7 86 06            [24] 1495 	mov	ar6,@r0
      0017D9 08               [12] 1496 	inc	r0
      0017DA 86 07            [24] 1497 	mov	ar7,@r0
      0017DC 18               [12] 1498 	dec	r0
      0017DD 18               [12] 1499 	dec	r0
      0017DE 18               [12] 1500 	dec	r0
      0017DF E5 08            [12] 1501 	mov	a,_bp
      0017E1 24 12            [12] 1502 	add	a,#0x12
      0017E3 F8               [12] 1503 	mov	r0,a
      0017E4 EA               [12] 1504 	mov	a,r2
      0017E5 26               [12] 1505 	add	a,@r0
      0017E6 FA               [12] 1506 	mov	r2,a
      0017E7 EB               [12] 1507 	mov	a,r3
      0017E8 08               [12] 1508 	inc	r0
      0017E9 36               [12] 1509 	addc	a,@r0
      0017EA FB               [12] 1510 	mov	r3,a
      0017EB EE               [12] 1511 	mov	a,r6
      0017EC 08               [12] 1512 	inc	r0
      0017ED 36               [12] 1513 	addc	a,@r0
      0017EE FE               [12] 1514 	mov	r6,a
      0017EF EF               [12] 1515 	mov	a,r7
      0017F0 08               [12] 1516 	inc	r0
      0017F1 36               [12] 1517 	addc	a,@r0
      0017F2 FF               [12] 1518 	mov	r7,a
      0017F3 E5 08            [12] 1519 	mov	a,_bp
      0017F5 24 42            [12] 1520 	add	a,#0x42
      0017F7 F8               [12] 1521 	mov	r0,a
      0017F8 A6 02            [24] 1522 	mov	@r0,ar2
      0017FA 08               [12] 1523 	inc	r0
      0017FB A6 03            [24] 1524 	mov	@r0,ar3
      0017FD 08               [12] 1525 	inc	r0
      0017FE A6 06            [24] 1526 	mov	@r0,ar6
      001800 08               [12] 1527 	inc	r0
      001801 A6 07            [24] 1528 	mov	@r0,ar7
                                   1529 ;	tea.c:57: vp[1] += (a ^ b ^ c);
      001803 E5 08            [12] 1530 	mov	a,_bp
      001805 24 0D            [12] 1531 	add	a,#0x0d
      001807 F8               [12] 1532 	mov	r0,a
      001808 E5 08            [12] 1533 	mov	a,_bp
      00180A 24 12            [12] 1534 	add	a,#0x12
      00180C F9               [12] 1535 	mov	r1,a
      00180D 86 00            [24] 1536 	mov	ar0,@r0
      00180F E6               [12] 1537 	mov	a,@r0
      001810 F7               [12] 1538 	mov	@r1,a
      001811 08               [12] 1539 	inc	r0
      001812 E6               [12] 1540 	mov	a,@r0
      001813 09               [12] 1541 	inc	r1
      001814 F7               [12] 1542 	mov	@r1,a
      001815 08               [12] 1543 	inc	r0
      001816 E6               [12] 1544 	mov	a,@r0
      001817 09               [12] 1545 	inc	r1
      001818 F7               [12] 1546 	mov	@r1,a
      001819 08               [12] 1547 	inc	r0
      00181A E6               [12] 1548 	mov	a,@r0
      00181B 09               [12] 1549 	inc	r1
      00181C F7               [12] 1550 	mov	@r1,a
      00181D 18               [12] 1551 	dec	r0
      00181E 18               [12] 1552 	dec	r0
      00181F 18               [12] 1553 	dec	r0
      001820 E5 08            [12] 1554 	mov	a,_bp
      001822 24 3A            [12] 1555 	add	a,#0x3a
      001824 F8               [12] 1556 	mov	r0,a
      001825 E5 08            [12] 1557 	mov	a,_bp
      001827 24 3E            [12] 1558 	add	a,#0x3e
      001829 F9               [12] 1559 	mov	r1,a
      00182A E7               [12] 1560 	mov	a,@r1
      00182B 66               [12] 1561 	xrl	a,@r0
      00182C FA               [12] 1562 	mov	r2,a
      00182D 09               [12] 1563 	inc	r1
      00182E E7               [12] 1564 	mov	a,@r1
      00182F 08               [12] 1565 	inc	r0
      001830 66               [12] 1566 	xrl	a,@r0
      001831 FB               [12] 1567 	mov	r3,a
      001832 09               [12] 1568 	inc	r1
      001833 E7               [12] 1569 	mov	a,@r1
      001834 08               [12] 1570 	inc	r0
      001835 66               [12] 1571 	xrl	a,@r0
      001836 FE               [12] 1572 	mov	r6,a
      001837 09               [12] 1573 	inc	r1
      001838 E7               [12] 1574 	mov	a,@r1
      001839 08               [12] 1575 	inc	r0
      00183A 66               [12] 1576 	xrl	a,@r0
      00183B FF               [12] 1577 	mov	r7,a
      00183C D0 01            [24] 1578 	pop	ar1
      00183E E5 08            [12] 1579 	mov	a,_bp
      001840 24 42            [12] 1580 	add	a,#0x42
      001842 F8               [12] 1581 	mov	r0,a
      001843 E6               [12] 1582 	mov	a,@r0
      001844 62 02            [12] 1583 	xrl	ar2,a
      001846 08               [12] 1584 	inc	r0
      001847 E6               [12] 1585 	mov	a,@r0
      001848 62 03            [12] 1586 	xrl	ar3,a
      00184A 08               [12] 1587 	inc	r0
      00184B E6               [12] 1588 	mov	a,@r0
      00184C 62 06            [12] 1589 	xrl	ar6,a
      00184E 08               [12] 1590 	inc	r0
      00184F E6               [12] 1591 	mov	a,@r0
      001850 62 07            [12] 1592 	xrl	ar7,a
      001852 E5 08            [12] 1593 	mov	a,_bp
      001854 24 12            [12] 1594 	add	a,#0x12
      001856 F8               [12] 1595 	mov	r0,a
      001857 EA               [12] 1596 	mov	a,r2
      001858 26               [12] 1597 	add	a,@r0
      001859 FA               [12] 1598 	mov	r2,a
      00185A EB               [12] 1599 	mov	a,r3
      00185B 08               [12] 1600 	inc	r0
      00185C 36               [12] 1601 	addc	a,@r0
      00185D FB               [12] 1602 	mov	r3,a
      00185E EE               [12] 1603 	mov	a,r6
      00185F 08               [12] 1604 	inc	r0
      001860 36               [12] 1605 	addc	a,@r0
      001861 FE               [12] 1606 	mov	r6,a
      001862 EF               [12] 1607 	mov	a,r7
      001863 08               [12] 1608 	inc	r0
      001864 36               [12] 1609 	addc	a,@r0
      001865 FF               [12] 1610 	mov	r7,a
      001866 E5 08            [12] 1611 	mov	a,_bp
      001868 24 0D            [12] 1612 	add	a,#0x0d
      00186A F8               [12] 1613 	mov	r0,a
      00186B 86 00            [24] 1614 	mov	ar0,@r0
      00186D A6 02            [24] 1615 	mov	@r0,ar2
      00186F 08               [12] 1616 	inc	r0
      001870 A6 03            [24] 1617 	mov	@r0,ar3
      001872 08               [12] 1618 	inc	r0
      001873 A6 06            [24] 1619 	mov	@r0,ar6
      001875 08               [12] 1620 	inc	r0
      001876 A6 07            [24] 1621 	mov	@r0,ar7
      001878 18               [12] 1622 	dec	r0
      001879 18               [12] 1623 	dec	r0
      00187A 18               [12] 1624 	dec	r0
      00187B E5 08            [12] 1625 	mov	a,_bp
      00187D 24 32            [12] 1626 	add	a,#0x32
      00187F F8               [12] 1627 	mov	r0,a
      001880 16               [12] 1628 	dec	@r0
      001881 B6 FF 0C         [24] 1629 	cjne	@r0,#0xff,00169$
      001884 08               [12] 1630 	inc	r0
      001885 16               [12] 1631 	dec	@r0
      001886 B6 FF 07         [24] 1632 	cjne	@r0,#0xff,00169$
      001889 08               [12] 1633 	inc	r0
      00188A 16               [12] 1634 	dec	@r0
      00188B B6 FF 02         [24] 1635 	cjne	@r0,#0xff,00169$
      00188E 08               [12] 1636 	inc	r0
      00188F 16               [12] 1637 	dec	@r0
      001890                       1638 00169$:
      001890 D0 00            [24] 1639 	pop	ar0
                                   1640 ;	tea.c:42: for (i=0; i < 32; i++) {                       /* basic cycle start */
      001892 E5 08            [12] 1641 	mov	a,_bp
      001894 24 32            [12] 1642 	add	a,#0x32
      001896 F8               [12] 1643 	mov	r0,a
      001897 E6               [12] 1644 	mov	a,@r0
      001898 08               [12] 1645 	inc	r0
      001899 46               [12] 1646 	orl	a,@r0
      00189A 08               [12] 1647 	inc	r0
      00189B 46               [12] 1648 	orl	a,@r0
      00189C 08               [12] 1649 	inc	r0
      00189D 46               [12] 1650 	orl	a,@r0
      00189E D0 00            [24] 1651 	pop	ar0
      0018A0 60 03            [24] 1652 	jz	00170$
      0018A2 02 15 16         [24] 1653 	ljmp	00117$
      0018A5                       1654 00170$:
                                   1655 ;	tea.c:61: for(i = 0; i < 8; i++)
      0018A5 E5 08            [12] 1656 	mov	a,_bp
      0018A7 24 32            [12] 1657 	add	a,#0x32
      0018A9 F8               [12] 1658 	mov	r0,a
      0018AA E4               [12] 1659 	clr	a
      0018AB F6               [12] 1660 	mov	@r0,a
      0018AC 08               [12] 1661 	inc	r0
      0018AD F6               [12] 1662 	mov	@r0,a
      0018AE 08               [12] 1663 	inc	r0
      0018AF F6               [12] 1664 	mov	@r0,a
      0018B0 08               [12] 1665 	inc	r0
      0018B1 F6               [12] 1666 	mov	@r0,a
      0018B2                       1667 00118$:
                                   1668 ;	tea.c:63: uint8_t shift = 24 - 8 * (i%4);
      0018B2 E5 08            [12] 1669 	mov	a,_bp
      0018B4 24 32            [12] 1670 	add	a,#0x32
      0018B6 F8               [12] 1671 	mov	r0,a
      0018B7 74 03            [12] 1672 	mov	a,#0x03
      0018B9 56               [12] 1673 	anl	a,@r0
      0018BA FA               [12] 1674 	mov	r2,a
      0018BB E4               [12] 1675 	clr	a
      0018BC EA               [12] 1676 	mov	a,r2
      0018BD C4               [12] 1677 	swap	a
      0018BE 03               [12] 1678 	rr	a
      0018BF 54 F8            [12] 1679 	anl	a,#0xf8
      0018C1 FA               [12] 1680 	mov	r2,a
      0018C2 E5 08            [12] 1681 	mov	a,_bp
      0018C4 24 3A            [12] 1682 	add	a,#0x3a
      0018C6 F8               [12] 1683 	mov	r0,a
      0018C7 74 18            [12] 1684 	mov	a,#0x18
      0018C9 C3               [12] 1685 	clr	c
      0018CA 9A               [12] 1686 	subb	a,r2
      0018CB F6               [12] 1687 	mov	@r0,a
                                   1688 ;	tea.c:64: v[i] = (vp[i/4] >> shift) & 0xff;
      0018CC A8 08            [24] 1689 	mov	r0,_bp
      0018CE 08               [12] 1690 	inc	r0
      0018CF C0 01            [24] 1691 	push	ar1
      0018D1 E5 08            [12] 1692 	mov	a,_bp
      0018D3 24 32            [12] 1693 	add	a,#0x32
      0018D5 F9               [12] 1694 	mov	r1,a
      0018D6 E7               [12] 1695 	mov	a,@r1
      0018D7 26               [12] 1696 	add	a,@r0
      0018D8 C0 E0            [24] 1697 	push	acc
      0018DA 09               [12] 1698 	inc	r1
      0018DB E7               [12] 1699 	mov	a,@r1
      0018DC 08               [12] 1700 	inc	r0
      0018DD 36               [12] 1701 	addc	a,@r0
      0018DE C0 E0            [24] 1702 	push	acc
      0018E0 08               [12] 1703 	inc	r0
      0018E1 E6               [12] 1704 	mov	a,@r0
      0018E2 C0 E0            [24] 1705 	push	acc
      0018E4 E5 08            [12] 1706 	mov	a,_bp
      0018E6 24 14            [12] 1707 	add	a,#0x14
      0018E8 F8               [12] 1708 	mov	r0,a
      0018E9 D0 E0            [24] 1709 	pop	acc
      0018EB F6               [12] 1710 	mov	@r0,a
      0018EC 18               [12] 1711 	dec	r0
      0018ED D0 E0            [24] 1712 	pop	acc
      0018EF F6               [12] 1713 	mov	@r0,a
      0018F0 18               [12] 1714 	dec	r0
      0018F1 D0 E0            [24] 1715 	pop	acc
      0018F3 F6               [12] 1716 	mov	@r0,a
      0018F4 D0 01            [24] 1717 	pop	ar1
      0018F6 E5 08            [12] 1718 	mov	a,_bp
      0018F8 24 32            [12] 1719 	add	a,#0x32
      0018FA F8               [12] 1720 	mov	r0,a
      0018FB 08               [12] 1721 	inc	r0
      0018FC 08               [12] 1722 	inc	r0
      0018FD 08               [12] 1723 	inc	r0
      0018FE E6               [12] 1724 	mov	a,@r0
      0018FF C3               [12] 1725 	clr	c
      001900 13               [12] 1726 	rrc	a
      001901 FF               [12] 1727 	mov	r7,a
      001902 18               [12] 1728 	dec	r0
      001903 E6               [12] 1729 	mov	a,@r0
      001904 13               [12] 1730 	rrc	a
      001905 FC               [12] 1731 	mov	r4,a
      001906 18               [12] 1732 	dec	r0
      001907 E6               [12] 1733 	mov	a,@r0
      001908 13               [12] 1734 	rrc	a
      001909 FB               [12] 1735 	mov	r3,a
      00190A 18               [12] 1736 	dec	r0
      00190B E6               [12] 1737 	mov	a,@r0
      00190C 13               [12] 1738 	rrc	a
      00190D FA               [12] 1739 	mov	r2,a
      00190E EF               [12] 1740 	mov	a,r7
      00190F C3               [12] 1741 	clr	c
      001910 13               [12] 1742 	rrc	a
      001911 FF               [12] 1743 	mov	r7,a
      001912 EC               [12] 1744 	mov	a,r4
      001913 13               [12] 1745 	rrc	a
      001914 FC               [12] 1746 	mov	r4,a
      001915 EB               [12] 1747 	mov	a,r3
      001916 13               [12] 1748 	rrc	a
      001917 FB               [12] 1749 	mov	r3,a
      001918 EA               [12] 1750 	mov	a,r2
      001919 13               [12] 1751 	rrc	a
      00191A 25 E0            [12] 1752 	add	a,acc
      00191C FA               [12] 1753 	mov	r2,a
      00191D EB               [12] 1754 	mov	a,r3
      00191E 33               [12] 1755 	rlc	a
      00191F FB               [12] 1756 	mov	r3,a
      001920 EC               [12] 1757 	mov	a,r4
      001921 33               [12] 1758 	rlc	a
      001922 FC               [12] 1759 	mov	r4,a
      001923 EF               [12] 1760 	mov	a,r7
      001924 33               [12] 1761 	rlc	a
      001925 FF               [12] 1762 	mov	r7,a
      001926 EA               [12] 1763 	mov	a,r2
      001927 2A               [12] 1764 	add	a,r2
      001928 FA               [12] 1765 	mov	r2,a
      001929 EB               [12] 1766 	mov	a,r3
      00192A 33               [12] 1767 	rlc	a
      00192B FB               [12] 1768 	mov	r3,a
      00192C EC               [12] 1769 	mov	a,r4
      00192D 33               [12] 1770 	rlc	a
      00192E EF               [12] 1771 	mov	a,r7
      00192F 33               [12] 1772 	rlc	a
      001930 EA               [12] 1773 	mov	a,r2
      001931 29               [12] 1774 	add	a,r1
      001932 FF               [12] 1775 	mov	r7,a
      001933 A8 07            [24] 1776 	mov	r0,ar7
      001935 86 04            [24] 1777 	mov	ar4,@r0
      001937 08               [12] 1778 	inc	r0
      001938 86 05            [24] 1779 	mov	ar5,@r0
      00193A 08               [12] 1780 	inc	r0
      00193B 86 06            [24] 1781 	mov	ar6,@r0
      00193D 08               [12] 1782 	inc	r0
      00193E 86 07            [24] 1783 	mov	ar7,@r0
      001940 E5 08            [12] 1784 	mov	a,_bp
      001942 24 3A            [12] 1785 	add	a,#0x3a
      001944 F8               [12] 1786 	mov	r0,a
      001945 86 F0            [24] 1787 	mov	b,@r0
      001947 05 F0            [12] 1788 	inc	b
      001949 80 0D            [24] 1789 	sjmp	00172$
      00194B                       1790 00171$:
      00194B C3               [12] 1791 	clr	c
      00194C EF               [12] 1792 	mov	a,r7
      00194D 13               [12] 1793 	rrc	a
      00194E FF               [12] 1794 	mov	r7,a
      00194F EE               [12] 1795 	mov	a,r6
      001950 13               [12] 1796 	rrc	a
      001951 FE               [12] 1797 	mov	r6,a
      001952 ED               [12] 1798 	mov	a,r5
      001953 13               [12] 1799 	rrc	a
      001954 FD               [12] 1800 	mov	r5,a
      001955 EC               [12] 1801 	mov	a,r4
      001956 13               [12] 1802 	rrc	a
      001957 FC               [12] 1803 	mov	r4,a
      001958                       1804 00172$:
      001958 D5 F0 F0         [24] 1805 	djnz	b,00171$
      00195B E4               [12] 1806 	clr	a
      00195C FD               [12] 1807 	mov	r5,a
      00195D FE               [12] 1808 	mov	r6,a
      00195E FF               [12] 1809 	mov	r7,a
      00195F E5 08            [12] 1810 	mov	a,_bp
      001961 24 12            [12] 1811 	add	a,#0x12
      001963 F8               [12] 1812 	mov	r0,a
      001964 86 82            [24] 1813 	mov	dpl,@r0
      001966 08               [12] 1814 	inc	r0
      001967 86 83            [24] 1815 	mov	dph,@r0
      001969 08               [12] 1816 	inc	r0
      00196A 86 F0            [24] 1817 	mov	b,@r0
      00196C EC               [12] 1818 	mov	a,r4
      00196D 12 1B 7D         [24] 1819 	lcall	__gptrput
                                   1820 ;	tea.c:61: for(i = 0; i < 8; i++)
      001970 E5 08            [12] 1821 	mov	a,_bp
      001972 24 32            [12] 1822 	add	a,#0x32
      001974 F8               [12] 1823 	mov	r0,a
      001975 06               [12] 1824 	inc	@r0
      001976 B6 00 0C         [24] 1825 	cjne	@r0,#0x00,00173$
      001979 08               [12] 1826 	inc	r0
      00197A 06               [12] 1827 	inc	@r0
      00197B B6 00 07         [24] 1828 	cjne	@r0,#0x00,00173$
      00197E 08               [12] 1829 	inc	r0
      00197F 06               [12] 1830 	inc	@r0
      001980 B6 00 02         [24] 1831 	cjne	@r0,#0x00,00173$
      001983 08               [12] 1832 	inc	r0
      001984 06               [12] 1833 	inc	@r0
      001985                       1834 00173$:
      001985 E5 08            [12] 1835 	mov	a,_bp
      001987 24 32            [12] 1836 	add	a,#0x32
      001989 F8               [12] 1837 	mov	r0,a
      00198A C3               [12] 1838 	clr	c
      00198B E6               [12] 1839 	mov	a,@r0
      00198C 94 08            [12] 1840 	subb	a,#0x08
      00198E 08               [12] 1841 	inc	r0
      00198F E6               [12] 1842 	mov	a,@r0
      001990 94 00            [12] 1843 	subb	a,#0x00
      001992 08               [12] 1844 	inc	r0
      001993 E6               [12] 1845 	mov	a,@r0
      001994 94 00            [12] 1846 	subb	a,#0x00
      001996 08               [12] 1847 	inc	r0
      001997 E6               [12] 1848 	mov	a,@r0
      001998 94 00            [12] 1849 	subb	a,#0x00
      00199A 50 03            [24] 1850 	jnc	00174$
      00199C 02 18 B2         [24] 1851 	ljmp	00118$
      00199F                       1852 00174$:
      00199F 85 08 81         [24] 1853 	mov	sp,_bp
      0019A2 D0 08            [24] 1854 	pop	_bp
      0019A4 22               [24] 1855 	ret
                                   1856 ;------------------------------------------------------------
                                   1857 ;Allocation info for local variables in function 'main_tea'
                                   1858 ;------------------------------------------------------------
                                   1859 ;input                     Allocated to stack - _bp +4
                                   1860 ;key                       Allocated to stack - _bp +12
                                   1861 ;sloc0                     Allocated to stack - _bp +1
                                   1862 ;------------------------------------------------------------
                                   1863 ;	tea.c:68: void main_tea(void) 
                                   1864 ;	-----------------------------------------
                                   1865 ;	 function main_tea
                                   1866 ;	-----------------------------------------
      0019A5                       1867 _main_tea:
      0019A5 C0 08            [24] 1868 	push	_bp
      0019A7 E5 81            [12] 1869 	mov	a,sp
      0019A9 F5 08            [12] 1870 	mov	_bp,a
      0019AB 24 1B            [12] 1871 	add	a,#0x1b
      0019AD F5 81            [12] 1872 	mov	sp,a
                                   1873 ;	tea.c:75: while(1)
      0019AF                       1874 00104$:
                                   1875 ;	tea.c:78: if(simpleserial_get(input, key, TEA_BLOCK_SIZE, TEA_KEY_SIZE))
      0019AF E5 08            [12] 1876 	mov	a,_bp
      0019B1 24 0C            [12] 1877 	add	a,#0x0c
      0019B3 FF               [12] 1878 	mov	r7,a
      0019B4 A8 08            [24] 1879 	mov	r0,_bp
      0019B6 08               [12] 1880 	inc	r0
      0019B7 A6 07            [24] 1881 	mov	@r0,ar7
      0019B9 08               [12] 1882 	inc	r0
      0019BA 76 00            [12] 1883 	mov	@r0,#0x00
      0019BC 08               [12] 1884 	inc	r0
      0019BD 76 40            [12] 1885 	mov	@r0,#0x40
      0019BF E5 08            [12] 1886 	mov	a,_bp
      0019C1 24 04            [12] 1887 	add	a,#0x04
      0019C3 FB               [12] 1888 	mov	r3,a
      0019C4 FA               [12] 1889 	mov	r2,a
      0019C5 7D 00            [12] 1890 	mov	r5,#0x00
      0019C7 7E 40            [12] 1891 	mov	r6,#0x40
      0019C9 C0 07            [24] 1892 	push	ar7
      0019CB C0 03            [24] 1893 	push	ar3
      0019CD 74 10            [12] 1894 	mov	a,#0x10
      0019CF C0 E0            [24] 1895 	push	acc
      0019D1 03               [12] 1896 	rr	a
      0019D2 C0 E0            [24] 1897 	push	acc
      0019D4 A8 08            [24] 1898 	mov	r0,_bp
      0019D6 08               [12] 1899 	inc	r0
      0019D7 E6               [12] 1900 	mov	a,@r0
      0019D8 C0 E0            [24] 1901 	push	acc
      0019DA 08               [12] 1902 	inc	r0
      0019DB E6               [12] 1903 	mov	a,@r0
      0019DC C0 E0            [24] 1904 	push	acc
      0019DE 08               [12] 1905 	inc	r0
      0019DF E6               [12] 1906 	mov	a,@r0
      0019E0 C0 E0            [24] 1907 	push	acc
      0019E2 8A 82            [24] 1908 	mov	dpl,r2
      0019E4 8D 83            [24] 1909 	mov	dph,r5
      0019E6 8E F0            [24] 1910 	mov	b,r6
      0019E8 12 10 87         [24] 1911 	lcall	_simpleserial_get
      0019EB AD 82            [24] 1912 	mov	r5,dpl
      0019ED AE 83            [24] 1913 	mov	r6,dph
      0019EF E5 81            [12] 1914 	mov	a,sp
      0019F1 24 FB            [12] 1915 	add	a,#0xfb
      0019F3 F5 81            [12] 1916 	mov	sp,a
      0019F5 D0 03            [24] 1917 	pop	ar3
      0019F7 D0 07            [24] 1918 	pop	ar7
      0019F9 ED               [12] 1919 	mov	a,r5
      0019FA 4E               [12] 1920 	orl	a,r6
      0019FB 60 B2            [24] 1921 	jz	00104$
                                   1922 ;	tea.c:81: trigger_high();
      0019FD 43 90 01         [24] 1923 	orl	_P1,#0x01
                                   1924 ;	tea.c:82: tea_encrypt(input, key);
      001A00 A8 08            [24] 1925 	mov	r0,_bp
      001A02 08               [12] 1926 	inc	r0
      001A03 A6 07            [24] 1927 	mov	@r0,ar7
      001A05 08               [12] 1928 	inc	r0
      001A06 76 00            [12] 1929 	mov	@r0,#0x00
      001A08 08               [12] 1930 	inc	r0
      001A09 76 40            [12] 1931 	mov	@r0,#0x40
      001A0B 8B 04            [24] 1932 	mov	ar4,r3
      001A0D 8C 02            [24] 1933 	mov	ar2,r4
      001A0F 7C 00            [12] 1934 	mov	r4,#0x00
      001A11 7F 40            [12] 1935 	mov	r7,#0x40
      001A13 C0 03            [24] 1936 	push	ar3
      001A15 A8 08            [24] 1937 	mov	r0,_bp
      001A17 08               [12] 1938 	inc	r0
      001A18 E6               [12] 1939 	mov	a,@r0
      001A19 C0 E0            [24] 1940 	push	acc
      001A1B 08               [12] 1941 	inc	r0
      001A1C E6               [12] 1942 	mov	a,@r0
      001A1D C0 E0            [24] 1943 	push	acc
      001A1F 08               [12] 1944 	inc	r0
      001A20 E6               [12] 1945 	mov	a,@r0
      001A21 C0 E0            [24] 1946 	push	acc
      001A23 8A 82            [24] 1947 	mov	dpl,r2
      001A25 8C 83            [24] 1948 	mov	dph,r4
      001A27 8F F0            [24] 1949 	mov	b,r7
      001A29 12 12 1F         [24] 1950 	lcall	_tea_encrypt
      001A2C 15 81            [12] 1951 	dec	sp
      001A2E 15 81            [12] 1952 	dec	sp
      001A30 15 81            [12] 1953 	dec	sp
      001A32 D0 03            [24] 1954 	pop	ar3
                                   1955 ;	tea.c:83: trigger_low();
      001A34 53 90 FE         [24] 1956 	anl	_P1,#0xfe
                                   1957 ;	tea.c:84: simpleserial_put(input, TEA_BLOCK_SIZE);
      001A37 7F 00            [12] 1958 	mov	r7,#0x00
      001A39 7E 40            [12] 1959 	mov	r6,#0x40
      001A3B 74 08            [12] 1960 	mov	a,#0x08
      001A3D C0 E0            [24] 1961 	push	acc
      001A3F 8B 82            [24] 1962 	mov	dpl,r3
      001A41 8F 83            [24] 1963 	mov	dph,r7
      001A43 8E F0            [24] 1964 	mov	b,r6
      001A45 12 11 A8         [24] 1965 	lcall	_simpleserial_put
      001A48 15 81            [12] 1966 	dec	sp
      001A4A 02 19 AF         [24] 1967 	ljmp	00104$
      001A4D 85 08 81         [24] 1968 	mov	sp,_bp
      001A50 D0 08            [24] 1969 	pop	_bp
      001A52 22               [24] 1970 	ret
                                   1971 	.area CSEG    (CODE)
                                   1972 	.area CONST   (CODE)
                                   1973 	.area XINIT   (CODE)
                                   1974 	.area CABS    (ABS,CODE)
