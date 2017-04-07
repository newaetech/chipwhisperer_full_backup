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
                                     98 ;size                      Allocated to stack - _bp -6
                                     99 ;ascii                     Allocated to stack - _bp +1
                                    100 ;i                         Allocated to stack - _bp +4
                                    101 ;n_hi                      Allocated to registers r5 
                                    102 ;n_lo                      Allocated to stack - _bp +6
                                    103 ;------------------------------------------------------------
                                    104 ;	simpleserial.c:19: void simpleserial_encode(uint8_t* ascii, uint8_t* data, uint8_t size)
                                    105 ;	-----------------------------------------
                                    106 ;	 function simpleserial_encode
                                    107 ;	-----------------------------------------
      000172                        108 _simpleserial_encode:
                           000007   109 	ar7 = 0x07
                           000006   110 	ar6 = 0x06
                           000005   111 	ar5 = 0x05
                           000004   112 	ar4 = 0x04
                           000003   113 	ar3 = 0x03
                           000002   114 	ar2 = 0x02
                           000001   115 	ar1 = 0x01
                           000000   116 	ar0 = 0x00
      000172 C0 08            [24]  117 	push	_bp
      000174 85 81 08         [24]  118 	mov	_bp,sp
      000177 C0 82            [24]  119 	push	dpl
      000179 C0 83            [24]  120 	push	dph
      00017B C0 F0            [24]  121 	push	b
      00017D 05 81            [12]  122 	inc	sp
      00017F 05 81            [12]  123 	inc	sp
      000181 05 81            [12]  124 	inc	sp
                                    125 ;	simpleserial.c:23: ascii[0] = 'r';
      000183 A8 08            [24]  126 	mov	r0,_bp
      000185 08               [12]  127 	inc	r0
      000186 86 82            [24]  128 	mov	dpl,@r0
      000188 08               [12]  129 	inc	r0
      000189 86 83            [24]  130 	mov	dph,@r0
      00018B 08               [12]  131 	inc	r0
      00018C 86 F0            [24]  132 	mov	b,@r0
      00018E 74 72            [12]  133 	mov	a,#0x72
      000190 12 0D 7C         [24]  134 	lcall	__gptrput
                                    135 ;	simpleserial.c:24: for(i = 0; i < size; i++)
      000193 E5 08            [12]  136 	mov	a,_bp
      000195 24 04            [12]  137 	add	a,#0x04
      000197 F8               [12]  138 	mov	r0,a
      000198 E4               [12]  139 	clr	a
      000199 F6               [12]  140 	mov	@r0,a
      00019A 08               [12]  141 	inc	r0
      00019B F6               [12]  142 	mov	@r0,a
      00019C                        143 00103$:
      00019C E5 08            [12]  144 	mov	a,_bp
      00019E 24 FA            [12]  145 	add	a,#0xfa
      0001A0 F8               [12]  146 	mov	r0,a
      0001A1 86 02            [24]  147 	mov	ar2,@r0
      0001A3 7F 00            [12]  148 	mov	r7,#0x00
      0001A5 E5 08            [12]  149 	mov	a,_bp
      0001A7 24 04            [12]  150 	add	a,#0x04
      0001A9 F8               [12]  151 	mov	r0,a
      0001AA C3               [12]  152 	clr	c
      0001AB E6               [12]  153 	mov	a,@r0
      0001AC 9A               [12]  154 	subb	a,r2
      0001AD 08               [12]  155 	inc	r0
      0001AE E6               [12]  156 	mov	a,@r0
      0001AF 64 80            [12]  157 	xrl	a,#0x80
      0001B1 8F F0            [24]  158 	mov	b,r7
      0001B3 63 F0 80         [24]  159 	xrl	b,#0x80
      0001B6 95 F0            [12]  160 	subb	a,b
      0001B8 40 03            [24]  161 	jc	00114$
      0001BA 02 02 51         [24]  162 	ljmp	00101$
      0001BD                        163 00114$:
                                    164 ;	simpleserial.c:26: unsigned char n_hi = (data[i] >> 4) & 0x0f;
      0001BD E5 08            [12]  165 	mov	a,_bp
      0001BF 24 FB            [12]  166 	add	a,#0xfb
      0001C1 F8               [12]  167 	mov	r0,a
      0001C2 E5 08            [12]  168 	mov	a,_bp
      0001C4 24 04            [12]  169 	add	a,#0x04
      0001C6 F9               [12]  170 	mov	r1,a
      0001C7 E7               [12]  171 	mov	a,@r1
      0001C8 26               [12]  172 	add	a,@r0
      0001C9 FA               [12]  173 	mov	r2,a
      0001CA 09               [12]  174 	inc	r1
      0001CB E7               [12]  175 	mov	a,@r1
      0001CC 08               [12]  176 	inc	r0
      0001CD 36               [12]  177 	addc	a,@r0
      0001CE FB               [12]  178 	mov	r3,a
      0001CF 08               [12]  179 	inc	r0
      0001D0 86 04            [24]  180 	mov	ar4,@r0
      0001D2 8A 82            [24]  181 	mov	dpl,r2
      0001D4 8B 83            [24]  182 	mov	dph,r3
      0001D6 8C F0            [24]  183 	mov	b,r4
      0001D8 12 0D 97         [24]  184 	lcall	__gptrget
      0001DB FA               [12]  185 	mov	r2,a
      0001DC C4               [12]  186 	swap	a
      0001DD 54 0F            [12]  187 	anl	a,#0x0f
      0001DF FD               [12]  188 	mov	r5,a
                                    189 ;	simpleserial.c:27: unsigned char n_lo =  data[i]       & 0x0f;
      0001E0 E5 08            [12]  190 	mov	a,_bp
      0001E2 24 06            [12]  191 	add	a,#0x06
      0001E4 F8               [12]  192 	mov	r0,a
      0001E5 74 0F            [12]  193 	mov	a,#0x0f
      0001E7 5A               [12]  194 	anl	a,r2
      0001E8 F6               [12]  195 	mov	@r0,a
                                    196 ;	simpleserial.c:28: ascii[2*i+1] = hex_lookup[n_hi];
      0001E9 E5 08            [12]  197 	mov	a,_bp
      0001EB 24 04            [12]  198 	add	a,#0x04
      0001ED F8               [12]  199 	mov	r0,a
      0001EE 86 02            [24]  200 	mov	ar2,@r0
      0001F0 08               [12]  201 	inc	r0
      0001F1 E6               [12]  202 	mov	a,@r0
      0001F2 CA               [12]  203 	xch	a,r2
      0001F3 25 E0            [12]  204 	add	a,acc
      0001F5 CA               [12]  205 	xch	a,r2
      0001F6 33               [12]  206 	rlc	a
      0001F7 FB               [12]  207 	mov	r3,a
      0001F8 74 01            [12]  208 	mov	a,#0x01
      0001FA 2A               [12]  209 	add	a,r2
      0001FB FC               [12]  210 	mov	r4,a
      0001FC E4               [12]  211 	clr	a
      0001FD 3B               [12]  212 	addc	a,r3
      0001FE FF               [12]  213 	mov	r7,a
      0001FF A8 08            [24]  214 	mov	r0,_bp
      000201 08               [12]  215 	inc	r0
      000202 EC               [12]  216 	mov	a,r4
      000203 26               [12]  217 	add	a,@r0
      000204 FC               [12]  218 	mov	r4,a
      000205 EF               [12]  219 	mov	a,r7
      000206 08               [12]  220 	inc	r0
      000207 36               [12]  221 	addc	a,@r0
      000208 FF               [12]  222 	mov	r7,a
      000209 08               [12]  223 	inc	r0
      00020A 86 06            [24]  224 	mov	ar6,@r0
      00020C ED               [12]  225 	mov	a,r5
      00020D 90 0D B7         [24]  226 	mov	dptr,#_hex_lookup
      000210 93               [24]  227 	movc	a,@a+dptr
      000211 FD               [12]  228 	mov	r5,a
      000212 8C 82            [24]  229 	mov	dpl,r4
      000214 8F 83            [24]  230 	mov	dph,r7
      000216 8E F0            [24]  231 	mov	b,r6
      000218 12 0D 7C         [24]  232 	lcall	__gptrput
                                    233 ;	simpleserial.c:29: ascii[2*i+2] = hex_lookup[n_lo];
      00021B 74 02            [12]  234 	mov	a,#0x02
      00021D 2A               [12]  235 	add	a,r2
      00021E FA               [12]  236 	mov	r2,a
      00021F E4               [12]  237 	clr	a
      000220 3B               [12]  238 	addc	a,r3
      000221 FB               [12]  239 	mov	r3,a
      000222 A8 08            [24]  240 	mov	r0,_bp
      000224 08               [12]  241 	inc	r0
      000225 EA               [12]  242 	mov	a,r2
      000226 26               [12]  243 	add	a,@r0
      000227 FA               [12]  244 	mov	r2,a
      000228 EB               [12]  245 	mov	a,r3
      000229 08               [12]  246 	inc	r0
      00022A 36               [12]  247 	addc	a,@r0
      00022B FB               [12]  248 	mov	r3,a
      00022C 08               [12]  249 	inc	r0
      00022D 86 07            [24]  250 	mov	ar7,@r0
      00022F E5 08            [12]  251 	mov	a,_bp
      000231 24 06            [12]  252 	add	a,#0x06
      000233 F8               [12]  253 	mov	r0,a
      000234 E6               [12]  254 	mov	a,@r0
      000235 90 0D B7         [24]  255 	mov	dptr,#_hex_lookup
      000238 93               [24]  256 	movc	a,@a+dptr
      000239 FE               [12]  257 	mov	r6,a
      00023A 8A 82            [24]  258 	mov	dpl,r2
      00023C 8B 83            [24]  259 	mov	dph,r3
      00023E 8F F0            [24]  260 	mov	b,r7
      000240 12 0D 7C         [24]  261 	lcall	__gptrput
                                    262 ;	simpleserial.c:24: for(i = 0; i < size; i++)
      000243 E5 08            [12]  263 	mov	a,_bp
      000245 24 04            [12]  264 	add	a,#0x04
      000247 F8               [12]  265 	mov	r0,a
      000248 06               [12]  266 	inc	@r0
      000249 B6 00 02         [24]  267 	cjne	@r0,#0x00,00115$
      00024C 08               [12]  268 	inc	r0
      00024D 06               [12]  269 	inc	@r0
      00024E                        270 00115$:
      00024E 02 01 9C         [24]  271 	ljmp	00103$
      000251                        272 00101$:
                                    273 ;	simpleserial.c:31: ascii[2*size+1] = '\n';
      000251 E5 08            [12]  274 	mov	a,_bp
      000253 24 FA            [12]  275 	add	a,#0xfa
      000255 F8               [12]  276 	mov	r0,a
      000256 E6               [12]  277 	mov	a,@r0
      000257 75 F0 02         [24]  278 	mov	b,#0x02
      00025A A4               [48]  279 	mul	ab
      00025B 24 01            [12]  280 	add	a,#0x01
      00025D FE               [12]  281 	mov	r6,a
      00025E E4               [12]  282 	clr	a
      00025F 35 F0            [12]  283 	addc	a,b
      000261 FF               [12]  284 	mov	r7,a
      000262 A8 08            [24]  285 	mov	r0,_bp
      000264 08               [12]  286 	inc	r0
      000265 EE               [12]  287 	mov	a,r6
      000266 26               [12]  288 	add	a,@r0
      000267 FE               [12]  289 	mov	r6,a
      000268 EF               [12]  290 	mov	a,r7
      000269 08               [12]  291 	inc	r0
      00026A 36               [12]  292 	addc	a,@r0
      00026B FF               [12]  293 	mov	r7,a
      00026C 08               [12]  294 	inc	r0
      00026D 86 05            [24]  295 	mov	ar5,@r0
      00026F 8E 82            [24]  296 	mov	dpl,r6
      000271 8F 83            [24]  297 	mov	dph,r7
      000273 8D F0            [24]  298 	mov	b,r5
      000275 74 0A            [12]  299 	mov	a,#0x0a
      000277 12 0D 7C         [24]  300 	lcall	__gptrput
      00027A 85 08 81         [24]  301 	mov	sp,_bp
      00027D D0 08            [24]  302 	pop	_bp
      00027F 22               [24]  303 	ret
                                    304 ;------------------------------------------------------------
                                    305 ;Allocation info for local variables in function 'simpleserial_decode'
                                    306 ;------------------------------------------------------------
                                    307 ;data                      Allocated to stack - _bp -5
                                    308 ;size                      Allocated to stack - _bp -6
                                    309 ;ascii                     Allocated to stack - _bp +1
                                    310 ;i                         Allocated to registers r3 r4 
                                    311 ;n_hi                      Allocated to stack - _bp +4
                                    312 ;n_lo                      Allocated to stack - _bp +5
                                    313 ;------------------------------------------------------------
                                    314 ;	simpleserial.c:35: void simpleserial_decode(uint8_t* ascii, uint8_t* data, uint8_t size)
                                    315 ;	-----------------------------------------
                                    316 ;	 function simpleserial_decode
                                    317 ;	-----------------------------------------
      000280                        318 _simpleserial_decode:
      000280 C0 08            [24]  319 	push	_bp
      000282 85 81 08         [24]  320 	mov	_bp,sp
      000285 C0 82            [24]  321 	push	dpl
      000287 C0 83            [24]  322 	push	dph
      000289 C0 F0            [24]  323 	push	b
      00028B 05 81            [12]  324 	inc	sp
      00028D 05 81            [12]  325 	inc	sp
                                    326 ;	simpleserial.c:39: for(i = 0; i < size; i++)
      00028F 7B 00            [12]  327 	mov	r3,#0x00
      000291 7C 00            [12]  328 	mov	r4,#0x00
      000293                        329 00115$:
      000293 E5 08            [12]  330 	mov	a,_bp
      000295 24 FA            [12]  331 	add	a,#0xfa
      000297 F8               [12]  332 	mov	r0,a
      000298 86 02            [24]  333 	mov	ar2,@r0
      00029A 7F 00            [12]  334 	mov	r7,#0x00
      00029C C3               [12]  335 	clr	c
      00029D EB               [12]  336 	mov	a,r3
      00029E 9A               [12]  337 	subb	a,r2
      00029F EC               [12]  338 	mov	a,r4
      0002A0 64 80            [12]  339 	xrl	a,#0x80
      0002A2 8F F0            [24]  340 	mov	b,r7
      0002A4 63 F0 80         [24]  341 	xrl	b,#0x80
      0002A7 95 F0            [12]  342 	subb	a,b
      0002A9 40 03            [24]  343 	jc	00138$
      0002AB 02 04 58         [24]  344 	ljmp	00117$
      0002AE                        345 00138$:
                                    346 ;	simpleserial.c:41: unsigned char n_hi = ascii[2*i + 1];
      0002AE 8B 02            [24]  347 	mov	ar2,r3
      0002B0 EC               [12]  348 	mov	a,r4
      0002B1 CA               [12]  349 	xch	a,r2
      0002B2 25 E0            [12]  350 	add	a,acc
      0002B4 CA               [12]  351 	xch	a,r2
      0002B5 33               [12]  352 	rlc	a
      0002B6 FF               [12]  353 	mov	r7,a
      0002B7 74 01            [12]  354 	mov	a,#0x01
      0002B9 2A               [12]  355 	add	a,r2
      0002BA FD               [12]  356 	mov	r5,a
      0002BB E4               [12]  357 	clr	a
      0002BC 3F               [12]  358 	addc	a,r7
      0002BD FE               [12]  359 	mov	r6,a
      0002BE C0 03            [24]  360 	push	ar3
      0002C0 C0 04            [24]  361 	push	ar4
      0002C2 A8 08            [24]  362 	mov	r0,_bp
      0002C4 08               [12]  363 	inc	r0
      0002C5 ED               [12]  364 	mov	a,r5
      0002C6 26               [12]  365 	add	a,@r0
      0002C7 FD               [12]  366 	mov	r5,a
      0002C8 EE               [12]  367 	mov	a,r6
      0002C9 08               [12]  368 	inc	r0
      0002CA 36               [12]  369 	addc	a,@r0
      0002CB FC               [12]  370 	mov	r4,a
      0002CC 08               [12]  371 	inc	r0
      0002CD 86 06            [24]  372 	mov	ar6,@r0
      0002CF 8D 82            [24]  373 	mov	dpl,r5
      0002D1 8C 83            [24]  374 	mov	dph,r4
      0002D3 8E F0            [24]  375 	mov	b,r6
      0002D5 E5 08            [12]  376 	mov	a,_bp
      0002D7 24 04            [12]  377 	add	a,#0x04
      0002D9 F8               [12]  378 	mov	r0,a
      0002DA 12 0D 97         [24]  379 	lcall	__gptrget
      0002DD F6               [12]  380 	mov	@r0,a
                                    381 ;	simpleserial.c:42: unsigned char n_lo = ascii[2*i + 2];
      0002DE 74 02            [12]  382 	mov	a,#0x02
      0002E0 2A               [12]  383 	add	a,r2
      0002E1 FA               [12]  384 	mov	r2,a
      0002E2 E4               [12]  385 	clr	a
      0002E3 3F               [12]  386 	addc	a,r7
      0002E4 FF               [12]  387 	mov	r7,a
      0002E5 A8 08            [24]  388 	mov	r0,_bp
      0002E7 08               [12]  389 	inc	r0
      0002E8 EA               [12]  390 	mov	a,r2
      0002E9 26               [12]  391 	add	a,@r0
      0002EA FA               [12]  392 	mov	r2,a
      0002EB EF               [12]  393 	mov	a,r7
      0002EC 08               [12]  394 	inc	r0
      0002ED 36               [12]  395 	addc	a,@r0
      0002EE FF               [12]  396 	mov	r7,a
      0002EF 08               [12]  397 	inc	r0
      0002F0 86 06            [24]  398 	mov	ar6,@r0
      0002F2 8A 82            [24]  399 	mov	dpl,r2
      0002F4 8F 83            [24]  400 	mov	dph,r7
      0002F6 8E F0            [24]  401 	mov	b,r6
      0002F8 E5 08            [12]  402 	mov	a,_bp
      0002FA 24 05            [12]  403 	add	a,#0x05
      0002FC F8               [12]  404 	mov	r0,a
      0002FD 12 0D 97         [24]  405 	lcall	__gptrget
      000300 F6               [12]  406 	mov	@r0,a
                                    407 ;	simpleserial.c:44: if(n_lo <= '9')
      000301 E5 08            [12]  408 	mov	a,_bp
      000303 24 05            [12]  409 	add	a,#0x05
      000305 F8               [12]  410 	mov	r0,a
      000306 C3               [12]  411 	clr	c
      000307 74 39            [12]  412 	mov	a,#0x39
      000309 96               [12]  413 	subb	a,@r0
      00030A D0 04            [24]  414 	pop	ar4
      00030C D0 03            [24]  415 	pop	ar3
      00030E 40 23            [24]  416 	jc	00105$
                                    417 ;	simpleserial.c:45: data[i] = n_lo - '0';
      000310 E5 08            [12]  418 	mov	a,_bp
      000312 24 FB            [12]  419 	add	a,#0xfb
      000314 F8               [12]  420 	mov	r0,a
      000315 EB               [12]  421 	mov	a,r3
      000316 26               [12]  422 	add	a,@r0
      000317 FD               [12]  423 	mov	r5,a
      000318 EC               [12]  424 	mov	a,r4
      000319 08               [12]  425 	inc	r0
      00031A 36               [12]  426 	addc	a,@r0
      00031B FE               [12]  427 	mov	r6,a
      00031C 08               [12]  428 	inc	r0
      00031D 86 07            [24]  429 	mov	ar7,@r0
      00031F E5 08            [12]  430 	mov	a,_bp
      000321 24 05            [12]  431 	add	a,#0x05
      000323 F8               [12]  432 	mov	r0,a
      000324 E6               [12]  433 	mov	a,@r0
      000325 24 D0            [12]  434 	add	a,#0xd0
      000327 FA               [12]  435 	mov	r2,a
      000328 8D 82            [24]  436 	mov	dpl,r5
      00032A 8E 83            [24]  437 	mov	dph,r6
      00032C 8F F0            [24]  438 	mov	b,r7
      00032E 12 0D 7C         [24]  439 	lcall	__gptrput
      000331 80 52            [24]  440 	sjmp	00106$
      000333                        441 00105$:
                                    442 ;	simpleserial.c:46: else if(n_lo <= 'F')
      000333 E5 08            [12]  443 	mov	a,_bp
      000335 24 05            [12]  444 	add	a,#0x05
      000337 F8               [12]  445 	mov	r0,a
      000338 E6               [12]  446 	mov	a,@r0
      000339 24 B9            [12]  447 	add	a,#0xff - 0x46
      00033B 40 25            [24]  448 	jc	00102$
                                    449 ;	simpleserial.c:47: data[i] = n_lo - 'A' + 10;
      00033D E5 08            [12]  450 	mov	a,_bp
      00033F 24 FB            [12]  451 	add	a,#0xfb
      000341 F8               [12]  452 	mov	r0,a
      000342 EB               [12]  453 	mov	a,r3
      000343 26               [12]  454 	add	a,@r0
      000344 FD               [12]  455 	mov	r5,a
      000345 EC               [12]  456 	mov	a,r4
      000346 08               [12]  457 	inc	r0
      000347 36               [12]  458 	addc	a,@r0
      000348 FE               [12]  459 	mov	r6,a
      000349 08               [12]  460 	inc	r0
      00034A 86 07            [24]  461 	mov	ar7,@r0
      00034C E5 08            [12]  462 	mov	a,_bp
      00034E 24 05            [12]  463 	add	a,#0x05
      000350 F8               [12]  464 	mov	r0,a
      000351 86 02            [24]  465 	mov	ar2,@r0
      000353 74 C9            [12]  466 	mov	a,#0xc9
      000355 2A               [12]  467 	add	a,r2
      000356 FA               [12]  468 	mov	r2,a
      000357 8D 82            [24]  469 	mov	dpl,r5
      000359 8E 83            [24]  470 	mov	dph,r6
      00035B 8F F0            [24]  471 	mov	b,r7
      00035D 12 0D 7C         [24]  472 	lcall	__gptrput
      000360 80 23            [24]  473 	sjmp	00106$
      000362                        474 00102$:
                                    475 ;	simpleserial.c:49: data[i] = n_lo - 'a' + 10;
      000362 E5 08            [12]  476 	mov	a,_bp
      000364 24 FB            [12]  477 	add	a,#0xfb
      000366 F8               [12]  478 	mov	r0,a
      000367 EB               [12]  479 	mov	a,r3
      000368 26               [12]  480 	add	a,@r0
      000369 FD               [12]  481 	mov	r5,a
      00036A EC               [12]  482 	mov	a,r4
      00036B 08               [12]  483 	inc	r0
      00036C 36               [12]  484 	addc	a,@r0
      00036D FE               [12]  485 	mov	r6,a
      00036E 08               [12]  486 	inc	r0
      00036F 86 07            [24]  487 	mov	ar7,@r0
      000371 E5 08            [12]  488 	mov	a,_bp
      000373 24 05            [12]  489 	add	a,#0x05
      000375 F8               [12]  490 	mov	r0,a
      000376 86 02            [24]  491 	mov	ar2,@r0
      000378 74 A9            [12]  492 	mov	a,#0xa9
      00037A 2A               [12]  493 	add	a,r2
      00037B FA               [12]  494 	mov	r2,a
      00037C 8D 82            [24]  495 	mov	dpl,r5
      00037E 8E 83            [24]  496 	mov	dph,r6
      000380 8F F0            [24]  497 	mov	b,r7
      000382 12 0D 7C         [24]  498 	lcall	__gptrput
      000385                        499 00106$:
                                    500 ;	simpleserial.c:51: if(n_hi <= '9')
      000385 E5 08            [12]  501 	mov	a,_bp
      000387 24 04            [12]  502 	add	a,#0x04
      000389 F8               [12]  503 	mov	r0,a
      00038A E6               [12]  504 	mov	a,@r0
      00038B 24 C6            [12]  505 	add	a,#0xff - 0x39
      00038D 40 3D            [24]  506 	jc	00111$
                                    507 ;	simpleserial.c:52: data[i] |= (n_hi - '0') << 4;
      00038F E5 08            [12]  508 	mov	a,_bp
      000391 24 FB            [12]  509 	add	a,#0xfb
      000393 F8               [12]  510 	mov	r0,a
      000394 EB               [12]  511 	mov	a,r3
      000395 26               [12]  512 	add	a,@r0
      000396 FD               [12]  513 	mov	r5,a
      000397 EC               [12]  514 	mov	a,r4
      000398 08               [12]  515 	inc	r0
      000399 36               [12]  516 	addc	a,@r0
      00039A FE               [12]  517 	mov	r6,a
      00039B 08               [12]  518 	inc	r0
      00039C 86 07            [24]  519 	mov	ar7,@r0
      00039E 8D 82            [24]  520 	mov	dpl,r5
      0003A0 8E 83            [24]  521 	mov	dph,r6
      0003A2 8F F0            [24]  522 	mov	b,r7
      0003A4 12 0D 97         [24]  523 	lcall	__gptrget
      0003A7 FA               [12]  524 	mov	r2,a
      0003A8 C0 03            [24]  525 	push	ar3
      0003AA C0 04            [24]  526 	push	ar4
      0003AC E5 08            [12]  527 	mov	a,_bp
      0003AE 24 04            [12]  528 	add	a,#0x04
      0003B0 F8               [12]  529 	mov	r0,a
      0003B1 E6               [12]  530 	mov	a,@r0
      0003B2 24 D0            [12]  531 	add	a,#0xd0
      0003B4 C4               [12]  532 	swap	a
      0003B5 54 F0            [12]  533 	anl	a,#0xf0
      0003B7 FC               [12]  534 	mov	r4,a
      0003B8 EA               [12]  535 	mov	a,r2
      0003B9 42 04            [12]  536 	orl	ar4,a
      0003BB 8D 82            [24]  537 	mov	dpl,r5
      0003BD 8E 83            [24]  538 	mov	dph,r6
      0003BF 8F F0            [24]  539 	mov	b,r7
      0003C1 EC               [12]  540 	mov	a,r4
      0003C2 12 0D 7C         [24]  541 	lcall	__gptrput
      0003C5 D0 04            [24]  542 	pop	ar4
      0003C7 D0 03            [24]  543 	pop	ar3
      0003C9 02 04 50         [24]  544 	ljmp	00116$
      0003CC                        545 00111$:
                                    546 ;	simpleserial.c:53: else if(n_hi <= 'F')
      0003CC E5 08            [12]  547 	mov	a,_bp
      0003CE 24 04            [12]  548 	add	a,#0x04
      0003D0 F8               [12]  549 	mov	r0,a
      0003D1 E6               [12]  550 	mov	a,@r0
      0003D2 24 B9            [12]  551 	add	a,#0xff - 0x46
      0003D4 40 3E            [24]  552 	jc	00108$
                                    553 ;	simpleserial.c:54: data[i] |= (n_hi - 'A' + 10) << 4;
      0003D6 E5 08            [12]  554 	mov	a,_bp
      0003D8 24 FB            [12]  555 	add	a,#0xfb
      0003DA F8               [12]  556 	mov	r0,a
      0003DB EB               [12]  557 	mov	a,r3
      0003DC 26               [12]  558 	add	a,@r0
      0003DD FD               [12]  559 	mov	r5,a
      0003DE EC               [12]  560 	mov	a,r4
      0003DF 08               [12]  561 	inc	r0
      0003E0 36               [12]  562 	addc	a,@r0
      0003E1 FE               [12]  563 	mov	r6,a
      0003E2 08               [12]  564 	inc	r0
      0003E3 86 07            [24]  565 	mov	ar7,@r0
      0003E5 8D 82            [24]  566 	mov	dpl,r5
      0003E7 8E 83            [24]  567 	mov	dph,r6
      0003E9 8F F0            [24]  568 	mov	b,r7
      0003EB 12 0D 97         [24]  569 	lcall	__gptrget
      0003EE FA               [12]  570 	mov	r2,a
      0003EF C0 03            [24]  571 	push	ar3
      0003F1 C0 04            [24]  572 	push	ar4
      0003F3 E5 08            [12]  573 	mov	a,_bp
      0003F5 24 04            [12]  574 	add	a,#0x04
      0003F7 F8               [12]  575 	mov	r0,a
      0003F8 86 04            [24]  576 	mov	ar4,@r0
      0003FA 74 C9            [12]  577 	mov	a,#0xc9
      0003FC 2C               [12]  578 	add	a,r4
      0003FD C4               [12]  579 	swap	a
      0003FE 54 F0            [12]  580 	anl	a,#0xf0
      000400 FC               [12]  581 	mov	r4,a
      000401 EA               [12]  582 	mov	a,r2
      000402 42 04            [12]  583 	orl	ar4,a
      000404 8D 82            [24]  584 	mov	dpl,r5
      000406 8E 83            [24]  585 	mov	dph,r6
      000408 8F F0            [24]  586 	mov	b,r7
      00040A EC               [12]  587 	mov	a,r4
      00040B 12 0D 7C         [24]  588 	lcall	__gptrput
      00040E D0 04            [24]  589 	pop	ar4
      000410 D0 03            [24]  590 	pop	ar3
      000412 80 3C            [24]  591 	sjmp	00116$
      000414                        592 00108$:
                                    593 ;	simpleserial.c:56: data[i] |= (n_hi - 'a' + 10) << 4;
      000414 E5 08            [12]  594 	mov	a,_bp
      000416 24 FB            [12]  595 	add	a,#0xfb
      000418 F8               [12]  596 	mov	r0,a
      000419 EB               [12]  597 	mov	a,r3
      00041A 26               [12]  598 	add	a,@r0
      00041B FD               [12]  599 	mov	r5,a
      00041C EC               [12]  600 	mov	a,r4
      00041D 08               [12]  601 	inc	r0
      00041E 36               [12]  602 	addc	a,@r0
      00041F FE               [12]  603 	mov	r6,a
      000420 08               [12]  604 	inc	r0
      000421 86 07            [24]  605 	mov	ar7,@r0
      000423 8D 82            [24]  606 	mov	dpl,r5
      000425 8E 83            [24]  607 	mov	dph,r6
      000427 8F F0            [24]  608 	mov	b,r7
      000429 12 0D 97         [24]  609 	lcall	__gptrget
      00042C FA               [12]  610 	mov	r2,a
      00042D C0 03            [24]  611 	push	ar3
      00042F C0 04            [24]  612 	push	ar4
      000431 E5 08            [12]  613 	mov	a,_bp
      000433 24 04            [12]  614 	add	a,#0x04
      000435 F8               [12]  615 	mov	r0,a
      000436 86 04            [24]  616 	mov	ar4,@r0
      000438 74 A9            [12]  617 	mov	a,#0xa9
      00043A 2C               [12]  618 	add	a,r4
      00043B C4               [12]  619 	swap	a
      00043C 54 F0            [12]  620 	anl	a,#0xf0
      00043E FC               [12]  621 	mov	r4,a
      00043F EA               [12]  622 	mov	a,r2
      000440 42 04            [12]  623 	orl	ar4,a
      000442 8D 82            [24]  624 	mov	dpl,r5
      000444 8E 83            [24]  625 	mov	dph,r6
      000446 8F F0            [24]  626 	mov	b,r7
      000448 EC               [12]  627 	mov	a,r4
      000449 12 0D 7C         [24]  628 	lcall	__gptrput
                                    629 ;	simpleserial.c:39: for(i = 0; i < size; i++)
      00044C D0 04            [24]  630 	pop	ar4
      00044E D0 03            [24]  631 	pop	ar3
                                    632 ;	simpleserial.c:56: data[i] |= (n_hi - 'a' + 10) << 4;
      000450                        633 00116$:
                                    634 ;	simpleserial.c:39: for(i = 0; i < size; i++)
      000450 0B               [12]  635 	inc	r3
      000451 BB 00 01         [24]  636 	cjne	r3,#0x00,00143$
      000454 0C               [12]  637 	inc	r4
      000455                        638 00143$:
      000455 02 02 93         [24]  639 	ljmp	00115$
      000458                        640 00117$:
      000458 85 08 81         [24]  641 	mov	sp,_bp
      00045B D0 08            [24]  642 	pop	_bp
      00045D 22               [24]  643 	ret
                                    644 ;------------------------------------------------------------
                                    645 ;Allocation info for local variables in function 'simpleserial_get'
                                    646 ;------------------------------------------------------------
                                    647 ;key                       Allocated to stack - _bp -5
                                    648 ;size_input                Allocated to stack - _bp -6
                                    649 ;size_key                  Allocated to stack - _bp -7
                                    650 ;input                     Allocated to stack - _bp +1
                                    651 ;ascii_buffer              Allocated to stack - _bp +4
                                    652 ;ascii_pos                 Allocated to registers r4 
                                    653 ;state                     Allocated to registers r3 
                                    654 ;length_limit              Allocated to registers r7 
                                    655 ;sloc0                     Allocated to stack - _bp +41
                                    656 ;------------------------------------------------------------
                                    657 ;	simpleserial.c:60: int simpleserial_get(uint8_t* input, uint8_t* key, uint8_t size_input, uint8_t size_key)
                                    658 ;	-----------------------------------------
                                    659 ;	 function simpleserial_get
                                    660 ;	-----------------------------------------
      00045E                        661 _simpleserial_get:
      00045E C0 08            [24]  662 	push	_bp
      000460 85 81 08         [24]  663 	mov	_bp,sp
      000463 C0 82            [24]  664 	push	dpl
      000465 C0 83            [24]  665 	push	dph
      000467 C0 F0            [24]  666 	push	b
      000469 E5 81            [12]  667 	mov	a,sp
      00046B 24 22            [12]  668 	add	a,#0x22
      00046D F5 81            [12]  669 	mov	sp,a
                                    670 ;	simpleserial.c:64: unsigned char ascii_pos = 0;
                                    671 ;	simpleserial.c:67: unsigned char state = STATE_IDLE;
                                    672 ;	simpleserial.c:70: while(1)
      00046F E4               [12]  673 	clr	a
      000470 FC               [12]  674 	mov	r4,a
      000471 FB               [12]  675 	mov	r3,a
      000472 E5 08            [12]  676 	mov	a,_bp
      000474 24 04            [12]  677 	add	a,#0x04
      000476 FA               [12]  678 	mov	r2,a
      000477                        679 00130$:
                                    680 ;	simpleserial.c:73: ascii_buffer[ascii_pos] = getchar();
      000477 EC               [12]  681 	mov	a,r4
      000478 2A               [12]  682 	add	a,r2
      000479 F9               [12]  683 	mov	r1,a
      00047A C0 04            [24]  684 	push	ar4
      00047C C0 03            [24]  685 	push	ar3
      00047E C0 02            [24]  686 	push	ar2
      000480 C0 01            [24]  687 	push	ar1
      000482 12 00 A0         [24]  688 	lcall	_getchar
      000485 E5 82            [12]  689 	mov	a,dpl
      000487 D0 01            [24]  690 	pop	ar1
      000489 D0 02            [24]  691 	pop	ar2
      00048B D0 03            [24]  692 	pop	ar3
      00048D D0 04            [24]  693 	pop	ar4
      00048F F7               [12]  694 	mov	@r1,a
                                    695 ;	simpleserial.c:75: if(state == STATE_IDLE)
      000490 EB               [12]  696 	mov	a,r3
      000491 70 19            [24]  697 	jnz	00127$
                                    698 ;	simpleserial.c:79: if(ascii_buffer[ascii_pos] == 'k')
      000493 87 07            [24]  699 	mov	ar7,@r1
      000495 BF 6B 05         [24]  700 	cjne	r7,#0x6b,00104$
                                    701 ;	simpleserial.c:81: state = STATE_KEY;
      000498 7B 02            [12]  702 	mov	r3,#0x02
                                    703 ;	simpleserial.c:82: ascii_pos++;
      00049A 0C               [12]  704 	inc	r4
                                    705 ;	simpleserial.c:83: continue;
      00049B 80 DA            [24]  706 	sjmp	00130$
      00049D                        707 00104$:
                                    708 ;	simpleserial.c:85: else if(ascii_buffer[ascii_pos] == 'p')
      00049D 87 07            [24]  709 	mov	ar7,@r1
      00049F BF 70 02         [24]  710 	cjne	r7,#0x70,00173$
      0004A2 80 03            [24]  711 	sjmp	00174$
      0004A4                        712 00173$:
      0004A4 02 05 76         [24]  713 	ljmp	00128$
      0004A7                        714 00174$:
                                    715 ;	simpleserial.c:87: state = STATE_PT;
      0004A7 7B 01            [12]  716 	mov	r3,#0x01
                                    717 ;	simpleserial.c:88: ascii_pos++;
      0004A9 0C               [12]  718 	inc	r4
                                    719 ;	simpleserial.c:89: continue;
      0004AA 80 CB            [24]  720 	sjmp	00130$
      0004AC                        721 00127$:
                                    722 ;	simpleserial.c:95: if(state == STATE_PT)
      0004AC BB 01 03         [24]  723 	cjne	r3,#0x01,00175$
      0004AF EB               [12]  724 	mov	a,r3
      0004B0 80 01            [24]  725 	sjmp	00176$
      0004B2                        726 00175$:
      0004B2 E4               [12]  727 	clr	a
      0004B3                        728 00176$:
      0004B3 FD               [12]  729 	mov	r5,a
      0004B4 60 09            [24]  730 	jz	00107$
                                    731 ;	simpleserial.c:96: length_limit = size_input;
      0004B6 E5 08            [12]  732 	mov	a,_bp
      0004B8 24 FA            [12]  733 	add	a,#0xfa
      0004BA F8               [12]  734 	mov	r0,a
      0004BB 86 07            [24]  735 	mov	ar7,@r0
      0004BD 80 07            [24]  736 	sjmp	00108$
      0004BF                        737 00107$:
                                    738 ;	simpleserial.c:98: length_limit = size_key;
      0004BF E5 08            [12]  739 	mov	a,_bp
      0004C1 24 F9            [12]  740 	add	a,#0xf9
      0004C3 F8               [12]  741 	mov	r0,a
      0004C4 86 07            [24]  742 	mov	ar7,@r0
      0004C6                        743 00108$:
                                    744 ;	simpleserial.c:99: length_limit = 2*length_limit + 1;
      0004C6 EF               [12]  745 	mov	a,r7
      0004C7 2F               [12]  746 	add	a,r7
      0004C8 FE               [12]  747 	mov	r6,a
      0004C9 04               [12]  748 	inc	a
      0004CA FF               [12]  749 	mov	r7,a
                                    750 ;	simpleserial.c:102: if(ascii_pos < length_limit)
      0004CB C3               [12]  751 	clr	c
      0004CC EC               [12]  752 	mov	a,r4
      0004CD 9F               [12]  753 	subb	a,r7
      0004CE 50 32            [24]  754 	jnc	00124$
                                    755 ;	simpleserial.c:104: if((ascii_buffer[ascii_pos] >= '0' && ascii_buffer[ascii_pos] <= '9')
      0004D0 EC               [12]  756 	mov	a,r4
      0004D1 2A               [12]  757 	add	a,r2
      0004D2 F9               [12]  758 	mov	r1,a
      0004D3 87 06            [24]  759 	mov	ar6,@r1
      0004D5 BE 30 00         [24]  760 	cjne	r6,#0x30,00179$
      0004D8                        761 00179$:
      0004D8 40 05            [24]  762 	jc	00113$
      0004DA EE               [12]  763 	mov	a,r6
      0004DB 24 C6            [12]  764 	add	a,#0xff - 0x39
      0004DD 50 1F            [24]  765 	jnc	00109$
      0004DF                        766 00113$:
                                    767 ;	simpleserial.c:105: || (ascii_buffer[ascii_pos] >= 'A' && ascii_buffer[ascii_pos] <= 'F')
      0004DF EC               [12]  768 	mov	a,r4
      0004E0 2A               [12]  769 	add	a,r2
      0004E1 F9               [12]  770 	mov	r1,a
      0004E2 87 06            [24]  771 	mov	ar6,@r1
      0004E4 BE 41 00         [24]  772 	cjne	r6,#0x41,00182$
      0004E7                        773 00182$:
      0004E7 40 05            [24]  774 	jc	00115$
      0004E9 EE               [12]  775 	mov	a,r6
      0004EA 24 B9            [12]  776 	add	a,#0xff - 0x46
      0004EC 50 10            [24]  777 	jnc	00109$
      0004EE                        778 00115$:
                                    779 ;	simpleserial.c:106: || (ascii_buffer[ascii_pos] >= 'a' && ascii_buffer[ascii_pos] <= 'f')
      0004EE BE 61 00         [24]  780 	cjne	r6,#0x61,00185$
      0004F1                        781 00185$:
      0004F1 50 03            [24]  782 	jnc	00186$
      0004F3 02 05 76         [24]  783 	ljmp	00128$
      0004F6                        784 00186$:
      0004F6 EE               [12]  785 	mov	a,r6
      0004F7 24 99            [12]  786 	add	a,#0xff - 0x66
      0004F9 50 03            [24]  787 	jnc	00187$
      0004FB 02 05 76         [24]  788 	ljmp	00128$
      0004FE                        789 00187$:
      0004FE                        790 00109$:
                                    791 ;	simpleserial.c:109: ascii_pos++;
      0004FE 0C               [12]  792 	inc	r4
                                    793 ;	simpleserial.c:110: continue;
      0004FF 02 04 77         [24]  794 	ljmp	00130$
      000502                        795 00124$:
                                    796 ;	simpleserial.c:115: else if((ascii_pos == length_limit)
      000502 EC               [12]  797 	mov	a,r4
      000503 B5 07 70         [24]  798 	cjne	a,ar7,00128$
                                    799 ;	simpleserial.c:116: && ((ascii_buffer[ascii_pos] == '\n') || (ascii_buffer[ascii_pos] == '\r')))
      000506 EC               [12]  800 	mov	a,r4
      000507 2A               [12]  801 	add	a,r2
      000508 F9               [12]  802 	mov	r1,a
      000509 87 07            [24]  803 	mov	ar7,@r1
      00050B BF 0A 02         [24]  804 	cjne	r7,#0x0a,00190$
      00050E 80 03            [24]  805 	sjmp	00119$
      000510                        806 00190$:
      000510 BF 0D 63         [24]  807 	cjne	r7,#0x0d,00128$
      000513                        808 00119$:
                                    809 ;	simpleserial.c:119: if(state == STATE_PT)
      000513 ED               [12]  810 	mov	a,r5
      000514 60 30            [24]  811 	jz	00117$
                                    812 ;	simpleserial.c:121: simpleserial_decode(ascii_buffer, input, size_input);
      000516 8A 07            [24]  813 	mov	ar7,r2
      000518 7E 00            [12]  814 	mov	r6,#0x00
      00051A 7D 40            [12]  815 	mov	r5,#0x40
      00051C E5 08            [12]  816 	mov	a,_bp
      00051E 24 FA            [12]  817 	add	a,#0xfa
      000520 F8               [12]  818 	mov	r0,a
      000521 E6               [12]  819 	mov	a,@r0
      000522 C0 E0            [24]  820 	push	acc
      000524 A8 08            [24]  821 	mov	r0,_bp
      000526 08               [12]  822 	inc	r0
      000527 E6               [12]  823 	mov	a,@r0
      000528 C0 E0            [24]  824 	push	acc
      00052A 08               [12]  825 	inc	r0
      00052B E6               [12]  826 	mov	a,@r0
      00052C C0 E0            [24]  827 	push	acc
      00052E 08               [12]  828 	inc	r0
      00052F E6               [12]  829 	mov	a,@r0
      000530 C0 E0            [24]  830 	push	acc
      000532 8F 82            [24]  831 	mov	dpl,r7
      000534 8E 83            [24]  832 	mov	dph,r6
      000536 8D F0            [24]  833 	mov	b,r5
      000538 12 02 80         [24]  834 	lcall	_simpleserial_decode
      00053B E5 81            [12]  835 	mov	a,sp
      00053D 24 FC            [12]  836 	add	a,#0xfc
      00053F F5 81            [12]  837 	mov	sp,a
                                    838 ;	simpleserial.c:122: return 1;
      000541 90 00 01         [24]  839 	mov	dptr,#0x0001
      000544 80 33            [24]  840 	sjmp	00132$
      000546                        841 00117$:
                                    842 ;	simpleserial.c:126: simpleserial_decode(ascii_buffer, key, size_key);
      000546 7F 00            [12]  843 	mov	r7,#0x00
      000548 7E 40            [12]  844 	mov	r6,#0x40
      00054A E5 08            [12]  845 	mov	a,_bp
      00054C 24 F9            [12]  846 	add	a,#0xf9
      00054E F8               [12]  847 	mov	r0,a
      00054F E6               [12]  848 	mov	a,@r0
      000550 C0 E0            [24]  849 	push	acc
      000552 E5 08            [12]  850 	mov	a,_bp
      000554 24 FB            [12]  851 	add	a,#0xfb
      000556 F8               [12]  852 	mov	r0,a
      000557 E6               [12]  853 	mov	a,@r0
      000558 C0 E0            [24]  854 	push	acc
      00055A 08               [12]  855 	inc	r0
      00055B E6               [12]  856 	mov	a,@r0
      00055C C0 E0            [24]  857 	push	acc
      00055E 08               [12]  858 	inc	r0
      00055F E6               [12]  859 	mov	a,@r0
      000560 C0 E0            [24]  860 	push	acc
      000562 8A 82            [24]  861 	mov	dpl,r2
      000564 8F 83            [24]  862 	mov	dph,r7
      000566 8E F0            [24]  863 	mov	b,r6
      000568 12 02 80         [24]  864 	lcall	_simpleserial_decode
      00056B E5 81            [12]  865 	mov	a,sp
      00056D 24 FC            [12]  866 	add	a,#0xfc
      00056F F5 81            [12]  867 	mov	sp,a
                                    868 ;	simpleserial.c:127: return 0;
      000571 90 00 00         [24]  869 	mov	dptr,#0x0000
      000574 80 03            [24]  870 	sjmp	00132$
      000576                        871 00128$:
                                    872 ;	simpleserial.c:134: return 0;
      000576 90 00 00         [24]  873 	mov	dptr,#0x0000
      000579                        874 00132$:
      000579 85 08 81         [24]  875 	mov	sp,_bp
      00057C D0 08            [24]  876 	pop	_bp
      00057E 22               [24]  877 	ret
                                    878 ;------------------------------------------------------------
                                    879 ;Allocation info for local variables in function 'simpleserial_put'
                                    880 ;------------------------------------------------------------
                                    881 ;size                      Allocated to stack - _bp -3
                                    882 ;output                    Allocated to stack - _bp +1
                                    883 ;ascii_buffer              Allocated to stack - _bp +4
                                    884 ;ascii_pos                 Allocated to registers r7 
                                    885 ;size_ascii                Allocated to registers r4 
                                    886 ;------------------------------------------------------------
                                    887 ;	simpleserial.c:138: void simpleserial_put(uint8_t* output, uint8_t size)
                                    888 ;	-----------------------------------------
                                    889 ;	 function simpleserial_put
                                    890 ;	-----------------------------------------
      00057F                        891 _simpleserial_put:
      00057F C0 08            [24]  892 	push	_bp
      000581 85 81 08         [24]  893 	mov	_bp,sp
      000584 C0 82            [24]  894 	push	dpl
      000586 C0 83            [24]  895 	push	dph
      000588 C0 F0            [24]  896 	push	b
      00058A E5 81            [12]  897 	mov	a,sp
      00058C 24 22            [12]  898 	add	a,#0x22
      00058E F5 81            [12]  899 	mov	sp,a
                                    900 ;	simpleserial.c:143: uint8_t size_ascii = 2*size+2;
      000590 E5 08            [12]  901 	mov	a,_bp
      000592 24 FD            [12]  902 	add	a,#0xfd
      000594 F8               [12]  903 	mov	r0,a
      000595 E6               [12]  904 	mov	a,@r0
      000596 25 E0            [12]  905 	add	a,acc
      000598 FC               [12]  906 	mov	r4,a
      000599 0C               [12]  907 	inc	r4
      00059A 0C               [12]  908 	inc	r4
                                    909 ;	simpleserial.c:145: simpleserial_encode(ascii_buffer, output, size);
      00059B E5 08            [12]  910 	mov	a,_bp
      00059D 24 04            [12]  911 	add	a,#0x04
      00059F FB               [12]  912 	mov	r3,a
      0005A0 FA               [12]  913 	mov	r2,a
      0005A1 7E 00            [12]  914 	mov	r6,#0x00
      0005A3 7F 40            [12]  915 	mov	r7,#0x40
      0005A5 C0 04            [24]  916 	push	ar4
      0005A7 C0 03            [24]  917 	push	ar3
      0005A9 E5 08            [12]  918 	mov	a,_bp
      0005AB 24 FD            [12]  919 	add	a,#0xfd
      0005AD F8               [12]  920 	mov	r0,a
      0005AE E6               [12]  921 	mov	a,@r0
      0005AF C0 E0            [24]  922 	push	acc
      0005B1 A8 08            [24]  923 	mov	r0,_bp
      0005B3 08               [12]  924 	inc	r0
      0005B4 E6               [12]  925 	mov	a,@r0
      0005B5 C0 E0            [24]  926 	push	acc
      0005B7 08               [12]  927 	inc	r0
      0005B8 E6               [12]  928 	mov	a,@r0
      0005B9 C0 E0            [24]  929 	push	acc
      0005BB 08               [12]  930 	inc	r0
      0005BC E6               [12]  931 	mov	a,@r0
      0005BD C0 E0            [24]  932 	push	acc
      0005BF 8A 82            [24]  933 	mov	dpl,r2
      0005C1 8E 83            [24]  934 	mov	dph,r6
      0005C3 8F F0            [24]  935 	mov	b,r7
      0005C5 12 01 72         [24]  936 	lcall	_simpleserial_encode
      0005C8 E5 81            [12]  937 	mov	a,sp
      0005CA 24 FC            [12]  938 	add	a,#0xfc
      0005CC F5 81            [12]  939 	mov	sp,a
      0005CE D0 03            [24]  940 	pop	ar3
      0005D0 D0 04            [24]  941 	pop	ar4
                                    942 ;	simpleserial.c:146: for(ascii_pos = 0; ascii_pos < size_ascii; ascii_pos++)
      0005D2 7F 00            [12]  943 	mov	r7,#0x00
      0005D4                        944 00103$:
      0005D4 C3               [12]  945 	clr	c
      0005D5 EF               [12]  946 	mov	a,r7
      0005D6 9C               [12]  947 	subb	a,r4
      0005D7 50 17            [24]  948 	jnc	00105$
                                    949 ;	simpleserial.c:148: putchar(ascii_buffer[ascii_pos]);
      0005D9 EF               [12]  950 	mov	a,r7
      0005DA 2B               [12]  951 	add	a,r3
      0005DB F9               [12]  952 	mov	r1,a
      0005DC 87 82            [24]  953 	mov	dpl,@r1
      0005DE C0 07            [24]  954 	push	ar7
      0005E0 C0 04            [24]  955 	push	ar4
      0005E2 C0 03            [24]  956 	push	ar3
      0005E4 12 00 7B         [24]  957 	lcall	_putchar
      0005E7 D0 03            [24]  958 	pop	ar3
      0005E9 D0 04            [24]  959 	pop	ar4
      0005EB D0 07            [24]  960 	pop	ar7
                                    961 ;	simpleserial.c:146: for(ascii_pos = 0; ascii_pos < size_ascii; ascii_pos++)
      0005ED 0F               [12]  962 	inc	r7
      0005EE 80 E4            [24]  963 	sjmp	00103$
      0005F0                        964 00105$:
      0005F0 85 08 81         [24]  965 	mov	sp,_bp
      0005F3 D0 08            [24]  966 	pop	_bp
      0005F5 22               [24]  967 	ret
                                    968 	.area CSEG    (CODE)
                                    969 	.area CONST   (CODE)
      000DB7                        970 _hex_lookup:
      000DB7 30                     971 	.db #0x30	; 48	'0'
      000DB8 31                     972 	.db #0x31	; 49	'1'
      000DB9 32                     973 	.db #0x32	; 50	'2'
      000DBA 33                     974 	.db #0x33	; 51	'3'
      000DBB 34                     975 	.db #0x34	; 52	'4'
      000DBC 35                     976 	.db #0x35	; 53	'5'
      000DBD 36                     977 	.db #0x36	; 54	'6'
      000DBE 37                     978 	.db #0x37	; 55	'7'
      000DBF 38                     979 	.db #0x38	; 56	'8'
      000DC0 39                     980 	.db #0x39	; 57	'9'
      000DC1 41                     981 	.db #0x41	; 65	'A'
      000DC2 42                     982 	.db #0x42	; 66	'B'
      000DC3 43                     983 	.db #0x43	; 67	'C'
      000DC4 44                     984 	.db #0x44	; 68	'D'
      000DC5 45                     985 	.db #0x45	; 69	'E'
      000DC6 46                     986 	.db #0x46	; 70	'F'
                                    987 	.area XINIT   (CODE)
                                    988 	.area CABS    (ABS,CODE)
