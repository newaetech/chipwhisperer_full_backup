/*
 * Copyright (c) 2013 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2107 NXP
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#if !defined(CLOCK_MPC57XX_H)
#define CLOCK_MPC57XX_H

/*!
 * @file clock_MPC57XX.h
 */

#include "status.h"
#include <stdint.h>
#include <stdbool.h>
#include "MPC5676R.h"
#include "MPC5676R_features.h"

/*! @file clock_MPC5748G.h */

/*!
 * @file clock_MPC57xx.h
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 2.5, Global macro not referenced.
 * This is required to implement enumeration entries as macro.
 * Enumeration entries (macros) are referenced from application code.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 2.3, Global typedef not referenced.
 * The specified typedefs might be used in the user code for composing masks
 * which could be passed to some of the clock_manager driver functions.
 *
 */

/*!
 * @ingroup clock_manager
 * @defgroup clock_MPC57xx
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* @brief Key and inverted key values so write access to MCTL register is permitted */
#define FEATURE_MC_ME_KEY           (0x5AF0U)
#define FEATURE_MC_ME_KEY_INV       (0xA50FU)

/* @brief Maximum number of the CMUs that can be configured */
#define CMU_MAX_NO 9u

/* @brief EXTAL0 clock frequency. */
extern uint32_t g_xtal0ClkFreq;

/* @brief EXTAL1 clock frequency. */
extern uint32_t g_xtal1ClkFreq;

/* @brief SIPI_REF clock frequency. */
extern uint32_t g_sipiRefClkFreq;

/*!
 * @brief SIU clock divider
 * Implements siu_clk_div_t_Class
 */
