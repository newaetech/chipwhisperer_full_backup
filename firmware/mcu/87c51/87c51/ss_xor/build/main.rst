                                      1 ;--------------------------------------------------------
                                      2 ; File Created by SDCC : free open source ANSI-C Compiler
                                      3 ; Version 3.6.0 #9615 (MINGW32)
                                      4 ;--------------------------------------------------------
                                      5 	.module main
                                      6 	.optsdcc -mmcs51 --model-small
                                      7 	
                                      8 ;--------------------------------------------------------
                                      9 ; Public variables in this module
                                     10 ;--------------------------------------------------------
                                     11 	.globl _main
                                     12 	.globl _xor_encrypt
                                     13 	.globl _simpleserial_put
                                     14 	.globl _simpleserial_get
                                     15 	.globl _inituart
                                     16 	.globl _SIO_SM0
                                     17 	.globl _SIO_SM1
                                     18 	.globl _SIO_SM2
                                     19 	.globl _SIO_REN
                                     20 	.globl _SIO_TB8
                                     21 	.globl _SIO_RB8
                                     22 	.globl _SIO_TI
                                     23 	.globl _SIO_RI
                                     24 	.globl _SIO_TR1
                                     25 	.globl _SIO_SBUF
                                     26 	.globl _SIO_SCON
                                     27 	.globl _SIO_TL1
                                     28 	.globl _SIO_TH1
                                     29 	.globl _SIO_TMOD
                                     30 	.globl _SIO_PCON
                                     31 	.globl _putchar
                                     32 	.globl _getchar
                                     33 ;--------------------------------------------------------
                                     34 ; special function registers
                                     35 ;--------------------------------------------------------
                                     36 	.area RSEG    (ABS,DATA)
      000000                         37 	.org 0x0000
                           000087    38 _SIO_PCON	=	0x0087
                           000089    39 _SIO_TMOD	=	0x0089
                           00008D    40 _SIO_TH1	=	0x008d
                           00008B    41 _SIO_TL1	=	0x008b
                           000098    42 _SIO_SCON	=	0x0098
                           000099    43 _SIO_SBUF	=	0x0099
                                     44 ;--------------------------------------------------------
                                     45 ; special function bits
                                     46 ;--------------------------------------------------------
                                     47 	.area RSEG    (ABS,DATA)
      000000                         48 	.org 0x0000
                           00008E    49 _SIO_TR1	=	0x008e
                           000098    50 _SIO_RI	=	0x0098
                           000099    51 _SIO_TI	=	0x0099
                           00009A    52 _SIO_RB8	=	0x009a
                           00009B    53 _SIO_TB8	=	0x009b
                           00009C    54 _SIO_REN	=	0x009c
                           00009D    55 _SIO_SM2	=	0x009d
                           00009E    56 _SIO_SM1	=	0x009e
                           00009F    57 _SIO_SM0	=	0x009f
                                     58 ;--------------------------------------------------------
                                     59 ; overlayable register banks
                                     60 ;--------------------------------------------------------
                                     61 	.area REG_BANK_0	(REL,OVR,DATA)
      000000                         62 	.ds 8
                                     63 ;--------------------------------------------------------
                                     64 ; internal ram data
                                     65 ;--------------------------------------------------------
                                     66 	.area DSEG    (DATA)
                                     67 ;--------------------------------------------------------
                                     68 ; overlayable items in internal ram 
                                     69 ;--------------------------------------------------------
                                     70 ;--------------------------------------------------------
                                     71 ; Stack segment in internal ram 
                                     72 ;--------------------------------------------------------
                                     73 	.area	SSEG
      000009                         74 __start__stack:
      000009                         75 	.ds	1
                                     76 
                                     77 ;--------------------------------------------------------
                                     78 ; indirectly addressable internal ram data
                                     79 ;--------------------------------------------------------
                                     80 	.area ISEG    (DATA)
                                     81 ;--------------------------------------------------------
                                     82 ; absolute internal ram data
                                     83 ;--------------------------------------------------------
                                     84 	.area IABS    (ABS,DATA)
                                     85 	.area IABS    (ABS,DATA)
                                     86 ;--------------------------------------------------------
                                     87 ; bit data
                                     88 ;--------------------------------------------------------
                                     89 	.area BSEG    (BIT)
                                     90 ;--------------------------------------------------------
                                     91 ; paged external ram data
                                     92 ;--------------------------------------------------------
                                     93 	.area PSEG    (PAG,XDATA)
                                     94 ;--------------------------------------------------------
                                     95 ; external ram data
                                     96 ;--------------------------------------------------------
                                     97 	.area XSEG    (XDATA)
                                     98 ;--------------------------------------------------------
                                     99 ; absolute external ram data
                                    100 ;--------------------------------------------------------
                                    101 	.area XABS    (ABS,XDATA)
                                    102 ;--------------------------------------------------------
                                    103 ; external initialized ram data
                                    104 ;--------------------------------------------------------
                                    105 	.area XISEG   (XDATA)
                                    106 	.area HOME    (CODE)
                                    107 	.area GSINIT0 (CODE)
                                    108 	.area GSINIT1 (CODE)
                                    109 	.area GSINIT2 (CODE)
                                    110 	.area GSINIT3 (CODE)
                                    111 	.area GSINIT4 (CODE)
                                    112 	.area GSINIT5 (CODE)
                                    113 	.area GSINIT  (CODE)
                                    114 	.area GSFINAL (CODE)
                                    115 	.area CSEG    (CODE)
                                    116 ;--------------------------------------------------------
                                    117 ; interrupt vector 
                                    118 ;--------------------------------------------------------
                                    119 	.area HOME    (CODE)
      000000                        120 __interrupt_vect:
      000000 02 00 06         [24]  121 	ljmp	__sdcc_gsinit_startup
                                    122 ;--------------------------------------------------------
                                    123 ; global & static initialisations
                                    124 ;--------------------------------------------------------
                                    125 	.area HOME    (CODE)
                                    126 	.area GSINIT  (CODE)
                                    127 	.area GSFINAL (CODE)
                                    128 	.area GSINIT  (CODE)
                                    129 	.globl __sdcc_gsinit_startup
                                    130 	.globl __sdcc_program_startup
                                    131 	.globl __start__stack
                                    132 	.globl __mcs51_genXINIT
                                    133 	.globl __mcs51_genXRAMCLEAR
                                    134 	.globl __mcs51_genRAMCLEAR
                                    135 	.area GSFINAL (CODE)
      00005F 02 00 03         [24]  136 	ljmp	__sdcc_program_startup
                                    137 ;--------------------------------------------------------
                                    138 ; Home
                                    139 ;--------------------------------------------------------
                                    140 	.area HOME    (CODE)
                                    141 	.area HOME    (CODE)
      000003                        142 __sdcc_program_startup:
      000003 02 00 B5         [24]  143 	ljmp	_main
                                    144 ;	return from main will return to caller
                                    145 ;--------------------------------------------------------
                                    146 ; code
                                    147 ;--------------------------------------------------------
                                    148 	.area CSEG    (CODE)
                                    149 ;------------------------------------------------------------
                                    150 ;Allocation info for local variables in function 'inituart'
                                    151 ;------------------------------------------------------------
                                    152 ;t1_reload                 Allocated to registers r7 
                                    153 ;------------------------------------------------------------
                                    154 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:50: void inituart (unsigned char t1_reload)
                                    155 ;	-----------------------------------------
                                    156 ;	 function inituart
                                    157 ;	-----------------------------------------
      000062                        158 _inituart:
                           000007   159 	ar7 = 0x07
                           000006   160 	ar6 = 0x06
                           000005   161 	ar5 = 0x05
                           000004   162 	ar4 = 0x04
                           000003   163 	ar3 = 0x03
                           000002   164 	ar2 = 0x02
                           000001   165 	ar1 = 0x01
                           000000   166 	ar0 = 0x00
      000062 AF 82            [24]  167 	mov	r7,dpl
                                    168 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:52: SIO_TR1=0;
      000064 C2 8E            [12]  169 	clr	_SIO_TR1
                                    170 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:53: SIO_TMOD=(SIO_TMOD&0x0f)|0x20;
      000066 74 0F            [12]  171 	mov	a,#0x0f
      000068 55 89            [12]  172 	anl	a,_SIO_TMOD
      00006A 44 20            [12]  173 	orl	a,#0x20
      00006C F5 89            [12]  174 	mov	_SIO_TMOD,a
                                    175 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:54: SIO_PCON|=0x80;
      00006E 43 87 80         [24]  176 	orl	_SIO_PCON,#0x80
                                    177 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:55: SIO_TH1=SIO_TL1=t1_reload;
      000071 8F 8B            [24]  178 	mov	_SIO_TL1,r7
      000073 8F 8D            [24]  179 	mov	_SIO_TH1,r7
                                    180 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:56: SIO_TR1=1;
      000075 D2 8E            [12]  181 	setb	_SIO_TR1
                                    182 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:57: SIO_SCON=0x52;
      000077 75 98 52         [24]  183 	mov	_SIO_SCON,#0x52
      00007A 22               [24]  184 	ret
                                    185 ;------------------------------------------------------------
                                    186 ;Allocation info for local variables in function 'putchar'
                                    187 ;------------------------------------------------------------
                                    188 ;c                         Allocated to registers r7 
                                    189 ;------------------------------------------------------------
                                    190 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:60: void putchar (char c)
                                    191 ;	-----------------------------------------
                                    192 ;	 function putchar
                                    193 ;	-----------------------------------------
      00007B                        194 _putchar:
      00007B AF 82            [24]  195 	mov	r7,dpl
                                    196 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:62: if((!SIO_SM0)&&(!SIO_SM1)) inituart(0xff);
      00007D 20 9F 0D         [24]  197 	jb	_SIO_SM0,00102$
      000080 20 9E 0A         [24]  198 	jb	_SIO_SM1,00102$
      000083 75 82 FF         [24]  199 	mov	dpl,#0xff
      000086 C0 07            [24]  200 	push	ar7
      000088 12 00 62         [24]  201 	lcall	_inituart
      00008B D0 07            [24]  202 	pop	ar7
      00008D                        203 00102$:
                                    204 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:63: if (c=='\n')
      00008D BF 0A 08         [24]  205 	cjne	r7,#0x0a,00109$
                                    206 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:65: while (!SIO_TI);
      000090                        207 00104$:
                                    208 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:66: SIO_TI=0;
      000090 10 99 02         [24]  209 	jbc	_SIO_TI,00136$
      000093 80 FB            [24]  210 	sjmp	00104$
      000095                        211 00136$:
                                    212 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:67: SIO_SBUF='\r';
      000095 75 99 0D         [24]  213 	mov	_SIO_SBUF,#0x0d
                                    214 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:69: while (!SIO_TI);
      000098                        215 00109$:
                                    216 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:70: SIO_TI=0;
      000098 10 99 02         [24]  217 	jbc	_SIO_TI,00137$
      00009B 80 FB            [24]  218 	sjmp	00109$
      00009D                        219 00137$:
                                    220 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:71: SIO_SBUF=c;
      00009D 8F 99            [24]  221 	mov	_SIO_SBUF,r7
      00009F 22               [24]  222 	ret
                                    223 ;------------------------------------------------------------
                                    224 ;Allocation info for local variables in function 'getchar'
                                    225 ;------------------------------------------------------------
                                    226 ;c                         Allocated to registers 
                                    227 ;------------------------------------------------------------
                                    228 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:74: char getchar (void)
                                    229 ;	-----------------------------------------
                                    230 ;	 function getchar
                                    231 ;	-----------------------------------------
      0000A0                        232 _getchar:
                                    233 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:78: if((!SIO_SM0)&&(!SIO_SM1)) inituart(0xff);
      0000A0 20 9F 09         [24]  234 	jb	_SIO_SM0,00104$
      0000A3 20 9E 06         [24]  235 	jb	_SIO_SM1,00104$
      0000A6 75 82 FF         [24]  236 	mov	dpl,#0xff
      0000A9 12 00 62         [24]  237 	lcall	_inituart
                                    238 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:80: while (!SIO_RI);
      0000AC                        239 00104$:
                                    240 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:81: SIO_RI=0;
      0000AC 10 98 02         [24]  241 	jbc	_SIO_RI,00123$
      0000AF 80 FB            [24]  242 	sjmp	00104$
      0000B1                        243 00123$:
                                    244 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:82: c=SIO_SBUF;
      0000B1 85 99 82         [24]  245 	mov	dpl,_SIO_SBUF
                                    246 ;	C:/Program Files (x86)/SDCC/bin/../include/mcs51/serial_IO.h:83: return c;
      0000B4 22               [24]  247 	ret
                                    248 ;------------------------------------------------------------
                                    249 ;Allocation info for local variables in function 'main'
                                    250 ;------------------------------------------------------------
                                    251 ;input                     Allocated to stack - _bp +4
                                    252 ;key                       Allocated to stack - _bp +20
                                    253 ;sloc0                     Allocated to stack - _bp +1
                                    254 ;------------------------------------------------------------
                                    255 ;	main.c:16: void main(void) 
                                    256 ;	-----------------------------------------
                                    257 ;	 function main
                                    258 ;	-----------------------------------------
      0000B5                        259 _main:
      0000B5 C0 08            [24]  260 	push	_bp
      0000B7 E5 81            [12]  261 	mov	a,sp
      0000B9 F5 08            [12]  262 	mov	_bp,a
      0000BB 24 23            [12]  263 	add	a,#0x23
      0000BD F5 81            [12]  264 	mov	sp,a
                                    265 ;	main.c:24: inituart(248);
      0000BF 75 82 F8         [24]  266 	mov	dpl,#0xf8
      0000C2 12 00 62         [24]  267 	lcall	_inituart
                                    268 ;	main.c:27: putchar('\n');
      0000C5 75 82 0A         [24]  269 	mov	dpl,#0x0a
      0000C8 12 00 7B         [24]  270 	lcall	_putchar
                                    271 ;	main.c:30: while(1)
      0000CB                        272 00104$:
                                    273 ;	main.c:33: if(simpleserial_get(input, key))
      0000CB E5 08            [12]  274 	mov	a,_bp
      0000CD 24 14            [12]  275 	add	a,#0x14
      0000CF FF               [12]  276 	mov	r7,a
      0000D0 A8 08            [24]  277 	mov	r0,_bp
      0000D2 08               [12]  278 	inc	r0
      0000D3 A6 07            [24]  279 	mov	@r0,ar7
      0000D5 08               [12]  280 	inc	r0
      0000D6 76 00            [12]  281 	mov	@r0,#0x00
      0000D8 08               [12]  282 	inc	r0
      0000D9 76 40            [12]  283 	mov	@r0,#0x40
      0000DB E5 08            [12]  284 	mov	a,_bp
      0000DD 24 04            [12]  285 	add	a,#0x04
      0000DF FB               [12]  286 	mov	r3,a
      0000E0 FA               [12]  287 	mov	r2,a
      0000E1 7D 00            [12]  288 	mov	r5,#0x00
      0000E3 7E 40            [12]  289 	mov	r6,#0x40
      0000E5 C0 07            [24]  290 	push	ar7
      0000E7 C0 03            [24]  291 	push	ar3
      0000E9 A8 08            [24]  292 	mov	r0,_bp
      0000EB 08               [12]  293 	inc	r0
      0000EC E6               [12]  294 	mov	a,@r0
      0000ED C0 E0            [24]  295 	push	acc
      0000EF 08               [12]  296 	inc	r0
      0000F0 E6               [12]  297 	mov	a,@r0
      0000F1 C0 E0            [24]  298 	push	acc
      0000F3 08               [12]  299 	inc	r0
      0000F4 E6               [12]  300 	mov	a,@r0
      0000F5 C0 E0            [24]  301 	push	acc
      0000F7 8A 82            [24]  302 	mov	dpl,r2
      0000F9 8D 83            [24]  303 	mov	dph,r5
      0000FB 8E F0            [24]  304 	mov	b,r6
      0000FD 12 03 A0         [24]  305 	lcall	_simpleserial_get
      000100 AD 82            [24]  306 	mov	r5,dpl
      000102 AE 83            [24]  307 	mov	r6,dph
      000104 15 81            [12]  308 	dec	sp
      000106 15 81            [12]  309 	dec	sp
      000108 15 81            [12]  310 	dec	sp
      00010A D0 03            [24]  311 	pop	ar3
      00010C D0 07            [24]  312 	pop	ar7
      00010E ED               [12]  313 	mov	a,r5
      00010F 4E               [12]  314 	orl	a,r6
      000110 60 B9            [24]  315 	jz	00104$
                                    316 ;	main.c:36: xor_encrypt(input, key);
      000112 A8 08            [24]  317 	mov	r0,_bp
      000114 08               [12]  318 	inc	r0
      000115 A6 07            [24]  319 	mov	@r0,ar7
      000117 08               [12]  320 	inc	r0
      000118 76 00            [12]  321 	mov	@r0,#0x00
      00011A 08               [12]  322 	inc	r0
      00011B 76 40            [12]  323 	mov	@r0,#0x40
      00011D 8B 04            [24]  324 	mov	ar4,r3
      00011F 8C 02            [24]  325 	mov	ar2,r4
      000121 7C 00            [12]  326 	mov	r4,#0x00
      000123 7F 40            [12]  327 	mov	r7,#0x40
      000125 C0 03            [24]  328 	push	ar3
      000127 A8 08            [24]  329 	mov	r0,_bp
      000129 08               [12]  330 	inc	r0
      00012A E6               [12]  331 	mov	a,@r0
      00012B C0 E0            [24]  332 	push	acc
      00012D 08               [12]  333 	inc	r0
      00012E E6               [12]  334 	mov	a,@r0
      00012F C0 E0            [24]  335 	push	acc
      000131 08               [12]  336 	inc	r0
      000132 E6               [12]  337 	mov	a,@r0
      000133 C0 E0            [24]  338 	push	acc
      000135 8A 82            [24]  339 	mov	dpl,r2
      000137 8C 83            [24]  340 	mov	dph,r4
      000139 8F F0            [24]  341 	mov	b,r7
      00013B 12 04 D9         [24]  342 	lcall	_xor_encrypt
      00013E 15 81            [12]  343 	dec	sp
      000140 15 81            [12]  344 	dec	sp
      000142 15 81            [12]  345 	dec	sp
      000144 D0 03            [24]  346 	pop	ar3
                                    347 ;	main.c:37: simpleserial_put(input);
      000146 7F 00            [12]  348 	mov	r7,#0x00
      000148 7E 40            [12]  349 	mov	r6,#0x40
      00014A 8B 82            [24]  350 	mov	dpl,r3
      00014C 8F 83            [24]  351 	mov	dph,r7
      00014E 8E F0            [24]  352 	mov	b,r6
      000150 12 04 7F         [24]  353 	lcall	_simpleserial_put
      000153 02 00 CB         [24]  354 	ljmp	00104$
      000156 85 08 81         [24]  355 	mov	sp,_bp
      000159 D0 08            [24]  356 	pop	_bp
      00015B 22               [24]  357 	ret
                                    358 	.area CSEG    (CODE)
                                    359 	.area CONST   (CODE)
                                    360 	.area XINIT   (CODE)
                                    361 	.area CABS    (ABS,CODE)
