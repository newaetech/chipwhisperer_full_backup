                                      1 ;--------------------------------------------------------
                                      2 ; File Created by SDCC : free open source ANSI-C Compiler
                                      3 ; Version 3.6.0 #9615 (MINGW32)
                                      4 ;--------------------------------------------------------
                                      5 	.module aes
                                      6 	.optsdcc -mmcs51 --model-small
                                      7 	
                                      8 ;--------------------------------------------------------
                                      9 ; Public variables in this module
                                     10 ;--------------------------------------------------------
                                     11 	.globl _aes_encrypt
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
                                    133 	.globl _main_aes
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
                                    331 ;Allocation info for local variables in function 'main_aes'
                                    332 ;------------------------------------------------------------
                                    333 ;input                     Allocated to stack - _bp +4
                                    334 ;key                       Allocated to stack - _bp +20
                                    335 ;sloc0                     Allocated to stack - _bp +1
                                    336 ;------------------------------------------------------------
                                    337 ;	aes.c:21: void main_aes(void) 
                                    338 ;	-----------------------------------------
                                    339 ;	 function main_aes
                                    340 ;	-----------------------------------------
      0000F8                        341 _main_aes:
                           000007   342 	ar7 = 0x07
                           000006   343 	ar6 = 0x06
                           000005   344 	ar5 = 0x05
                           000004   345 	ar4 = 0x04
                           000003   346 	ar3 = 0x03
                           000002   347 	ar2 = 0x02
                           000001   348 	ar1 = 0x01
                           000000   349 	ar0 = 0x00
      0000F8 C0 08            [24]  350 	push	_bp
      0000FA E5 81            [12]  351 	mov	a,sp
      0000FC F5 08            [12]  352 	mov	_bp,a
      0000FE 24 23            [12]  353 	add	a,#0x23
      000100 F5 81            [12]  354 	mov	sp,a
                                    355 ;	aes.c:28: while(1)
      000102                        356 00104$:
                                    357 ;	aes.c:31: if(simpleserial_get(input, key, AES_BLOCK_SIZE, AES_KEY_SIZE))
      000102 E5 08            [12]  358 	mov	a,_bp
      000104 24 14            [12]  359 	add	a,#0x14
      000106 FF               [12]  360 	mov	r7,a
      000107 A8 08            [24]  361 	mov	r0,_bp
      000109 08               [12]  362 	inc	r0
      00010A A6 07            [24]  363 	mov	@r0,ar7
      00010C 08               [12]  364 	inc	r0
      00010D 76 00            [12]  365 	mov	@r0,#0x00
      00010F 08               [12]  366 	inc	r0
      000110 76 40            [12]  367 	mov	@r0,#0x40
      000112 E5 08            [12]  368 	mov	a,_bp
      000114 24 04            [12]  369 	add	a,#0x04
      000116 FB               [12]  370 	mov	r3,a
      000117 FA               [12]  371 	mov	r2,a
      000118 7D 00            [12]  372 	mov	r5,#0x00
      00011A 7E 40            [12]  373 	mov	r6,#0x40
      00011C C0 07            [24]  374 	push	ar7
      00011E C0 03            [24]  375 	push	ar3
      000120 74 10            [12]  376 	mov	a,#0x10
      000122 C0 E0            [24]  377 	push	acc
      000124 C0 E0            [24]  378 	push	acc
      000126 A8 08            [24]  379 	mov	r0,_bp
      000128 08               [12]  380 	inc	r0
      000129 E6               [12]  381 	mov	a,@r0
      00012A C0 E0            [24]  382 	push	acc
      00012C 08               [12]  383 	inc	r0
      00012D E6               [12]  384 	mov	a,@r0
      00012E C0 E0            [24]  385 	push	acc
      000130 08               [12]  386 	inc	r0
      000131 E6               [12]  387 	mov	a,@r0
      000132 C0 E0            [24]  388 	push	acc
      000134 8A 82            [24]  389 	mov	dpl,r2
      000136 8D 83            [24]  390 	mov	dph,r5
      000138 8E F0            [24]  391 	mov	b,r6
      00013A 12 10 87         [24]  392 	lcall	_simpleserial_get
      00013D AD 82            [24]  393 	mov	r5,dpl
      00013F AE 83            [24]  394 	mov	r6,dph
      000141 E5 81            [12]  395 	mov	a,sp
      000143 24 FB            [12]  396 	add	a,#0xfb
      000145 F5 81            [12]  397 	mov	sp,a
      000147 D0 03            [24]  398 	pop	ar3
      000149 D0 07            [24]  399 	pop	ar7
      00014B ED               [12]  400 	mov	a,r5
      00014C 4E               [12]  401 	orl	a,r6
      00014D 60 B3            [24]  402 	jz	00104$
                                    403 ;	aes.c:34: trigger_high();
      00014F 43 90 01         [24]  404 	orl	_P1,#0x01
                                    405 ;	aes.c:35: aes_encrypt(input, key);
      000152 A8 08            [24]  406 	mov	r0,_bp
      000154 08               [12]  407 	inc	r0
      000155 A6 07            [24]  408 	mov	@r0,ar7
      000157 08               [12]  409 	inc	r0
      000158 76 00            [12]  410 	mov	@r0,#0x00
      00015A 08               [12]  411 	inc	r0
      00015B 76 40            [12]  412 	mov	@r0,#0x40
      00015D 8B 04            [24]  413 	mov	ar4,r3
      00015F 8C 02            [24]  414 	mov	ar2,r4
      000161 7C 00            [12]  415 	mov	r4,#0x00
      000163 7F 40            [12]  416 	mov	r7,#0x40
      000165 C0 03            [24]  417 	push	ar3
      000167 A8 08            [24]  418 	mov	r0,_bp
      000169 08               [12]  419 	inc	r0
      00016A E6               [12]  420 	mov	a,@r0
      00016B C0 E0            [24]  421 	push	acc
      00016D 08               [12]  422 	inc	r0
      00016E E6               [12]  423 	mov	a,@r0
      00016F C0 E0            [24]  424 	push	acc
      000171 08               [12]  425 	inc	r0
      000172 E6               [12]  426 	mov	a,@r0
      000173 C0 E0            [24]  427 	push	acc
      000175 8A 82            [24]  428 	mov	dpl,r2
      000177 8C 83            [24]  429 	mov	dph,r4
      000179 8F F0            [24]  430 	mov	b,r7
      00017B 12 0A 1A         [24]  431 	lcall	_aes_encrypt
      00017E 15 81            [12]  432 	dec	sp
      000180 15 81            [12]  433 	dec	sp
      000182 15 81            [12]  434 	dec	sp
      000184 D0 03            [24]  435 	pop	ar3
                                    436 ;	aes.c:36: trigger_low();
      000186 53 90 FE         [24]  437 	anl	_P1,#0xfe
                                    438 ;	aes.c:37: simpleserial_put(input, AES_BLOCK_SIZE);
      000189 7F 00            [12]  439 	mov	r7,#0x00
      00018B 7E 40            [12]  440 	mov	r6,#0x40
      00018D 74 10            [12]  441 	mov	a,#0x10
      00018F C0 E0            [24]  442 	push	acc
      000191 8B 82            [24]  443 	mov	dpl,r3
      000193 8F 83            [24]  444 	mov	dph,r7
      000195 8E F0            [24]  445 	mov	b,r6
      000197 12 11 A8         [24]  446 	lcall	_simpleserial_put
      00019A 15 81            [12]  447 	dec	sp
      00019C 02 01 02         [24]  448 	ljmp	00104$
      00019F 85 08 81         [24]  449 	mov	sp,_bp
      0001A2 D0 08            [24]  450 	pop	_bp
      0001A4 22               [24]  451 	ret
                                    452 	.area CSEG    (CODE)
                                    453 	.area CONST   (CODE)
                                    454 	.area XINIT   (CODE)
                                    455 	.area CABS    (ABS,CODE)