typedef enum {

	SIU_CLOCK_DIV_BY_1 = 0U, /*!< Divided by 1                 */
	SIU_CLOCK_DIV_BY_2 = 1U, /*!< Divided by 2                 */
	SIU_CLOCK_DIV_BY_3 = 2U, /*!< Divided by 3                 */
	SIU_CLOCK_DIV_BY_4 = 3U, /*!< Divided by 4                 */
	SIU_CLOCK_DIV_BY_5 = 4U, /*!< Divided by 5                 */
	SIU_CLOCK_DIV_BY_6 = 5U, /*!< Divided by 6                 */
	SIU_CLOCK_DIV_BY_7 = 6U, /*!< Divided by 7                 */
	SIU_CLOCK_DIV_BY_8 = 7U, /*!< Divided by 8                 */
	SIU_CLOCK_DIV_BY_9 = 8U, /*!< Divided by 9                 */
	SIU_CLOCK_DIV_BY_10 = 9U, /*!< Divided by 10                */
	SIU_CLOCK_DIV_BY_11 = 10U, /*!< Divided by 11                */
	SIU_CLOCK_DIV_BY_12 = 11U, /*!< Divided by 12                */
	SIU_CLOCK_DIV_BY_13 = 12U, /*!< Divided by 13                */
	SIU_CLOCK_DIV_BY_14 = 13U, /*!< Divided by 14                */
	SIU_CLOCK_DIV_BY_15 = 14U, /*!< Divided by 15                */
	SIU_CLOCK_DIV_BY_16 = 15U, /*!< Divided by 16                */
	SIU_CLOCK_DIV_BY_17 = 16U, /*!< Divided by 17                */
	SIU_CLOCK_DIV_BY_18 = 17U, /*!< Divided by 18                */
	SIU_CLOCK_DIV_BY_19 = 18U, /*!< Divided by 19                */
	SIU_CLOCK_DIV_BY_20 = 19U, /*!< Divided by 20                */
	SIU_CLOCK_DIV_BY_21 = 20U, /*!< Divided by 21                */
	SIU_CLOCK_DIV_BY_22 = 21U, /*!< Divided by 22                */
	SIU_CLOCK_DIV_BY_23 = 22U, /*!< Divided by 23                */
	SIU_CLOCK_DIV_BY_24 = 23U, /*!< Divided by 24                */
	SIU_CLOCK_DIV_BY_25 = 24U, /*!< Divided by 25                */
	SIU_CLOCK_DIV_BY_26 = 25U, /*!< Divided by 26                */
	SIU_CLOCK_DIV_BY_27 = 26U, /*!< Divided by 27                */
	SIU_CLOCK_DIV_BY_28 = 27U, /*!< Divided by 28                */
	SIU_CLOCK_DIV_BY_29 = 28U, /*!< Divided by 29                */
	SIU_CLOCK_DIV_BY_30 = 29U, /*!< Divided by 30                */
	SIU_CLOCK_DIV_BY_31 = 30U, /*!< Divided by 31                */
	SIU_CLOCK_DIV_BY_32 = 31U, /*!< Divided by 32                */
	SIU_CLOCK_DIV_BY_33 = 32U, /*!< Divided by 33                */
	SIU_CLOCK_DIV_BY_34 = 33U, /*!< Divided by 34                */
	SIU_CLOCK_DIV_BY_35 = 34U, /*!< Divided by 35                */
	SIU_CLOCK_DIV_BY_36 = 35U, /*!< Divided by 36                */
	SIU_CLOCK_DIV_BY_37 = 36U, /*!< Divided by 37                */
	SIU_CLOCK_DIV_BY_38 = 37U, /*!< Divided by 38                */
	SIU_CLOCK_DIV_BY_39 = 38U, /*!< Divided by 39                */
	SIU_CLOCK_DIV_BY_40 = 39U, /*!< Divided by 40                */
	SIU_CLOCK_DIV_BY_41 = 40U, /*!< Divided by 41                */
	SIU_CLOCK_DIV_BY_42 = 41U, /*!< Divided by 42                */
	SIU_CLOCK_DIV_BY_43 = 42U, /*!< Divided by 43                */
	SIU_CLOCK_DIV_BY_44 = 43U, /*!< Divided by 44                */
	SIU_CLOCK_DIV_BY_45 = 44U, /*!< Divided by 45                */
	SIU_CLOCK_DIV_BY_46 = 45U, /*!< Divided by 46                */
	SIU_CLOCK_DIV_BY_47 = 46U, /*!< Divided by 47                */
	SIU_CLOCK_DIV_BY_48 = 47U, /*!< Divided by 48                */
	SIU_CLOCK_DIV_BY_49 = 48U, /*!< Divided by 49                */
	SIU_CLOCK_DIV_BY_50 = 49U, /*!< Divided by 50                */
	SIU_CLOCK_DIV_BY_51 = 50U, /*!< Divided by 51                */
	SIU_CLOCK_DIV_BY_52 = 51U, /*!< Divided by 52                */
	SIU_CLOCK_DIV_BY_53 = 52U, /*!< Divided by 53                */
	SIU_CLOCK_DIV_BY_54 = 53U, /*!< Divided by 54                */
	SIU_CLOCK_DIV_BY_55 = 54U, /*!< Divided by 55                */
	SIU_CLOCK_DIV_BY_56 = 55U, /*!< Divided by 56                */
	SIU_CLOCK_DIV_BY_57 = 56U, /*!< Divided by 57                */
	SIU_CLOCK_DIV_BY_58 = 57U, /*!< Divided by 58                */
	SIU_CLOCK_DIV_BY_59 = 58U, /*!< Divided by 59                */
	SIU_CLOCK_DIV_BY_60 = 59U, /*!< Divided by 60                */
	SIU_CLOCK_DIV_BY_61 = 60U, /*!< Divided by 61                */
	SIU_CLOCK_DIV_BY_62 = 61U, /*!< Divided by 62                */
	SIU_CLOCK_DIV_BY_63 = 62U, /*!< Divided by 63                */
	SIU_CLOCK_DIV_BY_64 = 63U, /*!< Divided by 64                */
	SIU_CLOCK_DIV_BY_65 = 64U, /*!< Divided by 65                */
	SIU_CLOCK_DIV_BY_66 = 65U, /*!< Divided by 66                */
	SIU_CLOCK_DIV_BY_67 = 66U, /*!< Divided by 67                */
	SIU_CLOCK_DIV_BY_68 = 67U, /*!< Divided by 68                */
	SIU_CLOCK_DIV_BY_69 = 68U, /*!< Divided by 69                */
	SIU_CLOCK_DIV_BY_70 = 69U, /*!< Divided by 70                */
	SIU_CLOCK_DIV_BY_71 = 70U, /*!< Divided by 71                */
	SIU_CLOCK_DIV_BY_72 = 71U, /*!< Divided by 72                */
	SIU_CLOCK_DIV_BY_73 = 72U, /*!< Divided by 73                */
	SIU_CLOCK_DIV_BY_74 = 73U, /*!< Divided by 74                */
	SIU_CLOCK_DIV_BY_75 = 74U, /*!< Divided by 75                */
	SIU_CLOCK_DIV_BY_76 = 75U, /*!< Divided by 76                */
	SIU_CLOCK_DIV_BY_77 = 76U, /*!< Divided by 77                */
	SIU_CLOCK_DIV_BY_78 = 77U, /*!< Divided by 78                */
	SIU_CLOCK_DIV_BY_79 = 78U, /*!< Divided by 79                */
	SIU_CLOCK_DIV_BY_80 = 79U, /*!< Divided by 80                */
	SIU_CLOCK_DIV_BY_81 = 80U, /*!< Divided by 81                */
	SIU_CLOCK_DIV_BY_82 = 81U, /*!< Divided by 82                */
	SIU_CLOCK_DIV_BY_83 = 82U, /*!< Divided by 83                */
	SIU_CLOCK_DIV_BY_84 = 83U, /*!< Divided by 84                */
	SIU_CLOCK_DIV_BY_85 = 84U, /*!< Divided by 85                */
	SIU_CLOCK_DIV_BY_86 = 85U, /*!< Divided by 86                */
	SIU_CLOCK_DIV_BY_87 = 86U, /*!< Divided by 87                */
	SIU_CLOCK_DIV_BY_88 = 87U, /*!< Divided by 88                */
	SIU_CLOCK_DIV_BY_89 = 88U, /*!< Divided by 89                */
	SIU_CLOCK_DIV_BY_90 = 89U, /*!< Divided by 90                */
	SIU_CLOCK_DIV_BY_91 = 90U, /*!< Divided by 91                */
	SIU_CLOCK_DIV_BY_92 = 91U, /*!< Divided by 92                */
	SIU_CLOCK_DIV_BY_93 = 92U, /*!< Divided by 93                */
	SIU_CLOCK_DIV_BY_94 = 93U, /*!< Divided by 94                */
	SIU_CLOCK_DIV_BY_95 = 94U, /*!< Divided by 95                */
	SIU_CLOCK_DIV_BY_96 = 95U, /*!< Divided by 96                */
	SIU_CLOCK_DIV_BY_97 = 96U, /*!< Divided by 97                */
	SIU_CLOCK_DIV_BY_98 = 97U, /*!< Divided by 98                */
	SIU_CLOCK_DIV_BY_99 = 98U, /*!< Divided by 99                */
	SIU_CLOCK_DIV_BY_100 = 99U, /*!< Divided by 100               */
	SIU_CLOCK_DIV_BY_101 = 100U, /*!< Divided by 101               */
	SIU_CLOCK_DIV_BY_102 = 101U, /*!< Divided by 102               */
	SIU_CLOCK_DIV_BY_103 = 102U, /*!< Divided by 103               */
	SIU_CLOCK_DIV_BY_104 = 103U, /*!< Divided by 104               */
	SIU_CLOCK_DIV_BY_105 = 104U, /*!< Divided by 105               */
	SIU_CLOCK_DIV_BY_106 = 105U, /*!< Divided by 106               */
	SIU_CLOCK_DIV_BY_107 = 106U, /*!< Divided by 107               */
	SIU_CLOCK_DIV_BY_108 = 107U, /*!< Divided by 108               */
	SIU_CLOCK_DIV_BY_109 = 108U, /*!< Divided by 109               */
	SIU_CLOCK_DIV_BY_110 = 109U, /*!< Divided by 110               */
	SIU_CLOCK_DIV_BY_111 = 110U, /*!< Divided by 111               */
	SIU_CLOCK_DIV_BY_112 = 111U, /*!< Divided by 112               */
	SIU_CLOCK_DIV_BY_113 = 112U, /*!< Divided by 113               */
	SIU_CLOCK_DIV_BY_114 = 113U, /*!< Divided by 114               */
	SIU_CLOCK_DIV_BY_115 = 114U, /*!< Divided by 115               */
	SIU_CLOCK_DIV_BY_116 = 115U, /*!< Divided by 116               */
	SIU_CLOCK_DIV_BY_117 = 116U, /*!< Divided by 117               */
	SIU_CLOCK_DIV_BY_118 = 117U, /*!< Divided by 118               */
	SIU_CLOCK_DIV_BY_119 = 118U, /*!< Divided by 119               */
	SIU_CLOCK_DIV_BY_120 = 119U, /*!< Divided by 120               */
	SIU_CLOCK_DIV_BY_121 = 120U, /*!< Divided by 121               */
	SIU_CLOCK_DIV_BY_122 = 121U, /*!< Divided by 122               */
	SIU_CLOCK_DIV_BY_123 = 122U, /*!< Divided by 123               */
	SIU_CLOCK_DIV_BY_124 = 123U, /*!< Divided by 124               */
	SIU_CLOCK_DIV_BY_125 = 124U, /*!< Divided by 125               */
	SIU_CLOCK_DIV_BY_126 = 125U, /*!< Divided by 126               */
	SIU_CLOCK_DIV_BY_127 = 126U, /*!< Divided by 127               */
	SIU_CLOCK_DIV_BY_128 = 127U, /*!< Divided by 128               */
	SIU_CLOCK_DIV_BY_129 = 128U, /*!< Divided by 129               */
	SIU_CLOCK_DIV_BY_130 = 129U, /*!< Divided by 130               */
	SIU_CLOCK_DIV_BY_131 = 130U, /*!< Divided by 131               */
	SIU_CLOCK_DIV_BY_132 = 131U, /*!< Divided by 132               */
	SIU_CLOCK_DIV_BY_133 = 132U, /*!< Divided by 133               */
	SIU_CLOCK_DIV_BY_134 = 133U, /*!< Divided by 134               */
	SIU_CLOCK_DIV_BY_135 = 134U, /*!< Divided by 135               */
	SIU_CLOCK_DIV_BY_136 = 135U, /*!< Divided by 136               */
	SIU_CLOCK_DIV_BY_137 = 136U, /*!< Divided by 137               */
	SIU_CLOCK_DIV_BY_138 = 137U, /*!< Divided by 138               */
	SIU_CLOCK_DIV_BY_139 = 138U, /*!< Divided by 139               */
	SIU_CLOCK_DIV_BY_140 = 139U, /*!< Divided by 140               */
	SIU_CLOCK_DIV_BY_141 = 140U, /*!< Divided by 141               */
	SIU_CLOCK_DIV_BY_142 = 141U, /*!< Divided by 142               */
	SIU_CLOCK_DIV_BY_143 = 142U, /*!< Divided by 143               */
	SIU_CLOCK_DIV_BY_144 = 143U, /*!< Divided by 144               */
	SIU_CLOCK_DIV_BY_145 = 144U, /*!< Divided by 145               */
	SIU_CLOCK_DIV_BY_146 = 145U, /*!< Divided by 146               */
	SIU_CLOCK_DIV_BY_147 = 146U, /*!< Divided by 147               */
	SIU_CLOCK_DIV_BY_148 = 147U, /*!< Divided by 148               */
	SIU_CLOCK_DIV_BY_149 = 148U, /*!< Divided by 149               */
	SIU_CLOCK_DIV_BY_150 = 149U, /*!< Divided by 150               */
	SIU_CLOCK_DIV_BY_151 = 150U, /*!< Divided by 151               */
	SIU_CLOCK_DIV_BY_152 = 151U, /*!< Divided by 152               */
	SIU_CLOCK_DIV_BY_153 = 152U, /*!< Divided by 153               */
	SIU_CLOCK_DIV_BY_154 = 153U, /*!< Divided by 154               */
	SIU_CLOCK_DIV_BY_155 = 154U, /*!< Divided by 155               */
	SIU_CLOCK_DIV_BY_156 = 155U, /*!< Divided by 156               */
	SIU_CLOCK_DIV_BY_157 = 156U, /*!< Divided by 157               */
	SIU_CLOCK_DIV_BY_158 = 157U, /*!< Divided by 158               */
	SIU_CLOCK_DIV_BY_159 = 158U, /*!< Divided by 159               */
	SIU_CLOCK_DIV_BY_160 = 159U, /*!< Divided by 160               */
	SIU_CLOCK_DIV_BY_161 = 160U, /*!< Divided by 161               */
	SIU_CLOCK_DIV_BY_162 = 161U, /*!< Divided by 162               */
	SIU_CLOCK_DIV_BY_163 = 162U, /*!< Divided by 163               */
	SIU_CLOCK_DIV_BY_164 = 163U, /*!< Divided by 164               */
	SIU_CLOCK_DIV_BY_165 = 164U, /*!< Divided by 165               */
	SIU_CLOCK_DIV_BY_166 = 165U, /*!< Divided by 166               */
	SIU_CLOCK_DIV_BY_167 = 166U, /*!< Divided by 167               */
	SIU_CLOCK_DIV_BY_168 = 167U, /*!< Divided by 168               */
	SIU_CLOCK_DIV_BY_169 = 168U, /*!< Divided by 169               */
	SIU_CLOCK_DIV_BY_170 = 169U, /*!< Divided by 170               */
	SIU_CLOCK_DIV_BY_171 = 170U, /*!< Divided by 171               */
	SIU_CLOCK_DIV_BY_172 = 171U, /*!< Divided by 172               */
	SIU_CLOCK_DIV_BY_173 = 172U, /*!< Divided by 173               */
	SIU_CLOCK_DIV_BY_174 = 173U, /*!< Divided by 174               */
	SIU_CLOCK_DIV_BY_175 = 174U, /*!< Divided by 175               */
	SIU_CLOCK_DIV_BY_176 = 175U, /*!< Divided by 176               */
	SIU_CLOCK_DIV_BY_177 = 176U, /*!< Divided by 177               */
	SIU_CLOCK_DIV_BY_178 = 177U, /*!< Divided by 178               */
	SIU_CLOCK_DIV_BY_179 = 178U, /*!< Divided by 179               */
	SIU_CLOCK_DIV_BY_180 = 179U, /*!< Divided by 180               */
	SIU_CLOCK_DIV_BY_181 = 180U, /*!< Divided by 181               */
	SIU_CLOCK_DIV_BY_182 = 181U, /*!< Divided by 182               */
	SIU_CLOCK_DIV_BY_183 = 182U, /*!< Divided by 183               */
	SIU_CLOCK_DIV_BY_184 = 183U, /*!< Divided by 184               */
	SIU_CLOCK_DIV_BY_185 = 184U, /*!< Divided by 185               */
	SIU_CLOCK_DIV_BY_186 = 185U, /*!< Divided by 186               */
	SIU_CLOCK_DIV_BY_187 = 186U, /*!< Divided by 187               */
	SIU_CLOCK_DIV_BY_188 = 187U, /*!< Divided by 188               */
	SIU_CLOCK_DIV_BY_189 = 188U, /*!< Divided by 189               */
	SIU_CLOCK_DIV_BY_190 = 189U, /*!< Divided by 190               */
	SIU_CLOCK_DIV_BY_191 = 190U, /*!< Divided by 191               */
	SIU_CLOCK_DIV_BY_192 = 191U, /*!< Divided by 192               */
	SIU_CLOCK_DIV_BY_193 = 192U, /*!< Divided by 193               */
	SIU_CLOCK_DIV_BY_194 = 193U, /*!< Divided by 194               */
	SIU_CLOCK_DIV_BY_195 = 194U, /*!< Divided by 195               */
	SIU_CLOCK_DIV_BY_196 = 195U, /*!< Divided by 196               */
	SIU_CLOCK_DIV_BY_197 = 196U, /*!< Divided by 197               */
	SIU_CLOCK_DIV_BY_198 = 197U, /*!< Divided by 198               */
	SIU_CLOCK_DIV_BY_199 = 198U, /*!< Divided by 199               */
	SIU_CLOCK_DIV_BY_200 = 199U, /*!< Divided by 200               */
	SIU_CLOCK_DIV_BY_201 = 200U, /*!< Divided by 201               */
	SIU_CLOCK_DIV_BY_202 = 201U, /*!< Divided by 202               */
	SIU_CLOCK_DIV_BY_203 = 202U, /*!< Divided by 203               */
	SIU_CLOCK_DIV_BY_204 = 203U, /*!< Divided by 204               */
	SIU_CLOCK_DIV_BY_205 = 204U, /*!< Divided by 205               */
	SIU_CLOCK_DIV_BY_206 = 205U, /*!< Divided by 206               */
	SIU_CLOCK_DIV_BY_207 = 206U, /*!< Divided by 207               */
	SIU_CLOCK_DIV_BY_208 = 207U, /*!< Divided by 208               */
	SIU_CLOCK_DIV_BY_209 = 208U, /*!< Divided by 209               */
	SIU_CLOCK_DIV_BY_210 = 209U, /*!< Divided by 210               */
	SIU_CLOCK_DIV_BY_211 = 210U, /*!< Divided by 211               */
	SIU_CLOCK_DIV_BY_212 = 211U, /*!< Divided by 212               */
	SIU_CLOCK_DIV_BY_213 = 212U, /*!< Divided by 213               */
	SIU_CLOCK_DIV_BY_214 = 213U, /*!< Divided by 214               */
	SIU_CLOCK_DIV_BY_215 = 214U, /*!< Divided by 215               */
	SIU_CLOCK_DIV_BY_216 = 215U, /*!< Divided by 216               */
	SIU_CLOCK_DIV_BY_217 = 216U, /*!< Divided by 217               */
	SIU_CLOCK_DIV_BY_218 = 217U, /*!< Divided by 218               */
	SIU_CLOCK_DIV_BY_219 = 218U, /*!< Divided by 219               */
	SIU_CLOCK_DIV_BY_220 = 219U, /*!< Divided by 220               */
	SIU_CLOCK_DIV_BY_221 = 220U, /*!< Divided by 221               */
	SIU_CLOCK_DIV_BY_222 = 221U, /*!< Divided by 222               */
	SIU_CLOCK_DIV_BY_223 = 222U, /*!< Divided by 223               */
	SIU_CLOCK_DIV_BY_224 = 223U, /*!< Divided by 224               */
	SIU_CLOCK_DIV_BY_225 = 224U, /*!< Divided by 225               */
	SIU_CLOCK_DIV_BY_226 = 225U, /*!< Divided by 226               */
	SIU_CLOCK_DIV_BY_227 = 226U, /*!< Divided by 227               */
	SIU_CLOCK_DIV_BY_228 = 227U, /*!< Divided by 228               */
	SIU_CLOCK_DIV_BY_229 = 228U, /*!< Divided by 229               */
	SIU_CLOCK_DIV_BY_230 = 229U, /*!< Divided by 230               */
	SIU_CLOCK_DIV_BY_231 = 230U, /*!< Divided by 231               */
	SIU_CLOCK_DIV_BY_232 = 231U, /*!< Divided by 232               */
	SIU_CLOCK_DIV_BY_233 = 232U, /*!< Divided by 233               */
	SIU_CLOCK_DIV_BY_234 = 233U, /*!< Divided by 234               */
	SIU_CLOCK_DIV_BY_235 = 234U, /*!< Divided by 235               */
	SIU_CLOCK_DIV_BY_236 = 235U, /*!< Divided by 236               */
	SIU_CLOCK_DIV_BY_237 = 236U, /*!< Divided by 237               */
	SIU_CLOCK_DIV_BY_238 = 237U, /*!< Divided by 238               */
	SIU_CLOCK_DIV_BY_239 = 238U, /*!< Divided by 239               */
	SIU_CLOCK_DIV_BY_240 = 239U, /*!< Divided by 240               */
	SIU_CLOCK_DIV_BY_241 = 240U, /*!< Divided by 241               */
	SIU_CLOCK_DIV_BY_242 = 241U, /*!< Divided by 242               */
	SIU_CLOCK_DIV_BY_243 = 242U, /*!< Divided by 243               */
	SIU_CLOCK_DIV_BY_244 = 243U, /*!< Divided by 244               */
	SIU_CLOCK_DIV_BY_245 = 244U, /*!< Divided by 245               */
	SIU_CLOCK_DIV_BY_246 = 245U, /*!< Divided by 246               */
	SIU_CLOCK_DIV_BY_247 = 246U, /*!< Divided by 247               */
	SIU_CLOCK_DIV_BY_248 = 247U, /*!< Divided by 248               */
	SIU_CLOCK_DIV_BY_249 = 248U, /*!< Divided by 249               */
	SIU_CLOCK_DIV_BY_250 = 249U, /*!< Divided by 250               */
	SIU_CLOCK_DIV_BY_251 = 250U, /*!< Divided by 251               */
	SIU_CLOCK_DIV_BY_252 = 251U, /*!< Divided by 252               */
	SIU_CLOCK_DIV_BY_253 = 252U, /*!< Divided by 253               */
	SIU_CLOCK_DIV_BY_254 = 253U, /*!< Divided by 254               */
	SIU_CLOCK_DIV_BY_255 = 254U, /*!< Divided by 255               */
	SIU_CLOCK_DIV_BY_256 = 255U, /*!< Divided by 256               */
	SIU_CLOCK_DIV_BY_257 = 256U, /*!< Divided by 257               */
	SIU_CLOCK_DIV_BY_258 = 257U, /*!< Divided by 258               */
	SIU_CLOCK_DIV_BY_259 = 258U, /*!< Divided by 259               */
	SIU_CLOCK_DIV_BY_260 = 259U, /*!< Divided by 260               */
	SIU_CLOCK_DIV_BY_261 = 260U, /*!< Divided by 261               */
	SIU_CLOCK_DIV_BY_262 = 261U, /*!< Divided by 262               */
	SIU_CLOCK_DIV_BY_263 = 262U, /*!< Divided by 263               */
	SIU_CLOCK_DIV_BY_264 = 263U, /*!< Divided by 264               */
	SIU_CLOCK_DIV_BY_265 = 264U, /*!< Divided by 265               */
	SIU_CLOCK_DIV_BY_266 = 265U, /*!< Divided by 266               */
	SIU_CLOCK_DIV_BY_267 = 266U, /*!< Divided by 267               */
	SIU_CLOCK_DIV_BY_268 = 267U, /*!< Divided by 268               */
	SIU_CLOCK_DIV_BY_269 = 268U, /*!< Divided by 269               */
	SIU_CLOCK_DIV_BY_270 = 269U, /*!< Divided by 270               */
	SIU_CLOCK_DIV_BY_271 = 270U, /*!< Divided by 271               */
	SIU_CLOCK_DIV_BY_272 = 271U, /*!< Divided by 272               */
	SIU_CLOCK_DIV_BY_273 = 272U, /*!< Divided by 273               */
	SIU_CLOCK_DIV_BY_274 = 273U, /*!< Divided by 274               */
	SIU_CLOCK_DIV_BY_275 = 274U, /*!< Divided by 275               */
	SIU_CLOCK_DIV_BY_276 = 275U, /*!< Divided by 276               */
	SIU_CLOCK_DIV_BY_277 = 276U, /*!< Divided by 277               */
	SIU_CLOCK_DIV_BY_278 = 277U, /*!< Divided by 278               */
	SIU_CLOCK_DIV_BY_279 = 278U, /*!< Divided by 279               */
	SIU_CLOCK_DIV_BY_280 = 279U, /*!< Divided by 280               */
	SIU_CLOCK_DIV_BY_281 = 280U, /*!< Divided by 281               */
	SIU_CLOCK_DIV_BY_282 = 281U, /*!< Divided by 282               */
	SIU_CLOCK_DIV_BY_283 = 282U, /*!< Divided by 283               */
	SIU_CLOCK_DIV_BY_284 = 283U, /*!< Divided by 284               */
	SIU_CLOCK_DIV_BY_285 = 284U, /*!< Divided by 285               */
	SIU_CLOCK_DIV_BY_286 = 285U, /*!< Divided by 286               */
	SIU_CLOCK_DIV_BY_287 = 286U, /*!< Divided by 287               */
	SIU_CLOCK_DIV_BY_288 = 287U, /*!< Divided by 288               */
	SIU_CLOCK_DIV_BY_289 = 288U, /*!< Divided by 289               */
	SIU_CLOCK_DIV_BY_290 = 289U, /*!< Divided by 290               */
	SIU_CLOCK_DIV_BY_291 = 290U, /*!< Divided by 291               */
	SIU_CLOCK_DIV_BY_292 = 291U, /*!< Divided by 292               */
	SIU_CLOCK_DIV_BY_293 = 292U, /*!< Divided by 293               */
	SIU_CLOCK_DIV_BY_294 = 293U, /*!< Divided by 294               */
	SIU_CLOCK_DIV_BY_295 = 294U, /*!< Divided by 295               */
	SIU_CLOCK_DIV_BY_296 = 295U, /*!< Divided by 296               */
	SIU_CLOCK_DIV_BY_297 = 296U, /*!< Divided by 297               */
	SIU_CLOCK_DIV_BY_298 = 297U, /*!< Divided by 298               */
	SIU_CLOCK_DIV_BY_299 = 298U, /*!< Divided by 299               */
	SIU_CLOCK_DIV_BY_300 = 299U, /*!< Divided by 300               */
	SIU_CLOCK_DIV_BY_301 = 300U, /*!< Divided by 301               */
	SIU_CLOCK_DIV_BY_302 = 301U, /*!< Divided by 302               */
	SIU_CLOCK_DIV_BY_303 = 302U, /*!< Divided by 303               */
	SIU_CLOCK_DIV_BY_304 = 303U, /*!< Divided by 304               */
	SIU_CLOCK_DIV_BY_305 = 304U, /*!< Divided by 305               */
	SIU_CLOCK_DIV_BY_306 = 305U, /*!< Divided by 306               */
	SIU_CLOCK_DIV_BY_307 = 306U, /*!< Divided by 307               */
	SIU_CLOCK_DIV_BY_308 = 307U, /*!< Divided by 308               */
	SIU_CLOCK_DIV_BY_309 = 308U, /*!< Divided by 309               */
	SIU_CLOCK_DIV_BY_310 = 309U, /*!< Divided by 310               */
	SIU_CLOCK_DIV_BY_311 = 310U, /*!< Divided by 311               */
	SIU_CLOCK_DIV_BY_312 = 311U, /*!< Divided by 312               */
	SIU_CLOCK_DIV_BY_313 = 312U, /*!< Divided by 313               */
	SIU_CLOCK_DIV_BY_314 = 313U, /*!< Divided by 314               */
	SIU_CLOCK_DIV_BY_315 = 314U, /*!< Divided by 315               */
	SIU_CLOCK_DIV_BY_316 = 315U, /*!< Divided by 316               */
	SIU_CLOCK_DIV_BY_317 = 316U, /*!< Divided by 317               */
	SIU_CLOCK_DIV_BY_318 = 317U, /*!< Divided by 318               */
	SIU_CLOCK_DIV_BY_319 = 318U, /*!< Divided by 319               */
	SIU_CLOCK_DIV_BY_320 = 319U, /*!< Divided by 320               */
	SIU_CLOCK_DIV_BY_321 = 320U, /*!< Divided by 321               */
	SIU_CLOCK_DIV_BY_322 = 321U, /*!< Divided by 322               */
	SIU_CLOCK_DIV_BY_323 = 322U, /*!< Divided by 323               */
	SIU_CLOCK_DIV_BY_324 = 323U, /*!< Divided by 324               */
	SIU_CLOCK_DIV_BY_325 = 324U, /*!< Divided by 325               */
	SIU_CLOCK_DIV_BY_326 = 325U, /*!< Divided by 326               */
	SIU_CLOCK_DIV_BY_327 = 326U, /*!< Divided by 327               */
	SIU_CLOCK_DIV_BY_328 = 327U, /*!< Divided by 328               */
	SIU_CLOCK_DIV_BY_329 = 328U, /*!< Divided by 329               */
	SIU_CLOCK_DIV_BY_330 = 329U, /*!< Divided by 330               */
	SIU_CLOCK_DIV_BY_331 = 330U, /*!< Divided by 331               */
	SIU_CLOCK_DIV_BY_332 = 331U, /*!< Divided by 332               */
	SIU_CLOCK_DIV_BY_333 = 332U, /*!< Divided by 333               */
	SIU_CLOCK_DIV_BY_334 = 333U, /*!< Divided by 334               */
	SIU_CLOCK_DIV_BY_335 = 334U, /*!< Divided by 335               */
	SIU_CLOCK_DIV_BY_336 = 335U, /*!< Divided by 336               */
	SIU_CLOCK_DIV_BY_337 = 336U, /*!< Divided by 337               */
	SIU_CLOCK_DIV_BY_338 = 337U, /*!< Divided by 338               */
	SIU_CLOCK_DIV_BY_339 = 338U, /*!< Divided by 339               */
	SIU_CLOCK_DIV_BY_340 = 339U, /*!< Divided by 340               */
	SIU_CLOCK_DIV_BY_341 = 340U, /*!< Divided by 341               */
	SIU_CLOCK_DIV_BY_342 = 341U, /*!< Divided by 342               */
	SIU_CLOCK_DIV_BY_343 = 342U, /*!< Divided by 343               */
	SIU_CLOCK_DIV_BY_344 = 343U, /*!< Divided by 344               */
	SIU_CLOCK_DIV_BY_345 = 344U, /*!< Divided by 345               */
	SIU_CLOCK_DIV_BY_346 = 345U, /*!< Divided by 346               */
	SIU_CLOCK_DIV_BY_347 = 346U, /*!< Divided by 347               */
	SIU_CLOCK_DIV_BY_348 = 347U, /*!< Divided by 348               */
	SIU_CLOCK_DIV_BY_349 = 348U, /*!< Divided by 349               */
	SIU_CLOCK_DIV_BY_350 = 349U, /*!< Divided by 350               */
	SIU_CLOCK_DIV_BY_351 = 350U, /*!< Divided by 351               */
	SIU_CLOCK_DIV_BY_352 = 351U, /*!< Divided by 352               */
	SIU_CLOCK_DIV_BY_353 = 352U, /*!< Divided by 353               */
	SIU_CLOCK_DIV_BY_354 = 353U, /*!< Divided by 354               */
	SIU_CLOCK_DIV_BY_355 = 354U, /*!< Divided by 355               */
	SIU_CLOCK_DIV_BY_356 = 355U, /*!< Divided by 356               */
	SIU_CLOCK_DIV_BY_357 = 356U, /*!< Divided by 357               */
	SIU_CLOCK_DIV_BY_358 = 357U, /*!< Divided by 358               */
	SIU_CLOCK_DIV_BY_359 = 358U, /*!< Divided by 359               */
	SIU_CLOCK_DIV_BY_360 = 359U, /*!< Divided by 360               */
	SIU_CLOCK_DIV_BY_361 = 360U, /*!< Divided by 361               */
	SIU_CLOCK_DIV_BY_362 = 361U, /*!< Divided by 362               */
	SIU_CLOCK_DIV_BY_363 = 362U, /*!< Divided by 363               */
	SIU_CLOCK_DIV_BY_364 = 363U, /*!< Divided by 364               */
	SIU_CLOCK_DIV_BY_365 = 364U, /*!< Divided by 365               */
	SIU_CLOCK_DIV_BY_366 = 365U, /*!< Divided by 366               */
	SIU_CLOCK_DIV_BY_367 = 366U, /*!< Divided by 367               */
	SIU_CLOCK_DIV_BY_368 = 367U, /*!< Divided by 368               */
	SIU_CLOCK_DIV_BY_369 = 368U, /*!< Divided by 369               */
	SIU_CLOCK_DIV_BY_370 = 369U, /*!< Divided by 370               */
	SIU_CLOCK_DIV_BY_371 = 370U, /*!< Divided by 371               */
	SIU_CLOCK_DIV_BY_372 = 371U, /*!< Divided by 372               */
	SIU_CLOCK_DIV_BY_373 = 372U, /*!< Divided by 373               */
	SIU_CLOCK_DIV_BY_374 = 373U, /*!< Divided by 374               */
	SIU_CLOCK_DIV_BY_375 = 374U, /*!< Divided by 375               */
	SIU_CLOCK_DIV_BY_376 = 375U, /*!< Divided by 376               */
	SIU_CLOCK_DIV_BY_377 = 376U, /*!< Divided by 377               */
	SIU_CLOCK_DIV_BY_378 = 377U, /*!< Divided by 378               */
	SIU_CLOCK_DIV_BY_379 = 378U, /*!< Divided by 379               */
	SIU_CLOCK_DIV_BY_380 = 379U, /*!< Divided by 380               */
	SIU_CLOCK_DIV_BY_381 = 380U, /*!< Divided by 381               */
	SIU_CLOCK_DIV_BY_382 = 381U, /*!< Divided by 382               */
	SIU_CLOCK_DIV_BY_383 = 382U, /*!< Divided by 383               */
	SIU_CLOCK_DIV_BY_384 = 383U, /*!< Divided by 384               */
	SIU_CLOCK_DIV_BY_385 = 384U, /*!< Divided by 385               */
	SIU_CLOCK_DIV_BY_386 = 385U, /*!< Divided by 386               */
	SIU_CLOCK_DIV_BY_387 = 386U, /*!< Divided by 387               */
	SIU_CLOCK_DIV_BY_388 = 387U, /*!< Divided by 388               */
	SIU_CLOCK_DIV_BY_389 = 388U, /*!< Divided by 389               */
	SIU_CLOCK_DIV_BY_390 = 389U, /*!< Divided by 390               */
	SIU_CLOCK_DIV_BY_391 = 390U, /*!< Divided by 391               */
	SIU_CLOCK_DIV_BY_392 = 391U, /*!< Divided by 392               */
	SIU_CLOCK_DIV_BY_393 = 392U, /*!< Divided by 393               */
	SIU_CLOCK_DIV_BY_394 = 393U, /*!< Divided by 394               */
	SIU_CLOCK_DIV_BY_395 = 394U, /*!< Divided by 395               */
	SIU_CLOCK_DIV_BY_396 = 395U, /*!< Divided by 396               */
	SIU_CLOCK_DIV_BY_397 = 396U, /*!< Divided by 397               */
	SIU_CLOCK_DIV_BY_398 = 397U, /*!< Divided by 398               */
	SIU_CLOCK_DIV_BY_399 = 398U, /*!< Divided by 399               */
	SIU_CLOCK_DIV_BY_400 = 399U, /*!< Divided by 400               */
	SIU_CLOCK_DIV_BY_401 = 400U, /*!< Divided by 401               */
	SIU_CLOCK_DIV_BY_402 = 401U, /*!< Divided by 402               */
	SIU_CLOCK_DIV_BY_403 = 402U, /*!< Divided by 403               */
	SIU_CLOCK_DIV_BY_404 = 403U, /*!< Divided by 404               */
	SIU_CLOCK_DIV_BY_405 = 404U, /*!< Divided by 405               */
	SIU_CLOCK_DIV_BY_406 = 405U, /*!< Divided by 406               */
	SIU_CLOCK_DIV_BY_407 = 406U, /*!< Divided by 407               */
	SIU_CLOCK_DIV_BY_408 = 407U, /*!< Divided by 408               */
	SIU_CLOCK_DIV_BY_409 = 408U, /*!< Divided by 409               */
	SIU_CLOCK_DIV_BY_410 = 409U, /*!< Divided by 410               */
	SIU_CLOCK_DIV_BY_411 = 410U, /*!< Divided by 411               */
	SIU_CLOCK_DIV_BY_412 = 411U, /*!< Divided by 412               */
	SIU_CLOCK_DIV_BY_413 = 412U, /*!< Divided by 413               */
	SIU_CLOCK_DIV_BY_414 = 413U, /*!< Divided by 414               */
	SIU_CLOCK_DIV_BY_415 = 414U, /*!< Divided by 415               */
	SIU_CLOCK_DIV_BY_416 = 415U, /*!< Divided by 416               */
	SIU_CLOCK_DIV_BY_417 = 416U, /*!< Divided by 417               */
	SIU_CLOCK_DIV_BY_418 = 417U, /*!< Divided by 418               */
	SIU_CLOCK_DIV_BY_419 = 418U, /*!< Divided by 419               */
	SIU_CLOCK_DIV_BY_420 = 419U, /*!< Divided by 420               */
	SIU_CLOCK_DIV_BY_421 = 420U, /*!< Divided by 421               */
	SIU_CLOCK_DIV_BY_422 = 421U, /*!< Divided by 422               */
	SIU_CLOCK_DIV_BY_423 = 422U, /*!< Divided by 423               */
	SIU_CLOCK_DIV_BY_424 = 423U, /*!< Divided by 424               */
	SIU_CLOCK_DIV_BY_425 = 424U, /*!< Divided by 425               */
	SIU_CLOCK_DIV_BY_426 = 425U, /*!< Divided by 426               */
	SIU_CLOCK_DIV_BY_427 = 426U, /*!< Divided by 427               */
	SIU_CLOCK_DIV_BY_428 = 427U, /*!< Divided by 428               */
	SIU_CLOCK_DIV_BY_429 = 428U, /*!< Divided by 429               */
	SIU_CLOCK_DIV_BY_430 = 429U, /*!< Divided by 430               */
	SIU_CLOCK_DIV_BY_431 = 430U, /*!< Divided by 431               */
	SIU_CLOCK_DIV_BY_432 = 431U, /*!< Divided by 432               */
	SIU_CLOCK_DIV_BY_433 = 432U, /*!< Divided by 433               */
	SIU_CLOCK_DIV_BY_434 = 433U, /*!< Divided by 434               */
	SIU_CLOCK_DIV_BY_435 = 434U, /*!< Divided by 435               */
	SIU_CLOCK_DIV_BY_436 = 435U, /*!< Divided by 436               */
	SIU_CLOCK_DIV_BY_437 = 436U, /*!< Divided by 437               */
	SIU_CLOCK_DIV_BY_438 = 437U, /*!< Divided by 438               */
	SIU_CLOCK_DIV_BY_439 = 438U, /*!< Divided by 439               */
	SIU_CLOCK_DIV_BY_440 = 439U, /*!< Divided by 440               */
	SIU_CLOCK_DIV_BY_441 = 440U, /*!< Divided by 441               */
	SIU_CLOCK_DIV_BY_442 = 441U, /*!< Divided by 442               */
	SIU_CLOCK_DIV_BY_443 = 442U, /*!< Divided by 443               */
	SIU_CLOCK_DIV_BY_444 = 443U, /*!< Divided by 444               */
	SIU_CLOCK_DIV_BY_445 = 444U, /*!< Divided by 445               */
	SIU_CLOCK_DIV_BY_446 = 445U, /*!< Divided by 446               */
	SIU_CLOCK_DIV_BY_447 = 446U, /*!< Divided by 447               */
	SIU_CLOCK_DIV_BY_448 = 447U, /*!< Divided by 448               */
	SIU_CLOCK_DIV_BY_449 = 448U, /*!< Divided by 449               */
	SIU_CLOCK_DIV_BY_450 = 449U, /*!< Divided by 450               */
	SIU_CLOCK_DIV_BY_451 = 450U, /*!< Divided by 451               */
	SIU_CLOCK_DIV_BY_452 = 451U, /*!< Divided by 452               */
	SIU_CLOCK_DIV_BY_453 = 452U, /*!< Divided by 453               */
	SIU_CLOCK_DIV_BY_454 = 453U, /*!< Divided by 454               */
	SIU_CLOCK_DIV_BY_455 = 454U, /*!< Divided by 455               */
	SIU_CLOCK_DIV_BY_456 = 455U, /*!< Divided by 456               */
	SIU_CLOCK_DIV_BY_457 = 456U, /*!< Divided by 457               */
	SIU_CLOCK_DIV_BY_458 = 457U, /*!< Divided by 458               */
	SIU_CLOCK_DIV_BY_459 = 458U, /*!< Divided by 459               */
	SIU_CLOCK_DIV_BY_460 = 459U, /*!< Divided by 460               */
	SIU_CLOCK_DIV_BY_461 = 460U, /*!< Divided by 461               */
	SIU_CLOCK_DIV_BY_462 = 461U, /*!< Divided by 462               */
	SIU_CLOCK_DIV_BY_463 = 462U, /*!< Divided by 463               */
	SIU_CLOCK_DIV_BY_464 = 463U, /*!< Divided by 464               */
	SIU_CLOCK_DIV_BY_465 = 464U, /*!< Divided by 465               */
	SIU_CLOCK_DIV_BY_466 = 465U, /*!< Divided by 466               */
	SIU_CLOCK_DIV_BY_467 = 466U, /*!< Divided by 467               */
	SIU_CLOCK_DIV_BY_468 = 467U, /*!< Divided by 468               */
	SIU_CLOCK_DIV_BY_469 = 468U, /*!< Divided by 469               */
	SIU_CLOCK_DIV_BY_470 = 469U, /*!< Divided by 470               */
	SIU_CLOCK_DIV_BY_471 = 470U, /*!< Divided by 471               */
	SIU_CLOCK_DIV_BY_472 = 471U, /*!< Divided by 472               */
	SIU_CLOCK_DIV_BY_473 = 472U, /*!< Divided by 473               */
	SIU_CLOCK_DIV_BY_474 = 473U, /*!< Divided by 474               */
	SIU_CLOCK_DIV_BY_475 = 474U, /*!< Divided by 475               */
	SIU_CLOCK_DIV_BY_476 = 475U, /*!< Divided by 476               */
	SIU_CLOCK_DIV_BY_477 = 476U, /*!< Divided by 477               */
	SIU_CLOCK_DIV_BY_478 = 477U, /*!< Divided by 478               */
	SIU_CLOCK_DIV_BY_479 = 478U, /*!< Divided by 479               */
	SIU_CLOCK_DIV_BY_480 = 479U, /*!< Divided by 480               */
	SIU_CLOCK_DIV_BY_481 = 480U, /*!< Divided by 481               */
	SIU_CLOCK_DIV_BY_482 = 481U, /*!< Divided by 482               */
	SIU_CLOCK_DIV_BY_483 = 482U, /*!< Divided by 483               */
	SIU_CLOCK_DIV_BY_484 = 483U, /*!< Divided by 484               */
	SIU_CLOCK_DIV_BY_485 = 484U, /*!< Divided by 485               */
	SIU_CLOCK_DIV_BY_486 = 485U, /*!< Divided by 486               */
	SIU_CLOCK_DIV_BY_487 = 486U, /*!< Divided by 487               */
	SIU_CLOCK_DIV_BY_488 = 487U, /*!< Divided by 488               */
	SIU_CLOCK_DIV_BY_489 = 488U, /*!< Divided by 489               */
	SIU_CLOCK_DIV_BY_490 = 489U, /*!< Divided by 490               */
	SIU_CLOCK_DIV_BY_491 = 490U, /*!< Divided by 491               */
	SIU_CLOCK_DIV_BY_492 = 491U, /*!< Divided by 492               */
	SIU_CLOCK_DIV_BY_493 = 492U, /*!< Divided by 493               */
	SIU_CLOCK_DIV_BY_494 = 493U, /*!< Divided by 494               */
	SIU_CLOCK_DIV_BY_495 = 494U, /*!< Divided by 495               */
	SIU_CLOCK_DIV_BY_496 = 495U, /*!< Divided by 496               */
	SIU_CLOCK_DIV_BY_497 = 496U, /*!< Divided by 497               */
	SIU_CLOCK_DIV_BY_498 = 497U, /*!< Divided by 498               */
	SIU_CLOCK_DIV_BY_499 = 498U, /*!< Divided by 499               */
	SIU_CLOCK_DIV_BY_500 = 499U, /*!< Divided by 500               */
	SIU_CLOCK_DIV_BY_501 = 500U, /*!< Divided by 501               */
	SIU_CLOCK_DIV_BY_502 = 501U, /*!< Divided by 502               */
	SIU_CLOCK_DIV_BY_503 = 502U, /*!< Divided by 503               */
	SIU_CLOCK_DIV_BY_504 = 503U, /*!< Divided by 504               */
	SIU_CLOCK_DIV_BY_505 = 504U, /*!< Divided by 505               */
	SIU_CLOCK_DIV_BY_506 = 505U, /*!< Divided by 506               */
	SIU_CLOCK_DIV_BY_507 = 506U, /*!< Divided by 507               */
	SIU_CLOCK_DIV_BY_508 = 507U, /*!< Divided by 508               */
	SIU_CLOCK_DIV_BY_509 = 508U, /*!< Divided by 509               */
	SIU_CLOCK_DIV_BY_510 = 509U, /*!< Divided by 510               */
	SIU_CLOCK_DIV_BY_511 = 510U, /*!< Divided by 511               */
	SIU_CLOCK_DIV_BY_512 = 511U, /*!< Divided by 512               */
	SIU_CLOCK_DIV_BY_513 = 512U, /*!< Divided by 513               */
	SIU_CLOCK_DIV_BY_514 = 513U, /*!< Divided by 514               */
	SIU_CLOCK_DIV_BY_515 = 514U, /*!< Divided by 515               */
	SIU_CLOCK_DIV_BY_516 = 515U, /*!< Divided by 516               */
	SIU_CLOCK_DIV_BY_517 = 516U, /*!< Divided by 517               */
	SIU_CLOCK_DIV_BY_518 = 517U, /*!< Divided by 518               */
	SIU_CLOCK_DIV_BY_519 = 518U, /*!< Divided by 519               */
	SIU_CLOCK_DIV_BY_520 = 519U, /*!< Divided by 520               */
	SIU_CLOCK_DIV_BY_521 = 520U, /*!< Divided by 521               */
	SIU_CLOCK_DIV_BY_522 = 521U, /*!< Divided by 522               */
	SIU_CLOCK_DIV_BY_523 = 522U, /*!< Divided by 523               */
	SIU_CLOCK_DIV_BY_524 = 523U, /*!< Divided by 524               */
	SIU_CLOCK_DIV_BY_525 = 524U, /*!< Divided by 525               */
	SIU_CLOCK_DIV_BY_526 = 525U, /*!< Divided by 526               */
	SIU_CLOCK_DIV_BY_527 = 526U, /*!< Divided by 527               */
	SIU_CLOCK_DIV_BY_528 = 527U, /*!< Divided by 528               */
	SIU_CLOCK_DIV_BY_529 = 528U, /*!< Divided by 529               */
	SIU_CLOCK_DIV_BY_530 = 529U, /*!< Divided by 530               */
	SIU_CLOCK_DIV_BY_531 = 530U, /*!< Divided by 531               */
	SIU_CLOCK_DIV_BY_532 = 531U, /*!< Divided by 532               */
	SIU_CLOCK_DIV_BY_533 = 532U, /*!< Divided by 533               */
	SIU_CLOCK_DIV_BY_534 = 533U, /*!< Divided by 534               */
	SIU_CLOCK_DIV_BY_535 = 534U, /*!< Divided by 535               */
	SIU_CLOCK_DIV_BY_536 = 535U, /*!< Divided by 536               */
	SIU_CLOCK_DIV_BY_537 = 536U, /*!< Divided by 537               */
	SIU_CLOCK_DIV_BY_538 = 537U, /*!< Divided by 538               */
	SIU_CLOCK_DIV_BY_539 = 538U, /*!< Divided by 539               */
	SIU_CLOCK_DIV_BY_540 = 539U, /*!< Divided by 540               */
	SIU_CLOCK_DIV_BY_541 = 540U, /*!< Divided by 541               */
	SIU_CLOCK_DIV_BY_542 = 541U, /*!< Divided by 542               */
	SIU_CLOCK_DIV_BY_543 = 542U, /*!< Divided by 543               */
	SIU_CLOCK_DIV_BY_544 = 543U, /*!< Divided by 544               */
	SIU_CLOCK_DIV_BY_545 = 544U, /*!< Divided by 545               */
	SIU_CLOCK_DIV_BY_546 = 545U, /*!< Divided by 546               */
	SIU_CLOCK_DIV_BY_547 = 546U, /*!< Divided by 547               */
	SIU_CLOCK_DIV_BY_548 = 547U, /*!< Divided by 548               */
	SIU_CLOCK_DIV_BY_549 = 548U, /*!< Divided by 549               */
	SIU_CLOCK_DIV_BY_550 = 549U, /*!< Divided by 550               */
	SIU_CLOCK_DIV_BY_551 = 550U, /*!< Divided by 551               */
	SIU_CLOCK_DIV_BY_552 = 551U, /*!< Divided by 552               */
	SIU_CLOCK_DIV_BY_553 = 552U, /*!< Divided by 553               */
	SIU_CLOCK_DIV_BY_554 = 553U, /*!< Divided by 554               */
	SIU_CLOCK_DIV_BY_555 = 554U, /*!< Divided by 555               */
	SIU_CLOCK_DIV_BY_556 = 555U, /*!< Divided by 556               */
	SIU_CLOCK_DIV_BY_557 = 556U, /*!< Divided by 557               */
	SIU_CLOCK_DIV_BY_558 = 557U, /*!< Divided by 558               */
	SIU_CLOCK_DIV_BY_559 = 558U, /*!< Divided by 559               */
	SIU_CLOCK_DIV_BY_560 = 559U, /*!< Divided by 560               */
	SIU_CLOCK_DIV_BY_561 = 560U, /*!< Divided by 561               */
	SIU_CLOCK_DIV_BY_562 = 561U, /*!< Divided by 562               */
	SIU_CLOCK_DIV_BY_563 = 562U, /*!< Divided by 563               */
	SIU_CLOCK_DIV_BY_564 = 563U, /*!< Divided by 564               */
	SIU_CLOCK_DIV_BY_565 = 564U, /*!< Divided by 565               */
	SIU_CLOCK_DIV_BY_566 = 565U, /*!< Divided by 566               */
	SIU_CLOCK_DIV_BY_567 = 566U, /*!< Divided by 567               */
	SIU_CLOCK_DIV_BY_568 = 567U, /*!< Divided by 568               */
	SIU_CLOCK_DIV_BY_569 = 568U, /*!< Divided by 569               */
	SIU_CLOCK_DIV_BY_570 = 569U, /*!< Divided by 570               */
	SIU_CLOCK_DIV_BY_571 = 570U, /*!< Divided by 571               */
	SIU_CLOCK_DIV_BY_572 = 571U, /*!< Divided by 572               */
	SIU_CLOCK_DIV_BY_573 = 572U, /*!< Divided by 573               */
	SIU_CLOCK_DIV_BY_574 = 573U, /*!< Divided by 574               */
	SIU_CLOCK_DIV_BY_575 = 574U, /*!< Divided by 575               */
	SIU_CLOCK_DIV_BY_576 = 575U, /*!< Divided by 576               */
	SIU_CLOCK_DIV_BY_577 = 576U, /*!< Divided by 577               */
	SIU_CLOCK_DIV_BY_578 = 577U, /*!< Divided by 578               */
	SIU_CLOCK_DIV_BY_579 = 578U, /*!< Divided by 579               */
	SIU_CLOCK_DIV_BY_580 = 579U, /*!< Divided by 580               */
	SIU_CLOCK_DIV_BY_581 = 580U, /*!< Divided by 581               */
	SIU_CLOCK_DIV_BY_582 = 581U, /*!< Divided by 582               */
	SIU_CLOCK_DIV_BY_583 = 582U, /*!< Divided by 583               */
	SIU_CLOCK_DIV_BY_584 = 583U, /*!< Divided by 584               */
	SIU_CLOCK_DIV_BY_585 = 584U, /*!< Divided by 585               */
	SIU_CLOCK_DIV_BY_586 = 585U, /*!< Divided by 586               */
	SIU_CLOCK_DIV_BY_587 = 586U, /*!< Divided by 587               */
	SIU_CLOCK_DIV_BY_588 = 587U, /*!< Divided by 588               */
	SIU_CLOCK_DIV_BY_589 = 588U, /*!< Divided by 589               */
	SIU_CLOCK_DIV_BY_590 = 589U, /*!< Divided by 590               */
	SIU_CLOCK_DIV_BY_591 = 590U, /*!< Divided by 591               */
	SIU_CLOCK_DIV_BY_592 = 591U, /*!< Divided by 592               */
	SIU_CLOCK_DIV_BY_593 = 592U, /*!< Divided by 593               */
	SIU_CLOCK_DIV_BY_594 = 593U, /*!< Divided by 594               */
	SIU_CLOCK_DIV_BY_595 = 594U, /*!< Divided by 595               */
	SIU_CLOCK_DIV_BY_596 = 595U, /*!< Divided by 596               */
	SIU_CLOCK_DIV_BY_597 = 596U, /*!< Divided by 597               */
	SIU_CLOCK_DIV_BY_598 = 597U, /*!< Divided by 598               */
	SIU_CLOCK_DIV_BY_599 = 598U, /*!< Divided by 599               */
	SIU_CLOCK_DIV_BY_600 = 599U, /*!< Divided by 600               */
	SIU_CLOCK_DIV_BY_601 = 600U, /*!< Divided by 601               */
	SIU_CLOCK_DIV_BY_602 = 601U, /*!< Divided by 602               */
	SIU_CLOCK_DIV_BY_603 = 602U, /*!< Divided by 603               */
	SIU_CLOCK_DIV_BY_604 = 603U, /*!< Divided by 604               */
	SIU_CLOCK_DIV_BY_605 = 604U, /*!< Divided by 605               */
	SIU_CLOCK_DIV_BY_606 = 605U, /*!< Divided by 606               */
	SIU_CLOCK_DIV_BY_607 = 606U, /*!< Divided by 607               */
	SIU_CLOCK_DIV_BY_608 = 607U, /*!< Divided by 608               */
	SIU_CLOCK_DIV_BY_609 = 608U, /*!< Divided by 609               */
	SIU_CLOCK_DIV_BY_610 = 609U, /*!< Divided by 610               */
	SIU_CLOCK_DIV_BY_611 = 610U, /*!< Divided by 611               */
	SIU_CLOCK_DIV_BY_612 = 611U, /*!< Divided by 612               */
	SIU_CLOCK_DIV_BY_613 = 612U, /*!< Divided by 613               */
	SIU_CLOCK_DIV_BY_614 = 613U, /*!< Divided by 614               */
	SIU_CLOCK_DIV_BY_615 = 614U, /*!< Divided by 615               */
	SIU_CLOCK_DIV_BY_616 = 615U, /*!< Divided by 616               */
	SIU_CLOCK_DIV_BY_617 = 616U, /*!< Divided by 617               */
	SIU_CLOCK_DIV_BY_618 = 617U, /*!< Divided by 618               */
	SIU_CLOCK_DIV_BY_619 = 618U, /*!< Divided by 619               */
	SIU_CLOCK_DIV_BY_620 = 619U, /*!< Divided by 620               */
	SIU_CLOCK_DIV_BY_621 = 620U, /*!< Divided by 621               */
	SIU_CLOCK_DIV_BY_622 = 621U, /*!< Divided by 622               */
	SIU_CLOCK_DIV_BY_623 = 622U, /*!< Divided by 623               */
	SIU_CLOCK_DIV_BY_624 = 623U, /*!< Divided by 624               */
	SIU_CLOCK_DIV_BY_625 = 624U, /*!< Divided by 625               */
	SIU_CLOCK_DIV_BY_626 = 625U, /*!< Divided by 626               */
	SIU_CLOCK_DIV_BY_627 = 626U, /*!< Divided by 627               */
	SIU_CLOCK_DIV_BY_628 = 627U, /*!< Divided by 628               */
	SIU_CLOCK_DIV_BY_629 = 628U, /*!< Divided by 629               */
	SIU_CLOCK_DIV_BY_630 = 629U, /*!< Divided by 630               */
	SIU_CLOCK_DIV_BY_631 = 630U, /*!< Divided by 631               */
	SIU_CLOCK_DIV_BY_632 = 631U, /*!< Divided by 632               */
	SIU_CLOCK_DIV_BY_633 = 632U, /*!< Divided by 633               */
	SIU_CLOCK_DIV_BY_634 = 633U, /*!< Divided by 634               */
	SIU_CLOCK_DIV_BY_635 = 634U, /*!< Divided by 635               */
	SIU_CLOCK_DIV_BY_636 = 635U, /*!< Divided by 636               */
	SIU_CLOCK_DIV_BY_637 = 636U, /*!< Divided by 637               */
	SIU_CLOCK_DIV_BY_638 = 637U, /*!< Divided by 638               */
	SIU_CLOCK_DIV_BY_639 = 638U, /*!< Divided by 639               */
	SIU_CLOCK_DIV_BY_640 = 639U, /*!< Divided by 640               */
	SIU_CLOCK_DIV_BY_641 = 640U, /*!< Divided by 641               */
	SIU_CLOCK_DIV_BY_642 = 641U, /*!< Divided by 642               */
	SIU_CLOCK_DIV_BY_643 = 642U, /*!< Divided by 643               */
	SIU_CLOCK_DIV_BY_644 = 643U, /*!< Divided by 644               */
	SIU_CLOCK_DIV_BY_645 = 644U, /*!< Divided by 645               */
	SIU_CLOCK_DIV_BY_646 = 645U, /*!< Divided by 646               */
	SIU_CLOCK_DIV_BY_647 = 646U, /*!< Divided by 647               */
	SIU_CLOCK_DIV_BY_648 = 647U, /*!< Divided by 648               */
	SIU_CLOCK_DIV_BY_649 = 648U, /*!< Divided by 649               */
	SIU_CLOCK_DIV_BY_650 = 649U, /*!< Divided by 650               */
	SIU_CLOCK_DIV_BY_651 = 650U, /*!< Divided by 651               */
	SIU_CLOCK_DIV_BY_652 = 651U, /*!< Divided by 652               */
	SIU_CLOCK_DIV_BY_653 = 652U, /*!< Divided by 653               */
	SIU_CLOCK_DIV_BY_654 = 653U, /*!< Divided by 654               */
	SIU_CLOCK_DIV_BY_655 = 654U, /*!< Divided by 655               */
	SIU_CLOCK_DIV_BY_656 = 655U, /*!< Divided by 656               */
	SIU_CLOCK_DIV_BY_657 = 656U, /*!< Divided by 657               */
	SIU_CLOCK_DIV_BY_658 = 657U, /*!< Divided by 658               */
	SIU_CLOCK_DIV_BY_659 = 658U, /*!< Divided by 659               */
	SIU_CLOCK_DIV_BY_660 = 659U, /*!< Divided by 660               */
	SIU_CLOCK_DIV_BY_661 = 660U, /*!< Divided by 661               */
	SIU_CLOCK_DIV_BY_662 = 661U, /*!< Divided by 662               */
	SIU_CLOCK_DIV_BY_663 = 662U, /*!< Divided by 663               */
	SIU_CLOCK_DIV_BY_664 = 663U, /*!< Divided by 664               */
	SIU_CLOCK_DIV_BY_665 = 664U, /*!< Divided by 665               */
	SIU_CLOCK_DIV_BY_666 = 665U, /*!< Divided by 666               */
	SIU_CLOCK_DIV_BY_667 = 666U, /*!< Divided by 667               */
	SIU_CLOCK_DIV_BY_668 = 667U, /*!< Divided by 668               */
	SIU_CLOCK_DIV_BY_669 = 668U, /*!< Divided by 669               */
	SIU_CLOCK_DIV_BY_670 = 669U, /*!< Divided by 670               */
	SIU_CLOCK_DIV_BY_671 = 670U, /*!< Divided by 671               */
	SIU_CLOCK_DIV_BY_672 = 671U, /*!< Divided by 672               */
	SIU_CLOCK_DIV_BY_673 = 672U, /*!< Divided by 673               */
	SIU_CLOCK_DIV_BY_674 = 673U, /*!< Divided by 674               */
	SIU_CLOCK_DIV_BY_675 = 674U, /*!< Divided by 675               */
	SIU_CLOCK_DIV_BY_676 = 675U, /*!< Divided by 676               */
	SIU_CLOCK_DIV_BY_677 = 676U, /*!< Divided by 677               */
	SIU_CLOCK_DIV_BY_678 = 677U, /*!< Divided by 678               */
	SIU_CLOCK_DIV_BY_679 = 678U, /*!< Divided by 679               */
	SIU_CLOCK_DIV_BY_680 = 679U, /*!< Divided by 680               */
	SIU_CLOCK_DIV_BY_681 = 680U, /*!< Divided by 681               */
	SIU_CLOCK_DIV_BY_682 = 681U, /*!< Divided by 682               */
	SIU_CLOCK_DIV_BY_683 = 682U, /*!< Divided by 683               */
	SIU_CLOCK_DIV_BY_684 = 683U, /*!< Divided by 684               */
	SIU_CLOCK_DIV_BY_685 = 684U, /*!< Divided by 685               */
	SIU_CLOCK_DIV_BY_686 = 685U, /*!< Divided by 686               */
	SIU_CLOCK_DIV_BY_687 = 686U, /*!< Divided by 687               */
	SIU_CLOCK_DIV_BY_688 = 687U, /*!< Divided by 688               */
	SIU_CLOCK_DIV_BY_689 = 688U, /*!< Divided by 689               */
	SIU_CLOCK_DIV_BY_690 = 689U, /*!< Divided by 690               */
	SIU_CLOCK_DIV_BY_691 = 690U, /*!< Divided by 691               */
	SIU_CLOCK_DIV_BY_692 = 691U, /*!< Divided by 692               */
	SIU_CLOCK_DIV_BY_693 = 692U, /*!< Divided by 693               */
	SIU_CLOCK_DIV_BY_694 = 693U, /*!< Divided by 694               */
	SIU_CLOCK_DIV_BY_695 = 694U, /*!< Divided by 695               */
	SIU_CLOCK_DIV_BY_696 = 695U, /*!< Divided by 696               */
	SIU_CLOCK_DIV_BY_697 = 696U, /*!< Divided by 697               */
	SIU_CLOCK_DIV_BY_698 = 697U, /*!< Divided by 698               */
	SIU_CLOCK_DIV_BY_699 = 698U, /*!< Divided by 699               */
	SIU_CLOCK_DIV_BY_700 = 699U, /*!< Divided by 700               */
	SIU_CLOCK_DIV_BY_701 = 700U, /*!< Divided by 701               */
	SIU_CLOCK_DIV_BY_702 = 701U, /*!< Divided by 702               */
	SIU_CLOCK_DIV_BY_703 = 702U, /*!< Divided by 703               */
	SIU_CLOCK_DIV_BY_704 = 703U, /*!< Divided by 704               */
	SIU_CLOCK_DIV_BY_705 = 704U, /*!< Divided by 705               */
	SIU_CLOCK_DIV_BY_706 = 705U, /*!< Divided by 706               */
	SIU_CLOCK_DIV_BY_707 = 706U, /*!< Divided by 707               */
	SIU_CLOCK_DIV_BY_708 = 707U, /*!< Divided by 708               */
	SIU_CLOCK_DIV_BY_709 = 708U, /*!< Divided by 709               */
	SIU_CLOCK_DIV_BY_710 = 709U, /*!< Divided by 710               */
	SIU_CLOCK_DIV_BY_711 = 710U, /*!< Divided by 711               */
	SIU_CLOCK_DIV_BY_712 = 711U, /*!< Divided by 712               */
	SIU_CLOCK_DIV_BY_713 = 712U, /*!< Divided by 713               */
	SIU_CLOCK_DIV_BY_714 = 713U, /*!< Divided by 714               */
	SIU_CLOCK_DIV_BY_715 = 714U, /*!< Divided by 715               */
	SIU_CLOCK_DIV_BY_716 = 715U, /*!< Divided by 716               */
	SIU_CLOCK_DIV_BY_717 = 716U, /*!< Divided by 717               */
	SIU_CLOCK_DIV_BY_718 = 717U, /*!< Divided by 718               */
	SIU_CLOCK_DIV_BY_719 = 718U, /*!< Divided by 719               */
	SIU_CLOCK_DIV_BY_720 = 719U, /*!< Divided by 720               */
	SIU_CLOCK_DIV_BY_721 = 720U, /*!< Divided by 721               */
	SIU_CLOCK_DIV_BY_722 = 721U, /*!< Divided by 722               */
	SIU_CLOCK_DIV_BY_723 = 722U, /*!< Divided by 723               */
	SIU_CLOCK_DIV_BY_724 = 723U, /*!< Divided by 724               */
	SIU_CLOCK_DIV_BY_725 = 724U, /*!< Divided by 725               */
	SIU_CLOCK_DIV_BY_726 = 725U, /*!< Divided by 726               */
	SIU_CLOCK_DIV_BY_727 = 726U, /*!< Divided by 727               */
	SIU_CLOCK_DIV_BY_728 = 727U, /*!< Divided by 728               */
	SIU_CLOCK_DIV_BY_729 = 728U, /*!< Divided by 729               */
	SIU_CLOCK_DIV_BY_730 = 729U, /*!< Divided by 730               */
	SIU_CLOCK_DIV_BY_731 = 730U, /*!< Divided by 731               */
	SIU_CLOCK_DIV_BY_732 = 731U, /*!< Divided by 732               */
	SIU_CLOCK_DIV_BY_733 = 732U, /*!< Divided by 733               */
	SIU_CLOCK_DIV_BY_734 = 733U, /*!< Divided by 734               */
	SIU_CLOCK_DIV_BY_735 = 734U, /*!< Divided by 735               */
	SIU_CLOCK_DIV_BY_736 = 735U, /*!< Divided by 736               */
	SIU_CLOCK_DIV_BY_737 = 736U, /*!< Divided by 737               */
	SIU_CLOCK_DIV_BY_738 = 737U, /*!< Divided by 738               */
	SIU_CLOCK_DIV_BY_739 = 738U, /*!< Divided by 739               */
	SIU_CLOCK_DIV_BY_740 = 739U, /*!< Divided by 740               */
	SIU_CLOCK_DIV_BY_741 = 740U, /*!< Divided by 741               */
	SIU_CLOCK_DIV_BY_742 = 741U, /*!< Divided by 742               */
	SIU_CLOCK_DIV_BY_743 = 742U, /*!< Divided by 743               */
	SIU_CLOCK_DIV_BY_744 = 743U, /*!< Divided by 744               */
	SIU_CLOCK_DIV_BY_745 = 744U, /*!< Divided by 745               */
	SIU_CLOCK_DIV_BY_746 = 745U, /*!< Divided by 746               */
	SIU_CLOCK_DIV_BY_747 = 746U, /*!< Divided by 747               */
	SIU_CLOCK_DIV_BY_748 = 747U, /*!< Divided by 748               */
	SIU_CLOCK_DIV_BY_749 = 748U, /*!< Divided by 749               */
	SIU_CLOCK_DIV_BY_750 = 749U, /*!< Divided by 750               */
	SIU_CLOCK_DIV_BY_751 = 750U, /*!< Divided by 751               */
	SIU_CLOCK_DIV_BY_752 = 751U, /*!< Divided by 752               */
	SIU_CLOCK_DIV_BY_753 = 752U, /*!< Divided by 753               */
	SIU_CLOCK_DIV_BY_754 = 753U, /*!< Divided by 754               */
	SIU_CLOCK_DIV_BY_755 = 754U, /*!< Divided by 755               */
	SIU_CLOCK_DIV_BY_756 = 755U, /*!< Divided by 756               */
	SIU_CLOCK_DIV_BY_757 = 756U, /*!< Divided by 757               */
	SIU_CLOCK_DIV_BY_758 = 757U, /*!< Divided by 758               */
	SIU_CLOCK_DIV_BY_759 = 758U, /*!< Divided by 759               */
	SIU_CLOCK_DIV_BY_760 = 759U, /*!< Divided by 760               */
	SIU_CLOCK_DIV_BY_761 = 760U, /*!< Divided by 761               */
	SIU_CLOCK_DIV_BY_762 = 761U, /*!< Divided by 762               */
	SIU_CLOCK_DIV_BY_763 = 762U, /*!< Divided by 763               */
	SIU_CLOCK_DIV_BY_764 = 763U, /*!< Divided by 764               */
	SIU_CLOCK_DIV_BY_765 = 764U, /*!< Divided by 765               */
	SIU_CLOCK_DIV_BY_766 = 765U, /*!< Divided by 766               */
	SIU_CLOCK_DIV_BY_767 = 766U, /*!< Divided by 767               */
	SIU_CLOCK_DIV_BY_768 = 767U, /*!< Divided by 768               */
	SIU_CLOCK_DIV_BY_769 = 768U, /*!< Divided by 769               */
	SIU_CLOCK_DIV_BY_770 = 769U, /*!< Divided by 770               */
	SIU_CLOCK_DIV_BY_771 = 770U, /*!< Divided by 771               */
	SIU_CLOCK_DIV_BY_772 = 771U, /*!< Divided by 772               */
	SIU_CLOCK_DIV_BY_773 = 772U, /*!< Divided by 773               */
	SIU_CLOCK_DIV_BY_774 = 773U, /*!< Divided by 774               */
	SIU_CLOCK_DIV_BY_775 = 774U, /*!< Divided by 775               */
	SIU_CLOCK_DIV_BY_776 = 775U, /*!< Divided by 776               */
	SIU_CLOCK_DIV_BY_777 = 776U, /*!< Divided by 777               */
	SIU_CLOCK_DIV_BY_778 = 777U, /*!< Divided by 778               */
	SIU_CLOCK_DIV_BY_779 = 778U, /*!< Divided by 779               */
	SIU_CLOCK_DIV_BY_780 = 779U, /*!< Divided by 780               */
	SIU_CLOCK_DIV_BY_781 = 780U, /*!< Divided by 781               */
	SIU_CLOCK_DIV_BY_782 = 781U, /*!< Divided by 782               */
	SIU_CLOCK_DIV_BY_783 = 782U, /*!< Divided by 783               */
	SIU_CLOCK_DIV_BY_784 = 783U, /*!< Divided by 784               */
	SIU_CLOCK_DIV_BY_785 = 784U, /*!< Divided by 785               */
	SIU_CLOCK_DIV_BY_786 = 785U, /*!< Divided by 786               */
	SIU_CLOCK_DIV_BY_787 = 786U, /*!< Divided by 787               */
	SIU_CLOCK_DIV_BY_788 = 787U, /*!< Divided by 788               */
	SIU_CLOCK_DIV_BY_789 = 788U, /*!< Divided by 789               */
	SIU_CLOCK_DIV_BY_790 = 789U, /*!< Divided by 790               */
	SIU_CLOCK_DIV_BY_791 = 790U, /*!< Divided by 791               */
	SIU_CLOCK_DIV_BY_792 = 791U, /*!< Divided by 792               */
	SIU_CLOCK_DIV_BY_793 = 792U, /*!< Divided by 793               */
	SIU_CLOCK_DIV_BY_794 = 793U, /*!< Divided by 794               */
	SIU_CLOCK_DIV_BY_795 = 794U, /*!< Divided by 795               */
	SIU_CLOCK_DIV_BY_796 = 795U, /*!< Divided by 796               */
	SIU_CLOCK_DIV_BY_797 = 796U, /*!< Divided by 797               */
	SIU_CLOCK_DIV_BY_798 = 797U, /*!< Divided by 798               */
	SIU_CLOCK_DIV_BY_799 = 798U, /*!< Divided by 799               */
	SIU_CLOCK_DIV_BY_800 = 799U, /*!< Divided by 800               */
	SIU_CLOCK_DIV_BY_801 = 800U, /*!< Divided by 801               */
	SIU_CLOCK_DIV_BY_802 = 801U, /*!< Divided by 802               */
	SIU_CLOCK_DIV_BY_803 = 802U, /*!< Divided by 803               */
	SIU_CLOCK_DIV_BY_804 = 803U, /*!< Divided by 804               */
	SIU_CLOCK_DIV_BY_805 = 804U, /*!< Divided by 805               */
	SIU_CLOCK_DIV_BY_806 = 805U, /*!< Divided by 806               */
	SIU_CLOCK_DIV_BY_807 = 806U, /*!< Divided by 807               */
	SIU_CLOCK_DIV_BY_808 = 807U, /*!< Divided by 808               */
	SIU_CLOCK_DIV_BY_809 = 808U, /*!< Divided by 809               */
	SIU_CLOCK_DIV_BY_810 = 809U, /*!< Divided by 810               */
	SIU_CLOCK_DIV_BY_811 = 810U, /*!< Divided by 811               */
	SIU_CLOCK_DIV_BY_812 = 811U, /*!< Divided by 812               */
	SIU_CLOCK_DIV_BY_813 = 812U, /*!< Divided by 813               */
	SIU_CLOCK_DIV_BY_814 = 813U, /*!< Divided by 814               */
	SIU_CLOCK_DIV_BY_815 = 814U, /*!< Divided by 815               */
	SIU_CLOCK_DIV_BY_816 = 815U, /*!< Divided by 816               */
	SIU_CLOCK_DIV_BY_817 = 816U, /*!< Divided by 817               */
	SIU_CLOCK_DIV_BY_818 = 817U, /*!< Divided by 818               */
	SIU_CLOCK_DIV_BY_819 = 818U, /*!< Divided by 819               */
	SIU_CLOCK_DIV_BY_820 = 819U, /*!< Divided by 820               */
	SIU_CLOCK_DIV_BY_821 = 820U, /*!< Divided by 821               */
	SIU_CLOCK_DIV_BY_822 = 821U, /*!< Divided by 822               */
	SIU_CLOCK_DIV_BY_823 = 822U, /*!< Divided by 823               */
	SIU_CLOCK_DIV_BY_824 = 823U, /*!< Divided by 824               */
	SIU_CLOCK_DIV_BY_825 = 824U, /*!< Divided by 825               */
	SIU_CLOCK_DIV_BY_826 = 825U, /*!< Divided by 826               */
	SIU_CLOCK_DIV_BY_827 = 826U, /*!< Divided by 827               */
	SIU_CLOCK_DIV_BY_828 = 827U, /*!< Divided by 828               */
	SIU_CLOCK_DIV_BY_829 = 828U, /*!< Divided by 829               */
	SIU_CLOCK_DIV_BY_830 = 829U, /*!< Divided by 830               */
	SIU_CLOCK_DIV_BY_831 = 830U, /*!< Divided by 831               */
	SIU_CLOCK_DIV_BY_832 = 831U, /*!< Divided by 832               */
	SIU_CLOCK_DIV_BY_833 = 832U, /*!< Divided by 833               */
	SIU_CLOCK_DIV_BY_834 = 833U, /*!< Divided by 834               */
	SIU_CLOCK_DIV_BY_835 = 834U, /*!< Divided by 835               */
	SIU_CLOCK_DIV_BY_836 = 835U, /*!< Divided by 836               */
	SIU_CLOCK_DIV_BY_837 = 836U, /*!< Divided by 837               */
	SIU_CLOCK_DIV_BY_838 = 837U, /*!< Divided by 838               */
	SIU_CLOCK_DIV_BY_839 = 838U, /*!< Divided by 839               */
	SIU_CLOCK_DIV_BY_840 = 839U, /*!< Divided by 840               */
	SIU_CLOCK_DIV_BY_841 = 840U, /*!< Divided by 841               */
	SIU_CLOCK_DIV_BY_842 = 841U, /*!< Divided by 842               */
	SIU_CLOCK_DIV_BY_843 = 842U, /*!< Divided by 843               */
	SIU_CLOCK_DIV_BY_844 = 843U, /*!< Divided by 844               */
	SIU_CLOCK_DIV_BY_845 = 844U, /*!< Divided by 845               */
	SIU_CLOCK_DIV_BY_846 = 845U, /*!< Divided by 846               */
	SIU_CLOCK_DIV_BY_847 = 846U, /*!< Divided by 847               */
	SIU_CLOCK_DIV_BY_848 = 847U, /*!< Divided by 848               */
	SIU_CLOCK_DIV_BY_849 = 848U, /*!< Divided by 849               */
	SIU_CLOCK_DIV_BY_850 = 849U, /*!< Divided by 850               */
	SIU_CLOCK_DIV_BY_851 = 850U, /*!< Divided by 851               */
	SIU_CLOCK_DIV_BY_852 = 851U, /*!< Divided by 852               */
	SIU_CLOCK_DIV_BY_853 = 852U, /*!< Divided by 853               */
	SIU_CLOCK_DIV_BY_854 = 853U, /*!< Divided by 854               */
	SIU_CLOCK_DIV_BY_855 = 854U, /*!< Divided by 855               */
	SIU_CLOCK_DIV_BY_856 = 855U, /*!< Divided by 856               */
	SIU_CLOCK_DIV_BY_857 = 856U, /*!< Divided by 857               */
	SIU_CLOCK_DIV_BY_858 = 857U, /*!< Divided by 858               */
	SIU_CLOCK_DIV_BY_859 = 858U, /*!< Divided by 859               */
	SIU_CLOCK_DIV_BY_860 = 859U, /*!< Divided by 860               */
	SIU_CLOCK_DIV_BY_861 = 860U, /*!< Divided by 861               */
	SIU_CLOCK_DIV_BY_862 = 861U, /*!< Divided by 862               */
	SIU_CLOCK_DIV_BY_863 = 862U, /*!< Divided by 863               */
	SIU_CLOCK_DIV_BY_864 = 863U, /*!< Divided by 864               */
	SIU_CLOCK_DIV_BY_865 = 864U, /*!< Divided by 865               */
	SIU_CLOCK_DIV_BY_866 = 865U, /*!< Divided by 866               */
	SIU_CLOCK_DIV_BY_867 = 866U, /*!< Divided by 867               */
	SIU_CLOCK_DIV_BY_868 = 867U, /*!< Divided by 868               */
	SIU_CLOCK_DIV_BY_869 = 868U, /*!< Divided by 869               */
	SIU_CLOCK_DIV_BY_870 = 869U, /*!< Divided by 870               */
	SIU_CLOCK_DIV_BY_871 = 870U, /*!< Divided by 871               */
	SIU_CLOCK_DIV_BY_872 = 871U, /*!< Divided by 872               */
	SIU_CLOCK_DIV_BY_873 = 872U, /*!< Divided by 873               */
	SIU_CLOCK_DIV_BY_874 = 873U, /*!< Divided by 874               */
	SIU_CLOCK_DIV_BY_875 = 874U, /*!< Divided by 875               */
	SIU_CLOCK_DIV_BY_876 = 875U, /*!< Divided by 876               */
	SIU_CLOCK_DIV_BY_877 = 876U, /*!< Divided by 877               */
	SIU_CLOCK_DIV_BY_878 = 877U, /*!< Divided by 878               */
	SIU_CLOCK_DIV_BY_879 = 878U, /*!< Divided by 879               */
	SIU_CLOCK_DIV_BY_880 = 879U, /*!< Divided by 880               */
	SIU_CLOCK_DIV_BY_881 = 880U, /*!< Divided by 881               */
	SIU_CLOCK_DIV_BY_882 = 881U, /*!< Divided by 882               */
	SIU_CLOCK_DIV_BY_883 = 882U, /*!< Divided by 883               */
	SIU_CLOCK_DIV_BY_884 = 883U, /*!< Divided by 884               */
	SIU_CLOCK_DIV_BY_885 = 884U, /*!< Divided by 885               */
	SIU_CLOCK_DIV_BY_886 = 885U, /*!< Divided by 886               */
	SIU_CLOCK_DIV_BY_887 = 886U, /*!< Divided by 887               */
	SIU_CLOCK_DIV_BY_888 = 887U, /*!< Divided by 888               */
	SIU_CLOCK_DIV_BY_889 = 888U, /*!< Divided by 889               */
	SIU_CLOCK_DIV_BY_890 = 889U, /*!< Divided by 890               */
	SIU_CLOCK_DIV_BY_891 = 890U, /*!< Divided by 891               */
	SIU_CLOCK_DIV_BY_892 = 891U, /*!< Divided by 892               */
	SIU_CLOCK_DIV_BY_893 = 892U, /*!< Divided by 893               */
	SIU_CLOCK_DIV_BY_894 = 893U, /*!< Divided by 894               */
	SIU_CLOCK_DIV_BY_895 = 894U, /*!< Divided by 895               */
	SIU_CLOCK_DIV_BY_896 = 895U, /*!< Divided by 896               */
	SIU_CLOCK_DIV_BY_897 = 896U, /*!< Divided by 897               */
	SIU_CLOCK_DIV_BY_898 = 897U, /*!< Divided by 898               */
	SIU_CLOCK_DIV_BY_899 = 898U, /*!< Divided by 899               */
	SIU_CLOCK_DIV_BY_900 = 899U, /*!< Divided by 900               */
	SIU_CLOCK_DIV_BY_901 = 900U, /*!< Divided by 901               */
	SIU_CLOCK_DIV_BY_902 = 901U, /*!< Divided by 902               */
	SIU_CLOCK_DIV_BY_903 = 902U, /*!< Divided by 903               */
	SIU_CLOCK_DIV_BY_904 = 903U, /*!< Divided by 904               */
	SIU_CLOCK_DIV_BY_905 = 904U, /*!< Divided by 905               */
	SIU_CLOCK_DIV_BY_906 = 905U, /*!< Divided by 906               */
	SIU_CLOCK_DIV_BY_907 = 906U, /*!< Divided by 907               */
	SIU_CLOCK_DIV_BY_908 = 907U, /*!< Divided by 908               */
	SIU_CLOCK_DIV_BY_909 = 908U, /*!< Divided by 909               */
	SIU_CLOCK_DIV_BY_910 = 909U, /*!< Divided by 910               */
	SIU_CLOCK_DIV_BY_911 = 910U, /*!< Divided by 911               */
	SIU_CLOCK_DIV_BY_912 = 911U, /*!< Divided by 912               */
	SIU_CLOCK_DIV_BY_913 = 912U, /*!< Divided by 913               */
	SIU_CLOCK_DIV_BY_914 = 913U, /*!< Divided by 914               */
	SIU_CLOCK_DIV_BY_915 = 914U, /*!< Divided by 915               */
	SIU_CLOCK_DIV_BY_916 = 915U, /*!< Divided by 916               */
	SIU_CLOCK_DIV_BY_917 = 916U, /*!< Divided by 917               */
	SIU_CLOCK_DIV_BY_918 = 917U, /*!< Divided by 918               */
	SIU_CLOCK_DIV_BY_919 = 918U, /*!< Divided by 919               */
	SIU_CLOCK_DIV_BY_920 = 919U, /*!< Divided by 920               */
	SIU_CLOCK_DIV_BY_921 = 920U, /*!< Divided by 921               */
	SIU_CLOCK_DIV_BY_922 = 921U, /*!< Divided by 922               */
	SIU_CLOCK_DIV_BY_923 = 922U, /*!< Divided by 923               */
	SIU_CLOCK_DIV_BY_924 = 923U, /*!< Divided by 924               */
	SIU_CLOCK_DIV_BY_925 = 924U, /*!< Divided by 925               */
	SIU_CLOCK_DIV_BY_926 = 925U, /*!< Divided by 926               */
	SIU_CLOCK_DIV_BY_927 = 926U, /*!< Divided by 927               */
	SIU_CLOCK_DIV_BY_928 = 927U, /*!< Divided by 928               */
	SIU_CLOCK_DIV_BY_929 = 928U, /*!< Divided by 929               */
	SIU_CLOCK_DIV_BY_930 = 929U, /*!< Divided by 930               */
	SIU_CLOCK_DIV_BY_931 = 930U, /*!< Divided by 931               */
	SIU_CLOCK_DIV_BY_932 = 931U, /*!< Divided by 932               */
	SIU_CLOCK_DIV_BY_933 = 932U, /*!< Divided by 933               */
	SIU_CLOCK_DIV_BY_934 = 933U, /*!< Divided by 934               */
	SIU_CLOCK_DIV_BY_935 = 934U, /*!< Divided by 935               */
	SIU_CLOCK_DIV_BY_936 = 935U, /*!< Divided by 936               */
	SIU_CLOCK_DIV_BY_937 = 936U, /*!< Divided by 937               */
	SIU_CLOCK_DIV_BY_938 = 937U, /*!< Divided by 938               */
	SIU_CLOCK_DIV_BY_939 = 938U, /*!< Divided by 939               */
	SIU_CLOCK_DIV_BY_940 = 939U, /*!< Divided by 940               */
	SIU_CLOCK_DIV_BY_941 = 940U, /*!< Divided by 941               */
	SIU_CLOCK_DIV_BY_942 = 941U, /*!< Divided by 942               */
	SIU_CLOCK_DIV_BY_943 = 942U, /*!< Divided by 943               */
	SIU_CLOCK_DIV_BY_944 = 943U, /*!< Divided by 944               */
	SIU_CLOCK_DIV_BY_945 = 944U, /*!< Divided by 945               */
	SIU_CLOCK_DIV_BY_946 = 945U, /*!< Divided by 946               */
	SIU_CLOCK_DIV_BY_947 = 946U, /*!< Divided by 947               */
	SIU_CLOCK_DIV_BY_948 = 947U, /*!< Divided by 948               */
	SIU_CLOCK_DIV_BY_949 = 948U, /*!< Divided by 949               */
	SIU_CLOCK_DIV_BY_950 = 949U, /*!< Divided by 950               */
	SIU_CLOCK_DIV_BY_951 = 950U, /*!< Divided by 951               */
	SIU_CLOCK_DIV_BY_952 = 951U, /*!< Divided by 952               */
	SIU_CLOCK_DIV_BY_953 = 952U, /*!< Divided by 953               */
	SIU_CLOCK_DIV_BY_954 = 953U, /*!< Divided by 954               */
	SIU_CLOCK_DIV_BY_955 = 954U, /*!< Divided by 955               */
	SIU_CLOCK_DIV_BY_956 = 955U, /*!< Divided by 956               */
	SIU_CLOCK_DIV_BY_957 = 956U, /*!< Divided by 957               */
	SIU_CLOCK_DIV_BY_958 = 957U, /*!< Divided by 958               */
	SIU_CLOCK_DIV_BY_959 = 958U, /*!< Divided by 959               */
	SIU_CLOCK_DIV_BY_960 = 959U, /*!< Divided by 960               */
	SIU_CLOCK_DIV_BY_961 = 960U, /*!< Divided by 961               */
	SIU_CLOCK_DIV_BY_962 = 961U, /*!< Divided by 962               */
	SIU_CLOCK_DIV_BY_963 = 962U, /*!< Divided by 963               */
	SIU_CLOCK_DIV_BY_964 = 963U, /*!< Divided by 964               */
	SIU_CLOCK_DIV_BY_965 = 964U, /*!< Divided by 965               */
	SIU_CLOCK_DIV_BY_966 = 965U, /*!< Divided by 966               */
	SIU_CLOCK_DIV_BY_967 = 966U, /*!< Divided by 967               */
	SIU_CLOCK_DIV_BY_968 = 967U, /*!< Divided by 968               */
	SIU_CLOCK_DIV_BY_969 = 968U, /*!< Divided by 969               */
	SIU_CLOCK_DIV_BY_970 = 969U, /*!< Divided by 970               */
	SIU_CLOCK_DIV_BY_971 = 970U, /*!< Divided by 971               */
	SIU_CLOCK_DIV_BY_972 = 971U, /*!< Divided by 972               */
	SIU_CLOCK_DIV_BY_973 = 972U, /*!< Divided by 973               */
	SIU_CLOCK_DIV_BY_974 = 973U, /*!< Divided by 974               */
	SIU_CLOCK_DIV_BY_975 = 974U, /*!< Divided by 975               */
	SIU_CLOCK_DIV_BY_976 = 975U, /*!< Divided by 976               */
	SIU_CLOCK_DIV_BY_977 = 976U, /*!< Divided by 977               */
	SIU_CLOCK_DIV_BY_978 = 977U, /*!< Divided by 978               */
	SIU_CLOCK_DIV_BY_979 = 978U, /*!< Divided by 979               */
	SIU_CLOCK_DIV_BY_980 = 979U, /*!< Divided by 980               */
	SIU_CLOCK_DIV_BY_981 = 980U, /*!< Divided by 981               */
	SIU_CLOCK_DIV_BY_982 = 981U, /*!< Divided by 982               */
	SIU_CLOCK_DIV_BY_983 = 982U, /*!< Divided by 983               */
	SIU_CLOCK_DIV_BY_984 = 983U, /*!< Divided by 984               */
	SIU_CLOCK_DIV_BY_985 = 984U, /*!< Divided by 985               */
	SIU_CLOCK_DIV_BY_986 = 985U, /*!< Divided by 986               */
	SIU_CLOCK_DIV_BY_987 = 986U, /*!< Divided by 987               */
	SIU_CLOCK_DIV_BY_988 = 987U, /*!< Divided by 988               */
	SIU_CLOCK_DIV_BY_989 = 988U, /*!< Divided by 989               */
	SIU_CLOCK_DIV_BY_990 = 989U, /*!< Divided by 990               */
	SIU_CLOCK_DIV_BY_991 = 990U, /*!< Divided by 991               */
	SIU_CLOCK_DIV_BY_992 = 991U, /*!< Divided by 992               */
	SIU_CLOCK_DIV_BY_993 = 992U, /*!< Divided by 993               */
	SIU_CLOCK_DIV_BY_994 = 993U, /*!< Divided by 994               */
	SIU_CLOCK_DIV_BY_995 = 994U, /*!< Divided by 995               */
	SIU_CLOCK_DIV_BY_996 = 995U, /*!< Divided by 996               */
	SIU_CLOCK_DIV_BY_997 = 996U, /*!< Divided by 997               */
	SIU_CLOCK_DIV_BY_998 = 997U, /*!< Divided by 998               */
	SIU_CLOCK_DIV_BY_999 = 998U, /*!< Divided by 999               */
	SIU_CLOCK_DIV_BY_1000 = 999U, /*!< Divided by 1000              */
	SIU_CLOCK_DIV_BY_1001 = 1000U, /*!< Divided by 1001              */
	SIU_CLOCK_DIV_BY_1002 = 1001U, /*!< Divided by 1002              */
	SIU_CLOCK_DIV_BY_1003 = 1002U, /*!< Divided by 1003              */
	SIU_CLOCK_DIV_BY_1004 = 1003U, /*!< Divided by 1004              */
	SIU_CLOCK_DIV_BY_1005 = 1004U, /*!< Divided by 1005              */
	SIU_CLOCK_DIV_BY_1006 = 1005U, /*!< Divided by 1006              */
	SIU_CLOCK_DIV_BY_1007 = 1006U, /*!< Divided by 1007              */
	SIU_CLOCK_DIV_BY_1008 = 1007U, /*!< Divided by 1008              */
	SIU_CLOCK_DIV_BY_1009 = 1008U, /*!< Divided by 1009              */
	SIU_CLOCK_DIV_BY_1010 = 1009U, /*!< Divided by 1010              */
	SIU_CLOCK_DIV_BY_1011 = 1010U, /*!< Divided by 1011              */
	SIU_CLOCK_DIV_BY_1012 = 1011U, /*!< Divided by 1012              */
	SIU_CLOCK_DIV_BY_1013 = 1012U, /*!< Divided by 1013              */
	SIU_CLOCK_DIV_BY_1014 = 1013U, /*!< Divided by 1014              */
	SIU_CLOCK_DIV_BY_1015 = 1014U, /*!< Divided by 1015              */
	SIU_CLOCK_DIV_BY_1016 = 1015U, /*!< Divided by 1016              */
	SIU_CLOCK_DIV_BY_1017 = 1016U, /*!< Divided by 1017              */
	SIU_CLOCK_DIV_BY_1018 = 1017U, /*!< Divided by 1018              */
	SIU_CLOCK_DIV_BY_1019 = 1018U, /*!< Divided by 1019              */
	SIU_CLOCK_DIV_BY_1020 = 1019U, /*!< Divided by 1020              */
	SIU_CLOCK_DIV_BY_1021 = 1020U, /*!< Divided by 1021              */
	SIU_CLOCK_DIV_BY_1022 = 1021U, /*!< Divided by 1022              */
	SIU_CLOCK_DIV_BY_1023 = 1022U, /*!< Divided by 1023              */
	SIU_CLOCK_DIV_BY_1024 = 1023U, /*!< Divided by 1024              */
	SIU_CLOCK_DISABLE = 1024U, /*!< Disable system clock divider */
} siu_clk_div_t;

