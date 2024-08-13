/*
 * Copyright (c) 2016, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the Intel Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef HDC1000_UTILS_H_
#define HDC1000_UTILS_H_

/** Swap the high and low byte of a word(16bit). */
#define BYTE_SWAP(_in_) (((0x00ff & _in_) << 8) | ((0xff00 & _in_) >> 8))
/** Extract the high of a word(16bit). */
#define HIGH_BYTE(_in_) ((_in_ & 0xFF00) >> 8)
/** Extract the low  of a word(16bit). */
#define LOW_BYTE(_in_) ((_in_ & 0x00FF))
/** Check the application supports the underlying SoC. */
#define IS_SUPPORTED_SoC() ((SOC_ID_INVALID != util_get_soc()) ? true : false)

/**
 * The soc_id_t enums are used to identify supported platforms.
 */
typedef enum {
	SOC_ID_INVALID, /**< Enum invalid value. */
	SOC_ID_D2000,   /**< For Quark D2000. */
	SOC_ID_SE       /**< For Quark SE. */
} soc_id_t;

/**
 * Print the float value.
 *
 * Presently the printing of float is not supported in QMSI.
 * Function implement logic for:-
 * 1. splitting the float into decimal and fractional.
 * 2. print decimal separately.
 * 3. fractional prints up to 5 digit precision.
 *
 * @param[in] input Float number to be printed.
 */
void util_print_float(float input);

/**
 * Get SoC ID.
 *
 * Get the current SoC value by checking the macro enabled among
 * Quark D2000 and Quark SE.
 *
 * @return values for soc_id_t.
 */
soc_id_t util_get_soc(void);

#endif /* HDC1000_UTILS_H_ */
