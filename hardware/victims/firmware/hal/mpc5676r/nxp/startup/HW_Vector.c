/****************************
# HW_Vector.c - INTC hardware vector mode branch table example
# Description: INTC vector branch table when using INTC in HW vector mode
#            **** NOTE **** ONLY 100 EXAMPLE VECTORS ARE IMPLEMENTED HERE
# Rev 1.0 May 26, 2016 D Chung
# Copyright NXP Semiconductor, Inc. 2016. All rights reserved
*****************************/

/* 
#include "compiler_api.h"


#define PPCASM   __asm__
#define VEC_ALIGN_4B PPCASM (" .align 4 ")
#define PPCASMF(x) PPCASM (" " tostring(x) " ")
#define PPCASMF2(x,x1) PPCASM (" " tostring(x) "," tostring(x1) " ")

PPCASMF( .globl IntcHandlerBranchTable );
PPCASMF( .globl hw_vect0 );
PPCASMF( .globl hw_vect1 );
PPCASMF( .globl hw_vect2 );
PPCASMF( .globl hw_vect3 );
PPCASMF( .globl hw_vect4 );
PPCASMF( .globl hw_vect5 );
PPCASMF( .globl hw_vect6 );
PPCASMF( .globl hw_vect7 );
PPCASMF( .globl hw_vect8 );
PPCASMF( .globl hw_vect9 );
PPCASMF( .globl hw_vect10 );
PPCASMF( .globl hw_vect11 );
PPCASMF( .globl hw_vect12 );
PPCASMF( .globl hw_vect13 );
PPCASMF( .globl hw_vect14 );
PPCASMF( .globl hw_vect15 );
PPCASMF( .globl hw_vect16 );
PPCASMF( .globl hw_vect17 );
PPCASMF( .globl hw_vect18 );
PPCASMF( .globl hw_vect19 );
PPCASMF( .globl hw_vect20 );
PPCASMF( .globl hw_vect21 );
PPCASMF( .globl hw_vect22 );
PPCASMF( .globl hw_vect23 );
PPCASMF( .globl hw_vect24 );
PPCASMF( .globl hw_vect25 );
PPCASMF( .globl hw_vect26 );
PPCASMF( .globl hw_vect27 );
PPCASMF( .globl hw_vect28 );
PPCASMF( .globl hw_vect29 );
PPCASMF( .globl hw_vect30 );
PPCASMF( .globl hw_vect31 );
PPCASMF( .globl hw_vect32 );
PPCASMF( .globl hw_vect33 );
PPCASMF( .globl hw_vect34 );
PPCASMF( .globl hw_vect35 );
PPCASMF( .globl hw_vect36 );
PPCASMF( .globl hw_vect37 );
PPCASMF( .globl hw_vect38 );
PPCASMF( .globl hw_vect39 );
PPCASMF( .globl hw_vect40 );
PPCASMF( .globl hw_vect41 );
PPCASMF( .globl hw_vect42 );
PPCASMF( .globl hw_vect43 );
PPCASMF( .globl hw_vect44 );
PPCASMF( .globl hw_vect45 );
PPCASMF( .globl hw_vect46 );
PPCASMF( .globl hw_vect47 );
PPCASMF( .globl hw_vect48 );
PPCASMF( .globl hw_vect49 );
PPCASMF( .globl hw_vect50 );
PPCASMF( .globl hw_vect51 );
PPCASMF( .globl hw_vect52 );
PPCASMF( .globl hw_vect53 );
PPCASMF( .globl hw_vect54 );
PPCASMF( .globl hw_vect55 );
PPCASMF( .globl hw_vect56 );
PPCASMF( .globl hw_vect57 );
PPCASMF( .globl hw_vect58 );
PPCASMF( .globl hw_vect59 );
PPCASMF( .globl hw_vect60 );
PPCASMF( .globl hw_vect61 );
PPCASMF( .globl hw_vect62 );
PPCASMF( .globl hw_vect63 );
PPCASMF( .globl hw_vect64 );
PPCASMF( .globl hw_vect65 );
PPCASMF( .globl hw_vect66 );
PPCASMF( .globl hw_vect67 );
PPCASMF( .globl hw_vect68 );
PPCASMF( .globl hw_vect69 );
PPCASMF( .globl hw_vect70 );
PPCASMF( .globl hw_vect71 );
PPCASMF( .globl hw_vect72 );
PPCASMF( .globl hw_vect73 );
PPCASMF( .globl hw_vect74 );
PPCASMF( .globl hw_vect75 );
PPCASMF( .globl hw_vect76 );
PPCASMF( .globl hw_vect77 );
PPCASMF( .globl hw_vect78 );
PPCASMF( .globl hw_vect79 );
PPCASMF( .globl hw_vect80 );
PPCASMF( .globl hw_vect81 );
PPCASMF( .globl hw_vect82 );
PPCASMF( .globl hw_vect83 );
PPCASMF( .globl hw_vect84 );
PPCASMF( .globl hw_vect85 );
PPCASMF( .globl hw_vect86 );
PPCASMF( .globl hw_vect87 );
PPCASMF( .globl hw_vect88 );
PPCASMF( .globl hw_vect89 );
PPCASMF( .globl hw_vect90 );
PPCASMF( .globl hw_vect91 );
PPCASMF( .globl hw_vect92 );
PPCASMF( .globl hw_vect93 );
PPCASMF( .globl hw_vect94 );
PPCASMF( .globl hw_vect95 );
PPCASMF( .globl hw_vect96 );
PPCASMF( .globl hw_vect97 );
PPCASMF( .globl hw_vect98 );
PPCASMF( .globl hw_vect99 );
PPCASMF( .globl hw_vect100 );
PPCASMF( .globl hw_vect101 );
PPCASMF( .globl hw_vect102 );
PPCASMF( .globl hw_vect103 );
PPCASMF( .globl hw_vect104 );
PPCASMF( .globl hw_vect105 );
PPCASMF( .globl hw_vect106 );
PPCASMF( .globl hw_vect107 );
PPCASMF( .globl hw_vect108 );
PPCASMF( .globl hw_vect109 );
PPCASMF( .globl hw_vect110 );
PPCASMF( .globl hw_vect111 );
PPCASMF( .globl hw_vect112 );
PPCASMF( .globl hw_vect113 );
PPCASMF( .globl hw_vect114 );
PPCASMF( .globl hw_vect115 );
PPCASMF( .globl hw_vect116 );
PPCASMF( .globl hw_vect117 );
PPCASMF( .globl hw_vect118 );
PPCASMF( .globl hw_vect119 );
PPCASMF( .globl hw_vect120 );
PPCASMF( .globl hw_vect121 );
PPCASMF( .globl hw_vect122 );
PPCASMF( .globl hw_vect123 );
PPCASMF( .globl hw_vect124 );
PPCASMF( .globl hw_vect125 );
PPCASMF( .globl hw_vect126 );
PPCASMF( .globl hw_vect127 );
PPCASMF( .globl hw_vect128 );
PPCASMF( .globl hw_vect129 );
PPCASMF( .globl hw_vect130 );
PPCASMF( .globl hw_vect131 );
PPCASMF( .globl hw_vect132 );
PPCASMF( .globl hw_vect133 );
PPCASMF( .globl hw_vect134 );
PPCASMF( .globl hw_vect135 );
PPCASMF( .globl hw_vect136 );
PPCASMF( .globl hw_vect137 );
PPCASMF( .globl hw_vect138 );
PPCASMF( .globl hw_vect139 );
PPCASMF( .globl hw_vect140 );
PPCASMF( .globl hw_vect141 );
PPCASMF( .globl hw_vect142 );
PPCASMF( .globl hw_vect143 );
PPCASMF( .globl hw_vect144 );
PPCASMF( .globl hw_vect145 );
PPCASMF( .globl hw_vect146 );
PPCASMF( .globl hw_vect147 );
PPCASMF( .globl hw_vect148 );
PPCASMF( .globl hw_vect149 );
PPCASMF( .globl hw_vect150 );
PPCASMF( .globl hw_vect151 );
PPCASMF( .globl hw_vect152 );
PPCASMF( .globl hw_vect153 );
PPCASMF( .globl hw_vect154 );
PPCASMF( .globl hw_vect155 );
PPCASMF( .globl hw_vect156 );
PPCASMF( .globl hw_vect157 );
PPCASMF( .globl hw_vect158 );
PPCASMF( .globl hw_vect159 );
PPCASMF( .globl hw_vect160 );
PPCASMF( .globl hw_vect161 );
PPCASMF( .globl hw_vect162 );
PPCASMF( .globl hw_vect163 );
PPCASMF( .globl hw_vect164 );
PPCASMF( .globl hw_vect165 );
PPCASMF( .globl hw_vect166 );
PPCASMF( .globl hw_vect167 );
PPCASMF( .globl hw_vect168 );
PPCASMF( .globl hw_vect169 );
PPCASMF( .globl hw_vect170 );
PPCASMF( .globl hw_vect171 );
PPCASMF( .globl hw_vect172 );
PPCASMF( .globl hw_vect173 );
PPCASMF( .globl hw_vect174 );
PPCASMF( .globl hw_vect175 );
PPCASMF( .globl hw_vect176 );
PPCASMF( .globl hw_vect177 );
PPCASMF( .globl hw_vect178 );
PPCASMF( .globl hw_vect179 );
PPCASMF( .globl hw_vect180 );
PPCASMF( .globl hw_vect181 );
PPCASMF( .globl hw_vect182 );
PPCASMF( .globl hw_vect183 );
PPCASMF( .globl hw_vect184 );
PPCASMF( .globl hw_vect185 );
PPCASMF( .globl hw_vect186 );
PPCASMF( .globl hw_vect187 );
PPCASMF( .globl hw_vect188 );
PPCASMF( .globl hw_vect189 );
PPCASMF( .globl hw_vect190 );
PPCASMF( .globl hw_vect191 );
PPCASMF( .globl hw_vect192 );
PPCASMF( .globl hw_vect193 );
PPCASMF( .globl hw_vect194 );
PPCASMF( .globl hw_vect195 );
PPCASMF( .globl hw_vect196 );
PPCASMF( .globl hw_vect197 );
PPCASMF( .globl hw_vect198 );
PPCASMF( .globl hw_vect199 );
PPCASMF( .globl hw_vect200 );
PPCASMF( .globl hw_vect100 );
PPCASMF( .globl hw_vect201 );
PPCASMF( .globl hw_vect202 );
PPCASMF( .globl hw_vect203 );
PPCASMF( .globl hw_vect204 );
PPCASMF( .globl hw_vect205 );
PPCASMF( .globl hw_vect206 );
PPCASMF( .globl hw_vect207 );
PPCASMF( .globl hw_vect208 );
PPCASMF( .globl hw_vect209 );
PPCASMF( .globl hw_vect210 );
PPCASMF( .globl hw_vect211 );
PPCASMF( .globl hw_vect212 );
PPCASMF( .globl hw_vect213 );
PPCASMF( .globl hw_vect214 );
PPCASMF( .globl hw_vect215 );
PPCASMF( .globl hw_vect216 );
PPCASMF( .globl hw_vect217 );
PPCASMF( .globl hw_vect218 );
PPCASMF( .globl hw_vect219 );
PPCASMF( .globl hw_vect220 );
PPCASMF( .globl hw_vect221 );
PPCASMF( .globl hw_vect222 );
PPCASMF( .globl hw_vect223 );
PPCASMF( .globl hw_vect224 );
PPCASMF( .globl hw_vect225 );
PPCASMF( .globl hw_vect226 );
PPCASMF( .globl hw_vect227 );
PPCASMF( .globl hw_vect228 );
PPCASMF( .globl hw_vect229 );
PPCASMF( .globl hw_vect230 );
PPCASMF( .globl hw_vect231 );
PPCASMF( .globl hw_vect232 );
PPCASMF( .globl hw_vect233 );
PPCASMF( .globl hw_vect234 );
PPCASMF( .globl hw_vect235 );
PPCASMF( .globl hw_vect236 );
PPCASMF( .globl hw_vect237 );
PPCASMF( .globl hw_vect238 );
PPCASMF( .globl hw_vect239 );
PPCASMF( .globl hw_vect240 );
PPCASMF( .globl hw_vect241 );
PPCASMF( .globl hw_vect242 );
PPCASMF( .globl hw_vect243 );
PPCASMF( .globl hw_vect244 );
PPCASMF( .globl hw_vect245 );
PPCASMF( .globl hw_vect246 );
PPCASMF( .globl hw_vect247 );
PPCASMF( .globl hw_vect248 );
PPCASMF( .globl hw_vect249 );
PPCASMF( .globl hw_vect250 );
PPCASMF( .globl hw_vect251 );
PPCASMF( .globl hw_vect252 );
PPCASMF( .globl hw_vect253 );
PPCASMF( .globl hw_vect254 );
PPCASMF( .globl hw_vect255 );
PPCASMF( .globl hw_vect256 );
PPCASMF( .globl hw_vect257 );
PPCASMF( .globl hw_vect258 );
PPCASMF( .globl hw_vect259 );
PPCASMF( .globl hw_vect260 );
PPCASMF( .globl hw_vect261 );
PPCASMF( .globl hw_vect262 );
PPCASMF( .globl hw_vect263 );
PPCASMF( .globl hw_vect264 );
PPCASMF( .globl hw_vect265 );
PPCASMF( .globl hw_vect266 );
PPCASMF( .globl hw_vect267 );
PPCASMF( .globl hw_vect268 );
PPCASMF( .globl hw_vect269 );
PPCASMF( .globl hw_vect270 );
PPCASMF( .globl hw_vect271 );
PPCASMF( .globl hw_vect272 );
PPCASMF( .globl hw_vect273 );
PPCASMF( .globl hw_vect274 );
PPCASMF( .globl hw_vect275 );
PPCASMF( .globl hw_vect276 );
PPCASMF( .globl hw_vect277 );
PPCASMF( .globl hw_vect278 );
PPCASMF( .globl hw_vect279 );
PPCASMF( .globl hw_vect280 );
PPCASMF( .globl hw_vect281 );
PPCASMF( .globl hw_vect282 );
PPCASMF( .globl hw_vect283 );
PPCASMF( .globl hw_vect284 );
PPCASMF( .globl hw_vect285 );
PPCASMF( .globl hw_vect286 );
PPCASMF( .globl hw_vect287 );
PPCASMF( .globl hw_vect288 );
PPCASMF( .globl hw_vect289 );
PPCASMF( .globl hw_vect290 );
PPCASMF( .globl hw_vect291 );
PPCASMF( .globl hw_vect292 );
PPCASMF( .globl hw_vect293 );
PPCASMF( .globl hw_vect294 );
PPCASMF( .globl hw_vect295 );
PPCASMF( .globl hw_vect296 );
PPCASMF( .globl hw_vect297 );
PPCASMF( .globl hw_vect298 );
PPCASMF( .globl hw_vect299 );
PPCASMF( .globl hw_vect300 );
PPCASMF( .globl hw_vect301 );
PPCASMF( .globl hw_vect302 );
PPCASMF( .globl hw_vect303 );
PPCASMF( .globl hw_vect304 );



PPCASMF( .extern     Pit3Handler);

PPCASMF2( .section    .intc_hw_branch_table, "ax" );

VEC_ALIGN_4B;
PPCASMF( IntcHandlerBranchTable: );
PPCASMF( hw_vect0: );				//Hardware Vector 0
PPCASMF( e_b   hw_vect0 );

VEC_ALIGN_4B;
PPCASMF( hw_vect1: );				//Hardware Vector 1
PPCASMF( e_b   hw_vect1 );

VEC_ALIGN_4B;
PPCASMF( hw_vect2: );				//Hardware Vector 2
PPCASMF( e_b   hw_vect2 );

VEC_ALIGN_4B;
PPCASMF( hw_vect3: );				//Hardware Vector 3
PPCASMF( e_b   hw_vect3 );

VEC_ALIGN_4B;
PPCASMF( hw_vect4: );				//Hardware Vector 4
PPCASMF( e_b  hw_vect4 );

VEC_ALIGN_4B;
PPCASMF( hw_vect5: );				//Hardware Vector 5
PPCASMF( e_b   hw_vect5 );

VEC_ALIGN_4B;
PPCASMF( hw_vect6: );				//Hardware Vector 6
PPCASMF( e_b   hw_vect6 );

VEC_ALIGN_4B;

PPCASMF( hw_vect7: );				//Hardware Vector 7
PPCASMF( e_b   hw_vect7 );

VEC_ALIGN_4B;
PPCASMF( hw_vect8: );				//Hardware Vector 8
PPCASMF( e_b   hw_vect8 );

VEC_ALIGN_4B;
PPCASMF( hw_vect9: );				//Hardware Vector 9
PPCASMF( e_b   hw_vect9 );

VEC_ALIGN_4B;
PPCASMF( hw_vect10: );				//Hardware Vector 10
PPCASMF( e_b   hw_vect10 );

VEC_ALIGN_4B;
PPCASMF( hw_vect11: );				//Hardware Vector 11
PPCASMF( e_b   hw_vect11 );

VEC_ALIGN_4B;
PPCASMF( hw_vect12: );				//Hardware Vector 12
PPCASMF( e_b   hw_vect12 );

VEC_ALIGN_4B;
PPCASMF( hw_vect13: );				//Hardware Vector 13
PPCASMF( e_b   hw_vect13 );

VEC_ALIGN_4B;
PPCASMF( hw_vect14: );				//Hardware Vector 14
PPCASMF( e_b   hw_vect14 );

VEC_ALIGN_4B;
PPCASMF( hw_vect15: );				//Hardware Vector 15
PPCASMF( e_b   hw_vect15 );

VEC_ALIGN_4B;
PPCASMF( hw_vect16: );				//Hardware Vector 16
PPCASMF( e_b   hw_vect16 );

VEC_ALIGN_4B;
PPCASMF( hw_vect17: );				//Hardware Vector 17
PPCASMF( e_b   hw_vect17 );

VEC_ALIGN_4B;
PPCASMF( hw_vect18: );				//Hardware Vector 18
PPCASMF( e_b   hw_vect18 );

VEC_ALIGN_4B;
PPCASMF( hw_vect19: );				//Hardware Vector 19
PPCASMF( e_b   hw_vect19 );

VEC_ALIGN_4B;
PPCASMF( hw_vect20: );				//Hardware Vector 20
PPCASMF( e_b   hw_vect20 );

VEC_ALIGN_4B;
PPCASMF( hw_vect21: );				//Hardware Vector 21
PPCASMF( e_b   hw_vect21 );

VEC_ALIGN_4B;
PPCASMF( hw_vect22: );				//Hardware Vector 22
PPCASMF( e_b   hw_vect22 );

VEC_ALIGN_4B;
PPCASMF( hw_vect23: );				//Hardware Vector 23
PPCASMF( e_b   hw_vect23 );

VEC_ALIGN_4B;
PPCASMF( hw_vect24: );				//Hardware Vector 24
PPCASMF( e_b   hw_vect24 );

VEC_ALIGN_4B;
PPCASMF( hw_vect25: );				//Hardware Vector 25
PPCASMF( e_b   hw_vect25 );

VEC_ALIGN_4B;
PPCASMF( hw_vect26: );				//Hardware Vector 26
PPCASMF( e_b   hw_vect26 );

VEC_ALIGN_4B;
PPCASMF( hw_vect27: );				//Hardware Vector 27
PPCASMF( e_b   hw_vect27 );

VEC_ALIGN_4B;
PPCASMF( hw_vect28: );				//Hardware Vector 28
PPCASMF( e_b   hw_vect28 );

VEC_ALIGN_4B;
PPCASMF( hw_vect29: );				//Hardware Vector 29
PPCASMF( e_b   hw_vect29 );

VEC_ALIGN_4B;
PPCASMF( hw_vect30: );				//Hardware Vector 30
PPCASMF( e_b   hw_vect30 );

VEC_ALIGN_4B;
PPCASMF( hw_vect31: );				//Hardware Vector 31
PPCASMF( e_b   hw_vect31 );

VEC_ALIGN_4B;
PPCASMF( hw_vect32: );				//Hardware Vector 32
PPCASMF( e_b   hw_vect32 );

VEC_ALIGN_4B;
PPCASMF( hw_vect33: );				//Hardware Vector 33
PPCASMF( e_b   hw_vect33 );

VEC_ALIGN_4B;
PPCASMF( hw_vect34: );				//Hardware Vector 34
PPCASMF( e_b   hw_vect34 );

VEC_ALIGN_4B;
PPCASMF( hw_vect35: );				//Hardware Vector 35
PPCASMF( e_b   hw_vect35 );

VEC_ALIGN_4B;
PPCASMF( hw_vect36: );				//Hardware Vector 36
PPCASMF( e_b   hw_vect36 );

VEC_ALIGN_4B;
PPCASMF( hw_vect37: );				//Hardware Vector 37
PPCASMF( e_b   hw_vect37 );

VEC_ALIGN_4B;
PPCASMF( hw_vect38: );				//Hardware Vector 38
PPCASMF( e_b   hw_vect38 );

VEC_ALIGN_4B;
PPCASMF( hw_vect39: );				//Hardware Vector 39
PPCASMF( e_b   hw_vect39 );

VEC_ALIGN_4B;
PPCASMF( hw_vect40: );				//Hardware Vector 40
PPCASMF( e_b   hw_vect40 );

VEC_ALIGN_4B;
PPCASMF( hw_vect41: );				//Hardware Vector 41
PPCASMF( e_b   hw_vect41 );

VEC_ALIGN_4B;
PPCASMF( hw_vect42: );				//Hardware Vector 42
PPCASMF( e_b   hw_vect42 );

VEC_ALIGN_4B;
PPCASMF( hw_vect43: );				//Hardware Vector 43
PPCASMF( e_b   hw_vect43 );

VEC_ALIGN_4B;
PPCASMF( hw_vect44: );				//Hardware Vector 44
PPCASMF( e_b   hw_vect44 );

VEC_ALIGN_4B;
PPCASMF( hw_vect45: );				//Hardware Vector 45
PPCASMF( e_b   hw_vect45 );

VEC_ALIGN_4B;
PPCASMF( hw_vect46: );				//Hardware Vector 46
PPCASMF( e_b   hw_vect46 );

VEC_ALIGN_4B;
PPCASMF( hw_vect47: );				//Hardware Vector 47
PPCASMF( e_b   hw_vect47 );

VEC_ALIGN_4B;
PPCASMF( hw_vect48: );				//Hardware Vector 48
PPCASMF( e_b   hw_vect48 );

VEC_ALIGN_4B;
PPCASMF( hw_vect49: );				//Hardware Vector 49
PPCASMF( e_b   hw_vect49 );

VEC_ALIGN_4B;
PPCASMF( hw_vect50: );				//Hardware Vector 50
PPCASMF( e_b   hw_vect50 );

VEC_ALIGN_4B;
PPCASMF( hw_vect51: );				//Hardware Vector 51
PPCASMF( e_b   hw_vect51 );

VEC_ALIGN_4B;
PPCASMF( hw_vect52: );				//Hardware Vector 52
PPCASMF( e_b   hw_vect52 );

VEC_ALIGN_4B;
PPCASMF( hw_vect53: );				//Hardware Vector 53
PPCASMF( e_b   hw_vect53 );

VEC_ALIGN_4B;
PPCASMF( hw_vect54: );				//Hardware Vector 54
PPCASMF( e_b   hw_vect54 );

VEC_ALIGN_4B;
PPCASMF( hw_vect55: );				//Hardware Vector 55
PPCASMF( e_b   hw_vect55 );

VEC_ALIGN_4B;
PPCASMF( hw_vect56: );				//Hardware Vector 56
PPCASMF( e_b   hw_vect56 );

VEC_ALIGN_4B;
PPCASMF( hw_vect57: );				//Hardware Vector 57
PPCASMF( e_b   hw_vect57 );

VEC_ALIGN_4B;
PPCASMF( hw_vect58: );				//Hardware Vector 58
PPCASMF( e_b   hw_vect58 );

VEC_ALIGN_4B;
PPCASMF( hw_vect59: );				//Hardware Vector 59
PPCASMF( e_b	hw_vect59 );

VEC_ALIGN_4B;
PPCASMF( hw_vect60: );				//Hardware Vector 60
PPCASMF( e_b	hw_vect60 );

VEC_ALIGN_4B;
PPCASMF( hw_vect61: );				//Hardware Vector 61
PPCASMF( e_b   hw_vect61 );

VEC_ALIGN_4B;
PPCASMF( hw_vect62: );				//Hardware Vector 62
PPCASMF( e_b   hw_vect62 );

VEC_ALIGN_4B;
PPCASMF( hw_vect63: );				//Hardware Vector 63
PPCASMF( e_b   hw_vect63 );

VEC_ALIGN_4B;
PPCASMF( hw_vect64: );				//Hardware Vector 64
PPCASMF( e_b   hw_vect64 );

VEC_ALIGN_4B;
PPCASMF( hw_vect65: );				//Hardware Vector 65
PPCASMF( e_b   hw_vect65 );

VEC_ALIGN_4B;
PPCASMF( hw_vect66: );				//Hardware Vector 66
PPCASMF( e_b   hw_vect66 );

VEC_ALIGN_4B;
PPCASMF( hw_vect67: );				//Hardware Vector 67
PPCASMF( e_b   hw_vect67 );

VEC_ALIGN_4B;
PPCASMF( hw_vect68: );				//Hardware Vector 68
PPCASMF( e_b   hw_vect68 );

VEC_ALIGN_4B;
PPCASMF( hw_vect69: );				//Hardware Vector 69
PPCASMF( e_b   hw_vect69 );

VEC_ALIGN_4B;
PPCASMF( hw_vect70: );				//Hardware Vector 70
PPCASMF( e_b   hw_vect70 );

VEC_ALIGN_4B;
PPCASMF( hw_vect71: );				//Hardware Vector 71
PPCASMF( e_b   hw_vect71 );

VEC_ALIGN_4B;
PPCASMF( hw_vect72: );				//Hardware Vector 72
PPCASMF( e_b   hw_vect72 );

VEC_ALIGN_4B;
PPCASMF( hw_vect73: );				//Hardware Vector 73
PPCASMF( e_b   hw_vect73 );

VEC_ALIGN_4B;
PPCASMF( hw_vect74: );				//Hardware Vector 74
PPCASMF( e_b   hw_vect74 );

VEC_ALIGN_4B;
PPCASMF( hw_vect75: );				//Hardware Vector 75
PPCASMF( e_b   hw_vect75 );

VEC_ALIGN_4B;
PPCASMF( hw_vect76: );				//Hardware Vector 76
PPCASMF( e_b   hw_vect76 );

VEC_ALIGN_4B;
PPCASMF( hw_vect77: );				//Hardware Vector 77
PPCASMF( e_b   hw_vect77 );

VEC_ALIGN_4B;
PPCASMF( hw_vect78: );				//Hardware Vector 78
PPCASMF( e_b   hw_vect78 );

VEC_ALIGN_4B;
PPCASMF( hw_vect79: );				//Hardware Vector 79
PPCASMF( e_b   hw_vect79 );

VEC_ALIGN_4B;
PPCASMF( hw_vect80: );				//Hardware Vector 80
PPCASMF( e_b   hw_vect80 );

VEC_ALIGN_4B;
PPCASMF( hw_vect81: );				//Hardware Vector 81
PPCASMF( e_b   hw_vect81 );

VEC_ALIGN_4B;
PPCASMF( hw_vect82: );				//Hardware Vector 82
PPCASMF( e_b   hw_vect82 );

VEC_ALIGN_4B;
PPCASMF( hw_vect83: );				//Hardware Vector 83
PPCASMF( e_b   hw_vect83 );

VEC_ALIGN_4B;
PPCASMF( hw_vect84: );				//Hardware Vector 84
PPCASMF( e_b   hw_vect84 );

VEC_ALIGN_4B;
PPCASMF( hw_vect85: );				//Hardware Vector 85
PPCASMF( e_b   hw_vect85 );

VEC_ALIGN_4B;
PPCASMF( hw_vect86: );				//Hardware Vector 86
PPCASMF( e_b   hw_vect86 );

VEC_ALIGN_4B;
PPCASMF( hw_vect87: );				//Hardware Vector 87
PPCASMF( e_b   hw_vect87 );

VEC_ALIGN_4B;
PPCASMF( hw_vect88: );				//Hardware Vector 88
PPCASMF( e_b   hw_vect88 );

VEC_ALIGN_4B;
PPCASMF( hw_vect89: );				//Hardware Vector 89
PPCASMF( e_b   hw_vect89 );

VEC_ALIGN_4B;
PPCASMF( hw_vect90: );				//Hardware Vector 90
PPCASMF( e_b   hw_vect90 );

VEC_ALIGN_4B;
PPCASMF( hw_vect91: );				//Hardware Vector 91
PPCASMF( e_b   hw_vect91 );

VEC_ALIGN_4B;
PPCASMF( hw_vect92: );				//Hardware Vector 92
PPCASMF( e_b   hw_vect92 );

VEC_ALIGN_4B;
PPCASMF( hw_vect93: );				//Hardware Vector 93
PPCASMF( e_b   hw_vect93 );

VEC_ALIGN_4B;
PPCASMF( hw_vect94: );				//Hardware Vector 94
PPCASMF( e_b   hw_vect94 );

VEC_ALIGN_4B;
PPCASMF( hw_vect95: );				//Hardware Vector 95
PPCASMF( e_b   hw_vect95 );

VEC_ALIGN_4B;
PPCASMF( hw_vect96: );				//Hardware Vector 96
PPCASMF( e_b   hw_vect96 );

VEC_ALIGN_4B;
PPCASMF( hw_vect97: );				//Hardware Vector 97
PPCASMF( e_b   hw_vect97 );

VEC_ALIGN_4B;
PPCASMF( hw_vect98: );				//Hardware Vector 98
PPCASMF( e_b   hw_vect98 );

VEC_ALIGN_4B;
PPCASMF( hw_vect99: );				//Hardware Vector 99
PPCASMF( e_b   hw_vect99 );

VEC_ALIGN_4B;
PPCASMF( hw_vect100: );				//Hardware Vector 100
PPCASMF( e_b   hw_vect100 );

VEC_ALIGN_4B;
PPCASMF( hw_vect101: );				//Hardware Vector 100
PPCASMF( e_b   hw_vect101 );

VEC_ALIGN_4B;
PPCASMF( hw_vect102: );				//Hardware Vector 102
PPCASMF( e_b   hw_vect102 );

VEC_ALIGN_4B;
PPCASMF( hw_vect103: );				//Hardware Vector 103
PPCASMF( e_b   hw_vect103 );

VEC_ALIGN_4B;
PPCASMF( hw_vect104: );				//Hardware Vector 104
PPCASMF( e_b  hw_vect104 );

VEC_ALIGN_4B;
PPCASMF( hw_vect105: );				//Hardware Vector 105
PPCASMF( e_b   hw_vect105 );

VEC_ALIGN_4B;
PPCASMF( hw_vect106: );				//Hardware Vector 106
PPCASMF( e_b   hw_vect106 );

VEC_ALIGN_4B;

PPCASMF( hw_vect107: );				//Hardware Vector 107
PPCASMF( e_b   hw_vect107 );

VEC_ALIGN_4B;
PPCASMF( hw_vect108: );				//Hardware Vector 108
PPCASMF( e_b   hw_vect108 );

VEC_ALIGN_4B;
PPCASMF( hw_vect109: );				//Hardware Vector 109
PPCASMF( e_b   hw_vect109 );

VEC_ALIGN_4B;
PPCASMF( hw_vect110: );				//Hardware Vector 110
PPCASMF( e_b   hw_vect110 );

VEC_ALIGN_4B;
PPCASMF( hw_vect111: );				//Hardware Vector 111
PPCASMF( e_b   hw_vect111 );

VEC_ALIGN_4B;
PPCASMF( hw_vect112: );				//Hardware Vector 112
PPCASMF( e_b   hw_vect112 );

VEC_ALIGN_4B;
PPCASMF( hw_vect113: );				//Hardware Vector 113
PPCASMF( e_b   hw_vect113 );

VEC_ALIGN_4B;
PPCASMF( hw_vect114: );				//Hardware Vector 114
PPCASMF( e_b   hw_vect114 );

VEC_ALIGN_4B;
PPCASMF( hw_vect115: );				//Hardware Vector 115
PPCASMF( e_b   hw_vect115 );

VEC_ALIGN_4B;
PPCASMF( hw_vect116: );				//Hardware Vector 116
PPCASMF( e_b   hw_vect116 );

VEC_ALIGN_4B;
PPCASMF( hw_vect117: );				//Hardware Vector 117
PPCASMF( e_b   hw_vect117 );

VEC_ALIGN_4B;
PPCASMF( hw_vect118: );				//Hardware Vector 118
PPCASMF( e_b   hw_vect118 );

VEC_ALIGN_4B;
PPCASMF( hw_vect119: );				//Hardware Vector 119
PPCASMF( e_b   hw_vect119 );

VEC_ALIGN_4B;
PPCASMF( hw_vect120: );				//Hardware Vector 120
PPCASMF( e_b   hw_vect120 );

VEC_ALIGN_4B;
PPCASMF( hw_vect121: );				//Hardware Vector 121
PPCASMF( e_b   hw_vect121 );

VEC_ALIGN_4B;
PPCASMF( hw_vect122: );				//Hardware Vector 122
PPCASMF( e_b   hw_vect122 );

VEC_ALIGN_4B;
PPCASMF( hw_vect123: );				//Hardware Vector 123
PPCASMF( e_b   hw_vect123 );

VEC_ALIGN_4B;
PPCASMF( hw_vect124: );				//Hardware Vector 124
PPCASMF( e_b   hw_vect124 );

VEC_ALIGN_4B;
PPCASMF( hw_vect125: );				//Hardware Vector 125
PPCASMF( e_b   hw_vect125 );

VEC_ALIGN_4B;
PPCASMF( hw_vect126: );				//Hardware Vector 126
PPCASMF( e_b   hw_vect126 );

VEC_ALIGN_4B;
PPCASMF( hw_vect127: );				//Hardware Vector 127
PPCASMF( e_b   hw_vect127 );

VEC_ALIGN_4B;
PPCASMF( hw_vect128: );				//Hardware Vector 128
PPCASMF( e_b   hw_vect128 );

VEC_ALIGN_4B;
PPCASMF( hw_vect129: );				//Hardware Vector 129
PPCASMF( e_b   hw_vect129 );

VEC_ALIGN_4B;
PPCASMF( hw_vect130: );				//Hardware Vector 130
PPCASMF( e_b   hw_vect130 );

VEC_ALIGN_4B;
PPCASMF( hw_vect131: );				//Hardware Vector 131
PPCASMF( e_b   hw_vect131 );

VEC_ALIGN_4B;
PPCASMF( hw_vect132: );				//Hardware Vector 132
PPCASMF( e_b   hw_vect132 );

VEC_ALIGN_4B;
PPCASMF( hw_vect133: );				//Hardware Vector 133
PPCASMF( e_b   hw_vect133 );

VEC_ALIGN_4B;
PPCASMF( hw_vect134: );				//Hardware Vector 134
PPCASMF( e_b   hw_vect134 );

VEC_ALIGN_4B;
PPCASMF( hw_vect135: );				//Hardware Vector 135
PPCASMF( e_b   hw_vect135 );

VEC_ALIGN_4B;
PPCASMF( hw_vect136: );				//Hardware Vector 136
PPCASMF( e_b   hw_vect136 );

VEC_ALIGN_4B;
PPCASMF( hw_vect137: );				//Hardware Vector 137
PPCASMF( e_b   hw_vect137 );

VEC_ALIGN_4B;
PPCASMF( hw_vect138: );				//Hardware Vector 138
PPCASMF( e_b   hw_vect138 );

VEC_ALIGN_4B;
PPCASMF( hw_vect139: );				//Hardware Vector 139
PPCASMF( e_b   hw_vect139 );

VEC_ALIGN_4B;
PPCASMF( hw_vect140: );				//Hardware Vector 140
PPCASMF( e_b   hw_vect140 );

VEC_ALIGN_4B;
PPCASMF( hw_vect141: );				//Hardware Vector 141
PPCASMF( e_b   hw_vect141 );

VEC_ALIGN_4B;
PPCASMF( hw_vect142: );				//Hardware Vector 142
PPCASMF( e_b   hw_vect142 );

VEC_ALIGN_4B;
PPCASMF( hw_vect143: );				//Hardware Vector 143
PPCASMF( e_b   hw_vect143 );

VEC_ALIGN_4B;
PPCASMF( hw_vect144: );				//Hardware Vector 144
PPCASMF( e_b   hw_vect144 );

VEC_ALIGN_4B;
PPCASMF( hw_vect145: );				//Hardware Vector 145
PPCASMF( e_b   hw_vect145 );

VEC_ALIGN_4B;
PPCASMF( hw_vect146: );				//Hardware Vector 146
PPCASMF( e_b   hw_vect146 );

VEC_ALIGN_4B;
PPCASMF( hw_vect147: );				//Hardware Vector 147
PPCASMF( e_b   hw_vect147 );

VEC_ALIGN_4B;
PPCASMF( hw_vect148: );				//Hardware Vector 148
PPCASMF( e_b   hw_vect148 );

VEC_ALIGN_4B;
PPCASMF( hw_vect149: );				//Hardware Vector 149
PPCASMF( e_b   hw_vect149 );

VEC_ALIGN_4B;
PPCASMF( hw_vect150: );				//Hardware Vector 150
PPCASMF( e_b   hw_vect150 );

VEC_ALIGN_4B;
PPCASMF( hw_vect151: );				//Hardware Vector 151
PPCASMF( e_b   hw_vect151 );

VEC_ALIGN_4B;
PPCASMF( hw_vect152: );				//Hardware Vector 152
PPCASMF( e_b   hw_vect152 );

VEC_ALIGN_4B;
PPCASMF( hw_vect153: );				//Hardware Vector 153
PPCASMF( e_b   hw_vect153 );

VEC_ALIGN_4B;
PPCASMF( hw_vect154: );				//Hardware Vector 154
PPCASMF( e_b   hw_vect154 );

VEC_ALIGN_4B;
PPCASMF( hw_vect155: );				//Hardware Vector 155
PPCASMF( e_b   hw_vect155 );

VEC_ALIGN_4B;
PPCASMF( hw_vect156: );				//Hardware Vector 156
PPCASMF( e_b   hw_vect156 );

VEC_ALIGN_4B;
PPCASMF( hw_vect157: );				//Hardware Vector 157
PPCASMF( e_b   hw_vect157 );

VEC_ALIGN_4B;
PPCASMF( hw_vect158: );				//Hardware Vector 158
PPCASMF( e_b   hw_vect158 );

VEC_ALIGN_4B;
PPCASMF( hw_vect159: );				//Hardware Vector 159
PPCASMF( e_b	hw_vect159 );

VEC_ALIGN_4B;
PPCASMF( hw_vect160: );				//Hardware Vector 160
PPCASMF( e_b	hw_vect160 );

VEC_ALIGN_4B;
PPCASMF( hw_vect161: );				//Hardware Vector 161
PPCASMF( e_b   hw_vect161 );

VEC_ALIGN_4B;
PPCASMF( hw_vect162: );				//Hardware Vector 162
PPCASMF( e_b   hw_vect162 );

VEC_ALIGN_4B;
PPCASMF( hw_vect163: );				//Hardware Vector 163
PPCASMF( e_b   hw_vect163 );

VEC_ALIGN_4B;
PPCASMF( hw_vect164: );				//Hardware Vector 164
PPCASMF( e_b   hw_vect164 );

VEC_ALIGN_4B;
PPCASMF( hw_vect165: );				//Hardware Vector 165
PPCASMF( e_b   hw_vect165 );

VEC_ALIGN_4B;
PPCASMF( hw_vect166: );				//Hardware Vector 166
PPCASMF( e_b   hw_vect166 );

VEC_ALIGN_4B;
PPCASMF( hw_vect167: );				//Hardware Vector 167
PPCASMF( e_b   hw_vect167 );

VEC_ALIGN_4B;
PPCASMF( hw_vect168: );				//Hardware Vector 168
PPCASMF( e_b   hw_vect168 );

VEC_ALIGN_4B;
PPCASMF( hw_vect169: );				//Hardware Vector 169
PPCASMF( e_b   hw_vect169 );

VEC_ALIGN_4B;
PPCASMF( hw_vect170: );				//Hardware Vector 170
PPCASMF( e_b   hw_vect170 );

VEC_ALIGN_4B;
PPCASMF( hw_vect171: );				//Hardware Vector 171
PPCASMF( e_b   hw_vect171 );

VEC_ALIGN_4B;
PPCASMF( hw_vect172: );				//Hardware Vector 172
PPCASMF( e_b   hw_vect172 );

VEC_ALIGN_4B;
PPCASMF( hw_vect173: );				//Hardware Vector 173
PPCASMF( e_b   hw_vect173 );

VEC_ALIGN_4B;
PPCASMF( hw_vect174: );				//Hardware Vector 174
PPCASMF( e_b   hw_vect174 );

VEC_ALIGN_4B;
PPCASMF( hw_vect175: );				//Hardware Vector 175
PPCASMF( e_b   hw_vect175 );

VEC_ALIGN_4B;
PPCASMF( hw_vect176: );				//Hardware Vector 176
PPCASMF( e_b   hw_vect176 );

VEC_ALIGN_4B;
PPCASMF( hw_vect177: );				//Hardware Vector 177
PPCASMF( e_b   hw_vect177 );

VEC_ALIGN_4B;
PPCASMF( hw_vect178: );				//Hardware Vector 178
PPCASMF( e_b   hw_vect178 );

VEC_ALIGN_4B;
PPCASMF( hw_vect179: );				//Hardware Vector 179
PPCASMF( e_b   hw_vect179 );

VEC_ALIGN_4B;
PPCASMF( hw_vect180: );				//Hardware Vector 180
PPCASMF( e_b   hw_vect180 );

VEC_ALIGN_4B;
PPCASMF( hw_vect181: );				//Hardware Vector 181
PPCASMF( e_b   hw_vect181 );

VEC_ALIGN_4B;
PPCASMF( hw_vect182: );				//Hardware Vector 182
PPCASMF( e_b   hw_vect182 );

VEC_ALIGN_4B;
PPCASMF( hw_vect183: );				//Hardware Vector 183
PPCASMF( e_b   hw_vect183 );

VEC_ALIGN_4B;
PPCASMF( hw_vect184: );				//Hardware Vector 184
PPCASMF( e_b   hw_vect184 );

VEC_ALIGN_4B;
PPCASMF( hw_vect185: );				//Hardware Vector 185
PPCASMF( e_b   hw_vect185 );

VEC_ALIGN_4B;
PPCASMF( hw_vect186: );				//Hardware Vector 186
PPCASMF( e_b   hw_vect186 );

VEC_ALIGN_4B;
PPCASMF( hw_vect187: );				//Hardware Vector 187
PPCASMF( e_b   hw_vect187 );

VEC_ALIGN_4B;
PPCASMF( hw_vect188: );				//Hardware Vector 188
PPCASMF( e_b   hw_vect188 );

VEC_ALIGN_4B;
PPCASMF( hw_vect189: );				//Hardware Vector 189
PPCASMF( e_b   hw_vect189 );

VEC_ALIGN_4B;
PPCASMF( hw_vect190: );				//Hardware Vector 190
PPCASMF( e_b   hw_vect190 );

VEC_ALIGN_4B;
PPCASMF( hw_vect191: );				//Hardware Vector 191
PPCASMF( e_b   hw_vect191 );

VEC_ALIGN_4B;
PPCASMF( hw_vect192: );				//Hardware Vector 192
PPCASMF( e_b   hw_vect192 );

VEC_ALIGN_4B;
PPCASMF( hw_vect193: );				//Hardware Vector 193
PPCASMF( e_b   hw_vect193 );

VEC_ALIGN_4B;
PPCASMF( hw_vect194: );				//Hardware Vector 194
PPCASMF( e_b   hw_vect194 );

VEC_ALIGN_4B;
PPCASMF( hw_vect195: );				//Hardware Vector 195
PPCASMF( e_b   hw_vect195 );

VEC_ALIGN_4B;
PPCASMF( hw_vect196: );				//Hardware Vector 196
PPCASMF( e_b   hw_vect196 );

VEC_ALIGN_4B;
PPCASMF( hw_vect197: );				//Hardware Vector 197
PPCASMF( e_b   hw_vect197 );

VEC_ALIGN_4B;
PPCASMF( hw_vect198: );				//Hardware Vector 198
PPCASMF( e_b   hw_vect198 );

VEC_ALIGN_4B;
PPCASMF( hw_vect199: );				//Hardware Vector 199
PPCASMF( e_b   hw_vect199 );

VEC_ALIGN_4B;
PPCASMF( hw_vect200: );				//Hardware Vector 200
PPCASMF( e_b   hw_vect200 );

VEC_ALIGN_4B;
PPCASMF( hw_vect201: );				//Hardware Vector 201
PPCASMF( e_b   hw_vect201 );

VEC_ALIGN_4B;
PPCASMF( hw_vect202: );				//Hardware Vector 202
PPCASMF( e_b   hw_vect202 );

VEC_ALIGN_4B;
PPCASMF( hw_vect203: );				//Hardware Vector 203
PPCASMF( e_b   hw_vect203 );

VEC_ALIGN_4B;
PPCASMF( hw_vect204: );				//Hardware Vector 204
PPCASMF( e_b  hw_vect204 );

VEC_ALIGN_4B;
PPCASMF( hw_vect205: );				//Hardware Vector 205
PPCASMF( e_b   hw_vect205 );

VEC_ALIGN_4B;
PPCASMF( hw_vect206: );				//Hardware Vector 206
PPCASMF( e_b   hw_vect206 );

VEC_ALIGN_4B;

PPCASMF( hw_vect207: );				//Hardware Vector 207
PPCASMF( e_b   hw_vect207 );

VEC_ALIGN_4B;
PPCASMF( hw_vect208: );				//Hardware Vector 208
PPCASMF( e_b   hw_vect208 );

VEC_ALIGN_4B;
PPCASMF( hw_vect209: );				//Hardware Vector 209
PPCASMF( e_b   hw_vect209 );

VEC_ALIGN_4B;
PPCASMF( hw_vect210: );				//Hardware Vector 210
PPCASMF( e_b   hw_vect210 );

VEC_ALIGN_4B;
PPCASMF( hw_vect211: );				//Hardware Vector 211
PPCASMF( e_b   hw_vect211 );

VEC_ALIGN_4B;
PPCASMF( hw_vect212: );				//Hardware Vector 212
PPCASMF( e_b   hw_vect212 );

VEC_ALIGN_4B;
PPCASMF( hw_vect213: );				//Hardware Vector 213
PPCASMF( e_b   hw_vect213 );

VEC_ALIGN_4B;
PPCASMF( hw_vect214: );				//Hardware Vector 214
PPCASMF( e_b   hw_vect214 );

VEC_ALIGN_4B;
PPCASMF( hw_vect215: );				//Hardware Vector 215
PPCASMF( e_b   hw_vect215 );

VEC_ALIGN_4B;
PPCASMF( hw_vect216: );				//Hardware Vector 216
PPCASMF( e_b   hw_vect216 );

VEC_ALIGN_4B;
PPCASMF( hw_vect217: );				//Hardware Vector 217
PPCASMF( e_b   hw_vect217 );

VEC_ALIGN_4B;
PPCASMF( hw_vect218: );				//Hardware Vector 218
PPCASMF( e_b   hw_vect218 );

VEC_ALIGN_4B;
PPCASMF( hw_vect219: );				//Hardware Vector 219
PPCASMF( e_b   hw_vect219 );

VEC_ALIGN_4B;
PPCASMF( hw_vect220: );				//Hardware Vector 220
PPCASMF( e_b   hw_vect220 );

VEC_ALIGN_4B;
PPCASMF( hw_vect221: );				//Hardware Vector 221
PPCASMF( e_b   hw_vect221 );

VEC_ALIGN_4B;
PPCASMF( hw_vect222: );				//Hardware Vector 222
PPCASMF( e_b   hw_vect222 );

VEC_ALIGN_4B;
PPCASMF( hw_vect223: );				//Hardware Vector 223
PPCASMF( e_b   hw_vect223 );

VEC_ALIGN_4B;
PPCASMF( hw_vect224: );				//Hardware Vector 224
PPCASMF( e_b   hw_vect224 );

VEC_ALIGN_4B;
PPCASMF( hw_vect225: );				//Hardware Vector 225
PPCASMF( e_b   hw_vect225 );

VEC_ALIGN_4B;
PPCASMF( hw_vect226: );				//Hardware Vector 226
PPCASMF( e_b  hw_vect226 );

VEC_ALIGN_4B;
PPCASMF( hw_vect227: );				//Hardware Vector 127
PPCASMF( e_b   hw_vect227 );

VEC_ALIGN_4B;
PPCASMF( hw_vect228: );				//Hardware Vector 128
PPCASMF( e_b   hw_vect228 );

VEC_ALIGN_4B;
PPCASMF( hw_vect229: );				//Hardware Vector 129
PPCASMF( e_b   hw_vect229 );

VEC_ALIGN_4B;
PPCASMF( hw_vect230: );				//Hardware Vector 130
PPCASMF( e_b   hw_vect230 );

VEC_ALIGN_4B;
PPCASMF( hw_vect231: );				//Hardware Vector 131
PPCASMF( e_b   hw_vect231 );

VEC_ALIGN_4B;
PPCASMF( hw_vect232: );				//Hardware Vector 132
PPCASMF( e_b   hw_vect232 );

VEC_ALIGN_4B;
PPCASMF( hw_vect233: );				//Hardware Vector 133
PPCASMF( e_b   hw_vect233 );

VEC_ALIGN_4B;
PPCASMF( hw_vect234: );				//Hardware Vector 134
PPCASMF( e_b   hw_vect234 );

VEC_ALIGN_4B;
PPCASMF( hw_vect235: );				//Hardware Vector 135
PPCASMF( e_b   hw_vect235 );

VEC_ALIGN_4B;
PPCASMF( hw_vect236: );				//Hardware Vector 136
PPCASMF( e_b   hw_vect236 );

VEC_ALIGN_4B;
PPCASMF( hw_vect237: );				//Hardware Vector 137
PPCASMF( e_b   hw_vect237 );

VEC_ALIGN_4B;
PPCASMF( hw_vect238: );				//Hardware Vector 138
PPCASMF( e_b   hw_vect238 );

VEC_ALIGN_4B;
PPCASMF( hw_vect239: );				//Hardware Vector 139
PPCASMF( e_b   hw_vect239 );

VEC_ALIGN_4B;
PPCASMF( hw_vect240: );				//Hardware Vector 140
PPCASMF( e_b   hw_vect240 );

VEC_ALIGN_4B;
PPCASMF( hw_vect241: );				//Hardware Vector 141
PPCASMF( e_b   hw_vect241 );

VEC_ALIGN_4B;
PPCASMF( hw_vect242: );				//Hardware Vector 142
PPCASMF( e_b   hw_vect242 );

VEC_ALIGN_4B;
PPCASMF( hw_vect243: );				//Hardware Vector 143
PPCASMF( e_b   hw_vect243 );

VEC_ALIGN_4B;
PPCASMF( hw_vect244: );				//Hardware Vector 144
PPCASMF( e_b   hw_vect244 );

VEC_ALIGN_4B;
PPCASMF( hw_vect245: );				//Hardware Vector 144
PPCASMF( e_b   hw_vect245 );

VEC_ALIGN_4B;
PPCASMF( hw_vect246: );				//Hardware Vector 202
PPCASMF( e_b   hw_vect246 );

VEC_ALIGN_4B;
PPCASMF( hw_vect247: );				//Hardware Vector 203
PPCASMF( e_b   hw_vect247 );

VEC_ALIGN_4B;
PPCASMF( hw_vect248: );				//Hardware Vector 204
PPCASMF( e_b  hw_vect248 );

VEC_ALIGN_4B;
PPCASMF( hw_vect249: );				//Hardware Vector 205
PPCASMF( e_b   hw_vect249 );

VEC_ALIGN_4B;
PPCASMF( hw_vect250: );				//Hardware Vector 150
PPCASMF( e_b   hw_vect250 );

VEC_ALIGN_4B;
PPCASMF( hw_vect251: );				//Hardware Vector 151
PPCASMF( e_b   hw_vect251 );

VEC_ALIGN_4B;
PPCASMF( hw_vect252: );				//Hardware Vector 152
PPCASMF( e_b   hw_vect252 );

VEC_ALIGN_4B;
PPCASMF( hw_vect253: );				//Hardware Vector 153
PPCASMF( e_b   hw_vect253 );

VEC_ALIGN_4B;
PPCASMF( hw_vect254: );				//Hardware Vector 154
PPCASMF( e_b   hw_vect254 );

VEC_ALIGN_4B;
PPCASMF( hw_vect255: );				//Hardware Vector 155
PPCASMF( e_b   hw_vect255 );

VEC_ALIGN_4B;
PPCASMF( hw_vect256: );				//Hardware Vector 156
PPCASMF( e_b   hw_vect256 );

VEC_ALIGN_4B;
PPCASMF( hw_vect257: );				//Hardware Vector 157
PPCASMF( e_b   hw_vect257 );

VEC_ALIGN_4B;
PPCASMF( hw_vect258: );				//Hardware Vector 158
PPCASMF( e_b   hw_vect258 );

VEC_ALIGN_4B;
PPCASMF( hw_vect259: );				//Hardware Vector 159
PPCASMF( e_b	hw_vect259 );

VEC_ALIGN_4B;
PPCASMF( hw_vect260: );				//Hardware Vector 160
PPCASMF( e_b	hw_vect260 );

VEC_ALIGN_4B;
PPCASMF( hw_vect261: );				//Hardware Vector 161
PPCASMF( e_b   hw_vect261 );

VEC_ALIGN_4B;
PPCASMF( hw_vect262: );				//Hardware Vector 162
PPCASMF( e_b   hw_vect262 );

VEC_ALIGN_4B;
PPCASMF( hw_vect263: );				//Hardware Vector 163
PPCASMF( e_b   hw_vect263 );

VEC_ALIGN_4B;
PPCASMF( hw_vect264: );				//Hardware Vector 164
PPCASMF( e_b   hw_vect264 );

VEC_ALIGN_4B;
PPCASMF( hw_vect265: );				//Hardware Vector 165
PPCASMF( e_b   hw_vect265 );

VEC_ALIGN_4B;
PPCASMF( hw_vect266: );				//Hardware Vector 166
PPCASMF( e_b   hw_vect266 );

VEC_ALIGN_4B;
PPCASMF( hw_vect267: );				//Hardware Vector 167
PPCASMF( e_b   hw_vect267 );

VEC_ALIGN_4B;
PPCASMF( hw_vect268: );				//Hardware Vector 168
PPCASMF( e_b   hw_vect268 );

VEC_ALIGN_4B;
PPCASMF( hw_vect269: );				//Hardware Vector 169
PPCASMF( e_b   hw_vect269 );

VEC_ALIGN_4B;
PPCASMF( hw_vect270: );				//Hardware Vector 170
PPCASMF( e_b   hw_vect270 );

VEC_ALIGN_4B;
PPCASMF( hw_vect271: );				//Hardware Vector 171
PPCASMF( e_b   hw_vect271 );

VEC_ALIGN_4B;
PPCASMF( hw_vect272: );				//Hardware Vector 172
PPCASMF( e_b   hw_vect272 );

VEC_ALIGN_4B;
PPCASMF( hw_vect273: );				//Hardware Vector 173
PPCASMF( e_b   hw_vect273 );

VEC_ALIGN_4B;
PPCASMF( hw_vect274: );				//Hardware Vector 174
PPCASMF( e_b   hw_vect274 );

VEC_ALIGN_4B;
PPCASMF( hw_vect275: );				//Hardware Vector 175
PPCASMF( e_b   hw_vect275 );

VEC_ALIGN_4B;
PPCASMF( hw_vect276: );				//Hardware Vector 176
PPCASMF( e_b   hw_vect276 );

VEC_ALIGN_4B;
PPCASMF( hw_vect277: );				//Hardware Vector 177
PPCASMF( e_b   hw_vect277 );

VEC_ALIGN_4B;
PPCASMF( hw_vect278: );				//Hardware Vector 178
PPCASMF( e_b   hw_vect278 );

VEC_ALIGN_4B;
PPCASMF( hw_vect279: );				//Hardware Vector 179
PPCASMF( e_b   hw_vect279 );

VEC_ALIGN_4B;
PPCASMF( hw_vect280: );				//Hardware Vector 180
PPCASMF( e_b   hw_vect280 );

VEC_ALIGN_4B;
PPCASMF( hw_vect281: );				//Hardware Vector 181
PPCASMF( e_b   hw_vect281 );

VEC_ALIGN_4B;
PPCASMF( hw_vect282: );				//Hardware Vector 182
PPCASMF( e_b   hw_vect282 );

VEC_ALIGN_4B;
PPCASMF( hw_vect283: );				//Hardware Vector 183
PPCASMF( e_b   hw_vect283 );

VEC_ALIGN_4B;
PPCASMF( hw_vect284: );				//Hardware Vector 184
PPCASMF( e_b   hw_vect284 );

VEC_ALIGN_4B;
PPCASMF( hw_vect285: );				//Hardware Vector 185
PPCASMF( e_b   hw_vect285 );

VEC_ALIGN_4B;
PPCASMF( hw_vect286: );				//Hardware Vector 186
PPCASMF( e_b   hw_vect286 );

VEC_ALIGN_4B;
PPCASMF( hw_vect287: );				//Hardware Vector 187
PPCASMF( e_b   hw_vect287 );

VEC_ALIGN_4B;
PPCASMF( hw_vect288: );				//Hardware Vector 188
PPCASMF( e_b   hw_vect288 );

VEC_ALIGN_4B;
PPCASMF( hw_vect289: );				//Hardware Vector 189
PPCASMF( e_b   hw_vect289 );

VEC_ALIGN_4B;
PPCASMF( hw_vect290: );				//Hardware Vector 190
PPCASMF( e_b   hw_vect290 );

VEC_ALIGN_4B;
PPCASMF( hw_vect291: );				//Hardware Vector 191
PPCASMF( e_b   hw_vect291 );

VEC_ALIGN_4B;
PPCASMF( hw_vect292: );				//Hardware Vector 192
PPCASMF( e_b   hw_vect292 );

VEC_ALIGN_4B;
PPCASMF( hw_vect293: );				//Hardware Vector 193
PPCASMF( e_b   hw_vect293 );

VEC_ALIGN_4B;
PPCASMF( hw_vect294: );				//Hardware Vector 194
PPCASMF( e_b   hw_vect294 );

VEC_ALIGN_4B;
PPCASMF( hw_vect295: );				//Hardware Vector 195
PPCASMF( e_b   hw_vect295 );

VEC_ALIGN_4B;
PPCASMF( hw_vect296: );				//Hardware Vector 196
PPCASMF( e_b   hw_vect296 );

VEC_ALIGN_4B;
PPCASMF( hw_vect297: );				//Hardware Vector 197
PPCASMF( e_b   hw_vect297 );

VEC_ALIGN_4B;
PPCASMF( hw_vect298: );				//Hardware Vector 198
PPCASMF( e_b   hw_vect298 );

VEC_ALIGN_4B;
PPCASMF( hw_vect299: );				//Hardware Vector 199
PPCASMF( e_b   hw_vect299 );

VEC_ALIGN_4B;
PPCASMF( hw_vect300: );				//Hardware Vector 200
PPCASMF( e_b   hw_vect300 );

VEC_ALIGN_4B;
PPCASMF( hw_vect301: );				//Hardware Vector 201
PPCASMF( e_b   hw_vect301 );

VEC_ALIGN_4B;
PPCASMF( hw_vect302: );				//Hardware Vector 202
PPCASMF( e_b   hw_vect302 );

VEC_ALIGN_4B;
PPCASMF( hw_vect303: );				//Hardware Vector 203
PPCASMF( e_b   hw_vect303 );

VEC_ALIGN_4B;
PPCASMF( hw_vect304: );				//Hardware Vector 204
PPCASMF( e_b  Pit3Handler );


 */