/*!
 * @brief SIU auxiliary clock selector.
 * Implements siu_clk_sel_t_Class
 */
typedef uint8_t siu_clk_sel_t;

#define SIU_PER_CLK_SEL_CORE_CLK         0U
#define SIU_PER_CLK_SEL_PLL0_PHI0        1U

#define SIU_MCAN_CLK_SEL_XOSC_CLK         0U
#define SIU_MCAN_CLK_SEL_PER_CLK          1U

#define SIU_LFASTx_SEL_XOSC              0U
#define SIU_LFASTx_SEL_SIPI_REF_CLK      1U
#define SIU_LFASTx_SEL_PER_CLK           2U

#define PLL_REFERENCE_XOSC               0U
#define PLL_REFERENCE_IRCOSC             1U
#define PLL_REFERENCE_PLL0_PHI1          1U

#define SIU_SYSTEM_CLOCK_SRC_IRCOSC      0U
#define SIU_SYSTEM_CLOCK_SRC_XOSC        1U
#define SIU_SYSTEM_CLOCK_SRC_PLL0_PHI0   3U
#define SIU_SYSTEM_CLOCK_SRC_PLL1_PHI0   2U

#define SIU_ENG_CLOCKOUT_PLAT_CLK        0U
#define SIU_ENG_CLOCKOUT_XOSC_CLK        1U

