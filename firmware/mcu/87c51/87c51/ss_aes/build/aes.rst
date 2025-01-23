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
                                     11 	.globl _update_key
                                     12 	.globl _mix_columns
                                     13 	.globl _shift_rows
                                     14 	.globl _sub_bytes
                                     15 	.globl _add_round_key
                                     16 	.globl _xtime
                                     17 	.globl _aes_encrypt
                                     18 ;--------------------------------------------------------
                                     19 ; special function registers
                                     20 ;--------------------------------------------------------
                                     21 	.area RSEG    (ABS,DATA)
      000000                         22 	.org 0x0000
                                     23 ;--------------------------------------------------------
                                     24 ; special function bits
                                     25 ;--------------------------------------------------------
                                     26 	.area RSEG    (ABS,DATA)
      000000                         27 	.org 0x0000
                                     28 ;--------------------------------------------------------
                                     29 ; overlayable register banks
                                     30 ;--------------------------------------------------------
                                     31 	.area REG_BANK_0	(REL,OVR,DATA)
      000000                         32 	.ds 8
                                     33 ;--------------------------------------------------------
                                     34 ; internal ram data
                                     35 ;--------------------------------------------------------
                                     36 	.area DSEG    (DATA)
                                     37 ;--------------------------------------------------------
                                     38 ; overlayable items in internal ram 
                                     39 ;--------------------------------------------------------
                                     40 ;--------------------------------------------------------
                                     41 ; indirectly addressable internal ram data
                                     42 ;--------------------------------------------------------
                                     43 	.area ISEG    (DATA)
                                     44 ;--------------------------------------------------------
                                     45 ; absolute internal ram data
                                     46 ;--------------------------------------------------------
                                     47 	.area IABS    (ABS,DATA)
                                     48 	.area IABS    (ABS,DATA)
                                     49 ;--------------------------------------------------------
                                     50 ; bit data
                                     51 ;--------------------------------------------------------
                                     52 	.area BSEG    (BIT)
                                     53 ;--------------------------------------------------------
                                     54 ; paged external ram data
                                     55 ;--------------------------------------------------------
                                     56 	.area PSEG    (PAG,XDATA)
                                     57 ;--------------------------------------------------------
                                     58 ; external ram data
                                     59 ;--------------------------------------------------------
                                     60 	.area XSEG    (XDATA)
                                     61 ;--------------------------------------------------------
                                     62 ; absolute external ram data
                                     63 ;--------------------------------------------------------
                                     64 	.area XABS    (ABS,XDATA)
                                     65 ;--------------------------------------------------------
                                     66 ; external initialized ram data
                                     67 ;--------------------------------------------------------
                                     68 	.area XISEG   (XDATA)
                                     69 	.area HOME    (CODE)
                                     70 	.area GSINIT0 (CODE)
                                     71 	.area GSINIT1 (CODE)
                                     72 	.area GSINIT2 (CODE)
                                     73 	.area GSINIT3 (CODE)
                                     74 	.area GSINIT4 (CODE)
                                     75 	.area GSINIT5 (CODE)
                                     76 	.area GSINIT  (CODE)
                                     77 	.area GSFINAL (CODE)
                                     78 	.area CSEG    (CODE)
                                     79 ;--------------------------------------------------------
                                     80 ; global & static initialisations
                                     81 ;--------------------------------------------------------
                                     82 	.area HOME    (CODE)
                                     83 	.area GSINIT  (CODE)
                                     84 	.area GSFINAL (CODE)
                                     85 	.area GSINIT  (CODE)
                                     86 ;--------------------------------------------------------
                                     87 ; Home
                                     88 ;--------------------------------------------------------
                                     89 	.area HOME    (CODE)
                                     90 	.area HOME    (CODE)
                                     91 ;--------------------------------------------------------
                                     92 ; code
                                     93 ;--------------------------------------------------------
                                     94 	.area CSEG    (CODE)
                                     95 ;------------------------------------------------------------
                                     96 ;Allocation info for local variables in function 'xtime'
                                     97 ;------------------------------------------------------------
                                     98 ;x                         Allocated to registers r7 
                                     99 ;------------------------------------------------------------
                                    100 ;	aes.c:70: uint8_t xtime(uint8_t x)
                                    101 ;	-----------------------------------------
                                    102 ;	 function xtime
                                    103 ;	-----------------------------------------
      000165                        104 _xtime:
                           000007   105 	ar7 = 0x07
                           000006   106 	ar6 = 0x06
                           000005   107 	ar5 = 0x05
                           000004   108 	ar4 = 0x04
                           000003   109 	ar3 = 0x03
                           000002   110 	ar2 = 0x02
                           000001   111 	ar1 = 0x01
                           000000   112 	ar0 = 0x00
                                    113 ;	aes.c:72: return ((x<<1) ^ (((x>>7) & 1) * 0x1b));
      000165 E5 82            [12]  114 	mov	a,dpl
      000167 FF               [12]  115 	mov	r7,a
      000168 25 E0            [12]  116 	add	a,acc
      00016A FE               [12]  117 	mov	r6,a
      00016B EF               [12]  118 	mov	a,r7
      00016C 23               [12]  119 	rl	a
      00016D 54 01            [12]  120 	anl	a,#0x01
      00016F 75 F0 1B         [24]  121 	mov	b,#0x1b
      000172 A4               [48]  122 	mul	ab
      000173 6E               [12]  123 	xrl	a,r6
      000174 F5 82            [12]  124 	mov	dpl,a
      000176 22               [24]  125 	ret
                                    126 ;------------------------------------------------------------
                                    127 ;Allocation info for local variables in function 'add_round_key'
                                    128 ;------------------------------------------------------------
                                    129 ;w                         Allocated to stack - _bp -5
                                    130 ;state                     Allocated to stack - _bp +2
                                    131 ;i                         Allocated to stack - _bp +5
                                    132 ;sloc0                     Allocated to stack - _bp +1
                                    133 ;------------------------------------------------------------
                                    134 ;	aes.c:76: void add_round_key(uint8_t* state, uint8_t* w)
                                    135 ;	-----------------------------------------
                                    136 ;	 function add_round_key
                                    137 ;	-----------------------------------------
      000177                        138 _add_round_key:
      000177 C0 08            [24]  139 	push	_bp
      000179 E5 81            [12]  140 	mov	a,sp
      00017B F5 08            [12]  141 	mov	_bp,a
      00017D 24 05            [12]  142 	add	a,#0x05
      00017F F5 81            [12]  143 	mov	sp,a
      000181 A8 08            [24]  144 	mov	r0,_bp
      000183 08               [12]  145 	inc	r0
      000184 08               [12]  146 	inc	r0
      000185 A6 82            [24]  147 	mov	@r0,dpl
      000187 08               [12]  148 	inc	r0
      000188 A6 83            [24]  149 	mov	@r0,dph
      00018A 08               [12]  150 	inc	r0
      00018B A6 F0            [24]  151 	mov	@r0,b
                                    152 ;	aes.c:79: for(i = 0; i < KEYLEN; i++)
      00018D E5 08            [12]  153 	mov	a,_bp
      00018F 24 05            [12]  154 	add	a,#0x05
      000191 F8               [12]  155 	mov	r0,a
      000192 76 00            [12]  156 	mov	@r0,#0x00
      000194                        157 00102$:
                                    158 ;	aes.c:80: state[i] ^= w[i];
      000194 A8 08            [24]  159 	mov	r0,_bp
      000196 08               [12]  160 	inc	r0
      000197 08               [12]  161 	inc	r0
      000198 E5 08            [12]  162 	mov	a,_bp
      00019A 24 05            [12]  163 	add	a,#0x05
      00019C F9               [12]  164 	mov	r1,a
      00019D E7               [12]  165 	mov	a,@r1
      00019E 26               [12]  166 	add	a,@r0
      00019F FA               [12]  167 	mov	r2,a
      0001A0 E4               [12]  168 	clr	a
      0001A1 08               [12]  169 	inc	r0
      0001A2 36               [12]  170 	addc	a,@r0
      0001A3 FB               [12]  171 	mov	r3,a
      0001A4 08               [12]  172 	inc	r0
      0001A5 86 07            [24]  173 	mov	ar7,@r0
      0001A7 8A 82            [24]  174 	mov	dpl,r2
      0001A9 8B 83            [24]  175 	mov	dph,r3
      0001AB 8F F0            [24]  176 	mov	b,r7
      0001AD A8 08            [24]  177 	mov	r0,_bp
      0001AF 08               [12]  178 	inc	r0
      0001B0 12 0F 4C         [24]  179 	lcall	__gptrget
      0001B3 F6               [12]  180 	mov	@r0,a
      0001B4 E5 08            [12]  181 	mov	a,_bp
      0001B6 24 FB            [12]  182 	add	a,#0xfb
      0001B8 F8               [12]  183 	mov	r0,a
      0001B9 E5 08            [12]  184 	mov	a,_bp
      0001BB 24 05            [12]  185 	add	a,#0x05
      0001BD F9               [12]  186 	mov	r1,a
      0001BE E7               [12]  187 	mov	a,@r1
      0001BF 26               [12]  188 	add	a,@r0
      0001C0 FC               [12]  189 	mov	r4,a
      0001C1 E4               [12]  190 	clr	a
      0001C2 08               [12]  191 	inc	r0
      0001C3 36               [12]  192 	addc	a,@r0
      0001C4 FD               [12]  193 	mov	r5,a
      0001C5 08               [12]  194 	inc	r0
      0001C6 86 06            [24]  195 	mov	ar6,@r0
      0001C8 8C 82            [24]  196 	mov	dpl,r4
      0001CA 8D 83            [24]  197 	mov	dph,r5
      0001CC 8E F0            [24]  198 	mov	b,r6
      0001CE 12 0F 4C         [24]  199 	lcall	__gptrget
      0001D1 FC               [12]  200 	mov	r4,a
      0001D2 A8 08            [24]  201 	mov	r0,_bp
      0001D4 08               [12]  202 	inc	r0
      0001D5 E6               [12]  203 	mov	a,@r0
      0001D6 62 04            [12]  204 	xrl	ar4,a
      0001D8 8A 82            [24]  205 	mov	dpl,r2
      0001DA 8B 83            [24]  206 	mov	dph,r3
      0001DC 8F F0            [24]  207 	mov	b,r7
      0001DE EC               [12]  208 	mov	a,r4
      0001DF 12 0F 31         [24]  209 	lcall	__gptrput
                                    210 ;	aes.c:79: for(i = 0; i < KEYLEN; i++)
      0001E2 E5 08            [12]  211 	mov	a,_bp
      0001E4 24 05            [12]  212 	add	a,#0x05
      0001E6 F8               [12]  213 	mov	r0,a
      0001E7 06               [12]  214 	inc	@r0
      0001E8 E5 08            [12]  215 	mov	a,_bp
      0001EA 24 05            [12]  216 	add	a,#0x05
      0001EC F8               [12]  217 	mov	r0,a
      0001ED B6 10 00         [24]  218 	cjne	@r0,#0x10,00110$
      0001F0                        219 00110$:
      0001F0 40 A2            [24]  220 	jc	00102$
      0001F2 85 08 81         [24]  221 	mov	sp,_bp
      0001F5 D0 08            [24]  222 	pop	_bp
      0001F7 22               [24]  223 	ret
                                    224 ;------------------------------------------------------------
                                    225 ;Allocation info for local variables in function 'sub_bytes'
                                    226 ;------------------------------------------------------------
                                    227 ;state                     Allocated to registers r5 r6 r7 
                                    228 ;i                         Allocated to registers r4 
                                    229 ;------------------------------------------------------------
                                    230 ;	aes.c:83: void sub_bytes(uint8_t* state)
                                    231 ;	-----------------------------------------
                                    232 ;	 function sub_bytes
                                    233 ;	-----------------------------------------
      0001F8                        234 _sub_bytes:
      0001F8 AD 82            [24]  235 	mov	r5,dpl
      0001FA AE 83            [24]  236 	mov	r6,dph
      0001FC AF F0            [24]  237 	mov	r7,b
                                    238 ;	aes.c:86: for(i = 0; i < KEYLEN; i++)
      0001FE 7C 00            [12]  239 	mov	r4,#0x00
      000200                        240 00102$:
                                    241 ;	aes.c:87: state[i] = s_box[state[i]];
      000200 EC               [12]  242 	mov	a,r4
      000201 2D               [12]  243 	add	a,r5
      000202 F9               [12]  244 	mov	r1,a
      000203 E4               [12]  245 	clr	a
      000204 3E               [12]  246 	addc	a,r6
      000205 FA               [12]  247 	mov	r2,a
      000206 8F 03            [24]  248 	mov	ar3,r7
      000208 89 82            [24]  249 	mov	dpl,r1
      00020A 8A 83            [24]  250 	mov	dph,r2
      00020C 8B F0            [24]  251 	mov	b,r3
      00020E 12 0F 4C         [24]  252 	lcall	__gptrget
      000211 90 0F 6C         [24]  253 	mov	dptr,#_s_box
      000214 93               [24]  254 	movc	a,@a+dptr
      000215 F8               [12]  255 	mov	r0,a
      000216 89 82            [24]  256 	mov	dpl,r1
      000218 8A 83            [24]  257 	mov	dph,r2
      00021A 8B F0            [24]  258 	mov	b,r3
      00021C 12 0F 31         [24]  259 	lcall	__gptrput
                                    260 ;	aes.c:86: for(i = 0; i < KEYLEN; i++)
      00021F 0C               [12]  261 	inc	r4
      000220 BC 10 00         [24]  262 	cjne	r4,#0x10,00110$
      000223                        263 00110$:
      000223 40 DB            [24]  264 	jc	00102$
      000225 22               [24]  265 	ret
                                    266 ;------------------------------------------------------------
                                    267 ;Allocation info for local variables in function 'shift_rows'
                                    268 ;------------------------------------------------------------
                                    269 ;state                     Allocated to stack - _bp +1
                                    270 ;temp                      Allocated to stack - _bp +7
                                    271 ;sloc0                     Allocated to stack - _bp +4
                                    272 ;------------------------------------------------------------
                                    273 ;	aes.c:90: void shift_rows(uint8_t* state)
                                    274 ;	-----------------------------------------
                                    275 ;	 function shift_rows
                                    276 ;	-----------------------------------------
      000226                        277 _shift_rows:
      000226 C0 08            [24]  278 	push	_bp
      000228 85 81 08         [24]  279 	mov	_bp,sp
      00022B C0 82            [24]  280 	push	dpl
      00022D C0 83            [24]  281 	push	dph
      00022F C0 F0            [24]  282 	push	b
      000231 E5 81            [12]  283 	mov	a,sp
      000233 24 04            [12]  284 	add	a,#0x04
      000235 F5 81            [12]  285 	mov	sp,a
                                    286 ;	aes.c:93: temp = state[1];
      000237 A8 08            [24]  287 	mov	r0,_bp
      000239 08               [12]  288 	inc	r0
      00023A 74 01            [12]  289 	mov	a,#0x01
      00023C 26               [12]  290 	add	a,@r0
      00023D FA               [12]  291 	mov	r2,a
      00023E E4               [12]  292 	clr	a
      00023F 08               [12]  293 	inc	r0
      000240 36               [12]  294 	addc	a,@r0
      000241 FB               [12]  295 	mov	r3,a
      000242 08               [12]  296 	inc	r0
      000243 86 04            [24]  297 	mov	ar4,@r0
      000245 8A 82            [24]  298 	mov	dpl,r2
      000247 8B 83            [24]  299 	mov	dph,r3
      000249 8C F0            [24]  300 	mov	b,r4
      00024B E5 08            [12]  301 	mov	a,_bp
      00024D 24 07            [12]  302 	add	a,#0x07
      00024F F8               [12]  303 	mov	r0,a
      000250 12 0F 4C         [24]  304 	lcall	__gptrget
      000253 F6               [12]  305 	mov	@r0,a
                                    306 ;	aes.c:94: state[1] = state[5];
      000254 A8 08            [24]  307 	mov	r0,_bp
      000256 08               [12]  308 	inc	r0
      000257 E5 08            [12]  309 	mov	a,_bp
      000259 24 04            [12]  310 	add	a,#0x04
      00025B F9               [12]  311 	mov	r1,a
      00025C 74 05            [12]  312 	mov	a,#0x05
      00025E 26               [12]  313 	add	a,@r0
      00025F F7               [12]  314 	mov	@r1,a
      000260 E4               [12]  315 	clr	a
      000261 08               [12]  316 	inc	r0
      000262 36               [12]  317 	addc	a,@r0
      000263 09               [12]  318 	inc	r1
      000264 F7               [12]  319 	mov	@r1,a
      000265 08               [12]  320 	inc	r0
      000266 09               [12]  321 	inc	r1
      000267 E6               [12]  322 	mov	a,@r0
      000268 F7               [12]  323 	mov	@r1,a
      000269 E5 08            [12]  324 	mov	a,_bp
      00026B 24 04            [12]  325 	add	a,#0x04
      00026D F8               [12]  326 	mov	r0,a
      00026E 86 82            [24]  327 	mov	dpl,@r0
      000270 08               [12]  328 	inc	r0
      000271 86 83            [24]  329 	mov	dph,@r0
      000273 08               [12]  330 	inc	r0
      000274 86 F0            [24]  331 	mov	b,@r0
      000276 12 0F 4C         [24]  332 	lcall	__gptrget
      000279 8A 82            [24]  333 	mov	dpl,r2
      00027B 8B 83            [24]  334 	mov	dph,r3
      00027D 8C F0            [24]  335 	mov	b,r4
      00027F 12 0F 31         [24]  336 	lcall	__gptrput
                                    337 ;	aes.c:95: state[5] = state[9];
      000282 A8 08            [24]  338 	mov	r0,_bp
      000284 08               [12]  339 	inc	r0
      000285 74 09            [12]  340 	mov	a,#0x09
      000287 26               [12]  341 	add	a,@r0
      000288 FD               [12]  342 	mov	r5,a
      000289 E4               [12]  343 	clr	a
      00028A 08               [12]  344 	inc	r0
      00028B 36               [12]  345 	addc	a,@r0
      00028C FE               [12]  346 	mov	r6,a
      00028D 08               [12]  347 	inc	r0
      00028E 86 07            [24]  348 	mov	ar7,@r0
      000290 8D 82            [24]  349 	mov	dpl,r5
      000292 8E 83            [24]  350 	mov	dph,r6
      000294 8F F0            [24]  351 	mov	b,r7
      000296 12 0F 4C         [24]  352 	lcall	__gptrget
      000299 FC               [12]  353 	mov	r4,a
      00029A E5 08            [12]  354 	mov	a,_bp
      00029C 24 04            [12]  355 	add	a,#0x04
      00029E F8               [12]  356 	mov	r0,a
      00029F 86 82            [24]  357 	mov	dpl,@r0
      0002A1 08               [12]  358 	inc	r0
      0002A2 86 83            [24]  359 	mov	dph,@r0
      0002A4 08               [12]  360 	inc	r0
      0002A5 86 F0            [24]  361 	mov	b,@r0
      0002A7 EC               [12]  362 	mov	a,r4
      0002A8 12 0F 31         [24]  363 	lcall	__gptrput
                                    364 ;	aes.c:96: state[9] = state[13];
      0002AB A8 08            [24]  365 	mov	r0,_bp
      0002AD 08               [12]  366 	inc	r0
      0002AE E5 08            [12]  367 	mov	a,_bp
      0002B0 24 04            [12]  368 	add	a,#0x04
      0002B2 F9               [12]  369 	mov	r1,a
      0002B3 74 0D            [12]  370 	mov	a,#0x0d
      0002B5 26               [12]  371 	add	a,@r0
      0002B6 F7               [12]  372 	mov	@r1,a
      0002B7 E4               [12]  373 	clr	a
      0002B8 08               [12]  374 	inc	r0
      0002B9 36               [12]  375 	addc	a,@r0
      0002BA 09               [12]  376 	inc	r1
      0002BB F7               [12]  377 	mov	@r1,a
      0002BC 08               [12]  378 	inc	r0
      0002BD 09               [12]  379 	inc	r1
      0002BE E6               [12]  380 	mov	a,@r0
      0002BF F7               [12]  381 	mov	@r1,a
      0002C0 E5 08            [12]  382 	mov	a,_bp
      0002C2 24 04            [12]  383 	add	a,#0x04
      0002C4 F8               [12]  384 	mov	r0,a
      0002C5 86 82            [24]  385 	mov	dpl,@r0
      0002C7 08               [12]  386 	inc	r0
      0002C8 86 83            [24]  387 	mov	dph,@r0
      0002CA 08               [12]  388 	inc	r0
      0002CB 86 F0            [24]  389 	mov	b,@r0
      0002CD 12 0F 4C         [24]  390 	lcall	__gptrget
      0002D0 8D 82            [24]  391 	mov	dpl,r5
      0002D2 8E 83            [24]  392 	mov	dph,r6
      0002D4 8F F0            [24]  393 	mov	b,r7
      0002D6 12 0F 31         [24]  394 	lcall	__gptrput
                                    395 ;	aes.c:97: state[13] = temp;
      0002D9 E5 08            [12]  396 	mov	a,_bp
      0002DB 24 04            [12]  397 	add	a,#0x04
      0002DD F8               [12]  398 	mov	r0,a
      0002DE 86 82            [24]  399 	mov	dpl,@r0
      0002E0 08               [12]  400 	inc	r0
      0002E1 86 83            [24]  401 	mov	dph,@r0
      0002E3 08               [12]  402 	inc	r0
      0002E4 86 F0            [24]  403 	mov	b,@r0
      0002E6 E5 08            [12]  404 	mov	a,_bp
      0002E8 24 07            [12]  405 	add	a,#0x07
      0002EA F9               [12]  406 	mov	r1,a
      0002EB E7               [12]  407 	mov	a,@r1
      0002EC 12 0F 31         [24]  408 	lcall	__gptrput
                                    409 ;	aes.c:99: temp = state[2];
      0002EF A8 08            [24]  410 	mov	r0,_bp
      0002F1 08               [12]  411 	inc	r0
      0002F2 74 02            [12]  412 	mov	a,#0x02
      0002F4 26               [12]  413 	add	a,@r0
      0002F5 FD               [12]  414 	mov	r5,a
      0002F6 E4               [12]  415 	clr	a
      0002F7 08               [12]  416 	inc	r0
      0002F8 36               [12]  417 	addc	a,@r0
      0002F9 FE               [12]  418 	mov	r6,a
      0002FA 08               [12]  419 	inc	r0
      0002FB 86 07            [24]  420 	mov	ar7,@r0
      0002FD 8D 82            [24]  421 	mov	dpl,r5
      0002FF 8E 83            [24]  422 	mov	dph,r6
      000301 8F F0            [24]  423 	mov	b,r7
      000303 E5 08            [12]  424 	mov	a,_bp
      000305 24 07            [12]  425 	add	a,#0x07
      000307 F8               [12]  426 	mov	r0,a
      000308 12 0F 4C         [24]  427 	lcall	__gptrget
      00030B F6               [12]  428 	mov	@r0,a
                                    429 ;	aes.c:100: state[2] = state[10];
      00030C A8 08            [24]  430 	mov	r0,_bp
      00030E 08               [12]  431 	inc	r0
      00030F E5 08            [12]  432 	mov	a,_bp
      000311 24 04            [12]  433 	add	a,#0x04
      000313 F9               [12]  434 	mov	r1,a
      000314 74 0A            [12]  435 	mov	a,#0x0a
      000316 26               [12]  436 	add	a,@r0
      000317 F7               [12]  437 	mov	@r1,a
      000318 E4               [12]  438 	clr	a
      000319 08               [12]  439 	inc	r0
      00031A 36               [12]  440 	addc	a,@r0
      00031B 09               [12]  441 	inc	r1
      00031C F7               [12]  442 	mov	@r1,a
      00031D 08               [12]  443 	inc	r0
      00031E 09               [12]  444 	inc	r1
      00031F E6               [12]  445 	mov	a,@r0
      000320 F7               [12]  446 	mov	@r1,a
      000321 E5 08            [12]  447 	mov	a,_bp
      000323 24 04            [12]  448 	add	a,#0x04
      000325 F8               [12]  449 	mov	r0,a
      000326 86 82            [24]  450 	mov	dpl,@r0
      000328 08               [12]  451 	inc	r0
      000329 86 83            [24]  452 	mov	dph,@r0
      00032B 08               [12]  453 	inc	r0
      00032C 86 F0            [24]  454 	mov	b,@r0
      00032E 12 0F 4C         [24]  455 	lcall	__gptrget
      000331 8D 82            [24]  456 	mov	dpl,r5
      000333 8E 83            [24]  457 	mov	dph,r6
      000335 8F F0            [24]  458 	mov	b,r7
      000337 12 0F 31         [24]  459 	lcall	__gptrput
                                    460 ;	aes.c:101: state[10] = temp;
      00033A E5 08            [12]  461 	mov	a,_bp
      00033C 24 04            [12]  462 	add	a,#0x04
      00033E F8               [12]  463 	mov	r0,a
      00033F 86 82            [24]  464 	mov	dpl,@r0
      000341 08               [12]  465 	inc	r0
      000342 86 83            [24]  466 	mov	dph,@r0
      000344 08               [12]  467 	inc	r0
      000345 86 F0            [24]  468 	mov	b,@r0
      000347 E5 08            [12]  469 	mov	a,_bp
      000349 24 07            [12]  470 	add	a,#0x07
      00034B F9               [12]  471 	mov	r1,a
      00034C E7               [12]  472 	mov	a,@r1
      00034D 12 0F 31         [24]  473 	lcall	__gptrput
                                    474 ;	aes.c:103: temp = state[6];
      000350 A8 08            [24]  475 	mov	r0,_bp
      000352 08               [12]  476 	inc	r0
      000353 74 06            [12]  477 	mov	a,#0x06
      000355 26               [12]  478 	add	a,@r0
      000356 FD               [12]  479 	mov	r5,a
      000357 E4               [12]  480 	clr	a
      000358 08               [12]  481 	inc	r0
      000359 36               [12]  482 	addc	a,@r0
      00035A FE               [12]  483 	mov	r6,a
      00035B 08               [12]  484 	inc	r0
      00035C 86 07            [24]  485 	mov	ar7,@r0
      00035E 8D 82            [24]  486 	mov	dpl,r5
      000360 8E 83            [24]  487 	mov	dph,r6
      000362 8F F0            [24]  488 	mov	b,r7
      000364 E5 08            [12]  489 	mov	a,_bp
      000366 24 07            [12]  490 	add	a,#0x07
      000368 F8               [12]  491 	mov	r0,a
      000369 12 0F 4C         [24]  492 	lcall	__gptrget
      00036C F6               [12]  493 	mov	@r0,a
                                    494 ;	aes.c:104: state[6] = state[14];
      00036D A8 08            [24]  495 	mov	r0,_bp
      00036F 08               [12]  496 	inc	r0
      000370 E5 08            [12]  497 	mov	a,_bp
      000372 24 04            [12]  498 	add	a,#0x04
      000374 F9               [12]  499 	mov	r1,a
      000375 74 0E            [12]  500 	mov	a,#0x0e
      000377 26               [12]  501 	add	a,@r0
      000378 F7               [12]  502 	mov	@r1,a
      000379 E4               [12]  503 	clr	a
      00037A 08               [12]  504 	inc	r0
      00037B 36               [12]  505 	addc	a,@r0
      00037C 09               [12]  506 	inc	r1
      00037D F7               [12]  507 	mov	@r1,a
      00037E 08               [12]  508 	inc	r0
      00037F 09               [12]  509 	inc	r1
      000380 E6               [12]  510 	mov	a,@r0
      000381 F7               [12]  511 	mov	@r1,a
      000382 E5 08            [12]  512 	mov	a,_bp
      000384 24 04            [12]  513 	add	a,#0x04
      000386 F8               [12]  514 	mov	r0,a
      000387 86 82            [24]  515 	mov	dpl,@r0
      000389 08               [12]  516 	inc	r0
      00038A 86 83            [24]  517 	mov	dph,@r0
      00038C 08               [12]  518 	inc	r0
      00038D 86 F0            [24]  519 	mov	b,@r0
      00038F 12 0F 4C         [24]  520 	lcall	__gptrget
      000392 8D 82            [24]  521 	mov	dpl,r5
      000394 8E 83            [24]  522 	mov	dph,r6
      000396 8F F0            [24]  523 	mov	b,r7
      000398 12 0F 31         [24]  524 	lcall	__gptrput
                                    525 ;	aes.c:105: state[14] = temp;
      00039B E5 08            [12]  526 	mov	a,_bp
      00039D 24 04            [12]  527 	add	a,#0x04
      00039F F8               [12]  528 	mov	r0,a
      0003A0 86 82            [24]  529 	mov	dpl,@r0
      0003A2 08               [12]  530 	inc	r0
      0003A3 86 83            [24]  531 	mov	dph,@r0
      0003A5 08               [12]  532 	inc	r0
      0003A6 86 F0            [24]  533 	mov	b,@r0
      0003A8 E5 08            [12]  534 	mov	a,_bp
      0003AA 24 07            [12]  535 	add	a,#0x07
      0003AC F9               [12]  536 	mov	r1,a
      0003AD E7               [12]  537 	mov	a,@r1
      0003AE 12 0F 31         [24]  538 	lcall	__gptrput
                                    539 ;	aes.c:107: temp = state[3];
      0003B1 A8 08            [24]  540 	mov	r0,_bp
      0003B3 08               [12]  541 	inc	r0
      0003B4 74 03            [12]  542 	mov	a,#0x03
      0003B6 26               [12]  543 	add	a,@r0
      0003B7 FD               [12]  544 	mov	r5,a
      0003B8 E4               [12]  545 	clr	a
      0003B9 08               [12]  546 	inc	r0
      0003BA 36               [12]  547 	addc	a,@r0
      0003BB FE               [12]  548 	mov	r6,a
      0003BC 08               [12]  549 	inc	r0
      0003BD 86 07            [24]  550 	mov	ar7,@r0
      0003BF 8D 82            [24]  551 	mov	dpl,r5
      0003C1 8E 83            [24]  552 	mov	dph,r6
      0003C3 8F F0            [24]  553 	mov	b,r7
      0003C5 E5 08            [12]  554 	mov	a,_bp
      0003C7 24 07            [12]  555 	add	a,#0x07
      0003C9 F8               [12]  556 	mov	r0,a
      0003CA 12 0F 4C         [24]  557 	lcall	__gptrget
      0003CD F6               [12]  558 	mov	@r0,a
                                    559 ;	aes.c:108: state[3]  = state[15];
      0003CE A8 08            [24]  560 	mov	r0,_bp
      0003D0 08               [12]  561 	inc	r0
      0003D1 E5 08            [12]  562 	mov	a,_bp
      0003D3 24 04            [12]  563 	add	a,#0x04
      0003D5 F9               [12]  564 	mov	r1,a
      0003D6 74 0F            [12]  565 	mov	a,#0x0f
      0003D8 26               [12]  566 	add	a,@r0
      0003D9 F7               [12]  567 	mov	@r1,a
      0003DA E4               [12]  568 	clr	a
      0003DB 08               [12]  569 	inc	r0
      0003DC 36               [12]  570 	addc	a,@r0
      0003DD 09               [12]  571 	inc	r1
      0003DE F7               [12]  572 	mov	@r1,a
      0003DF 08               [12]  573 	inc	r0
      0003E0 09               [12]  574 	inc	r1
      0003E1 E6               [12]  575 	mov	a,@r0
      0003E2 F7               [12]  576 	mov	@r1,a
      0003E3 E5 08            [12]  577 	mov	a,_bp
      0003E5 24 04            [12]  578 	add	a,#0x04
      0003E7 F8               [12]  579 	mov	r0,a
      0003E8 86 82            [24]  580 	mov	dpl,@r0
      0003EA 08               [12]  581 	inc	r0
      0003EB 86 83            [24]  582 	mov	dph,@r0
      0003ED 08               [12]  583 	inc	r0
      0003EE 86 F0            [24]  584 	mov	b,@r0
      0003F0 12 0F 4C         [24]  585 	lcall	__gptrget
      0003F3 8D 82            [24]  586 	mov	dpl,r5
      0003F5 8E 83            [24]  587 	mov	dph,r6
      0003F7 8F F0            [24]  588 	mov	b,r7
      0003F9 12 0F 31         [24]  589 	lcall	__gptrput
                                    590 ;	aes.c:109: state[15] = state[11];
      0003FC A8 08            [24]  591 	mov	r0,_bp
      0003FE 08               [12]  592 	inc	r0
      0003FF 74 0B            [12]  593 	mov	a,#0x0b
      000401 26               [12]  594 	add	a,@r0
      000402 FD               [12]  595 	mov	r5,a
      000403 E4               [12]  596 	clr	a
      000404 08               [12]  597 	inc	r0
      000405 36               [12]  598 	addc	a,@r0
      000406 FE               [12]  599 	mov	r6,a
      000407 08               [12]  600 	inc	r0
      000408 86 07            [24]  601 	mov	ar7,@r0
      00040A 8D 82            [24]  602 	mov	dpl,r5
      00040C 8E 83            [24]  603 	mov	dph,r6
      00040E 8F F0            [24]  604 	mov	b,r7
      000410 12 0F 4C         [24]  605 	lcall	__gptrget
      000413 FC               [12]  606 	mov	r4,a
      000414 E5 08            [12]  607 	mov	a,_bp
      000416 24 04            [12]  608 	add	a,#0x04
      000418 F8               [12]  609 	mov	r0,a
      000419 86 82            [24]  610 	mov	dpl,@r0
      00041B 08               [12]  611 	inc	r0
      00041C 86 83            [24]  612 	mov	dph,@r0
      00041E 08               [12]  613 	inc	r0
      00041F 86 F0            [24]  614 	mov	b,@r0
      000421 EC               [12]  615 	mov	a,r4
      000422 12 0F 31         [24]  616 	lcall	__gptrput
                                    617 ;	aes.c:110: state[11] = state[7];
      000425 A8 08            [24]  618 	mov	r0,_bp
      000427 08               [12]  619 	inc	r0
      000428 E5 08            [12]  620 	mov	a,_bp
      00042A 24 04            [12]  621 	add	a,#0x04
      00042C F9               [12]  622 	mov	r1,a
      00042D 74 07            [12]  623 	mov	a,#0x07
      00042F 26               [12]  624 	add	a,@r0
      000430 F7               [12]  625 	mov	@r1,a
      000431 E4               [12]  626 	clr	a
      000432 08               [12]  627 	inc	r0
      000433 36               [12]  628 	addc	a,@r0
      000434 09               [12]  629 	inc	r1
      000435 F7               [12]  630 	mov	@r1,a
      000436 08               [12]  631 	inc	r0
      000437 09               [12]  632 	inc	r1
      000438 E6               [12]  633 	mov	a,@r0
      000439 F7               [12]  634 	mov	@r1,a
      00043A E5 08            [12]  635 	mov	a,_bp
      00043C 24 04            [12]  636 	add	a,#0x04
      00043E F8               [12]  637 	mov	r0,a
      00043F 86 82            [24]  638 	mov	dpl,@r0
      000441 08               [12]  639 	inc	r0
      000442 86 83            [24]  640 	mov	dph,@r0
      000444 08               [12]  641 	inc	r0
      000445 86 F0            [24]  642 	mov	b,@r0
      000447 12 0F 4C         [24]  643 	lcall	__gptrget
      00044A 8D 82            [24]  644 	mov	dpl,r5
      00044C 8E 83            [24]  645 	mov	dph,r6
      00044E 8F F0            [24]  646 	mov	b,r7
      000450 12 0F 31         [24]  647 	lcall	__gptrput
                                    648 ;	aes.c:111: state[7]  = temp;
      000453 E5 08            [12]  649 	mov	a,_bp
      000455 24 04            [12]  650 	add	a,#0x04
      000457 F8               [12]  651 	mov	r0,a
      000458 86 82            [24]  652 	mov	dpl,@r0
      00045A 08               [12]  653 	inc	r0
      00045B 86 83            [24]  654 	mov	dph,@r0
      00045D 08               [12]  655 	inc	r0
      00045E 86 F0            [24]  656 	mov	b,@r0
      000460 E5 08            [12]  657 	mov	a,_bp
      000462 24 07            [12]  658 	add	a,#0x07
      000464 F9               [12]  659 	mov	r1,a
      000465 E7               [12]  660 	mov	a,@r1
      000466 12 0F 31         [24]  661 	lcall	__gptrput
      000469 85 08 81         [24]  662 	mov	sp,_bp
      00046C D0 08            [24]  663 	pop	_bp
      00046E 22               [24]  664 	ret
                                    665 ;------------------------------------------------------------
                                    666 ;Allocation info for local variables in function 'mix_columns'
                                    667 ;------------------------------------------------------------
                                    668 ;state                     Allocated to stack - _bp +1
                                    669 ;col                       Allocated to stack - _bp +18
                                    670 ;Tmp                       Allocated to stack - _bp +19
                                    671 ;Tm                        Allocated to registers r5 
                                    672 ;t                         Allocated to stack - _bp +20
                                    673 ;sloc0                     Allocated to stack - _bp +4
                                    674 ;sloc1                     Allocated to stack - _bp +5
                                    675 ;sloc2                     Allocated to stack - _bp +8
                                    676 ;sloc3                     Allocated to stack - _bp +9
                                    677 ;sloc4                     Allocated to stack - _bp +11
                                    678 ;sloc5                     Allocated to stack - _bp +14
                                    679 ;sloc6                     Allocated to stack - _bp +15
                                    680 ;------------------------------------------------------------
                                    681 ;	aes.c:114: void mix_columns(uint8_t* state)
                                    682 ;	-----------------------------------------
                                    683 ;	 function mix_columns
                                    684 ;	-----------------------------------------
      00046F                        685 _mix_columns:
      00046F C0 08            [24]  686 	push	_bp
      000471 85 81 08         [24]  687 	mov	_bp,sp
      000474 C0 82            [24]  688 	push	dpl
      000476 C0 83            [24]  689 	push	dph
      000478 C0 F0            [24]  690 	push	b
      00047A E5 81            [12]  691 	mov	a,sp
      00047C 24 11            [12]  692 	add	a,#0x11
      00047E F5 81            [12]  693 	mov	sp,a
                                    694 ;	aes.c:117: for(col = 0; col < 4; col++)
      000480 E5 08            [12]  695 	mov	a,_bp
      000482 24 12            [12]  696 	add	a,#0x12
      000484 F8               [12]  697 	mov	r0,a
      000485 76 00            [12]  698 	mov	@r0,#0x00
      000487                        699 00102$:
                                    700 ;	aes.c:120: Tmp = state[4*col+0] ^ state[4*col+1] ^ state[4*col+2] ^ state[4*col+3];
      000487 E5 08            [12]  701 	mov	a,_bp
      000489 24 12            [12]  702 	add	a,#0x12
      00048B F8               [12]  703 	mov	r0,a
      00048C E5 08            [12]  704 	mov	a,_bp
      00048E 24 09            [12]  705 	add	a,#0x09
      000490 F9               [12]  706 	mov	r1,a
      000491 E6               [12]  707 	mov	a,@r0
      000492 75 F0 04         [24]  708 	mov	b,#0x04
      000495 A4               [48]  709 	mul	ab
      000496 F7               [12]  710 	mov	@r1,a
      000497 09               [12]  711 	inc	r1
      000498 A7 F0            [24]  712 	mov	@r1,b
      00049A A8 08            [24]  713 	mov	r0,_bp
      00049C 08               [12]  714 	inc	r0
      00049D E5 08            [12]  715 	mov	a,_bp
      00049F 24 09            [12]  716 	add	a,#0x09
      0004A1 F9               [12]  717 	mov	r1,a
      0004A2 E7               [12]  718 	mov	a,@r1
      0004A3 26               [12]  719 	add	a,@r0
      0004A4 C0 E0            [24]  720 	push	acc
      0004A6 09               [12]  721 	inc	r1
      0004A7 E7               [12]  722 	mov	a,@r1
      0004A8 08               [12]  723 	inc	r0
      0004A9 36               [12]  724 	addc	a,@r0
      0004AA C0 E0            [24]  725 	push	acc
      0004AC 08               [12]  726 	inc	r0
      0004AD E6               [12]  727 	mov	a,@r0
      0004AE C0 E0            [24]  728 	push	acc
      0004B0 E5 08            [12]  729 	mov	a,_bp
      0004B2 24 07            [12]  730 	add	a,#0x07
      0004B4 F8               [12]  731 	mov	r0,a
      0004B5 D0 E0            [24]  732 	pop	acc
      0004B7 F6               [12]  733 	mov	@r0,a
      0004B8 18               [12]  734 	dec	r0
      0004B9 D0 E0            [24]  735 	pop	acc
      0004BB F6               [12]  736 	mov	@r0,a
      0004BC 18               [12]  737 	dec	r0
      0004BD D0 E0            [24]  738 	pop	acc
      0004BF F6               [12]  739 	mov	@r0,a
      0004C0 E5 08            [12]  740 	mov	a,_bp
      0004C2 24 05            [12]  741 	add	a,#0x05
      0004C4 F8               [12]  742 	mov	r0,a
      0004C5 86 82            [24]  743 	mov	dpl,@r0
      0004C7 08               [12]  744 	inc	r0
      0004C8 86 83            [24]  745 	mov	dph,@r0
      0004CA 08               [12]  746 	inc	r0
      0004CB 86 F0            [24]  747 	mov	b,@r0
      0004CD E5 08            [12]  748 	mov	a,_bp
      0004CF 24 04            [12]  749 	add	a,#0x04
      0004D1 F9               [12]  750 	mov	r1,a
      0004D2 12 0F 4C         [24]  751 	lcall	__gptrget
      0004D5 F7               [12]  752 	mov	@r1,a
      0004D6 E5 08            [12]  753 	mov	a,_bp
      0004D8 24 09            [12]  754 	add	a,#0x09
      0004DA F8               [12]  755 	mov	r0,a
      0004DB 74 01            [12]  756 	mov	a,#0x01
      0004DD 26               [12]  757 	add	a,@r0
      0004DE FD               [12]  758 	mov	r5,a
      0004DF E4               [12]  759 	clr	a
      0004E0 08               [12]  760 	inc	r0
      0004E1 36               [12]  761 	addc	a,@r0
      0004E2 FF               [12]  762 	mov	r7,a
      0004E3 A8 08            [24]  763 	mov	r0,_bp
      0004E5 08               [12]  764 	inc	r0
      0004E6 E5 08            [12]  765 	mov	a,_bp
      0004E8 24 0B            [12]  766 	add	a,#0x0b
      0004EA F9               [12]  767 	mov	r1,a
      0004EB ED               [12]  768 	mov	a,r5
      0004EC 26               [12]  769 	add	a,@r0
      0004ED F7               [12]  770 	mov	@r1,a
      0004EE EF               [12]  771 	mov	a,r7
      0004EF 08               [12]  772 	inc	r0
      0004F0 36               [12]  773 	addc	a,@r0
      0004F1 09               [12]  774 	inc	r1
      0004F2 F7               [12]  775 	mov	@r1,a
      0004F3 08               [12]  776 	inc	r0
      0004F4 09               [12]  777 	inc	r1
      0004F5 E6               [12]  778 	mov	a,@r0
      0004F6 F7               [12]  779 	mov	@r1,a
      0004F7 E5 08            [12]  780 	mov	a,_bp
      0004F9 24 0B            [12]  781 	add	a,#0x0b
      0004FB F8               [12]  782 	mov	r0,a
      0004FC 86 82            [24]  783 	mov	dpl,@r0
      0004FE 08               [12]  784 	inc	r0
      0004FF 86 83            [24]  785 	mov	dph,@r0
      000501 08               [12]  786 	inc	r0
      000502 86 F0            [24]  787 	mov	b,@r0
      000504 12 0F 4C         [24]  788 	lcall	__gptrget
      000507 FC               [12]  789 	mov	r4,a
      000508 E5 08            [12]  790 	mov	a,_bp
      00050A 24 04            [12]  791 	add	a,#0x04
      00050C F8               [12]  792 	mov	r0,a
      00050D E5 08            [12]  793 	mov	a,_bp
      00050F 24 08            [12]  794 	add	a,#0x08
      000511 F9               [12]  795 	mov	r1,a
      000512 EC               [12]  796 	mov	a,r4
      000513 66               [12]  797 	xrl	a,@r0
      000514 F7               [12]  798 	mov	@r1,a
      000515 E5 08            [12]  799 	mov	a,_bp
      000517 24 09            [12]  800 	add	a,#0x09
      000519 F8               [12]  801 	mov	r0,a
      00051A 74 02            [12]  802 	mov	a,#0x02
      00051C 26               [12]  803 	add	a,@r0
      00051D FB               [12]  804 	mov	r3,a
      00051E E4               [12]  805 	clr	a
      00051F 08               [12]  806 	inc	r0
      000520 36               [12]  807 	addc	a,@r0
      000521 FC               [12]  808 	mov	r4,a
      000522 A8 08            [24]  809 	mov	r0,_bp
      000524 08               [12]  810 	inc	r0
      000525 EB               [12]  811 	mov	a,r3
      000526 26               [12]  812 	add	a,@r0
      000527 FB               [12]  813 	mov	r3,a
      000528 EC               [12]  814 	mov	a,r4
      000529 08               [12]  815 	inc	r0
      00052A 36               [12]  816 	addc	a,@r0
      00052B FC               [12]  817 	mov	r4,a
      00052C 08               [12]  818 	inc	r0
      00052D 86 02            [24]  819 	mov	ar2,@r0
      00052F 8B 82            [24]  820 	mov	dpl,r3
      000531 8C 83            [24]  821 	mov	dph,r4
      000533 8A F0            [24]  822 	mov	b,r2
      000535 12 0F 4C         [24]  823 	lcall	__gptrget
      000538 FF               [12]  824 	mov	r7,a
      000539 E5 08            [12]  825 	mov	a,_bp
      00053B 24 08            [12]  826 	add	a,#0x08
      00053D F8               [12]  827 	mov	r0,a
      00053E E5 08            [12]  828 	mov	a,_bp
      000540 24 0E            [12]  829 	add	a,#0x0e
      000542 F9               [12]  830 	mov	r1,a
      000543 EF               [12]  831 	mov	a,r7
      000544 66               [12]  832 	xrl	a,@r0
      000545 F7               [12]  833 	mov	@r1,a
      000546 E5 08            [12]  834 	mov	a,_bp
      000548 24 09            [12]  835 	add	a,#0x09
      00054A F8               [12]  836 	mov	r0,a
      00054B 74 03            [12]  837 	mov	a,#0x03
      00054D 26               [12]  838 	add	a,@r0
      00054E FD               [12]  839 	mov	r5,a
      00054F E4               [12]  840 	clr	a
      000550 08               [12]  841 	inc	r0
      000551 36               [12]  842 	addc	a,@r0
      000552 FE               [12]  843 	mov	r6,a
      000553 A8 08            [24]  844 	mov	r0,_bp
      000555 08               [12]  845 	inc	r0
      000556 E5 08            [12]  846 	mov	a,_bp
      000558 24 0F            [12]  847 	add	a,#0x0f
      00055A F9               [12]  848 	mov	r1,a
      00055B ED               [12]  849 	mov	a,r5
      00055C 26               [12]  850 	add	a,@r0
      00055D F7               [12]  851 	mov	@r1,a
      00055E EE               [12]  852 	mov	a,r6
      00055F 08               [12]  853 	inc	r0
      000560 36               [12]  854 	addc	a,@r0
      000561 09               [12]  855 	inc	r1
      000562 F7               [12]  856 	mov	@r1,a
      000563 08               [12]  857 	inc	r0
      000564 09               [12]  858 	inc	r1
      000565 E6               [12]  859 	mov	a,@r0
      000566 F7               [12]  860 	mov	@r1,a
      000567 E5 08            [12]  861 	mov	a,_bp
      000569 24 0F            [12]  862 	add	a,#0x0f
      00056B F8               [12]  863 	mov	r0,a
      00056C 86 82            [24]  864 	mov	dpl,@r0
      00056E 08               [12]  865 	inc	r0
      00056F 86 83            [24]  866 	mov	dph,@r0
      000571 08               [12]  867 	inc	r0
      000572 86 F0            [24]  868 	mov	b,@r0
      000574 12 0F 4C         [24]  869 	lcall	__gptrget
      000577 FF               [12]  870 	mov	r7,a
      000578 E5 08            [12]  871 	mov	a,_bp
      00057A 24 0E            [12]  872 	add	a,#0x0e
      00057C F8               [12]  873 	mov	r0,a
      00057D E6               [12]  874 	mov	a,@r0
      00057E 62 07            [12]  875 	xrl	ar7,a
      000580 E5 08            [12]  876 	mov	a,_bp
      000582 24 13            [12]  877 	add	a,#0x13
      000584 F8               [12]  878 	mov	r0,a
      000585 A6 07            [24]  879 	mov	@r0,ar7
                                    880 ;	aes.c:121: t  = state[4*col];
      000587 E5 08            [12]  881 	mov	a,_bp
      000589 24 04            [12]  882 	add	a,#0x04
      00058B F8               [12]  883 	mov	r0,a
      00058C 86 06            [24]  884 	mov	ar6,@r0
      00058E E5 08            [12]  885 	mov	a,_bp
      000590 24 14            [12]  886 	add	a,#0x14
      000592 F8               [12]  887 	mov	r0,a
      000593 A6 06            [24]  888 	mov	@r0,ar6
                                    889 ;	aes.c:122: Tm = state[4*col+0] ^ state[4*col+1]; Tm = xtime(Tm); state[4*col+0] ^= (Tm ^ Tmp);
      000595 E5 08            [12]  890 	mov	a,_bp
      000597 24 08            [12]  891 	add	a,#0x08
      000599 F8               [12]  892 	mov	r0,a
      00059A 86 05            [24]  893 	mov	ar5,@r0
      00059C 8D 82            [24]  894 	mov	dpl,r5
      00059E C0 04            [24]  895 	push	ar4
      0005A0 C0 03            [24]  896 	push	ar3
      0005A2 C0 02            [24]  897 	push	ar2
      0005A4 12 01 65         [24]  898 	lcall	_xtime
      0005A7 AE 82            [24]  899 	mov	r6,dpl
      0005A9 D0 02            [24]  900 	pop	ar2
      0005AB D0 03            [24]  901 	pop	ar3
      0005AD D0 04            [24]  902 	pop	ar4
      0005AF 8E 05            [24]  903 	mov	ar5,r6
      0005B1 E5 08            [12]  904 	mov	a,_bp
      0005B3 24 05            [12]  905 	add	a,#0x05
      0005B5 F8               [12]  906 	mov	r0,a
      0005B6 86 82            [24]  907 	mov	dpl,@r0
      0005B8 08               [12]  908 	inc	r0
      0005B9 86 83            [24]  909 	mov	dph,@r0
      0005BB 08               [12]  910 	inc	r0
      0005BC 86 F0            [24]  911 	mov	b,@r0
      0005BE 12 0F 4C         [24]  912 	lcall	__gptrget
      0005C1 FE               [12]  913 	mov	r6,a
      0005C2 E5 08            [12]  914 	mov	a,_bp
      0005C4 24 13            [12]  915 	add	a,#0x13
      0005C6 F8               [12]  916 	mov	r0,a
      0005C7 E6               [12]  917 	mov	a,@r0
      0005C8 6D               [12]  918 	xrl	a,r5
      0005C9 62 06            [12]  919 	xrl	ar6,a
      0005CB E5 08            [12]  920 	mov	a,_bp
      0005CD 24 05            [12]  921 	add	a,#0x05
      0005CF F8               [12]  922 	mov	r0,a
      0005D0 86 82            [24]  923 	mov	dpl,@r0
      0005D2 08               [12]  924 	inc	r0
      0005D3 86 83            [24]  925 	mov	dph,@r0
      0005D5 08               [12]  926 	inc	r0
      0005D6 86 F0            [24]  927 	mov	b,@r0
      0005D8 EE               [12]  928 	mov	a,r6
      0005D9 12 0F 31         [24]  929 	lcall	__gptrput
                                    930 ;	aes.c:123: Tm = state[4*col+1] ^ state[4*col+2]; Tm = xtime(Tm); state[4*col+1] ^= (Tm ^ Tmp);
      0005DC E5 08            [12]  931 	mov	a,_bp
      0005DE 24 0B            [12]  932 	add	a,#0x0b
      0005E0 F8               [12]  933 	mov	r0,a
      0005E1 86 82            [24]  934 	mov	dpl,@r0
      0005E3 08               [12]  935 	inc	r0
      0005E4 86 83            [24]  936 	mov	dph,@r0
      0005E6 08               [12]  937 	inc	r0
      0005E7 86 F0            [24]  938 	mov	b,@r0
      0005E9 12 0F 4C         [24]  939 	lcall	__gptrget
      0005EC FF               [12]  940 	mov	r7,a
      0005ED 8B 82            [24]  941 	mov	dpl,r3
      0005EF 8C 83            [24]  942 	mov	dph,r4
      0005F1 8A F0            [24]  943 	mov	b,r2
      0005F3 12 0F 4C         [24]  944 	lcall	__gptrget
      0005F6 62 07            [12]  945 	xrl	ar7,a
      0005F8 8F 05            [24]  946 	mov	ar5,r7
      0005FA 8D 82            [24]  947 	mov	dpl,r5
      0005FC C0 04            [24]  948 	push	ar4
      0005FE C0 03            [24]  949 	push	ar3
      000600 C0 02            [24]  950 	push	ar2
      000602 12 01 65         [24]  951 	lcall	_xtime
      000605 AF 82            [24]  952 	mov	r7,dpl
      000607 D0 02            [24]  953 	pop	ar2
      000609 D0 03            [24]  954 	pop	ar3
      00060B D0 04            [24]  955 	pop	ar4
      00060D 8F 05            [24]  956 	mov	ar5,r7
      00060F E5 08            [12]  957 	mov	a,_bp
      000611 24 0B            [12]  958 	add	a,#0x0b
      000613 F8               [12]  959 	mov	r0,a
      000614 86 82            [24]  960 	mov	dpl,@r0
      000616 08               [12]  961 	inc	r0
      000617 86 83            [24]  962 	mov	dph,@r0
      000619 08               [12]  963 	inc	r0
      00061A 86 F0            [24]  964 	mov	b,@r0
      00061C 12 0F 4C         [24]  965 	lcall	__gptrget
      00061F FF               [12]  966 	mov	r7,a
      000620 E5 08            [12]  967 	mov	a,_bp
      000622 24 13            [12]  968 	add	a,#0x13
      000624 F8               [12]  969 	mov	r0,a
      000625 E6               [12]  970 	mov	a,@r0
      000626 6D               [12]  971 	xrl	a,r5
      000627 62 07            [12]  972 	xrl	ar7,a
      000629 E5 08            [12]  973 	mov	a,_bp
      00062B 24 0B            [12]  974 	add	a,#0x0b
      00062D F8               [12]  975 	mov	r0,a
      00062E 86 82            [24]  976 	mov	dpl,@r0
      000630 08               [12]  977 	inc	r0
      000631 86 83            [24]  978 	mov	dph,@r0
      000633 08               [12]  979 	inc	r0
      000634 86 F0            [24]  980 	mov	b,@r0
      000636 EF               [12]  981 	mov	a,r7
      000637 12 0F 31         [24]  982 	lcall	__gptrput
                                    983 ;	aes.c:124: Tm = state[4*col+2] ^ state[4*col+3]; Tm = xtime(Tm); state[4*col+2] ^= (Tm ^ Tmp);
      00063A 8B 82            [24]  984 	mov	dpl,r3
      00063C 8C 83            [24]  985 	mov	dph,r4
      00063E 8A F0            [24]  986 	mov	b,r2
      000640 12 0F 4C         [24]  987 	lcall	__gptrget
      000643 FF               [12]  988 	mov	r7,a
      000644 E5 08            [12]  989 	mov	a,_bp
      000646 24 0F            [12]  990 	add	a,#0x0f
      000648 F8               [12]  991 	mov	r0,a
      000649 86 82            [24]  992 	mov	dpl,@r0
      00064B 08               [12]  993 	inc	r0
      00064C 86 83            [24]  994 	mov	dph,@r0
      00064E 08               [12]  995 	inc	r0
      00064F 86 F0            [24]  996 	mov	b,@r0
      000651 12 0F 4C         [24]  997 	lcall	__gptrget
      000654 62 07            [12]  998 	xrl	ar7,a
      000656 8F 05            [24]  999 	mov	ar5,r7
      000658 8D 82            [24] 1000 	mov	dpl,r5
      00065A C0 04            [24] 1001 	push	ar4
      00065C C0 03            [24] 1002 	push	ar3
      00065E C0 02            [24] 1003 	push	ar2
      000660 12 01 65         [24] 1004 	lcall	_xtime
      000663 AF 82            [24] 1005 	mov	r7,dpl
      000665 D0 02            [24] 1006 	pop	ar2
      000667 D0 03            [24] 1007 	pop	ar3
      000669 D0 04            [24] 1008 	pop	ar4
      00066B 8F 05            [24] 1009 	mov	ar5,r7
      00066D 8B 82            [24] 1010 	mov	dpl,r3
      00066F 8C 83            [24] 1011 	mov	dph,r4
      000671 8A F0            [24] 1012 	mov	b,r2
      000673 12 0F 4C         [24] 1013 	lcall	__gptrget
      000676 FF               [12] 1014 	mov	r7,a
      000677 E5 08            [12] 1015 	mov	a,_bp
      000679 24 13            [12] 1016 	add	a,#0x13
      00067B F8               [12] 1017 	mov	r0,a
      00067C E6               [12] 1018 	mov	a,@r0
      00067D 6D               [12] 1019 	xrl	a,r5
      00067E 62 07            [12] 1020 	xrl	ar7,a
      000680 8B 82            [24] 1021 	mov	dpl,r3
      000682 8C 83            [24] 1022 	mov	dph,r4
      000684 8A F0            [24] 1023 	mov	b,r2
      000686 EF               [12] 1024 	mov	a,r7
      000687 12 0F 31         [24] 1025 	lcall	__gptrput
                                   1026 ;	aes.c:125: Tm = state[4*col+3] ^ t;              Tm = xtime(Tm); state[4*col+3] ^= (Tm ^ Tmp);
      00068A E5 08            [12] 1027 	mov	a,_bp
      00068C 24 0F            [12] 1028 	add	a,#0x0f
      00068E F8               [12] 1029 	mov	r0,a
      00068F 86 82            [24] 1030 	mov	dpl,@r0
      000691 08               [12] 1031 	inc	r0
      000692 86 83            [24] 1032 	mov	dph,@r0
      000694 08               [12] 1033 	inc	r0
      000695 86 F0            [24] 1034 	mov	b,@r0
      000697 12 0F 4C         [24] 1035 	lcall	__gptrget
      00069A FF               [12] 1036 	mov	r7,a
      00069B E5 08            [12] 1037 	mov	a,_bp
      00069D 24 14            [12] 1038 	add	a,#0x14
      00069F F8               [12] 1039 	mov	r0,a
      0006A0 E6               [12] 1040 	mov	a,@r0
      0006A1 62 07            [12] 1041 	xrl	ar7,a
      0006A3 8F 05            [24] 1042 	mov	ar5,r7
      0006A5 8D 82            [24] 1043 	mov	dpl,r5
      0006A7 12 01 65         [24] 1044 	lcall	_xtime
      0006AA AF 82            [24] 1045 	mov	r7,dpl
      0006AC 8F 05            [24] 1046 	mov	ar5,r7
      0006AE E5 08            [12] 1047 	mov	a,_bp
      0006B0 24 0F            [12] 1048 	add	a,#0x0f
      0006B2 F8               [12] 1049 	mov	r0,a
      0006B3 86 82            [24] 1050 	mov	dpl,@r0
      0006B5 08               [12] 1051 	inc	r0
      0006B6 86 83            [24] 1052 	mov	dph,@r0
      0006B8 08               [12] 1053 	inc	r0
      0006B9 86 F0            [24] 1054 	mov	b,@r0
      0006BB 12 0F 4C         [24] 1055 	lcall	__gptrget
      0006BE FF               [12] 1056 	mov	r7,a
      0006BF E5 08            [12] 1057 	mov	a,_bp
      0006C1 24 13            [12] 1058 	add	a,#0x13
      0006C3 F8               [12] 1059 	mov	r0,a
      0006C4 E6               [12] 1060 	mov	a,@r0
      0006C5 62 05            [12] 1061 	xrl	ar5,a
      0006C7 ED               [12] 1062 	mov	a,r5
      0006C8 62 07            [12] 1063 	xrl	ar7,a
      0006CA E5 08            [12] 1064 	mov	a,_bp
      0006CC 24 0F            [12] 1065 	add	a,#0x0f
      0006CE F8               [12] 1066 	mov	r0,a
      0006CF 86 82            [24] 1067 	mov	dpl,@r0
      0006D1 08               [12] 1068 	inc	r0
      0006D2 86 83            [24] 1069 	mov	dph,@r0
      0006D4 08               [12] 1070 	inc	r0
      0006D5 86 F0            [24] 1071 	mov	b,@r0
      0006D7 EF               [12] 1072 	mov	a,r7
      0006D8 12 0F 31         [24] 1073 	lcall	__gptrput
                                   1074 ;	aes.c:117: for(col = 0; col < 4; col++)
      0006DB E5 08            [12] 1075 	mov	a,_bp
      0006DD 24 12            [12] 1076 	add	a,#0x12
      0006DF F8               [12] 1077 	mov	r0,a
      0006E0 06               [12] 1078 	inc	@r0
      0006E1 E5 08            [12] 1079 	mov	a,_bp
      0006E3 24 12            [12] 1080 	add	a,#0x12
      0006E5 F8               [12] 1081 	mov	r0,a
      0006E6 B6 04 00         [24] 1082 	cjne	@r0,#0x04,00110$
      0006E9                       1083 00110$:
      0006E9 50 03            [24] 1084 	jnc	00111$
      0006EB 02 04 87         [24] 1085 	ljmp	00102$
      0006EE                       1086 00111$:
      0006EE 85 08 81         [24] 1087 	mov	sp,_bp
      0006F1 D0 08            [24] 1088 	pop	_bp
      0006F3 22               [24] 1089 	ret
                                   1090 ;------------------------------------------------------------
                                   1091 ;Allocation info for local variables in function 'update_key'
                                   1092 ;------------------------------------------------------------
                                   1093 ;round                     Allocated to stack - _bp -3
                                   1094 ;w                         Allocated to stack - _bp +1
                                   1095 ;temp                      Allocated to stack - _bp +18
                                   1096 ;i                         Allocated to stack - _bp +22
                                   1097 ;sloc0                     Allocated to stack - _bp +4
                                   1098 ;sloc1                     Allocated to stack - _bp +6
                                   1099 ;sloc2                     Allocated to stack - _bp +9
                                   1100 ;sloc3                     Allocated to stack - _bp +12
                                   1101 ;sloc4                     Allocated to stack - _bp +15
                                   1102 ;sloc5                     Allocated to stack - _bp +16
                                   1103 ;sloc6                     Allocated to stack - _bp +17
                                   1104 ;------------------------------------------------------------
                                   1105 ;	aes.c:129: void update_key(uint8_t* w, uint8_t round)
                                   1106 ;	-----------------------------------------
                                   1107 ;	 function update_key
                                   1108 ;	-----------------------------------------
      0006F4                       1109 _update_key:
      0006F4 C0 08            [24] 1110 	push	_bp
      0006F6 85 81 08         [24] 1111 	mov	_bp,sp
      0006F9 C0 82            [24] 1112 	push	dpl
      0006FB C0 83            [24] 1113 	push	dph
      0006FD C0 F0            [24] 1114 	push	b
      0006FF E5 81            [12] 1115 	mov	a,sp
      000701 24 13            [12] 1116 	add	a,#0x13
      000703 F5 81            [12] 1117 	mov	sp,a
                                   1118 ;	aes.c:134: for(i = 0; i < 4; i++)
      000705 A8 08            [24] 1119 	mov	r0,_bp
      000707 08               [12] 1120 	inc	r0
      000708 74 0D            [12] 1121 	mov	a,#0x0d
      00070A 26               [12] 1122 	add	a,@r0
      00070B FA               [12] 1123 	mov	r2,a
      00070C E4               [12] 1124 	clr	a
      00070D 08               [12] 1125 	inc	r0
      00070E 36               [12] 1126 	addc	a,@r0
      00070F FB               [12] 1127 	mov	r3,a
      000710 08               [12] 1128 	inc	r0
      000711 86 04            [24] 1129 	mov	ar4,@r0
      000713 E5 08            [12] 1130 	mov	a,_bp
      000715 24 FD            [12] 1131 	add	a,#0xfd
      000717 F8               [12] 1132 	mov	r0,a
      000718 E5 08            [12] 1133 	mov	a,_bp
      00071A 24 04            [12] 1134 	add	a,#0x04
      00071C F9               [12] 1135 	mov	r1,a
      00071D E6               [12] 1136 	mov	a,@r0
      00071E 24 6C            [12] 1137 	add	a,#_rcon
      000720 F7               [12] 1138 	mov	@r1,a
      000721 E4               [12] 1139 	clr	a
      000722 34 10            [12] 1140 	addc	a,#(_rcon >> 8)
      000724 09               [12] 1141 	inc	r1
      000725 F7               [12] 1142 	mov	@r1,a
      000726 A8 08            [24] 1143 	mov	r0,_bp
      000728 08               [12] 1144 	inc	r0
      000729 E5 08            [12] 1145 	mov	a,_bp
      00072B 24 06            [12] 1146 	add	a,#0x06
      00072D F9               [12] 1147 	mov	r1,a
      00072E 74 0E            [12] 1148 	mov	a,#0x0e
      000730 26               [12] 1149 	add	a,@r0
      000731 F7               [12] 1150 	mov	@r1,a
      000732 E4               [12] 1151 	clr	a
      000733 08               [12] 1152 	inc	r0
      000734 36               [12] 1153 	addc	a,@r0
      000735 09               [12] 1154 	inc	r1
      000736 F7               [12] 1155 	mov	@r1,a
      000737 08               [12] 1156 	inc	r0
      000738 09               [12] 1157 	inc	r1
      000739 E6               [12] 1158 	mov	a,@r0
      00073A F7               [12] 1159 	mov	@r1,a
      00073B A8 08            [24] 1160 	mov	r0,_bp
      00073D 08               [12] 1161 	inc	r0
      00073E E5 08            [12] 1162 	mov	a,_bp
      000740 24 09            [12] 1163 	add	a,#0x09
      000742 F9               [12] 1164 	mov	r1,a
      000743 74 0F            [12] 1165 	mov	a,#0x0f
      000745 26               [12] 1166 	add	a,@r0
      000746 F7               [12] 1167 	mov	@r1,a
      000747 E4               [12] 1168 	clr	a
      000748 08               [12] 1169 	inc	r0
      000749 36               [12] 1170 	addc	a,@r0
      00074A 09               [12] 1171 	inc	r1
      00074B F7               [12] 1172 	mov	@r1,a
      00074C 08               [12] 1173 	inc	r0
      00074D 09               [12] 1174 	inc	r1
      00074E E6               [12] 1175 	mov	a,@r0
      00074F F7               [12] 1176 	mov	@r1,a
      000750 A8 08            [24] 1177 	mov	r0,_bp
      000752 08               [12] 1178 	inc	r0
      000753 E5 08            [12] 1179 	mov	a,_bp
      000755 24 0C            [12] 1180 	add	a,#0x0c
      000757 F9               [12] 1181 	mov	r1,a
      000758 74 0C            [12] 1182 	mov	a,#0x0c
      00075A 26               [12] 1183 	add	a,@r0
      00075B F7               [12] 1184 	mov	@r1,a
      00075C E4               [12] 1185 	clr	a
      00075D 08               [12] 1186 	inc	r0
      00075E 36               [12] 1187 	addc	a,@r0
      00075F 09               [12] 1188 	inc	r1
      000760 F7               [12] 1189 	mov	@r1,a
      000761 08               [12] 1190 	inc	r0
      000762 09               [12] 1191 	inc	r1
      000763 E6               [12] 1192 	mov	a,@r0
      000764 F7               [12] 1193 	mov	@r1,a
      000765 E5 08            [12] 1194 	mov	a,_bp
      000767 24 12            [12] 1195 	add	a,#0x12
      000769 F9               [12] 1196 	mov	r1,a
      00076A 04               [12] 1197 	inc	a
      00076B F8               [12] 1198 	mov	r0,a
      00076C C0 00            [24] 1199 	push	ar0
      00076E E5 08            [12] 1200 	mov	a,_bp
      000770 24 0F            [12] 1201 	add	a,#0x0f
      000772 F8               [12] 1202 	mov	r0,a
      000773 74 02            [12] 1203 	mov	a,#0x02
      000775 29               [12] 1204 	add	a,r1
      000776 F6               [12] 1205 	mov	@r0,a
      000777 E5 08            [12] 1206 	mov	a,_bp
      000779 24 10            [12] 1207 	add	a,#0x10
      00077B F8               [12] 1208 	mov	r0,a
      00077C 74 03            [12] 1209 	mov	a,#0x03
      00077E 29               [12] 1210 	add	a,r1
      00077F F6               [12] 1211 	mov	@r0,a
      000780 E5 08            [12] 1212 	mov	a,_bp
      000782 24 16            [12] 1213 	add	a,#0x16
      000784 F8               [12] 1214 	mov	r0,a
      000785 76 00            [12] 1215 	mov	@r0,#0x00
      000787 D0 00            [24] 1216 	pop	ar0
      000789                       1217 00105$:
                                   1218 ;	aes.c:136: if(i == 0)
      000789 C0 00            [24] 1219 	push	ar0
      00078B E5 08            [12] 1220 	mov	a,_bp
      00078D 24 16            [12] 1221 	add	a,#0x16
      00078F F8               [12] 1222 	mov	r0,a
      000790 E6               [12] 1223 	mov	a,@r0
      000791 D0 00            [24] 1224 	pop	ar0
      000793 60 03            [24] 1225 	jz	00116$
      000795 02 08 13         [24] 1226 	ljmp	00102$
      000798                       1227 00116$:
                                   1228 ;	aes.c:138: temp[0] = s_box[w[13]] ^ rcon[round];
      000798 8A 82            [24] 1229 	mov	dpl,r2
      00079A 8B 83            [24] 1230 	mov	dph,r3
      00079C 8C F0            [24] 1231 	mov	b,r4
      00079E 12 0F 4C         [24] 1232 	lcall	__gptrget
      0007A1 90 0F 6C         [24] 1233 	mov	dptr,#_s_box
      0007A4 93               [24] 1234 	movc	a,@a+dptr
      0007A5 FE               [12] 1235 	mov	r6,a
      0007A6 C0 00            [24] 1236 	push	ar0
      0007A8 E5 08            [12] 1237 	mov	a,_bp
      0007AA 24 04            [12] 1238 	add	a,#0x04
      0007AC F8               [12] 1239 	mov	r0,a
      0007AD 86 82            [24] 1240 	mov	dpl,@r0
      0007AF 08               [12] 1241 	inc	r0
      0007B0 86 83            [24] 1242 	mov	dph,@r0
      0007B2 E4               [12] 1243 	clr	a
      0007B3 93               [24] 1244 	movc	a,@a+dptr
      0007B4 FD               [12] 1245 	mov	r5,a
      0007B5 6E               [12] 1246 	xrl	a,r6
      0007B6 F7               [12] 1247 	mov	@r1,a
                                   1248 ;	aes.c:139: temp[1] = s_box[w[14]];
      0007B7 E5 08            [12] 1249 	mov	a,_bp
      0007B9 24 06            [12] 1250 	add	a,#0x06
      0007BB F8               [12] 1251 	mov	r0,a
      0007BC 86 82            [24] 1252 	mov	dpl,@r0
      0007BE 08               [12] 1253 	inc	r0
      0007BF 86 83            [24] 1254 	mov	dph,@r0
      0007C1 08               [12] 1255 	inc	r0
      0007C2 86 F0            [24] 1256 	mov	b,@r0
      0007C4 12 0F 4C         [24] 1257 	lcall	__gptrget
      0007C7 FE               [12] 1258 	mov	r6,a
      0007C8 D0 00            [24] 1259 	pop	ar0
      0007CA EE               [12] 1260 	mov	a,r6
      0007CB 90 0F 6C         [24] 1261 	mov	dptr,#_s_box
      0007CE 93               [24] 1262 	movc	a,@a+dptr
      0007CF F6               [12] 1263 	mov	@r0,a
                                   1264 ;	aes.c:140: temp[2] = s_box[w[15]];
      0007D0 C0 00            [24] 1265 	push	ar0
      0007D2 E5 08            [12] 1266 	mov	a,_bp
      0007D4 24 09            [12] 1267 	add	a,#0x09
      0007D6 F8               [12] 1268 	mov	r0,a
      0007D7 86 82            [24] 1269 	mov	dpl,@r0
      0007D9 08               [12] 1270 	inc	r0
      0007DA 86 83            [24] 1271 	mov	dph,@r0
      0007DC 08               [12] 1272 	inc	r0
      0007DD 86 F0            [24] 1273 	mov	b,@r0
      0007DF 12 0F 4C         [24] 1274 	lcall	__gptrget
      0007E2 90 0F 6C         [24] 1275 	mov	dptr,#_s_box
      0007E5 93               [24] 1276 	movc	a,@a+dptr
      0007E6 FE               [12] 1277 	mov	r6,a
      0007E7 E5 08            [12] 1278 	mov	a,_bp
      0007E9 24 0F            [12] 1279 	add	a,#0x0f
      0007EB F8               [12] 1280 	mov	r0,a
      0007EC 86 00            [24] 1281 	mov	ar0,@r0
      0007EE A6 06            [24] 1282 	mov	@r0,ar6
                                   1283 ;	aes.c:141: temp[3] = s_box[w[12]];
      0007F0 E5 08            [12] 1284 	mov	a,_bp
      0007F2 24 0C            [12] 1285 	add	a,#0x0c
      0007F4 F8               [12] 1286 	mov	r0,a
      0007F5 86 82            [24] 1287 	mov	dpl,@r0
      0007F7 08               [12] 1288 	inc	r0
      0007F8 86 83            [24] 1289 	mov	dph,@r0
      0007FA 08               [12] 1290 	inc	r0
      0007FB 86 F0            [24] 1291 	mov	b,@r0
      0007FD 12 0F 4C         [24] 1292 	lcall	__gptrget
      000800 90 0F 6C         [24] 1293 	mov	dptr,#_s_box
      000803 93               [24] 1294 	movc	a,@a+dptr
      000804 FE               [12] 1295 	mov	r6,a
      000805 E5 08            [12] 1296 	mov	a,_bp
      000807 24 10            [12] 1297 	add	a,#0x10
      000809 F8               [12] 1298 	mov	r0,a
      00080A 86 00            [24] 1299 	mov	ar0,@r0
      00080C A6 06            [24] 1300 	mov	@r0,ar6
      00080E D0 00            [24] 1301 	pop	ar0
      000810 02 08 C8         [24] 1302 	ljmp	00103$
      000813                       1303 00102$:
                                   1304 ;	aes.c:145: temp[0] = w[4*(i-1) + 0];
      000813 C0 02            [24] 1305 	push	ar2
      000815 C0 03            [24] 1306 	push	ar3
      000817 C0 04            [24] 1307 	push	ar4
      000819 C0 00            [24] 1308 	push	ar0
      00081B E5 08            [12] 1309 	mov	a,_bp
      00081D 24 16            [12] 1310 	add	a,#0x16
      00081F F8               [12] 1311 	mov	r0,a
      000820 86 05            [24] 1312 	mov	ar5,@r0
      000822 7E 00            [12] 1313 	mov	r6,#0x00
      000824 D0 00            [24] 1314 	pop	ar0
      000826 1D               [12] 1315 	dec	r5
      000827 BD FF 01         [24] 1316 	cjne	r5,#0xff,00117$
      00082A 1E               [12] 1317 	dec	r6
      00082B                       1318 00117$:
      00082B EE               [12] 1319 	mov	a,r6
      00082C CD               [12] 1320 	xch	a,r5
      00082D 25 E0            [12] 1321 	add	a,acc
      00082F CD               [12] 1322 	xch	a,r5
      000830 33               [12] 1323 	rlc	a
      000831 CD               [12] 1324 	xch	a,r5
      000832 25 E0            [12] 1325 	add	a,acc
      000834 CD               [12] 1326 	xch	a,r5
      000835 33               [12] 1327 	rlc	a
      000836 FE               [12] 1328 	mov	r6,a
      000837 C0 00            [24] 1329 	push	ar0
      000839 A8 08            [24] 1330 	mov	r0,_bp
      00083B 08               [12] 1331 	inc	r0
      00083C ED               [12] 1332 	mov	a,r5
      00083D 26               [12] 1333 	add	a,@r0
      00083E FA               [12] 1334 	mov	r2,a
      00083F EE               [12] 1335 	mov	a,r6
      000840 08               [12] 1336 	inc	r0
      000841 36               [12] 1337 	addc	a,@r0
      000842 FB               [12] 1338 	mov	r3,a
      000843 08               [12] 1339 	inc	r0
      000844 86 04            [24] 1340 	mov	ar4,@r0
      000846 8A 82            [24] 1341 	mov	dpl,r2
      000848 8B 83            [24] 1342 	mov	dph,r3
      00084A 8C F0            [24] 1343 	mov	b,r4
      00084C 12 0F 4C         [24] 1344 	lcall	__gptrget
      00084F F7               [12] 1345 	mov	@r1,a
                                   1346 ;	aes.c:146: temp[1] = w[4*(i-1) + 1];
      000850 74 01            [12] 1347 	mov	a,#0x01
      000852 2D               [12] 1348 	add	a,r5
      000853 FB               [12] 1349 	mov	r3,a
      000854 E4               [12] 1350 	clr	a
      000855 3E               [12] 1351 	addc	a,r6
      000856 FC               [12] 1352 	mov	r4,a
      000857 A8 08            [24] 1353 	mov	r0,_bp
      000859 08               [12] 1354 	inc	r0
      00085A EB               [12] 1355 	mov	a,r3
      00085B 26               [12] 1356 	add	a,@r0
      00085C FB               [12] 1357 	mov	r3,a
      00085D EC               [12] 1358 	mov	a,r4
      00085E 08               [12] 1359 	inc	r0
      00085F 36               [12] 1360 	addc	a,@r0
      000860 FC               [12] 1361 	mov	r4,a
      000861 08               [12] 1362 	inc	r0
      000862 86 02            [24] 1363 	mov	ar2,@r0
      000864 D0 00            [24] 1364 	pop	ar0
      000866 8B 82            [24] 1365 	mov	dpl,r3
      000868 8C 83            [24] 1366 	mov	dph,r4
      00086A 8A F0            [24] 1367 	mov	b,r2
      00086C 12 0F 4C         [24] 1368 	lcall	__gptrget
      00086F F6               [12] 1369 	mov	@r0,a
                                   1370 ;	aes.c:147: temp[2] = w[4*(i-1) + 2];
      000870 74 02            [12] 1371 	mov	a,#0x02
      000872 2D               [12] 1372 	add	a,r5
      000873 FB               [12] 1373 	mov	r3,a
      000874 E4               [12] 1374 	clr	a
      000875 3E               [12] 1375 	addc	a,r6
      000876 FC               [12] 1376 	mov	r4,a
      000877 C0 00            [24] 1377 	push	ar0
      000879 A8 08            [24] 1378 	mov	r0,_bp
      00087B 08               [12] 1379 	inc	r0
      00087C EB               [12] 1380 	mov	a,r3
      00087D 26               [12] 1381 	add	a,@r0
      00087E FB               [12] 1382 	mov	r3,a
      00087F EC               [12] 1383 	mov	a,r4
      000880 08               [12] 1384 	inc	r0
      000881 36               [12] 1385 	addc	a,@r0
      000882 FC               [12] 1386 	mov	r4,a
      000883 08               [12] 1387 	inc	r0
      000884 86 02            [24] 1388 	mov	ar2,@r0
      000886 8B 82            [24] 1389 	mov	dpl,r3
      000888 8C 83            [24] 1390 	mov	dph,r4
      00088A 8A F0            [24] 1391 	mov	b,r2
      00088C 12 0F 4C         [24] 1392 	lcall	__gptrget
      00088F FB               [12] 1393 	mov	r3,a
      000890 E5 08            [12] 1394 	mov	a,_bp
      000892 24 0F            [12] 1395 	add	a,#0x0f
      000894 F8               [12] 1396 	mov	r0,a
      000895 86 00            [24] 1397 	mov	ar0,@r0
      000897 A6 03            [24] 1398 	mov	@r0,ar3
                                   1399 ;	aes.c:148: temp[3] = w[4*(i-1) + 3];
      000899 74 03            [12] 1400 	mov	a,#0x03
      00089B 2D               [12] 1401 	add	a,r5
      00089C FD               [12] 1402 	mov	r5,a
      00089D E4               [12] 1403 	clr	a
      00089E 3E               [12] 1404 	addc	a,r6
      00089F FE               [12] 1405 	mov	r6,a
      0008A0 A8 08            [24] 1406 	mov	r0,_bp
      0008A2 08               [12] 1407 	inc	r0
      0008A3 ED               [12] 1408 	mov	a,r5
      0008A4 26               [12] 1409 	add	a,@r0
      0008A5 FD               [12] 1410 	mov	r5,a
      0008A6 EE               [12] 1411 	mov	a,r6
      0008A7 08               [12] 1412 	inc	r0
      0008A8 36               [12] 1413 	addc	a,@r0
      0008A9 FE               [12] 1414 	mov	r6,a
      0008AA 08               [12] 1415 	inc	r0
      0008AB 86 04            [24] 1416 	mov	ar4,@r0
      0008AD 8D 82            [24] 1417 	mov	dpl,r5
      0008AF 8E 83            [24] 1418 	mov	dph,r6
      0008B1 8C F0            [24] 1419 	mov	b,r4
      0008B3 12 0F 4C         [24] 1420 	lcall	__gptrget
      0008B6 FD               [12] 1421 	mov	r5,a
      0008B7 E5 08            [12] 1422 	mov	a,_bp
      0008B9 24 10            [12] 1423 	add	a,#0x10
      0008BB F8               [12] 1424 	mov	r0,a
      0008BC 86 00            [24] 1425 	mov	ar0,@r0
      0008BE A6 05            [24] 1426 	mov	@r0,ar5
      0008C0 D0 00            [24] 1427 	pop	ar0
                                   1428 ;	aes.c:134: for(i = 0; i < 4; i++)
      0008C2 D0 04            [24] 1429 	pop	ar4
      0008C4 D0 03            [24] 1430 	pop	ar3
      0008C6 D0 02            [24] 1431 	pop	ar2
                                   1432 ;	aes.c:148: temp[3] = w[4*(i-1) + 3];
      0008C8                       1433 00103$:
                                   1434 ;	aes.c:151: w[4*i+0] ^= temp[0];
      0008C8 C0 02            [24] 1435 	push	ar2
      0008CA C0 03            [24] 1436 	push	ar3
      0008CC C0 04            [24] 1437 	push	ar4
      0008CE C0 00            [24] 1438 	push	ar0
      0008D0 E5 08            [12] 1439 	mov	a,_bp
      0008D2 24 16            [12] 1440 	add	a,#0x16
      0008D4 F8               [12] 1441 	mov	r0,a
      0008D5 E6               [12] 1442 	mov	a,@r0
      0008D6 75 F0 04         [24] 1443 	mov	b,#0x04
      0008D9 A4               [48] 1444 	mul	ab
      0008DA FD               [12] 1445 	mov	r5,a
      0008DB AE F0            [24] 1446 	mov	r6,b
      0008DD A8 08            [24] 1447 	mov	r0,_bp
      0008DF 08               [12] 1448 	inc	r0
      0008E0 ED               [12] 1449 	mov	a,r5
      0008E1 26               [12] 1450 	add	a,@r0
      0008E2 FA               [12] 1451 	mov	r2,a
      0008E3 EE               [12] 1452 	mov	a,r6
      0008E4 08               [12] 1453 	inc	r0
      0008E5 36               [12] 1454 	addc	a,@r0
      0008E6 FB               [12] 1455 	mov	r3,a
      0008E7 08               [12] 1456 	inc	r0
      0008E8 86 04            [24] 1457 	mov	ar4,@r0
      0008EA 8A 82            [24] 1458 	mov	dpl,r2
      0008EC 8B 83            [24] 1459 	mov	dph,r3
      0008EE 8C F0            [24] 1460 	mov	b,r4
      0008F0 E5 08            [12] 1461 	mov	a,_bp
      0008F2 24 11            [12] 1462 	add	a,#0x11
      0008F4 F8               [12] 1463 	mov	r0,a
      0008F5 12 0F 4C         [24] 1464 	lcall	__gptrget
      0008F8 F6               [12] 1465 	mov	@r0,a
      0008F9 87 07            [24] 1466 	mov	ar7,@r1
      0008FB E5 08            [12] 1467 	mov	a,_bp
      0008FD 24 11            [12] 1468 	add	a,#0x11
      0008FF F8               [12] 1469 	mov	r0,a
      000900 E6               [12] 1470 	mov	a,@r0
      000901 62 07            [12] 1471 	xrl	ar7,a
      000903 8A 82            [24] 1472 	mov	dpl,r2
      000905 8B 83            [24] 1473 	mov	dph,r3
      000907 8C F0            [24] 1474 	mov	b,r4
      000909 EF               [12] 1475 	mov	a,r7
      00090A 12 0F 31         [24] 1476 	lcall	__gptrput
                                   1477 ;	aes.c:152: w[4*i+1] ^= temp[1];
      00090D 74 01            [12] 1478 	mov	a,#0x01
      00090F 2D               [12] 1479 	add	a,r5
      000910 FC               [12] 1480 	mov	r4,a
      000911 E4               [12] 1481 	clr	a
      000912 3E               [12] 1482 	addc	a,r6
      000913 FF               [12] 1483 	mov	r7,a
      000914 A8 08            [24] 1484 	mov	r0,_bp
      000916 08               [12] 1485 	inc	r0
      000917 EC               [12] 1486 	mov	a,r4
      000918 26               [12] 1487 	add	a,@r0
      000919 FC               [12] 1488 	mov	r4,a
      00091A EF               [12] 1489 	mov	a,r7
      00091B 08               [12] 1490 	inc	r0
      00091C 36               [12] 1491 	addc	a,@r0
      00091D FF               [12] 1492 	mov	r7,a
      00091E 08               [12] 1493 	inc	r0
      00091F 86 03            [24] 1494 	mov	ar3,@r0
      000921 8C 82            [24] 1495 	mov	dpl,r4
      000923 8F 83            [24] 1496 	mov	dph,r7
      000925 8B F0            [24] 1497 	mov	b,r3
      000927 E5 08            [12] 1498 	mov	a,_bp
      000929 24 11            [12] 1499 	add	a,#0x11
      00092B F8               [12] 1500 	mov	r0,a
      00092C 12 0F 4C         [24] 1501 	lcall	__gptrget
      00092F F6               [12] 1502 	mov	@r0,a
      000930 D0 00            [24] 1503 	pop	ar0
      000932 86 02            [24] 1504 	mov	ar2,@r0
      000934 C0 00            [24] 1505 	push	ar0
      000936 E5 08            [12] 1506 	mov	a,_bp
      000938 24 11            [12] 1507 	add	a,#0x11
      00093A F8               [12] 1508 	mov	r0,a
      00093B E6               [12] 1509 	mov	a,@r0
      00093C 62 02            [12] 1510 	xrl	ar2,a
      00093E 8C 82            [24] 1511 	mov	dpl,r4
      000940 8F 83            [24] 1512 	mov	dph,r7
      000942 8B F0            [24] 1513 	mov	b,r3
      000944 EA               [12] 1514 	mov	a,r2
      000945 12 0F 31         [24] 1515 	lcall	__gptrput
                                   1516 ;	aes.c:153: w[4*i+2] ^= temp[2];
      000948 74 02            [12] 1517 	mov	a,#0x02
      00094A 2D               [12] 1518 	add	a,r5
      00094B FC               [12] 1519 	mov	r4,a
      00094C E4               [12] 1520 	clr	a
      00094D 3E               [12] 1521 	addc	a,r6
      00094E FF               [12] 1522 	mov	r7,a
      00094F A8 08            [24] 1523 	mov	r0,_bp
      000951 08               [12] 1524 	inc	r0
      000952 EC               [12] 1525 	mov	a,r4
      000953 26               [12] 1526 	add	a,@r0
      000954 FC               [12] 1527 	mov	r4,a
      000955 EF               [12] 1528 	mov	a,r7
      000956 08               [12] 1529 	inc	r0
      000957 36               [12] 1530 	addc	a,@r0
      000958 FF               [12] 1531 	mov	r7,a
      000959 08               [12] 1532 	inc	r0
      00095A 86 03            [24] 1533 	mov	ar3,@r0
      00095C 8C 82            [24] 1534 	mov	dpl,r4
      00095E 8F 83            [24] 1535 	mov	dph,r7
      000960 8B F0            [24] 1536 	mov	b,r3
      000962 E5 08            [12] 1537 	mov	a,_bp
      000964 24 11            [12] 1538 	add	a,#0x11
      000966 F8               [12] 1539 	mov	r0,a
      000967 12 0F 4C         [24] 1540 	lcall	__gptrget
      00096A F6               [12] 1541 	mov	@r0,a
      00096B E5 08            [12] 1542 	mov	a,_bp
      00096D 24 0F            [12] 1543 	add	a,#0x0f
      00096F F8               [12] 1544 	mov	r0,a
      000970 86 00            [24] 1545 	mov	ar0,@r0
      000972 86 02            [24] 1546 	mov	ar2,@r0
      000974 E5 08            [12] 1547 	mov	a,_bp
      000976 24 11            [12] 1548 	add	a,#0x11
      000978 F8               [12] 1549 	mov	r0,a
      000979 E6               [12] 1550 	mov	a,@r0
      00097A 62 02            [12] 1551 	xrl	ar2,a
      00097C 8C 82            [24] 1552 	mov	dpl,r4
      00097E 8F 83            [24] 1553 	mov	dph,r7
      000980 8B F0            [24] 1554 	mov	b,r3
      000982 EA               [12] 1555 	mov	a,r2
      000983 12 0F 31         [24] 1556 	lcall	__gptrput
                                   1557 ;	aes.c:154: w[4*i+3] ^= temp[3];
      000986 74 03            [12] 1558 	mov	a,#0x03
      000988 2D               [12] 1559 	add	a,r5
      000989 FD               [12] 1560 	mov	r5,a
      00098A E4               [12] 1561 	clr	a
      00098B 3E               [12] 1562 	addc	a,r6
      00098C FE               [12] 1563 	mov	r6,a
      00098D A8 08            [24] 1564 	mov	r0,_bp
      00098F 08               [12] 1565 	inc	r0
      000990 ED               [12] 1566 	mov	a,r5
      000991 26               [12] 1567 	add	a,@r0
      000992 FD               [12] 1568 	mov	r5,a
      000993 EE               [12] 1569 	mov	a,r6
      000994 08               [12] 1570 	inc	r0
      000995 36               [12] 1571 	addc	a,@r0
      000996 FE               [12] 1572 	mov	r6,a
      000997 08               [12] 1573 	inc	r0
      000998 86 07            [24] 1574 	mov	ar7,@r0
      00099A 8D 82            [24] 1575 	mov	dpl,r5
      00099C 8E 83            [24] 1576 	mov	dph,r6
      00099E 8F F0            [24] 1577 	mov	b,r7
      0009A0 12 0F 4C         [24] 1578 	lcall	__gptrget
      0009A3 FC               [12] 1579 	mov	r4,a
      0009A4 E5 08            [12] 1580 	mov	a,_bp
      0009A6 24 10            [12] 1581 	add	a,#0x10
      0009A8 F8               [12] 1582 	mov	r0,a
      0009A9 86 00            [24] 1583 	mov	ar0,@r0
      0009AB E6               [12] 1584 	mov	a,@r0
      0009AC FB               [12] 1585 	mov	r3,a
      0009AD 62 04            [12] 1586 	xrl	ar4,a
      0009AF 8D 82            [24] 1587 	mov	dpl,r5
      0009B1 8E 83            [24] 1588 	mov	dph,r6
      0009B3 8F F0            [24] 1589 	mov	b,r7
      0009B5 EC               [12] 1590 	mov	a,r4
      0009B6 12 0F 31         [24] 1591 	lcall	__gptrput
                                   1592 ;	aes.c:134: for(i = 0; i < 4; i++)
      0009B9 E5 08            [12] 1593 	mov	a,_bp
      0009BB 24 16            [12] 1594 	add	a,#0x16
      0009BD F8               [12] 1595 	mov	r0,a
      0009BE 06               [12] 1596 	inc	@r0
      0009BF E5 08            [12] 1597 	mov	a,_bp
      0009C1 24 16            [12] 1598 	add	a,#0x16
      0009C3 F8               [12] 1599 	mov	r0,a
      0009C4 B6 04 00         [24] 1600 	cjne	@r0,#0x04,00118$
      0009C7                       1601 00118$:
      0009C7 D0 00            [24] 1602 	pop	ar0
      0009C9 D0 04            [24] 1603 	pop	ar4
      0009CB D0 03            [24] 1604 	pop	ar3
      0009CD D0 02            [24] 1605 	pop	ar2
      0009CF 50 03            [24] 1606 	jnc	00119$
      0009D1 02 07 89         [24] 1607 	ljmp	00105$
      0009D4                       1608 00119$:
      0009D4 85 08 81         [24] 1609 	mov	sp,_bp
      0009D7 D0 08            [24] 1610 	pop	_bp
      0009D9 22               [24] 1611 	ret
                                   1612 ;------------------------------------------------------------
                                   1613 ;Allocation info for local variables in function 'aes_encrypt'
                                   1614 ;------------------------------------------------------------
                                   1615 ;key                       Allocated to stack - _bp -5
                                   1616 ;state                     Allocated to stack - _bp +1
                                   1617 ;i                         Allocated to registers r3 
                                   1618 ;w                         Allocated to stack - _bp +4
                                   1619 ;------------------------------------------------------------
                                   1620 ;	aes.c:158: void aes_encrypt(uint8_t* state, uint8_t* key)
                                   1621 ;	-----------------------------------------
                                   1622 ;	 function aes_encrypt
                                   1623 ;	-----------------------------------------
      0009DA                       1624 _aes_encrypt:
      0009DA C0 08            [24] 1625 	push	_bp
      0009DC 85 81 08         [24] 1626 	mov	_bp,sp
      0009DF C0 82            [24] 1627 	push	dpl
      0009E1 C0 83            [24] 1628 	push	dph
      0009E3 C0 F0            [24] 1629 	push	b
      0009E5 E5 81            [12] 1630 	mov	a,sp
      0009E7 24 10            [12] 1631 	add	a,#0x10
      0009E9 F5 81            [12] 1632 	mov	sp,a
                                   1633 ;	aes.c:162: for(i = 0; i < KEYLEN; i++)
      0009EB E5 08            [12] 1634 	mov	a,_bp
      0009ED 24 04            [12] 1635 	add	a,#0x04
      0009EF FC               [12] 1636 	mov	r4,a
      0009F0 7B 00            [12] 1637 	mov	r3,#0x00
      0009F2                       1638 00103$:
                                   1639 ;	aes.c:163: w[i] = key[i];
      0009F2 EB               [12] 1640 	mov	a,r3
      0009F3 2C               [12] 1641 	add	a,r4
      0009F4 F9               [12] 1642 	mov	r1,a
      0009F5 E5 08            [12] 1643 	mov	a,_bp
      0009F7 24 FB            [12] 1644 	add	a,#0xfb
      0009F9 F8               [12] 1645 	mov	r0,a
      0009FA EB               [12] 1646 	mov	a,r3
      0009FB 26               [12] 1647 	add	a,@r0
      0009FC FA               [12] 1648 	mov	r2,a
      0009FD E4               [12] 1649 	clr	a
      0009FE 08               [12] 1650 	inc	r0
      0009FF 36               [12] 1651 	addc	a,@r0
      000A00 FE               [12] 1652 	mov	r6,a
      000A01 08               [12] 1653 	inc	r0
      000A02 86 07            [24] 1654 	mov	ar7,@r0
      000A04 8A 82            [24] 1655 	mov	dpl,r2
      000A06 8E 83            [24] 1656 	mov	dph,r6
      000A08 8F F0            [24] 1657 	mov	b,r7
      000A0A 12 0F 4C         [24] 1658 	lcall	__gptrget
      000A0D FA               [12] 1659 	mov	r2,a
      000A0E F7               [12] 1660 	mov	@r1,a
                                   1661 ;	aes.c:162: for(i = 0; i < KEYLEN; i++)
      000A0F 0B               [12] 1662 	inc	r3
      000A10 BB 10 00         [24] 1663 	cjne	r3,#0x10,00123$
      000A13                       1664 00123$:
      000A13 40 DD            [24] 1665 	jc	00103$
                                   1666 ;	aes.c:165: add_round_key(state, w);
      000A15 8C 03            [24] 1667 	mov	ar3,r4
      000A17 8B 02            [24] 1668 	mov	ar2,r3
      000A19 7B 00            [12] 1669 	mov	r3,#0x00
      000A1B 7F 40            [12] 1670 	mov	r7,#0x40
      000A1D C0 04            [24] 1671 	push	ar4
      000A1F C0 02            [24] 1672 	push	ar2
      000A21 C0 03            [24] 1673 	push	ar3
      000A23 C0 07            [24] 1674 	push	ar7
      000A25 A8 08            [24] 1675 	mov	r0,_bp
      000A27 08               [12] 1676 	inc	r0
      000A28 86 82            [24] 1677 	mov	dpl,@r0
      000A2A 08               [12] 1678 	inc	r0
      000A2B 86 83            [24] 1679 	mov	dph,@r0
      000A2D 08               [12] 1680 	inc	r0
      000A2E 86 F0            [24] 1681 	mov	b,@r0
      000A30 12 01 77         [24] 1682 	lcall	_add_round_key
      000A33 15 81            [12] 1683 	dec	sp
      000A35 15 81            [12] 1684 	dec	sp
      000A37 15 81            [12] 1685 	dec	sp
      000A39 D0 04            [24] 1686 	pop	ar4
                                   1687 ;	aes.c:166: for(i = 1; i < Nr; i++)
      000A3B 8C 07            [24] 1688 	mov	ar7,r4
      000A3D 8C 06            [24] 1689 	mov	ar6,r4
      000A3F 7D 01            [12] 1690 	mov	r5,#0x01
      000A41                       1691 00105$:
                                   1692 ;	aes.c:168: sub_bytes(state);
      000A41 C0 04            [24] 1693 	push	ar4
      000A43 A8 08            [24] 1694 	mov	r0,_bp
      000A45 08               [12] 1695 	inc	r0
      000A46 86 82            [24] 1696 	mov	dpl,@r0
      000A48 08               [12] 1697 	inc	r0
      000A49 86 83            [24] 1698 	mov	dph,@r0
      000A4B 08               [12] 1699 	inc	r0
      000A4C 86 F0            [24] 1700 	mov	b,@r0
      000A4E C0 07            [24] 1701 	push	ar7
      000A50 C0 06            [24] 1702 	push	ar6
      000A52 C0 05            [24] 1703 	push	ar5
      000A54 C0 04            [24] 1704 	push	ar4
      000A56 12 01 F8         [24] 1705 	lcall	_sub_bytes
                                   1706 ;	aes.c:169: shift_rows(state);
      000A59 A8 08            [24] 1707 	mov	r0,_bp
      000A5B 08               [12] 1708 	inc	r0
      000A5C 86 82            [24] 1709 	mov	dpl,@r0
      000A5E 08               [12] 1710 	inc	r0
      000A5F 86 83            [24] 1711 	mov	dph,@r0
      000A61 08               [12] 1712 	inc	r0
      000A62 86 F0            [24] 1713 	mov	b,@r0
      000A64 12 02 26         [24] 1714 	lcall	_shift_rows
                                   1715 ;	aes.c:170: mix_columns(state);
      000A67 A8 08            [24] 1716 	mov	r0,_bp
      000A69 08               [12] 1717 	inc	r0
      000A6A 86 82            [24] 1718 	mov	dpl,@r0
      000A6C 08               [12] 1719 	inc	r0
      000A6D 86 83            [24] 1720 	mov	dph,@r0
      000A6F 08               [12] 1721 	inc	r0
      000A70 86 F0            [24] 1722 	mov	b,@r0
      000A72 12 04 6F         [24] 1723 	lcall	_mix_columns
      000A75 D0 04            [24] 1724 	pop	ar4
      000A77 D0 05            [24] 1725 	pop	ar5
      000A79 D0 06            [24] 1726 	pop	ar6
      000A7B D0 07            [24] 1727 	pop	ar7
                                   1728 ;	aes.c:171: update_key(w, i);	
      000A7D 8F 02            [24] 1729 	mov	ar2,r7
      000A7F 7B 00            [12] 1730 	mov	r3,#0x00
      000A81 7C 40            [12] 1731 	mov	r4,#0x40
      000A83 C0 07            [24] 1732 	push	ar7
      000A85 C0 06            [24] 1733 	push	ar6
      000A87 C0 05            [24] 1734 	push	ar5
      000A89 C0 04            [24] 1735 	push	ar4
      000A8B C0 05            [24] 1736 	push	ar5
      000A8D 8A 82            [24] 1737 	mov	dpl,r2
      000A8F 8B 83            [24] 1738 	mov	dph,r3
      000A91 8C F0            [24] 1739 	mov	b,r4
      000A93 12 06 F4         [24] 1740 	lcall	_update_key
      000A96 15 81            [12] 1741 	dec	sp
      000A98 D0 04            [24] 1742 	pop	ar4
      000A9A D0 05            [24] 1743 	pop	ar5
      000A9C D0 06            [24] 1744 	pop	ar6
                                   1745 ;	aes.c:172: add_round_key(state, w);
      000A9E 8E 02            [24] 1746 	mov	ar2,r6
      000AA0 7B 00            [12] 1747 	mov	r3,#0x00
      000AA2 7C 40            [12] 1748 	mov	r4,#0x40
      000AA4 C0 06            [24] 1749 	push	ar6
      000AA6 C0 05            [24] 1750 	push	ar5
      000AA8 C0 04            [24] 1751 	push	ar4
      000AAA C0 02            [24] 1752 	push	ar2
      000AAC C0 03            [24] 1753 	push	ar3
      000AAE C0 04            [24] 1754 	push	ar4
      000AB0 A8 08            [24] 1755 	mov	r0,_bp
      000AB2 08               [12] 1756 	inc	r0
      000AB3 86 82            [24] 1757 	mov	dpl,@r0
      000AB5 08               [12] 1758 	inc	r0
      000AB6 86 83            [24] 1759 	mov	dph,@r0
      000AB8 08               [12] 1760 	inc	r0
      000AB9 86 F0            [24] 1761 	mov	b,@r0
      000ABB 12 01 77         [24] 1762 	lcall	_add_round_key
      000ABE 15 81            [12] 1763 	dec	sp
      000AC0 15 81            [12] 1764 	dec	sp
      000AC2 15 81            [12] 1765 	dec	sp
      000AC4 D0 04            [24] 1766 	pop	ar4
      000AC6 D0 05            [24] 1767 	pop	ar5
      000AC8 D0 06            [24] 1768 	pop	ar6
      000ACA D0 07            [24] 1769 	pop	ar7
                                   1770 ;	aes.c:166: for(i = 1; i < Nr; i++)
      000ACC 0D               [12] 1771 	inc	r5
      000ACD BD 0A 00         [24] 1772 	cjne	r5,#0x0a,00125$
      000AD0                       1773 00125$:
      000AD0 D0 04            [24] 1774 	pop	ar4
      000AD2 50 03            [24] 1775 	jnc	00126$
      000AD4 02 0A 41         [24] 1776 	ljmp	00105$
      000AD7                       1777 00126$:
                                   1778 ;	aes.c:175: sub_bytes(state);
      000AD7 A8 08            [24] 1779 	mov	r0,_bp
      000AD9 08               [12] 1780 	inc	r0
      000ADA 86 82            [24] 1781 	mov	dpl,@r0
      000ADC 08               [12] 1782 	inc	r0
      000ADD 86 83            [24] 1783 	mov	dph,@r0
      000ADF 08               [12] 1784 	inc	r0
      000AE0 86 F0            [24] 1785 	mov	b,@r0
      000AE2 C0 05            [24] 1786 	push	ar5
      000AE4 C0 04            [24] 1787 	push	ar4
      000AE6 12 01 F8         [24] 1788 	lcall	_sub_bytes
                                   1789 ;	aes.c:176: shift_rows(state);
      000AE9 A8 08            [24] 1790 	mov	r0,_bp
      000AEB 08               [12] 1791 	inc	r0
      000AEC 86 82            [24] 1792 	mov	dpl,@r0
      000AEE 08               [12] 1793 	inc	r0
      000AEF 86 83            [24] 1794 	mov	dph,@r0
      000AF1 08               [12] 1795 	inc	r0
      000AF2 86 F0            [24] 1796 	mov	b,@r0
      000AF4 12 02 26         [24] 1797 	lcall	_shift_rows
      000AF7 D0 04            [24] 1798 	pop	ar4
      000AF9 D0 05            [24] 1799 	pop	ar5
                                   1800 ;	aes.c:177: update_key(w, i);
      000AFB 8C 07            [24] 1801 	mov	ar7,r4
      000AFD 7E 00            [12] 1802 	mov	r6,#0x00
      000AFF 7B 40            [12] 1803 	mov	r3,#0x40
      000B01 C0 04            [24] 1804 	push	ar4
      000B03 C0 05            [24] 1805 	push	ar5
      000B05 8F 82            [24] 1806 	mov	dpl,r7
      000B07 8E 83            [24] 1807 	mov	dph,r6
      000B09 8B F0            [24] 1808 	mov	b,r3
      000B0B 12 06 F4         [24] 1809 	lcall	_update_key
      000B0E 15 81            [12] 1810 	dec	sp
                                   1811 ;	aes.c:178: add_round_key(state, w);
      000B10 7F 00            [12] 1812 	mov	r7,#0x00
      000B12 7E 40            [12] 1813 	mov	r6,#0x40
      000B14 C0 07            [24] 1814 	push	ar7
      000B16 C0 06            [24] 1815 	push	ar6
      000B18 A8 08            [24] 1816 	mov	r0,_bp
      000B1A 08               [12] 1817 	inc	r0
      000B1B 86 82            [24] 1818 	mov	dpl,@r0
      000B1D 08               [12] 1819 	inc	r0
      000B1E 86 83            [24] 1820 	mov	dph,@r0
      000B20 08               [12] 1821 	inc	r0
      000B21 86 F0            [24] 1822 	mov	b,@r0
      000B23 12 01 77         [24] 1823 	lcall	_add_round_key
      000B26 15 81            [12] 1824 	dec	sp
      000B28 15 81            [12] 1825 	dec	sp
      000B2A 15 81            [12] 1826 	dec	sp
      000B2C 85 08 81         [24] 1827 	mov	sp,_bp
      000B2F D0 08            [24] 1828 	pop	_bp
      000B31 22               [24] 1829 	ret
                                   1830 	.area CSEG    (CODE)
                                   1831 	.area CONST   (CODE)
      000F6C                       1832 _s_box:
      000F6C 63                    1833 	.db #0x63	; 99	'c'
      000F6D 7C                    1834 	.db #0x7c	; 124
      000F6E 77                    1835 	.db #0x77	; 119	'w'
      000F6F 7B                    1836 	.db #0x7b	; 123
      000F70 F2                    1837 	.db #0xf2	; 242
      000F71 6B                    1838 	.db #0x6b	; 107	'k'
      000F72 6F                    1839 	.db #0x6f	; 111	'o'
      000F73 C5                    1840 	.db #0xc5	; 197
      000F74 30                    1841 	.db #0x30	; 48	'0'
      000F75 01                    1842 	.db #0x01	; 1
      000F76 67                    1843 	.db #0x67	; 103	'g'
      000F77 2B                    1844 	.db #0x2b	; 43
      000F78 FE                    1845 	.db #0xfe	; 254
      000F79 D7                    1846 	.db #0xd7	; 215
      000F7A AB                    1847 	.db #0xab	; 171
      000F7B 76                    1848 	.db #0x76	; 118	'v'
      000F7C CA                    1849 	.db #0xca	; 202
      000F7D 82                    1850 	.db #0x82	; 130
      000F7E C9                    1851 	.db #0xc9	; 201
      000F7F 7D                    1852 	.db #0x7d	; 125
      000F80 FA                    1853 	.db #0xfa	; 250
      000F81 59                    1854 	.db #0x59	; 89	'Y'
      000F82 47                    1855 	.db #0x47	; 71	'G'
      000F83 F0                    1856 	.db #0xf0	; 240
      000F84 AD                    1857 	.db #0xad	; 173
      000F85 D4                    1858 	.db #0xd4	; 212
      000F86 A2                    1859 	.db #0xa2	; 162
      000F87 AF                    1860 	.db #0xaf	; 175
      000F88 9C                    1861 	.db #0x9c	; 156
      000F89 A4                    1862 	.db #0xa4	; 164
      000F8A 72                    1863 	.db #0x72	; 114	'r'
      000F8B C0                    1864 	.db #0xc0	; 192
      000F8C B7                    1865 	.db #0xb7	; 183
      000F8D FD                    1866 	.db #0xfd	; 253
      000F8E 93                    1867 	.db #0x93	; 147
      000F8F 26                    1868 	.db #0x26	; 38
      000F90 36                    1869 	.db #0x36	; 54	'6'
      000F91 3F                    1870 	.db #0x3f	; 63
      000F92 F7                    1871 	.db #0xf7	; 247
      000F93 CC                    1872 	.db #0xcc	; 204
      000F94 34                    1873 	.db #0x34	; 52	'4'
      000F95 A5                    1874 	.db #0xa5	; 165
      000F96 E5                    1875 	.db #0xe5	; 229
      000F97 F1                    1876 	.db #0xf1	; 241
      000F98 71                    1877 	.db #0x71	; 113	'q'
      000F99 D8                    1878 	.db #0xd8	; 216
      000F9A 31                    1879 	.db #0x31	; 49	'1'
      000F9B 15                    1880 	.db #0x15	; 21
      000F9C 04                    1881 	.db #0x04	; 4
      000F9D C7                    1882 	.db #0xc7	; 199
      000F9E 23                    1883 	.db #0x23	; 35
      000F9F C3                    1884 	.db #0xc3	; 195
      000FA0 18                    1885 	.db #0x18	; 24
      000FA1 96                    1886 	.db #0x96	; 150
      000FA2 05                    1887 	.db #0x05	; 5
      000FA3 9A                    1888 	.db #0x9a	; 154
      000FA4 07                    1889 	.db #0x07	; 7
      000FA5 12                    1890 	.db #0x12	; 18
      000FA6 80                    1891 	.db #0x80	; 128
      000FA7 E2                    1892 	.db #0xe2	; 226
      000FA8 EB                    1893 	.db #0xeb	; 235
      000FA9 27                    1894 	.db #0x27	; 39
      000FAA B2                    1895 	.db #0xb2	; 178
      000FAB 75                    1896 	.db #0x75	; 117	'u'
      000FAC 09                    1897 	.db #0x09	; 9
      000FAD 83                    1898 	.db #0x83	; 131
      000FAE 2C                    1899 	.db #0x2c	; 44
      000FAF 1A                    1900 	.db #0x1a	; 26
      000FB0 1B                    1901 	.db #0x1b	; 27
      000FB1 6E                    1902 	.db #0x6e	; 110	'n'
      000FB2 5A                    1903 	.db #0x5a	; 90	'Z'
      000FB3 A0                    1904 	.db #0xa0	; 160
      000FB4 52                    1905 	.db #0x52	; 82	'R'
      000FB5 3B                    1906 	.db #0x3b	; 59
      000FB6 D6                    1907 	.db #0xd6	; 214
      000FB7 B3                    1908 	.db #0xb3	; 179
      000FB8 29                    1909 	.db #0x29	; 41
      000FB9 E3                    1910 	.db #0xe3	; 227
      000FBA 2F                    1911 	.db #0x2f	; 47
      000FBB 84                    1912 	.db #0x84	; 132
      000FBC 53                    1913 	.db #0x53	; 83	'S'
      000FBD D1                    1914 	.db #0xd1	; 209
      000FBE 00                    1915 	.db #0x00	; 0
      000FBF ED                    1916 	.db #0xed	; 237
      000FC0 20                    1917 	.db #0x20	; 32
      000FC1 FC                    1918 	.db #0xfc	; 252
      000FC2 B1                    1919 	.db #0xb1	; 177
      000FC3 5B                    1920 	.db #0x5b	; 91
      000FC4 6A                    1921 	.db #0x6a	; 106	'j'
      000FC5 CB                    1922 	.db #0xcb	; 203
      000FC6 BE                    1923 	.db #0xbe	; 190
      000FC7 39                    1924 	.db #0x39	; 57	'9'
      000FC8 4A                    1925 	.db #0x4a	; 74	'J'
      000FC9 4C                    1926 	.db #0x4c	; 76	'L'
      000FCA 58                    1927 	.db #0x58	; 88	'X'
      000FCB CF                    1928 	.db #0xcf	; 207
      000FCC D0                    1929 	.db #0xd0	; 208
      000FCD EF                    1930 	.db #0xef	; 239
      000FCE AA                    1931 	.db #0xaa	; 170
      000FCF FB                    1932 	.db #0xfb	; 251
      000FD0 43                    1933 	.db #0x43	; 67	'C'
      000FD1 4D                    1934 	.db #0x4d	; 77	'M'
      000FD2 33                    1935 	.db #0x33	; 51	'3'
      000FD3 85                    1936 	.db #0x85	; 133
      000FD4 45                    1937 	.db #0x45	; 69	'E'
      000FD5 F9                    1938 	.db #0xf9	; 249
      000FD6 02                    1939 	.db #0x02	; 2
      000FD7 7F                    1940 	.db #0x7f	; 127
      000FD8 50                    1941 	.db #0x50	; 80	'P'
      000FD9 3C                    1942 	.db #0x3c	; 60
      000FDA 9F                    1943 	.db #0x9f	; 159
      000FDB A8                    1944 	.db #0xa8	; 168
      000FDC 51                    1945 	.db #0x51	; 81	'Q'
      000FDD A3                    1946 	.db #0xa3	; 163
      000FDE 40                    1947 	.db #0x40	; 64
      000FDF 8F                    1948 	.db #0x8f	; 143
      000FE0 92                    1949 	.db #0x92	; 146
      000FE1 9D                    1950 	.db #0x9d	; 157
      000FE2 38                    1951 	.db #0x38	; 56	'8'
      000FE3 F5                    1952 	.db #0xf5	; 245
      000FE4 BC                    1953 	.db #0xbc	; 188
      000FE5 B6                    1954 	.db #0xb6	; 182
      000FE6 DA                    1955 	.db #0xda	; 218
      000FE7 21                    1956 	.db #0x21	; 33
      000FE8 10                    1957 	.db #0x10	; 16
      000FE9 FF                    1958 	.db #0xff	; 255
      000FEA F3                    1959 	.db #0xf3	; 243
      000FEB D2                    1960 	.db #0xd2	; 210
      000FEC CD                    1961 	.db #0xcd	; 205
      000FED 0C                    1962 	.db #0x0c	; 12
      000FEE 13                    1963 	.db #0x13	; 19
      000FEF EC                    1964 	.db #0xec	; 236
      000FF0 5F                    1965 	.db #0x5f	; 95
      000FF1 97                    1966 	.db #0x97	; 151
      000FF2 44                    1967 	.db #0x44	; 68	'D'
      000FF3 17                    1968 	.db #0x17	; 23
      000FF4 C4                    1969 	.db #0xc4	; 196
      000FF5 A7                    1970 	.db #0xa7	; 167
      000FF6 7E                    1971 	.db #0x7e	; 126
      000FF7 3D                    1972 	.db #0x3d	; 61
      000FF8 64                    1973 	.db #0x64	; 100	'd'
      000FF9 5D                    1974 	.db #0x5d	; 93
      000FFA 19                    1975 	.db #0x19	; 25
      000FFB 73                    1976 	.db #0x73	; 115	's'
      000FFC 60                    1977 	.db #0x60	; 96
      000FFD 81                    1978 	.db #0x81	; 129
      000FFE 4F                    1979 	.db #0x4f	; 79	'O'
      000FFF DC                    1980 	.db #0xdc	; 220
      001000 22                    1981 	.db #0x22	; 34
      001001 2A                    1982 	.db #0x2a	; 42
      001002 90                    1983 	.db #0x90	; 144
      001003 88                    1984 	.db #0x88	; 136
      001004 46                    1985 	.db #0x46	; 70	'F'
      001005 EE                    1986 	.db #0xee	; 238
      001006 B8                    1987 	.db #0xb8	; 184
      001007 14                    1988 	.db #0x14	; 20
      001008 DE                    1989 	.db #0xde	; 222
      001009 5E                    1990 	.db #0x5e	; 94
      00100A 0B                    1991 	.db #0x0b	; 11
      00100B DB                    1992 	.db #0xdb	; 219
      00100C E0                    1993 	.db #0xe0	; 224
      00100D 32                    1994 	.db #0x32	; 50	'2'
      00100E 3A                    1995 	.db #0x3a	; 58
      00100F 0A                    1996 	.db #0x0a	; 10
      001010 49                    1997 	.db #0x49	; 73	'I'
      001011 06                    1998 	.db #0x06	; 6
      001012 24                    1999 	.db #0x24	; 36
      001013 5C                    2000 	.db #0x5c	; 92
      001014 C2                    2001 	.db #0xc2	; 194
      001015 D3                    2002 	.db #0xd3	; 211
      001016 AC                    2003 	.db #0xac	; 172
      001017 62                    2004 	.db #0x62	; 98	'b'
      001018 91                    2005 	.db #0x91	; 145
      001019 95                    2006 	.db #0x95	; 149
      00101A E4                    2007 	.db #0xe4	; 228
      00101B 79                    2008 	.db #0x79	; 121	'y'
      00101C E7                    2009 	.db #0xe7	; 231
      00101D C8                    2010 	.db #0xc8	; 200
      00101E 37                    2011 	.db #0x37	; 55	'7'
      00101F 6D                    2012 	.db #0x6d	; 109	'm'
      001020 8D                    2013 	.db #0x8d	; 141
      001021 D5                    2014 	.db #0xd5	; 213
      001022 4E                    2015 	.db #0x4e	; 78	'N'
      001023 A9                    2016 	.db #0xa9	; 169
      001024 6C                    2017 	.db #0x6c	; 108	'l'
      001025 56                    2018 	.db #0x56	; 86	'V'
      001026 F4                    2019 	.db #0xf4	; 244
      001027 EA                    2020 	.db #0xea	; 234
      001028 65                    2021 	.db #0x65	; 101	'e'
      001029 7A                    2022 	.db #0x7a	; 122	'z'
      00102A AE                    2023 	.db #0xae	; 174
      00102B 08                    2024 	.db #0x08	; 8
      00102C BA                    2025 	.db #0xba	; 186
      00102D 78                    2026 	.db #0x78	; 120	'x'
      00102E 25                    2027 	.db #0x25	; 37
      00102F 2E                    2028 	.db #0x2e	; 46
      001030 1C                    2029 	.db #0x1c	; 28
      001031 A6                    2030 	.db #0xa6	; 166
      001032 B4                    2031 	.db #0xb4	; 180
      001033 C6                    2032 	.db #0xc6	; 198
      001034 E8                    2033 	.db #0xe8	; 232
      001035 DD                    2034 	.db #0xdd	; 221
      001036 74                    2035 	.db #0x74	; 116	't'
      001037 1F                    2036 	.db #0x1f	; 31
      001038 4B                    2037 	.db #0x4b	; 75	'K'
      001039 BD                    2038 	.db #0xbd	; 189
      00103A 8B                    2039 	.db #0x8b	; 139
      00103B 8A                    2040 	.db #0x8a	; 138
      00103C 70                    2041 	.db #0x70	; 112	'p'
      00103D 3E                    2042 	.db #0x3e	; 62
      00103E B5                    2043 	.db #0xb5	; 181
      00103F 66                    2044 	.db #0x66	; 102	'f'
      001040 48                    2045 	.db #0x48	; 72	'H'
      001041 03                    2046 	.db #0x03	; 3
      001042 F6                    2047 	.db #0xf6	; 246
      001043 0E                    2048 	.db #0x0e	; 14
      001044 61                    2049 	.db #0x61	; 97	'a'
      001045 35                    2050 	.db #0x35	; 53	'5'
      001046 57                    2051 	.db #0x57	; 87	'W'
      001047 B9                    2052 	.db #0xb9	; 185
      001048 86                    2053 	.db #0x86	; 134
      001049 C1                    2054 	.db #0xc1	; 193
      00104A 1D                    2055 	.db #0x1d	; 29
      00104B 9E                    2056 	.db #0x9e	; 158
      00104C E1                    2057 	.db #0xe1	; 225
      00104D F8                    2058 	.db #0xf8	; 248
      00104E 98                    2059 	.db #0x98	; 152
      00104F 11                    2060 	.db #0x11	; 17
      001050 69                    2061 	.db #0x69	; 105	'i'
      001051 D9                    2062 	.db #0xd9	; 217
      001052 8E                    2063 	.db #0x8e	; 142
      001053 94                    2064 	.db #0x94	; 148
      001054 9B                    2065 	.db #0x9b	; 155
      001055 1E                    2066 	.db #0x1e	; 30
      001056 87                    2067 	.db #0x87	; 135
      001057 E9                    2068 	.db #0xe9	; 233
      001058 CE                    2069 	.db #0xce	; 206
      001059 55                    2070 	.db #0x55	; 85	'U'
      00105A 28                    2071 	.db #0x28	; 40
      00105B DF                    2072 	.db #0xdf	; 223
      00105C 8C                    2073 	.db #0x8c	; 140
      00105D A1                    2074 	.db #0xa1	; 161
      00105E 89                    2075 	.db #0x89	; 137
      00105F 0D                    2076 	.db #0x0d	; 13
      001060 BF                    2077 	.db #0xbf	; 191
      001061 E6                    2078 	.db #0xe6	; 230
      001062 42                    2079 	.db #0x42	; 66	'B'
      001063 68                    2080 	.db #0x68	; 104	'h'
      001064 41                    2081 	.db #0x41	; 65	'A'
      001065 99                    2082 	.db #0x99	; 153
      001066 2D                    2083 	.db #0x2d	; 45
      001067 0F                    2084 	.db #0x0f	; 15
      001068 B0                    2085 	.db #0xb0	; 176
      001069 54                    2086 	.db #0x54	; 84	'T'
      00106A BB                    2087 	.db #0xbb	; 187
      00106B 16                    2088 	.db #0x16	; 22
      00106C                       2089 _rcon:
      00106C 00                    2090 	.db #0x00	; 0
      00106D 01                    2091 	.db #0x01	; 1
      00106E 02                    2092 	.db #0x02	; 2
      00106F 04                    2093 	.db #0x04	; 4
      001070 08                    2094 	.db #0x08	; 8
      001071 10                    2095 	.db #0x10	; 16
      001072 20                    2096 	.db #0x20	; 32
      001073 40                    2097 	.db #0x40	; 64
      001074 80                    2098 	.db #0x80	; 128
      001075 1B                    2099 	.db #0x1b	; 27
      001076 36                    2100 	.db #0x36	; 54	'6'
                                   2101 	.area XINIT   (CODE)
                                   2102 	.area CABS    (ABS,CODE)
