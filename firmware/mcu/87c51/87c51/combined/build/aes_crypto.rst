                                      1 ;--------------------------------------------------------
                                      2 ; File Created by SDCC : free open source ANSI-C Compiler
                                      3 ; Version 3.6.0 #9615 (MINGW32)
                                      4 ;--------------------------------------------------------
                                      5 	.module aes_crypto
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
                                    100 ;	aes_crypto.c:70: uint8_t xtime(uint8_t x)
                                    101 ;	-----------------------------------------
                                    102 ;	 function xtime
                                    103 ;	-----------------------------------------
      0001A5                        104 _xtime:
                           000007   105 	ar7 = 0x07
                           000006   106 	ar6 = 0x06
                           000005   107 	ar5 = 0x05
                           000004   108 	ar4 = 0x04
                           000003   109 	ar3 = 0x03
                           000002   110 	ar2 = 0x02
                           000001   111 	ar1 = 0x01
                           000000   112 	ar0 = 0x00
                                    113 ;	aes_crypto.c:72: return ((x<<1) ^ (((x>>7) & 1) * 0x1b));
      0001A5 E5 82            [12]  114 	mov	a,dpl
      0001A7 FF               [12]  115 	mov	r7,a
      0001A8 25 E0            [12]  116 	add	a,acc
      0001AA FE               [12]  117 	mov	r6,a
      0001AB EF               [12]  118 	mov	a,r7
      0001AC 23               [12]  119 	rl	a
      0001AD 54 01            [12]  120 	anl	a,#0x01
      0001AF 75 F0 1B         [24]  121 	mov	b,#0x1b
      0001B2 A4               [48]  122 	mul	ab
      0001B3 6E               [12]  123 	xrl	a,r6
      0001B4 F5 82            [12]  124 	mov	dpl,a
      0001B6 22               [24]  125 	ret
                                    126 ;------------------------------------------------------------
                                    127 ;Allocation info for local variables in function 'add_round_key'
                                    128 ;------------------------------------------------------------
                                    129 ;w                         Allocated to stack - _bp -5
                                    130 ;state                     Allocated to stack - _bp +2
                                    131 ;i                         Allocated to stack - _bp +5
                                    132 ;sloc0                     Allocated to stack - _bp +1
                                    133 ;------------------------------------------------------------
                                    134 ;	aes_crypto.c:76: void add_round_key(uint8_t* state, uint8_t* w)
                                    135 ;	-----------------------------------------
                                    136 ;	 function add_round_key
                                    137 ;	-----------------------------------------
      0001B7                        138 _add_round_key:
      0001B7 C0 08            [24]  139 	push	_bp
      0001B9 E5 81            [12]  140 	mov	a,sp
      0001BB F5 08            [12]  141 	mov	_bp,a
      0001BD 24 05            [12]  142 	add	a,#0x05
      0001BF F5 81            [12]  143 	mov	sp,a
      0001C1 A8 08            [24]  144 	mov	r0,_bp
      0001C3 08               [12]  145 	inc	r0
      0001C4 08               [12]  146 	inc	r0
      0001C5 A6 82            [24]  147 	mov	@r0,dpl
      0001C7 08               [12]  148 	inc	r0
      0001C8 A6 83            [24]  149 	mov	@r0,dph
      0001CA 08               [12]  150 	inc	r0
      0001CB A6 F0            [24]  151 	mov	@r0,b
                                    152 ;	aes_crypto.c:79: for(i = 0; i < KEYLEN; i++)
      0001CD E5 08            [12]  153 	mov	a,_bp
      0001CF 24 05            [12]  154 	add	a,#0x05
      0001D1 F8               [12]  155 	mov	r0,a
      0001D2 76 00            [12]  156 	mov	@r0,#0x00
      0001D4                        157 00102$:
                                    158 ;	aes_crypto.c:80: state[i] ^= w[i];
      0001D4 A8 08            [24]  159 	mov	r0,_bp
      0001D6 08               [12]  160 	inc	r0
      0001D7 08               [12]  161 	inc	r0
      0001D8 E5 08            [12]  162 	mov	a,_bp
      0001DA 24 05            [12]  163 	add	a,#0x05
      0001DC F9               [12]  164 	mov	r1,a
      0001DD E7               [12]  165 	mov	a,@r1
      0001DE 26               [12]  166 	add	a,@r0
      0001DF FA               [12]  167 	mov	r2,a
      0001E0 E4               [12]  168 	clr	a
      0001E1 08               [12]  169 	inc	r0
      0001E2 36               [12]  170 	addc	a,@r0
      0001E3 FB               [12]  171 	mov	r3,a
      0001E4 08               [12]  172 	inc	r0
      0001E5 86 07            [24]  173 	mov	ar7,@r0
      0001E7 8A 82            [24]  174 	mov	dpl,r2
      0001E9 8B 83            [24]  175 	mov	dph,r3
      0001EB 8F F0            [24]  176 	mov	b,r7
      0001ED A8 08            [24]  177 	mov	r0,_bp
      0001EF 08               [12]  178 	inc	r0
      0001F0 12 1C A1         [24]  179 	lcall	__gptrget
      0001F3 F6               [12]  180 	mov	@r0,a
      0001F4 E5 08            [12]  181 	mov	a,_bp
      0001F6 24 FB            [12]  182 	add	a,#0xfb
      0001F8 F8               [12]  183 	mov	r0,a
      0001F9 E5 08            [12]  184 	mov	a,_bp
      0001FB 24 05            [12]  185 	add	a,#0x05
      0001FD F9               [12]  186 	mov	r1,a
      0001FE E7               [12]  187 	mov	a,@r1
      0001FF 26               [12]  188 	add	a,@r0
      000200 FC               [12]  189 	mov	r4,a
      000201 E4               [12]  190 	clr	a
      000202 08               [12]  191 	inc	r0
      000203 36               [12]  192 	addc	a,@r0
      000204 FD               [12]  193 	mov	r5,a
      000205 08               [12]  194 	inc	r0
      000206 86 06            [24]  195 	mov	ar6,@r0
      000208 8C 82            [24]  196 	mov	dpl,r4
      00020A 8D 83            [24]  197 	mov	dph,r5
      00020C 8E F0            [24]  198 	mov	b,r6
      00020E 12 1C A1         [24]  199 	lcall	__gptrget
      000211 FC               [12]  200 	mov	r4,a
      000212 A8 08            [24]  201 	mov	r0,_bp
      000214 08               [12]  202 	inc	r0
      000215 E6               [12]  203 	mov	a,@r0
      000216 62 04            [12]  204 	xrl	ar4,a
      000218 8A 82            [24]  205 	mov	dpl,r2
      00021A 8B 83            [24]  206 	mov	dph,r3
      00021C 8F F0            [24]  207 	mov	b,r7
      00021E EC               [12]  208 	mov	a,r4
      00021F 12 1B 7D         [24]  209 	lcall	__gptrput
                                    210 ;	aes_crypto.c:79: for(i = 0; i < KEYLEN; i++)
      000222 E5 08            [12]  211 	mov	a,_bp
      000224 24 05            [12]  212 	add	a,#0x05
      000226 F8               [12]  213 	mov	r0,a
      000227 06               [12]  214 	inc	@r0
      000228 E5 08            [12]  215 	mov	a,_bp
      00022A 24 05            [12]  216 	add	a,#0x05
      00022C F8               [12]  217 	mov	r0,a
      00022D B6 10 00         [24]  218 	cjne	@r0,#0x10,00110$
      000230                        219 00110$:
      000230 40 A2            [24]  220 	jc	00102$
      000232 85 08 81         [24]  221 	mov	sp,_bp
      000235 D0 08            [24]  222 	pop	_bp
      000237 22               [24]  223 	ret
                                    224 ;------------------------------------------------------------
                                    225 ;Allocation info for local variables in function 'sub_bytes'
                                    226 ;------------------------------------------------------------
                                    227 ;state                     Allocated to registers r5 r6 r7 
                                    228 ;i                         Allocated to registers r4 
                                    229 ;------------------------------------------------------------
                                    230 ;	aes_crypto.c:83: void sub_bytes(uint8_t* state)
                                    231 ;	-----------------------------------------
                                    232 ;	 function sub_bytes
                                    233 ;	-----------------------------------------
      000238                        234 _sub_bytes:
      000238 AD 82            [24]  235 	mov	r5,dpl
      00023A AE 83            [24]  236 	mov	r6,dph
      00023C AF F0            [24]  237 	mov	r7,b
                                    238 ;	aes_crypto.c:86: for(i = 0; i < KEYLEN; i++)
      00023E 7C 00            [12]  239 	mov	r4,#0x00
      000240                        240 00102$:
                                    241 ;	aes_crypto.c:87: state[i] = s_box[state[i]];
      000240 EC               [12]  242 	mov	a,r4
      000241 2D               [12]  243 	add	a,r5
      000242 F9               [12]  244 	mov	r1,a
      000243 E4               [12]  245 	clr	a
      000244 3E               [12]  246 	addc	a,r6
      000245 FA               [12]  247 	mov	r2,a
      000246 8F 03            [24]  248 	mov	ar3,r7
      000248 89 82            [24]  249 	mov	dpl,r1
      00024A 8A 83            [24]  250 	mov	dph,r2
      00024C 8B F0            [24]  251 	mov	b,r3
      00024E 12 1C A1         [24]  252 	lcall	__gptrget
      000251 90 1C C1         [24]  253 	mov	dptr,#_s_box
      000254 93               [24]  254 	movc	a,@a+dptr
      000255 F8               [12]  255 	mov	r0,a
      000256 89 82            [24]  256 	mov	dpl,r1
      000258 8A 83            [24]  257 	mov	dph,r2
      00025A 8B F0            [24]  258 	mov	b,r3
      00025C 12 1B 7D         [24]  259 	lcall	__gptrput
                                    260 ;	aes_crypto.c:86: for(i = 0; i < KEYLEN; i++)
      00025F 0C               [12]  261 	inc	r4
      000260 BC 10 00         [24]  262 	cjne	r4,#0x10,00110$
      000263                        263 00110$:
      000263 40 DB            [24]  264 	jc	00102$
      000265 22               [24]  265 	ret
                                    266 ;------------------------------------------------------------
                                    267 ;Allocation info for local variables in function 'shift_rows'
                                    268 ;------------------------------------------------------------
                                    269 ;state                     Allocated to stack - _bp +1
                                    270 ;temp                      Allocated to stack - _bp +7
                                    271 ;sloc0                     Allocated to stack - _bp +4
                                    272 ;------------------------------------------------------------
                                    273 ;	aes_crypto.c:90: void shift_rows(uint8_t* state)
                                    274 ;	-----------------------------------------
                                    275 ;	 function shift_rows
                                    276 ;	-----------------------------------------
      000266                        277 _shift_rows:
      000266 C0 08            [24]  278 	push	_bp
      000268 85 81 08         [24]  279 	mov	_bp,sp
      00026B C0 82            [24]  280 	push	dpl
      00026D C0 83            [24]  281 	push	dph
      00026F C0 F0            [24]  282 	push	b
      000271 E5 81            [12]  283 	mov	a,sp
      000273 24 04            [12]  284 	add	a,#0x04
      000275 F5 81            [12]  285 	mov	sp,a
                                    286 ;	aes_crypto.c:93: temp = state[1];
      000277 A8 08            [24]  287 	mov	r0,_bp
      000279 08               [12]  288 	inc	r0
      00027A 74 01            [12]  289 	mov	a,#0x01
      00027C 26               [12]  290 	add	a,@r0
      00027D FA               [12]  291 	mov	r2,a
      00027E E4               [12]  292 	clr	a
      00027F 08               [12]  293 	inc	r0
      000280 36               [12]  294 	addc	a,@r0
      000281 FB               [12]  295 	mov	r3,a
      000282 08               [12]  296 	inc	r0
      000283 86 04            [24]  297 	mov	ar4,@r0
      000285 8A 82            [24]  298 	mov	dpl,r2
      000287 8B 83            [24]  299 	mov	dph,r3
      000289 8C F0            [24]  300 	mov	b,r4
      00028B E5 08            [12]  301 	mov	a,_bp
      00028D 24 07            [12]  302 	add	a,#0x07
      00028F F8               [12]  303 	mov	r0,a
      000290 12 1C A1         [24]  304 	lcall	__gptrget
      000293 F6               [12]  305 	mov	@r0,a
                                    306 ;	aes_crypto.c:94: state[1] = state[5];
      000294 A8 08            [24]  307 	mov	r0,_bp
      000296 08               [12]  308 	inc	r0
      000297 E5 08            [12]  309 	mov	a,_bp
      000299 24 04            [12]  310 	add	a,#0x04
      00029B F9               [12]  311 	mov	r1,a
      00029C 74 05            [12]  312 	mov	a,#0x05
      00029E 26               [12]  313 	add	a,@r0
      00029F F7               [12]  314 	mov	@r1,a
      0002A0 E4               [12]  315 	clr	a
      0002A1 08               [12]  316 	inc	r0
      0002A2 36               [12]  317 	addc	a,@r0
      0002A3 09               [12]  318 	inc	r1
      0002A4 F7               [12]  319 	mov	@r1,a
      0002A5 08               [12]  320 	inc	r0
      0002A6 09               [12]  321 	inc	r1
      0002A7 E6               [12]  322 	mov	a,@r0
      0002A8 F7               [12]  323 	mov	@r1,a
      0002A9 E5 08            [12]  324 	mov	a,_bp
      0002AB 24 04            [12]  325 	add	a,#0x04
      0002AD F8               [12]  326 	mov	r0,a
      0002AE 86 82            [24]  327 	mov	dpl,@r0
      0002B0 08               [12]  328 	inc	r0
      0002B1 86 83            [24]  329 	mov	dph,@r0
      0002B3 08               [12]  330 	inc	r0
      0002B4 86 F0            [24]  331 	mov	b,@r0
      0002B6 12 1C A1         [24]  332 	lcall	__gptrget
      0002B9 8A 82            [24]  333 	mov	dpl,r2
      0002BB 8B 83            [24]  334 	mov	dph,r3
      0002BD 8C F0            [24]  335 	mov	b,r4
      0002BF 12 1B 7D         [24]  336 	lcall	__gptrput
                                    337 ;	aes_crypto.c:95: state[5] = state[9];
      0002C2 A8 08            [24]  338 	mov	r0,_bp
      0002C4 08               [12]  339 	inc	r0
      0002C5 74 09            [12]  340 	mov	a,#0x09
      0002C7 26               [12]  341 	add	a,@r0
      0002C8 FD               [12]  342 	mov	r5,a
      0002C9 E4               [12]  343 	clr	a
      0002CA 08               [12]  344 	inc	r0
      0002CB 36               [12]  345 	addc	a,@r0
      0002CC FE               [12]  346 	mov	r6,a
      0002CD 08               [12]  347 	inc	r0
      0002CE 86 07            [24]  348 	mov	ar7,@r0
      0002D0 8D 82            [24]  349 	mov	dpl,r5
      0002D2 8E 83            [24]  350 	mov	dph,r6
      0002D4 8F F0            [24]  351 	mov	b,r7
      0002D6 12 1C A1         [24]  352 	lcall	__gptrget
      0002D9 FC               [12]  353 	mov	r4,a
      0002DA E5 08            [12]  354 	mov	a,_bp
      0002DC 24 04            [12]  355 	add	a,#0x04
      0002DE F8               [12]  356 	mov	r0,a
      0002DF 86 82            [24]  357 	mov	dpl,@r0
      0002E1 08               [12]  358 	inc	r0
      0002E2 86 83            [24]  359 	mov	dph,@r0
      0002E4 08               [12]  360 	inc	r0
      0002E5 86 F0            [24]  361 	mov	b,@r0
      0002E7 EC               [12]  362 	mov	a,r4
      0002E8 12 1B 7D         [24]  363 	lcall	__gptrput
                                    364 ;	aes_crypto.c:96: state[9] = state[13];
      0002EB A8 08            [24]  365 	mov	r0,_bp
      0002ED 08               [12]  366 	inc	r0
      0002EE E5 08            [12]  367 	mov	a,_bp
      0002F0 24 04            [12]  368 	add	a,#0x04
      0002F2 F9               [12]  369 	mov	r1,a
      0002F3 74 0D            [12]  370 	mov	a,#0x0d
      0002F5 26               [12]  371 	add	a,@r0
      0002F6 F7               [12]  372 	mov	@r1,a
      0002F7 E4               [12]  373 	clr	a
      0002F8 08               [12]  374 	inc	r0
      0002F9 36               [12]  375 	addc	a,@r0
      0002FA 09               [12]  376 	inc	r1
      0002FB F7               [12]  377 	mov	@r1,a
      0002FC 08               [12]  378 	inc	r0
      0002FD 09               [12]  379 	inc	r1
      0002FE E6               [12]  380 	mov	a,@r0
      0002FF F7               [12]  381 	mov	@r1,a
      000300 E5 08            [12]  382 	mov	a,_bp
      000302 24 04            [12]  383 	add	a,#0x04
      000304 F8               [12]  384 	mov	r0,a
      000305 86 82            [24]  385 	mov	dpl,@r0
      000307 08               [12]  386 	inc	r0
      000308 86 83            [24]  387 	mov	dph,@r0
      00030A 08               [12]  388 	inc	r0
      00030B 86 F0            [24]  389 	mov	b,@r0
      00030D 12 1C A1         [24]  390 	lcall	__gptrget
      000310 8D 82            [24]  391 	mov	dpl,r5
      000312 8E 83            [24]  392 	mov	dph,r6
      000314 8F F0            [24]  393 	mov	b,r7
      000316 12 1B 7D         [24]  394 	lcall	__gptrput
                                    395 ;	aes_crypto.c:97: state[13] = temp;
      000319 E5 08            [12]  396 	mov	a,_bp
      00031B 24 04            [12]  397 	add	a,#0x04
      00031D F8               [12]  398 	mov	r0,a
      00031E 86 82            [24]  399 	mov	dpl,@r0
      000320 08               [12]  400 	inc	r0
      000321 86 83            [24]  401 	mov	dph,@r0
      000323 08               [12]  402 	inc	r0
      000324 86 F0            [24]  403 	mov	b,@r0
      000326 E5 08            [12]  404 	mov	a,_bp
      000328 24 07            [12]  405 	add	a,#0x07
      00032A F9               [12]  406 	mov	r1,a
      00032B E7               [12]  407 	mov	a,@r1
      00032C 12 1B 7D         [24]  408 	lcall	__gptrput
                                    409 ;	aes_crypto.c:99: temp = state[2];
      00032F A8 08            [24]  410 	mov	r0,_bp
      000331 08               [12]  411 	inc	r0
      000332 74 02            [12]  412 	mov	a,#0x02
      000334 26               [12]  413 	add	a,@r0
      000335 FD               [12]  414 	mov	r5,a
      000336 E4               [12]  415 	clr	a
      000337 08               [12]  416 	inc	r0
      000338 36               [12]  417 	addc	a,@r0
      000339 FE               [12]  418 	mov	r6,a
      00033A 08               [12]  419 	inc	r0
      00033B 86 07            [24]  420 	mov	ar7,@r0
      00033D 8D 82            [24]  421 	mov	dpl,r5
      00033F 8E 83            [24]  422 	mov	dph,r6
      000341 8F F0            [24]  423 	mov	b,r7
      000343 E5 08            [12]  424 	mov	a,_bp
      000345 24 07            [12]  425 	add	a,#0x07
      000347 F8               [12]  426 	mov	r0,a
      000348 12 1C A1         [24]  427 	lcall	__gptrget
      00034B F6               [12]  428 	mov	@r0,a
                                    429 ;	aes_crypto.c:100: state[2] = state[10];
      00034C A8 08            [24]  430 	mov	r0,_bp
      00034E 08               [12]  431 	inc	r0
      00034F E5 08            [12]  432 	mov	a,_bp
      000351 24 04            [12]  433 	add	a,#0x04
      000353 F9               [12]  434 	mov	r1,a
      000354 74 0A            [12]  435 	mov	a,#0x0a
      000356 26               [12]  436 	add	a,@r0
      000357 F7               [12]  437 	mov	@r1,a
      000358 E4               [12]  438 	clr	a
      000359 08               [12]  439 	inc	r0
      00035A 36               [12]  440 	addc	a,@r0
      00035B 09               [12]  441 	inc	r1
      00035C F7               [12]  442 	mov	@r1,a
      00035D 08               [12]  443 	inc	r0
      00035E 09               [12]  444 	inc	r1
      00035F E6               [12]  445 	mov	a,@r0
      000360 F7               [12]  446 	mov	@r1,a
      000361 E5 08            [12]  447 	mov	a,_bp
      000363 24 04            [12]  448 	add	a,#0x04
      000365 F8               [12]  449 	mov	r0,a
      000366 86 82            [24]  450 	mov	dpl,@r0
      000368 08               [12]  451 	inc	r0
      000369 86 83            [24]  452 	mov	dph,@r0
      00036B 08               [12]  453 	inc	r0
      00036C 86 F0            [24]  454 	mov	b,@r0
      00036E 12 1C A1         [24]  455 	lcall	__gptrget
      000371 8D 82            [24]  456 	mov	dpl,r5
      000373 8E 83            [24]  457 	mov	dph,r6
      000375 8F F0            [24]  458 	mov	b,r7
      000377 12 1B 7D         [24]  459 	lcall	__gptrput
                                    460 ;	aes_crypto.c:101: state[10] = temp;
      00037A E5 08            [12]  461 	mov	a,_bp
      00037C 24 04            [12]  462 	add	a,#0x04
      00037E F8               [12]  463 	mov	r0,a
      00037F 86 82            [24]  464 	mov	dpl,@r0
      000381 08               [12]  465 	inc	r0
      000382 86 83            [24]  466 	mov	dph,@r0
      000384 08               [12]  467 	inc	r0
      000385 86 F0            [24]  468 	mov	b,@r0
      000387 E5 08            [12]  469 	mov	a,_bp
      000389 24 07            [12]  470 	add	a,#0x07
      00038B F9               [12]  471 	mov	r1,a
      00038C E7               [12]  472 	mov	a,@r1
      00038D 12 1B 7D         [24]  473 	lcall	__gptrput
                                    474 ;	aes_crypto.c:103: temp = state[6];
      000390 A8 08            [24]  475 	mov	r0,_bp
      000392 08               [12]  476 	inc	r0
      000393 74 06            [12]  477 	mov	a,#0x06
      000395 26               [12]  478 	add	a,@r0
      000396 FD               [12]  479 	mov	r5,a
      000397 E4               [12]  480 	clr	a
      000398 08               [12]  481 	inc	r0
      000399 36               [12]  482 	addc	a,@r0
      00039A FE               [12]  483 	mov	r6,a
      00039B 08               [12]  484 	inc	r0
      00039C 86 07            [24]  485 	mov	ar7,@r0
      00039E 8D 82            [24]  486 	mov	dpl,r5
      0003A0 8E 83            [24]  487 	mov	dph,r6
      0003A2 8F F0            [24]  488 	mov	b,r7
      0003A4 E5 08            [12]  489 	mov	a,_bp
      0003A6 24 07            [12]  490 	add	a,#0x07
      0003A8 F8               [12]  491 	mov	r0,a
      0003A9 12 1C A1         [24]  492 	lcall	__gptrget
      0003AC F6               [12]  493 	mov	@r0,a
                                    494 ;	aes_crypto.c:104: state[6] = state[14];
      0003AD A8 08            [24]  495 	mov	r0,_bp
      0003AF 08               [12]  496 	inc	r0
      0003B0 E5 08            [12]  497 	mov	a,_bp
      0003B2 24 04            [12]  498 	add	a,#0x04
      0003B4 F9               [12]  499 	mov	r1,a
      0003B5 74 0E            [12]  500 	mov	a,#0x0e
      0003B7 26               [12]  501 	add	a,@r0
      0003B8 F7               [12]  502 	mov	@r1,a
      0003B9 E4               [12]  503 	clr	a
      0003BA 08               [12]  504 	inc	r0
      0003BB 36               [12]  505 	addc	a,@r0
      0003BC 09               [12]  506 	inc	r1
      0003BD F7               [12]  507 	mov	@r1,a
      0003BE 08               [12]  508 	inc	r0
      0003BF 09               [12]  509 	inc	r1
      0003C0 E6               [12]  510 	mov	a,@r0
      0003C1 F7               [12]  511 	mov	@r1,a
      0003C2 E5 08            [12]  512 	mov	a,_bp
      0003C4 24 04            [12]  513 	add	a,#0x04
      0003C6 F8               [12]  514 	mov	r0,a
      0003C7 86 82            [24]  515 	mov	dpl,@r0
      0003C9 08               [12]  516 	inc	r0
      0003CA 86 83            [24]  517 	mov	dph,@r0
      0003CC 08               [12]  518 	inc	r0
      0003CD 86 F0            [24]  519 	mov	b,@r0
      0003CF 12 1C A1         [24]  520 	lcall	__gptrget
      0003D2 8D 82            [24]  521 	mov	dpl,r5
      0003D4 8E 83            [24]  522 	mov	dph,r6
      0003D6 8F F0            [24]  523 	mov	b,r7
      0003D8 12 1B 7D         [24]  524 	lcall	__gptrput
                                    525 ;	aes_crypto.c:105: state[14] = temp;
      0003DB E5 08            [12]  526 	mov	a,_bp
      0003DD 24 04            [12]  527 	add	a,#0x04
      0003DF F8               [12]  528 	mov	r0,a
      0003E0 86 82            [24]  529 	mov	dpl,@r0
      0003E2 08               [12]  530 	inc	r0
      0003E3 86 83            [24]  531 	mov	dph,@r0
      0003E5 08               [12]  532 	inc	r0
      0003E6 86 F0            [24]  533 	mov	b,@r0
      0003E8 E5 08            [12]  534 	mov	a,_bp
      0003EA 24 07            [12]  535 	add	a,#0x07
      0003EC F9               [12]  536 	mov	r1,a
      0003ED E7               [12]  537 	mov	a,@r1
      0003EE 12 1B 7D         [24]  538 	lcall	__gptrput
                                    539 ;	aes_crypto.c:107: temp = state[3];
      0003F1 A8 08            [24]  540 	mov	r0,_bp
      0003F3 08               [12]  541 	inc	r0
      0003F4 74 03            [12]  542 	mov	a,#0x03
      0003F6 26               [12]  543 	add	a,@r0
      0003F7 FD               [12]  544 	mov	r5,a
      0003F8 E4               [12]  545 	clr	a
      0003F9 08               [12]  546 	inc	r0
      0003FA 36               [12]  547 	addc	a,@r0
      0003FB FE               [12]  548 	mov	r6,a
      0003FC 08               [12]  549 	inc	r0
      0003FD 86 07            [24]  550 	mov	ar7,@r0
      0003FF 8D 82            [24]  551 	mov	dpl,r5
      000401 8E 83            [24]  552 	mov	dph,r6
      000403 8F F0            [24]  553 	mov	b,r7
      000405 E5 08            [12]  554 	mov	a,_bp
      000407 24 07            [12]  555 	add	a,#0x07
      000409 F8               [12]  556 	mov	r0,a
      00040A 12 1C A1         [24]  557 	lcall	__gptrget
      00040D F6               [12]  558 	mov	@r0,a
                                    559 ;	aes_crypto.c:108: state[3]  = state[15];
      00040E A8 08            [24]  560 	mov	r0,_bp
      000410 08               [12]  561 	inc	r0
      000411 E5 08            [12]  562 	mov	a,_bp
      000413 24 04            [12]  563 	add	a,#0x04
      000415 F9               [12]  564 	mov	r1,a
      000416 74 0F            [12]  565 	mov	a,#0x0f
      000418 26               [12]  566 	add	a,@r0
      000419 F7               [12]  567 	mov	@r1,a
      00041A E4               [12]  568 	clr	a
      00041B 08               [12]  569 	inc	r0
      00041C 36               [12]  570 	addc	a,@r0
      00041D 09               [12]  571 	inc	r1
      00041E F7               [12]  572 	mov	@r1,a
      00041F 08               [12]  573 	inc	r0
      000420 09               [12]  574 	inc	r1
      000421 E6               [12]  575 	mov	a,@r0
      000422 F7               [12]  576 	mov	@r1,a
      000423 E5 08            [12]  577 	mov	a,_bp
      000425 24 04            [12]  578 	add	a,#0x04
      000427 F8               [12]  579 	mov	r0,a
      000428 86 82            [24]  580 	mov	dpl,@r0
      00042A 08               [12]  581 	inc	r0
      00042B 86 83            [24]  582 	mov	dph,@r0
      00042D 08               [12]  583 	inc	r0
      00042E 86 F0            [24]  584 	mov	b,@r0
      000430 12 1C A1         [24]  585 	lcall	__gptrget
      000433 8D 82            [24]  586 	mov	dpl,r5
      000435 8E 83            [24]  587 	mov	dph,r6
      000437 8F F0            [24]  588 	mov	b,r7
      000439 12 1B 7D         [24]  589 	lcall	__gptrput
                                    590 ;	aes_crypto.c:109: state[15] = state[11];
      00043C A8 08            [24]  591 	mov	r0,_bp
      00043E 08               [12]  592 	inc	r0
      00043F 74 0B            [12]  593 	mov	a,#0x0b
      000441 26               [12]  594 	add	a,@r0
      000442 FD               [12]  595 	mov	r5,a
      000443 E4               [12]  596 	clr	a
      000444 08               [12]  597 	inc	r0
      000445 36               [12]  598 	addc	a,@r0
      000446 FE               [12]  599 	mov	r6,a
      000447 08               [12]  600 	inc	r0
      000448 86 07            [24]  601 	mov	ar7,@r0
      00044A 8D 82            [24]  602 	mov	dpl,r5
      00044C 8E 83            [24]  603 	mov	dph,r6
      00044E 8F F0            [24]  604 	mov	b,r7
      000450 12 1C A1         [24]  605 	lcall	__gptrget
      000453 FC               [12]  606 	mov	r4,a
      000454 E5 08            [12]  607 	mov	a,_bp
      000456 24 04            [12]  608 	add	a,#0x04
      000458 F8               [12]  609 	mov	r0,a
      000459 86 82            [24]  610 	mov	dpl,@r0
      00045B 08               [12]  611 	inc	r0
      00045C 86 83            [24]  612 	mov	dph,@r0
      00045E 08               [12]  613 	inc	r0
      00045F 86 F0            [24]  614 	mov	b,@r0
      000461 EC               [12]  615 	mov	a,r4
      000462 12 1B 7D         [24]  616 	lcall	__gptrput
                                    617 ;	aes_crypto.c:110: state[11] = state[7];
      000465 A8 08            [24]  618 	mov	r0,_bp
      000467 08               [12]  619 	inc	r0
      000468 E5 08            [12]  620 	mov	a,_bp
      00046A 24 04            [12]  621 	add	a,#0x04
      00046C F9               [12]  622 	mov	r1,a
      00046D 74 07            [12]  623 	mov	a,#0x07
      00046F 26               [12]  624 	add	a,@r0
      000470 F7               [12]  625 	mov	@r1,a
      000471 E4               [12]  626 	clr	a
      000472 08               [12]  627 	inc	r0
      000473 36               [12]  628 	addc	a,@r0
      000474 09               [12]  629 	inc	r1
      000475 F7               [12]  630 	mov	@r1,a
      000476 08               [12]  631 	inc	r0
      000477 09               [12]  632 	inc	r1
      000478 E6               [12]  633 	mov	a,@r0
      000479 F7               [12]  634 	mov	@r1,a
      00047A E5 08            [12]  635 	mov	a,_bp
      00047C 24 04            [12]  636 	add	a,#0x04
      00047E F8               [12]  637 	mov	r0,a
      00047F 86 82            [24]  638 	mov	dpl,@r0
      000481 08               [12]  639 	inc	r0
      000482 86 83            [24]  640 	mov	dph,@r0
      000484 08               [12]  641 	inc	r0
      000485 86 F0            [24]  642 	mov	b,@r0
      000487 12 1C A1         [24]  643 	lcall	__gptrget
      00048A 8D 82            [24]  644 	mov	dpl,r5
      00048C 8E 83            [24]  645 	mov	dph,r6
      00048E 8F F0            [24]  646 	mov	b,r7
      000490 12 1B 7D         [24]  647 	lcall	__gptrput
                                    648 ;	aes_crypto.c:111: state[7]  = temp;
      000493 E5 08            [12]  649 	mov	a,_bp
      000495 24 04            [12]  650 	add	a,#0x04
      000497 F8               [12]  651 	mov	r0,a
      000498 86 82            [24]  652 	mov	dpl,@r0
      00049A 08               [12]  653 	inc	r0
      00049B 86 83            [24]  654 	mov	dph,@r0
      00049D 08               [12]  655 	inc	r0
      00049E 86 F0            [24]  656 	mov	b,@r0
      0004A0 E5 08            [12]  657 	mov	a,_bp
      0004A2 24 07            [12]  658 	add	a,#0x07
      0004A4 F9               [12]  659 	mov	r1,a
      0004A5 E7               [12]  660 	mov	a,@r1
      0004A6 12 1B 7D         [24]  661 	lcall	__gptrput
      0004A9 85 08 81         [24]  662 	mov	sp,_bp
      0004AC D0 08            [24]  663 	pop	_bp
      0004AE 22               [24]  664 	ret
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
                                    681 ;	aes_crypto.c:114: void mix_columns(uint8_t* state)
                                    682 ;	-----------------------------------------
                                    683 ;	 function mix_columns
                                    684 ;	-----------------------------------------
      0004AF                        685 _mix_columns:
      0004AF C0 08            [24]  686 	push	_bp
      0004B1 85 81 08         [24]  687 	mov	_bp,sp
      0004B4 C0 82            [24]  688 	push	dpl
      0004B6 C0 83            [24]  689 	push	dph
      0004B8 C0 F0            [24]  690 	push	b
      0004BA E5 81            [12]  691 	mov	a,sp
      0004BC 24 11            [12]  692 	add	a,#0x11
      0004BE F5 81            [12]  693 	mov	sp,a
                                    694 ;	aes_crypto.c:117: for(col = 0; col < 4; col++)
      0004C0 E5 08            [12]  695 	mov	a,_bp
      0004C2 24 12            [12]  696 	add	a,#0x12
      0004C4 F8               [12]  697 	mov	r0,a
      0004C5 76 00            [12]  698 	mov	@r0,#0x00
      0004C7                        699 00102$:
                                    700 ;	aes_crypto.c:120: Tmp = state[4*col+0] ^ state[4*col+1] ^ state[4*col+2] ^ state[4*col+3];
      0004C7 E5 08            [12]  701 	mov	a,_bp
      0004C9 24 12            [12]  702 	add	a,#0x12
      0004CB F8               [12]  703 	mov	r0,a
      0004CC E5 08            [12]  704 	mov	a,_bp
      0004CE 24 09            [12]  705 	add	a,#0x09
      0004D0 F9               [12]  706 	mov	r1,a
      0004D1 E6               [12]  707 	mov	a,@r0
      0004D2 75 F0 04         [24]  708 	mov	b,#0x04
      0004D5 A4               [48]  709 	mul	ab
      0004D6 F7               [12]  710 	mov	@r1,a
      0004D7 09               [12]  711 	inc	r1
      0004D8 A7 F0            [24]  712 	mov	@r1,b
      0004DA A8 08            [24]  713 	mov	r0,_bp
      0004DC 08               [12]  714 	inc	r0
      0004DD E5 08            [12]  715 	mov	a,_bp
      0004DF 24 09            [12]  716 	add	a,#0x09
      0004E1 F9               [12]  717 	mov	r1,a
      0004E2 E7               [12]  718 	mov	a,@r1
      0004E3 26               [12]  719 	add	a,@r0
      0004E4 C0 E0            [24]  720 	push	acc
      0004E6 09               [12]  721 	inc	r1
      0004E7 E7               [12]  722 	mov	a,@r1
      0004E8 08               [12]  723 	inc	r0
      0004E9 36               [12]  724 	addc	a,@r0
      0004EA C0 E0            [24]  725 	push	acc
      0004EC 08               [12]  726 	inc	r0
      0004ED E6               [12]  727 	mov	a,@r0
      0004EE C0 E0            [24]  728 	push	acc
      0004F0 E5 08            [12]  729 	mov	a,_bp
      0004F2 24 07            [12]  730 	add	a,#0x07
      0004F4 F8               [12]  731 	mov	r0,a
      0004F5 D0 E0            [24]  732 	pop	acc
      0004F7 F6               [12]  733 	mov	@r0,a
      0004F8 18               [12]  734 	dec	r0
      0004F9 D0 E0            [24]  735 	pop	acc
      0004FB F6               [12]  736 	mov	@r0,a
      0004FC 18               [12]  737 	dec	r0
      0004FD D0 E0            [24]  738 	pop	acc
      0004FF F6               [12]  739 	mov	@r0,a
      000500 E5 08            [12]  740 	mov	a,_bp
      000502 24 05            [12]  741 	add	a,#0x05
      000504 F8               [12]  742 	mov	r0,a
      000505 86 82            [24]  743 	mov	dpl,@r0
      000507 08               [12]  744 	inc	r0
      000508 86 83            [24]  745 	mov	dph,@r0
      00050A 08               [12]  746 	inc	r0
      00050B 86 F0            [24]  747 	mov	b,@r0
      00050D E5 08            [12]  748 	mov	a,_bp
      00050F 24 04            [12]  749 	add	a,#0x04
      000511 F9               [12]  750 	mov	r1,a
      000512 12 1C A1         [24]  751 	lcall	__gptrget
      000515 F7               [12]  752 	mov	@r1,a
      000516 E5 08            [12]  753 	mov	a,_bp
      000518 24 09            [12]  754 	add	a,#0x09
      00051A F8               [12]  755 	mov	r0,a
      00051B 74 01            [12]  756 	mov	a,#0x01
      00051D 26               [12]  757 	add	a,@r0
      00051E FD               [12]  758 	mov	r5,a
      00051F E4               [12]  759 	clr	a
      000520 08               [12]  760 	inc	r0
      000521 36               [12]  761 	addc	a,@r0
      000522 FF               [12]  762 	mov	r7,a
      000523 A8 08            [24]  763 	mov	r0,_bp
      000525 08               [12]  764 	inc	r0
      000526 E5 08            [12]  765 	mov	a,_bp
      000528 24 0B            [12]  766 	add	a,#0x0b
      00052A F9               [12]  767 	mov	r1,a
      00052B ED               [12]  768 	mov	a,r5
      00052C 26               [12]  769 	add	a,@r0
      00052D F7               [12]  770 	mov	@r1,a
      00052E EF               [12]  771 	mov	a,r7
      00052F 08               [12]  772 	inc	r0
      000530 36               [12]  773 	addc	a,@r0
      000531 09               [12]  774 	inc	r1
      000532 F7               [12]  775 	mov	@r1,a
      000533 08               [12]  776 	inc	r0
      000534 09               [12]  777 	inc	r1
      000535 E6               [12]  778 	mov	a,@r0
      000536 F7               [12]  779 	mov	@r1,a
      000537 E5 08            [12]  780 	mov	a,_bp
      000539 24 0B            [12]  781 	add	a,#0x0b
      00053B F8               [12]  782 	mov	r0,a
      00053C 86 82            [24]  783 	mov	dpl,@r0
      00053E 08               [12]  784 	inc	r0
      00053F 86 83            [24]  785 	mov	dph,@r0
      000541 08               [12]  786 	inc	r0
      000542 86 F0            [24]  787 	mov	b,@r0
      000544 12 1C A1         [24]  788 	lcall	__gptrget
      000547 FC               [12]  789 	mov	r4,a
      000548 E5 08            [12]  790 	mov	a,_bp
      00054A 24 04            [12]  791 	add	a,#0x04
      00054C F8               [12]  792 	mov	r0,a
      00054D E5 08            [12]  793 	mov	a,_bp
      00054F 24 08            [12]  794 	add	a,#0x08
      000551 F9               [12]  795 	mov	r1,a
      000552 EC               [12]  796 	mov	a,r4
      000553 66               [12]  797 	xrl	a,@r0
      000554 F7               [12]  798 	mov	@r1,a
      000555 E5 08            [12]  799 	mov	a,_bp
      000557 24 09            [12]  800 	add	a,#0x09
      000559 F8               [12]  801 	mov	r0,a
      00055A 74 02            [12]  802 	mov	a,#0x02
      00055C 26               [12]  803 	add	a,@r0
      00055D FB               [12]  804 	mov	r3,a
      00055E E4               [12]  805 	clr	a
      00055F 08               [12]  806 	inc	r0
      000560 36               [12]  807 	addc	a,@r0
      000561 FC               [12]  808 	mov	r4,a
      000562 A8 08            [24]  809 	mov	r0,_bp
      000564 08               [12]  810 	inc	r0
      000565 EB               [12]  811 	mov	a,r3
      000566 26               [12]  812 	add	a,@r0
      000567 FB               [12]  813 	mov	r3,a
      000568 EC               [12]  814 	mov	a,r4
      000569 08               [12]  815 	inc	r0
      00056A 36               [12]  816 	addc	a,@r0
      00056B FC               [12]  817 	mov	r4,a
      00056C 08               [12]  818 	inc	r0
      00056D 86 02            [24]  819 	mov	ar2,@r0
      00056F 8B 82            [24]  820 	mov	dpl,r3
      000571 8C 83            [24]  821 	mov	dph,r4
      000573 8A F0            [24]  822 	mov	b,r2
      000575 12 1C A1         [24]  823 	lcall	__gptrget
      000578 FF               [12]  824 	mov	r7,a
      000579 E5 08            [12]  825 	mov	a,_bp
      00057B 24 08            [12]  826 	add	a,#0x08
      00057D F8               [12]  827 	mov	r0,a
      00057E E5 08            [12]  828 	mov	a,_bp
      000580 24 0E            [12]  829 	add	a,#0x0e
      000582 F9               [12]  830 	mov	r1,a
      000583 EF               [12]  831 	mov	a,r7
      000584 66               [12]  832 	xrl	a,@r0
      000585 F7               [12]  833 	mov	@r1,a
      000586 E5 08            [12]  834 	mov	a,_bp
      000588 24 09            [12]  835 	add	a,#0x09
      00058A F8               [12]  836 	mov	r0,a
      00058B 74 03            [12]  837 	mov	a,#0x03
      00058D 26               [12]  838 	add	a,@r0
      00058E FD               [12]  839 	mov	r5,a
      00058F E4               [12]  840 	clr	a
      000590 08               [12]  841 	inc	r0
      000591 36               [12]  842 	addc	a,@r0
      000592 FE               [12]  843 	mov	r6,a
      000593 A8 08            [24]  844 	mov	r0,_bp
      000595 08               [12]  845 	inc	r0
      000596 E5 08            [12]  846 	mov	a,_bp
      000598 24 0F            [12]  847 	add	a,#0x0f
      00059A F9               [12]  848 	mov	r1,a
      00059B ED               [12]  849 	mov	a,r5
      00059C 26               [12]  850 	add	a,@r0
      00059D F7               [12]  851 	mov	@r1,a
      00059E EE               [12]  852 	mov	a,r6
      00059F 08               [12]  853 	inc	r0
      0005A0 36               [12]  854 	addc	a,@r0
      0005A1 09               [12]  855 	inc	r1
      0005A2 F7               [12]  856 	mov	@r1,a
      0005A3 08               [12]  857 	inc	r0
      0005A4 09               [12]  858 	inc	r1
      0005A5 E6               [12]  859 	mov	a,@r0
      0005A6 F7               [12]  860 	mov	@r1,a
      0005A7 E5 08            [12]  861 	mov	a,_bp
      0005A9 24 0F            [12]  862 	add	a,#0x0f
      0005AB F8               [12]  863 	mov	r0,a
      0005AC 86 82            [24]  864 	mov	dpl,@r0
      0005AE 08               [12]  865 	inc	r0
      0005AF 86 83            [24]  866 	mov	dph,@r0
      0005B1 08               [12]  867 	inc	r0
      0005B2 86 F0            [24]  868 	mov	b,@r0
      0005B4 12 1C A1         [24]  869 	lcall	__gptrget
      0005B7 FF               [12]  870 	mov	r7,a
      0005B8 E5 08            [12]  871 	mov	a,_bp
      0005BA 24 0E            [12]  872 	add	a,#0x0e
      0005BC F8               [12]  873 	mov	r0,a
      0005BD E6               [12]  874 	mov	a,@r0
      0005BE 62 07            [12]  875 	xrl	ar7,a
      0005C0 E5 08            [12]  876 	mov	a,_bp
      0005C2 24 13            [12]  877 	add	a,#0x13
      0005C4 F8               [12]  878 	mov	r0,a
      0005C5 A6 07            [24]  879 	mov	@r0,ar7
                                    880 ;	aes_crypto.c:121: t  = state[4*col];
      0005C7 E5 08            [12]  881 	mov	a,_bp
      0005C9 24 04            [12]  882 	add	a,#0x04
      0005CB F8               [12]  883 	mov	r0,a
      0005CC 86 06            [24]  884 	mov	ar6,@r0
      0005CE E5 08            [12]  885 	mov	a,_bp
      0005D0 24 14            [12]  886 	add	a,#0x14
      0005D2 F8               [12]  887 	mov	r0,a
      0005D3 A6 06            [24]  888 	mov	@r0,ar6
                                    889 ;	aes_crypto.c:122: Tm = state[4*col+0] ^ state[4*col+1]; Tm = xtime(Tm); state[4*col+0] ^= (Tm ^ Tmp);
      0005D5 E5 08            [12]  890 	mov	a,_bp
      0005D7 24 08            [12]  891 	add	a,#0x08
      0005D9 F8               [12]  892 	mov	r0,a
      0005DA 86 05            [24]  893 	mov	ar5,@r0
      0005DC 8D 82            [24]  894 	mov	dpl,r5
      0005DE C0 04            [24]  895 	push	ar4
      0005E0 C0 03            [24]  896 	push	ar3
      0005E2 C0 02            [24]  897 	push	ar2
      0005E4 12 01 A5         [24]  898 	lcall	_xtime
      0005E7 AE 82            [24]  899 	mov	r6,dpl
      0005E9 D0 02            [24]  900 	pop	ar2
      0005EB D0 03            [24]  901 	pop	ar3
      0005ED D0 04            [24]  902 	pop	ar4
      0005EF 8E 05            [24]  903 	mov	ar5,r6
      0005F1 E5 08            [12]  904 	mov	a,_bp
      0005F3 24 05            [12]  905 	add	a,#0x05
      0005F5 F8               [12]  906 	mov	r0,a
      0005F6 86 82            [24]  907 	mov	dpl,@r0
      0005F8 08               [12]  908 	inc	r0
      0005F9 86 83            [24]  909 	mov	dph,@r0
      0005FB 08               [12]  910 	inc	r0
      0005FC 86 F0            [24]  911 	mov	b,@r0
      0005FE 12 1C A1         [24]  912 	lcall	__gptrget
      000601 FE               [12]  913 	mov	r6,a
      000602 E5 08            [12]  914 	mov	a,_bp
      000604 24 13            [12]  915 	add	a,#0x13
      000606 F8               [12]  916 	mov	r0,a
      000607 E6               [12]  917 	mov	a,@r0
      000608 6D               [12]  918 	xrl	a,r5
      000609 62 06            [12]  919 	xrl	ar6,a
      00060B E5 08            [12]  920 	mov	a,_bp
      00060D 24 05            [12]  921 	add	a,#0x05
      00060F F8               [12]  922 	mov	r0,a
      000610 86 82            [24]  923 	mov	dpl,@r0
      000612 08               [12]  924 	inc	r0
      000613 86 83            [24]  925 	mov	dph,@r0
      000615 08               [12]  926 	inc	r0
      000616 86 F0            [24]  927 	mov	b,@r0
      000618 EE               [12]  928 	mov	a,r6
      000619 12 1B 7D         [24]  929 	lcall	__gptrput
                                    930 ;	aes_crypto.c:123: Tm = state[4*col+1] ^ state[4*col+2]; Tm = xtime(Tm); state[4*col+1] ^= (Tm ^ Tmp);
      00061C E5 08            [12]  931 	mov	a,_bp
      00061E 24 0B            [12]  932 	add	a,#0x0b
      000620 F8               [12]  933 	mov	r0,a
      000621 86 82            [24]  934 	mov	dpl,@r0
      000623 08               [12]  935 	inc	r0
      000624 86 83            [24]  936 	mov	dph,@r0
      000626 08               [12]  937 	inc	r0
      000627 86 F0            [24]  938 	mov	b,@r0
      000629 12 1C A1         [24]  939 	lcall	__gptrget
      00062C FF               [12]  940 	mov	r7,a
      00062D 8B 82            [24]  941 	mov	dpl,r3
      00062F 8C 83            [24]  942 	mov	dph,r4
      000631 8A F0            [24]  943 	mov	b,r2
      000633 12 1C A1         [24]  944 	lcall	__gptrget
      000636 62 07            [12]  945 	xrl	ar7,a
      000638 8F 05            [24]  946 	mov	ar5,r7
      00063A 8D 82            [24]  947 	mov	dpl,r5
      00063C C0 04            [24]  948 	push	ar4
      00063E C0 03            [24]  949 	push	ar3
      000640 C0 02            [24]  950 	push	ar2
      000642 12 01 A5         [24]  951 	lcall	_xtime
      000645 AF 82            [24]  952 	mov	r7,dpl
      000647 D0 02            [24]  953 	pop	ar2
      000649 D0 03            [24]  954 	pop	ar3
      00064B D0 04            [24]  955 	pop	ar4
      00064D 8F 05            [24]  956 	mov	ar5,r7
      00064F E5 08            [12]  957 	mov	a,_bp
      000651 24 0B            [12]  958 	add	a,#0x0b
      000653 F8               [12]  959 	mov	r0,a
      000654 86 82            [24]  960 	mov	dpl,@r0
      000656 08               [12]  961 	inc	r0
      000657 86 83            [24]  962 	mov	dph,@r0
      000659 08               [12]  963 	inc	r0
      00065A 86 F0            [24]  964 	mov	b,@r0
      00065C 12 1C A1         [24]  965 	lcall	__gptrget
      00065F FF               [12]  966 	mov	r7,a
      000660 E5 08            [12]  967 	mov	a,_bp
      000662 24 13            [12]  968 	add	a,#0x13
      000664 F8               [12]  969 	mov	r0,a
      000665 E6               [12]  970 	mov	a,@r0
      000666 6D               [12]  971 	xrl	a,r5
      000667 62 07            [12]  972 	xrl	ar7,a
      000669 E5 08            [12]  973 	mov	a,_bp
      00066B 24 0B            [12]  974 	add	a,#0x0b
      00066D F8               [12]  975 	mov	r0,a
      00066E 86 82            [24]  976 	mov	dpl,@r0
      000670 08               [12]  977 	inc	r0
      000671 86 83            [24]  978 	mov	dph,@r0
      000673 08               [12]  979 	inc	r0
      000674 86 F0            [24]  980 	mov	b,@r0
      000676 EF               [12]  981 	mov	a,r7
      000677 12 1B 7D         [24]  982 	lcall	__gptrput
                                    983 ;	aes_crypto.c:124: Tm = state[4*col+2] ^ state[4*col+3]; Tm = xtime(Tm); state[4*col+2] ^= (Tm ^ Tmp);
      00067A 8B 82            [24]  984 	mov	dpl,r3
      00067C 8C 83            [24]  985 	mov	dph,r4
      00067E 8A F0            [24]  986 	mov	b,r2
      000680 12 1C A1         [24]  987 	lcall	__gptrget
      000683 FF               [12]  988 	mov	r7,a
      000684 E5 08            [12]  989 	mov	a,_bp
      000686 24 0F            [12]  990 	add	a,#0x0f
      000688 F8               [12]  991 	mov	r0,a
      000689 86 82            [24]  992 	mov	dpl,@r0
      00068B 08               [12]  993 	inc	r0
      00068C 86 83            [24]  994 	mov	dph,@r0
      00068E 08               [12]  995 	inc	r0
      00068F 86 F0            [24]  996 	mov	b,@r0
      000691 12 1C A1         [24]  997 	lcall	__gptrget
      000694 62 07            [12]  998 	xrl	ar7,a
      000696 8F 05            [24]  999 	mov	ar5,r7
      000698 8D 82            [24] 1000 	mov	dpl,r5
      00069A C0 04            [24] 1001 	push	ar4
      00069C C0 03            [24] 1002 	push	ar3
      00069E C0 02            [24] 1003 	push	ar2
      0006A0 12 01 A5         [24] 1004 	lcall	_xtime
      0006A3 AF 82            [24] 1005 	mov	r7,dpl
      0006A5 D0 02            [24] 1006 	pop	ar2
      0006A7 D0 03            [24] 1007 	pop	ar3
      0006A9 D0 04            [24] 1008 	pop	ar4
      0006AB 8F 05            [24] 1009 	mov	ar5,r7
      0006AD 8B 82            [24] 1010 	mov	dpl,r3
      0006AF 8C 83            [24] 1011 	mov	dph,r4
      0006B1 8A F0            [24] 1012 	mov	b,r2
      0006B3 12 1C A1         [24] 1013 	lcall	__gptrget
      0006B6 FF               [12] 1014 	mov	r7,a
      0006B7 E5 08            [12] 1015 	mov	a,_bp
      0006B9 24 13            [12] 1016 	add	a,#0x13
      0006BB F8               [12] 1017 	mov	r0,a
      0006BC E6               [12] 1018 	mov	a,@r0
      0006BD 6D               [12] 1019 	xrl	a,r5
      0006BE 62 07            [12] 1020 	xrl	ar7,a
      0006C0 8B 82            [24] 1021 	mov	dpl,r3
      0006C2 8C 83            [24] 1022 	mov	dph,r4
      0006C4 8A F0            [24] 1023 	mov	b,r2
      0006C6 EF               [12] 1024 	mov	a,r7
      0006C7 12 1B 7D         [24] 1025 	lcall	__gptrput
                                   1026 ;	aes_crypto.c:125: Tm = state[4*col+3] ^ t;              Tm = xtime(Tm); state[4*col+3] ^= (Tm ^ Tmp);
      0006CA E5 08            [12] 1027 	mov	a,_bp
      0006CC 24 0F            [12] 1028 	add	a,#0x0f
      0006CE F8               [12] 1029 	mov	r0,a
      0006CF 86 82            [24] 1030 	mov	dpl,@r0
      0006D1 08               [12] 1031 	inc	r0
      0006D2 86 83            [24] 1032 	mov	dph,@r0
      0006D4 08               [12] 1033 	inc	r0
      0006D5 86 F0            [24] 1034 	mov	b,@r0
      0006D7 12 1C A1         [24] 1035 	lcall	__gptrget
      0006DA FF               [12] 1036 	mov	r7,a
      0006DB E5 08            [12] 1037 	mov	a,_bp
      0006DD 24 14            [12] 1038 	add	a,#0x14
      0006DF F8               [12] 1039 	mov	r0,a
      0006E0 E6               [12] 1040 	mov	a,@r0
      0006E1 62 07            [12] 1041 	xrl	ar7,a
      0006E3 8F 05            [24] 1042 	mov	ar5,r7
      0006E5 8D 82            [24] 1043 	mov	dpl,r5
      0006E7 12 01 A5         [24] 1044 	lcall	_xtime
      0006EA AF 82            [24] 1045 	mov	r7,dpl
      0006EC 8F 05            [24] 1046 	mov	ar5,r7
      0006EE E5 08            [12] 1047 	mov	a,_bp
      0006F0 24 0F            [12] 1048 	add	a,#0x0f
      0006F2 F8               [12] 1049 	mov	r0,a
      0006F3 86 82            [24] 1050 	mov	dpl,@r0
      0006F5 08               [12] 1051 	inc	r0
      0006F6 86 83            [24] 1052 	mov	dph,@r0
      0006F8 08               [12] 1053 	inc	r0
      0006F9 86 F0            [24] 1054 	mov	b,@r0
      0006FB 12 1C A1         [24] 1055 	lcall	__gptrget
      0006FE FF               [12] 1056 	mov	r7,a
      0006FF E5 08            [12] 1057 	mov	a,_bp
      000701 24 13            [12] 1058 	add	a,#0x13
      000703 F8               [12] 1059 	mov	r0,a
      000704 E6               [12] 1060 	mov	a,@r0
      000705 62 05            [12] 1061 	xrl	ar5,a
      000707 ED               [12] 1062 	mov	a,r5
      000708 62 07            [12] 1063 	xrl	ar7,a
      00070A E5 08            [12] 1064 	mov	a,_bp
      00070C 24 0F            [12] 1065 	add	a,#0x0f
      00070E F8               [12] 1066 	mov	r0,a
      00070F 86 82            [24] 1067 	mov	dpl,@r0
      000711 08               [12] 1068 	inc	r0
      000712 86 83            [24] 1069 	mov	dph,@r0
      000714 08               [12] 1070 	inc	r0
      000715 86 F0            [24] 1071 	mov	b,@r0
      000717 EF               [12] 1072 	mov	a,r7
      000718 12 1B 7D         [24] 1073 	lcall	__gptrput
                                   1074 ;	aes_crypto.c:117: for(col = 0; col < 4; col++)
      00071B E5 08            [12] 1075 	mov	a,_bp
      00071D 24 12            [12] 1076 	add	a,#0x12
      00071F F8               [12] 1077 	mov	r0,a
      000720 06               [12] 1078 	inc	@r0
      000721 E5 08            [12] 1079 	mov	a,_bp
      000723 24 12            [12] 1080 	add	a,#0x12
      000725 F8               [12] 1081 	mov	r0,a
      000726 B6 04 00         [24] 1082 	cjne	@r0,#0x04,00110$
      000729                       1083 00110$:
      000729 50 03            [24] 1084 	jnc	00111$
      00072B 02 04 C7         [24] 1085 	ljmp	00102$
      00072E                       1086 00111$:
      00072E 85 08 81         [24] 1087 	mov	sp,_bp
      000731 D0 08            [24] 1088 	pop	_bp
      000733 22               [24] 1089 	ret
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
                                   1105 ;	aes_crypto.c:129: void update_key(uint8_t* w, uint8_t round)
                                   1106 ;	-----------------------------------------
                                   1107 ;	 function update_key
                                   1108 ;	-----------------------------------------
      000734                       1109 _update_key:
      000734 C0 08            [24] 1110 	push	_bp
      000736 85 81 08         [24] 1111 	mov	_bp,sp
      000739 C0 82            [24] 1112 	push	dpl
      00073B C0 83            [24] 1113 	push	dph
      00073D C0 F0            [24] 1114 	push	b
      00073F E5 81            [12] 1115 	mov	a,sp
      000741 24 13            [12] 1116 	add	a,#0x13
      000743 F5 81            [12] 1117 	mov	sp,a
                                   1118 ;	aes_crypto.c:134: for(i = 0; i < 4; i++)
      000745 A8 08            [24] 1119 	mov	r0,_bp
      000747 08               [12] 1120 	inc	r0
      000748 74 0D            [12] 1121 	mov	a,#0x0d
      00074A 26               [12] 1122 	add	a,@r0
      00074B FA               [12] 1123 	mov	r2,a
      00074C E4               [12] 1124 	clr	a
      00074D 08               [12] 1125 	inc	r0
      00074E 36               [12] 1126 	addc	a,@r0
      00074F FB               [12] 1127 	mov	r3,a
      000750 08               [12] 1128 	inc	r0
      000751 86 04            [24] 1129 	mov	ar4,@r0
      000753 E5 08            [12] 1130 	mov	a,_bp
      000755 24 FD            [12] 1131 	add	a,#0xfd
      000757 F8               [12] 1132 	mov	r0,a
      000758 E5 08            [12] 1133 	mov	a,_bp
      00075A 24 04            [12] 1134 	add	a,#0x04
      00075C F9               [12] 1135 	mov	r1,a
      00075D E6               [12] 1136 	mov	a,@r0
      00075E 24 C1            [12] 1137 	add	a,#_rcon
      000760 F7               [12] 1138 	mov	@r1,a
      000761 E4               [12] 1139 	clr	a
      000762 34 1D            [12] 1140 	addc	a,#(_rcon >> 8)
      000764 09               [12] 1141 	inc	r1
      000765 F7               [12] 1142 	mov	@r1,a
      000766 A8 08            [24] 1143 	mov	r0,_bp
      000768 08               [12] 1144 	inc	r0
      000769 E5 08            [12] 1145 	mov	a,_bp
      00076B 24 06            [12] 1146 	add	a,#0x06
      00076D F9               [12] 1147 	mov	r1,a
      00076E 74 0E            [12] 1148 	mov	a,#0x0e
      000770 26               [12] 1149 	add	a,@r0
      000771 F7               [12] 1150 	mov	@r1,a
      000772 E4               [12] 1151 	clr	a
      000773 08               [12] 1152 	inc	r0
      000774 36               [12] 1153 	addc	a,@r0
      000775 09               [12] 1154 	inc	r1
      000776 F7               [12] 1155 	mov	@r1,a
      000777 08               [12] 1156 	inc	r0
      000778 09               [12] 1157 	inc	r1
      000779 E6               [12] 1158 	mov	a,@r0
      00077A F7               [12] 1159 	mov	@r1,a
      00077B A8 08            [24] 1160 	mov	r0,_bp
      00077D 08               [12] 1161 	inc	r0
      00077E E5 08            [12] 1162 	mov	a,_bp
      000780 24 09            [12] 1163 	add	a,#0x09
      000782 F9               [12] 1164 	mov	r1,a
      000783 74 0F            [12] 1165 	mov	a,#0x0f
      000785 26               [12] 1166 	add	a,@r0
      000786 F7               [12] 1167 	mov	@r1,a
      000787 E4               [12] 1168 	clr	a
      000788 08               [12] 1169 	inc	r0
      000789 36               [12] 1170 	addc	a,@r0
      00078A 09               [12] 1171 	inc	r1
      00078B F7               [12] 1172 	mov	@r1,a
      00078C 08               [12] 1173 	inc	r0
      00078D 09               [12] 1174 	inc	r1
      00078E E6               [12] 1175 	mov	a,@r0
      00078F F7               [12] 1176 	mov	@r1,a
      000790 A8 08            [24] 1177 	mov	r0,_bp
      000792 08               [12] 1178 	inc	r0
      000793 E5 08            [12] 1179 	mov	a,_bp
      000795 24 0C            [12] 1180 	add	a,#0x0c
      000797 F9               [12] 1181 	mov	r1,a
      000798 74 0C            [12] 1182 	mov	a,#0x0c
      00079A 26               [12] 1183 	add	a,@r0
      00079B F7               [12] 1184 	mov	@r1,a
      00079C E4               [12] 1185 	clr	a
      00079D 08               [12] 1186 	inc	r0
      00079E 36               [12] 1187 	addc	a,@r0
      00079F 09               [12] 1188 	inc	r1
      0007A0 F7               [12] 1189 	mov	@r1,a
      0007A1 08               [12] 1190 	inc	r0
      0007A2 09               [12] 1191 	inc	r1
      0007A3 E6               [12] 1192 	mov	a,@r0
      0007A4 F7               [12] 1193 	mov	@r1,a
      0007A5 E5 08            [12] 1194 	mov	a,_bp
      0007A7 24 12            [12] 1195 	add	a,#0x12
      0007A9 F9               [12] 1196 	mov	r1,a
      0007AA 04               [12] 1197 	inc	a
      0007AB F8               [12] 1198 	mov	r0,a
      0007AC C0 00            [24] 1199 	push	ar0
      0007AE E5 08            [12] 1200 	mov	a,_bp
      0007B0 24 0F            [12] 1201 	add	a,#0x0f
      0007B2 F8               [12] 1202 	mov	r0,a
      0007B3 74 02            [12] 1203 	mov	a,#0x02
      0007B5 29               [12] 1204 	add	a,r1
      0007B6 F6               [12] 1205 	mov	@r0,a
      0007B7 E5 08            [12] 1206 	mov	a,_bp
      0007B9 24 10            [12] 1207 	add	a,#0x10
      0007BB F8               [12] 1208 	mov	r0,a
      0007BC 74 03            [12] 1209 	mov	a,#0x03
      0007BE 29               [12] 1210 	add	a,r1
      0007BF F6               [12] 1211 	mov	@r0,a
      0007C0 E5 08            [12] 1212 	mov	a,_bp
      0007C2 24 16            [12] 1213 	add	a,#0x16
      0007C4 F8               [12] 1214 	mov	r0,a
      0007C5 76 00            [12] 1215 	mov	@r0,#0x00
      0007C7 D0 00            [24] 1216 	pop	ar0
      0007C9                       1217 00105$:
                                   1218 ;	aes_crypto.c:136: if(i == 0)
      0007C9 C0 00            [24] 1219 	push	ar0
      0007CB E5 08            [12] 1220 	mov	a,_bp
      0007CD 24 16            [12] 1221 	add	a,#0x16
      0007CF F8               [12] 1222 	mov	r0,a
      0007D0 E6               [12] 1223 	mov	a,@r0
      0007D1 D0 00            [24] 1224 	pop	ar0
      0007D3 60 03            [24] 1225 	jz	00116$
      0007D5 02 08 53         [24] 1226 	ljmp	00102$
      0007D8                       1227 00116$:
                                   1228 ;	aes_crypto.c:138: temp[0] = s_box[w[13]] ^ rcon[round];
      0007D8 8A 82            [24] 1229 	mov	dpl,r2
      0007DA 8B 83            [24] 1230 	mov	dph,r3
      0007DC 8C F0            [24] 1231 	mov	b,r4
      0007DE 12 1C A1         [24] 1232 	lcall	__gptrget
      0007E1 90 1C C1         [24] 1233 	mov	dptr,#_s_box
      0007E4 93               [24] 1234 	movc	a,@a+dptr
      0007E5 FE               [12] 1235 	mov	r6,a
      0007E6 C0 00            [24] 1236 	push	ar0
      0007E8 E5 08            [12] 1237 	mov	a,_bp
      0007EA 24 04            [12] 1238 	add	a,#0x04
      0007EC F8               [12] 1239 	mov	r0,a
      0007ED 86 82            [24] 1240 	mov	dpl,@r0
      0007EF 08               [12] 1241 	inc	r0
      0007F0 86 83            [24] 1242 	mov	dph,@r0
      0007F2 E4               [12] 1243 	clr	a
      0007F3 93               [24] 1244 	movc	a,@a+dptr
      0007F4 FD               [12] 1245 	mov	r5,a
      0007F5 6E               [12] 1246 	xrl	a,r6
      0007F6 F7               [12] 1247 	mov	@r1,a
                                   1248 ;	aes_crypto.c:139: temp[1] = s_box[w[14]];
      0007F7 E5 08            [12] 1249 	mov	a,_bp
      0007F9 24 06            [12] 1250 	add	a,#0x06
      0007FB F8               [12] 1251 	mov	r0,a
      0007FC 86 82            [24] 1252 	mov	dpl,@r0
      0007FE 08               [12] 1253 	inc	r0
      0007FF 86 83            [24] 1254 	mov	dph,@r0
      000801 08               [12] 1255 	inc	r0
      000802 86 F0            [24] 1256 	mov	b,@r0
      000804 12 1C A1         [24] 1257 	lcall	__gptrget
      000807 FE               [12] 1258 	mov	r6,a
      000808 D0 00            [24] 1259 	pop	ar0
      00080A EE               [12] 1260 	mov	a,r6
      00080B 90 1C C1         [24] 1261 	mov	dptr,#_s_box
      00080E 93               [24] 1262 	movc	a,@a+dptr
      00080F F6               [12] 1263 	mov	@r0,a
                                   1264 ;	aes_crypto.c:140: temp[2] = s_box[w[15]];
      000810 C0 00            [24] 1265 	push	ar0
      000812 E5 08            [12] 1266 	mov	a,_bp
      000814 24 09            [12] 1267 	add	a,#0x09
      000816 F8               [12] 1268 	mov	r0,a
      000817 86 82            [24] 1269 	mov	dpl,@r0
      000819 08               [12] 1270 	inc	r0
      00081A 86 83            [24] 1271 	mov	dph,@r0
      00081C 08               [12] 1272 	inc	r0
      00081D 86 F0            [24] 1273 	mov	b,@r0
      00081F 12 1C A1         [24] 1274 	lcall	__gptrget
      000822 90 1C C1         [24] 1275 	mov	dptr,#_s_box
      000825 93               [24] 1276 	movc	a,@a+dptr
      000826 FE               [12] 1277 	mov	r6,a
      000827 E5 08            [12] 1278 	mov	a,_bp
      000829 24 0F            [12] 1279 	add	a,#0x0f
      00082B F8               [12] 1280 	mov	r0,a
      00082C 86 00            [24] 1281 	mov	ar0,@r0
      00082E A6 06            [24] 1282 	mov	@r0,ar6
                                   1283 ;	aes_crypto.c:141: temp[3] = s_box[w[12]];
      000830 E5 08            [12] 1284 	mov	a,_bp
      000832 24 0C            [12] 1285 	add	a,#0x0c
      000834 F8               [12] 1286 	mov	r0,a
      000835 86 82            [24] 1287 	mov	dpl,@r0
      000837 08               [12] 1288 	inc	r0
      000838 86 83            [24] 1289 	mov	dph,@r0
      00083A 08               [12] 1290 	inc	r0
      00083B 86 F0            [24] 1291 	mov	b,@r0
      00083D 12 1C A1         [24] 1292 	lcall	__gptrget
      000840 90 1C C1         [24] 1293 	mov	dptr,#_s_box
      000843 93               [24] 1294 	movc	a,@a+dptr
      000844 FE               [12] 1295 	mov	r6,a
      000845 E5 08            [12] 1296 	mov	a,_bp
      000847 24 10            [12] 1297 	add	a,#0x10
      000849 F8               [12] 1298 	mov	r0,a
      00084A 86 00            [24] 1299 	mov	ar0,@r0
      00084C A6 06            [24] 1300 	mov	@r0,ar6
      00084E D0 00            [24] 1301 	pop	ar0
      000850 02 09 08         [24] 1302 	ljmp	00103$
      000853                       1303 00102$:
                                   1304 ;	aes_crypto.c:145: temp[0] = w[4*(i-1) + 0];
      000853 C0 02            [24] 1305 	push	ar2
      000855 C0 03            [24] 1306 	push	ar3
      000857 C0 04            [24] 1307 	push	ar4
      000859 C0 00            [24] 1308 	push	ar0
      00085B E5 08            [12] 1309 	mov	a,_bp
      00085D 24 16            [12] 1310 	add	a,#0x16
      00085F F8               [12] 1311 	mov	r0,a
      000860 86 05            [24] 1312 	mov	ar5,@r0
      000862 7E 00            [12] 1313 	mov	r6,#0x00
      000864 D0 00            [24] 1314 	pop	ar0
      000866 1D               [12] 1315 	dec	r5
      000867 BD FF 01         [24] 1316 	cjne	r5,#0xff,00117$
      00086A 1E               [12] 1317 	dec	r6
      00086B                       1318 00117$:
      00086B EE               [12] 1319 	mov	a,r6
      00086C CD               [12] 1320 	xch	a,r5
      00086D 25 E0            [12] 1321 	add	a,acc
      00086F CD               [12] 1322 	xch	a,r5
      000870 33               [12] 1323 	rlc	a
      000871 CD               [12] 1324 	xch	a,r5
      000872 25 E0            [12] 1325 	add	a,acc
      000874 CD               [12] 1326 	xch	a,r5
      000875 33               [12] 1327 	rlc	a
      000876 FE               [12] 1328 	mov	r6,a
      000877 C0 00            [24] 1329 	push	ar0
      000879 A8 08            [24] 1330 	mov	r0,_bp
      00087B 08               [12] 1331 	inc	r0
      00087C ED               [12] 1332 	mov	a,r5
      00087D 26               [12] 1333 	add	a,@r0
      00087E FA               [12] 1334 	mov	r2,a
      00087F EE               [12] 1335 	mov	a,r6
      000880 08               [12] 1336 	inc	r0
      000881 36               [12] 1337 	addc	a,@r0
      000882 FB               [12] 1338 	mov	r3,a
      000883 08               [12] 1339 	inc	r0
      000884 86 04            [24] 1340 	mov	ar4,@r0
      000886 8A 82            [24] 1341 	mov	dpl,r2
      000888 8B 83            [24] 1342 	mov	dph,r3
      00088A 8C F0            [24] 1343 	mov	b,r4
      00088C 12 1C A1         [24] 1344 	lcall	__gptrget
      00088F F7               [12] 1345 	mov	@r1,a
                                   1346 ;	aes_crypto.c:146: temp[1] = w[4*(i-1) + 1];
      000890 74 01            [12] 1347 	mov	a,#0x01
      000892 2D               [12] 1348 	add	a,r5
      000893 FB               [12] 1349 	mov	r3,a
      000894 E4               [12] 1350 	clr	a
      000895 3E               [12] 1351 	addc	a,r6
      000896 FC               [12] 1352 	mov	r4,a
      000897 A8 08            [24] 1353 	mov	r0,_bp
      000899 08               [12] 1354 	inc	r0
      00089A EB               [12] 1355 	mov	a,r3
      00089B 26               [12] 1356 	add	a,@r0
      00089C FB               [12] 1357 	mov	r3,a
      00089D EC               [12] 1358 	mov	a,r4
      00089E 08               [12] 1359 	inc	r0
      00089F 36               [12] 1360 	addc	a,@r0
      0008A0 FC               [12] 1361 	mov	r4,a
      0008A1 08               [12] 1362 	inc	r0
      0008A2 86 02            [24] 1363 	mov	ar2,@r0
      0008A4 D0 00            [24] 1364 	pop	ar0
      0008A6 8B 82            [24] 1365 	mov	dpl,r3
      0008A8 8C 83            [24] 1366 	mov	dph,r4
      0008AA 8A F0            [24] 1367 	mov	b,r2
      0008AC 12 1C A1         [24] 1368 	lcall	__gptrget
      0008AF F6               [12] 1369 	mov	@r0,a
                                   1370 ;	aes_crypto.c:147: temp[2] = w[4*(i-1) + 2];
      0008B0 74 02            [12] 1371 	mov	a,#0x02
      0008B2 2D               [12] 1372 	add	a,r5
      0008B3 FB               [12] 1373 	mov	r3,a
      0008B4 E4               [12] 1374 	clr	a
      0008B5 3E               [12] 1375 	addc	a,r6
      0008B6 FC               [12] 1376 	mov	r4,a
      0008B7 C0 00            [24] 1377 	push	ar0
      0008B9 A8 08            [24] 1378 	mov	r0,_bp
      0008BB 08               [12] 1379 	inc	r0
      0008BC EB               [12] 1380 	mov	a,r3
      0008BD 26               [12] 1381 	add	a,@r0
      0008BE FB               [12] 1382 	mov	r3,a
      0008BF EC               [12] 1383 	mov	a,r4
      0008C0 08               [12] 1384 	inc	r0
      0008C1 36               [12] 1385 	addc	a,@r0
      0008C2 FC               [12] 1386 	mov	r4,a
      0008C3 08               [12] 1387 	inc	r0
      0008C4 86 02            [24] 1388 	mov	ar2,@r0
      0008C6 8B 82            [24] 1389 	mov	dpl,r3
      0008C8 8C 83            [24] 1390 	mov	dph,r4
      0008CA 8A F0            [24] 1391 	mov	b,r2
      0008CC 12 1C A1         [24] 1392 	lcall	__gptrget
      0008CF FB               [12] 1393 	mov	r3,a
      0008D0 E5 08            [12] 1394 	mov	a,_bp
      0008D2 24 0F            [12] 1395 	add	a,#0x0f
      0008D4 F8               [12] 1396 	mov	r0,a
      0008D5 86 00            [24] 1397 	mov	ar0,@r0
      0008D7 A6 03            [24] 1398 	mov	@r0,ar3
                                   1399 ;	aes_crypto.c:148: temp[3] = w[4*(i-1) + 3];
      0008D9 74 03            [12] 1400 	mov	a,#0x03
      0008DB 2D               [12] 1401 	add	a,r5
      0008DC FD               [12] 1402 	mov	r5,a
      0008DD E4               [12] 1403 	clr	a
      0008DE 3E               [12] 1404 	addc	a,r6
      0008DF FE               [12] 1405 	mov	r6,a
      0008E0 A8 08            [24] 1406 	mov	r0,_bp
      0008E2 08               [12] 1407 	inc	r0
      0008E3 ED               [12] 1408 	mov	a,r5
      0008E4 26               [12] 1409 	add	a,@r0
      0008E5 FD               [12] 1410 	mov	r5,a
      0008E6 EE               [12] 1411 	mov	a,r6
      0008E7 08               [12] 1412 	inc	r0
      0008E8 36               [12] 1413 	addc	a,@r0
      0008E9 FE               [12] 1414 	mov	r6,a
      0008EA 08               [12] 1415 	inc	r0
      0008EB 86 04            [24] 1416 	mov	ar4,@r0
      0008ED 8D 82            [24] 1417 	mov	dpl,r5
      0008EF 8E 83            [24] 1418 	mov	dph,r6
      0008F1 8C F0            [24] 1419 	mov	b,r4
      0008F3 12 1C A1         [24] 1420 	lcall	__gptrget
      0008F6 FD               [12] 1421 	mov	r5,a
      0008F7 E5 08            [12] 1422 	mov	a,_bp
      0008F9 24 10            [12] 1423 	add	a,#0x10
      0008FB F8               [12] 1424 	mov	r0,a
      0008FC 86 00            [24] 1425 	mov	ar0,@r0
      0008FE A6 05            [24] 1426 	mov	@r0,ar5
      000900 D0 00            [24] 1427 	pop	ar0
                                   1428 ;	aes_crypto.c:134: for(i = 0; i < 4; i++)
      000902 D0 04            [24] 1429 	pop	ar4
      000904 D0 03            [24] 1430 	pop	ar3
      000906 D0 02            [24] 1431 	pop	ar2
                                   1432 ;	aes_crypto.c:148: temp[3] = w[4*(i-1) + 3];
      000908                       1433 00103$:
                                   1434 ;	aes_crypto.c:151: w[4*i+0] ^= temp[0];
      000908 C0 02            [24] 1435 	push	ar2
      00090A C0 03            [24] 1436 	push	ar3
      00090C C0 04            [24] 1437 	push	ar4
      00090E C0 00            [24] 1438 	push	ar0
      000910 E5 08            [12] 1439 	mov	a,_bp
      000912 24 16            [12] 1440 	add	a,#0x16
      000914 F8               [12] 1441 	mov	r0,a
      000915 E6               [12] 1442 	mov	a,@r0
      000916 75 F0 04         [24] 1443 	mov	b,#0x04
      000919 A4               [48] 1444 	mul	ab
      00091A FD               [12] 1445 	mov	r5,a
      00091B AE F0            [24] 1446 	mov	r6,b
      00091D A8 08            [24] 1447 	mov	r0,_bp
      00091F 08               [12] 1448 	inc	r0
      000920 ED               [12] 1449 	mov	a,r5
      000921 26               [12] 1450 	add	a,@r0
      000922 FA               [12] 1451 	mov	r2,a
      000923 EE               [12] 1452 	mov	a,r6
      000924 08               [12] 1453 	inc	r0
      000925 36               [12] 1454 	addc	a,@r0
      000926 FB               [12] 1455 	mov	r3,a
      000927 08               [12] 1456 	inc	r0
      000928 86 04            [24] 1457 	mov	ar4,@r0
      00092A 8A 82            [24] 1458 	mov	dpl,r2
      00092C 8B 83            [24] 1459 	mov	dph,r3
      00092E 8C F0            [24] 1460 	mov	b,r4
      000930 E5 08            [12] 1461 	mov	a,_bp
      000932 24 11            [12] 1462 	add	a,#0x11
      000934 F8               [12] 1463 	mov	r0,a
      000935 12 1C A1         [24] 1464 	lcall	__gptrget
      000938 F6               [12] 1465 	mov	@r0,a
      000939 87 07            [24] 1466 	mov	ar7,@r1
      00093B E5 08            [12] 1467 	mov	a,_bp
      00093D 24 11            [12] 1468 	add	a,#0x11
      00093F F8               [12] 1469 	mov	r0,a
      000940 E6               [12] 1470 	mov	a,@r0
      000941 62 07            [12] 1471 	xrl	ar7,a
      000943 8A 82            [24] 1472 	mov	dpl,r2
      000945 8B 83            [24] 1473 	mov	dph,r3
      000947 8C F0            [24] 1474 	mov	b,r4
      000949 EF               [12] 1475 	mov	a,r7
      00094A 12 1B 7D         [24] 1476 	lcall	__gptrput
                                   1477 ;	aes_crypto.c:152: w[4*i+1] ^= temp[1];
      00094D 74 01            [12] 1478 	mov	a,#0x01
      00094F 2D               [12] 1479 	add	a,r5
      000950 FC               [12] 1480 	mov	r4,a
      000951 E4               [12] 1481 	clr	a
      000952 3E               [12] 1482 	addc	a,r6
      000953 FF               [12] 1483 	mov	r7,a
      000954 A8 08            [24] 1484 	mov	r0,_bp
      000956 08               [12] 1485 	inc	r0
      000957 EC               [12] 1486 	mov	a,r4
      000958 26               [12] 1487 	add	a,@r0
      000959 FC               [12] 1488 	mov	r4,a
      00095A EF               [12] 1489 	mov	a,r7
      00095B 08               [12] 1490 	inc	r0
      00095C 36               [12] 1491 	addc	a,@r0
      00095D FF               [12] 1492 	mov	r7,a
      00095E 08               [12] 1493 	inc	r0
      00095F 86 03            [24] 1494 	mov	ar3,@r0
      000961 8C 82            [24] 1495 	mov	dpl,r4
      000963 8F 83            [24] 1496 	mov	dph,r7
      000965 8B F0            [24] 1497 	mov	b,r3
      000967 E5 08            [12] 1498 	mov	a,_bp
      000969 24 11            [12] 1499 	add	a,#0x11
      00096B F8               [12] 1500 	mov	r0,a
      00096C 12 1C A1         [24] 1501 	lcall	__gptrget
      00096F F6               [12] 1502 	mov	@r0,a
      000970 D0 00            [24] 1503 	pop	ar0
      000972 86 02            [24] 1504 	mov	ar2,@r0
      000974 C0 00            [24] 1505 	push	ar0
      000976 E5 08            [12] 1506 	mov	a,_bp
      000978 24 11            [12] 1507 	add	a,#0x11
      00097A F8               [12] 1508 	mov	r0,a
      00097B E6               [12] 1509 	mov	a,@r0
      00097C 62 02            [12] 1510 	xrl	ar2,a
      00097E 8C 82            [24] 1511 	mov	dpl,r4
      000980 8F 83            [24] 1512 	mov	dph,r7
      000982 8B F0            [24] 1513 	mov	b,r3
      000984 EA               [12] 1514 	mov	a,r2
      000985 12 1B 7D         [24] 1515 	lcall	__gptrput
                                   1516 ;	aes_crypto.c:153: w[4*i+2] ^= temp[2];
      000988 74 02            [12] 1517 	mov	a,#0x02
      00098A 2D               [12] 1518 	add	a,r5
      00098B FC               [12] 1519 	mov	r4,a
      00098C E4               [12] 1520 	clr	a
      00098D 3E               [12] 1521 	addc	a,r6
      00098E FF               [12] 1522 	mov	r7,a
      00098F A8 08            [24] 1523 	mov	r0,_bp
      000991 08               [12] 1524 	inc	r0
      000992 EC               [12] 1525 	mov	a,r4
      000993 26               [12] 1526 	add	a,@r0
      000994 FC               [12] 1527 	mov	r4,a
      000995 EF               [12] 1528 	mov	a,r7
      000996 08               [12] 1529 	inc	r0
      000997 36               [12] 1530 	addc	a,@r0
      000998 FF               [12] 1531 	mov	r7,a
      000999 08               [12] 1532 	inc	r0
      00099A 86 03            [24] 1533 	mov	ar3,@r0
      00099C 8C 82            [24] 1534 	mov	dpl,r4
      00099E 8F 83            [24] 1535 	mov	dph,r7
      0009A0 8B F0            [24] 1536 	mov	b,r3
      0009A2 E5 08            [12] 1537 	mov	a,_bp
      0009A4 24 11            [12] 1538 	add	a,#0x11
      0009A6 F8               [12] 1539 	mov	r0,a
      0009A7 12 1C A1         [24] 1540 	lcall	__gptrget
      0009AA F6               [12] 1541 	mov	@r0,a
      0009AB E5 08            [12] 1542 	mov	a,_bp
      0009AD 24 0F            [12] 1543 	add	a,#0x0f
      0009AF F8               [12] 1544 	mov	r0,a
      0009B0 86 00            [24] 1545 	mov	ar0,@r0
      0009B2 86 02            [24] 1546 	mov	ar2,@r0
      0009B4 E5 08            [12] 1547 	mov	a,_bp
      0009B6 24 11            [12] 1548 	add	a,#0x11
      0009B8 F8               [12] 1549 	mov	r0,a
      0009B9 E6               [12] 1550 	mov	a,@r0
      0009BA 62 02            [12] 1551 	xrl	ar2,a
      0009BC 8C 82            [24] 1552 	mov	dpl,r4
      0009BE 8F 83            [24] 1553 	mov	dph,r7
      0009C0 8B F0            [24] 1554 	mov	b,r3
      0009C2 EA               [12] 1555 	mov	a,r2
      0009C3 12 1B 7D         [24] 1556 	lcall	__gptrput
                                   1557 ;	aes_crypto.c:154: w[4*i+3] ^= temp[3];
      0009C6 74 03            [12] 1558 	mov	a,#0x03
      0009C8 2D               [12] 1559 	add	a,r5
      0009C9 FD               [12] 1560 	mov	r5,a
      0009CA E4               [12] 1561 	clr	a
      0009CB 3E               [12] 1562 	addc	a,r6
      0009CC FE               [12] 1563 	mov	r6,a
      0009CD A8 08            [24] 1564 	mov	r0,_bp
      0009CF 08               [12] 1565 	inc	r0
      0009D0 ED               [12] 1566 	mov	a,r5
      0009D1 26               [12] 1567 	add	a,@r0
      0009D2 FD               [12] 1568 	mov	r5,a
      0009D3 EE               [12] 1569 	mov	a,r6
      0009D4 08               [12] 1570 	inc	r0
      0009D5 36               [12] 1571 	addc	a,@r0
      0009D6 FE               [12] 1572 	mov	r6,a
      0009D7 08               [12] 1573 	inc	r0
      0009D8 86 07            [24] 1574 	mov	ar7,@r0
      0009DA 8D 82            [24] 1575 	mov	dpl,r5
      0009DC 8E 83            [24] 1576 	mov	dph,r6
      0009DE 8F F0            [24] 1577 	mov	b,r7
      0009E0 12 1C A1         [24] 1578 	lcall	__gptrget
      0009E3 FC               [12] 1579 	mov	r4,a
      0009E4 E5 08            [12] 1580 	mov	a,_bp
      0009E6 24 10            [12] 1581 	add	a,#0x10
      0009E8 F8               [12] 1582 	mov	r0,a
      0009E9 86 00            [24] 1583 	mov	ar0,@r0
      0009EB E6               [12] 1584 	mov	a,@r0
      0009EC FB               [12] 1585 	mov	r3,a
      0009ED 62 04            [12] 1586 	xrl	ar4,a
      0009EF 8D 82            [24] 1587 	mov	dpl,r5
      0009F1 8E 83            [24] 1588 	mov	dph,r6
      0009F3 8F F0            [24] 1589 	mov	b,r7
      0009F5 EC               [12] 1590 	mov	a,r4
      0009F6 12 1B 7D         [24] 1591 	lcall	__gptrput
                                   1592 ;	aes_crypto.c:134: for(i = 0; i < 4; i++)
      0009F9 E5 08            [12] 1593 	mov	a,_bp
      0009FB 24 16            [12] 1594 	add	a,#0x16
      0009FD F8               [12] 1595 	mov	r0,a
      0009FE 06               [12] 1596 	inc	@r0
      0009FF E5 08            [12] 1597 	mov	a,_bp
      000A01 24 16            [12] 1598 	add	a,#0x16
      000A03 F8               [12] 1599 	mov	r0,a
      000A04 B6 04 00         [24] 1600 	cjne	@r0,#0x04,00118$
      000A07                       1601 00118$:
      000A07 D0 00            [24] 1602 	pop	ar0
      000A09 D0 04            [24] 1603 	pop	ar4
      000A0B D0 03            [24] 1604 	pop	ar3
      000A0D D0 02            [24] 1605 	pop	ar2
      000A0F 50 03            [24] 1606 	jnc	00119$
      000A11 02 07 C9         [24] 1607 	ljmp	00105$
      000A14                       1608 00119$:
      000A14 85 08 81         [24] 1609 	mov	sp,_bp
      000A17 D0 08            [24] 1610 	pop	_bp
      000A19 22               [24] 1611 	ret
                                   1612 ;------------------------------------------------------------
                                   1613 ;Allocation info for local variables in function 'aes_encrypt'
                                   1614 ;------------------------------------------------------------
                                   1615 ;key                       Allocated to stack - _bp -5
                                   1616 ;state                     Allocated to stack - _bp +1
                                   1617 ;i                         Allocated to registers r3 
                                   1618 ;w                         Allocated to stack - _bp +4
                                   1619 ;------------------------------------------------------------
                                   1620 ;	aes_crypto.c:158: void aes_encrypt(uint8_t* state, uint8_t* key)
                                   1621 ;	-----------------------------------------
                                   1622 ;	 function aes_encrypt
                                   1623 ;	-----------------------------------------
      000A1A                       1624 _aes_encrypt:
      000A1A C0 08            [24] 1625 	push	_bp
      000A1C 85 81 08         [24] 1626 	mov	_bp,sp
      000A1F C0 82            [24] 1627 	push	dpl
      000A21 C0 83            [24] 1628 	push	dph
      000A23 C0 F0            [24] 1629 	push	b
      000A25 E5 81            [12] 1630 	mov	a,sp
      000A27 24 10            [12] 1631 	add	a,#0x10
      000A29 F5 81            [12] 1632 	mov	sp,a
                                   1633 ;	aes_crypto.c:162: for(i = 0; i < KEYLEN; i++)
      000A2B E5 08            [12] 1634 	mov	a,_bp
      000A2D 24 04            [12] 1635 	add	a,#0x04
      000A2F FC               [12] 1636 	mov	r4,a
      000A30 7B 00            [12] 1637 	mov	r3,#0x00
      000A32                       1638 00103$:
                                   1639 ;	aes_crypto.c:163: w[i] = key[i];
      000A32 EB               [12] 1640 	mov	a,r3
      000A33 2C               [12] 1641 	add	a,r4
      000A34 F9               [12] 1642 	mov	r1,a
      000A35 E5 08            [12] 1643 	mov	a,_bp
      000A37 24 FB            [12] 1644 	add	a,#0xfb
      000A39 F8               [12] 1645 	mov	r0,a
      000A3A EB               [12] 1646 	mov	a,r3
      000A3B 26               [12] 1647 	add	a,@r0
      000A3C FA               [12] 1648 	mov	r2,a
      000A3D E4               [12] 1649 	clr	a
      000A3E 08               [12] 1650 	inc	r0
      000A3F 36               [12] 1651 	addc	a,@r0
      000A40 FE               [12] 1652 	mov	r6,a
      000A41 08               [12] 1653 	inc	r0
      000A42 86 07            [24] 1654 	mov	ar7,@r0
      000A44 8A 82            [24] 1655 	mov	dpl,r2
      000A46 8E 83            [24] 1656 	mov	dph,r6
      000A48 8F F0            [24] 1657 	mov	b,r7
      000A4A 12 1C A1         [24] 1658 	lcall	__gptrget
      000A4D FA               [12] 1659 	mov	r2,a
      000A4E F7               [12] 1660 	mov	@r1,a
                                   1661 ;	aes_crypto.c:162: for(i = 0; i < KEYLEN; i++)
      000A4F 0B               [12] 1662 	inc	r3
      000A50 BB 10 00         [24] 1663 	cjne	r3,#0x10,00123$
      000A53                       1664 00123$:
      000A53 40 DD            [24] 1665 	jc	00103$
                                   1666 ;	aes_crypto.c:165: add_round_key(state, w);
      000A55 8C 03            [24] 1667 	mov	ar3,r4
      000A57 8B 02            [24] 1668 	mov	ar2,r3
      000A59 7B 00            [12] 1669 	mov	r3,#0x00
      000A5B 7F 40            [12] 1670 	mov	r7,#0x40
      000A5D C0 04            [24] 1671 	push	ar4
      000A5F C0 02            [24] 1672 	push	ar2
      000A61 C0 03            [24] 1673 	push	ar3
      000A63 C0 07            [24] 1674 	push	ar7
      000A65 A8 08            [24] 1675 	mov	r0,_bp
      000A67 08               [12] 1676 	inc	r0
      000A68 86 82            [24] 1677 	mov	dpl,@r0
      000A6A 08               [12] 1678 	inc	r0
      000A6B 86 83            [24] 1679 	mov	dph,@r0
      000A6D 08               [12] 1680 	inc	r0
      000A6E 86 F0            [24] 1681 	mov	b,@r0
      000A70 12 01 B7         [24] 1682 	lcall	_add_round_key
      000A73 15 81            [12] 1683 	dec	sp
      000A75 15 81            [12] 1684 	dec	sp
      000A77 15 81            [12] 1685 	dec	sp
      000A79 D0 04            [24] 1686 	pop	ar4
                                   1687 ;	aes_crypto.c:166: for(i = 1; i < Nr; i++)
      000A7B 8C 07            [24] 1688 	mov	ar7,r4
      000A7D 8C 06            [24] 1689 	mov	ar6,r4
      000A7F 7D 01            [12] 1690 	mov	r5,#0x01
      000A81                       1691 00105$:
                                   1692 ;	aes_crypto.c:168: sub_bytes(state);
      000A81 C0 04            [24] 1693 	push	ar4
      000A83 A8 08            [24] 1694 	mov	r0,_bp
      000A85 08               [12] 1695 	inc	r0
      000A86 86 82            [24] 1696 	mov	dpl,@r0
      000A88 08               [12] 1697 	inc	r0
      000A89 86 83            [24] 1698 	mov	dph,@r0
      000A8B 08               [12] 1699 	inc	r0
      000A8C 86 F0            [24] 1700 	mov	b,@r0
      000A8E C0 07            [24] 1701 	push	ar7
      000A90 C0 06            [24] 1702 	push	ar6
      000A92 C0 05            [24] 1703 	push	ar5
      000A94 C0 04            [24] 1704 	push	ar4
      000A96 12 02 38         [24] 1705 	lcall	_sub_bytes
                                   1706 ;	aes_crypto.c:169: shift_rows(state);
      000A99 A8 08            [24] 1707 	mov	r0,_bp
      000A9B 08               [12] 1708 	inc	r0
      000A9C 86 82            [24] 1709 	mov	dpl,@r0
      000A9E 08               [12] 1710 	inc	r0
      000A9F 86 83            [24] 1711 	mov	dph,@r0
      000AA1 08               [12] 1712 	inc	r0
      000AA2 86 F0            [24] 1713 	mov	b,@r0
      000AA4 12 02 66         [24] 1714 	lcall	_shift_rows
                                   1715 ;	aes_crypto.c:170: mix_columns(state);
      000AA7 A8 08            [24] 1716 	mov	r0,_bp
      000AA9 08               [12] 1717 	inc	r0
      000AAA 86 82            [24] 1718 	mov	dpl,@r0
      000AAC 08               [12] 1719 	inc	r0
      000AAD 86 83            [24] 1720 	mov	dph,@r0
      000AAF 08               [12] 1721 	inc	r0
      000AB0 86 F0            [24] 1722 	mov	b,@r0
      000AB2 12 04 AF         [24] 1723 	lcall	_mix_columns
      000AB5 D0 04            [24] 1724 	pop	ar4
      000AB7 D0 05            [24] 1725 	pop	ar5
      000AB9 D0 06            [24] 1726 	pop	ar6
      000ABB D0 07            [24] 1727 	pop	ar7
                                   1728 ;	aes_crypto.c:171: update_key(w, i);	
      000ABD 8F 02            [24] 1729 	mov	ar2,r7
      000ABF 7B 00            [12] 1730 	mov	r3,#0x00
      000AC1 7C 40            [12] 1731 	mov	r4,#0x40
      000AC3 C0 07            [24] 1732 	push	ar7
      000AC5 C0 06            [24] 1733 	push	ar6
      000AC7 C0 05            [24] 1734 	push	ar5
      000AC9 C0 04            [24] 1735 	push	ar4
      000ACB C0 05            [24] 1736 	push	ar5
      000ACD 8A 82            [24] 1737 	mov	dpl,r2
      000ACF 8B 83            [24] 1738 	mov	dph,r3
      000AD1 8C F0            [24] 1739 	mov	b,r4
      000AD3 12 07 34         [24] 1740 	lcall	_update_key
      000AD6 15 81            [12] 1741 	dec	sp
      000AD8 D0 04            [24] 1742 	pop	ar4
      000ADA D0 05            [24] 1743 	pop	ar5
      000ADC D0 06            [24] 1744 	pop	ar6
                                   1745 ;	aes_crypto.c:172: add_round_key(state, w);
      000ADE 8E 02            [24] 1746 	mov	ar2,r6
      000AE0 7B 00            [12] 1747 	mov	r3,#0x00
      000AE2 7C 40            [12] 1748 	mov	r4,#0x40
      000AE4 C0 06            [24] 1749 	push	ar6
      000AE6 C0 05            [24] 1750 	push	ar5
      000AE8 C0 04            [24] 1751 	push	ar4
      000AEA C0 02            [24] 1752 	push	ar2
      000AEC C0 03            [24] 1753 	push	ar3
      000AEE C0 04            [24] 1754 	push	ar4
      000AF0 A8 08            [24] 1755 	mov	r0,_bp
      000AF2 08               [12] 1756 	inc	r0
      000AF3 86 82            [24] 1757 	mov	dpl,@r0
      000AF5 08               [12] 1758 	inc	r0
      000AF6 86 83            [24] 1759 	mov	dph,@r0
      000AF8 08               [12] 1760 	inc	r0
      000AF9 86 F0            [24] 1761 	mov	b,@r0
      000AFB 12 01 B7         [24] 1762 	lcall	_add_round_key
      000AFE 15 81            [12] 1763 	dec	sp
      000B00 15 81            [12] 1764 	dec	sp
      000B02 15 81            [12] 1765 	dec	sp
      000B04 D0 04            [24] 1766 	pop	ar4
      000B06 D0 05            [24] 1767 	pop	ar5
      000B08 D0 06            [24] 1768 	pop	ar6
      000B0A D0 07            [24] 1769 	pop	ar7
                                   1770 ;	aes_crypto.c:166: for(i = 1; i < Nr; i++)
      000B0C 0D               [12] 1771 	inc	r5
      000B0D BD 0A 00         [24] 1772 	cjne	r5,#0x0a,00125$
      000B10                       1773 00125$:
      000B10 D0 04            [24] 1774 	pop	ar4
      000B12 50 03            [24] 1775 	jnc	00126$
      000B14 02 0A 81         [24] 1776 	ljmp	00105$
      000B17                       1777 00126$:
                                   1778 ;	aes_crypto.c:175: sub_bytes(state);
      000B17 A8 08            [24] 1779 	mov	r0,_bp
      000B19 08               [12] 1780 	inc	r0
      000B1A 86 82            [24] 1781 	mov	dpl,@r0
      000B1C 08               [12] 1782 	inc	r0
      000B1D 86 83            [24] 1783 	mov	dph,@r0
      000B1F 08               [12] 1784 	inc	r0
      000B20 86 F0            [24] 1785 	mov	b,@r0
      000B22 C0 05            [24] 1786 	push	ar5
      000B24 C0 04            [24] 1787 	push	ar4
      000B26 12 02 38         [24] 1788 	lcall	_sub_bytes
                                   1789 ;	aes_crypto.c:176: shift_rows(state);
      000B29 A8 08            [24] 1790 	mov	r0,_bp
      000B2B 08               [12] 1791 	inc	r0
      000B2C 86 82            [24] 1792 	mov	dpl,@r0
      000B2E 08               [12] 1793 	inc	r0
      000B2F 86 83            [24] 1794 	mov	dph,@r0
      000B31 08               [12] 1795 	inc	r0
      000B32 86 F0            [24] 1796 	mov	b,@r0
      000B34 12 02 66         [24] 1797 	lcall	_shift_rows
      000B37 D0 04            [24] 1798 	pop	ar4
      000B39 D0 05            [24] 1799 	pop	ar5
                                   1800 ;	aes_crypto.c:177: update_key(w, i);
      000B3B 8C 07            [24] 1801 	mov	ar7,r4
      000B3D 7E 00            [12] 1802 	mov	r6,#0x00
      000B3F 7B 40            [12] 1803 	mov	r3,#0x40
      000B41 C0 04            [24] 1804 	push	ar4
      000B43 C0 05            [24] 1805 	push	ar5
      000B45 8F 82            [24] 1806 	mov	dpl,r7
      000B47 8E 83            [24] 1807 	mov	dph,r6
      000B49 8B F0            [24] 1808 	mov	b,r3
      000B4B 12 07 34         [24] 1809 	lcall	_update_key
      000B4E 15 81            [12] 1810 	dec	sp
                                   1811 ;	aes_crypto.c:178: add_round_key(state, w);
      000B50 7F 00            [12] 1812 	mov	r7,#0x00
      000B52 7E 40            [12] 1813 	mov	r6,#0x40
      000B54 C0 07            [24] 1814 	push	ar7
      000B56 C0 06            [24] 1815 	push	ar6
      000B58 A8 08            [24] 1816 	mov	r0,_bp
      000B5A 08               [12] 1817 	inc	r0
      000B5B 86 82            [24] 1818 	mov	dpl,@r0
      000B5D 08               [12] 1819 	inc	r0
      000B5E 86 83            [24] 1820 	mov	dph,@r0
      000B60 08               [12] 1821 	inc	r0
      000B61 86 F0            [24] 1822 	mov	b,@r0
      000B63 12 01 B7         [24] 1823 	lcall	_add_round_key
      000B66 15 81            [12] 1824 	dec	sp
      000B68 15 81            [12] 1825 	dec	sp
      000B6A 15 81            [12] 1826 	dec	sp
      000B6C 85 08 81         [24] 1827 	mov	sp,_bp
      000B6F D0 08            [24] 1828 	pop	_bp
      000B71 22               [24] 1829 	ret
                                   1830 	.area CSEG    (CODE)
                                   1831 	.area CONST   (CODE)
      001CC1                       1832 _s_box:
      001CC1 63                    1833 	.db #0x63	; 99	'c'
      001CC2 7C                    1834 	.db #0x7c	; 124
      001CC3 77                    1835 	.db #0x77	; 119	'w'
      001CC4 7B                    1836 	.db #0x7b	; 123
      001CC5 F2                    1837 	.db #0xf2	; 242
      001CC6 6B                    1838 	.db #0x6b	; 107	'k'
      001CC7 6F                    1839 	.db #0x6f	; 111	'o'
      001CC8 C5                    1840 	.db #0xc5	; 197
      001CC9 30                    1841 	.db #0x30	; 48	'0'
      001CCA 01                    1842 	.db #0x01	; 1
      001CCB 67                    1843 	.db #0x67	; 103	'g'
      001CCC 2B                    1844 	.db #0x2b	; 43
      001CCD FE                    1845 	.db #0xfe	; 254
      001CCE D7                    1846 	.db #0xd7	; 215
      001CCF AB                    1847 	.db #0xab	; 171
      001CD0 76                    1848 	.db #0x76	; 118	'v'
      001CD1 CA                    1849 	.db #0xca	; 202
      001CD2 82                    1850 	.db #0x82	; 130
      001CD3 C9                    1851 	.db #0xc9	; 201
      001CD4 7D                    1852 	.db #0x7d	; 125
      001CD5 FA                    1853 	.db #0xfa	; 250
      001CD6 59                    1854 	.db #0x59	; 89	'Y'
      001CD7 47                    1855 	.db #0x47	; 71	'G'
      001CD8 F0                    1856 	.db #0xf0	; 240
      001CD9 AD                    1857 	.db #0xad	; 173
      001CDA D4                    1858 	.db #0xd4	; 212
      001CDB A2                    1859 	.db #0xa2	; 162
      001CDC AF                    1860 	.db #0xaf	; 175
      001CDD 9C                    1861 	.db #0x9c	; 156
      001CDE A4                    1862 	.db #0xa4	; 164
      001CDF 72                    1863 	.db #0x72	; 114	'r'
      001CE0 C0                    1864 	.db #0xc0	; 192
      001CE1 B7                    1865 	.db #0xb7	; 183
      001CE2 FD                    1866 	.db #0xfd	; 253
      001CE3 93                    1867 	.db #0x93	; 147
      001CE4 26                    1868 	.db #0x26	; 38
      001CE5 36                    1869 	.db #0x36	; 54	'6'
      001CE6 3F                    1870 	.db #0x3f	; 63
      001CE7 F7                    1871 	.db #0xf7	; 247
      001CE8 CC                    1872 	.db #0xcc	; 204
      001CE9 34                    1873 	.db #0x34	; 52	'4'
      001CEA A5                    1874 	.db #0xa5	; 165
      001CEB E5                    1875 	.db #0xe5	; 229
      001CEC F1                    1876 	.db #0xf1	; 241
      001CED 71                    1877 	.db #0x71	; 113	'q'
      001CEE D8                    1878 	.db #0xd8	; 216
      001CEF 31                    1879 	.db #0x31	; 49	'1'
      001CF0 15                    1880 	.db #0x15	; 21
      001CF1 04                    1881 	.db #0x04	; 4
      001CF2 C7                    1882 	.db #0xc7	; 199
      001CF3 23                    1883 	.db #0x23	; 35
      001CF4 C3                    1884 	.db #0xc3	; 195
      001CF5 18                    1885 	.db #0x18	; 24
      001CF6 96                    1886 	.db #0x96	; 150
      001CF7 05                    1887 	.db #0x05	; 5
      001CF8 9A                    1888 	.db #0x9a	; 154
      001CF9 07                    1889 	.db #0x07	; 7
      001CFA 12                    1890 	.db #0x12	; 18
      001CFB 80                    1891 	.db #0x80	; 128
      001CFC E2                    1892 	.db #0xe2	; 226
      001CFD EB                    1893 	.db #0xeb	; 235
      001CFE 27                    1894 	.db #0x27	; 39
      001CFF B2                    1895 	.db #0xb2	; 178
      001D00 75                    1896 	.db #0x75	; 117	'u'
      001D01 09                    1897 	.db #0x09	; 9
      001D02 83                    1898 	.db #0x83	; 131
      001D03 2C                    1899 	.db #0x2c	; 44
      001D04 1A                    1900 	.db #0x1a	; 26
      001D05 1B                    1901 	.db #0x1b	; 27
      001D06 6E                    1902 	.db #0x6e	; 110	'n'
      001D07 5A                    1903 	.db #0x5a	; 90	'Z'
      001D08 A0                    1904 	.db #0xa0	; 160
      001D09 52                    1905 	.db #0x52	; 82	'R'
      001D0A 3B                    1906 	.db #0x3b	; 59
      001D0B D6                    1907 	.db #0xd6	; 214
      001D0C B3                    1908 	.db #0xb3	; 179
      001D0D 29                    1909 	.db #0x29	; 41
      001D0E E3                    1910 	.db #0xe3	; 227
      001D0F 2F                    1911 	.db #0x2f	; 47
      001D10 84                    1912 	.db #0x84	; 132
      001D11 53                    1913 	.db #0x53	; 83	'S'
      001D12 D1                    1914 	.db #0xd1	; 209
      001D13 00                    1915 	.db #0x00	; 0
      001D14 ED                    1916 	.db #0xed	; 237
      001D15 20                    1917 	.db #0x20	; 32
      001D16 FC                    1918 	.db #0xfc	; 252
      001D17 B1                    1919 	.db #0xb1	; 177
      001D18 5B                    1920 	.db #0x5b	; 91
      001D19 6A                    1921 	.db #0x6a	; 106	'j'
      001D1A CB                    1922 	.db #0xcb	; 203
      001D1B BE                    1923 	.db #0xbe	; 190
      001D1C 39                    1924 	.db #0x39	; 57	'9'
      001D1D 4A                    1925 	.db #0x4a	; 74	'J'
      001D1E 4C                    1926 	.db #0x4c	; 76	'L'
      001D1F 58                    1927 	.db #0x58	; 88	'X'
      001D20 CF                    1928 	.db #0xcf	; 207
      001D21 D0                    1929 	.db #0xd0	; 208
      001D22 EF                    1930 	.db #0xef	; 239
      001D23 AA                    1931 	.db #0xaa	; 170
      001D24 FB                    1932 	.db #0xfb	; 251
      001D25 43                    1933 	.db #0x43	; 67	'C'
      001D26 4D                    1934 	.db #0x4d	; 77	'M'
      001D27 33                    1935 	.db #0x33	; 51	'3'
      001D28 85                    1936 	.db #0x85	; 133
      001D29 45                    1937 	.db #0x45	; 69	'E'
      001D2A F9                    1938 	.db #0xf9	; 249
      001D2B 02                    1939 	.db #0x02	; 2
      001D2C 7F                    1940 	.db #0x7f	; 127
      001D2D 50                    1941 	.db #0x50	; 80	'P'
      001D2E 3C                    1942 	.db #0x3c	; 60
      001D2F 9F                    1943 	.db #0x9f	; 159
      001D30 A8                    1944 	.db #0xa8	; 168
      001D31 51                    1945 	.db #0x51	; 81	'Q'
      001D32 A3                    1946 	.db #0xa3	; 163
      001D33 40                    1947 	.db #0x40	; 64
      001D34 8F                    1948 	.db #0x8f	; 143
      001D35 92                    1949 	.db #0x92	; 146
      001D36 9D                    1950 	.db #0x9d	; 157
      001D37 38                    1951 	.db #0x38	; 56	'8'
      001D38 F5                    1952 	.db #0xf5	; 245
      001D39 BC                    1953 	.db #0xbc	; 188
      001D3A B6                    1954 	.db #0xb6	; 182
      001D3B DA                    1955 	.db #0xda	; 218
      001D3C 21                    1956 	.db #0x21	; 33
      001D3D 10                    1957 	.db #0x10	; 16
      001D3E FF                    1958 	.db #0xff	; 255
      001D3F F3                    1959 	.db #0xf3	; 243
      001D40 D2                    1960 	.db #0xd2	; 210
      001D41 CD                    1961 	.db #0xcd	; 205
      001D42 0C                    1962 	.db #0x0c	; 12
      001D43 13                    1963 	.db #0x13	; 19
      001D44 EC                    1964 	.db #0xec	; 236
      001D45 5F                    1965 	.db #0x5f	; 95
      001D46 97                    1966 	.db #0x97	; 151
      001D47 44                    1967 	.db #0x44	; 68	'D'
      001D48 17                    1968 	.db #0x17	; 23
      001D49 C4                    1969 	.db #0xc4	; 196
      001D4A A7                    1970 	.db #0xa7	; 167
      001D4B 7E                    1971 	.db #0x7e	; 126
      001D4C 3D                    1972 	.db #0x3d	; 61
      001D4D 64                    1973 	.db #0x64	; 100	'd'
      001D4E 5D                    1974 	.db #0x5d	; 93
      001D4F 19                    1975 	.db #0x19	; 25
      001D50 73                    1976 	.db #0x73	; 115	's'
      001D51 60                    1977 	.db #0x60	; 96
      001D52 81                    1978 	.db #0x81	; 129
      001D53 4F                    1979 	.db #0x4f	; 79	'O'
      001D54 DC                    1980 	.db #0xdc	; 220
      001D55 22                    1981 	.db #0x22	; 34
      001D56 2A                    1982 	.db #0x2a	; 42
      001D57 90                    1983 	.db #0x90	; 144
      001D58 88                    1984 	.db #0x88	; 136
      001D59 46                    1985 	.db #0x46	; 70	'F'
      001D5A EE                    1986 	.db #0xee	; 238
      001D5B B8                    1987 	.db #0xb8	; 184
      001D5C 14                    1988 	.db #0x14	; 20
      001D5D DE                    1989 	.db #0xde	; 222
      001D5E 5E                    1990 	.db #0x5e	; 94
      001D5F 0B                    1991 	.db #0x0b	; 11
      001D60 DB                    1992 	.db #0xdb	; 219
      001D61 E0                    1993 	.db #0xe0	; 224
      001D62 32                    1994 	.db #0x32	; 50	'2'
      001D63 3A                    1995 	.db #0x3a	; 58
      001D64 0A                    1996 	.db #0x0a	; 10
      001D65 49                    1997 	.db #0x49	; 73	'I'
      001D66 06                    1998 	.db #0x06	; 6
      001D67 24                    1999 	.db #0x24	; 36
      001D68 5C                    2000 	.db #0x5c	; 92
      001D69 C2                    2001 	.db #0xc2	; 194
      001D6A D3                    2002 	.db #0xd3	; 211
      001D6B AC                    2003 	.db #0xac	; 172
      001D6C 62                    2004 	.db #0x62	; 98	'b'
      001D6D 91                    2005 	.db #0x91	; 145
      001D6E 95                    2006 	.db #0x95	; 149
      001D6F E4                    2007 	.db #0xe4	; 228
      001D70 79                    2008 	.db #0x79	; 121	'y'
      001D71 E7                    2009 	.db #0xe7	; 231
      001D72 C8                    2010 	.db #0xc8	; 200
      001D73 37                    2011 	.db #0x37	; 55	'7'
      001D74 6D                    2012 	.db #0x6d	; 109	'm'
      001D75 8D                    2013 	.db #0x8d	; 141
      001D76 D5                    2014 	.db #0xd5	; 213
      001D77 4E                    2015 	.db #0x4e	; 78	'N'
      001D78 A9                    2016 	.db #0xa9	; 169
      001D79 6C                    2017 	.db #0x6c	; 108	'l'
      001D7A 56                    2018 	.db #0x56	; 86	'V'
      001D7B F4                    2019 	.db #0xf4	; 244
      001D7C EA                    2020 	.db #0xea	; 234
      001D7D 65                    2021 	.db #0x65	; 101	'e'
      001D7E 7A                    2022 	.db #0x7a	; 122	'z'
      001D7F AE                    2023 	.db #0xae	; 174
      001D80 08                    2024 	.db #0x08	; 8
      001D81 BA                    2025 	.db #0xba	; 186
      001D82 78                    2026 	.db #0x78	; 120	'x'
      001D83 25                    2027 	.db #0x25	; 37
      001D84 2E                    2028 	.db #0x2e	; 46
      001D85 1C                    2029 	.db #0x1c	; 28
      001D86 A6                    2030 	.db #0xa6	; 166
      001D87 B4                    2031 	.db #0xb4	; 180
      001D88 C6                    2032 	.db #0xc6	; 198
      001D89 E8                    2033 	.db #0xe8	; 232
      001D8A DD                    2034 	.db #0xdd	; 221
      001D8B 74                    2035 	.db #0x74	; 116	't'
      001D8C 1F                    2036 	.db #0x1f	; 31
      001D8D 4B                    2037 	.db #0x4b	; 75	'K'
      001D8E BD                    2038 	.db #0xbd	; 189
      001D8F 8B                    2039 	.db #0x8b	; 139
      001D90 8A                    2040 	.db #0x8a	; 138
      001D91 70                    2041 	.db #0x70	; 112	'p'
      001D92 3E                    2042 	.db #0x3e	; 62
      001D93 B5                    2043 	.db #0xb5	; 181
      001D94 66                    2044 	.db #0x66	; 102	'f'
      001D95 48                    2045 	.db #0x48	; 72	'H'
      001D96 03                    2046 	.db #0x03	; 3
      001D97 F6                    2047 	.db #0xf6	; 246
      001D98 0E                    2048 	.db #0x0e	; 14
      001D99 61                    2049 	.db #0x61	; 97	'a'
      001D9A 35                    2050 	.db #0x35	; 53	'5'
      001D9B 57                    2051 	.db #0x57	; 87	'W'
      001D9C B9                    2052 	.db #0xb9	; 185
      001D9D 86                    2053 	.db #0x86	; 134
      001D9E C1                    2054 	.db #0xc1	; 193
      001D9F 1D                    2055 	.db #0x1d	; 29
      001DA0 9E                    2056 	.db #0x9e	; 158
      001DA1 E1                    2057 	.db #0xe1	; 225
      001DA2 F8                    2058 	.db #0xf8	; 248
      001DA3 98                    2059 	.db #0x98	; 152
      001DA4 11                    2060 	.db #0x11	; 17
      001DA5 69                    2061 	.db #0x69	; 105	'i'
      001DA6 D9                    2062 	.db #0xd9	; 217
      001DA7 8E                    2063 	.db #0x8e	; 142
      001DA8 94                    2064 	.db #0x94	; 148
      001DA9 9B                    2065 	.db #0x9b	; 155
      001DAA 1E                    2066 	.db #0x1e	; 30
      001DAB 87                    2067 	.db #0x87	; 135
      001DAC E9                    2068 	.db #0xe9	; 233
      001DAD CE                    2069 	.db #0xce	; 206
      001DAE 55                    2070 	.db #0x55	; 85	'U'
      001DAF 28                    2071 	.db #0x28	; 40
      001DB0 DF                    2072 	.db #0xdf	; 223
      001DB1 8C                    2073 	.db #0x8c	; 140
      001DB2 A1                    2074 	.db #0xa1	; 161
      001DB3 89                    2075 	.db #0x89	; 137
      001DB4 0D                    2076 	.db #0x0d	; 13
      001DB5 BF                    2077 	.db #0xbf	; 191
      001DB6 E6                    2078 	.db #0xe6	; 230
      001DB7 42                    2079 	.db #0x42	; 66	'B'
      001DB8 68                    2080 	.db #0x68	; 104	'h'
      001DB9 41                    2081 	.db #0x41	; 65	'A'
      001DBA 99                    2082 	.db #0x99	; 153
      001DBB 2D                    2083 	.db #0x2d	; 45
      001DBC 0F                    2084 	.db #0x0f	; 15
      001DBD B0                    2085 	.db #0xb0	; 176
      001DBE 54                    2086 	.db #0x54	; 84	'T'
      001DBF BB                    2087 	.db #0xbb	; 187
      001DC0 16                    2088 	.db #0x16	; 22
      001DC1                       2089 _rcon:
      001DC1 00                    2090 	.db #0x00	; 0
      001DC2 01                    2091 	.db #0x01	; 1
      001DC3 02                    2092 	.db #0x02	; 2
      001DC4 04                    2093 	.db #0x04	; 4
      001DC5 08                    2094 	.db #0x08	; 8
      001DC6 10                    2095 	.db #0x10	; 16
      001DC7 20                    2096 	.db #0x20	; 32
      001DC8 40                    2097 	.db #0x40	; 64
      001DC9 80                    2098 	.db #0x80	; 128
      001DCA 1B                    2099 	.db #0x1b	; 27
      001DCB 36                    2100 	.db #0x36	; 54	'6'
                                   2101 	.area XINIT   (CODE)
                                   2102 	.area CABS    (ABS,CODE)