/*!
 * @brief SIU configuration structure.
 * Implements siu_config_t_Class
 */
typedef struct {

	bool SIU_CSE;
	bool SIU_CRC;
	bool SIU_DSPIA;
	bool SIU_DSPIB;
	bool SIU_DSPIC;
	bool SIU_DSPID;
	bool SIU_ENET;
	bool SIU_FLEXCANA;
	bool SIU_FLEXCANB;
	bool SIU_FLEXCANC;
	bool SIU_FLEXCAND;
	bool SIU_EMIOS0;
	bool SIU_EMIOS1;
	bool SIU_PSI;
	bool SIU_ESCIA;
	bool SIU_ESCIB;
	bool SIU_ESCIC;
	bool SIU_ESCID;
	bool SIU_ESCIE;
	bool SIU_ESCIF;
	bool SIU_PSI5A;
	bool SIU_PSI5B;
	bool SIU_DECFIL;
	bool SIU_ETPUC;
	bool SIU_NPC;
	bool SIU_PIT;
	bool SIU_STCU;
	bool SIU_SRX0;
	bool SIU_SRX1;
	bool SIU_EQADCA;
	bool SIU_EQADCB;
	bool SIU_SDD;
	bool SIU_SIPI;
	bool SIU_SDA;
	bool SIU_SDB;
	bool SIU_SDC;
	bool SIU_MCANB;
	bool SIU_DSPI;
	bool SIU_EBI;
	bool SIU_ETPUA;
	bool SIU_DSPIE;
	bool SIU_MCANA;
	siu_clk_sel_t pll0Reference;
	siu_clk_sel_t pll1Reference;
	siu_clk_sel_t scs;
	siu_clk_sel_t perClkSel;
	siu_clk_sel_t lfastSel;
	siu_clk_sel_t mcanSel;
	siu_clk_sel_t engClkoutSel;
	siu_clk_div_t coreClk;
	siu_clk_div_t pbridgeClk;
	siu_clk_div_t perClk;
	siu_clk_div_t etpuClk;
	siu_clk_div_t adcsdClk;
	siu_clk_div_t psi5Rx;
	siu_clk_div_t psi5Rx1M;
	siu_clk_div_t lfastClk;
	siu_clk_div_t clkout;
	siu_clk_div_t engClkout;

} siu_config_t;

