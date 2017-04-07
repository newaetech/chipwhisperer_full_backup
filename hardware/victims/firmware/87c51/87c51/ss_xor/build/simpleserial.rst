                                      1 ;--------------------------------------------------------
                                      2 ; File Created by SDCC : free open source ANSI-C Compiler
                                      3 ; Version 3.6.0 #9615 (MINGW32)
                                      4 ;--------------------------------------------------------
                                      5 	.module simpleserial
                                      6 	.optsdcc -mmcs51 --model-small
                                      7 	
                                      8 ;--------------------------------------------------------
                                      9 ; Public variables in this module
                                     10 ;--------------------------------------------------------
                                     11 	.globl _simpleserial_decode
                                     12 	.globl _simpleserial_encode
                                     13 	.globl _putchar
                                     14 	.globl _getchar
                                     15 	.globl _simpleserial_get
                                     16 	.globl _simpleserial_put
                                     17 ;--------------------------------------------------------
                                     18 ; special function registers
                                     19 ;--------------------------------------------------------
                                     20 	.area RSEG    (ABS,DATA)
      000000                         21 	.org 0x0000
                                     22 ;--------------------------------------------------------
                                     23 ; special function bits
                                     24 ;--------------------------------------------------------
                                     25 	.area RSEG    (ABS,DATA)
      000000                         26 	.org 0x0000
                                     27 ;--------------------------------------------------------
                                     28 ; overlayable register banks
                                     29 ;--------------------------------------------------------
                                     30 	.area REG_BANK_0	(REL,OVR,DATA)
      000000                         31 	.ds 8
                                     32 ;--------------------------------------------------------
                                     33 ; internal ram data
                                     34 ;--------------------------------------------------------
                                     35 	.area DSEG    (DATA)
                                     36 ;--------------------------------------------------------
                                     37 ; overlayable items in internal ram 
                                     38 ;--------------------------------------------------------
                                     39 ;--------------------------------------------------------
                                     40 ; indirectly addressable internal ram data
                                     41 ;--------------------------------------------------------
                                     42 	.area ISEG    (DATA)
                                     43 ;--------------------------------------------------------
                                     44 ; absolute internal ram data
                                     45 ;--------------------------------------------------------
                                     46 	.area IABS    (ABS,DATA)
                                     47 	.area IABS    (ABS,DATA)
                                     48 ;--------------------------------------------------------
                                     49 ; bit data
                                     50 ;--------------------------------------------------------
                                     51 	.area BSEG    (BIT)
                                     52 ;--------------------------------------------------------
                                     53 ; paged external ram data
                                     54 ;--------------------------------------------------------
                                     55 	.area PSEG    (PAG,XDATA)
                                     56 ;--------------------------------------------------------
                                     57 ; external ram data
                                     58 ;--------------------------------------------------------
                                     59 	.area XSEG    (XDATA)
                                     60 ;--------------------------------------------------------
                                     61 ; absolute external ram data
                                     62 ;--------------------------------------------------------
                                     63 	.area XABS    (ABS,XDATA)
                                     64 ;--------------------------------------------------------
                                     65 ; external initialized ram data
                                     66 ;--------------------------------------------------------
                                     67 	.area XISEG   (XDATA)
                                     68 	.area HOME    (CODE)
                                     69 	.area GSINIT0 (CODE)
                                     70 	.area GSINIT1 (CODE)
                                     71 	.area GSINIT2 (CODE)
                                     72 	.area GSINIT3 (CODE)
                                     73 	.area GSINIT4 (CODE)
                                     74 	.area GSINIT5 (CODE)
                                     75 	.area GSINIT  (CODE)
                                     76 	.area GSFINAL (CODE)
                                     77 	.area CSEG    (CODE)
                                     78 ;--------------------------------------------------------
                                     79 ; global & static initialisations
                                     80 ;--------------------------------------------------------
                                     81 	.area HOME    (CODE)
                                     82 	.area GSINIT  (CODE)
                                     83 	.area GSFINAL (CODE)
                                     84 	.area GSINIT  (CODE)
                                     85 ;--------------------------------------------------------
                                     86 ; Home
                                     87 ;--------------------------------------------------------
                                     88 	.area HOME    (CODE)
                                     89 	.area HOME    (CODE)
                                     90 ;--------------------------------------------------------
                                     91 ; code
                                     92 ;--------------------------------------------------------
                                     93 	.area CSEG    (CODE)
                                     94 ;------------------------------------------------------------
                                     95 ;Allocation info for local variables in function 'simpleserial_encode'
                                     96 ;------------------------------------------------------------
                                     97 ;data                      Allocated to stack - _bp -5
                                     98 ;ascii                     Allocated to stack - _bp +1
                                     99 ;i                         Allocated to stack - _bp +4
                                    100 ;n_hi                      Allocated to registers r5 
                                    101 ;n_lo                      Allocated to stack - _bp +6
                                    102 ;------------------------------------------------------------
                                    103 ;	simpleserial.c:18: void simpleserial_encode(uint8_t* ascii, uint8_t* data)
                                    104 ;	-----------------------------------------
                                    105 ;	 function simpleserial_encode
                                    106 ;	-----------------------------------------
      00015C                        107 _simpleserial_encode:
                           000007   108 	ar7 = 0x07
                           000006   109 	ar6 = 0x06
                           000005   110 	ar5 = 0x05
                           000004   111 	ar4 = 0x04
                           000003   112 	ar3 = 0x03
                           000002   113 	ar2 = 0x02
                           000001   114 	ar1 = 0x01
                           000000   115 	ar0 = 0x00
      00015C C0 08            [24]  116 	push	_bp
      00015E 85 81 08         [24]  117 	mov	_bp,sp
      000161 C0 82            [24]  118 	push	dpl
      000163 C0 83            [24]  119 	push	dph
      000165 C0 F0            [24]  120 	push	b
      000167 05 81            [12]  121 	inc	sp
      000169 05 81            [12]  122 	inc	sp
      00016B 05 81            [12]  123 	inc	sp
                                    124 ;	simpleserial.c:22: ascii[0] = 'r';
      00016D A8 08            [24]  125 	mov	r0,_bp
      00016F 08               [12]  126 	inc	r0
      000170 86 82            [24]  127 	mov	dpl,@r0
      000172 08               [12]  128 	inc	r0
      000173 86 83            [24]  129 	mov	dph,@r0
      000175 08               [12]  130 	inc	r0
      000176 86 F0            [24]  131 	mov	b,@r0
      000178 74 72            [12]  132 	mov	a,#0x72
      00017A 12 05 56         [24]  133 	lcall	__gptrput
                                    134 ;	simpleserial.c:23: for(i = 0; i < BLOCK_LENGTH; i++)
      00017D E5 08            [12]  135 	mov	a,_bp
      00017F 24 04            [12]  136 	add	a,#0x04
      000181 F8               [12]  137 	mov	r0,a
      000182 E4               [12]  138 	clr	a
      000183 F6               [12]  139 	mov	@r0,a
      000184 08               [12]  140 	inc	r0
      000185 F6               [12]  141 	mov	@r0,a
      000186                        142 00102$:
                                    143 ;	simpleserial.c:25: unsigned char n_hi = (data[i] >> 4) & 0x0f;
      000186 E5 08            [12]  144 	mov	a,_bp
      000188 24 FB            [12]  145 	add	a,#0xfb
      00018A F8               [12]  146 	mov	r0,a
      00018B E5 08            [12]  147 	mov	a,_bp
      00018D 24 04            [12]  148 	add	a,#0x04
      00018F F9               [12]  149 	mov	r1,a
      000190 E7               [12]  150 	mov	a,@r1
      000191 26               [12]  151 	add	a,@r0
      000192 FA               [12]  152 	mov	r2,a
      000193 09               [12]  153 	inc	r1
      000194 E7               [12]  154 	mov	a,@r1
      000195 08               [12]  155 	inc	r0
      000196 36               [12]  156 	addc	a,@r0
      000197 FB               [12]  157 	mov	r3,a
      000198 08               [12]  158 	inc	r0
      000199 86 04            [24]  159 	mov	ar4,@r0
      00019B 8A 82            [24]  160 	mov	dpl,r2
      00019D 8B 83            [24]  161 	mov	dph,r3
      00019F 8C F0            [24]  162 	mov	b,r4
      0001A1 12 05 71         [24]  163 	lcall	__gptrget
      0001A4 FA               [12]  164 	mov	r2,a
      0001A5 C4               [12]  165 	swap	a
      0001A6 54 0F            [12]  166 	anl	a,#0x0f
      0001A8 FD               [12]  167 	mov	r5,a
                                    168 ;	simpleserial.c:26: unsigned char n_lo =  data[i]       & 0x0f;
      0001A9 E5 08            [12]  169 	mov	a,_bp
      0001AB 24 06            [12]  170 	add	a,#0x06
      0001AD F8               [12]  171 	mov	r0,a
      0001AE 74 0F            [12]  172 	mov	a,#0x0f
      0001B0 5A               [12]  173 	anl	a,r2
      0001B1 F6               [12]  174 	mov	@r0,a
                                    175 ;	simpleserial.c:27: ascii[2*i+1] = hex_lookup[n_hi];
      0001B2 E5 08            [12]  176 	mov	a,_bp
      0001B4 24 04            [12]  177 	add	a,#0x04
      0001B6 F8               [12]  178 	mov	r0,a
      0001B7 86 02            [24]  179 	mov	ar2,@r0
      0001B9 08               [12]  180 	inc	r0
      0001BA E6               [12]  181 	mov	a,@r0
      0001BB CA               [12]  182 	xch	a,r2
      0001BC 25 E0            [12]  183 	add	a,acc
      0001BE CA               [12]  184 	xch	a,r2
      0001BF 33               [12]  185 	rlc	a
      0001C0 FB               [12]  186 	mov	r3,a
      0001C1 74 01            [12]  187 	mov	a,#0x01
      0001C3 2A               [12]  188 	add	a,r2
      0001C4 FC               [12]  189 	mov	r4,a
      0001C5 E4               [12]  190 	clr	a
      0001C6 3B               [12]  191 	addc	a,r3
      0001C7 FF               [12]  192 	mov	r7,a
      0001C8 A8 08            [24]  193 	mov	r0,_bp
      0001CA 08               [12]  194 	inc	r0
      0001CB EC               [12]  195 	mov	a,r4
      0001CC 26               [12]  196 	add	a,@r0
      0001CD FC               [12]  197 	mov	r4,a
      0001CE EF               [12]  198 	mov	a,r7
      0001CF 08               [12]  199 	inc	r0
      0001D0 36               [12]  200 	addc	a,@r0
      0001D1 FF               [12]  201 	mov	r7,a
      0001D2 08               [12]  202 	inc	r0
      0001D3 86 06            [24]  203 	mov	ar6,@r0
      0001D5 ED               [12]  204 	mov	a,r5
      0001D6 90 05 91         [24]  205 	mov	dptr,#_hex_lookup
      0001D9 93               [24]  206 	movc	a,@a+dptr
      0001DA FD               [12]  207 	mov	r5,a
      0001DB 8C 82            [24]  208 	mov	dpl,r4
      0001DD 8F 83            [24]  209 	mov	dph,r7
      0001DF 8E F0            [24]  210 	mov	b,r6
      0001E1 12 05 56         [24]  211 	lcall	__gptrput
                                    212 ;	simpleserial.c:28: ascii[2*i+2] = hex_lookup[n_lo];
      0001E4 74 02            [12]  213 	mov	a,#0x02
      0001E6 2A               [12]  214 	add	a,r2
      0001E7 FA               [12]  215 	mov	r2,a
      0001E8 E4               [12]  216 	clr	a
      0001E9 3B               [12]  217 	addc	a,r3
      0001EA FB               [12]  218 	mov	r3,a
      0001EB A8 08            [24]  219 	mov	r0,_bp
      0001ED 08               [12]  220 	inc	r0
      0001EE EA               [12]  221 	mov	a,r2
      0001EF 26               [12]  222 	add	a,@r0
      0001F0 FA               [12]  223 	mov	r2,a
      0001F1 EB               [12]  224 	mov	a,r3
      0001F2 08               [12]  225 	inc	r0
      0001F3 36               [12]  226 	addc	a,@r0
      0001F4 FB               [12]  227 	mov	r3,a
      0001F5 08               [12]  228 	inc	r0
      0001F6 86 07            [24]  229 	mov	ar7,@r0
      0001F8 E5 08            [12]  230 	mov	a,_bp
      0001FA 24 06            [12]  231 	add	a,#0x06
      0001FC F8               [12]  232 	mov	r0,a
      0001FD E6               [12]  233 	mov	a,@r0
      0001FE 90 05 91         [24]  234 	mov	dptr,#_hex_lookup
      000201 93               [24]  235 	movc	a,@a+dptr
      000202 FE               [12]  236 	mov	r6,a
      000203 8A 82            [24]  237 	mov	dpl,r2
      000205 8B 83            [24]  238 	mov	dph,r3
      000207 8F F0            [24]  239 	mov	b,r7
      000209 12 05 56         [24]  240 	lcall	__gptrput
                                    241 ;	simpleserial.c:23: for(i = 0; i < BLOCK_LENGTH; i++)
      00020C E5 08            [12]  242 	mov	a,_bp
      00020E 24 04            [12]  243 	add	a,#0x04
      000210 F8               [12]  244 	mov	r0,a
      000211 06               [12]  245 	inc	@r0
      000212 B6 00 02         [24]  246 	cjne	@r0,#0x00,00113$
      000215 08               [12]  247 	inc	r0
      000216 06               [12]  248 	inc	@r0
      000217                        249 00113$:
      000217 E5 08            [12]  250 	mov	a,_bp
      000219 24 04            [12]  251 	add	a,#0x04
      00021B F8               [12]  252 	mov	r0,a
      00021C C3               [12]  253 	clr	c
      00021D E6               [12]  254 	mov	a,@r0
      00021E 94 10            [12]  255 	subb	a,#0x10
      000220 08               [12]  256 	inc	r0
      000221 E6               [12]  257 	mov	a,@r0
      000222 64 80            [12]  258 	xrl	a,#0x80
      000224 94 80            [12]  259 	subb	a,#0x80
      000226 50 03            [24]  260 	jnc	00114$
      000228 02 01 86         [24]  261 	ljmp	00102$
      00022B                        262 00114$:
                                    263 ;	simpleserial.c:30: ascii[ASCII_LENGTH-1] = '\n';
      00022B A8 08            [24]  264 	mov	r0,_bp
      00022D 08               [12]  265 	inc	r0
      00022E 74 21            [12]  266 	mov	a,#0x21
      000230 26               [12]  267 	add	a,@r0
      000231 FD               [12]  268 	mov	r5,a
      000232 E4               [12]  269 	clr	a
      000233 08               [12]  270 	inc	r0
      000234 36               [12]  271 	addc	a,@r0
      000235 FE               [12]  272 	mov	r6,a
      000236 08               [12]  273 	inc	r0
      000237 86 07            [24]  274 	mov	ar7,@r0
      000239 8D 82            [24]  275 	mov	dpl,r5
      00023B 8E 83            [24]  276 	mov	dph,r6
      00023D 8F F0            [24]  277 	mov	b,r7
      00023F 74 0A            [12]  278 	mov	a,#0x0a
      000241 12 05 56         [24]  279 	lcall	__gptrput
      000244 85 08 81         [24]  280 	mov	sp,_bp
      000247 D0 08            [24]  281 	pop	_bp
      000249 22               [24]  282 	ret
                                    283 ;------------------------------------------------------------
                                    284 ;Allocation info for local variables in function 'simpleserial_decode'
                                    285 ;------------------------------------------------------------
                                    286 ;data                      Allocated to stack - _bp -5
                                    287 ;ascii                     Allocated to stack - _bp +1
                                    288 ;i                         Allocated to registers r3 r4 
                                    289 ;n_hi                      Allocated to stack - _bp +5
                                    290 ;n_lo                      Allocated to stack - _bp +4
                                    291 ;------------------------------------------------------------
                                    292 ;	simpleserial.c:34: void simpleserial_decode(uint8_t* ascii, uint8_t* data)
                                    293 ;	-----------------------------------------
                                    294 ;	 function simpleserial_decode
                                    295 ;	-----------------------------------------
      00024A                        296 _simpleserial_decode:
      00024A C0 08            [24]  297 	push	_bp
      00024C 85 81 08         [24]  298 	mov	_bp,sp
      00024F C0 82            [24]  299 	push	dpl
      000251 C0 83            [24]  300 	push	dph
      000253 C0 F0            [24]  301 	push	b
      000255 05 81            [12]  302 	inc	sp
      000257 05 81            [12]  303 	inc	sp
                                    304 ;	simpleserial.c:38: for(i = 0; i < BLOCK_LENGTH; i++)
      000259 7B 00            [12]  305 	mov	r3,#0x00
      00025B 7C 00            [12]  306 	mov	r4,#0x00
      00025D                        307 00108$:
                                    308 ;	simpleserial.c:40: unsigned char n_hi = ascii[2*i + 1];
      00025D 8B 02            [24]  309 	mov	ar2,r3
      00025F EC               [12]  310 	mov	a,r4
      000260 CA               [12]  311 	xch	a,r2
      000261 25 E0            [12]  312 	add	a,acc
      000263 CA               [12]  313 	xch	a,r2
      000264 33               [12]  314 	rlc	a
      000265 FF               [12]  315 	mov	r7,a
      000266 74 01            [12]  316 	mov	a,#0x01
      000268 2A               [12]  317 	add	a,r2
      000269 FD               [12]  318 	mov	r5,a
      00026A E4               [12]  319 	clr	a
      00026B 3F               [12]  320 	addc	a,r7
      00026C FE               [12]  321 	mov	r6,a
      00026D C0 03            [24]  322 	push	ar3
      00026F C0 04            [24]  323 	push	ar4
      000271 A8 08            [24]  324 	mov	r0,_bp
      000273 08               [12]  325 	inc	r0
      000274 ED               [12]  326 	mov	a,r5
      000275 26               [12]  327 	add	a,@r0
      000276 FD               [12]  328 	mov	r5,a
      000277 EE               [12]  329 	mov	a,r6
      000278 08               [12]  330 	inc	r0
      000279 36               [12]  331 	addc	a,@r0
      00027A FC               [12]  332 	mov	r4,a
      00027B 08               [12]  333 	inc	r0
      00027C 86 06            [24]  334 	mov	ar6,@r0
      00027E 8D 82            [24]  335 	mov	dpl,r5
      000280 8C 83            [24]  336 	mov	dph,r4
      000282 8E F0            [24]  337 	mov	b,r6
      000284 E5 08            [12]  338 	mov	a,_bp
      000286 24 05            [12]  339 	add	a,#0x05
      000288 F8               [12]  340 	mov	r0,a
      000289 12 05 71         [24]  341 	lcall	__gptrget
      00028C F6               [12]  342 	mov	@r0,a
                                    343 ;	simpleserial.c:41: unsigned char n_lo = ascii[2*i + 2];
      00028D 74 02            [12]  344 	mov	a,#0x02
      00028F 2A               [12]  345 	add	a,r2
      000290 FA               [12]  346 	mov	r2,a
      000291 E4               [12]  347 	clr	a
      000292 3F               [12]  348 	addc	a,r7
      000293 FF               [12]  349 	mov	r7,a
      000294 A8 08            [24]  350 	mov	r0,_bp
      000296 08               [12]  351 	inc	r0
      000297 EA               [12]  352 	mov	a,r2
      000298 26               [12]  353 	add	a,@r0
      000299 FA               [12]  354 	mov	r2,a
      00029A EF               [12]  355 	mov	a,r7
      00029B 08               [12]  356 	inc	r0
      00029C 36               [12]  357 	addc	a,@r0
      00029D FF               [12]  358 	mov	r7,a
      00029E 08               [12]  359 	inc	r0
      00029F 86 06            [24]  360 	mov	ar6,@r0
      0002A1 8A 82            [24]  361 	mov	dpl,r2
      0002A3 8F 83            [24]  362 	mov	dph,r7
      0002A5 8E F0            [24]  363 	mov	b,r6
      0002A7 E5 08            [12]  364 	mov	a,_bp
      0002A9 24 04            [12]  365 	add	a,#0x04
      0002AB F8               [12]  366 	mov	r0,a
      0002AC 12 05 71         [24]  367 	lcall	__gptrget
      0002AF F6               [12]  368 	mov	@r0,a
                                    369 ;	simpleserial.c:43: if(n_lo <= '9')
      0002B0 E5 08            [12]  370 	mov	a,_bp
      0002B2 24 04            [12]  371 	add	a,#0x04
      0002B4 F8               [12]  372 	mov	r0,a
      0002B5 C3               [12]  373 	clr	c
      0002B6 74 39            [12]  374 	mov	a,#0x39
      0002B8 96               [12]  375 	subb	a,@r0
      0002B9 D0 04            [24]  376 	pop	ar4
      0002BB D0 03            [24]  377 	pop	ar3
      0002BD 40 23            [24]  378 	jc	00102$
                                    379 ;	simpleserial.c:44: data[i] = n_lo - '0';
      0002BF E5 08            [12]  380 	mov	a,_bp
      0002C1 24 FB            [12]  381 	add	a,#0xfb
      0002C3 F8               [12]  382 	mov	r0,a
      0002C4 EB               [12]  383 	mov	a,r3
      0002C5 26               [12]  384 	add	a,@r0
      0002C6 FD               [12]  385 	mov	r5,a
      0002C7 EC               [12]  386 	mov	a,r4
      0002C8 08               [12]  387 	inc	r0
      0002C9 36               [12]  388 	addc	a,@r0
      0002CA FE               [12]  389 	mov	r6,a
      0002CB 08               [12]  390 	inc	r0
      0002CC 86 07            [24]  391 	mov	ar7,@r0
      0002CE E5 08            [12]  392 	mov	a,_bp
      0002D0 24 04            [12]  393 	add	a,#0x04
      0002D2 F8               [12]  394 	mov	r0,a
      0002D3 E6               [12]  395 	mov	a,@r0
      0002D4 24 D0            [12]  396 	add	a,#0xd0
      0002D6 FA               [12]  397 	mov	r2,a
      0002D7 8D 82            [24]  398 	mov	dpl,r5
      0002D9 8E 83            [24]  399 	mov	dph,r6
      0002DB 8F F0            [24]  400 	mov	b,r7
      0002DD 12 05 56         [24]  401 	lcall	__gptrput
      0002E0 80 23            [24]  402 	sjmp	00103$
      0002E2                        403 00102$:
                                    404 ;	simpleserial.c:46: data[i] = n_lo - 'A' + 10;
      0002E2 E5 08            [12]  405 	mov	a,_bp
      0002E4 24 FB            [12]  406 	add	a,#0xfb
      0002E6 F8               [12]  407 	mov	r0,a
      0002E7 EB               [12]  408 	mov	a,r3
      0002E8 26               [12]  409 	add	a,@r0
      0002E9 FD               [12]  410 	mov	r5,a
      0002EA EC               [12]  411 	mov	a,r4
      0002EB 08               [12]  412 	inc	r0
      0002EC 36               [12]  413 	addc	a,@r0
      0002ED FE               [12]  414 	mov	r6,a
      0002EE 08               [12]  415 	inc	r0
      0002EF 86 07            [24]  416 	mov	ar7,@r0
      0002F1 E5 08            [12]  417 	mov	a,_bp
      0002F3 24 04            [12]  418 	add	a,#0x04
      0002F5 F8               [12]  419 	mov	r0,a
      0002F6 86 02            [24]  420 	mov	ar2,@r0
      0002F8 74 C9            [12]  421 	mov	a,#0xc9
      0002FA 2A               [12]  422 	add	a,r2
      0002FB FA               [12]  423 	mov	r2,a
      0002FC 8D 82            [24]  424 	mov	dpl,r5
      0002FE 8E 83            [24]  425 	mov	dph,r6
      000300 8F F0            [24]  426 	mov	b,r7
      000302 12 05 56         [24]  427 	lcall	__gptrput
      000305                        428 00103$:
                                    429 ;	simpleserial.c:48: if(n_hi <= '9')
      000305 E5 08            [12]  430 	mov	a,_bp
      000307 24 05            [12]  431 	add	a,#0x05
      000309 F8               [12]  432 	mov	r0,a
      00030A E6               [12]  433 	mov	a,@r0
      00030B 24 C6            [12]  434 	add	a,#0xff - 0x39
      00030D 40 3C            [24]  435 	jc	00105$
                                    436 ;	simpleserial.c:49: data[i] |= (n_hi - '0') << 4;
      00030F E5 08            [12]  437 	mov	a,_bp
      000311 24 FB            [12]  438 	add	a,#0xfb
      000313 F8               [12]  439 	mov	r0,a
      000314 EB               [12]  440 	mov	a,r3
      000315 26               [12]  441 	add	a,@r0
      000316 FD               [12]  442 	mov	r5,a
      000317 EC               [12]  443 	mov	a,r4
      000318 08               [12]  444 	inc	r0
      000319 36               [12]  445 	addc	a,@r0
      00031A FE               [12]  446 	mov	r6,a
      00031B 08               [12]  447 	inc	r0
      00031C 86 07            [24]  448 	mov	ar7,@r0
      00031E 8D 82            [24]  449 	mov	dpl,r5
      000320 8E 83            [24]  450 	mov	dph,r6
      000322 8F F0            [24]  451 	mov	b,r7
      000324 12 05 71         [24]  452 	lcall	__gptrget
      000327 FA               [12]  453 	mov	r2,a
      000328 C0 03            [24]  454 	push	ar3
      00032A C0 04            [24]  455 	push	ar4
      00032C E5 08            [12]  456 	mov	a,_bp
      00032E 24 05            [12]  457 	add	a,#0x05
      000330 F8               [12]  458 	mov	r0,a
      000331 E6               [12]  459 	mov	a,@r0
      000332 24 D0            [12]  460 	add	a,#0xd0
      000334 C4               [12]  461 	swap	a
      000335 54 F0            [12]  462 	anl	a,#0xf0
      000337 FC               [12]  463 	mov	r4,a
      000338 EA               [12]  464 	mov	a,r2
      000339 42 04            [12]  465 	orl	ar4,a
      00033B 8D 82            [24]  466 	mov	dpl,r5
      00033D 8E 83            [24]  467 	mov	dph,r6
      00033F 8F F0            [24]  468 	mov	b,r7
      000341 EC               [12]  469 	mov	a,r4
      000342 12 05 56         [24]  470 	lcall	__gptrput
      000345 D0 04            [24]  471 	pop	ar4
      000347 D0 03            [24]  472 	pop	ar3
      000349 80 3C            [24]  473 	sjmp	00109$
      00034B                        474 00105$:
                                    475 ;	simpleserial.c:51: data[i] |= (n_hi - 'A' + 10) << 4;
      00034B E5 08            [12]  476 	mov	a,_bp
      00034D 24 FB            [12]  477 	add	a,#0xfb
      00034F F8               [12]  478 	mov	r0,a
      000350 EB               [12]  479 	mov	a,r3
      000351 26               [12]  480 	add	a,@r0
      000352 FD               [12]  481 	mov	r5,a
      000353 EC               [12]  482 	mov	a,r4
      000354 08               [12]  483 	inc	r0
      000355 36               [12]  484 	addc	a,@r0
      000356 FE               [12]  485 	mov	r6,a
      000357 08               [12]  486 	inc	r0
      000358 86 07            [24]  487 	mov	ar7,@r0
      00035A 8D 82            [24]  488 	mov	dpl,r5
      00035C 8E 83            [24]  489 	mov	dph,r6
      00035E 8F F0            [24]  490 	mov	b,r7
      000360 12 05 71         [24]  491 	lcall	__gptrget
      000363 FA               [12]  492 	mov	r2,a
      000364 C0 03            [24]  493 	push	ar3
      000366 C0 04            [24]  494 	push	ar4
      000368 E5 08            [12]  495 	mov	a,_bp
      00036A 24 05            [12]  496 	add	a,#0x05
      00036C F8               [12]  497 	mov	r0,a
      00036D 86 04            [24]  498 	mov	ar4,@r0
      00036F 74 C9            [12]  499 	mov	a,#0xc9
      000371 2C               [12]  500 	add	a,r4
      000372 C4               [12]  501 	swap	a
      000373 54 F0            [12]  502 	anl	a,#0xf0
      000375 FC               [12]  503 	mov	r4,a
      000376 EA               [12]  504 	mov	a,r2
      000377 42 04            [12]  505 	orl	ar4,a
      000379 8D 82            [24]  506 	mov	dpl,r5
      00037B 8E 83            [24]  507 	mov	dph,r6
      00037D 8F F0            [24]  508 	mov	b,r7
      00037F EC               [12]  509 	mov	a,r4
      000380 12 05 56         [24]  510 	lcall	__gptrput
                                    511 ;	simpleserial.c:38: for(i = 0; i < BLOCK_LENGTH; i++)
      000383 D0 04            [24]  512 	pop	ar4
      000385 D0 03            [24]  513 	pop	ar3
                                    514 ;	simpleserial.c:51: data[i] |= (n_hi - 'A' + 10) << 4;
      000387                        515 00109$:
                                    516 ;	simpleserial.c:38: for(i = 0; i < BLOCK_LENGTH; i++)
      000387 0B               [12]  517 	inc	r3
      000388 BB 00 01         [24]  518 	cjne	r3,#0x00,00124$
      00038B 0C               [12]  519 	inc	r4
      00038C                        520 00124$:
      00038C C3               [12]  521 	clr	c
      00038D EB               [12]  522 	mov	a,r3
      00038E 94 10            [12]  523 	subb	a,#0x10
      000390 EC               [12]  524 	mov	a,r4
      000391 64 80            [12]  525 	xrl	a,#0x80
      000393 94 80            [12]  526 	subb	a,#0x80
      000395 50 03            [24]  527 	jnc	00125$
      000397 02 02 5D         [24]  528 	ljmp	00108$
      00039A                        529 00125$:
      00039A 85 08 81         [24]  530 	mov	sp,_bp
      00039D D0 08            [24]  531 	pop	_bp
      00039F 22               [24]  532 	ret
                                    533 ;------------------------------------------------------------
                                    534 ;Allocation info for local variables in function 'simpleserial_get'
                                    535 ;------------------------------------------------------------
                                    536 ;key                       Allocated to stack - _bp -5
                                    537 ;input                     Allocated to stack - _bp +1
                                    538 ;ascii_buffer              Allocated to stack - _bp +4
                                    539 ;ascii_pos                 Allocated to registers r4 
                                    540 ;state                     Allocated to registers r3 
                                    541 ;sloc0                     Allocated to stack - _bp +40
                                    542 ;------------------------------------------------------------
                                    543 ;	simpleserial.c:55: int simpleserial_get(uint8_t* input, uint8_t* key)
                                    544 ;	-----------------------------------------
                                    545 ;	 function simpleserial_get
                                    546 ;	-----------------------------------------
      0003A0                        547 _simpleserial_get:
      0003A0 C0 08            [24]  548 	push	_bp
      0003A2 85 81 08         [24]  549 	mov	_bp,sp
      0003A5 C0 82            [24]  550 	push	dpl
      0003A7 C0 83            [24]  551 	push	dph
      0003A9 C0 F0            [24]  552 	push	b
      0003AB E5 81            [12]  553 	mov	a,sp
      0003AD 24 22            [12]  554 	add	a,#0x22
      0003AF F5 81            [12]  555 	mov	sp,a
                                    556 ;	simpleserial.c:59: unsigned char ascii_pos = 0;
                                    557 ;	simpleserial.c:62: unsigned char state = STATE_IDLE;
                                    558 ;	simpleserial.c:65: while(1)
      0003B1 E4               [12]  559 	clr	a
      0003B2 FC               [12]  560 	mov	r4,a
      0003B3 FB               [12]  561 	mov	r3,a
      0003B4 E5 08            [12]  562 	mov	a,_bp
      0003B6 24 04            [12]  563 	add	a,#0x04
      0003B8 FA               [12]  564 	mov	r2,a
      0003B9                        565 00125$:
                                    566 ;	simpleserial.c:68: ascii_buffer[ascii_pos] = getchar();
      0003B9 EC               [12]  567 	mov	a,r4
      0003BA 2A               [12]  568 	add	a,r2
      0003BB F9               [12]  569 	mov	r1,a
      0003BC C0 04            [24]  570 	push	ar4
      0003BE C0 03            [24]  571 	push	ar3
      0003C0 C0 02            [24]  572 	push	ar2
      0003C2 C0 01            [24]  573 	push	ar1
      0003C4 12 00 A0         [24]  574 	lcall	_getchar
      0003C7 E5 82            [12]  575 	mov	a,dpl
      0003C9 D0 01            [24]  576 	pop	ar1
      0003CB D0 02            [24]  577 	pop	ar2
      0003CD D0 03            [24]  578 	pop	ar3
      0003CF D0 04            [24]  579 	pop	ar4
      0003D1 F7               [12]  580 	mov	@r1,a
                                    581 ;	simpleserial.c:70: if(state == STATE_IDLE)
      0003D2 EB               [12]  582 	mov	a,r3
      0003D3 70 19            [24]  583 	jnz	00122$
                                    584 ;	simpleserial.c:74: if(ascii_buffer[ascii_pos] == 'k')
      0003D5 87 07            [24]  585 	mov	ar7,@r1
      0003D7 BF 6B 05         [24]  586 	cjne	r7,#0x6b,00104$
                                    587 ;	simpleserial.c:76: state = STATE_KEY;
      0003DA 7B 02            [12]  588 	mov	r3,#0x02
                                    589 ;	simpleserial.c:77: ascii_pos++;
      0003DC 0C               [12]  590 	inc	r4
                                    591 ;	simpleserial.c:78: continue;
      0003DD 80 DA            [24]  592 	sjmp	00125$
      0003DF                        593 00104$:
                                    594 ;	simpleserial.c:80: else if(ascii_buffer[ascii_pos] == 'p')
      0003DF 87 07            [24]  595 	mov	ar7,@r1
      0003E1 BF 70 02         [24]  596 	cjne	r7,#0x70,00162$
      0003E4 80 03            [24]  597 	sjmp	00163$
      0003E6                        598 00162$:
      0003E6 02 04 76         [24]  599 	ljmp	00123$
      0003E9                        600 00163$:
                                    601 ;	simpleserial.c:82: state = STATE_PT;
      0003E9 7B 01            [12]  602 	mov	r3,#0x01
                                    603 ;	simpleserial.c:83: ascii_pos++;
      0003EB 0C               [12]  604 	inc	r4
                                    605 ;	simpleserial.c:84: continue;
      0003EC 80 CB            [24]  606 	sjmp	00125$
      0003EE                        607 00122$:
                                    608 ;	simpleserial.c:90: if(ascii_pos < (ASCII_LENGTH - 1))
      0003EE BC 21 00         [24]  609 	cjne	r4,#0x21,00164$
      0003F1                        610 00164$:
      0003F1 50 1F            [24]  611 	jnc	00119$
                                    612 ;	simpleserial.c:92: if((ascii_buffer[ascii_pos] >= '0' && ascii_buffer[ascii_pos] <= '9')
      0003F3 87 07            [24]  613 	mov	ar7,@r1
      0003F5 BF 30 00         [24]  614 	cjne	r7,#0x30,00166$
      0003F8                        615 00166$:
      0003F8 40 06            [24]  616 	jc	00110$
      0003FA E7               [12]  617 	mov	a,@r1
      0003FB FF               [12]  618 	mov	r7,a
      0003FC 24 C6            [12]  619 	add	a,#0xff - 0x39
      0003FE 50 0F            [24]  620 	jnc	00106$
      000400                        621 00110$:
                                    622 ;	simpleserial.c:93: || (ascii_buffer[ascii_pos] >= 'A' && ascii_buffer[ascii_pos] <= 'F'))
      000400 EC               [12]  623 	mov	a,r4
      000401 2A               [12]  624 	add	a,r2
      000402 F8               [12]  625 	mov	r0,a
      000403 86 07            [24]  626 	mov	ar7,@r0
      000405 BF 41 00         [24]  627 	cjne	r7,#0x41,00169$
      000408                        628 00169$:
      000408 40 6C            [24]  629 	jc	00123$
      00040A EF               [12]  630 	mov	a,r7
      00040B 24 B9            [12]  631 	add	a,#0xff - 0x46
      00040D 40 67            [24]  632 	jc	00123$
      00040F                        633 00106$:
                                    634 ;	simpleserial.c:95: ascii_pos++;
      00040F 0C               [12]  635 	inc	r4
                                    636 ;	simpleserial.c:96: continue;
      000410 80 A7            [24]  637 	sjmp	00125$
      000412                        638 00119$:
                                    639 ;	simpleserial.c:101: else if((ascii_pos == ASCII_LENGTH-1)
      000412 BC 21 61         [24]  640 	cjne	r4,#0x21,00123$
                                    641 ;	simpleserial.c:102: && ((ascii_buffer[ascii_pos] == '\n') || (ascii_buffer[ascii_pos] == '\r')))
      000415 87 04            [24]  642 	mov	ar4,@r1
      000417 BC 0A 02         [24]  643 	cjne	r4,#0x0a,00174$
      00041A 80 05            [24]  644 	sjmp	00114$
      00041C                        645 00174$:
      00041C 87 04            [24]  646 	mov	ar4,@r1
      00041E BC 0D 55         [24]  647 	cjne	r4,#0x0d,00123$
      000421                        648 00114$:
                                    649 ;	simpleserial.c:105: if(state == STATE_PT)
      000421 BB 01 2A         [24]  650 	cjne	r3,#0x01,00112$
                                    651 ;	simpleserial.c:107: simpleserial_decode(ascii_buffer, input);
      000424 8A 04            [24]  652 	mov	ar4,r2
      000426 8C 03            [24]  653 	mov	ar3,r4
      000428 7C 00            [12]  654 	mov	r4,#0x00
      00042A 7F 40            [12]  655 	mov	r7,#0x40
      00042C A8 08            [24]  656 	mov	r0,_bp
      00042E 08               [12]  657 	inc	r0
      00042F E6               [12]  658 	mov	a,@r0
      000430 C0 E0            [24]  659 	push	acc
      000432 08               [12]  660 	inc	r0
      000433 E6               [12]  661 	mov	a,@r0
      000434 C0 E0            [24]  662 	push	acc
      000436 08               [12]  663 	inc	r0
      000437 E6               [12]  664 	mov	a,@r0
      000438 C0 E0            [24]  665 	push	acc
      00043A 8B 82            [24]  666 	mov	dpl,r3
      00043C 8C 83            [24]  667 	mov	dph,r4
      00043E 8F F0            [24]  668 	mov	b,r7
      000440 12 02 4A         [24]  669 	lcall	_simpleserial_decode
      000443 15 81            [12]  670 	dec	sp
      000445 15 81            [12]  671 	dec	sp
      000447 15 81            [12]  672 	dec	sp
                                    673 ;	simpleserial.c:108: return 1;
      000449 90 00 01         [24]  674 	mov	dptr,#0x0001
      00044C 80 2B            [24]  675 	sjmp	00127$
      00044E                        676 00112$:
                                    677 ;	simpleserial.c:112: simpleserial_decode(ascii_buffer, key);
      00044E 7F 00            [12]  678 	mov	r7,#0x00
      000450 7E 40            [12]  679 	mov	r6,#0x40
      000452 E5 08            [12]  680 	mov	a,_bp
      000454 24 FB            [12]  681 	add	a,#0xfb
      000456 F8               [12]  682 	mov	r0,a
      000457 E6               [12]  683 	mov	a,@r0
      000458 C0 E0            [24]  684 	push	acc
      00045A 08               [12]  685 	inc	r0
      00045B E6               [12]  686 	mov	a,@r0
      00045C C0 E0            [24]  687 	push	acc
      00045E 08               [12]  688 	inc	r0
      00045F E6               [12]  689 	mov	a,@r0
      000460 C0 E0            [24]  690 	push	acc
      000462 8A 82            [24]  691 	mov	dpl,r2
      000464 8F 83            [24]  692 	mov	dph,r7
      000466 8E F0            [24]  693 	mov	b,r6
      000468 12 02 4A         [24]  694 	lcall	_simpleserial_decode
      00046B 15 81            [12]  695 	dec	sp
      00046D 15 81            [12]  696 	dec	sp
      00046F 15 81            [12]  697 	dec	sp
                                    698 ;	simpleserial.c:113: return 0;
      000471 90 00 00         [24]  699 	mov	dptr,#0x0000
      000474 80 03            [24]  700 	sjmp	00127$
      000476                        701 00123$:
                                    702 ;	simpleserial.c:120: return 0;
      000476 90 00 00         [24]  703 	mov	dptr,#0x0000
      000479                        704 00127$:
      000479 85 08 81         [24]  705 	mov	sp,_bp
      00047C D0 08            [24]  706 	pop	_bp
      00047E 22               [24]  707 	ret
                                    708 ;------------------------------------------------------------
                                    709 ;Allocation info for local variables in function 'simpleserial_put'
                                    710 ;------------------------------------------------------------
                                    711 ;output                    Allocated to stack - _bp +1
                                    712 ;ascii_buffer              Allocated to stack - _bp +4
                                    713 ;ascii_pos                 Allocated to registers r7 
                                    714 ;------------------------------------------------------------
                                    715 ;	simpleserial.c:124: void simpleserial_put(uint8_t* output)
                                    716 ;	-----------------------------------------
                                    717 ;	 function simpleserial_put
                                    718 ;	-----------------------------------------
      00047F                        719 _simpleserial_put:
      00047F C0 08            [24]  720 	push	_bp
      000481 85 81 08         [24]  721 	mov	_bp,sp
      000484 C0 82            [24]  722 	push	dpl
      000486 C0 83            [24]  723 	push	dph
      000488 C0 F0            [24]  724 	push	b
      00048A E5 81            [12]  725 	mov	a,sp
      00048C 24 22            [12]  726 	add	a,#0x22
      00048E F5 81            [12]  727 	mov	sp,a
                                    728 ;	simpleserial.c:130: simpleserial_encode(ascii_buffer, output);
      000490 E5 08            [12]  729 	mov	a,_bp
      000492 24 04            [12]  730 	add	a,#0x04
      000494 FC               [12]  731 	mov	r4,a
      000495 FA               [12]  732 	mov	r2,a
      000496 7B 00            [12]  733 	mov	r3,#0x00
      000498 7F 40            [12]  734 	mov	r7,#0x40
      00049A C0 04            [24]  735 	push	ar4
      00049C A8 08            [24]  736 	mov	r0,_bp
      00049E 08               [12]  737 	inc	r0
      00049F E6               [12]  738 	mov	a,@r0
      0004A0 C0 E0            [24]  739 	push	acc
      0004A2 08               [12]  740 	inc	r0
      0004A3 E6               [12]  741 	mov	a,@r0
      0004A4 C0 E0            [24]  742 	push	acc
      0004A6 08               [12]  743 	inc	r0
      0004A7 E6               [12]  744 	mov	a,@r0
      0004A8 C0 E0            [24]  745 	push	acc
      0004AA 8A 82            [24]  746 	mov	dpl,r2
      0004AC 8B 83            [24]  747 	mov	dph,r3
      0004AE 8F F0            [24]  748 	mov	b,r7
      0004B0 12 01 5C         [24]  749 	lcall	_simpleserial_encode
      0004B3 15 81            [12]  750 	dec	sp
      0004B5 15 81            [12]  751 	dec	sp
      0004B7 15 81            [12]  752 	dec	sp
      0004B9 D0 04            [24]  753 	pop	ar4
                                    754 ;	simpleserial.c:131: for(ascii_pos = 0; ascii_pos < ASCII_LENGTH; ascii_pos++)
      0004BB 7F 00            [12]  755 	mov	r7,#0x00
      0004BD                        756 00102$:
                                    757 ;	simpleserial.c:133: putchar(ascii_buffer[ascii_pos]);
      0004BD EF               [12]  758 	mov	a,r7
      0004BE 2C               [12]  759 	add	a,r4
      0004BF F9               [12]  760 	mov	r1,a
      0004C0 87 82            [24]  761 	mov	dpl,@r1
      0004C2 C0 07            [24]  762 	push	ar7
      0004C4 C0 04            [24]  763 	push	ar4
      0004C6 12 00 7B         [24]  764 	lcall	_putchar
      0004C9 D0 04            [24]  765 	pop	ar4
      0004CB D0 07            [24]  766 	pop	ar7
                                    767 ;	simpleserial.c:131: for(ascii_pos = 0; ascii_pos < ASCII_LENGTH; ascii_pos++)
      0004CD 0F               [12]  768 	inc	r7
      0004CE BF 22 00         [24]  769 	cjne	r7,#0x22,00110$
      0004D1                        770 00110$:
      0004D1 40 EA            [24]  771 	jc	00102$
      0004D3 85 08 81         [24]  772 	mov	sp,_bp
      0004D6 D0 08            [24]  773 	pop	_bp
      0004D8 22               [24]  774 	ret
                                    775 	.area CSEG    (CODE)
                                    776 	.area CONST   (CODE)
      000591                        777 _hex_lookup:
      000591 30                     778 	.db #0x30	; 48	'0'
      000592 31                     779 	.db #0x31	; 49	'1'
      000593 32                     780 	.db #0x32	; 50	'2'
      000594 33                     781 	.db #0x33	; 51	'3'
      000595 34                     782 	.db #0x34	; 52	'4'
      000596 35                     783 	.db #0x35	; 53	'5'
      000597 36                     784 	.db #0x36	; 54	'6'
      000598 37                     785 	.db #0x37	; 55	'7'
      000599 38                     786 	.db #0x38	; 56	'8'
      00059A 39                     787 	.db #0x39	; 57	'9'
      00059B 41                     788 	.db #0x41	; 65	'A'
      00059C 42                     789 	.db #0x42	; 66	'B'
      00059D 43                     790 	.db #0x43	; 67	'C'
      00059E 44                     791 	.db #0x44	; 68	'D'
      00059F 45                     792 	.db #0x45	; 69	'E'
      0005A0 46                     793 	.db #0x46	; 70	'F'
                                    794 	.area XINIT   (CODE)
                                    795 	.area CABS    (ABS,CODE)
