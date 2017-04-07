                                      1 ;--------------------------------------------------------
                                      2 ; File Created by SDCC : free open source ANSI-C Compiler
                                      3 ; Version 3.6.0 #9615 (MINGW32)
                                      4 ;--------------------------------------------------------
                                      5 	.module xor
                                      6 	.optsdcc -mmcs51 --model-small
                                      7 	
                                      8 ;--------------------------------------------------------
                                      9 ; Public variables in this module
                                     10 ;--------------------------------------------------------
                                     11 	.globl _xor_encrypt
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
                                    133 	.globl _main_xor
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
                                    331 ;Allocation info for local variables in function 'xor_encrypt'
                                    332 ;------------------------------------------------------------
                                    333 ;key                       Allocated to stack - _bp -5
                                    334 ;state                     Allocated to stack - _bp +1
                                    335 ;i                         Allocated to stack - _bp +5
                                    336 ;sloc0                     Allocated to stack - _bp +4
                                    337 ;------------------------------------------------------------
                                    338 ;	xor.c:19: void xor_encrypt(uint8_t* state, uint8_t* key)
                                    339 ;	-----------------------------------------
                                    340 ;	 function xor_encrypt
                                    341 ;	-----------------------------------------
      001A53                        342 _xor_encrypt:
                           000007   343 	ar7 = 0x07
                           000006   344 	ar6 = 0x06
                           000005   345 	ar5 = 0x05
                           000004   346 	ar4 = 0x04
                           000003   347 	ar3 = 0x03
                           000002   348 	ar2 = 0x02
                           000001   349 	ar1 = 0x01
                           000000   350 	ar0 = 0x00
      001A53 C0 08            [24]  351 	push	_bp
      001A55 85 81 08         [24]  352 	mov	_bp,sp
      001A58 C0 82            [24]  353 	push	dpl
      001A5A C0 83            [24]  354 	push	dph
      001A5C C0 F0            [24]  355 	push	b
      001A5E 05 81            [12]  356 	inc	sp
      001A60 05 81            [12]  357 	inc	sp
                                    358 ;	xor.c:22: for(i = 0; i < XOR_BLOCK_SIZE; i++)
      001A62 E5 08            [12]  359 	mov	a,_bp
      001A64 24 05            [12]  360 	add	a,#0x05
      001A66 F8               [12]  361 	mov	r0,a
      001A67 76 00            [12]  362 	mov	@r0,#0x00
      001A69                        363 00102$:
                                    364 ;	xor.c:24: state[i] ^= key[i];
      001A69 A8 08            [24]  365 	mov	r0,_bp
      001A6B 08               [12]  366 	inc	r0
      001A6C E5 08            [12]  367 	mov	a,_bp
      001A6E 24 05            [12]  368 	add	a,#0x05
      001A70 F9               [12]  369 	mov	r1,a
      001A71 E7               [12]  370 	mov	a,@r1
      001A72 26               [12]  371 	add	a,@r0
      001A73 FA               [12]  372 	mov	r2,a
      001A74 E4               [12]  373 	clr	a
      001A75 08               [12]  374 	inc	r0
      001A76 36               [12]  375 	addc	a,@r0
      001A77 FB               [12]  376 	mov	r3,a
      001A78 08               [12]  377 	inc	r0
      001A79 86 07            [24]  378 	mov	ar7,@r0
      001A7B 8A 82            [24]  379 	mov	dpl,r2
      001A7D 8B 83            [24]  380 	mov	dph,r3
      001A7F 8F F0            [24]  381 	mov	b,r7
      001A81 E5 08            [12]  382 	mov	a,_bp
      001A83 24 04            [12]  383 	add	a,#0x04
      001A85 F8               [12]  384 	mov	r0,a
      001A86 12 1C A1         [24]  385 	lcall	__gptrget
      001A89 F6               [12]  386 	mov	@r0,a
      001A8A E5 08            [12]  387 	mov	a,_bp
      001A8C 24 FB            [12]  388 	add	a,#0xfb
      001A8E F8               [12]  389 	mov	r0,a
      001A8F E5 08            [12]  390 	mov	a,_bp
      001A91 24 05            [12]  391 	add	a,#0x05
      001A93 F9               [12]  392 	mov	r1,a
      001A94 E7               [12]  393 	mov	a,@r1
      001A95 26               [12]  394 	add	a,@r0
      001A96 FC               [12]  395 	mov	r4,a
      001A97 E4               [12]  396 	clr	a
      001A98 08               [12]  397 	inc	r0
      001A99 36               [12]  398 	addc	a,@r0
      001A9A FD               [12]  399 	mov	r5,a
      001A9B 08               [12]  400 	inc	r0
      001A9C 86 06            [24]  401 	mov	ar6,@r0
      001A9E 8C 82            [24]  402 	mov	dpl,r4
      001AA0 8D 83            [24]  403 	mov	dph,r5
      001AA2 8E F0            [24]  404 	mov	b,r6
      001AA4 12 1C A1         [24]  405 	lcall	__gptrget
      001AA7 FC               [12]  406 	mov	r4,a
      001AA8 E5 08            [12]  407 	mov	a,_bp
      001AAA 24 04            [12]  408 	add	a,#0x04
      001AAC F8               [12]  409 	mov	r0,a
      001AAD E6               [12]  410 	mov	a,@r0
      001AAE 62 04            [12]  411 	xrl	ar4,a
      001AB0 8A 82            [24]  412 	mov	dpl,r2
      001AB2 8B 83            [24]  413 	mov	dph,r3
      001AB4 8F F0            [24]  414 	mov	b,r7
      001AB6 EC               [12]  415 	mov	a,r4
      001AB7 12 1B 7D         [24]  416 	lcall	__gptrput
                                    417 ;	xor.c:22: for(i = 0; i < XOR_BLOCK_SIZE; i++)
      001ABA E5 08            [12]  418 	mov	a,_bp
      001ABC 24 05            [12]  419 	add	a,#0x05
      001ABE F8               [12]  420 	mov	r0,a
      001ABF 06               [12]  421 	inc	@r0
      001AC0 E5 08            [12]  422 	mov	a,_bp
      001AC2 24 05            [12]  423 	add	a,#0x05
      001AC4 F8               [12]  424 	mov	r0,a
      001AC5 B6 10 00         [24]  425 	cjne	@r0,#0x10,00110$
      001AC8                        426 00110$:
      001AC8 40 9F            [24]  427 	jc	00102$
      001ACA 85 08 81         [24]  428 	mov	sp,_bp
      001ACD D0 08            [24]  429 	pop	_bp
      001ACF 22               [24]  430 	ret
                                    431 ;------------------------------------------------------------
                                    432 ;Allocation info for local variables in function 'main_xor'
                                    433 ;------------------------------------------------------------
                                    434 ;input                     Allocated to stack - _bp +4
                                    435 ;key                       Allocated to stack - _bp +20
                                    436 ;sloc0                     Allocated to stack - _bp +1
                                    437 ;------------------------------------------------------------
                                    438 ;	xor.c:28: void main_xor(void) 
                                    439 ;	-----------------------------------------
                                    440 ;	 function main_xor
                                    441 ;	-----------------------------------------
      001AD0                        442 _main_xor:
      001AD0 C0 08            [24]  443 	push	_bp
      001AD2 E5 81            [12]  444 	mov	a,sp
      001AD4 F5 08            [12]  445 	mov	_bp,a
      001AD6 24 23            [12]  446 	add	a,#0x23
      001AD8 F5 81            [12]  447 	mov	sp,a
                                    448 ;	xor.c:35: while(1)
      001ADA                        449 00104$:
                                    450 ;	xor.c:38: if(simpleserial_get(input, key, XOR_BLOCK_SIZE, XOR_KEY_SIZE))
      001ADA E5 08            [12]  451 	mov	a,_bp
      001ADC 24 14            [12]  452 	add	a,#0x14
      001ADE FF               [12]  453 	mov	r7,a
      001ADF A8 08            [24]  454 	mov	r0,_bp
      001AE1 08               [12]  455 	inc	r0
      001AE2 A6 07            [24]  456 	mov	@r0,ar7
      001AE4 08               [12]  457 	inc	r0
      001AE5 76 00            [12]  458 	mov	@r0,#0x00
      001AE7 08               [12]  459 	inc	r0
      001AE8 76 40            [12]  460 	mov	@r0,#0x40
      001AEA E5 08            [12]  461 	mov	a,_bp
      001AEC 24 04            [12]  462 	add	a,#0x04
      001AEE FB               [12]  463 	mov	r3,a
      001AEF FA               [12]  464 	mov	r2,a
      001AF0 7D 00            [12]  465 	mov	r5,#0x00
      001AF2 7E 40            [12]  466 	mov	r6,#0x40
      001AF4 C0 07            [24]  467 	push	ar7
      001AF6 C0 03            [24]  468 	push	ar3
      001AF8 74 10            [12]  469 	mov	a,#0x10
      001AFA C0 E0            [24]  470 	push	acc
      001AFC C0 E0            [24]  471 	push	acc
      001AFE A8 08            [24]  472 	mov	r0,_bp
      001B00 08               [12]  473 	inc	r0
      001B01 E6               [12]  474 	mov	a,@r0
      001B02 C0 E0            [24]  475 	push	acc
      001B04 08               [12]  476 	inc	r0
      001B05 E6               [12]  477 	mov	a,@r0
      001B06 C0 E0            [24]  478 	push	acc
      001B08 08               [12]  479 	inc	r0
      001B09 E6               [12]  480 	mov	a,@r0
      001B0A C0 E0            [24]  481 	push	acc
      001B0C 8A 82            [24]  482 	mov	dpl,r2
      001B0E 8D 83            [24]  483 	mov	dph,r5
      001B10 8E F0            [24]  484 	mov	b,r6
      001B12 12 10 87         [24]  485 	lcall	_simpleserial_get
      001B15 AD 82            [24]  486 	mov	r5,dpl
      001B17 AE 83            [24]  487 	mov	r6,dph
      001B19 E5 81            [12]  488 	mov	a,sp
      001B1B 24 FB            [12]  489 	add	a,#0xfb
      001B1D F5 81            [12]  490 	mov	sp,a
      001B1F D0 03            [24]  491 	pop	ar3
      001B21 D0 07            [24]  492 	pop	ar7
      001B23 ED               [12]  493 	mov	a,r5
      001B24 4E               [12]  494 	orl	a,r6
      001B25 60 B3            [24]  495 	jz	00104$
                                    496 ;	xor.c:41: trigger_high();
      001B27 43 90 01         [24]  497 	orl	_P1,#0x01
                                    498 ;	xor.c:42: xor_encrypt(input, key);
      001B2A A8 08            [24]  499 	mov	r0,_bp
      001B2C 08               [12]  500 	inc	r0
      001B2D A6 07            [24]  501 	mov	@r0,ar7
      001B2F 08               [12]  502 	inc	r0
      001B30 76 00            [12]  503 	mov	@r0,#0x00
      001B32 08               [12]  504 	inc	r0
      001B33 76 40            [12]  505 	mov	@r0,#0x40
      001B35 8B 04            [24]  506 	mov	ar4,r3
      001B37 8C 02            [24]  507 	mov	ar2,r4
      001B39 7C 00            [12]  508 	mov	r4,#0x00
      001B3B 7F 40            [12]  509 	mov	r7,#0x40
      001B3D C0 03            [24]  510 	push	ar3
      001B3F A8 08            [24]  511 	mov	r0,_bp
      001B41 08               [12]  512 	inc	r0
      001B42 E6               [12]  513 	mov	a,@r0
      001B43 C0 E0            [24]  514 	push	acc
      001B45 08               [12]  515 	inc	r0
      001B46 E6               [12]  516 	mov	a,@r0
      001B47 C0 E0            [24]  517 	push	acc
      001B49 08               [12]  518 	inc	r0
      001B4A E6               [12]  519 	mov	a,@r0
      001B4B C0 E0            [24]  520 	push	acc
      001B4D 8A 82            [24]  521 	mov	dpl,r2
      001B4F 8C 83            [24]  522 	mov	dph,r4
      001B51 8F F0            [24]  523 	mov	b,r7
      001B53 12 1A 53         [24]  524 	lcall	_xor_encrypt
      001B56 15 81            [12]  525 	dec	sp
      001B58 15 81            [12]  526 	dec	sp
      001B5A 15 81            [12]  527 	dec	sp
      001B5C D0 03            [24]  528 	pop	ar3
                                    529 ;	xor.c:43: trigger_low();
      001B5E 53 90 FE         [24]  530 	anl	_P1,#0xfe
                                    531 ;	xor.c:44: simpleserial_put(input, XOR_BLOCK_SIZE);
      001B61 7F 00            [12]  532 	mov	r7,#0x00
      001B63 7E 40            [12]  533 	mov	r6,#0x40
      001B65 74 10            [12]  534 	mov	a,#0x10
      001B67 C0 E0            [24]  535 	push	acc
      001B69 8B 82            [24]  536 	mov	dpl,r3
      001B6B 8F 83            [24]  537 	mov	dph,r7
      001B6D 8E F0            [24]  538 	mov	b,r6
      001B6F 12 11 A8         [24]  539 	lcall	_simpleserial_put
      001B72 15 81            [12]  540 	dec	sp
      001B74 02 1A DA         [24]  541 	ljmp	00104$
      001B77 85 08 81         [24]  542 	mov	sp,_bp
      001B7A D0 08            [24]  543 	pop	_bp
      001B7C 22               [24]  544 	ret
                                    545 	.area CSEG    (CODE)
                                    546 	.area CONST   (CODE)
                                    547 	.area XINIT   (CODE)
                                    548 	.area CABS    (ABS,CODE)