/*!
 * @brief IRC clock trimming value
 * Implements cs_irc_clk_trim_t_Class
 */
typedef enum {

	IRC_CLOCK_TRIM_BY_MINUS_15 = 31U, /*!< Trim by minus 15 */
	IRC_CLOCK_TRIM_BY_MINUS_14 = 30U, /*!< Trim by minus 14 */
	IRC_CLOCK_TRIM_BY_MINUS_13 = 29U, /*!< Trim by minus 13 */
	IRC_CLOCK_TRIM_BY_MINUS_12 = 28U, /*!< Trim by minus 12 */
	IRC_CLOCK_TRIM_BY_MINUS_11 = 27U, /*!< Trim by minus 11 */
	IRC_CLOCK_TRIM_BY_MINUS_10 = 26U, /*!< Trim by minus 10 */
	IRC_CLOCK_TRIM_BY_MINUS_9 = 25U, /*!< Trim by minus 9  */
	IRC_CLOCK_TRIM_BY_MINUS_8 = 24U, /*!< Trim by minus 8  */
	IRC_CLOCK_TRIM_BY_MINUS_7 = 23U, /*!< Trim by minus 7  */
	IRC_CLOCK_TRIM_BY_MINUS_6 = 22U, /*!< Trim by minus 6  */
	IRC_CLOCK_TRIM_BY_MINUS_5 = 21U, /*!< Trim by minus 5  */
	IRC_CLOCK_TRIM_BY_MINUS_4 = 20U, /*!< Trim by minus 4  */
	IRC_CLOCK_TRIM_BY_MINUS_3 = 19U, /*!< Trim by minus 3  */
	IRC_CLOCK_TRIM_BY_MINUS_2 = 18U, /*!< Trim by minus 2  */
	IRC_CLOCK_TRIM_BY_MINUS_1 = 17U, /*!< Trim by minus 1  */
	IRC_CLOCK_NO_TRIMMING = 16U, /*!< No trimming      */
	IRC_CLOCK_TRIM_BY_PLUS_1 = 1U, /*!< Trim by plus 1   */
	IRC_CLOCK_TRIM_BY_PLUS_2 = 2U, /*!< Trim by plus 2   */
	IRC_CLOCK_TRIM_BY_PLUS_3 = 3U, /*!< Trim by plus 3   */
	IRC_CLOCK_TRIM_BY_PLUS_4 = 4U, /*!< Trim by plus 4   */
	IRC_CLOCK_TRIM_BY_PLUS_5 = 5U, /*!< Trim by plus 5   */
	IRC_CLOCK_TRIM_BY_PLUS_6 = 6U, /*!< Trim by plus 6   */
	IRC_CLOCK_TRIM_BY_PLUS_7 = 7U, /*!< Trim by plus 7   */
	IRC_CLOCK_TRIM_BY_PLUS_8 = 8U, /*!< Trim by plus 8   */
	IRC_CLOCK_TRIM_BY_PLUS_9 = 9U, /*!< Trim by plus 9   */
	IRC_CLOCK_TRIM_BY_PLUS_10 = 10U, /*!< Trim by plus 10  */
	IRC_CLOCK_TRIM_BY_PLUS_11 = 11U, /*!< Trim by plus 11  */
	IRC_CLOCK_TRIM_BY_PLUS_12 = 12U, /*!< Trim by plus 12  */
	IRC_CLOCK_TRIM_BY_PLUS_13 = 13U, /*!< Trim by plus 13  */
	IRC_CLOCK_TRIM_BY_PLUS_14 = 14U, /*!< Trim by plus 14  */
	IRC_CLOCK_TRIM_BY_PLUS_15 = 15U, /*!< Trim by plus 15  */

} cs_irc_clk_trim_t;

