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
      000B32                        107 _simpleserial_encode:
                           000007   108 	ar7 = 0x07
                           000006   109 	ar6 = 0x06
                           000005   110 	ar5 = 0x05
                           000004   111 	ar4 = 0x04
                           000003   112 	ar3 = 0x03
                           000002   113 	ar2 = 0x02
                           000001   114 	ar1 = 0x01
                           000000   115 	ar0 = 0x00
      000B32 C0 08            [24]  116 	push	_bp
      000B34 85 81 08         [24]  117 	mov	_bp,sp
      000B37 C0 82            [24]  118 	push	dpl
      000B39 C0 83            [24]  119 	push	dph
      000B3B C0 F0            [24]  120 	push	b
      000B3D 05 81            [12]  121 	inc	sp
      000B3F 05 81            [12]  122 	inc	sp
      000B41 05 81            [12]  123 	inc	sp
                                    124 ;	simpleserial.c:22: ascii[0] = 'r';
      000B43 A8 08            [24]  125 	mov	r0,_bp
      000B45 08               [12]  126 	inc	r0
      000B46 86 82            [24]  127 	mov	dpl,@r0
      000B48 08               [12]  128 	inc	r0
      000B49 86 83            [24]  129 	mov	dph,@r0
      000B4B 08               [12]  130 	inc	r0
      000B4C 86 F0            [24]  131 	mov	b,@r0
      000B4E 74 72            [12]  132 	mov	a,#0x72
      000B50 12 0F 31         [24]  133 	lcall	__gptrput
                                    134 ;	simpleserial.c:23: for(i = 0; i < BLOCK_LENGTH; i++)
      000B53 E5 08            [12]  135 	mov	a,_bp
      000B55 24 04            [12]  136 	add	a,#0x04
      000B57 F8               [12]  137 	mov	r0,a
      000B58 E4               [12]  138 	clr	a
      000B59 F6               [12]  139 	mov	@r0,a
      000B5A 08               [12]  140 	inc	r0
      000B5B F6               [12]  141 	mov	@r0,a
      000B5C                        142 00102$:
                                    143 ;	simpleserial.c:25: unsigned char n_hi = (data[i] >> 4) & 0x0f;
      000B5C E5 08            [12]  144 	mov	a,_bp
      000B5E 24 FB            [12]  145 	add	a,#0xfb
      000B60 F8               [12]  146 	mov	r0,a
      000B61 E5 08            [12]  147 	mov	a,_bp
      000B63 24 04            [12]  148 	add	a,#0x04
      000B65 F9               [12]  149 	mov	r1,a
      000B66 E7               [12]  150 	mov	a,@r1
      000B67 26               [12]  151 	add	a,@r0
      000B68 FA               [12]  152 	mov	r2,a
      000B69 09               [12]  153 	inc	r1
      000B6A E7               [12]  154 	mov	a,@r1
      000B6B 08               [12]  155 	inc	r0
      000B6C 36               [12]  156 	addc	a,@r0
      000B6D FB               [12]  157 	mov	r3,a
      000B6E 08               [12]  158 	inc	r0
      000B6F 86 04            [24]  159 	mov	ar4,@r0
      000B71 8A 82            [24]  160 	mov	dpl,r2
      000B73 8B 83            [24]  161 	mov	dph,r3
      000B75 8C F0            [24]  162 	mov	b,r4
      000B77 12 0F 4C         [24]  163 	lcall	__gptrget
      000B7A FA               [12]  164 	mov	r2,a
      000B7B C4               [12]  165 	swap	a
      000B7C 54 0F            [12]  166 	anl	a,#0x0f
      000B7E FD               [12]  167 	mov	r5,a
                                    168 ;	simpleserial.c:26: unsigned char n_lo =  data[i]       & 0x0f;
      000B7F E5 08            [12]  169 	mov	a,_bp
      000B81 24 06            [12]  170 	add	a,#0x06
      000B83 F8               [12]  171 	mov	r0,a
      000B84 74 0F            [12]  172 	mov	a,#0x0f
      000B86 5A               [12]  173 	anl	a,r2
      000B87 F6               [12]  174 	mov	@r0,a
                                    175 ;	simpleserial.c:27: ascii[2*i+1] = hex_lookup[n_hi];
      000B88 E5 08            [12]  176 	mov	a,_bp
      000B8A 24 04            [12]  177 	add	a,#0x04
      000B8C F8               [12]  178 	mov	r0,a
      000B8D 86 02            [24]  179 	mov	ar2,@r0
      000B8F 08               [12]  180 	inc	r0
      000B90 E6               [12]  181 	mov	a,@r0
      000B91 CA               [12]  182 	xch	a,r2
      000B92 25 E0            [12]  183 	add	a,acc
      000B94 CA               [12]  184 	xch	a,r2
      000B95 33               [12]  185 	rlc	a
      000B96 FB               [12]  186 	mov	r3,a
      000B97 74 01            [12]  187 	mov	a,#0x01
      000B99 2A               [12]  188 	add	a,r2
      000B9A FC               [12]  189 	mov	r4,a
      000B9B E4               [12]  190 	clr	a
      000B9C 3B               [12]  191 	addc	a,r3
      000B9D FF               [12]  192 	mov	r7,a
      000B9E A8 08            [24]  193 	mov	r0,_bp
      000BA0 08               [12]  194 	inc	r0
      000BA1 EC               [12]  195 	mov	a,r4
      000BA2 26               [12]  196 	add	a,@r0
      000BA3 FC               [12]  197 	mov	r4,a
      000BA4 EF               [12]  198 	mov	a,r7
      000BA5 08               [12]  199 	inc	r0
      000BA6 36               [12]  200 	addc	a,@r0
      000BA7 FF               [12]  201 	mov	r7,a
      000BA8 08               [12]  202 	inc	r0
      000BA9 86 06            [24]  203 	mov	ar6,@r0
      000BAB ED               [12]  204 	mov	a,r5
      000BAC 90 10 77         [24]  205 	mov	dptr,#_hex_lookup
      000BAF 93               [24]  206 	movc	a,@a+dptr
      000BB0 FD               [12]  207 	mov	r5,a
      000BB1 8C 82            [24]  208 	mov	dpl,r4
      000BB3 8F 83            [24]  209 	mov	dph,r7
      000BB5 8E F0            [24]  210 	mov	b,r6
      000BB7 12 0F 31         [24]  211 	lcall	__gptrput
                                    212 ;	simpleserial.c:28: ascii[2*i+2] = hex_lookup[n_lo];
      000BBA 74 02            [12]  213 	mov	a,#0x02
      000BBC 2A               [12]  214 	add	a,r2
      000BBD FA               [12]  215 	mov	r2,a
      000BBE E4               [12]  216 	clr	a
      000BBF 3B               [12]  217 	addc	a,r3
      000BC0 FB               [12]  218 	mov	r3,a
      000BC1 A8 08            [24]  219 	mov	r0,_bp
      000BC3 08               [12]  220 	inc	r0
      000BC4 EA               [12]  221 	mov	a,r2
      000BC5 26               [12]  222 	add	a,@r0
      000BC6 FA               [12]  223 	mov	r2,a
      000BC7 EB               [12]  224 	mov	a,r3
      000BC8 08               [12]  225 	inc	r0
      000BC9 36               [12]  226 	addc	a,@r0
      000BCA FB               [12]  227 	mov	r3,a
      000BCB 08               [12]  228 	inc	r0
      000BCC 86 07            [24]  229 	mov	ar7,@r0
      000BCE E5 08            [12]  230 	mov	a,_bp
      000BD0 24 06            [12]  231 	add	a,#0x06
      000BD2 F8               [12]  232 	mov	r0,a
      000BD3 E6               [12]  233 	mov	a,@r0
      000BD4 90 10 77         [24]  234 	mov	dptr,#_hex_lookup
      000BD7 93               [24]  235 	movc	a,@a+dptr
      000BD8 FE               [12]  236 	mov	r6,a
      000BD9 8A 82            [24]  237 	mov	dpl,r2
      000BDB 8B 83            [24]  238 	mov	dph,r3
      000BDD 8F F0            [24]  239 	mov	b,r7
      000BDF 12 0F 31         [24]  240 	lcall	__gptrput
                                    241 ;	simpleserial.c:23: for(i = 0; i < BLOCK_LENGTH; i++)
      000BE2 E5 08            [12]  242 	mov	a,_bp
      000BE4 24 04            [12]  243 	add	a,#0x04
      000BE6 F8               [12]  244 	mov	r0,a
      000BE7 06               [12]  245 	inc	@r0
      000BE8 B6 00 02         [24]  246 	cjne	@r0,#0x00,00113$
      000BEB 08               [12]  247 	inc	r0
      000BEC 06               [12]  248 	inc	@r0
      000BED                        249 00113$:
      000BED E5 08            [12]  250 	mov	a,_bp
      000BEF 24 04            [12]  251 	add	a,#0x04
      000BF1 F8               [12]  252 	mov	r0,a
      000BF2 C3               [12]  253 	clr	c
      000BF3 E6               [12]  254 	mov	a,@r0
      000BF4 94 10            [12]  255 	subb	a,#0x10
      000BF6 08               [12]  256 	inc	r0
      000BF7 E6               [12]  257 	mov	a,@r0
      000BF8 64 80            [12]  258 	xrl	a,#0x80
      000BFA 94 80            [12]  259 	subb	a,#0x80
      000BFC 50 03            [24]  260 	jnc	00114$
      000BFE 02 0B 5C         [24]  261 	ljmp	00102$
      000C01                        262 00114$:
                                    263 ;	simpleserial.c:30: ascii[ASCII_LENGTH-1] = '\n';
      000C01 A8 08            [24]  264 	mov	r0,_bp
      000C03 08               [12]  265 	inc	r0
      000C04 74 21            [12]  266 	mov	a,#0x21
      000C06 26               [12]  267 	add	a,@r0
      000C07 FD               [12]  268 	mov	r5,a
      000C08 E4               [12]  269 	clr	a
      000C09 08               [12]  270 	inc	r0
      000C0A 36               [12]  271 	addc	a,@r0
      000C0B FE               [12]  272 	mov	r6,a
      000C0C 08               [12]  273 	inc	r0
      000C0D 86 07            [24]  274 	mov	ar7,@r0
      000C0F 8D 82            [24]  275 	mov	dpl,r5
      000C11 8E 83            [24]  276 	mov	dph,r6
      000C13 8F F0            [24]  277 	mov	b,r7
      000C15 74 0A            [12]  278 	mov	a,#0x0a
      000C17 12 0F 31         [24]  279 	lcall	__gptrput
      000C1A 85 08 81         [24]  280 	mov	sp,_bp
      000C1D D0 08            [24]  281 	pop	_bp
      000C1F 22               [24]  282 	ret
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
      000C20                        296 _simpleserial_decode:
      000C20 C0 08            [24]  297 	push	_bp
      000C22 85 81 08         [24]  298 	mov	_bp,sp
      000C25 C0 82            [24]  299 	push	dpl
      000C27 C0 83            [24]  300 	push	dph
      000C29 C0 F0            [24]  301 	push	b
      000C2B 05 81            [12]  302 	inc	sp
      000C2D 05 81            [12]  303 	inc	sp
                                    304 ;	simpleserial.c:38: for(i = 0; i < BLOCK_LENGTH; i++)
      000C2F 7B 00            [12]  305 	mov	r3,#0x00
      000C31 7C 00            [12]  306 	mov	r4,#0x00
      000C33                        307 00114$:
                                    308 ;	simpleserial.c:40: unsigned char n_hi = ascii[2*i + 1];
      000C33 8B 02            [24]  309 	mov	ar2,r3
      000C35 EC               [12]  310 	mov	a,r4
      000C36 CA               [12]  311 	xch	a,r2
      000C37 25 E0            [12]  312 	add	a,acc
      000C39 CA               [12]  313 	xch	a,r2
      000C3A 33               [12]  314 	rlc	a
      000C3B FF               [12]  315 	mov	r7,a
      000C3C 74 01            [12]  316 	mov	a,#0x01
      000C3E 2A               [12]  317 	add	a,r2
      000C3F FD               [12]  318 	mov	r5,a
      000C40 E4               [12]  319 	clr	a
      000C41 3F               [12]  320 	addc	a,r7
      000C42 FE               [12]  321 	mov	r6,a
      000C43 C0 03            [24]  322 	push	ar3
      000C45 C0 04            [24]  323 	push	ar4
      000C47 A8 08            [24]  324 	mov	r0,_bp
      000C49 08               [12]  325 	inc	r0
      000C4A ED               [12]  326 	mov	a,r5
      000C4B 26               [12]  327 	add	a,@r0
      000C4C FD               [12]  328 	mov	r5,a
      000C4D EE               [12]  329 	mov	a,r6
      000C4E 08               [12]  330 	inc	r0
      000C4F 36               [12]  331 	addc	a,@r0
      000C50 FC               [12]  332 	mov	r4,a
      000C51 08               [12]  333 	inc	r0
      000C52 86 06            [24]  334 	mov	ar6,@r0
      000C54 8D 82            [24]  335 	mov	dpl,r5
      000C56 8C 83            [24]  336 	mov	dph,r4
      000C58 8E F0            [24]  337 	mov	b,r6
      000C5A E5 08            [12]  338 	mov	a,_bp
      000C5C 24 05            [12]  339 	add	a,#0x05
      000C5E F8               [12]  340 	mov	r0,a
      000C5F 12 0F 4C         [24]  341 	lcall	__gptrget
      000C62 F6               [12]  342 	mov	@r0,a
                                    343 ;	simpleserial.c:41: unsigned char n_lo = ascii[2*i + 2];
      000C63 74 02            [12]  344 	mov	a,#0x02
      000C65 2A               [12]  345 	add	a,r2
      000C66 FA               [12]  346 	mov	r2,a
      000C67 E4               [12]  347 	clr	a
      000C68 3F               [12]  348 	addc	a,r7
      000C69 FF               [12]  349 	mov	r7,a
      000C6A A8 08            [24]  350 	mov	r0,_bp
      000C6C 08               [12]  351 	inc	r0
      000C6D EA               [12]  352 	mov	a,r2
      000C6E 26               [12]  353 	add	a,@r0
      000C6F FA               [12]  354 	mov	r2,a
      000C70 EF               [12]  355 	mov	a,r7
      000C71 08               [12]  356 	inc	r0
      000C72 36               [12]  357 	addc	a,@r0
      000C73 FF               [12]  358 	mov	r7,a
      000C74 08               [12]  359 	inc	r0
      000C75 86 06            [24]  360 	mov	ar6,@r0
      000C77 8A 82            [24]  361 	mov	dpl,r2
      000C79 8F 83            [24]  362 	mov	dph,r7
      000C7B 8E F0            [24]  363 	mov	b,r6
      000C7D E5 08            [12]  364 	mov	a,_bp
      000C7F 24 04            [12]  365 	add	a,#0x04
      000C81 F8               [12]  366 	mov	r0,a
      000C82 12 0F 4C         [24]  367 	lcall	__gptrget
      000C85 F6               [12]  368 	mov	@r0,a
                                    369 ;	simpleserial.c:43: if(n_lo <= '9')
      000C86 E5 08            [12]  370 	mov	a,_bp
      000C88 24 04            [12]  371 	add	a,#0x04
      000C8A F8               [12]  372 	mov	r0,a
      000C8B C3               [12]  373 	clr	c
      000C8C 74 39            [12]  374 	mov	a,#0x39
      000C8E 96               [12]  375 	subb	a,@r0
      000C8F D0 04            [24]  376 	pop	ar4
      000C91 D0 03            [24]  377 	pop	ar3
      000C93 40 23            [24]  378 	jc	00105$
                                    379 ;	simpleserial.c:44: data[i] = n_lo - '0';
      000C95 E5 08            [12]  380 	mov	a,_bp
      000C97 24 FB            [12]  381 	add	a,#0xfb
      000C99 F8               [12]  382 	mov	r0,a
      000C9A EB               [12]  383 	mov	a,r3
      000C9B 26               [12]  384 	add	a,@r0
      000C9C FD               [12]  385 	mov	r5,a
      000C9D EC               [12]  386 	mov	a,r4
      000C9E 08               [12]  387 	inc	r0
      000C9F 36               [12]  388 	addc	a,@r0
      000CA0 FE               [12]  389 	mov	r6,a
      000CA1 08               [12]  390 	inc	r0
      000CA2 86 07            [24]  391 	mov	ar7,@r0
      000CA4 E5 08            [12]  392 	mov	a,_bp
      000CA6 24 04            [12]  393 	add	a,#0x04
      000CA8 F8               [12]  394 	mov	r0,a
      000CA9 E6               [12]  395 	mov	a,@r0
      000CAA 24 D0            [12]  396 	add	a,#0xd0
      000CAC FA               [12]  397 	mov	r2,a
      000CAD 8D 82            [24]  398 	mov	dpl,r5
      000CAF 8E 83            [24]  399 	mov	dph,r6
      000CB1 8F F0            [24]  400 	mov	b,r7
      000CB3 12 0F 31         [24]  401 	lcall	__gptrput
      000CB6 80 52            [24]  402 	sjmp	00106$
      000CB8                        403 00105$:
                                    404 ;	simpleserial.c:45: else if(n_lo <= 'F')
      000CB8 E5 08            [12]  405 	mov	a,_bp
      000CBA 24 04            [12]  406 	add	a,#0x04
      000CBC F8               [12]  407 	mov	r0,a
      000CBD E6               [12]  408 	mov	a,@r0
      000CBE 24 B9            [12]  409 	add	a,#0xff - 0x46
      000CC0 40 25            [24]  410 	jc	00102$
                                    411 ;	simpleserial.c:46: data[i] = n_lo - 'A' + 10;
      000CC2 E5 08            [12]  412 	mov	a,_bp
      000CC4 24 FB            [12]  413 	add	a,#0xfb
      000CC6 F8               [12]  414 	mov	r0,a
      000CC7 EB               [12]  415 	mov	a,r3
      000CC8 26               [12]  416 	add	a,@r0
      000CC9 FD               [12]  417 	mov	r5,a
      000CCA EC               [12]  418 	mov	a,r4
      000CCB 08               [12]  419 	inc	r0
      000CCC 36               [12]  420 	addc	a,@r0
      000CCD FE               [12]  421 	mov	r6,a
      000CCE 08               [12]  422 	inc	r0
      000CCF 86 07            [24]  423 	mov	ar7,@r0
      000CD1 E5 08            [12]  424 	mov	a,_bp
      000CD3 24 04            [12]  425 	add	a,#0x04
      000CD5 F8               [12]  426 	mov	r0,a
      000CD6 86 02            [24]  427 	mov	ar2,@r0
      000CD8 74 C9            [12]  428 	mov	a,#0xc9
      000CDA 2A               [12]  429 	add	a,r2
      000CDB FA               [12]  430 	mov	r2,a
      000CDC 8D 82            [24]  431 	mov	dpl,r5
      000CDE 8E 83            [24]  432 	mov	dph,r6
      000CE0 8F F0            [24]  433 	mov	b,r7
      000CE2 12 0F 31         [24]  434 	lcall	__gptrput
      000CE5 80 23            [24]  435 	sjmp	00106$
      000CE7                        436 00102$:
                                    437 ;	simpleserial.c:48: data[i] = n_lo - 'a' + 10;
      000CE7 E5 08            [12]  438 	mov	a,_bp
      000CE9 24 FB            [12]  439 	add	a,#0xfb
      000CEB F8               [12]  440 	mov	r0,a
      000CEC EB               [12]  441 	mov	a,r3
      000CED 26               [12]  442 	add	a,@r0
      000CEE FD               [12]  443 	mov	r5,a
      000CEF EC               [12]  444 	mov	a,r4
      000CF0 08               [12]  445 	inc	r0
      000CF1 36               [12]  446 	addc	a,@r0
      000CF2 FE               [12]  447 	mov	r6,a
      000CF3 08               [12]  448 	inc	r0
      000CF4 86 07            [24]  449 	mov	ar7,@r0
      000CF6 E5 08            [12]  450 	mov	a,_bp
      000CF8 24 04            [12]  451 	add	a,#0x04
      000CFA F8               [12]  452 	mov	r0,a
      000CFB 86 02            [24]  453 	mov	ar2,@r0
      000CFD 74 A9            [12]  454 	mov	a,#0xa9
      000CFF 2A               [12]  455 	add	a,r2
      000D00 FA               [12]  456 	mov	r2,a
      000D01 8D 82            [24]  457 	mov	dpl,r5
      000D03 8E 83            [24]  458 	mov	dph,r6
      000D05 8F F0            [24]  459 	mov	b,r7
      000D07 12 0F 31         [24]  460 	lcall	__gptrput
      000D0A                        461 00106$:
                                    462 ;	simpleserial.c:50: if(n_hi <= '9')
      000D0A E5 08            [12]  463 	mov	a,_bp
      000D0C 24 05            [12]  464 	add	a,#0x05
      000D0E F8               [12]  465 	mov	r0,a
      000D0F E6               [12]  466 	mov	a,@r0
      000D10 24 C6            [12]  467 	add	a,#0xff - 0x39
      000D12 40 3D            [24]  468 	jc	00111$
                                    469 ;	simpleserial.c:51: data[i] |= (n_hi - '0') << 4;
      000D14 E5 08            [12]  470 	mov	a,_bp
      000D16 24 FB            [12]  471 	add	a,#0xfb
      000D18 F8               [12]  472 	mov	r0,a
      000D19 EB               [12]  473 	mov	a,r3
      000D1A 26               [12]  474 	add	a,@r0
      000D1B FD               [12]  475 	mov	r5,a
      000D1C EC               [12]  476 	mov	a,r4
      000D1D 08               [12]  477 	inc	r0
      000D1E 36               [12]  478 	addc	a,@r0
      000D1F FE               [12]  479 	mov	r6,a
      000D20 08               [12]  480 	inc	r0
      000D21 86 07            [24]  481 	mov	ar7,@r0
      000D23 8D 82            [24]  482 	mov	dpl,r5
      000D25 8E 83            [24]  483 	mov	dph,r6
      000D27 8F F0            [24]  484 	mov	b,r7
      000D29 12 0F 4C         [24]  485 	lcall	__gptrget
      000D2C FA               [12]  486 	mov	r2,a
      000D2D C0 03            [24]  487 	push	ar3
      000D2F C0 04            [24]  488 	push	ar4
      000D31 E5 08            [12]  489 	mov	a,_bp
      000D33 24 05            [12]  490 	add	a,#0x05
      000D35 F8               [12]  491 	mov	r0,a
      000D36 E6               [12]  492 	mov	a,@r0
      000D37 24 D0            [12]  493 	add	a,#0xd0
      000D39 C4               [12]  494 	swap	a
      000D3A 54 F0            [12]  495 	anl	a,#0xf0
      000D3C FC               [12]  496 	mov	r4,a
      000D3D EA               [12]  497 	mov	a,r2
      000D3E 42 04            [12]  498 	orl	ar4,a
      000D40 8D 82            [24]  499 	mov	dpl,r5
      000D42 8E 83            [24]  500 	mov	dph,r6
      000D44 8F F0            [24]  501 	mov	b,r7
      000D46 EC               [12]  502 	mov	a,r4
      000D47 12 0F 31         [24]  503 	lcall	__gptrput
      000D4A D0 04            [24]  504 	pop	ar4
      000D4C D0 03            [24]  505 	pop	ar3
      000D4E 02 0D D5         [24]  506 	ljmp	00115$
      000D51                        507 00111$:
                                    508 ;	simpleserial.c:52: else if(n_hi <= 'F')
      000D51 E5 08            [12]  509 	mov	a,_bp
      000D53 24 05            [12]  510 	add	a,#0x05
      000D55 F8               [12]  511 	mov	r0,a
      000D56 E6               [12]  512 	mov	a,@r0
      000D57 24 B9            [12]  513 	add	a,#0xff - 0x46
      000D59 40 3E            [24]  514 	jc	00108$
                                    515 ;	simpleserial.c:53: data[i] |= (n_hi - 'A' + 10) << 4;
      000D5B E5 08            [12]  516 	mov	a,_bp
      000D5D 24 FB            [12]  517 	add	a,#0xfb
      000D5F F8               [12]  518 	mov	r0,a
      000D60 EB               [12]  519 	mov	a,r3
      000D61 26               [12]  520 	add	a,@r0
      000D62 FD               [12]  521 	mov	r5,a
      000D63 EC               [12]  522 	mov	a,r4
      000D64 08               [12]  523 	inc	r0
      000D65 36               [12]  524 	addc	a,@r0
      000D66 FE               [12]  525 	mov	r6,a
      000D67 08               [12]  526 	inc	r0
      000D68 86 07            [24]  527 	mov	ar7,@r0
      000D6A 8D 82            [24]  528 	mov	dpl,r5
      000D6C 8E 83            [24]  529 	mov	dph,r6
      000D6E 8F F0            [24]  530 	mov	b,r7
      000D70 12 0F 4C         [24]  531 	lcall	__gptrget
      000D73 FA               [12]  532 	mov	r2,a
      000D74 C0 03            [24]  533 	push	ar3
      000D76 C0 04            [24]  534 	push	ar4
      000D78 E5 08            [12]  535 	mov	a,_bp
      000D7A 24 05            [12]  536 	add	a,#0x05
      000D7C F8               [12]  537 	mov	r0,a
      000D7D 86 04            [24]  538 	mov	ar4,@r0
      000D7F 74 C9            [12]  539 	mov	a,#0xc9
      000D81 2C               [12]  540 	add	a,r4
      000D82 C4               [12]  541 	swap	a
      000D83 54 F0            [12]  542 	anl	a,#0xf0
      000D85 FC               [12]  543 	mov	r4,a
      000D86 EA               [12]  544 	mov	a,r2
      000D87 42 04            [12]  545 	orl	ar4,a
      000D89 8D 82            [24]  546 	mov	dpl,r5
      000D8B 8E 83            [24]  547 	mov	dph,r6
      000D8D 8F F0            [24]  548 	mov	b,r7
      000D8F EC               [12]  549 	mov	a,r4
      000D90 12 0F 31         [24]  550 	lcall	__gptrput
      000D93 D0 04            [24]  551 	pop	ar4
      000D95 D0 03            [24]  552 	pop	ar3
      000D97 80 3C            [24]  553 	sjmp	00115$
      000D99                        554 00108$:
                                    555 ;	simpleserial.c:55: data[i] |= (n_hi - 'a' + 10) << 4;
      000D99 E5 08            [12]  556 	mov	a,_bp
      000D9B 24 FB            [12]  557 	add	a,#0xfb
      000D9D F8               [12]  558 	mov	r0,a
      000D9E EB               [12]  559 	mov	a,r3
      000D9F 26               [12]  560 	add	a,@r0
      000DA0 FD               [12]  561 	mov	r5,a
      000DA1 EC               [12]  562 	mov	a,r4
      000DA2 08               [12]  563 	inc	r0
      000DA3 36               [12]  564 	addc	a,@r0
      000DA4 FE               [12]  565 	mov	r6,a
      000DA5 08               [12]  566 	inc	r0
      000DA6 86 07            [24]  567 	mov	ar7,@r0
      000DA8 8D 82            [24]  568 	mov	dpl,r5
      000DAA 8E 83            [24]  569 	mov	dph,r6
      000DAC 8F F0            [24]  570 	mov	b,r7
      000DAE 12 0F 4C         [24]  571 	lcall	__gptrget
      000DB1 FA               [12]  572 	mov	r2,a
      000DB2 C0 03            [24]  573 	push	ar3
      000DB4 C0 04            [24]  574 	push	ar4
      000DB6 E5 08            [12]  575 	mov	a,_bp
      000DB8 24 05            [12]  576 	add	a,#0x05
      000DBA F8               [12]  577 	mov	r0,a
      000DBB 86 04            [24]  578 	mov	ar4,@r0
      000DBD 74 A9            [12]  579 	mov	a,#0xa9
      000DBF 2C               [12]  580 	add	a,r4
      000DC0 C4               [12]  581 	swap	a
      000DC1 54 F0            [12]  582 	anl	a,#0xf0
      000DC3 FC               [12]  583 	mov	r4,a
      000DC4 EA               [12]  584 	mov	a,r2
      000DC5 42 04            [12]  585 	orl	ar4,a
      000DC7 8D 82            [24]  586 	mov	dpl,r5
      000DC9 8E 83            [24]  587 	mov	dph,r6
      000DCB 8F F0            [24]  588 	mov	b,r7
      000DCD EC               [12]  589 	mov	a,r4
      000DCE 12 0F 31         [24]  590 	lcall	__gptrput
                                    591 ;	simpleserial.c:38: for(i = 0; i < BLOCK_LENGTH; i++)
      000DD1 D0 04            [24]  592 	pop	ar4
      000DD3 D0 03            [24]  593 	pop	ar3
                                    594 ;	simpleserial.c:55: data[i] |= (n_hi - 'a' + 10) << 4;
      000DD5                        595 00115$:
                                    596 ;	simpleserial.c:38: for(i = 0; i < BLOCK_LENGTH; i++)
      000DD5 0B               [12]  597 	inc	r3
      000DD6 BB 00 01         [24]  598 	cjne	r3,#0x00,00138$
      000DD9 0C               [12]  599 	inc	r4
      000DDA                        600 00138$:
      000DDA C3               [12]  601 	clr	c
      000DDB EB               [12]  602 	mov	a,r3
      000DDC 94 10            [12]  603 	subb	a,#0x10
      000DDE EC               [12]  604 	mov	a,r4
      000DDF 64 80            [12]  605 	xrl	a,#0x80
      000DE1 94 80            [12]  606 	subb	a,#0x80
      000DE3 50 03            [24]  607 	jnc	00139$
      000DE5 02 0C 33         [24]  608 	ljmp	00114$
      000DE8                        609 00139$:
      000DE8 85 08 81         [24]  610 	mov	sp,_bp
      000DEB D0 08            [24]  611 	pop	_bp
      000DED 22               [24]  612 	ret
                                    613 ;------------------------------------------------------------
                                    614 ;Allocation info for local variables in function 'simpleserial_get'
                                    615 ;------------------------------------------------------------
                                    616 ;key                       Allocated to stack - _bp -5
                                    617 ;input                     Allocated to stack - _bp +1
                                    618 ;ascii_buffer              Allocated to stack - _bp +4
                                    619 ;ascii_pos                 Allocated to registers r4 
                                    620 ;state                     Allocated to registers r3 
                                    621 ;sloc0                     Allocated to stack - _bp +40
                                    622 ;------------------------------------------------------------
                                    623 ;	simpleserial.c:59: int simpleserial_get(uint8_t* input, uint8_t* key)
                                    624 ;	-----------------------------------------
                                    625 ;	 function simpleserial_get
                                    626 ;	-----------------------------------------
      000DEE                        627 _simpleserial_get:
      000DEE C0 08            [24]  628 	push	_bp
      000DF0 85 81 08         [24]  629 	mov	_bp,sp
      000DF3 C0 82            [24]  630 	push	dpl
      000DF5 C0 83            [24]  631 	push	dph
      000DF7 C0 F0            [24]  632 	push	b
      000DF9 E5 81            [12]  633 	mov	a,sp
      000DFB 24 22            [12]  634 	add	a,#0x22
      000DFD F5 81            [12]  635 	mov	sp,a
                                    636 ;	simpleserial.c:63: unsigned char ascii_pos = 0;
                                    637 ;	simpleserial.c:66: unsigned char state = STATE_IDLE;
                                    638 ;	simpleserial.c:69: while(1)
      000DFF E4               [12]  639 	clr	a
      000E00 FC               [12]  640 	mov	r4,a
      000E01 FB               [12]  641 	mov	r3,a
      000E02 E5 08            [12]  642 	mov	a,_bp
      000E04 24 04            [12]  643 	add	a,#0x04
      000E06 FA               [12]  644 	mov	r2,a
      000E07                        645 00127$:
                                    646 ;	simpleserial.c:72: ascii_buffer[ascii_pos] = getchar();
      000E07 EC               [12]  647 	mov	a,r4
      000E08 2A               [12]  648 	add	a,r2
      000E09 F9               [12]  649 	mov	r1,a
      000E0A C0 04            [24]  650 	push	ar4
      000E0C C0 03            [24]  651 	push	ar3
      000E0E C0 02            [24]  652 	push	ar2
      000E10 C0 01            [24]  653 	push	ar1
      000E12 12 00 A0         [24]  654 	lcall	_getchar
      000E15 E5 82            [12]  655 	mov	a,dpl
      000E17 D0 01            [24]  656 	pop	ar1
      000E19 D0 02            [24]  657 	pop	ar2
      000E1B D0 03            [24]  658 	pop	ar3
      000E1D D0 04            [24]  659 	pop	ar4
      000E1F F7               [12]  660 	mov	@r1,a
                                    661 ;	simpleserial.c:74: if(state == STATE_IDLE)
      000E20 EB               [12]  662 	mov	a,r3
      000E21 70 19            [24]  663 	jnz	00124$
                                    664 ;	simpleserial.c:78: if(ascii_buffer[ascii_pos] == 'k')
      000E23 87 07            [24]  665 	mov	ar7,@r1
      000E25 BF 6B 05         [24]  666 	cjne	r7,#0x6b,00104$
                                    667 ;	simpleserial.c:80: state = STATE_KEY;
      000E28 7B 02            [12]  668 	mov	r3,#0x02
                                    669 ;	simpleserial.c:81: ascii_pos++;
      000E2A 0C               [12]  670 	inc	r4
                                    671 ;	simpleserial.c:82: continue;
      000E2B 80 DA            [24]  672 	sjmp	00127$
      000E2D                        673 00104$:
                                    674 ;	simpleserial.c:84: else if(ascii_buffer[ascii_pos] == 'p')
      000E2D 87 07            [24]  675 	mov	ar7,@r1
      000E2F BF 70 02         [24]  676 	cjne	r7,#0x70,00167$
      000E32 80 03            [24]  677 	sjmp	00168$
      000E34                        678 00167$:
      000E34 02 0E CE         [24]  679 	ljmp	00125$
      000E37                        680 00168$:
                                    681 ;	simpleserial.c:86: state = STATE_PT;
      000E37 7B 01            [12]  682 	mov	r3,#0x01
                                    683 ;	simpleserial.c:87: ascii_pos++;
      000E39 0C               [12]  684 	inc	r4
                                    685 ;	simpleserial.c:88: continue;
      000E3A 80 CB            [24]  686 	sjmp	00127$
      000E3C                        687 00124$:
                                    688 ;	simpleserial.c:94: if(ascii_pos < (ASCII_LENGTH - 1))
      000E3C BC 21 00         [24]  689 	cjne	r4,#0x21,00169$
      000E3F                        690 00169$:
      000E3F 50 29            [24]  691 	jnc	00121$
                                    692 ;	simpleserial.c:96: if((ascii_buffer[ascii_pos] >= '0' && ascii_buffer[ascii_pos] <= '9')
      000E41 87 07            [24]  693 	mov	ar7,@r1
      000E43 BF 30 00         [24]  694 	cjne	r7,#0x30,00171$
      000E46                        695 00171$:
      000E46 40 06            [24]  696 	jc	00110$
      000E48 E7               [12]  697 	mov	a,@r1
      000E49 FF               [12]  698 	mov	r7,a
      000E4A 24 C6            [12]  699 	add	a,#0xff - 0x39
      000E4C 50 19            [24]  700 	jnc	00106$
      000E4E                        701 00110$:
                                    702 ;	simpleserial.c:97: || (ascii_buffer[ascii_pos] >= 'A' && ascii_buffer[ascii_pos] <= 'F')
      000E4E EC               [12]  703 	mov	a,r4
      000E4F 2A               [12]  704 	add	a,r2
      000E50 F8               [12]  705 	mov	r0,a
      000E51 86 07            [24]  706 	mov	ar7,@r0
      000E53 BF 41 00         [24]  707 	cjne	r7,#0x41,00174$
      000E56                        708 00174$:
      000E56 40 05            [24]  709 	jc	00112$
      000E58 EF               [12]  710 	mov	a,r7
      000E59 24 B9            [12]  711 	add	a,#0xff - 0x46
      000E5B 50 0A            [24]  712 	jnc	00106$
      000E5D                        713 00112$:
                                    714 ;	simpleserial.c:98: || (ascii_buffer[ascii_pos] >= 'a' && ascii_buffer[ascii_pos] <= 'f')
      000E5D BF 61 00         [24]  715 	cjne	r7,#0x61,00177$
      000E60                        716 00177$:
      000E60 40 6C            [24]  717 	jc	00125$
      000E62 EF               [12]  718 	mov	a,r7
      000E63 24 99            [12]  719 	add	a,#0xff - 0x66
      000E65 40 67            [24]  720 	jc	00125$
      000E67                        721 00106$:
                                    722 ;	simpleserial.c:101: ascii_pos++;
      000E67 0C               [12]  723 	inc	r4
                                    724 ;	simpleserial.c:102: continue;
      000E68 80 9D            [24]  725 	sjmp	00127$
      000E6A                        726 00121$:
                                    727 ;	simpleserial.c:107: else if((ascii_pos == ASCII_LENGTH-1)
      000E6A BC 21 61         [24]  728 	cjne	r4,#0x21,00125$
                                    729 ;	simpleserial.c:108: && ((ascii_buffer[ascii_pos] == '\n') || (ascii_buffer[ascii_pos] == '\r')))
      000E6D 87 04            [24]  730 	mov	ar4,@r1
      000E6F BC 0A 02         [24]  731 	cjne	r4,#0x0a,00182$
      000E72 80 05            [24]  732 	sjmp	00116$
      000E74                        733 00182$:
      000E74 87 04            [24]  734 	mov	ar4,@r1
      000E76 BC 0D 55         [24]  735 	cjne	r4,#0x0d,00125$
      000E79                        736 00116$:
                                    737 ;	simpleserial.c:111: if(state == STATE_PT)
      000E79 BB 01 2A         [24]  738 	cjne	r3,#0x01,00114$
                                    739 ;	simpleserial.c:113: simpleserial_decode(ascii_buffer, input);
      000E7C 8A 04            [24]  740 	mov	ar4,r2
      000E7E 8C 03            [24]  741 	mov	ar3,r4
      000E80 7C 00            [12]  742 	mov	r4,#0x00
      000E82 7F 40            [12]  743 	mov	r7,#0x40
      000E84 A8 08            [24]  744 	mov	r0,_bp
      000E86 08               [12]  745 	inc	r0
      000E87 E6               [12]  746 	mov	a,@r0
      000E88 C0 E0            [24]  747 	push	acc
      000E8A 08               [12]  748 	inc	r0
      000E8B E6               [12]  749 	mov	a,@r0
      000E8C C0 E0            [24]  750 	push	acc
      000E8E 08               [12]  751 	inc	r0
      000E8F E6               [12]  752 	mov	a,@r0
      000E90 C0 E0            [24]  753 	push	acc
      000E92 8B 82            [24]  754 	mov	dpl,r3
      000E94 8C 83            [24]  755 	mov	dph,r4
      000E96 8F F0            [24]  756 	mov	b,r7
      000E98 12 0C 20         [24]  757 	lcall	_simpleserial_decode
      000E9B 15 81            [12]  758 	dec	sp
      000E9D 15 81            [12]  759 	dec	sp
      000E9F 15 81            [12]  760 	dec	sp
                                    761 ;	simpleserial.c:114: return 1;
      000EA1 90 00 01         [24]  762 	mov	dptr,#0x0001
      000EA4 80 2B            [24]  763 	sjmp	00129$
      000EA6                        764 00114$:
                                    765 ;	simpleserial.c:118: simpleserial_decode(ascii_buffer, key);
      000EA6 7F 00            [12]  766 	mov	r7,#0x00
      000EA8 7E 40            [12]  767 	mov	r6,#0x40
      000EAA E5 08            [12]  768 	mov	a,_bp
      000EAC 24 FB            [12]  769 	add	a,#0xfb
      000EAE F8               [12]  770 	mov	r0,a
      000EAF E6               [12]  771 	mov	a,@r0
      000EB0 C0 E0            [24]  772 	push	acc
      000EB2 08               [12]  773 	inc	r0
      000EB3 E6               [12]  774 	mov	a,@r0
      000EB4 C0 E0            [24]  775 	push	acc
      000EB6 08               [12]  776 	inc	r0
      000EB7 E6               [12]  777 	mov	a,@r0
      000EB8 C0 E0            [24]  778 	push	acc
      000EBA 8A 82            [24]  779 	mov	dpl,r2
      000EBC 8F 83            [24]  780 	mov	dph,r7
      000EBE 8E F0            [24]  781 	mov	b,r6
      000EC0 12 0C 20         [24]  782 	lcall	_simpleserial_decode
      000EC3 15 81            [12]  783 	dec	sp
      000EC5 15 81            [12]  784 	dec	sp
      000EC7 15 81            [12]  785 	dec	sp
                                    786 ;	simpleserial.c:119: return 0;
      000EC9 90 00 00         [24]  787 	mov	dptr,#0x0000
      000ECC 80 03            [24]  788 	sjmp	00129$
      000ECE                        789 00125$:
                                    790 ;	simpleserial.c:126: return 0;
      000ECE 90 00 00         [24]  791 	mov	dptr,#0x0000
      000ED1                        792 00129$:
      000ED1 85 08 81         [24]  793 	mov	sp,_bp
      000ED4 D0 08            [24]  794 	pop	_bp
      000ED6 22               [24]  795 	ret
                                    796 ;------------------------------------------------------------
                                    797 ;Allocation info for local variables in function 'simpleserial_put'
                                    798 ;------------------------------------------------------------
                                    799 ;output                    Allocated to stack - _bp +1
                                    800 ;ascii_buffer              Allocated to stack - _bp +4
                                    801 ;ascii_pos                 Allocated to registers r7 
                                    802 ;------------------------------------------------------------
                                    803 ;	simpleserial.c:130: void simpleserial_put(uint8_t* output)
                                    804 ;	-----------------------------------------
                                    805 ;	 function simpleserial_put
                                    806 ;	-----------------------------------------
      000ED7                        807 _simpleserial_put:
      000ED7 C0 08            [24]  808 	push	_bp
      000ED9 85 81 08         [24]  809 	mov	_bp,sp
      000EDC C0 82            [24]  810 	push	dpl
      000EDE C0 83            [24]  811 	push	dph
      000EE0 C0 F0            [24]  812 	push	b
      000EE2 E5 81            [12]  813 	mov	a,sp
      000EE4 24 22            [12]  814 	add	a,#0x22
      000EE6 F5 81            [12]  815 	mov	sp,a
                                    816 ;	simpleserial.c:136: simpleserial_encode(ascii_buffer, output);
      000EE8 E5 08            [12]  817 	mov	a,_bp
      000EEA 24 04            [12]  818 	add	a,#0x04
      000EEC FC               [12]  819 	mov	r4,a
      000EED FA               [12]  820 	mov	r2,a
      000EEE 7B 00            [12]  821 	mov	r3,#0x00
      000EF0 7F 40            [12]  822 	mov	r7,#0x40
      000EF2 C0 04            [24]  823 	push	ar4
      000EF4 A8 08            [24]  824 	mov	r0,_bp
      000EF6 08               [12]  825 	inc	r0
      000EF7 E6               [12]  826 	mov	a,@r0
      000EF8 C0 E0            [24]  827 	push	acc
      000EFA 08               [12]  828 	inc	r0
      000EFB E6               [12]  829 	mov	a,@r0
      000EFC C0 E0            [24]  830 	push	acc
      000EFE 08               [12]  831 	inc	r0
      000EFF E6               [12]  832 	mov	a,@r0
      000F00 C0 E0            [24]  833 	push	acc
      000F02 8A 82            [24]  834 	mov	dpl,r2
      000F04 8B 83            [24]  835 	mov	dph,r3
      000F06 8F F0            [24]  836 	mov	b,r7
      000F08 12 0B 32         [24]  837 	lcall	_simpleserial_encode
      000F0B 15 81            [12]  838 	dec	sp
      000F0D 15 81            [12]  839 	dec	sp
      000F0F 15 81            [12]  840 	dec	sp
      000F11 D0 04            [24]  841 	pop	ar4
                                    842 ;	simpleserial.c:137: for(ascii_pos = 0; ascii_pos < ASCII_LENGTH; ascii_pos++)
      000F13 7F 00            [12]  843 	mov	r7,#0x00
      000F15                        844 00102$:
                                    845 ;	simpleserial.c:139: putchar(ascii_buffer[ascii_pos]);
      000F15 EF               [12]  846 	mov	a,r7
      000F16 2C               [12]  847 	add	a,r4
      000F17 F9               [12]  848 	mov	r1,a
      000F18 87 82            [24]  849 	mov	dpl,@r1
      000F1A C0 07            [24]  850 	push	ar7
      000F1C C0 04            [24]  851 	push	ar4
      000F1E 12 00 7B         [24]  852 	lcall	_putchar
      000F21 D0 04            [24]  853 	pop	ar4
      000F23 D0 07            [24]  854 	pop	ar7
                                    855 ;	simpleserial.c:137: for(ascii_pos = 0; ascii_pos < ASCII_LENGTH; ascii_pos++)
      000F25 0F               [12]  856 	inc	r7
      000F26 BF 22 00         [24]  857 	cjne	r7,#0x22,00110$
      000F29                        858 00110$:
      000F29 40 EA            [24]  859 	jc	00102$
      000F2B 85 08 81         [24]  860 	mov	sp,_bp
      000F2E D0 08            [24]  861 	pop	_bp
      000F30 22               [24]  862 	ret
                                    863 	.area CSEG    (CODE)
                                    864 	.area CONST   (CODE)
      001077                        865 _hex_lookup:
      001077 30                     866 	.db #0x30	; 48	'0'
      001078 31                     867 	.db #0x31	; 49	'1'
      001079 32                     868 	.db #0x32	; 50	'2'
      00107A 33                     869 	.db #0x33	; 51	'3'
      00107B 34                     870 	.db #0x34	; 52	'4'
      00107C 35                     871 	.db #0x35	; 53	'5'
      00107D 36                     872 	.db #0x36	; 54	'6'
      00107E 37                     873 	.db #0x37	; 55	'7'
      00107F 38                     874 	.db #0x38	; 56	'8'
      001080 39                     875 	.db #0x39	; 57	'9'
      001081 41                     876 	.db #0x41	; 65	'A'
      001082 42                     877 	.db #0x42	; 66	'B'
      001083 43                     878 	.db #0x43	; 67	'C'
      001084 44                     879 	.db #0x44	; 68	'D'
      001085 45                     880 	.db #0x45	; 69	'E'
      001086 46                     881 	.db #0x46	; 70	'F'
                                    882 	.area XINIT   (CODE)
                                    883 	.area CABS    (ABS,CODE)
