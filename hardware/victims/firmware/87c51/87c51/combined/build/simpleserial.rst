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
      000D9B                        108 _simpleserial_encode:
                           000007   109 	ar7 = 0x07
                           000006   110 	ar6 = 0x06
                           000005   111 	ar5 = 0x05
                           000004   112 	ar4 = 0x04
                           000003   113 	ar3 = 0x03
                           000002   114 	ar2 = 0x02
                           000001   115 	ar1 = 0x01
                           000000   116 	ar0 = 0x00
      000D9B C0 08            [24]  117 	push	_bp
      000D9D 85 81 08         [24]  118 	mov	_bp,sp
      000DA0 C0 82            [24]  119 	push	dpl
      000DA2 C0 83            [24]  120 	push	dph
      000DA4 C0 F0            [24]  121 	push	b
      000DA6 05 81            [12]  122 	inc	sp
      000DA8 05 81            [12]  123 	inc	sp
      000DAA 05 81            [12]  124 	inc	sp
                                    125 ;	simpleserial.c:23: ascii[0] = 'r';
      000DAC A8 08            [24]  126 	mov	r0,_bp
      000DAE 08               [12]  127 	inc	r0
      000DAF 86 82            [24]  128 	mov	dpl,@r0
      000DB1 08               [12]  129 	inc	r0
      000DB2 86 83            [24]  130 	mov	dph,@r0
      000DB4 08               [12]  131 	inc	r0
      000DB5 86 F0            [24]  132 	mov	b,@r0
      000DB7 74 72            [12]  133 	mov	a,#0x72
      000DB9 12 1B 7D         [24]  134 	lcall	__gptrput
                                    135 ;	simpleserial.c:24: for(i = 0; i < size; i++)
      000DBC E5 08            [12]  136 	mov	a,_bp
      000DBE 24 04            [12]  137 	add	a,#0x04
      000DC0 F8               [12]  138 	mov	r0,a
      000DC1 E4               [12]  139 	clr	a
      000DC2 F6               [12]  140 	mov	@r0,a
      000DC3 08               [12]  141 	inc	r0
      000DC4 F6               [12]  142 	mov	@r0,a
      000DC5                        143 00103$:
      000DC5 E5 08            [12]  144 	mov	a,_bp
      000DC7 24 FA            [12]  145 	add	a,#0xfa
      000DC9 F8               [12]  146 	mov	r0,a
      000DCA 86 02            [24]  147 	mov	ar2,@r0
      000DCC 7F 00            [12]  148 	mov	r7,#0x00
      000DCE E5 08            [12]  149 	mov	a,_bp
      000DD0 24 04            [12]  150 	add	a,#0x04
      000DD2 F8               [12]  151 	mov	r0,a
      000DD3 C3               [12]  152 	clr	c
      000DD4 E6               [12]  153 	mov	a,@r0
      000DD5 9A               [12]  154 	subb	a,r2
      000DD6 08               [12]  155 	inc	r0
      000DD7 E6               [12]  156 	mov	a,@r0
      000DD8 64 80            [12]  157 	xrl	a,#0x80
      000DDA 8F F0            [24]  158 	mov	b,r7
      000DDC 63 F0 80         [24]  159 	xrl	b,#0x80
      000DDF 95 F0            [12]  160 	subb	a,b
      000DE1 40 03            [24]  161 	jc	00114$
      000DE3 02 0E 7A         [24]  162 	ljmp	00101$
      000DE6                        163 00114$:
                                    164 ;	simpleserial.c:26: unsigned char n_hi = (data[i] >> 4) & 0x0f;
      000DE6 E5 08            [12]  165 	mov	a,_bp
      000DE8 24 FB            [12]  166 	add	a,#0xfb
      000DEA F8               [12]  167 	mov	r0,a
      000DEB E5 08            [12]  168 	mov	a,_bp
      000DED 24 04            [12]  169 	add	a,#0x04
      000DEF F9               [12]  170 	mov	r1,a
      000DF0 E7               [12]  171 	mov	a,@r1
      000DF1 26               [12]  172 	add	a,@r0
      000DF2 FA               [12]  173 	mov	r2,a
      000DF3 09               [12]  174 	inc	r1
      000DF4 E7               [12]  175 	mov	a,@r1
      000DF5 08               [12]  176 	inc	r0
      000DF6 36               [12]  177 	addc	a,@r0
      000DF7 FB               [12]  178 	mov	r3,a
      000DF8 08               [12]  179 	inc	r0
      000DF9 86 04            [24]  180 	mov	ar4,@r0
      000DFB 8A 82            [24]  181 	mov	dpl,r2
      000DFD 8B 83            [24]  182 	mov	dph,r3
      000DFF 8C F0            [24]  183 	mov	b,r4
      000E01 12 1C A1         [24]  184 	lcall	__gptrget
      000E04 FA               [12]  185 	mov	r2,a
      000E05 C4               [12]  186 	swap	a
      000E06 54 0F            [12]  187 	anl	a,#0x0f
      000E08 FD               [12]  188 	mov	r5,a
                                    189 ;	simpleserial.c:27: unsigned char n_lo =  data[i]       & 0x0f;
      000E09 E5 08            [12]  190 	mov	a,_bp
      000E0B 24 06            [12]  191 	add	a,#0x06
      000E0D F8               [12]  192 	mov	r0,a
      000E0E 74 0F            [12]  193 	mov	a,#0x0f
      000E10 5A               [12]  194 	anl	a,r2
      000E11 F6               [12]  195 	mov	@r0,a
                                    196 ;	simpleserial.c:28: ascii[2*i+1] = hex_lookup[n_hi];
      000E12 E5 08            [12]  197 	mov	a,_bp
      000E14 24 04            [12]  198 	add	a,#0x04
      000E16 F8               [12]  199 	mov	r0,a
      000E17 86 02            [24]  200 	mov	ar2,@r0
      000E19 08               [12]  201 	inc	r0
      000E1A E6               [12]  202 	mov	a,@r0
      000E1B CA               [12]  203 	xch	a,r2
      000E1C 25 E0            [12]  204 	add	a,acc
      000E1E CA               [12]  205 	xch	a,r2
      000E1F 33               [12]  206 	rlc	a
      000E20 FB               [12]  207 	mov	r3,a
      000E21 74 01            [12]  208 	mov	a,#0x01
      000E23 2A               [12]  209 	add	a,r2
      000E24 FC               [12]  210 	mov	r4,a
      000E25 E4               [12]  211 	clr	a
      000E26 3B               [12]  212 	addc	a,r3
      000E27 FF               [12]  213 	mov	r7,a
      000E28 A8 08            [24]  214 	mov	r0,_bp
      000E2A 08               [12]  215 	inc	r0
      000E2B EC               [12]  216 	mov	a,r4
      000E2C 26               [12]  217 	add	a,@r0
      000E2D FC               [12]  218 	mov	r4,a
      000E2E EF               [12]  219 	mov	a,r7
      000E2F 08               [12]  220 	inc	r0
      000E30 36               [12]  221 	addc	a,@r0
      000E31 FF               [12]  222 	mov	r7,a
      000E32 08               [12]  223 	inc	r0
      000E33 86 06            [24]  224 	mov	ar6,@r0
      000E35 ED               [12]  225 	mov	a,r5
      000E36 90 1E 9F         [24]  226 	mov	dptr,#_hex_lookup
      000E39 93               [24]  227 	movc	a,@a+dptr
      000E3A FD               [12]  228 	mov	r5,a
      000E3B 8C 82            [24]  229 	mov	dpl,r4
      000E3D 8F 83            [24]  230 	mov	dph,r7
      000E3F 8E F0            [24]  231 	mov	b,r6
      000E41 12 1B 7D         [24]  232 	lcall	__gptrput
                                    233 ;	simpleserial.c:29: ascii[2*i+2] = hex_lookup[n_lo];
      000E44 74 02            [12]  234 	mov	a,#0x02
      000E46 2A               [12]  235 	add	a,r2
      000E47 FA               [12]  236 	mov	r2,a
      000E48 E4               [12]  237 	clr	a
      000E49 3B               [12]  238 	addc	a,r3
      000E4A FB               [12]  239 	mov	r3,a
      000E4B A8 08            [24]  240 	mov	r0,_bp
      000E4D 08               [12]  241 	inc	r0
      000E4E EA               [12]  242 	mov	a,r2
      000E4F 26               [12]  243 	add	a,@r0
      000E50 FA               [12]  244 	mov	r2,a
      000E51 EB               [12]  245 	mov	a,r3
      000E52 08               [12]  246 	inc	r0
      000E53 36               [12]  247 	addc	a,@r0
      000E54 FB               [12]  248 	mov	r3,a
      000E55 08               [12]  249 	inc	r0
      000E56 86 07            [24]  250 	mov	ar7,@r0
      000E58 E5 08            [12]  251 	mov	a,_bp
      000E5A 24 06            [12]  252 	add	a,#0x06
      000E5C F8               [12]  253 	mov	r0,a
      000E5D E6               [12]  254 	mov	a,@r0
      000E5E 90 1E 9F         [24]  255 	mov	dptr,#_hex_lookup
      000E61 93               [24]  256 	movc	a,@a+dptr
      000E62 FE               [12]  257 	mov	r6,a
      000E63 8A 82            [24]  258 	mov	dpl,r2
      000E65 8B 83            [24]  259 	mov	dph,r3
      000E67 8F F0            [24]  260 	mov	b,r7
      000E69 12 1B 7D         [24]  261 	lcall	__gptrput
                                    262 ;	simpleserial.c:24: for(i = 0; i < size; i++)
      000E6C E5 08            [12]  263 	mov	a,_bp
      000E6E 24 04            [12]  264 	add	a,#0x04
      000E70 F8               [12]  265 	mov	r0,a
      000E71 06               [12]  266 	inc	@r0
      000E72 B6 00 02         [24]  267 	cjne	@r0,#0x00,00115$
      000E75 08               [12]  268 	inc	r0
      000E76 06               [12]  269 	inc	@r0
      000E77                        270 00115$:
      000E77 02 0D C5         [24]  271 	ljmp	00103$
      000E7A                        272 00101$:
                                    273 ;	simpleserial.c:31: ascii[2*size+1] = '\n';
      000E7A E5 08            [12]  274 	mov	a,_bp
      000E7C 24 FA            [12]  275 	add	a,#0xfa
      000E7E F8               [12]  276 	mov	r0,a
      000E7F E6               [12]  277 	mov	a,@r0
      000E80 75 F0 02         [24]  278 	mov	b,#0x02
      000E83 A4               [48]  279 	mul	ab
      000E84 24 01            [12]  280 	add	a,#0x01
      000E86 FE               [12]  281 	mov	r6,a
      000E87 E4               [12]  282 	clr	a
      000E88 35 F0            [12]  283 	addc	a,b
      000E8A FF               [12]  284 	mov	r7,a
      000E8B A8 08            [24]  285 	mov	r0,_bp
      000E8D 08               [12]  286 	inc	r0
      000E8E EE               [12]  287 	mov	a,r6
      000E8F 26               [12]  288 	add	a,@r0
      000E90 FE               [12]  289 	mov	r6,a
      000E91 EF               [12]  290 	mov	a,r7
      000E92 08               [12]  291 	inc	r0
      000E93 36               [12]  292 	addc	a,@r0
      000E94 FF               [12]  293 	mov	r7,a
      000E95 08               [12]  294 	inc	r0
      000E96 86 05            [24]  295 	mov	ar5,@r0
      000E98 8E 82            [24]  296 	mov	dpl,r6
      000E9A 8F 83            [24]  297 	mov	dph,r7
      000E9C 8D F0            [24]  298 	mov	b,r5
      000E9E 74 0A            [12]  299 	mov	a,#0x0a
      000EA0 12 1B 7D         [24]  300 	lcall	__gptrput
      000EA3 85 08 81         [24]  301 	mov	sp,_bp
      000EA6 D0 08            [24]  302 	pop	_bp
      000EA8 22               [24]  303 	ret
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
      000EA9                        318 _simpleserial_decode:
      000EA9 C0 08            [24]  319 	push	_bp
      000EAB 85 81 08         [24]  320 	mov	_bp,sp
      000EAE C0 82            [24]  321 	push	dpl
      000EB0 C0 83            [24]  322 	push	dph
      000EB2 C0 F0            [24]  323 	push	b
      000EB4 05 81            [12]  324 	inc	sp
      000EB6 05 81            [12]  325 	inc	sp
                                    326 ;	simpleserial.c:39: for(i = 0; i < size; i++)
      000EB8 7B 00            [12]  327 	mov	r3,#0x00
      000EBA 7C 00            [12]  328 	mov	r4,#0x00
      000EBC                        329 00115$:
      000EBC E5 08            [12]  330 	mov	a,_bp
      000EBE 24 FA            [12]  331 	add	a,#0xfa
      000EC0 F8               [12]  332 	mov	r0,a
      000EC1 86 02            [24]  333 	mov	ar2,@r0
      000EC3 7F 00            [12]  334 	mov	r7,#0x00
      000EC5 C3               [12]  335 	clr	c
      000EC6 EB               [12]  336 	mov	a,r3
      000EC7 9A               [12]  337 	subb	a,r2
      000EC8 EC               [12]  338 	mov	a,r4
      000EC9 64 80            [12]  339 	xrl	a,#0x80
      000ECB 8F F0            [24]  340 	mov	b,r7
      000ECD 63 F0 80         [24]  341 	xrl	b,#0x80
      000ED0 95 F0            [12]  342 	subb	a,b
      000ED2 40 03            [24]  343 	jc	00138$
      000ED4 02 10 81         [24]  344 	ljmp	00117$
      000ED7                        345 00138$:
                                    346 ;	simpleserial.c:41: unsigned char n_hi = ascii[2*i + 1];
      000ED7 8B 02            [24]  347 	mov	ar2,r3
      000ED9 EC               [12]  348 	mov	a,r4
      000EDA CA               [12]  349 	xch	a,r2
      000EDB 25 E0            [12]  350 	add	a,acc
      000EDD CA               [12]  351 	xch	a,r2
      000EDE 33               [12]  352 	rlc	a
      000EDF FF               [12]  353 	mov	r7,a
      000EE0 74 01            [12]  354 	mov	a,#0x01
      000EE2 2A               [12]  355 	add	a,r2
      000EE3 FD               [12]  356 	mov	r5,a
      000EE4 E4               [12]  357 	clr	a
      000EE5 3F               [12]  358 	addc	a,r7
      000EE6 FE               [12]  359 	mov	r6,a
      000EE7 C0 03            [24]  360 	push	ar3
      000EE9 C0 04            [24]  361 	push	ar4
      000EEB A8 08            [24]  362 	mov	r0,_bp
      000EED 08               [12]  363 	inc	r0
      000EEE ED               [12]  364 	mov	a,r5
      000EEF 26               [12]  365 	add	a,@r0
      000EF0 FD               [12]  366 	mov	r5,a
      000EF1 EE               [12]  367 	mov	a,r6
      000EF2 08               [12]  368 	inc	r0
      000EF3 36               [12]  369 	addc	a,@r0
      000EF4 FC               [12]  370 	mov	r4,a
      000EF5 08               [12]  371 	inc	r0
      000EF6 86 06            [24]  372 	mov	ar6,@r0
      000EF8 8D 82            [24]  373 	mov	dpl,r5
      000EFA 8C 83            [24]  374 	mov	dph,r4
      000EFC 8E F0            [24]  375 	mov	b,r6
      000EFE E5 08            [12]  376 	mov	a,_bp
      000F00 24 04            [12]  377 	add	a,#0x04
      000F02 F8               [12]  378 	mov	r0,a
      000F03 12 1C A1         [24]  379 	lcall	__gptrget
      000F06 F6               [12]  380 	mov	@r0,a
                                    381 ;	simpleserial.c:42: unsigned char n_lo = ascii[2*i + 2];
      000F07 74 02            [12]  382 	mov	a,#0x02
      000F09 2A               [12]  383 	add	a,r2
      000F0A FA               [12]  384 	mov	r2,a
      000F0B E4               [12]  385 	clr	a
      000F0C 3F               [12]  386 	addc	a,r7
      000F0D FF               [12]  387 	mov	r7,a
      000F0E A8 08            [24]  388 	mov	r0,_bp
      000F10 08               [12]  389 	inc	r0
      000F11 EA               [12]  390 	mov	a,r2
      000F12 26               [12]  391 	add	a,@r0
      000F13 FA               [12]  392 	mov	r2,a
      000F14 EF               [12]  393 	mov	a,r7
      000F15 08               [12]  394 	inc	r0
      000F16 36               [12]  395 	addc	a,@r0
      000F17 FF               [12]  396 	mov	r7,a
      000F18 08               [12]  397 	inc	r0
      000F19 86 06            [24]  398 	mov	ar6,@r0
      000F1B 8A 82            [24]  399 	mov	dpl,r2
      000F1D 8F 83            [24]  400 	mov	dph,r7
      000F1F 8E F0            [24]  401 	mov	b,r6
      000F21 E5 08            [12]  402 	mov	a,_bp
      000F23 24 05            [12]  403 	add	a,#0x05
      000F25 F8               [12]  404 	mov	r0,a
      000F26 12 1C A1         [24]  405 	lcall	__gptrget
      000F29 F6               [12]  406 	mov	@r0,a
                                    407 ;	simpleserial.c:44: if(n_lo <= '9')
      000F2A E5 08            [12]  408 	mov	a,_bp
      000F2C 24 05            [12]  409 	add	a,#0x05
      000F2E F8               [12]  410 	mov	r0,a
      000F2F C3               [12]  411 	clr	c
      000F30 74 39            [12]  412 	mov	a,#0x39
      000F32 96               [12]  413 	subb	a,@r0
      000F33 D0 04            [24]  414 	pop	ar4
      000F35 D0 03            [24]  415 	pop	ar3
      000F37 40 23            [24]  416 	jc	00105$
                                    417 ;	simpleserial.c:45: data[i] = n_lo - '0';
      000F39 E5 08            [12]  418 	mov	a,_bp
      000F3B 24 FB            [12]  419 	add	a,#0xfb
      000F3D F8               [12]  420 	mov	r0,a
      000F3E EB               [12]  421 	mov	a,r3
      000F3F 26               [12]  422 	add	a,@r0
      000F40 FD               [12]  423 	mov	r5,a
      000F41 EC               [12]  424 	mov	a,r4
      000F42 08               [12]  425 	inc	r0
      000F43 36               [12]  426 	addc	a,@r0
      000F44 FE               [12]  427 	mov	r6,a
      000F45 08               [12]  428 	inc	r0
      000F46 86 07            [24]  429 	mov	ar7,@r0
      000F48 E5 08            [12]  430 	mov	a,_bp
      000F4A 24 05            [12]  431 	add	a,#0x05
      000F4C F8               [12]  432 	mov	r0,a
      000F4D E6               [12]  433 	mov	a,@r0
      000F4E 24 D0            [12]  434 	add	a,#0xd0
      000F50 FA               [12]  435 	mov	r2,a
      000F51 8D 82            [24]  436 	mov	dpl,r5
      000F53 8E 83            [24]  437 	mov	dph,r6
      000F55 8F F0            [24]  438 	mov	b,r7
      000F57 12 1B 7D         [24]  439 	lcall	__gptrput
      000F5A 80 52            [24]  440 	sjmp	00106$
      000F5C                        441 00105$:
                                    442 ;	simpleserial.c:46: else if(n_lo <= 'F')
      000F5C E5 08            [12]  443 	mov	a,_bp
      000F5E 24 05            [12]  444 	add	a,#0x05
      000F60 F8               [12]  445 	mov	r0,a
      000F61 E6               [12]  446 	mov	a,@r0
      000F62 24 B9            [12]  447 	add	a,#0xff - 0x46
      000F64 40 25            [24]  448 	jc	00102$
                                    449 ;	simpleserial.c:47: data[i] = n_lo - 'A' + 10;
      000F66 E5 08            [12]  450 	mov	a,_bp
      000F68 24 FB            [12]  451 	add	a,#0xfb
      000F6A F8               [12]  452 	mov	r0,a
      000F6B EB               [12]  453 	mov	a,r3
      000F6C 26               [12]  454 	add	a,@r0
      000F6D FD               [12]  455 	mov	r5,a
      000F6E EC               [12]  456 	mov	a,r4
      000F6F 08               [12]  457 	inc	r0
      000F70 36               [12]  458 	addc	a,@r0
      000F71 FE               [12]  459 	mov	r6,a
      000F72 08               [12]  460 	inc	r0
      000F73 86 07            [24]  461 	mov	ar7,@r0
      000F75 E5 08            [12]  462 	mov	a,_bp
      000F77 24 05            [12]  463 	add	a,#0x05
      000F79 F8               [12]  464 	mov	r0,a
      000F7A 86 02            [24]  465 	mov	ar2,@r0
      000F7C 74 C9            [12]  466 	mov	a,#0xc9
      000F7E 2A               [12]  467 	add	a,r2
      000F7F FA               [12]  468 	mov	r2,a
      000F80 8D 82            [24]  469 	mov	dpl,r5
      000F82 8E 83            [24]  470 	mov	dph,r6
      000F84 8F F0            [24]  471 	mov	b,r7
      000F86 12 1B 7D         [24]  472 	lcall	__gptrput
      000F89 80 23            [24]  473 	sjmp	00106$
      000F8B                        474 00102$:
                                    475 ;	simpleserial.c:49: data[i] = n_lo - 'a' + 10;
      000F8B E5 08            [12]  476 	mov	a,_bp
      000F8D 24 FB            [12]  477 	add	a,#0xfb
      000F8F F8               [12]  478 	mov	r0,a
      000F90 EB               [12]  479 	mov	a,r3
      000F91 26               [12]  480 	add	a,@r0
      000F92 FD               [12]  481 	mov	r5,a
      000F93 EC               [12]  482 	mov	a,r4
      000F94 08               [12]  483 	inc	r0
      000F95 36               [12]  484 	addc	a,@r0
      000F96 FE               [12]  485 	mov	r6,a
      000F97 08               [12]  486 	inc	r0
      000F98 86 07            [24]  487 	mov	ar7,@r0
      000F9A E5 08            [12]  488 	mov	a,_bp
      000F9C 24 05            [12]  489 	add	a,#0x05
      000F9E F8               [12]  490 	mov	r0,a
      000F9F 86 02            [24]  491 	mov	ar2,@r0
      000FA1 74 A9            [12]  492 	mov	a,#0xa9
      000FA3 2A               [12]  493 	add	a,r2
      000FA4 FA               [12]  494 	mov	r2,a
      000FA5 8D 82            [24]  495 	mov	dpl,r5
      000FA7 8E 83            [24]  496 	mov	dph,r6
      000FA9 8F F0            [24]  497 	mov	b,r7
      000FAB 12 1B 7D         [24]  498 	lcall	__gptrput
      000FAE                        499 00106$:
                                    500 ;	simpleserial.c:51: if(n_hi <= '9')
      000FAE E5 08            [12]  501 	mov	a,_bp
      000FB0 24 04            [12]  502 	add	a,#0x04
      000FB2 F8               [12]  503 	mov	r0,a
      000FB3 E6               [12]  504 	mov	a,@r0
      000FB4 24 C6            [12]  505 	add	a,#0xff - 0x39
      000FB6 40 3D            [24]  506 	jc	00111$
                                    507 ;	simpleserial.c:52: data[i] |= (n_hi - '0') << 4;
      000FB8 E5 08            [12]  508 	mov	a,_bp
      000FBA 24 FB            [12]  509 	add	a,#0xfb
      000FBC F8               [12]  510 	mov	r0,a
      000FBD EB               [12]  511 	mov	a,r3
      000FBE 26               [12]  512 	add	a,@r0
      000FBF FD               [12]  513 	mov	r5,a
      000FC0 EC               [12]  514 	mov	a,r4
      000FC1 08               [12]  515 	inc	r0
      000FC2 36               [12]  516 	addc	a,@r0
      000FC3 FE               [12]  517 	mov	r6,a
      000FC4 08               [12]  518 	inc	r0
      000FC5 86 07            [24]  519 	mov	ar7,@r0
      000FC7 8D 82            [24]  520 	mov	dpl,r5
      000FC9 8E 83            [24]  521 	mov	dph,r6
      000FCB 8F F0            [24]  522 	mov	b,r7
      000FCD 12 1C A1         [24]  523 	lcall	__gptrget
      000FD0 FA               [12]  524 	mov	r2,a
      000FD1 C0 03            [24]  525 	push	ar3
      000FD3 C0 04            [24]  526 	push	ar4
      000FD5 E5 08            [12]  527 	mov	a,_bp
      000FD7 24 04            [12]  528 	add	a,#0x04
      000FD9 F8               [12]  529 	mov	r0,a
      000FDA E6               [12]  530 	mov	a,@r0
      000FDB 24 D0            [12]  531 	add	a,#0xd0
      000FDD C4               [12]  532 	swap	a
      000FDE 54 F0            [12]  533 	anl	a,#0xf0
      000FE0 FC               [12]  534 	mov	r4,a
      000FE1 EA               [12]  535 	mov	a,r2
      000FE2 42 04            [12]  536 	orl	ar4,a
      000FE4 8D 82            [24]  537 	mov	dpl,r5
      000FE6 8E 83            [24]  538 	mov	dph,r6
      000FE8 8F F0            [24]  539 	mov	b,r7
      000FEA EC               [12]  540 	mov	a,r4
      000FEB 12 1B 7D         [24]  541 	lcall	__gptrput
      000FEE D0 04            [24]  542 	pop	ar4
      000FF0 D0 03            [24]  543 	pop	ar3
      000FF2 02 10 79         [24]  544 	ljmp	00116$
      000FF5                        545 00111$:
                                    546 ;	simpleserial.c:53: else if(n_hi <= 'F')
      000FF5 E5 08            [12]  547 	mov	a,_bp
      000FF7 24 04            [12]  548 	add	a,#0x04
      000FF9 F8               [12]  549 	mov	r0,a
      000FFA E6               [12]  550 	mov	a,@r0
      000FFB 24 B9            [12]  551 	add	a,#0xff - 0x46
      000FFD 40 3E            [24]  552 	jc	00108$
                                    553 ;	simpleserial.c:54: data[i] |= (n_hi - 'A' + 10) << 4;
      000FFF E5 08            [12]  554 	mov	a,_bp
      001001 24 FB            [12]  555 	add	a,#0xfb
      001003 F8               [12]  556 	mov	r0,a
      001004 EB               [12]  557 	mov	a,r3
      001005 26               [12]  558 	add	a,@r0
      001006 FD               [12]  559 	mov	r5,a
      001007 EC               [12]  560 	mov	a,r4
      001008 08               [12]  561 	inc	r0
      001009 36               [12]  562 	addc	a,@r0
      00100A FE               [12]  563 	mov	r6,a
      00100B 08               [12]  564 	inc	r0
      00100C 86 07            [24]  565 	mov	ar7,@r0
      00100E 8D 82            [24]  566 	mov	dpl,r5
      001010 8E 83            [24]  567 	mov	dph,r6
      001012 8F F0            [24]  568 	mov	b,r7
      001014 12 1C A1         [24]  569 	lcall	__gptrget
      001017 FA               [12]  570 	mov	r2,a
      001018 C0 03            [24]  571 	push	ar3
      00101A C0 04            [24]  572 	push	ar4
      00101C E5 08            [12]  573 	mov	a,_bp
      00101E 24 04            [12]  574 	add	a,#0x04
      001020 F8               [12]  575 	mov	r0,a
      001021 86 04            [24]  576 	mov	ar4,@r0
      001023 74 C9            [12]  577 	mov	a,#0xc9
      001025 2C               [12]  578 	add	a,r4
      001026 C4               [12]  579 	swap	a
      001027 54 F0            [12]  580 	anl	a,#0xf0
      001029 FC               [12]  581 	mov	r4,a
      00102A EA               [12]  582 	mov	a,r2
      00102B 42 04            [12]  583 	orl	ar4,a
      00102D 8D 82            [24]  584 	mov	dpl,r5
      00102F 8E 83            [24]  585 	mov	dph,r6
      001031 8F F0            [24]  586 	mov	b,r7
      001033 EC               [12]  587 	mov	a,r4
      001034 12 1B 7D         [24]  588 	lcall	__gptrput
      001037 D0 04            [24]  589 	pop	ar4
      001039 D0 03            [24]  590 	pop	ar3
      00103B 80 3C            [24]  591 	sjmp	00116$
      00103D                        592 00108$:
                                    593 ;	simpleserial.c:56: data[i] |= (n_hi - 'a' + 10) << 4;
      00103D E5 08            [12]  594 	mov	a,_bp
      00103F 24 FB            [12]  595 	add	a,#0xfb
      001041 F8               [12]  596 	mov	r0,a
      001042 EB               [12]  597 	mov	a,r3
      001043 26               [12]  598 	add	a,@r0
      001044 FD               [12]  599 	mov	r5,a
      001045 EC               [12]  600 	mov	a,r4
      001046 08               [12]  601 	inc	r0
      001047 36               [12]  602 	addc	a,@r0
      001048 FE               [12]  603 	mov	r6,a
      001049 08               [12]  604 	inc	r0
      00104A 86 07            [24]  605 	mov	ar7,@r0
      00104C 8D 82            [24]  606 	mov	dpl,r5
      00104E 8E 83            [24]  607 	mov	dph,r6
      001050 8F F0            [24]  608 	mov	b,r7
      001052 12 1C A1         [24]  609 	lcall	__gptrget
      001055 FA               [12]  610 	mov	r2,a
      001056 C0 03            [24]  611 	push	ar3
      001058 C0 04            [24]  612 	push	ar4
      00105A E5 08            [12]  613 	mov	a,_bp
      00105C 24 04            [12]  614 	add	a,#0x04
      00105E F8               [12]  615 	mov	r0,a
      00105F 86 04            [24]  616 	mov	ar4,@r0
      001061 74 A9            [12]  617 	mov	a,#0xa9
      001063 2C               [12]  618 	add	a,r4
      001064 C4               [12]  619 	swap	a
      001065 54 F0            [12]  620 	anl	a,#0xf0
      001067 FC               [12]  621 	mov	r4,a
      001068 EA               [12]  622 	mov	a,r2
      001069 42 04            [12]  623 	orl	ar4,a
      00106B 8D 82            [24]  624 	mov	dpl,r5
      00106D 8E 83            [24]  625 	mov	dph,r6
      00106F 8F F0            [24]  626 	mov	b,r7
      001071 EC               [12]  627 	mov	a,r4
      001072 12 1B 7D         [24]  628 	lcall	__gptrput
                                    629 ;	simpleserial.c:39: for(i = 0; i < size; i++)
      001075 D0 04            [24]  630 	pop	ar4
      001077 D0 03            [24]  631 	pop	ar3
                                    632 ;	simpleserial.c:56: data[i] |= (n_hi - 'a' + 10) << 4;
      001079                        633 00116$:
                                    634 ;	simpleserial.c:39: for(i = 0; i < size; i++)
      001079 0B               [12]  635 	inc	r3
      00107A BB 00 01         [24]  636 	cjne	r3,#0x00,00143$
      00107D 0C               [12]  637 	inc	r4
      00107E                        638 00143$:
      00107E 02 0E BC         [24]  639 	ljmp	00115$
      001081                        640 00117$:
      001081 85 08 81         [24]  641 	mov	sp,_bp
      001084 D0 08            [24]  642 	pop	_bp
      001086 22               [24]  643 	ret
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
      001087                        661 _simpleserial_get:
      001087 C0 08            [24]  662 	push	_bp
      001089 85 81 08         [24]  663 	mov	_bp,sp
      00108C C0 82            [24]  664 	push	dpl
      00108E C0 83            [24]  665 	push	dph
      001090 C0 F0            [24]  666 	push	b
      001092 E5 81            [12]  667 	mov	a,sp
      001094 24 22            [12]  668 	add	a,#0x22
      001096 F5 81            [12]  669 	mov	sp,a
                                    670 ;	simpleserial.c:64: unsigned char ascii_pos = 0;
                                    671 ;	simpleserial.c:67: unsigned char state = STATE_IDLE;
                                    672 ;	simpleserial.c:70: while(1)
      001098 E4               [12]  673 	clr	a
      001099 FC               [12]  674 	mov	r4,a
      00109A FB               [12]  675 	mov	r3,a
      00109B E5 08            [12]  676 	mov	a,_bp
      00109D 24 04            [12]  677 	add	a,#0x04
      00109F FA               [12]  678 	mov	r2,a
      0010A0                        679 00130$:
                                    680 ;	simpleserial.c:73: ascii_buffer[ascii_pos] = getchar();
      0010A0 EC               [12]  681 	mov	a,r4
      0010A1 2A               [12]  682 	add	a,r2
      0010A2 F9               [12]  683 	mov	r1,a
      0010A3 C0 04            [24]  684 	push	ar4
      0010A5 C0 03            [24]  685 	push	ar3
      0010A7 C0 02            [24]  686 	push	ar2
      0010A9 C0 01            [24]  687 	push	ar1
      0010AB 12 00 A0         [24]  688 	lcall	_getchar
      0010AE E5 82            [12]  689 	mov	a,dpl
      0010B0 D0 01            [24]  690 	pop	ar1
      0010B2 D0 02            [24]  691 	pop	ar2
      0010B4 D0 03            [24]  692 	pop	ar3
      0010B6 D0 04            [24]  693 	pop	ar4
      0010B8 F7               [12]  694 	mov	@r1,a
                                    695 ;	simpleserial.c:75: if(state == STATE_IDLE)
      0010B9 EB               [12]  696 	mov	a,r3
      0010BA 70 19            [24]  697 	jnz	00127$
                                    698 ;	simpleserial.c:79: if(ascii_buffer[ascii_pos] == 'k')
      0010BC 87 07            [24]  699 	mov	ar7,@r1
      0010BE BF 6B 05         [24]  700 	cjne	r7,#0x6b,00104$
                                    701 ;	simpleserial.c:81: state = STATE_KEY;
      0010C1 7B 02            [12]  702 	mov	r3,#0x02
                                    703 ;	simpleserial.c:82: ascii_pos++;
      0010C3 0C               [12]  704 	inc	r4
                                    705 ;	simpleserial.c:83: continue;
      0010C4 80 DA            [24]  706 	sjmp	00130$
      0010C6                        707 00104$:
                                    708 ;	simpleserial.c:85: else if(ascii_buffer[ascii_pos] == 'p')
      0010C6 87 07            [24]  709 	mov	ar7,@r1
      0010C8 BF 70 02         [24]  710 	cjne	r7,#0x70,00173$
      0010CB 80 03            [24]  711 	sjmp	00174$
      0010CD                        712 00173$:
      0010CD 02 11 9F         [24]  713 	ljmp	00128$
      0010D0                        714 00174$:
                                    715 ;	simpleserial.c:87: state = STATE_PT;
      0010D0 7B 01            [12]  716 	mov	r3,#0x01
                                    717 ;	simpleserial.c:88: ascii_pos++;
      0010D2 0C               [12]  718 	inc	r4
                                    719 ;	simpleserial.c:89: continue;
      0010D3 80 CB            [24]  720 	sjmp	00130$
      0010D5                        721 00127$:
                                    722 ;	simpleserial.c:95: if(state == STATE_PT)
      0010D5 BB 01 03         [24]  723 	cjne	r3,#0x01,00175$
      0010D8 EB               [12]  724 	mov	a,r3
      0010D9 80 01            [24]  725 	sjmp	00176$
      0010DB                        726 00175$:
      0010DB E4               [12]  727 	clr	a
      0010DC                        728 00176$:
      0010DC FD               [12]  729 	mov	r5,a
      0010DD 60 09            [24]  730 	jz	00107$
                                    731 ;	simpleserial.c:96: length_limit = size_input;
      0010DF E5 08            [12]  732 	mov	a,_bp
      0010E1 24 FA            [12]  733 	add	a,#0xfa
      0010E3 F8               [12]  734 	mov	r0,a
      0010E4 86 07            [24]  735 	mov	ar7,@r0
      0010E6 80 07            [24]  736 	sjmp	00108$
      0010E8                        737 00107$:
                                    738 ;	simpleserial.c:98: length_limit = size_key;
      0010E8 E5 08            [12]  739 	mov	a,_bp
      0010EA 24 F9            [12]  740 	add	a,#0xf9
      0010EC F8               [12]  741 	mov	r0,a
      0010ED 86 07            [24]  742 	mov	ar7,@r0
      0010EF                        743 00108$:
                                    744 ;	simpleserial.c:99: length_limit = 2*length_limit + 1;
      0010EF EF               [12]  745 	mov	a,r7
      0010F0 2F               [12]  746 	add	a,r7
      0010F1 FE               [12]  747 	mov	r6,a
      0010F2 04               [12]  748 	inc	a
      0010F3 FF               [12]  749 	mov	r7,a
                                    750 ;	simpleserial.c:102: if(ascii_pos < length_limit)
      0010F4 C3               [12]  751 	clr	c
      0010F5 EC               [12]  752 	mov	a,r4
      0010F6 9F               [12]  753 	subb	a,r7
      0010F7 50 32            [24]  754 	jnc	00124$
                                    755 ;	simpleserial.c:104: if((ascii_buffer[ascii_pos] >= '0' && ascii_buffer[ascii_pos] <= '9')
      0010F9 EC               [12]  756 	mov	a,r4
      0010FA 2A               [12]  757 	add	a,r2
      0010FB F9               [12]  758 	mov	r1,a
      0010FC 87 06            [24]  759 	mov	ar6,@r1
      0010FE BE 30 00         [24]  760 	cjne	r6,#0x30,00179$
      001101                        761 00179$:
      001101 40 05            [24]  762 	jc	00113$
      001103 EE               [12]  763 	mov	a,r6
      001104 24 C6            [12]  764 	add	a,#0xff - 0x39
      001106 50 1F            [24]  765 	jnc	00109$
      001108                        766 00113$:
                                    767 ;	simpleserial.c:105: || (ascii_buffer[ascii_pos] >= 'A' && ascii_buffer[ascii_pos] <= 'F')
      001108 EC               [12]  768 	mov	a,r4
      001109 2A               [12]  769 	add	a,r2
      00110A F9               [12]  770 	mov	r1,a
      00110B 87 06            [24]  771 	mov	ar6,@r1
      00110D BE 41 00         [24]  772 	cjne	r6,#0x41,00182$
      001110                        773 00182$:
      001110 40 05            [24]  774 	jc	00115$
      001112 EE               [12]  775 	mov	a,r6
      001113 24 B9            [12]  776 	add	a,#0xff - 0x46
      001115 50 10            [24]  777 	jnc	00109$
      001117                        778 00115$:
                                    779 ;	simpleserial.c:106: || (ascii_buffer[ascii_pos] >= 'a' && ascii_buffer[ascii_pos] <= 'f')
      001117 BE 61 00         [24]  780 	cjne	r6,#0x61,00185$
      00111A                        781 00185$:
      00111A 50 03            [24]  782 	jnc	00186$
      00111C 02 11 9F         [24]  783 	ljmp	00128$
      00111F                        784 00186$:
      00111F EE               [12]  785 	mov	a,r6
      001120 24 99            [12]  786 	add	a,#0xff - 0x66
      001122 50 03            [24]  787 	jnc	00187$
      001124 02 11 9F         [24]  788 	ljmp	00128$
      001127                        789 00187$:
      001127                        790 00109$:
                                    791 ;	simpleserial.c:109: ascii_pos++;
      001127 0C               [12]  792 	inc	r4
                                    793 ;	simpleserial.c:110: continue;
      001128 02 10 A0         [24]  794 	ljmp	00130$
      00112B                        795 00124$:
                                    796 ;	simpleserial.c:115: else if((ascii_pos == length_limit)
      00112B EC               [12]  797 	mov	a,r4
      00112C B5 07 70         [24]  798 	cjne	a,ar7,00128$
                                    799 ;	simpleserial.c:116: && ((ascii_buffer[ascii_pos] == '\n') || (ascii_buffer[ascii_pos] == '\r')))
      00112F EC               [12]  800 	mov	a,r4
      001130 2A               [12]  801 	add	a,r2
      001131 F9               [12]  802 	mov	r1,a
      001132 87 07            [24]  803 	mov	ar7,@r1
      001134 BF 0A 02         [24]  804 	cjne	r7,#0x0a,00190$
      001137 80 03            [24]  805 	sjmp	00119$
      001139                        806 00190$:
      001139 BF 0D 63         [24]  807 	cjne	r7,#0x0d,00128$
      00113C                        808 00119$:
                                    809 ;	simpleserial.c:119: if(state == STATE_PT)
      00113C ED               [12]  810 	mov	a,r5
      00113D 60 30            [24]  811 	jz	00117$
                                    812 ;	simpleserial.c:121: simpleserial_decode(ascii_buffer, input, size_input);
      00113F 8A 07            [24]  813 	mov	ar7,r2
      001141 7E 00            [12]  814 	mov	r6,#0x00
      001143 7D 40            [12]  815 	mov	r5,#0x40
      001145 E5 08            [12]  816 	mov	a,_bp
      001147 24 FA            [12]  817 	add	a,#0xfa
      001149 F8               [12]  818 	mov	r0,a
      00114A E6               [12]  819 	mov	a,@r0
      00114B C0 E0            [24]  820 	push	acc
      00114D A8 08            [24]  821 	mov	r0,_bp
      00114F 08               [12]  822 	inc	r0
      001150 E6               [12]  823 	mov	a,@r0
      001151 C0 E0            [24]  824 	push	acc
      001153 08               [12]  825 	inc	r0
      001154 E6               [12]  826 	mov	a,@r0
      001155 C0 E0            [24]  827 	push	acc
      001157 08               [12]  828 	inc	r0
      001158 E6               [12]  829 	mov	a,@r0
      001159 C0 E0            [24]  830 	push	acc
      00115B 8F 82            [24]  831 	mov	dpl,r7
      00115D 8E 83            [24]  832 	mov	dph,r6
      00115F 8D F0            [24]  833 	mov	b,r5
      001161 12 0E A9         [24]  834 	lcall	_simpleserial_decode
      001164 E5 81            [12]  835 	mov	a,sp
      001166 24 FC            [12]  836 	add	a,#0xfc
      001168 F5 81            [12]  837 	mov	sp,a
                                    838 ;	simpleserial.c:122: return 1;
      00116A 90 00 01         [24]  839 	mov	dptr,#0x0001
      00116D 80 33            [24]  840 	sjmp	00132$
      00116F                        841 00117$:
                                    842 ;	simpleserial.c:126: simpleserial_decode(ascii_buffer, key, size_key);
      00116F 7F 00            [12]  843 	mov	r7,#0x00
      001171 7E 40            [12]  844 	mov	r6,#0x40
      001173 E5 08            [12]  845 	mov	a,_bp
      001175 24 F9            [12]  846 	add	a,#0xf9
      001177 F8               [12]  847 	mov	r0,a
      001178 E6               [12]  848 	mov	a,@r0
      001179 C0 E0            [24]  849 	push	acc
      00117B E5 08            [12]  850 	mov	a,_bp
      00117D 24 FB            [12]  851 	add	a,#0xfb
      00117F F8               [12]  852 	mov	r0,a
      001180 E6               [12]  853 	mov	a,@r0
      001181 C0 E0            [24]  854 	push	acc
      001183 08               [12]  855 	inc	r0
      001184 E6               [12]  856 	mov	a,@r0
      001185 C0 E0            [24]  857 	push	acc
      001187 08               [12]  858 	inc	r0
      001188 E6               [12]  859 	mov	a,@r0
      001189 C0 E0            [24]  860 	push	acc
      00118B 8A 82            [24]  861 	mov	dpl,r2
      00118D 8F 83            [24]  862 	mov	dph,r7
      00118F 8E F0            [24]  863 	mov	b,r6
      001191 12 0E A9         [24]  864 	lcall	_simpleserial_decode
      001194 E5 81            [12]  865 	mov	a,sp
      001196 24 FC            [12]  866 	add	a,#0xfc
      001198 F5 81            [12]  867 	mov	sp,a
                                    868 ;	simpleserial.c:127: return 0;
      00119A 90 00 00         [24]  869 	mov	dptr,#0x0000
      00119D 80 03            [24]  870 	sjmp	00132$
      00119F                        871 00128$:
                                    872 ;	simpleserial.c:134: return 0;
      00119F 90 00 00         [24]  873 	mov	dptr,#0x0000
      0011A2                        874 00132$:
      0011A2 85 08 81         [24]  875 	mov	sp,_bp
      0011A5 D0 08            [24]  876 	pop	_bp
      0011A7 22               [24]  877 	ret
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
      0011A8                        891 _simpleserial_put:
      0011A8 C0 08            [24]  892 	push	_bp
      0011AA 85 81 08         [24]  893 	mov	_bp,sp
      0011AD C0 82            [24]  894 	push	dpl
      0011AF C0 83            [24]  895 	push	dph
      0011B1 C0 F0            [24]  896 	push	b
      0011B3 E5 81            [12]  897 	mov	a,sp
      0011B5 24 22            [12]  898 	add	a,#0x22
      0011B7 F5 81            [12]  899 	mov	sp,a
                                    900 ;	simpleserial.c:143: uint8_t size_ascii = 2*size+2;
      0011B9 E5 08            [12]  901 	mov	a,_bp
      0011BB 24 FD            [12]  902 	add	a,#0xfd
      0011BD F8               [12]  903 	mov	r0,a
      0011BE E6               [12]  904 	mov	a,@r0
      0011BF 25 E0            [12]  905 	add	a,acc
      0011C1 FC               [12]  906 	mov	r4,a
      0011C2 0C               [12]  907 	inc	r4
      0011C3 0C               [12]  908 	inc	r4
                                    909 ;	simpleserial.c:145: simpleserial_encode(ascii_buffer, output, size);
      0011C4 E5 08            [12]  910 	mov	a,_bp
      0011C6 24 04            [12]  911 	add	a,#0x04
      0011C8 FB               [12]  912 	mov	r3,a
      0011C9 FA               [12]  913 	mov	r2,a
      0011CA 7E 00            [12]  914 	mov	r6,#0x00
      0011CC 7F 40            [12]  915 	mov	r7,#0x40
      0011CE C0 04            [24]  916 	push	ar4
      0011D0 C0 03            [24]  917 	push	ar3
      0011D2 E5 08            [12]  918 	mov	a,_bp
      0011D4 24 FD            [12]  919 	add	a,#0xfd
      0011D6 F8               [12]  920 	mov	r0,a
      0011D7 E6               [12]  921 	mov	a,@r0
      0011D8 C0 E0            [24]  922 	push	acc
      0011DA A8 08            [24]  923 	mov	r0,_bp
      0011DC 08               [12]  924 	inc	r0
      0011DD E6               [12]  925 	mov	a,@r0
      0011DE C0 E0            [24]  926 	push	acc
      0011E0 08               [12]  927 	inc	r0
      0011E1 E6               [12]  928 	mov	a,@r0
      0011E2 C0 E0            [24]  929 	push	acc
      0011E4 08               [12]  930 	inc	r0
      0011E5 E6               [12]  931 	mov	a,@r0
      0011E6 C0 E0            [24]  932 	push	acc
      0011E8 8A 82            [24]  933 	mov	dpl,r2
      0011EA 8E 83            [24]  934 	mov	dph,r6
      0011EC 8F F0            [24]  935 	mov	b,r7
      0011EE 12 0D 9B         [24]  936 	lcall	_simpleserial_encode
      0011F1 E5 81            [12]  937 	mov	a,sp
      0011F3 24 FC            [12]  938 	add	a,#0xfc
      0011F5 F5 81            [12]  939 	mov	sp,a
      0011F7 D0 03            [24]  940 	pop	ar3
      0011F9 D0 04            [24]  941 	pop	ar4
                                    942 ;	simpleserial.c:146: for(ascii_pos = 0; ascii_pos < size_ascii; ascii_pos++)
      0011FB 7F 00            [12]  943 	mov	r7,#0x00
      0011FD                        944 00103$:
      0011FD C3               [12]  945 	clr	c
      0011FE EF               [12]  946 	mov	a,r7
      0011FF 9C               [12]  947 	subb	a,r4
      001200 50 17            [24]  948 	jnc	00105$
                                    949 ;	simpleserial.c:148: putchar(ascii_buffer[ascii_pos]);
      001202 EF               [12]  950 	mov	a,r7
      001203 2B               [12]  951 	add	a,r3
      001204 F9               [12]  952 	mov	r1,a
      001205 87 82            [24]  953 	mov	dpl,@r1
      001207 C0 07            [24]  954 	push	ar7
      001209 C0 04            [24]  955 	push	ar4
      00120B C0 03            [24]  956 	push	ar3
      00120D 12 00 7B         [24]  957 	lcall	_putchar
      001210 D0 03            [24]  958 	pop	ar3
      001212 D0 04            [24]  959 	pop	ar4
      001214 D0 07            [24]  960 	pop	ar7
                                    961 ;	simpleserial.c:146: for(ascii_pos = 0; ascii_pos < size_ascii; ascii_pos++)
      001216 0F               [12]  962 	inc	r7
      001217 80 E4            [24]  963 	sjmp	00103$
      001219                        964 00105$:
      001219 85 08 81         [24]  965 	mov	sp,_bp
      00121C D0 08            [24]  966 	pop	_bp
      00121E 22               [24]  967 	ret
                                    968 	.area CSEG    (CODE)
                                    969 	.area CONST   (CODE)
      001E9F                        970 _hex_lookup:
      001E9F 30                     971 	.db #0x30	; 48	'0'
      001EA0 31                     972 	.db #0x31	; 49	'1'
      001EA1 32                     973 	.db #0x32	; 50	'2'
      001EA2 33                     974 	.db #0x33	; 51	'3'
      001EA3 34                     975 	.db #0x34	; 52	'4'
      001EA4 35                     976 	.db #0x35	; 53	'5'
      001EA5 36                     977 	.db #0x36	; 54	'6'
      001EA6 37                     978 	.db #0x37	; 55	'7'
      001EA7 38                     979 	.db #0x38	; 56	'8'
      001EA8 39                     980 	.db #0x39	; 57	'9'
      001EA9 41                     981 	.db #0x41	; 65	'A'
      001EAA 42                     982 	.db #0x42	; 66	'B'
      001EAB 43                     983 	.db #0x43	; 67	'C'
      001EAC 44                     984 	.db #0x44	; 68	'D'
      001EAD 45                     985 	.db #0x45	; 69	'E'
      001EAE 46                     986 	.db #0x46	; 70	'F'
                                    987 	.area XINIT   (CODE)
                                    988 	.area CABS    (ABS,CODE)