/*!
 * @brief XOSC bypass type.
 * Implements cs_xosc_bypass_t_Class
 */
typedef enum {
	XOSC_USE_CRYSTAL = 0U, XOSC_USE_EXTAL = 1U,

} cs_xosc_bypass_t;

/*!
 * @brief XOSC mode.
 * Implements cs_xosc_mode_t_Class
 */
typedef enum {
	XOSC_FULL_SWING_PIERCE_MODE = 0U, XOSC_LOOP_CONTROLLED_PIERCE_MODE = 1U,

} cs_xosc_mode_t;

/*!
 * @brief Clock Source XOSC configuration structure.
 * Implements cs_xosc_config_t_Class
 */
typedef struct {
	uint32_t freq; /*!< External oscillator frequency. */
	uint16_t startupDelay; /*!< Startup stabilization time.    */
	cs_xosc_bypass_t bypassOption; /*!< XOSC option                    */
} cs_xosc_config_t;

/*!
 * @brief eTPU clock divider
 * Implements siu_etpu_t_Class
 */
typedef enum {
	SIU_ETPU_DIV_BY_2 = 0U, /*!< Divide by 2 */
	SIU_ETPU_DIV_BY_1 = 1U, /*!< Divide by 1 */
} siu_etpu_t;

/*!
 * @brief SIU non-FM clock peripheral divider
 * Implements siu_perdiv_t_Class
 */
