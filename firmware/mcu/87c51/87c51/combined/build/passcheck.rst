                                      1 ;--------------------------------------------------------
                                      2 ; File Created by SDCC : free open source ANSI-C Compiler
                                      3 ; Version 3.6.0 #9615 (MINGW32)
                                      4 ;--------------------------------------------------------
                                      5 	.module passcheck
                                      6 	.optsdcc -mmcs51 --model-small
                                      7 	
                                      8 ;--------------------------------------------------------
                                      9 ; Public variables in this module
                                     10 ;--------------------------------------------------------
                                     11 	.globl _get_password
                                     12 	.globl _printf_tiny
                                     13 	.globl _getchar
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
                                    133 	.globl _main_passcheck
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
                                    331 ;Allocation info for local variables in function 'get_password'
                                    332 ;------------------------------------------------------------
                                    333 ;str                       Allocated to registers r5 r6 r7 
                                    334 ;n                         Allocated to registers r3 r4 
                                    335 ;sloc0                     Allocated to stack - _bp +1
                                    336 ;------------------------------------------------------------
                                    337 ;	passcheck.c:13: int get_password(char* str)
                                    338 ;	-----------------------------------------
                                    339 ;	 function get_password
                                    340 ;	-----------------------------------------
      000C15                        341 _get_password:
                           000007   342 	ar7 = 0x07
                           000006   343 	ar6 = 0x06
                           000005   344 	ar5 = 0x05
                           000004   345 	ar4 = 0x04
                           000003   346 	ar3 = 0x03
                           000002   347 	ar2 = 0x02
                           000001   348 	ar1 = 0x01
                           000000   349 	ar0 = 0x00
      000C15 C0 08            [24]  350 	push	_bp
      000C17 85 81 08         [24]  351 	mov	_bp,sp
      000C1A 05 81            [12]  352 	inc	sp
      000C1C 05 81            [12]  353 	inc	sp
      000C1E 05 81            [12]  354 	inc	sp
      000C20 AD 82            [24]  355 	mov	r5,dpl
      000C22 AE 83            [24]  356 	mov	r6,dph
      000C24 AF F0            [24]  357 	mov	r7,b
                                    358 ;	passcheck.c:16: while(1)
      000C26 7B 00            [12]  359 	mov	r3,#0x00
      000C28 7C 00            [12]  360 	mov	r4,#0x00
      000C2A                        361 00104$:
                                    362 ;	passcheck.c:18: str[n] = getchar();
      000C2A A8 08            [24]  363 	mov	r0,_bp
      000C2C 08               [12]  364 	inc	r0
      000C2D EB               [12]  365 	mov	a,r3
      000C2E 2D               [12]  366 	add	a,r5
      000C2F F6               [12]  367 	mov	@r0,a
      000C30 EC               [12]  368 	mov	a,r4
      000C31 3E               [12]  369 	addc	a,r6
      000C32 08               [12]  370 	inc	r0
      000C33 F6               [12]  371 	mov	@r0,a
      000C34 08               [12]  372 	inc	r0
      000C35 A6 07            [24]  373 	mov	@r0,ar7
      000C37 C0 05            [24]  374 	push	ar5
      000C39 C0 06            [24]  375 	push	ar6
      000C3B C0 07            [24]  376 	push	ar7
      000C3D C0 06            [24]  377 	push	ar6
      000C3F C0 05            [24]  378 	push	ar5
      000C41 C0 04            [24]  379 	push	ar4
      000C43 C0 03            [24]  380 	push	ar3
      000C45 12 00 A0         [24]  381 	lcall	_getchar
      000C48 AF 82            [24]  382 	mov	r7,dpl
      000C4A D0 03            [24]  383 	pop	ar3
      000C4C D0 04            [24]  384 	pop	ar4
      000C4E D0 05            [24]  385 	pop	ar5
      000C50 D0 06            [24]  386 	pop	ar6
      000C52 A8 08            [24]  387 	mov	r0,_bp
      000C54 08               [12]  388 	inc	r0
      000C55 86 82            [24]  389 	mov	dpl,@r0
      000C57 08               [12]  390 	inc	r0
      000C58 86 83            [24]  391 	mov	dph,@r0
      000C5A 08               [12]  392 	inc	r0
      000C5B 86 F0            [24]  393 	mov	b,@r0
      000C5D EF               [12]  394 	mov	a,r7
      000C5E 12 1B 7D         [24]  395 	lcall	__gptrput
                                    396 ;	passcheck.c:19: if(str[n] == '\r')
      000C61 BF 0D 02         [24]  397 	cjne	r7,#0x0d,00116$
      000C64 80 08            [24]  398 	sjmp	00117$
      000C66                        399 00116$:
      000C66 D0 07            [24]  400 	pop	ar7
      000C68 D0 06            [24]  401 	pop	ar6
      000C6A D0 05            [24]  402 	pop	ar5
      000C6C 80 17            [24]  403 	sjmp	00102$
      000C6E                        404 00117$:
      000C6E D0 07            [24]  405 	pop	ar7
      000C70 D0 06            [24]  406 	pop	ar6
      000C72 D0 05            [24]  407 	pop	ar5
                                    408 ;	passcheck.c:21: str[n] = '\0';
      000C74 A8 08            [24]  409 	mov	r0,_bp
      000C76 08               [12]  410 	inc	r0
      000C77 86 82            [24]  411 	mov	dpl,@r0
      000C79 08               [12]  412 	inc	r0
      000C7A 86 83            [24]  413 	mov	dph,@r0
      000C7C 08               [12]  414 	inc	r0
      000C7D 86 F0            [24]  415 	mov	b,@r0
      000C7F E4               [12]  416 	clr	a
      000C80 12 1B 7D         [24]  417 	lcall	__gptrput
                                    418 ;	passcheck.c:22: break;
      000C83 80 07            [24]  419 	sjmp	00105$
      000C85                        420 00102$:
                                    421 ;	passcheck.c:24: n++;
      000C85 0B               [12]  422 	inc	r3
      000C86 BB 00 A1         [24]  423 	cjne	r3,#0x00,00104$
      000C89 0C               [12]  424 	inc	r4
      000C8A 80 9E            [24]  425 	sjmp	00104$
      000C8C                        426 00105$:
                                    427 ;	passcheck.c:26: return n;
      000C8C 8B 82            [24]  428 	mov	dpl,r3
      000C8E 8C 83            [24]  429 	mov	dph,r4
      000C90 85 08 81         [24]  430 	mov	sp,_bp
      000C93 D0 08            [24]  431 	pop	_bp
      000C95 22               [24]  432 	ret
                                    433 ;------------------------------------------------------------
                                    434 ;Allocation info for local variables in function 'main_passcheck'
                                    435 ;------------------------------------------------------------
                                    436 ;typed_password            Allocated to stack - _bp +1
                                    437 ;i                         Allocated to registers r3 r4 
                                    438 ;success                   Allocated to registers r5 r6 
                                    439 ;------------------------------------------------------------
                                    440 ;	passcheck.c:29: void main_passcheck()
                                    441 ;	-----------------------------------------
                                    442 ;	 function main_passcheck
                                    443 ;	-----------------------------------------
      000C96                        444 _main_passcheck:
      000C96 C0 08            [24]  445 	push	_bp
      000C98 E5 81            [12]  446 	mov	a,sp
      000C9A F5 08            [12]  447 	mov	_bp,a
      000C9C 24 20            [12]  448 	add	a,#0x20
      000C9E F5 81            [12]  449 	mov	sp,a
                                    450 ;	passcheck.c:37: printf_tiny("Welcome to your 87C51 login, [username]\n");
      000CA0 74 E6            [12]  451 	mov	a,#___str_0
      000CA2 C0 E0            [24]  452 	push	acc
      000CA4 74 1D            [12]  453 	mov	a,#(___str_0 >> 8)
      000CA6 C0 E0            [24]  454 	push	acc
      000CA8 12 1B 98         [24]  455 	lcall	_printf_tiny
      000CAB 15 81            [12]  456 	dec	sp
      000CAD 15 81            [12]  457 	dec	sp
                                    458 ;	passcheck.c:38: printf_tiny("Please enter high-entropy password:\n");
      000CAF 74 0F            [12]  459 	mov	a,#___str_1
      000CB1 C0 E0            [24]  460 	push	acc
      000CB3 74 1E            [12]  461 	mov	a,#(___str_1 >> 8)
      000CB5 C0 E0            [24]  462 	push	acc
      000CB7 12 1B 98         [24]  463 	lcall	_printf_tiny
      000CBA 15 81            [12]  464 	dec	sp
      000CBC 15 81            [12]  465 	dec	sp
                                    466 ;	passcheck.c:41: get_password(typed_password);
      000CBE AF 08            [24]  467 	mov	r7,_bp
      000CC0 0F               [12]  468 	inc	r7
      000CC1 8F 04            [24]  469 	mov	ar4,r7
      000CC3 7D 00            [12]  470 	mov	r5,#0x00
      000CC5 7E 40            [12]  471 	mov	r6,#0x40
      000CC7 8C 82            [24]  472 	mov	dpl,r4
      000CC9 8D 83            [24]  473 	mov	dph,r5
      000CCB 8E F0            [24]  474 	mov	b,r6
      000CCD C0 07            [24]  475 	push	ar7
      000CCF 12 0C 15         [24]  476 	lcall	_get_password
      000CD2 D0 07            [24]  477 	pop	ar7
                                    478 ;	passcheck.c:44: trigger_high();
      000CD4 43 90 01         [24]  479 	orl	_P1,#0x01
                                    480 ;	passcheck.c:45: success = 1;
      000CD7 7D 01            [12]  481 	mov	r5,#0x01
      000CD9 7E 00            [12]  482 	mov	r6,#0x00
                                    483 ;	passcheck.c:46: for(i = 0; i < MAX_PASS_LENGTH; i++)
      000CDB 7B 00            [12]  484 	mov	r3,#0x00
      000CDD 7C 00            [12]  485 	mov	r4,#0x00
      000CDF                        486 00112$:
                                    487 ;	passcheck.c:48: if(stored_password[i] == '\0')
      000CDF EB               [12]  488 	mov	a,r3
      000CE0 24 DA            [12]  489 	add	a,#_stored_password
      000CE2 F5 82            [12]  490 	mov	dpl,a
      000CE4 EC               [12]  491 	mov	a,r4
      000CE5 34 1D            [12]  492 	addc	a,#(_stored_password >> 8)
      000CE7 F5 83            [12]  493 	mov	dph,a
      000CE9 E4               [12]  494 	clr	a
      000CEA 93               [24]  495 	movc	a,@a+dptr
      000CEB FA               [12]  496 	mov	r2,a
      000CEC 60 2C            [24]  497 	jz	00105$
                                    498 ;	passcheck.c:50: if(typed_password[i] != stored_password[i])
      000CEE C0 05            [24]  499 	push	ar5
      000CF0 C0 06            [24]  500 	push	ar6
      000CF2 EB               [12]  501 	mov	a,r3
      000CF3 2F               [12]  502 	add	a,r7
      000CF4 F9               [12]  503 	mov	r1,a
      000CF5 E7               [12]  504 	mov	a,@r1
      000CF6 FE               [12]  505 	mov	r6,a
      000CF7 B5 02 06         [24]  506 	cjne	a,ar2,00132$
      000CFA D0 06            [24]  507 	pop	ar6
      000CFC D0 05            [24]  508 	pop	ar5
      000CFE 80 0A            [24]  509 	sjmp	00113$
      000D00                        510 00132$:
      000D00 D0 06            [24]  511 	pop	ar6
      000D02 D0 05            [24]  512 	pop	ar5
                                    513 ;	passcheck.c:52: success = 0;
      000D04 7D 00            [12]  514 	mov	r5,#0x00
      000D06 7E 00            [12]  515 	mov	r6,#0x00
                                    516 ;	passcheck.c:53: break;
      000D08 80 10            [24]  517 	sjmp	00105$
      000D0A                        518 00113$:
                                    519 ;	passcheck.c:46: for(i = 0; i < MAX_PASS_LENGTH; i++)
      000D0A 0B               [12]  520 	inc	r3
      000D0B BB 00 01         [24]  521 	cjne	r3,#0x00,00133$
      000D0E 0C               [12]  522 	inc	r4
      000D0F                        523 00133$:
      000D0F C3               [12]  524 	clr	c
      000D10 EB               [12]  525 	mov	a,r3
      000D11 94 20            [12]  526 	subb	a,#0x20
      000D13 EC               [12]  527 	mov	a,r4
      000D14 64 80            [12]  528 	xrl	a,#0x80
      000D16 94 80            [12]  529 	subb	a,#0x80
      000D18 40 C5            [24]  530 	jc	00112$
      000D1A                        531 00105$:
                                    532 ;	passcheck.c:56: trigger_low();
      000D1A 53 90 FE         [24]  533 	anl	_P1,#0xfe
                                    534 ;	passcheck.c:59: if(success)
      000D1D ED               [12]  535 	mov	a,r5
      000D1E 4E               [12]  536 	orl	a,r6
      000D1F 60 26            [24]  537 	jz	00107$
                                    538 ;	passcheck.c:61: printf_tiny("Password check OK\n");
      000D21 74 34            [12]  539 	mov	a,#___str_2
      000D23 C0 E0            [24]  540 	push	acc
      000D25 74 1E            [12]  541 	mov	a,#(___str_2 >> 8)
      000D27 C0 E0            [24]  542 	push	acc
      000D29 12 1B 98         [24]  543 	lcall	_printf_tiny
      000D2C 15 81            [12]  544 	dec	sp
      000D2E 15 81            [12]  545 	dec	sp
                                    546 ;	passcheck.c:62: printf_tiny("Last login: [time]\n");
      000D30 74 47            [12]  547 	mov	a,#___str_3
      000D32 C0 E0            [24]  548 	push	acc
      000D34 74 1E            [12]  549 	mov	a,#(___str_3 >> 8)
      000D36 C0 E0            [24]  550 	push	acc
      000D38 12 1B 98         [24]  551 	lcall	_printf_tiny
      000D3B 15 81            [12]  552 	dec	sp
      000D3D 15 81            [12]  553 	dec	sp
                                    554 ;	passcheck.c:63: led1_high();
      000D3F 43 A0 20         [24]  555 	orl	_P2,#0x20
                                    556 ;	passcheck.c:64: led2_low();
      000D42 53 A0 EF         [24]  557 	anl	_P2,#0xef
      000D45 80 24            [24]  558 	sjmp	00110$
      000D47                        559 00107$:
                                    560 ;	passcheck.c:69: printf_tiny("Password check FAILED\n");
      000D47 74 5B            [12]  561 	mov	a,#___str_4
      000D49 C0 E0            [24]  562 	push	acc
      000D4B 74 1E            [12]  563 	mov	a,#(___str_4 >> 8)
      000D4D C0 E0            [24]  564 	push	acc
      000D4F 12 1B 98         [24]  565 	lcall	_printf_tiny
      000D52 15 81            [12]  566 	dec	sp
      000D54 15 81            [12]  567 	dec	sp
                                    568 ;	passcheck.c:70: printf_tiny("Reporting incident to police...\n");
      000D56 74 72            [12]  569 	mov	a,#___str_5
      000D58 C0 E0            [24]  570 	push	acc
      000D5A 74 1E            [12]  571 	mov	a,#(___str_5 >> 8)
      000D5C C0 E0            [24]  572 	push	acc
      000D5E 12 1B 98         [24]  573 	lcall	_printf_tiny
      000D61 15 81            [12]  574 	dec	sp
      000D63 15 81            [12]  575 	dec	sp
                                    576 ;	passcheck.c:71: led1_low();
      000D65 53 A0 DF         [24]  577 	anl	_P2,#0xdf
                                    578 ;	passcheck.c:72: led2_high();
      000D68 43 A0 10         [24]  579 	orl	_P2,#0x10
                                    580 ;	passcheck.c:77: while(1);
      000D6B                        581 00110$:
      000D6B 80 FE            [24]  582 	sjmp	00110$
      000D6D 85 08 81         [24]  583 	mov	sp,_bp
      000D70 D0 08            [24]  584 	pop	_bp
      000D72 22               [24]  585 	ret
                                    586 	.area CSEG    (CODE)
                                    587 	.area CONST   (CODE)
      001DDA                        588 _stored_password:
      001DDA 54 72 30 75 62 34 64   589 	.ascii "Tr0ub4dor&3"
             6F 72 26 33
      001DE5 00                     590 	.db 0x00
      001DE6                        591 ___str_0:
      001DE6 57 65 6C 63 6F 6D 65   592 	.ascii "Welcome to your 87C51 login, [username]"
             20 74 6F 20 79 6F 75
             72 20 38 37 43 35 31
             20 6C 6F 67 69 6E 2C
             20 5B 75 73 65 72 6E
             61 6D 65 5D
      001E0D 0A                     593 	.db 0x0a
      001E0E 00                     594 	.db 0x00
      001E0F                        595 ___str_1:
      001E0F 50 6C 65 61 73 65 20   596 	.ascii "Please enter high-entropy password:"
             65 6E 74 65 72 20 68
             69 67 68 2D 65 6E 74
             72 6F 70 79 20 70 61
             73 73 77 6F 72 64 3A
      001E32 0A                     597 	.db 0x0a
      001E33 00                     598 	.db 0x00
      001E34                        599 ___str_2:
      001E34 50 61 73 73 77 6F 72   600 	.ascii "Password check OK"
             64 20 63 68 65 63 6B
             20 4F 4B
      001E45 0A                     601 	.db 0x0a
      001E46 00                     602 	.db 0x00
      001E47                        603 ___str_3:
      001E47 4C 61 73 74 20 6C 6F   604 	.ascii "Last login: [time]"
             67 69 6E 3A 20 5B 74
             69 6D 65 5D
      001E59 0A                     605 	.db 0x0a
      001E5A 00                     606 	.db 0x00
      001E5B                        607 ___str_4:
      001E5B 50 61 73 73 77 6F 72   608 	.ascii "Password check FAILED"
             64 20 63 68 65 63 6B
             20 46 41 49 4C 45 44
      001E70 0A                     609 	.db 0x0a
      001E71 00                     610 	.db 0x00
      001E72                        611 ___str_5:
      001E72 52 65 70 6F 72 74 69   612 	.ascii "Reporting incident to police..."
             6E 67 20 69 6E 63 69
             64 65 6E 74 20 74 6F
             20 70 6F 6C 69 63 65
             2E 2E 2E
      001E91 0A                     613 	.db 0x0a
      001E92 00                     614 	.db 0x00
                                    615 	.area XINIT   (CODE)
                                    616 	.area CABS    (ABS,CODE)
