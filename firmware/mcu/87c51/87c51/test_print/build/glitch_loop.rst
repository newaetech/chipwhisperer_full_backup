                                      1 ;--------------------------------------------------------
                                      2 ; File Created by SDCC : free open source ANSI-C Compiler
                                      3 ; Version 3.6.0 #9615 (MINGW32)
                                      4 ;--------------------------------------------------------
                                      5 	.module glitch_loop
                                      6 	.optsdcc -mmcs51 --model-small
                                      7 	
                                      8 ;--------------------------------------------------------
                                      9 ; Public variables in this module
                                     10 ;--------------------------------------------------------
                                     11 	.globl _TF2
                                     12 	.globl _EXF2
                                     13 	.globl _RCLK
                                     14 	.globl _TCLK
                                     15 	.globl _EXEN2
                                     16 	.globl _TR2
                                     17 	.globl _C_T2
                                     18 	.globl _CP_RL2
                                     19 	.globl _T2CON_7
                                     20 	.globl _T2CON_6
                                     21 	.globl _T2CON_5
                                     22 	.globl _T2CON_4
                                     23 	.globl _T2CON_3
                                     24 	.globl _T2CON_2
                                     25 	.globl _T2CON_1
                                     26 	.globl _T2CON_0
                                     27 	.globl _PT2
                                     28 	.globl _ET2
                                     29 	.globl _CY
                                     30 	.globl _AC
                                     31 	.globl _F0
                                     32 	.globl _RS1
                                     33 	.globl _RS0
                                     34 	.globl _OV
                                     35 	.globl _F1
                                     36 	.globl _P
                                     37 	.globl _PS
                                     38 	.globl _PT1
                                     39 	.globl _PX1
                                     40 	.globl _PT0
                                     41 	.globl _PX0
                                     42 	.globl _RD
                                     43 	.globl _WR
                                     44 	.globl _T1
                                     45 	.globl _T0
                                     46 	.globl _INT1
                                     47 	.globl _INT0
                                     48 	.globl _TXD
                                     49 	.globl _RXD
                                     50 	.globl _P3_7
                                     51 	.globl _P3_6
                                     52 	.globl _P3_5
                                     53 	.globl _P3_4
                                     54 	.globl _P3_3
                                     55 	.globl _P3_2
                                     56 	.globl _P3_1
                                     57 	.globl _P3_0
                                     58 	.globl _EA
                                     59 	.globl _ES
                                     60 	.globl _ET1
                                     61 	.globl _EX1
                                     62 	.globl _ET0
                                     63 	.globl _EX0
                                     64 	.globl _P2_7
                                     65 	.globl _P2_6
                                     66 	.globl _P2_5
                                     67 	.globl _P2_4
                                     68 	.globl _P2_3
                                     69 	.globl _P2_2
                                     70 	.globl _P2_1
                                     71 	.globl _P2_0
                                     72 	.globl _SM0
                                     73 	.globl _SM1
                                     74 	.globl _SM2
                                     75 	.globl _REN
                                     76 	.globl _TB8
                                     77 	.globl _RB8
                                     78 	.globl _TI
                                     79 	.globl _RI
                                     80 	.globl _P1_7
                                     81 	.globl _P1_6
                                     82 	.globl _P1_5
                                     83 	.globl _P1_4
                                     84 	.globl _P1_3
                                     85 	.globl _P1_2
                                     86 	.globl _P1_1
                                     87 	.globl _P1_0
                                     88 	.globl _TF1
                                     89 	.globl _TR1
                                     90 	.globl _TF0
                                     91 	.globl _TR0
                                     92 	.globl _IE1
                                     93 	.globl _IT1
                                     94 	.globl _IE0
                                     95 	.globl _IT0
                                     96 	.globl _P0_7
                                     97 	.globl _P0_6
                                     98 	.globl _P0_5
                                     99 	.globl _P0_4
                                    100 	.globl _P0_3
                                    101 	.globl _P0_2
                                    102 	.globl _P0_1
                                    103 	.globl _P0_0
                                    104 	.globl _TH2
                                    105 	.globl _TL2
                                    106 	.globl _RCAP2H
                                    107 	.globl _RCAP2L
                                    108 	.globl _T2CON
                                    109 	.globl _B
                                    110 	.globl _ACC
                                    111 	.globl _PSW
                                    112 	.globl _IP
                                    113 	.globl _P3
                                    114 	.globl _IE
                                    115 	.globl _P2
                                    116 	.globl _SBUF
                                    117 	.globl _SCON
                                    118 	.globl _P1
                                    119 	.globl _TH1
                                    120 	.globl _TH0
                                    121 	.globl _TL1
                                    122 	.globl _TL0
                                    123 	.globl _TMOD
                                    124 	.globl _TCON
                                    125 	.globl _PCON
                                    126 	.globl _DPH
                                    127 	.globl _DPL
                                    128 	.globl _SP
                                    129 	.globl _P0
                                    130 	.globl _glitch_loop
                                    131 ;--------------------------------------------------------
                                    132 ; special function registers
                                    133 ;--------------------------------------------------------
                                    134 	.area RSEG    (ABS,DATA)
      000000                        135 	.org 0x0000
                           000080   136 _P0	=	0x0080
                           000081   137 _SP	=	0x0081
                           000082   138 _DPL	=	0x0082
                           000083   139 _DPH	=	0x0083
                           000087   140 _PCON	=	0x0087
                           000088   141 _TCON	=	0x0088
                           000089   142 _TMOD	=	0x0089
                           00008A   143 _TL0	=	0x008a
                           00008B   144 _TL1	=	0x008b
                           00008C   145 _TH0	=	0x008c
                           00008D   146 _TH1	=	0x008d
                           000090   147 _P1	=	0x0090
                           000098   148 _SCON	=	0x0098
                           000099   149 _SBUF	=	0x0099
                           0000A0   150 _P2	=	0x00a0
                           0000A8   151 _IE	=	0x00a8
                           0000B0   152 _P3	=	0x00b0
                           0000B8   153 _IP	=	0x00b8
                           0000D0   154 _PSW	=	0x00d0
                           0000E0   155 _ACC	=	0x00e0
                           0000F0   156 _B	=	0x00f0
                           0000C8   157 _T2CON	=	0x00c8
                           0000CA   158 _RCAP2L	=	0x00ca
                           0000CB   159 _RCAP2H	=	0x00cb
                           0000CC   160 _TL2	=	0x00cc
                           0000CD   161 _TH2	=	0x00cd
                                    162 ;--------------------------------------------------------
                                    163 ; special function bits
                                    164 ;--------------------------------------------------------
                                    165 	.area RSEG    (ABS,DATA)
      000000                        166 	.org 0x0000
                           000080   167 _P0_0	=	0x0080
                           000081   168 _P0_1	=	0x0081
                           000082   169 _P0_2	=	0x0082
                           000083   170 _P0_3	=	0x0083
                           000084   171 _P0_4	=	0x0084
                           000085   172 _P0_5	=	0x0085
                           000086   173 _P0_6	=	0x0086
                           000087   174 _P0_7	=	0x0087
                           000088   175 _IT0	=	0x0088
                           000089   176 _IE0	=	0x0089
                           00008A   177 _IT1	=	0x008a
                           00008B   178 _IE1	=	0x008b
                           00008C   179 _TR0	=	0x008c
                           00008D   180 _TF0	=	0x008d
                           00008E   181 _TR1	=	0x008e
                           00008F   182 _TF1	=	0x008f
                           000090   183 _P1_0	=	0x0090
                           000091   184 _P1_1	=	0x0091
                           000092   185 _P1_2	=	0x0092
                           000093   186 _P1_3	=	0x0093
                           000094   187 _P1_4	=	0x0094
                           000095   188 _P1_5	=	0x0095
                           000096   189 _P1_6	=	0x0096
                           000097   190 _P1_7	=	0x0097
                           000098   191 _RI	=	0x0098
                           000099   192 _TI	=	0x0099
                           00009A   193 _RB8	=	0x009a
                           00009B   194 _TB8	=	0x009b
                           00009C   195 _REN	=	0x009c
                           00009D   196 _SM2	=	0x009d
                           00009E   197 _SM1	=	0x009e
                           00009F   198 _SM0	=	0x009f
                           0000A0   199 _P2_0	=	0x00a0
                           0000A1   200 _P2_1	=	0x00a1
                           0000A2   201 _P2_2	=	0x00a2
                           0000A3   202 _P2_3	=	0x00a3
                           0000A4   203 _P2_4	=	0x00a4
                           0000A5   204 _P2_5	=	0x00a5
                           0000A6   205 _P2_6	=	0x00a6
                           0000A7   206 _P2_7	=	0x00a7
                           0000A8   207 _EX0	=	0x00a8
                           0000A9   208 _ET0	=	0x00a9
                           0000AA   209 _EX1	=	0x00aa
                           0000AB   210 _ET1	=	0x00ab
                           0000AC   211 _ES	=	0x00ac
                           0000AF   212 _EA	=	0x00af
                           0000B0   213 _P3_0	=	0x00b0
                           0000B1   214 _P3_1	=	0x00b1
                           0000B2   215 _P3_2	=	0x00b2
                           0000B3   216 _P3_3	=	0x00b3
                           0000B4   217 _P3_4	=	0x00b4
                           0000B5   218 _P3_5	=	0x00b5
                           0000B6   219 _P3_6	=	0x00b6
                           0000B7   220 _P3_7	=	0x00b7
                           0000B0   221 _RXD	=	0x00b0
                           0000B1   222 _TXD	=	0x00b1
                           0000B2   223 _INT0	=	0x00b2
                           0000B3   224 _INT1	=	0x00b3
                           0000B4   225 _T0	=	0x00b4
                           0000B5   226 _T1	=	0x00b5
                           0000B6   227 _WR	=	0x00b6
                           0000B7   228 _RD	=	0x00b7
                           0000B8   229 _PX0	=	0x00b8
                           0000B9   230 _PT0	=	0x00b9
                           0000BA   231 _PX1	=	0x00ba
                           0000BB   232 _PT1	=	0x00bb
                           0000BC   233 _PS	=	0x00bc
                           0000D0   234 _P	=	0x00d0
                           0000D1   235 _F1	=	0x00d1
                           0000D2   236 _OV	=	0x00d2
                           0000D3   237 _RS0	=	0x00d3
                           0000D4   238 _RS1	=	0x00d4
                           0000D5   239 _F0	=	0x00d5
                           0000D6   240 _AC	=	0x00d6
                           0000D7   241 _CY	=	0x00d7
                           0000AD   242 _ET2	=	0x00ad
                           0000BD   243 _PT2	=	0x00bd
                           0000C8   244 _T2CON_0	=	0x00c8
                           0000C9   245 _T2CON_1	=	0x00c9
                           0000CA   246 _T2CON_2	=	0x00ca
                           0000CB   247 _T2CON_3	=	0x00cb
                           0000CC   248 _T2CON_4	=	0x00cc
                           0000CD   249 _T2CON_5	=	0x00cd
                           0000CE   250 _T2CON_6	=	0x00ce
                           0000CF   251 _T2CON_7	=	0x00cf
                           0000C8   252 _CP_RL2	=	0x00c8
                           0000C9   253 _C_T2	=	0x00c9
                           0000CA   254 _TR2	=	0x00ca
                           0000CB   255 _EXEN2	=	0x00cb
                           0000CC   256 _TCLK	=	0x00cc
                           0000CD   257 _RCLK	=	0x00cd
                           0000CE   258 _EXF2	=	0x00ce
                           0000CF   259 _TF2	=	0x00cf
                                    260 ;--------------------------------------------------------
                                    261 ; overlayable register banks
                                    262 ;--------------------------------------------------------
                                    263 	.area REG_BANK_0	(REL,OVR,DATA)
      000000                        264 	.ds 8
                                    265 ;--------------------------------------------------------
                                    266 ; internal ram data
                                    267 ;--------------------------------------------------------
                                    268 	.area DSEG    (DATA)
      000008                        269 _glitch_loop_j_1_17:
      000008                        270 	.ds 2
      00000A                        271 _glitch_loop_inc_1_17:
      00000A                        272 	.ds 1
                                    273 ;--------------------------------------------------------
                                    274 ; overlayable items in internal ram 
                                    275 ;--------------------------------------------------------
                                    276 ;--------------------------------------------------------
                                    277 ; indirectly addressable internal ram data
                                    278 ;--------------------------------------------------------
                                    279 	.area ISEG    (DATA)
                                    280 ;--------------------------------------------------------
                                    281 ; absolute internal ram data
                                    282 ;--------------------------------------------------------
                                    283 	.area IABS    (ABS,DATA)
                                    284 	.area IABS    (ABS,DATA)
                                    285 ;--------------------------------------------------------
                                    286 ; bit data
                                    287 ;--------------------------------------------------------
                                    288 	.area BSEG    (BIT)
                                    289 ;--------------------------------------------------------
                                    290 ; paged external ram data
                                    291 ;--------------------------------------------------------
                                    292 	.area PSEG    (PAG,XDATA)
                                    293 ;--------------------------------------------------------
                                    294 ; external ram data
                                    295 ;--------------------------------------------------------
                                    296 	.area XSEG    (XDATA)
                                    297 ;--------------------------------------------------------
                                    298 ; absolute external ram data
                                    299 ;--------------------------------------------------------
                                    300 	.area XABS    (ABS,XDATA)
                                    301 ;--------------------------------------------------------
                                    302 ; external initialized ram data
                                    303 ;--------------------------------------------------------
                                    304 	.area XISEG   (XDATA)
                                    305 	.area HOME    (CODE)
                                    306 	.area GSINIT0 (CODE)
                                    307 	.area GSINIT1 (CODE)
                                    308 	.area GSINIT2 (CODE)
                                    309 	.area GSINIT3 (CODE)
                                    310 	.area GSINIT4 (CODE)
                                    311 	.area GSINIT5 (CODE)
                                    312 	.area GSINIT  (CODE)
                                    313 	.area GSFINAL (CODE)
                                    314 	.area CSEG    (CODE)
                                    315 ;--------------------------------------------------------
                                    316 ; global & static initialisations
                                    317 ;--------------------------------------------------------
                                    318 	.area HOME    (CODE)
                                    319 	.area GSINIT  (CODE)
                                    320 	.area GSFINAL (CODE)
                                    321 	.area GSINIT  (CODE)
                                    322 ;--------------------------------------------------------
                                    323 ; Home
                                    324 ;--------------------------------------------------------
                                    325 	.area HOME    (CODE)
                                    326 	.area HOME    (CODE)
                                    327 ;--------------------------------------------------------
                                    328 ; code
                                    329 ;--------------------------------------------------------
                                    330 	.area CSEG    (CODE)
                                    331 ;------------------------------------------------------------
                                    332 ;Allocation info for local variables in function 'glitch_loop'
                                    333 ;------------------------------------------------------------
                                    334 ;i                         Allocated to registers r1 r2 
                                    335 ;j                         Allocated with name '_glitch_loop_j_1_17'
                                    336 ;cnt                       Allocated to registers r3 r4 r5 r6 
                                    337 ;inc                       Allocated with name '_glitch_loop_inc_1_17'
                                    338 ;------------------------------------------------------------
                                    339 ;	glitch_loop.c:9: void glitch_loop(void)
                                    340 ;	-----------------------------------------
                                    341 ;	 function glitch_loop
                                    342 ;	-----------------------------------------
      0000F1                        343 _glitch_loop:
                           000007   344 	ar7 = 0x07
                           000006   345 	ar6 = 0x06
                           000005   346 	ar5 = 0x05
                           000004   347 	ar4 = 0x04
                           000003   348 	ar3 = 0x03
                           000002   349 	ar2 = 0x02
                           000001   350 	ar1 = 0x01
                           000000   351 	ar0 = 0x00
                                    352 ;	glitch_loop.c:14: unsigned char inc=0;
      0000F1 75 0A 00         [24]  353 	mov	_glitch_loop_inc_1_17,#0x00
                                    354 ;	glitch_loop.c:16: while (1) {
      0000F4                        355 00104$:
                                    356 ;	glitch_loop.c:17: cnt = 0;
      0000F4 7B 00            [12]  357 	mov	r3,#0x00
      0000F6 7C 00            [12]  358 	mov	r4,#0x00
      0000F8 7D 00            [12]  359 	mov	r5,#0x00
      0000FA 7E 00            [12]  360 	mov	r6,#0x00
                                    361 ;	glitch_loop.c:18: trigger_high();     		    /* Toggle P1.0 before each loop */
      0000FC 43 90 01         [24]  362 	orl	_P1,#0x01
                                    363 ;	glitch_loop.c:19: for(i = 0; i < 300; i++){
      0000FF 79 00            [12]  364 	mov	r1,#0x00
      000101 7A 00            [12]  365 	mov	r2,#0x00
      000103                        366 00109$:
                                    367 ;	glitch_loop.c:20: for(j = 0; j < 300; j++){
      000103 75 08 2C         [24]  368 	mov	_glitch_loop_j_1_17,#0x2c
      000106 75 09 01         [24]  369 	mov	(_glitch_loop_j_1_17 + 1),#0x01
      000109                        370 00108$:
                                    371 ;	glitch_loop.c:21: cnt++;
      000109 0B               [12]  372 	inc	r3
      00010A BB 00 09         [24]  373 	cjne	r3,#0x00,00128$
      00010D 0C               [12]  374 	inc	r4
      00010E BC 00 05         [24]  375 	cjne	r4,#0x00,00128$
      000111 0D               [12]  376 	inc	r5
      000112 BD 00 01         [24]  377 	cjne	r5,#0x00,00128$
      000115 0E               [12]  378 	inc	r6
      000116                        379 00128$:
      000116 E5 08            [12]  380 	mov	a,_glitch_loop_j_1_17
      000118 24 FF            [12]  381 	add	a,#0xff
      00011A F8               [12]  382 	mov	r0,a
      00011B E5 09            [12]  383 	mov	a,(_glitch_loop_j_1_17 + 1)
      00011D 34 FF            [12]  384 	addc	a,#0xff
      00011F FF               [12]  385 	mov	r7,a
      000120 88 08            [24]  386 	mov	_glitch_loop_j_1_17,r0
      000122 8F 09            [24]  387 	mov	(_glitch_loop_j_1_17 + 1),r7
                                    388 ;	glitch_loop.c:20: for(j = 0; j < 300; j++){
      000124 E8               [12]  389 	mov	a,r0
      000125 4F               [12]  390 	orl	a,r7
      000126 70 E1            [24]  391 	jnz	00108$
                                    392 ;	glitch_loop.c:19: for(i = 0; i < 300; i++){
      000128 09               [12]  393 	inc	r1
      000129 B9 00 01         [24]  394 	cjne	r1,#0x00,00130$
      00012C 0A               [12]  395 	inc	r2
      00012D                        396 00130$:
      00012D C3               [12]  397 	clr	c
      00012E E9               [12]  398 	mov	a,r1
      00012F 94 2C            [12]  399 	subb	a,#0x2c
      000131 EA               [12]  400 	mov	a,r2
      000132 94 01            [12]  401 	subb	a,#0x01
      000134 40 CD            [24]  402 	jc	00109$
                                    403 ;	glitch_loop.c:24: trigger_low();
      000136 53 90 FE         [24]  404 	anl	_P1,#0xfe
                                    405 ;	glitch_loop.c:25: inc++;
      000139 05 0A            [12]  406 	inc	_glitch_loop_inc_1_17
      00013B 80 B7            [24]  407 	sjmp	00104$
                                    408 	.area CSEG    (CODE)
                                    409 	.area CONST   (CODE)
                                    410 	.area XINIT   (CODE)
                                    411 	.area CABS    (ABS,CODE)
