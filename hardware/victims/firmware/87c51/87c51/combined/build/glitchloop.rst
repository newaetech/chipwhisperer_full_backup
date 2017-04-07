                                      1 ;--------------------------------------------------------
                                      2 ; File Created by SDCC : free open source ANSI-C Compiler
                                      3 ; Version 3.6.0 #9615 (MINGW32)
                                      4 ;--------------------------------------------------------
                                      5 	.module glitchloop
                                      6 	.optsdcc -mmcs51 --model-small
                                      7 	
                                      8 ;--------------------------------------------------------
                                      9 ; Public variables in this module
                                     10 ;--------------------------------------------------------
                                     11 	.globl _printf_tiny
                                     12 	.globl _TF2
                                     13 	.globl _EXF2
                                     14 	.globl _RCLK
                                     15 	.globl _TCLK
                                     16 	.globl _EXEN2
                                     17 	.globl _TR2
                                     18 	.globl _C_T2
                                     19 	.globl _CP_RL2
                                     20 	.globl _T2CON_7
                                     21 	.globl _T2CON_6
                                     22 	.globl _T2CON_5
                                     23 	.globl _T2CON_4
                                     24 	.globl _T2CON_3
                                     25 	.globl _T2CON_2
                                     26 	.globl _T2CON_1
                                     27 	.globl _T2CON_0
                                     28 	.globl _PT2
                                     29 	.globl _ET2
                                     30 	.globl _CY
                                     31 	.globl _AC
                                     32 	.globl _F0
                                     33 	.globl _RS1
                                     34 	.globl _RS0
                                     35 	.globl _OV
                                     36 	.globl _F1
                                     37 	.globl _P
                                     38 	.globl _PS
                                     39 	.globl _PT1
                                     40 	.globl _PX1
                                     41 	.globl _PT0
                                     42 	.globl _PX0
                                     43 	.globl _RD
                                     44 	.globl _WR
                                     45 	.globl _T1
                                     46 	.globl _T0
                                     47 	.globl _INT1
                                     48 	.globl _INT0
                                     49 	.globl _TXD
                                     50 	.globl _RXD
                                     51 	.globl _P3_7
                                     52 	.globl _P3_6
                                     53 	.globl _P3_5
                                     54 	.globl _P3_4
                                     55 	.globl _P3_3
                                     56 	.globl _P3_2
                                     57 	.globl _P3_1
                                     58 	.globl _P3_0
                                     59 	.globl _EA
                                     60 	.globl _ES
                                     61 	.globl _ET1
                                     62 	.globl _EX1
                                     63 	.globl _ET0
                                     64 	.globl _EX0
                                     65 	.globl _P2_7
                                     66 	.globl _P2_6
                                     67 	.globl _P2_5
                                     68 	.globl _P2_4
                                     69 	.globl _P2_3
                                     70 	.globl _P2_2
                                     71 	.globl _P2_1
                                     72 	.globl _P2_0
                                     73 	.globl _SM0
                                     74 	.globl _SM1
                                     75 	.globl _SM2
                                     76 	.globl _REN
                                     77 	.globl _TB8
                                     78 	.globl _RB8
                                     79 	.globl _TI
                                     80 	.globl _RI
                                     81 	.globl _P1_7
                                     82 	.globl _P1_6
                                     83 	.globl _P1_5
                                     84 	.globl _P1_4
                                     85 	.globl _P1_3
                                     86 	.globl _P1_2
                                     87 	.globl _P1_1
                                     88 	.globl _P1_0
                                     89 	.globl _TF1
                                     90 	.globl _TR1
                                     91 	.globl _TF0
                                     92 	.globl _TR0
                                     93 	.globl _IE1
                                     94 	.globl _IT1
                                     95 	.globl _IE0
                                     96 	.globl _IT0
                                     97 	.globl _P0_7
                                     98 	.globl _P0_6
                                     99 	.globl _P0_5
                                    100 	.globl _P0_4
                                    101 	.globl _P0_3
                                    102 	.globl _P0_2
                                    103 	.globl _P0_1
                                    104 	.globl _P0_0
                                    105 	.globl _TH2
                                    106 	.globl _TL2
                                    107 	.globl _RCAP2H
                                    108 	.globl _RCAP2L
                                    109 	.globl _T2CON
                                    110 	.globl _B
                                    111 	.globl _ACC
                                    112 	.globl _PSW
                                    113 	.globl _IP
                                    114 	.globl _P3
                                    115 	.globl _IE
                                    116 	.globl _P2
                                    117 	.globl _SBUF
                                    118 	.globl _SCON
                                    119 	.globl _P1
                                    120 	.globl _TH1
                                    121 	.globl _TH0
                                    122 	.globl _TL1
                                    123 	.globl _TL0
                                    124 	.globl _TMOD
                                    125 	.globl _TCON
                                    126 	.globl _PCON
                                    127 	.globl _DPH
                                    128 	.globl _DPL
                                    129 	.globl _SP
                                    130 	.globl _P0
                                    131 	.globl _main_glitchloop
                                    132 ;--------------------------------------------------------
                                    133 ; special function registers
                                    134 ;--------------------------------------------------------
                                    135 	.area RSEG    (ABS,DATA)
      000000                        136 	.org 0x0000
                           000080   137 _P0	=	0x0080
                           000081   138 _SP	=	0x0081
                           000082   139 _DPL	=	0x0082
                           000083   140 _DPH	=	0x0083
                           000087   141 _PCON	=	0x0087
                           000088   142 _TCON	=	0x0088
                           000089   143 _TMOD	=	0x0089
                           00008A   144 _TL0	=	0x008a
                           00008B   145 _TL1	=	0x008b
                           00008C   146 _TH0	=	0x008c
                           00008D   147 _TH1	=	0x008d
                           000090   148 _P1	=	0x0090
                           000098   149 _SCON	=	0x0098
                           000099   150 _SBUF	=	0x0099
                           0000A0   151 _P2	=	0x00a0
                           0000A8   152 _IE	=	0x00a8
                           0000B0   153 _P3	=	0x00b0
                           0000B8   154 _IP	=	0x00b8
                           0000D0   155 _PSW	=	0x00d0
                           0000E0   156 _ACC	=	0x00e0
                           0000F0   157 _B	=	0x00f0
                           0000C8   158 _T2CON	=	0x00c8
                           0000CA   159 _RCAP2L	=	0x00ca
                           0000CB   160 _RCAP2H	=	0x00cb
                           0000CC   161 _TL2	=	0x00cc
                           0000CD   162 _TH2	=	0x00cd
                                    163 ;--------------------------------------------------------
                                    164 ; special function bits
                                    165 ;--------------------------------------------------------
                                    166 	.area RSEG    (ABS,DATA)
      000000                        167 	.org 0x0000
                           000080   168 _P0_0	=	0x0080
                           000081   169 _P0_1	=	0x0081
                           000082   170 _P0_2	=	0x0082
                           000083   171 _P0_3	=	0x0083
                           000084   172 _P0_4	=	0x0084
                           000085   173 _P0_5	=	0x0085
                           000086   174 _P0_6	=	0x0086
                           000087   175 _P0_7	=	0x0087
                           000088   176 _IT0	=	0x0088
                           000089   177 _IE0	=	0x0089
                           00008A   178 _IT1	=	0x008a
                           00008B   179 _IE1	=	0x008b
                           00008C   180 _TR0	=	0x008c
                           00008D   181 _TF0	=	0x008d
                           00008E   182 _TR1	=	0x008e
                           00008F   183 _TF1	=	0x008f
                           000090   184 _P1_0	=	0x0090
                           000091   185 _P1_1	=	0x0091
                           000092   186 _P1_2	=	0x0092
                           000093   187 _P1_3	=	0x0093
                           000094   188 _P1_4	=	0x0094
                           000095   189 _P1_5	=	0x0095
                           000096   190 _P1_6	=	0x0096
                           000097   191 _P1_7	=	0x0097
                           000098   192 _RI	=	0x0098
                           000099   193 _TI	=	0x0099
                           00009A   194 _RB8	=	0x009a
                           00009B   195 _TB8	=	0x009b
                           00009C   196 _REN	=	0x009c
                           00009D   197 _SM2	=	0x009d
                           00009E   198 _SM1	=	0x009e
                           00009F   199 _SM0	=	0x009f
                           0000A0   200 _P2_0	=	0x00a0
                           0000A1   201 _P2_1	=	0x00a1
                           0000A2   202 _P2_2	=	0x00a2
                           0000A3   203 _P2_3	=	0x00a3
                           0000A4   204 _P2_4	=	0x00a4
                           0000A5   205 _P2_5	=	0x00a5
                           0000A6   206 _P2_6	=	0x00a6
                           0000A7   207 _P2_7	=	0x00a7
                           0000A8   208 _EX0	=	0x00a8
                           0000A9   209 _ET0	=	0x00a9
                           0000AA   210 _EX1	=	0x00aa
                           0000AB   211 _ET1	=	0x00ab
                           0000AC   212 _ES	=	0x00ac
                           0000AF   213 _EA	=	0x00af
                           0000B0   214 _P3_0	=	0x00b0
                           0000B1   215 _P3_1	=	0x00b1
                           0000B2   216 _P3_2	=	0x00b2
                           0000B3   217 _P3_3	=	0x00b3
                           0000B4   218 _P3_4	=	0x00b4
                           0000B5   219 _P3_5	=	0x00b5
                           0000B6   220 _P3_6	=	0x00b6
                           0000B7   221 _P3_7	=	0x00b7
                           0000B0   222 _RXD	=	0x00b0
                           0000B1   223 _TXD	=	0x00b1
                           0000B2   224 _INT0	=	0x00b2
                           0000B3   225 _INT1	=	0x00b3
                           0000B4   226 _T0	=	0x00b4
                           0000B5   227 _T1	=	0x00b5
                           0000B6   228 _WR	=	0x00b6
                           0000B7   229 _RD	=	0x00b7
                           0000B8   230 _PX0	=	0x00b8
                           0000B9   231 _PT0	=	0x00b9
                           0000BA   232 _PX1	=	0x00ba
                           0000BB   233 _PT1	=	0x00bb
                           0000BC   234 _PS	=	0x00bc
                           0000D0   235 _P	=	0x00d0
                           0000D1   236 _F1	=	0x00d1
                           0000D2   237 _OV	=	0x00d2
                           0000D3   238 _RS0	=	0x00d3
                           0000D4   239 _RS1	=	0x00d4
                           0000D5   240 _F0	=	0x00d5
                           0000D6   241 _AC	=	0x00d6
                           0000D7   242 _CY	=	0x00d7
                           0000AD   243 _ET2	=	0x00ad
                           0000BD   244 _PT2	=	0x00bd
                           0000C8   245 _T2CON_0	=	0x00c8
                           0000C9   246 _T2CON_1	=	0x00c9
                           0000CA   247 _T2CON_2	=	0x00ca
                           0000CB   248 _T2CON_3	=	0x00cb
                           0000CC   249 _T2CON_4	=	0x00cc
                           0000CD   250 _T2CON_5	=	0x00cd
                           0000CE   251 _T2CON_6	=	0x00ce
                           0000CF   252 _T2CON_7	=	0x00cf
                           0000C8   253 _CP_RL2	=	0x00c8
                           0000C9   254 _C_T2	=	0x00c9
                           0000CA   255 _TR2	=	0x00ca
                           0000CB   256 _EXEN2	=	0x00cb
                           0000CC   257 _TCLK	=	0x00cc
                           0000CD   258 _RCLK	=	0x00cd
                           0000CE   259 _EXF2	=	0x00ce
                           0000CF   260 _TF2	=	0x00cf
                                    261 ;--------------------------------------------------------
                                    262 ; overlayable register banks
                                    263 ;--------------------------------------------------------
                                    264 	.area REG_BANK_0	(REL,OVR,DATA)
      000000                        265 	.ds 8
                                    266 ;--------------------------------------------------------
                                    267 ; internal ram data
                                    268 ;--------------------------------------------------------
                                    269 	.area DSEG    (DATA)
                                    270 ;--------------------------------------------------------
                                    271 ; overlayable items in internal ram 
                                    272 ;--------------------------------------------------------
                                    273 ;--------------------------------------------------------
                                    274 ; indirectly addressable internal ram data
                                    275 ;--------------------------------------------------------
                                    276 	.area ISEG    (DATA)
                                    277 ;--------------------------------------------------------
                                    278 ; absolute internal ram data
                                    279 ;--------------------------------------------------------
                                    280 	.area IABS    (ABS,DATA)
                                    281 	.area IABS    (ABS,DATA)
                                    282 ;--------------------------------------------------------
                                    283 ; bit data
                                    284 ;--------------------------------------------------------
                                    285 	.area BSEG    (BIT)
                                    286 ;--------------------------------------------------------
                                    287 ; paged external ram data
                                    288 ;--------------------------------------------------------
                                    289 	.area PSEG    (PAG,XDATA)
                                    290 ;--------------------------------------------------------
                                    291 ; external ram data
                                    292 ;--------------------------------------------------------
                                    293 	.area XSEG    (XDATA)
                                    294 ;--------------------------------------------------------
                                    295 ; absolute external ram data
                                    296 ;--------------------------------------------------------
                                    297 	.area XABS    (ABS,XDATA)
                                    298 ;--------------------------------------------------------
                                    299 ; external initialized ram data
                                    300 ;--------------------------------------------------------
                                    301 	.area XISEG   (XDATA)
                                    302 	.area HOME    (CODE)
                                    303 	.area GSINIT0 (CODE)
                                    304 	.area GSINIT1 (CODE)
                                    305 	.area GSINIT2 (CODE)
                                    306 	.area GSINIT3 (CODE)
                                    307 	.area GSINIT4 (CODE)
                                    308 	.area GSINIT5 (CODE)
                                    309 	.area GSINIT  (CODE)
                                    310 	.area GSFINAL (CODE)
                                    311 	.area CSEG    (CODE)
                                    312 ;--------------------------------------------------------
                                    313 ; global & static initialisations
                                    314 ;--------------------------------------------------------
                                    315 	.area HOME    (CODE)
                                    316 	.area GSINIT  (CODE)
                                    317 	.area GSFINAL (CODE)
                                    318 	.area GSINIT  (CODE)
                                    319 ;--------------------------------------------------------
                                    320 ; Home
                                    321 ;--------------------------------------------------------
                                    322 	.area HOME    (CODE)
                                    323 	.area HOME    (CODE)
                                    324 ;--------------------------------------------------------
                                    325 ; code
                                    326 ;--------------------------------------------------------
                                    327 	.area CSEG    (CODE)
                                    328 ;------------------------------------------------------------
                                    329 ;Allocation info for local variables in function 'main_glitchloop'
                                    330 ;------------------------------------------------------------
                                    331 ;i                         Allocated to registers r3 r4 
                                    332 ;j                         Allocated to stack - _bp +3
                                    333 ;cnt                       Allocated to registers r6 r5 
                                    334 ;inc                       Allocated to registers 
                                    335 ;sloc0                     Allocated to stack - _bp +1
                                    336 ;------------------------------------------------------------
                                    337 ;	glitchloop.c:10: void main_glitchloop()
                                    338 ;	-----------------------------------------
                                    339 ;	 function main_glitchloop
                                    340 ;	-----------------------------------------
      000B72                        341 _main_glitchloop:
                           000007   342 	ar7 = 0x07
                           000006   343 	ar6 = 0x06
                           000005   344 	ar5 = 0x05
                           000004   345 	ar4 = 0x04
                           000003   346 	ar3 = 0x03
                           000002   347 	ar2 = 0x02
                           000001   348 	ar1 = 0x01
                           000000   349 	ar0 = 0x00
      000B72 C0 08            [24]  350 	push	_bp
      000B74 E5 81            [12]  351 	mov	a,sp
      000B76 F5 08            [12]  352 	mov	_bp,a
      000B78 24 04            [12]  353 	add	a,#0x04
      000B7A F5 81            [12]  354 	mov	sp,a
                                    355 ;	glitchloop.c:17: while (1) 
      000B7C 7F 00            [12]  356 	mov	r7,#0x00
      000B7E                        357 00104$:
                                    358 ;	glitchloop.c:19: cnt = 0;
      000B7E 7E 00            [12]  359 	mov	r6,#0x00
      000B80 7D 00            [12]  360 	mov	r5,#0x00
                                    361 ;	glitchloop.c:20: trigger_high();     		    
      000B82 43 90 01         [24]  362 	orl	_P1,#0x01
                                    363 ;	glitchloop.c:21: for(i = 0; i < 200; i++)
      000B85 7B 00            [12]  364 	mov	r3,#0x00
      000B87 7C 00            [12]  365 	mov	r4,#0x00
      000B89                        366 00109$:
                                    367 ;	glitchloop.c:23: for(j = 0; j < 200; j++)
      000B89 E5 08            [12]  368 	mov	a,_bp
      000B8B 24 03            [12]  369 	add	a,#0x03
      000B8D F8               [12]  370 	mov	r0,a
      000B8E 76 C8            [12]  371 	mov	@r0,#0xc8
      000B90 08               [12]  372 	inc	r0
      000B91 76 00            [12]  373 	mov	@r0,#0x00
      000B93 A8 08            [24]  374 	mov	r0,_bp
      000B95 08               [12]  375 	inc	r0
      000B96 A6 06            [24]  376 	mov	@r0,ar6
      000B98 08               [12]  377 	inc	r0
      000B99 A6 05            [24]  378 	mov	@r0,ar5
      000B9B                        379 00108$:
                                    380 ;	glitchloop.c:25: cnt++;
      000B9B A8 08            [24]  381 	mov	r0,_bp
      000B9D 08               [12]  382 	inc	r0
      000B9E 06               [12]  383 	inc	@r0
      000B9F B6 00 02         [24]  384 	cjne	@r0,#0x00,00130$
      000BA2 08               [12]  385 	inc	r0
      000BA3 06               [12]  386 	inc	@r0
      000BA4                        387 00130$:
      000BA4 E5 08            [12]  388 	mov	a,_bp
      000BA6 24 03            [12]  389 	add	a,#0x03
      000BA8 F8               [12]  390 	mov	r0,a
      000BA9 E6               [12]  391 	mov	a,@r0
      000BAA 24 FF            [12]  392 	add	a,#0xff
      000BAC FD               [12]  393 	mov	r5,a
      000BAD 08               [12]  394 	inc	r0
      000BAE E6               [12]  395 	mov	a,@r0
      000BAF 34 FF            [12]  396 	addc	a,#0xff
      000BB1 FE               [12]  397 	mov	r6,a
      000BB2 E5 08            [12]  398 	mov	a,_bp
      000BB4 24 03            [12]  399 	add	a,#0x03
      000BB6 F8               [12]  400 	mov	r0,a
      000BB7 A6 05            [24]  401 	mov	@r0,ar5
      000BB9 08               [12]  402 	inc	r0
      000BBA A6 06            [24]  403 	mov	@r0,ar6
                                    404 ;	glitchloop.c:23: for(j = 0; j < 200; j++)
      000BBC ED               [12]  405 	mov	a,r5
      000BBD 4E               [12]  406 	orl	a,r6
      000BBE 70 DB            [24]  407 	jnz	00108$
                                    408 ;	glitchloop.c:21: for(i = 0; i < 200; i++)
      000BC0 A8 08            [24]  409 	mov	r0,_bp
      000BC2 08               [12]  410 	inc	r0
      000BC3 86 06            [24]  411 	mov	ar6,@r0
      000BC5 08               [12]  412 	inc	r0
      000BC6 86 05            [24]  413 	mov	ar5,@r0
      000BC8 0B               [12]  414 	inc	r3
      000BC9 BB 00 01         [24]  415 	cjne	r3,#0x00,00132$
      000BCC 0C               [12]  416 	inc	r4
      000BCD                        417 00132$:
      000BCD C3               [12]  418 	clr	c
      000BCE EB               [12]  419 	mov	a,r3
      000BCF 94 C8            [12]  420 	subb	a,#0xc8
      000BD1 EC               [12]  421 	mov	a,r4
      000BD2 94 00            [12]  422 	subb	a,#0x00
      000BD4 40 B3            [24]  423 	jc	00109$
                                    424 ;	glitchloop.c:28: trigger_low();
      000BD6 53 90 FE         [24]  425 	anl	_P1,#0xfe
                                    426 ;	glitchloop.c:29: inc++;
      000BD9 0F               [12]  427 	inc	r7
                                    428 ;	glitchloop.c:32: printf_tiny("%u: %u %u %u\n", inc, i, j, cnt);
      000BDA 8F 05            [24]  429 	mov	ar5,r7
      000BDC 7E 00            [12]  430 	mov	r6,#0x00
      000BDE C0 07            [24]  431 	push	ar7
      000BE0 A8 08            [24]  432 	mov	r0,_bp
      000BE2 08               [12]  433 	inc	r0
      000BE3 E6               [12]  434 	mov	a,@r0
      000BE4 C0 E0            [24]  435 	push	acc
      000BE6 08               [12]  436 	inc	r0
      000BE7 E6               [12]  437 	mov	a,@r0
      000BE8 C0 E0            [24]  438 	push	acc
      000BEA 74 C8            [12]  439 	mov	a,#0xc8
      000BEC C0 E0            [24]  440 	push	acc
      000BEE E4               [12]  441 	clr	a
      000BEF C0 E0            [24]  442 	push	acc
      000BF1 C0 03            [24]  443 	push	ar3
      000BF3 C0 04            [24]  444 	push	ar4
      000BF5 C0 05            [24]  445 	push	ar5
      000BF7 C0 06            [24]  446 	push	ar6
      000BF9 74 CC            [12]  447 	mov	a,#___str_0
      000BFB C0 E0            [24]  448 	push	acc
      000BFD 74 1D            [12]  449 	mov	a,#(___str_0 >> 8)
      000BFF C0 E0            [24]  450 	push	acc
      000C01 12 1B 98         [24]  451 	lcall	_printf_tiny
      000C04 E5 81            [12]  452 	mov	a,sp
      000C06 24 F6            [12]  453 	add	a,#0xf6
      000C08 F5 81            [12]  454 	mov	sp,a
      000C0A D0 07            [24]  455 	pop	ar7
      000C0C 02 0B 7E         [24]  456 	ljmp	00104$
      000C0F 85 08 81         [24]  457 	mov	sp,_bp
      000C12 D0 08            [24]  458 	pop	_bp
      000C14 22               [24]  459 	ret
                                    460 	.area CSEG    (CODE)
                                    461 	.area CONST   (CODE)
      001DCC                        462 ___str_0:
      001DCC 25 75 3A 20 25 75 20   463 	.ascii "%u: %u %u %u"
             25 75 20 25 75
      001DD8 0A                     464 	.db 0x0a
      001DD9 00                     465 	.db 0x00
                                    466 	.area XINIT   (CODE)
                                    467 	.area CABS    (ABS,CODE)
