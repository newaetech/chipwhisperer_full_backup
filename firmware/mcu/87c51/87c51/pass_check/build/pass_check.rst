                                      1 ;--------------------------------------------------------
                                      2 ; File Created by SDCC : free open source ANSI-C Compiler
                                      3 ; Version 3.6.0 #9615 (MINGW32)
                                      4 ;--------------------------------------------------------
                                      5 	.module pass_check
                                      6 	.optsdcc -mmcs51 --model-small
                                      7 	
                                      8 ;--------------------------------------------------------
                                      9 ; Public variables in this module
                                     10 ;--------------------------------------------------------
                                     11 	.globl _get_password
                                     12 	.globl _getchar
                                     13 	.globl _printf_small
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
                                    133 	.globl _check_password
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
      000021                        272 _get_password_str_1_15:
      000021                        273 	.ds 3
      000024                        274 _check_password_typed_password_1_19:
      000024                        275 	.ds 32
                                    276 ;--------------------------------------------------------
                                    277 ; overlayable items in internal ram 
                                    278 ;--------------------------------------------------------
                                    279 ;--------------------------------------------------------
                                    280 ; indirectly addressable internal ram data
                                    281 ;--------------------------------------------------------
                                    282 	.area ISEG    (DATA)
                                    283 ;--------------------------------------------------------
                                    284 ; absolute internal ram data
                                    285 ;--------------------------------------------------------
                                    286 	.area IABS    (ABS,DATA)
                                    287 	.area IABS    (ABS,DATA)
                                    288 ;--------------------------------------------------------
                                    289 ; bit data
                                    290 ;--------------------------------------------------------
                                    291 	.area BSEG    (BIT)
                                    292 ;--------------------------------------------------------
                                    293 ; paged external ram data
                                    294 ;--------------------------------------------------------
                                    295 	.area PSEG    (PAG,XDATA)
                                    296 ;--------------------------------------------------------
                                    297 ; external ram data
                                    298 ;--------------------------------------------------------
                                    299 	.area XSEG    (XDATA)
                                    300 ;--------------------------------------------------------
                                    301 ; absolute external ram data
                                    302 ;--------------------------------------------------------
                                    303 	.area XABS    (ABS,XDATA)
                                    304 ;--------------------------------------------------------
                                    305 ; external initialized ram data
                                    306 ;--------------------------------------------------------
                                    307 	.area XISEG   (XDATA)
                                    308 	.area HOME    (CODE)
                                    309 	.area GSINIT0 (CODE)
                                    310 	.area GSINIT1 (CODE)
                                    311 	.area GSINIT2 (CODE)
                                    312 	.area GSINIT3 (CODE)
                                    313 	.area GSINIT4 (CODE)
                                    314 	.area GSINIT5 (CODE)
                                    315 	.area GSINIT  (CODE)
                                    316 	.area GSFINAL (CODE)
                                    317 	.area CSEG    (CODE)
                                    318 ;--------------------------------------------------------
                                    319 ; global & static initialisations
                                    320 ;--------------------------------------------------------
                                    321 	.area HOME    (CODE)
                                    322 	.area GSINIT  (CODE)
                                    323 	.area GSFINAL (CODE)
                                    324 	.area GSINIT  (CODE)
                                    325 ;--------------------------------------------------------
                                    326 ; Home
                                    327 ;--------------------------------------------------------
                                    328 	.area HOME    (CODE)
                                    329 	.area HOME    (CODE)
                                    330 ;--------------------------------------------------------
                                    331 ; code
                                    332 ;--------------------------------------------------------
                                    333 	.area CSEG    (CODE)
                                    334 ;------------------------------------------------------------
                                    335 ;Allocation info for local variables in function 'get_password'
                                    336 ;------------------------------------------------------------
                                    337 ;str                       Allocated with name '_get_password_str_1_15'
                                    338 ;n                         Allocated to registers r3 r4 
                                    339 ;------------------------------------------------------------
                                    340 ;	pass_check.c:23: int get_password(char* str)
                                    341 ;	-----------------------------------------
                                    342 ;	 function get_password
                                    343 ;	-----------------------------------------
      0000CC                        344 _get_password:
                           000007   345 	ar7 = 0x07
                           000006   346 	ar6 = 0x06
                           000005   347 	ar5 = 0x05
                           000004   348 	ar4 = 0x04
                           000003   349 	ar3 = 0x03
                           000002   350 	ar2 = 0x02
                           000001   351 	ar1 = 0x01
                           000000   352 	ar0 = 0x00
      0000CC 85 82 21         [24]  353 	mov	_get_password_str_1_15,dpl
      0000CF 85 83 22         [24]  354 	mov	(_get_password_str_1_15 + 1),dph
      0000D2 85 F0 23         [24]  355 	mov	(_get_password_str_1_15 + 2),b
                                    356 ;	pass_check.c:25: int n = 0;
      0000D5 7B 00            [12]  357 	mov	r3,#0x00
      0000D7 7C 00            [12]  358 	mov	r4,#0x00
                                    359 ;	pass_check.c:26: while(1)
      0000D9                        360 00104$:
                                    361 ;	pass_check.c:28: str[n] = getchar();
      0000D9 EB               [12]  362 	mov	a,r3
      0000DA 25 21            [12]  363 	add	a,_get_password_str_1_15
      0000DC F8               [12]  364 	mov	r0,a
      0000DD EC               [12]  365 	mov	a,r4
      0000DE 35 22            [12]  366 	addc	a,(_get_password_str_1_15 + 1)
      0000E0 F9               [12]  367 	mov	r1,a
      0000E1 AA 23            [24]  368 	mov	r2,(_get_password_str_1_15 + 2)
      0000E3 C0 04            [24]  369 	push	ar4
      0000E5 C0 03            [24]  370 	push	ar3
      0000E7 C0 02            [24]  371 	push	ar2
      0000E9 C0 01            [24]  372 	push	ar1
      0000EB C0 00            [24]  373 	push	ar0
      0000ED 12 00 A6         [24]  374 	lcall	_getchar
      0000F0 AF 82            [24]  375 	mov	r7,dpl
      0000F2 D0 00            [24]  376 	pop	ar0
      0000F4 D0 01            [24]  377 	pop	ar1
      0000F6 D0 02            [24]  378 	pop	ar2
      0000F8 D0 03            [24]  379 	pop	ar3
      0000FA D0 04            [24]  380 	pop	ar4
      0000FC 88 82            [24]  381 	mov	dpl,r0
      0000FE 89 83            [24]  382 	mov	dph,r1
      000100 8A F0            [24]  383 	mov	b,r2
      000102 EF               [12]  384 	mov	a,r7
      000103 12 01 FB         [24]  385 	lcall	__gptrput
                                    386 ;	pass_check.c:29: if(str[n] == '\r')
      000106 BF 0D 0C         [24]  387 	cjne	r7,#0x0d,00102$
                                    388 ;	pass_check.c:31: str[n] = '\0';
      000109 88 82            [24]  389 	mov	dpl,r0
      00010B 89 83            [24]  390 	mov	dph,r1
      00010D 8A F0            [24]  391 	mov	b,r2
      00010F E4               [12]  392 	clr	a
      000110 12 01 FB         [24]  393 	lcall	__gptrput
                                    394 ;	pass_check.c:32: break;
      000113 80 07            [24]  395 	sjmp	00105$
      000115                        396 00102$:
                                    397 ;	pass_check.c:34: n++;
      000115 0B               [12]  398 	inc	r3
      000116 BB 00 C0         [24]  399 	cjne	r3,#0x00,00104$
      000119 0C               [12]  400 	inc	r4
      00011A 80 BD            [24]  401 	sjmp	00104$
      00011C                        402 00105$:
                                    403 ;	pass_check.c:36: return n;
      00011C 8B 82            [24]  404 	mov	dpl,r3
      00011E 8C 83            [24]  405 	mov	dph,r4
      000120 22               [24]  406 	ret
                                    407 ;------------------------------------------------------------
                                    408 ;Allocation info for local variables in function 'check_password'
                                    409 ;------------------------------------------------------------
                                    410 ;typed_password            Allocated with name '_check_password_typed_password_1_19'
                                    411 ;i                         Allocated to registers r4 r5 
                                    412 ;success                   Allocated to registers r6 r7 
                                    413 ;------------------------------------------------------------
                                    414 ;	pass_check.c:39: void check_password()
                                    415 ;	-----------------------------------------
                                    416 ;	 function check_password
                                    417 ;	-----------------------------------------
      000121                        418 _check_password:
                                    419 ;	pass_check.c:45: trigger_low();
      000121 53 90 FE         [24]  420 	anl	_P1,#0xfe
                                    421 ;	pass_check.c:48: printf_small("Welcome to your 87C51 login, [username]\n");
      000124 74 75            [12]  422 	mov	a,#___str_0
      000126 C0 E0            [24]  423 	push	acc
      000128 74 06            [12]  424 	mov	a,#(___str_0 >> 8)
      00012A C0 E0            [24]  425 	push	acc
      00012C 74 80            [12]  426 	mov	a,#0x80
      00012E C0 E0            [24]  427 	push	acc
      000130 12 02 16         [24]  428 	lcall	_printf_small
      000133 15 81            [12]  429 	dec	sp
      000135 15 81            [12]  430 	dec	sp
      000137 15 81            [12]  431 	dec	sp
                                    432 ;	pass_check.c:49: printf_small("Please enter high-entropy password:\n");
      000139 74 9E            [12]  433 	mov	a,#___str_1
      00013B C0 E0            [24]  434 	push	acc
      00013D 74 06            [12]  435 	mov	a,#(___str_1 >> 8)
      00013F C0 E0            [24]  436 	push	acc
      000141 74 80            [12]  437 	mov	a,#0x80
      000143 C0 E0            [24]  438 	push	acc
      000145 12 02 16         [24]  439 	lcall	_printf_small
      000148 15 81            [12]  440 	dec	sp
      00014A 15 81            [12]  441 	dec	sp
      00014C 15 81            [12]  442 	dec	sp
                                    443 ;	pass_check.c:52: get_password(typed_password);
      00014E 90 00 24         [24]  444 	mov	dptr,#_check_password_typed_password_1_19
      000151 75 F0 40         [24]  445 	mov	b,#0x40
      000154 12 00 CC         [24]  446 	lcall	_get_password
                                    447 ;	pass_check.c:55: trigger_high();
      000157 43 90 01         [24]  448 	orl	_P1,#0x01
                                    449 ;	pass_check.c:56: success = 1;
      00015A 7E 01            [12]  450 	mov	r6,#0x01
      00015C 7F 00            [12]  451 	mov	r7,#0x00
                                    452 ;	pass_check.c:57: for(i = 0; i < MAX_PASS_LENGTH; i++)
      00015E 7C 00            [12]  453 	mov	r4,#0x00
      000160 7D 00            [12]  454 	mov	r5,#0x00
      000162                        455 00109$:
                                    456 ;	pass_check.c:59: if(stored_password[i] == '\0')
      000162 EC               [12]  457 	mov	a,r4
      000163 24 69            [12]  458 	add	a,#_stored_password
      000165 F5 82            [12]  459 	mov	dpl,a
      000167 ED               [12]  460 	mov	a,r5
      000168 34 06            [12]  461 	addc	a,#(_stored_password >> 8)
      00016A F5 83            [12]  462 	mov	dph,a
      00016C E4               [12]  463 	clr	a
      00016D 93               [24]  464 	movc	a,@a+dptr
      00016E FB               [12]  465 	mov	r3,a
      00016F 60 21            [24]  466 	jz	00105$
                                    467 ;	pass_check.c:61: if(typed_password[i] != stored_password[i])
      000171 EC               [12]  468 	mov	a,r4
      000172 24 24            [12]  469 	add	a,#_check_password_typed_password_1_19
      000174 F9               [12]  470 	mov	r1,a
      000175 E7               [12]  471 	mov	a,@r1
      000176 FA               [12]  472 	mov	r2,a
      000177 B5 03 02         [24]  473 	cjne	a,ar3,00126$
      00017A 80 06            [24]  474 	sjmp	00110$
      00017C                        475 00126$:
                                    476 ;	pass_check.c:63: success = 0;
      00017C 7E 00            [12]  477 	mov	r6,#0x00
      00017E 7F 00            [12]  478 	mov	r7,#0x00
                                    479 ;	pass_check.c:64: break;
      000180 80 10            [24]  480 	sjmp	00105$
      000182                        481 00110$:
                                    482 ;	pass_check.c:57: for(i = 0; i < MAX_PASS_LENGTH; i++)
      000182 0C               [12]  483 	inc	r4
      000183 BC 00 01         [24]  484 	cjne	r4,#0x00,00127$
      000186 0D               [12]  485 	inc	r5
      000187                        486 00127$:
      000187 C3               [12]  487 	clr	c
      000188 EC               [12]  488 	mov	a,r4
      000189 94 20            [12]  489 	subb	a,#0x20
      00018B ED               [12]  490 	mov	a,r5
      00018C 64 80            [12]  491 	xrl	a,#0x80
      00018E 94 80            [12]  492 	subb	a,#0x80
      000190 40 D0            [24]  493 	jc	00109$
      000192                        494 00105$:
                                    495 ;	pass_check.c:67: trigger_low();
      000192 53 90 FE         [24]  496 	anl	_P1,#0xfe
                                    497 ;	pass_check.c:70: if(success)
      000195 EE               [12]  498 	mov	a,r6
      000196 4F               [12]  499 	orl	a,r7
      000197 60 31            [24]  500 	jz	00107$
                                    501 ;	pass_check.c:72: printf_small("Password check OK\n");
      000199 74 C3            [12]  502 	mov	a,#___str_2
      00019B C0 E0            [24]  503 	push	acc
      00019D 74 06            [12]  504 	mov	a,#(___str_2 >> 8)
      00019F C0 E0            [24]  505 	push	acc
      0001A1 74 80            [12]  506 	mov	a,#0x80
      0001A3 C0 E0            [24]  507 	push	acc
      0001A5 12 02 16         [24]  508 	lcall	_printf_small
      0001A8 15 81            [12]  509 	dec	sp
      0001AA 15 81            [12]  510 	dec	sp
      0001AC 15 81            [12]  511 	dec	sp
                                    512 ;	pass_check.c:73: printf_small("Last login: [time]\n");
      0001AE 74 D6            [12]  513 	mov	a,#___str_3
      0001B0 C0 E0            [24]  514 	push	acc
      0001B2 74 06            [12]  515 	mov	a,#(___str_3 >> 8)
      0001B4 C0 E0            [24]  516 	push	acc
      0001B6 74 80            [12]  517 	mov	a,#0x80
      0001B8 C0 E0            [24]  518 	push	acc
      0001BA 12 02 16         [24]  519 	lcall	_printf_small
      0001BD 15 81            [12]  520 	dec	sp
      0001BF 15 81            [12]  521 	dec	sp
      0001C1 15 81            [12]  522 	dec	sp
                                    523 ;	pass_check.c:74: led1_high();
      0001C3 43 A0 20         [24]  524 	orl	_P2,#0x20
                                    525 ;	pass_check.c:75: led2_low();
      0001C6 53 A0 EF         [24]  526 	anl	_P2,#0xef
      0001C9 22               [24]  527 	ret
      0001CA                        528 00107$:
                                    529 ;	pass_check.c:80: printf_small("Password check FAILED\n");
      0001CA 74 EA            [12]  530 	mov	a,#___str_4
      0001CC C0 E0            [24]  531 	push	acc
      0001CE 74 06            [12]  532 	mov	a,#(___str_4 >> 8)
      0001D0 C0 E0            [24]  533 	push	acc
      0001D2 74 80            [12]  534 	mov	a,#0x80
      0001D4 C0 E0            [24]  535 	push	acc
      0001D6 12 02 16         [24]  536 	lcall	_printf_small
      0001D9 15 81            [12]  537 	dec	sp
      0001DB 15 81            [12]  538 	dec	sp
      0001DD 15 81            [12]  539 	dec	sp
                                    540 ;	pass_check.c:81: printf_small("Reporting incident to police...\n");
      0001DF 74 01            [12]  541 	mov	a,#___str_5
      0001E1 C0 E0            [24]  542 	push	acc
      0001E3 74 07            [12]  543 	mov	a,#(___str_5 >> 8)
      0001E5 C0 E0            [24]  544 	push	acc
      0001E7 74 80            [12]  545 	mov	a,#0x80
      0001E9 C0 E0            [24]  546 	push	acc
      0001EB 12 02 16         [24]  547 	lcall	_printf_small
      0001EE 15 81            [12]  548 	dec	sp
      0001F0 15 81            [12]  549 	dec	sp
      0001F2 15 81            [12]  550 	dec	sp
                                    551 ;	pass_check.c:82: led1_low();
      0001F4 53 A0 DF         [24]  552 	anl	_P2,#0xdf
                                    553 ;	pass_check.c:83: led2_high();
      0001F7 43 A0 10         [24]  554 	orl	_P2,#0x10
                                    555 ;	pass_check.c:86: return;
      0001FA 22               [24]  556 	ret
                                    557 	.area CSEG    (CODE)
                                    558 	.area CONST   (CODE)
      000669                        559 _stored_password:
      000669 54 72 30 75 62 34 64   560 	.ascii "Tr0ub4dor&3"
             6F 72 26 33
      000674 00                     561 	.db 0x00
      000675                        562 ___str_0:
      000675 57 65 6C 63 6F 6D 65   563 	.ascii "Welcome to your 87C51 login, [username]"
             20 74 6F 20 79 6F 75
             72 20 38 37 43 35 31
             20 6C 6F 67 69 6E 2C
             20 5B 75 73 65 72 6E
             61 6D 65 5D
      00069C 0A                     564 	.db 0x0a
      00069D 00                     565 	.db 0x00
      00069E                        566 ___str_1:
      00069E 50 6C 65 61 73 65 20   567 	.ascii "Please enter high-entropy password:"
             65 6E 74 65 72 20 68
             69 67 68 2D 65 6E 74
             72 6F 70 79 20 70 61
             73 73 77 6F 72 64 3A
      0006C1 0A                     568 	.db 0x0a
      0006C2 00                     569 	.db 0x00
      0006C3                        570 ___str_2:
      0006C3 50 61 73 73 77 6F 72   571 	.ascii "Password check OK"
             64 20 63 68 65 63 6B
             20 4F 4B
      0006D4 0A                     572 	.db 0x0a
      0006D5 00                     573 	.db 0x00
      0006D6                        574 ___str_3:
      0006D6 4C 61 73 74 20 6C 6F   575 	.ascii "Last login: [time]"
             67 69 6E 3A 20 5B 74
             69 6D 65 5D
      0006E8 0A                     576 	.db 0x0a
      0006E9 00                     577 	.db 0x00
      0006EA                        578 ___str_4:
      0006EA 50 61 73 73 77 6F 72   579 	.ascii "Password check FAILED"
             64 20 63 68 65 63 6B
             20 46 41 49 4C 45 44
      0006FF 0A                     580 	.db 0x0a
      000700 00                     581 	.db 0x00
      000701                        582 ___str_5:
      000701 52 65 70 6F 72 74 69   583 	.ascii "Reporting incident to police..."
             6E 67 20 69 6E 63 69
             64 65 6E 74 20 74 6F
             20 70 6F 6C 69 63 65
             2E 2E 2E
      000720 0A                     584 	.db 0x0a
      000721 00                     585 	.db 0x00
                                    586 	.area XINIT   (CODE)
                                    587 	.area CABS    (ABS,CODE)