typedef enum {
	SIU_PERDIV_DIV_BY_2 = 0U, /*!< Divide by 2 */
	SIU_PERDIV_DIV_BY_4 = 1U, /*!< Divide by 4 */
	SIU_PERDIV_DIV_BY_8 = 2U, /*!< Divide by 8 */
} siu_perdiv_t;

/*!
 * @brief SIU pbridge clock source
 * Implements siu_fmperdiv_t_Class
 */
typedef enum {
	SIU_PBRIDGE_DIV_BY_2 = 0U, /*!< Divide by 2 */
	SIU_PBRIDGE_DIV_BY_4 = 1U, /*!< Divide by 4 */
	SIU_PBRIDGE_DIV_BY_8 = 2U, /*!< Divide by 8 */
} siu_fmperdiv_t;

/*!
 * @brief SIU LFAST clock select
 * Implements siu_lfastsel_t_Class
 */
typedef enum {
	SIU_LFASTx_XOSC = 0U, /*!< Crystal oscillator (XOSC) */
	SIU_LFASTx_SIPI_REFCLK = 1U, /*!< SIPI_REFCLK               */
	SIU_LFASTx_PLL = 2U, /*!< PLL                       */
} siu_lfastsel_t;

/*!
 * @brief SIU peripheral clock select
 * Implements siu_perclksel_t_Class
 */
typedef enum {
	SIU_PERCLKSEL_SYSCLKDIV = 0U, /*!< Output of the SYSCLKDIV divider */
	SIU_PERCLKSEL_PLL0 = 1U, /*!< Output of PLL0                  */
} siu_perclksel_t;

/*!
 * @brief SIU system clock source select
 * Implements siu_sysclksel_t_Class
 */
typedef enum {
	SIU_SYSCLKSEL_IRCOSC = 0U, /*!< IRCOSC */
	SIU_SYSCLKSEL_XOSC = 1U, /*!< XOSC   */
	SIU_SYSCLKSEL_PLL1 = 2U, /*!< PLL1   */
	SIU_SYSCLKSEL_PLL0 = 3U, /*!< PLL0   */
} siu_sysclksel_t;

/*!
 * @brief SIU system clock source divider
 * Implements siu_sysclkdiv_t_Class
 */
typedef enum {
	SIU_SYSCLK_DIV_1 = 4U, /*!< Divide by 1  */
	SIU_SYSCLK_DIV_2 = 0U, /*!< Divide by 2  */
	SIU_SYSCLK_DIV_4 = 1U, /*!< Divide by 4  */
	SIU_SYSCLK_DIV_8 = 2U, /*!< Divide by 8  */
	SIU_SYSCLK_DIV_16 = 3U, /*!< Divide by 16 */
} siu_sysclkdiv_t;

/*!
 * @brief plldig clock divider
 * Implements cs_plldig_clk_prediv_t_Class
 */
typedef enum {

	PLLDIG_CLOCK_PREDIV_DISABLE = 0U, /*!< Disable predivider        */
	PLLDIG_CLOCK_PREDIV_BY_1 = 1U, /*!< Input clock divided by 1  */
	PLLDIG_CLOCK_PREDIV_BY_2 = 2U, /*!< Input clock divided by 2  */
	PLLDIG_CLOCK_PREDIV_BY_3 = 3U, /*!< Input clock divided by 3  */
	PLLDIG_CLOCK_PREDIV_BY_4 = 4U, /*!< Input clock divided by 4  */
	PLLDIG_CLOCK_PREDIV_BY_5 = 5U, /*!< Input clock divided by 5  */
	PLLDIG_CLOCK_PREDIV_BY_6 = 6U, /*!< Input clock divided by 6  */
	PLLDIG_CLOCK_PREDIV_BY_7 = 7U, /*!< Input clock divided by 7  */

} cs_plldig_clk_prediv_t;

/*!
 * @brief plldig clock configuration
 * Implements cs_plldig_pll_status_t_Class
 */
typedef enum {

	PLLDIG_PLL_OFF = 0U, /*!< Disable PLL        */
	PLLDIG_PLL_ON = 3U /*!< Enable PLL         */
} cs_plldig_pll_status_t;

/*!
 * @brief plldig clockout divider
 * Implements cs_plldig_clkout_div_t_Class
 */
typedef enum {

	PLLDIG_PHI_DIV_BY_1 = 0U, /*!< Divided by 1  */
	PLLDIG_PHI_DIV_BY_2 = 1U, /*!< Divided by 2  */
	PLLDIG_PHI_DIV_BY_3 = 2U, /*!< Divided by 3  */
	PLLDIG_PHI_DIV_BY_4 = 3U, /*!< Divided by 4  */
	PLLDIG_PHI_DIV_BY_5 = 4U, /*!< Divided by 5  */
	PLLDIG_PHI_DIV_BY_6 = 5U, /*!< Divided by 6  */
	PLLDIG_PHI_DIV_BY_7 = 6U, /*!< Divided by 7  */
	PLLDIG_PHI_DIV_BY_8 = 7U, /*!< Divided by 8  */
	PLLDIG_PHI_DIV_BY_9 = 8U, /*!< Divided by 9  */
	PLLDIG_PHI_DIV_BY_10 = 9U, /*!< Divided by 10 */
	PLLDIG_PHI_DIV_BY_11 = 10U, /*!< Divided by 11 */
	PLLDIG_PHI_DIV_BY_12 = 11U, /*!< Divided by 12 */
	PLLDIG_PHI_DIV_BY_13 = 12U, /*!< Divided by 13 */
	PLLDIG_PHI_DIV_BY_14 = 13U, /*!< Divided by 14 */
	PLLDIG_PHI_DIV_BY_15 = 14U, /*!< Divided by 15 */
	PLLDIG_PHI_DIV_BY_16 = 15U, /*!< Divided by 16 */
	PLLDIG_PHI_DIV_BY_17 = 16U, /*!< Divided by 17 */
	PLLDIG_PHI_DIV_BY_18 = 17U, /*!< Divided by 18 */
	PLLDIG_PHI_DIV_BY_19 = 18U, /*!< Divided by 19 */
	PLLDIG_PHI_DIV_BY_20 = 19U, /*!< Divided by 20 */
	PLLDIG_PHI_DIV_BY_21 = 20U, /*!< Divided by 21 */
	PLLDIG_PHI_DIV_BY_22 = 21U, /*!< Divided by 22 */
	PLLDIG_PHI_DIV_BY_23 = 22U, /*!< Divided by 23 */
	PLLDIG_PHI_DIV_BY_24 = 23U, /*!< Divided by 24 */
	PLLDIG_PHI_DIV_BY_25 = 24U, /*!< Divided by 25 */
	PLLDIG_PHI_DIV_BY_26 = 25U, /*!< Divided by 26 */
	PLLDIG_PHI_DIV_BY_27 = 26U, /*!< Divided by 27 */
	PLLDIG_PHI_DIV_BY_28 = 27U, /*!< Divided by 28 */
	PLLDIG_PHI_DIV_BY_29 = 28U, /*!< Divided by 29 */
	PLLDIG_PHI_DIV_BY_30 = 29U, /*!< Divided by 30 */
	PLLDIG_PHI_DIV_BY_31 = 30U, /*!< Divided by 31 */
	PLLDIG_PHI_DIV_BY_32 = 31U, /*!< Divided by 32 */
	PLLDIG_PHI_DIV_BY_33 = 32U, /*!< Divided by 33 */
	PLLDIG_PHI_DIV_BY_34 = 33U, /*!< Divided by 34 */
	PLLDIG_PHI_DIV_BY_35 = 34U, /*!< Divided by 35 */
	PLLDIG_PHI_DIV_BY_36 = 35U, /*!< Divided by 36 */
	PLLDIG_PHI_DIV_BY_37 = 36U, /*!< Divided by 37 */
	PLLDIG_PHI_DIV_BY_38 = 37U, /*!< Divided by 38 */
	PLLDIG_PHI_DIV_BY_39 = 38U, /*!< Divided by 39 */
	PLLDIG_PHI_DIV_BY_40 = 39U, /*!< Divided by 40 */
	PLLDIG_PHI_DIV_BY_41 = 40U, /*!< Divided by 41 */
	PLLDIG_PHI_DIV_BY_42 = 41U, /*!< Divided by 42 */
	PLLDIG_PHI_DIV_BY_43 = 42U, /*!< Divided by 43 */
	PLLDIG_PHI_DIV_BY_44 = 43U, /*!< Divided by 44 */
	PLLDIG_PHI_DIV_BY_45 = 44U, /*!< Divided by 45 */
	PLLDIG_PHI_DIV_BY_46 = 45U, /*!< Divided by 46 */
	PLLDIG_PHI_DIV_BY_47 = 46U, /*!< Divided by 47 */
	PLLDIG_PHI_DIV_BY_48 = 47U, /*!< Divided by 48 */
	PLLDIG_PHI_DIV_BY_49 = 48U, /*!< Divided by 49 */
	PLLDIG_PHI_DIV_BY_50 = 49U, /*!< Divided by 50 */
	PLLDIG_PHI_DIV_BY_51 = 50U, /*!< Divided by 51 */
	PLLDIG_PHI_DIV_BY_52 = 51U, /*!< Divided by 52 */
	PLLDIG_PHI_DIV_BY_53 = 52U, /*!< Divided by 53 */
	PLLDIG_PHI_DIV_BY_54 = 53U, /*!< Divided by 54 */
	PLLDIG_PHI_DIV_BY_55 = 54U, /*!< Divided by 55 */
	PLLDIG_PHI_DIV_BY_56 = 55U, /*!< Divided by 56 */
	PLLDIG_PHI_DIV_BY_57 = 56U, /*!< Divided by 57 */
	PLLDIG_PHI_DIV_BY_58 = 57U, /*!< Divided by 58 */
	PLLDIG_PHI_DIV_BY_59 = 58U, /*!< Divided by 59 */
	PLLDIG_PHI_DIV_BY_60 = 59U, /*!< Divided by 60 */
	PLLDIG_PHI_DIV_BY_61 = 60U, /*!< Divided by 61 */
	PLLDIG_PHI_DIV_BY_62 = 61U, /*!< Divided by 62 */
	PLLDIG_PHI_DIV_BY_63 = 62U, /*!< Divided by 63 */
	PLLDIG_PHI_DIV_BY_64 = 63U, /*!< Divided by 64 */
} cs_plldig_clkout_div_t;

/*!
 * @brief Modulation mode.
 * Implements cs_plldig_mod_type_t_Class
 */
typedef enum {
	CENTRE_SPREAD_MODULATION = 0U, DOWN_SPREAD_MODULATION = 1U,

} cs_plldig_mod_type_t;

/*!
 * @brief CMU IRCOSC low frequency divider.
 * Implements cmu_ircosc_divider_t_Class
 */
typedef enum {
	CMU_LO_FREQ_1 = 0x0,
	CMU_LO_FREQ_2 = 0x1,
	CMU_LO_FREQ_4 = 0x2,
	CMU_LO_FREQ_8 = 0x4,
} cmu_ircosc_divider_t;

/*!
 * @brief Clock Source PLLDIG configuration structure.
 * Implements cs_plldig_config_t_Class
 */
typedef struct {
	bool enable; /*!< PLL enabled/disabled                   */
	cs_plldig_clk_prediv_t predivider; /*!< Input clock predivider. (PREDIV)       */
	bool fracDivider; /*!< Enable fractional divider              */
	uint16_t fracDividerValue; /*!< Fractional divider value               */
	uint8_t mulFactorDiv; /*!< Multiplication factor divider          */
	cs_plldig_clkout_div_t phi0Divider; /*!< Divider for clockout0 - PHI (RFDPHI)   */
	cs_plldig_clkout_div_t phi1Divider; /*!< Divider for clockout1 - PHI1 (RFDPHI1) */
	bool modulation; /*!< Enable/disable modulation              */
	cs_plldig_mod_type_t modulationType; /*!< Modulation type                        */
	uint16_t modulationPeriod; /*!< Stepsize - modulation period           */
	uint16_t incrementStep; /*!< Stepno  - step no                      */
	bool rectangularDitherControl; /*!< Dither control enable */
	uint8_t rectangularDitherControlValue; /*!< Dither control value */
	bool triangularDitherControl; /*!< Dither control enable */
	uint8_t triangularDitherControlValue; /*!< Dither control value */
} cs_plldig_config_t;

/*!
 * @brief Clock Sources configuration structure.
 * Implements cs_config_t_Class
 */
typedef struct {
	cs_xosc_config_t xosc0Config;
	cs_plldig_config_t pll0Config;
	cs_plldig_config_t pll1Config;
	uint32_t sipiRefClkFreq0; /*!< Frequency of the SIPI_REF external clock */
} cs_config_t;

/*!
 * @brief Clock Monitoring Unit configuration structure for CMU_1 - CMU_8 and CMU_PLL0.
 * Implements monitoring_units_config_t_Class
 */
typedef struct {
	bool enable;
	uint32_t lo_freq;
	uint32_t hi_freq;
} monitoring_units_config_t;

/*!
 * @brief Clock Monitoring Units configuration structure.
 * Implements cmu_config_t_Class
 */
typedef struct {
	cmu_ircosc_divider_t cmu_rcdiv;
	monitoring_units_config_t cmu[CMU_MAX_NO];
} cmu_config_t;

/*!
 * @brief Clock configuration structure.
 * Implements clock_manager_user_config_t_Class
 */
typedef struct {
	siu_config_t siuConfig; /*!< SIU configuration.                           */
	cs_config_t clockSourcesConfig; /*!< Clock Sources configuration.                 */
	cmu_config_t cmuConfig; /*!< Clock Monitoring Unit configuration.		 */
} clock_manager_user_config_t;

typedef clock_manager_user_config_t clock_user_config_t;

/*!
 * @brief The clock notification type.
 * Implements clock_manager_notify_t_Class
 */
typedef enum {
	CLOCK_MANAGER_NOTIFY_RECOVER = 0x00U, /*!< Notify IP to recover to previous work state.      */
	CLOCK_MANAGER_NOTIFY_BEFORE = 0x01U, /*!< Notify IP that system will change clock setting.  */
	CLOCK_MANAGER_NOTIFY_AFTER = 0x02U, /*!< Notify IP that have changed to new clock setting. */
} clock_manager_notify_t;

/*!
 * @brief The callback type, indicates what kinds of notification this callback handles.
 * Implements clock_manager_callback_type_t_Class
 */
typedef enum {
	CLOCK_MANAGER_CALLBACK_BEFORE = 0x01U, /*!< Callback handles BEFORE notification.          */
	CLOCK_MANAGER_CALLBACK_AFTER = 0x02U, /*!< Callback handles AFTER notification.           */
	CLOCK_MANAGER_CALLBACK_BEFORE_AFTER = 0x03U /*!< Callback handles BEFORE and AFTER notification */
} clock_manager_callback_type_t;

/*!
 * @brief Clock transition policy.
 * Implements clock_manager_policy_t_Class
 */
typedef enum {
	CLOCK_MANAGER_POLICY_AGREEMENT, /*!< Clock transfers gracefully. */
	CLOCK_MANAGER_POLICY_FORCIBLE /*!< Clock transfers forcefully. */
} clock_manager_policy_t;

/*!
 * @brief Clock notification structure passed to clock callback function.
 * Implements clock_notify_struct_t_Class
 */
typedef struct {
	uint8_t targetClockConfigIndex; /*!< Target clock configuration index. */
	clock_manager_policy_t policy; /*!< Clock transition policy.          */
	clock_manager_notify_t notifyType; /*!< Clock notification type.          */
} clock_notify_struct_t;

/*!
 * @brief Type of clock callback functions.
 */
typedef status_t (*clock_manager_callback_t)(clock_notify_struct_t *notify,
		void* callbackData);

/*!
 * @brief Structure for callback function and its parameter.
 * Implements clock_manager_callback_user_config_t_Class
 */
typedef struct {
	clock_manager_callback_t callback; /*!< Entry of callback function.     */
	clock_manager_callback_type_t callbackType; /*!< Callback type.                  */
	void* callbackData; /*!< Parameter of callback function. */
} clock_manager_callback_user_config_t;

/*!
 * @brief Clock manager state structure.
 * Implements clock_manager_state_t_Class
 */
typedef struct {
	clock_manager_user_config_t const **configTable;/*!< Pointer to clock configure table.*/
	uint8_t clockConfigNum; /*!< Number of clock configurations.  */
	uint8_t curConfigIndex; /*!< Index of current configuration.  */
	clock_manager_callback_user_config_t **callbackConfig; /*!< Pointer to callback table.*/
	uint8_t callbackNum; /*!< Number of clock callbacks.       */
	uint8_t errorCallbackIndex; /*!< Index of callback returns error. */
} clock_manager_state_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

status_t CLOCK_SYS_SetConfiguration(clock_manager_user_config_t const* config);
status_t CLOCK_SYS_GetFreq(clock_names_t clockName, uint32_t *frequency);
status_t CLOCK_SYS_UpdateConfiguration(uint8_t targetConfigIndex,
		clock_manager_policy_t policy);
status_t CLOCK_SYS_Init(clock_manager_user_config_t const **clockConfigsPtr,
		uint8_t configsNumber,
		clock_manager_callback_user_config_t **callbacksPtr,
		uint8_t callbacksNumber);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

/*! @}*/

#endif /* CLOCK_MPC5748G_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/

