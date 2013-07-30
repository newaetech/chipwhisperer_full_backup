/**
 * \file
 *
 * \brief Non Volatile Memory controller driver
 *
 * Copyright (c) 2010 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#include <assembler.h>

#if defined(__GNUC__)
//! Value to write to CCP for access to protected IO registers.
#  define CCP_SPM_gc 0x9D

//! NVM busy flag
#  define NVM_NVMBUSY_bp 7

//! NVM command for loading flash buffer
#  define NVM_CMD_LOAD_FLASH_BUFFER_gc 0x23
#elif defined(__IAR_SYSTEMS_ASM__)
// All values are defined for IAR
#else
# error Unknown assembler
#endif

#ifndef __DOXYGEN__
	PUBLIC_FUNCTION(nvm_read_byte)
#if defined(__GNUC__)
	lds r20, NVM_CMD          ; Store NVM command register
	mov ZL, r22               ; Load byte index into low byte of Z.
	mov ZH, r23               ; Load high byte into Z.
	sts NVM_CMD, r24          ; Load prepared command into NVM Command register.
	lpm r24, Z                ; Perform an LPM to read out byte
	sts NVM_CMD, r20          ; Restore NVM command register
#elif defined(__IAR_SYSTEMS_ASM__)
	lds r20, NVM_CMD          ; Store NVM command register
	mov ZL, r18               ; Load byte index into low byte of Z.
	mov ZH, r19               ; Load high byte into Z.
	sts NVM_CMD, r16          ; Load prepared command into NVM Command register.
	lpm r16, Z                ; Perform an LPM to read out byte
	sts NVM_CMD, r20          ; Restore NVM command register
#endif

	ret

	END_FUNC(nvm_read_byte)

// IAR forgets about include files after each module, so need to include again
#if defined(__IAR_SYSTEMS_ASM__)
# include <ioavr.h>
#endif

	/**
	 * \brief Perform SPM command
	 */
	PUBLIC_FUNCTION_SEGMENT(nvm_common_spm, BOOT)

#if defined(__GNUC__)
	/**
	 * For GCC:
	 * \param address uint32_t r22:r25
	 * \param nvm_cmd uint8_t  r20
	 */
	in r25, RAMPZ         ; Store RAMPZ. Highest address byte is ignored, so using that
	out RAMPZ, r24        ; Load R24 into RAMPZ
	movw ZL, r22          ; Load R22:R23 into Z.
	lds r24, NVM_CMD      ; Store NVM command register (r24 is no longer needed)
	sts NVM_CMD, r20      ; Load prepared command into NVM Command register.
	ldi r23, CCP_SPM_gc   ; Prepare Protect SPM signature (r23 is no longer needed)
	sts CCP, r23          ; Enable SPM operation (this disables interrupts for 4 cycles).
	spm                   ; Self-program.
	sts NVM_CMD, r24      ; Restore NVM command register
	out RAMPZ, r25        ; Restore RAMPZ register.
#elif defined(__IAR_SYSTEMS_ASM__)
	/**
	 * For IAR:
	 * \param address uint32_t r16:r19
	 * \param nvm_cmd uint8_t  r20
	 */
	in r19, RAMPZ         ; Store RAMPZ. Highest address byte is ignored, so using that
	out RAMPZ, r18        ; Load R18 into RAMPZ
	movw ZL, r16          ; Load R16:R17 into Z.
	lds r18, NVM_CMD      ; Store NVM command register (r18 is no longer needed)
	sts NVM_CMD, r20      ; Load prepared command into NVM Command register.
	ldi r19, CCP_SPM_gc   ; Prepare Protect SPM signature (r19 is no longer needed)
	sts CCP, r19          ; Enable SPM operation (this disables interrupts for 4 cycles).
	spm                   ; Self-program.
	sts NVM_CMD, r18      ; Restore NVM command register
	out RAMPZ, r19        ; Restore RAMPZ register.
#endif

	ret

	END_FUNC(nvm_common_spm)

// IAR forgets about include files after each module, so need to include again
#if defined(__IAR_SYSTEMS_ASM__)
# include <ioavr.h>
#endif

	/**
	 * \brief Load byte to page buffer
	 *
	 */
	PUBLIC_FUNCTION_SEGMENT(nvm_flash_load_word_to_buffer, BOOT)

#if defined(__GNUC__)
	/**
	 * For GCC:
	 * \param word_addr uint32_t r22:r25
	 * \param data      uint16_t r20:r21
	 */
wait_nvm:
	lds r18, NVM_STATUS
	sbrc r18, NVM_NVMBUSY_bp
	rjmp wait_nvm

	in r25, RAMPZ         ; Store RAMPZ. Highest address byte is ignored, so using that
	out RAMPZ, r24        ; Load R24 into RAMPZ
	movw ZL, r22          ; Load R22:R23 into Z.

	lds r24, NVM_CMD      ; Store NVM command register (r24 is no longer needed)
	ldi r18, NVM_CMD_LOAD_FLASH_BUFFER_gc
	sts NVM_CMD, r18      ; Load prepared command into NVM Command register.

	movw r0, r20          ; Load R20:R21 into R0:R1
	spm                   ; Self-program.

	clr r1                ; Clear R1 for GCC _zero_reg_ to function properly.
	sts NVM_CMD, r24      ; Restore NVM command register
	out RAMPZ, r25        ; Restore RAMPZ register.
#elif defined(__IAR_SYSTEMS_ASM__)
	/**
	 * For IAR:
	 * \param word_addr uint32_t r16:r19
	 * \param data      uint16_t r20:r21
	 */
wait_nvm:
	lds r19, NVM_STATUS
	sbrc r19, NVM_NVMBUSY_bp
	rjmp wait_nvm

	in r19, RAMPZ         ; Store RAMPZ. Highest byte is ignored, so using that
	out RAMPZ, r18        ; Load R18 into RAMPZ
	movw ZL, r16          ; Load R16:R17 into Z.

	lds r18, NVM_CMD      ; Store NVM command register (r18 is no longer needed)
	ldi r17, NVM_CMD_LOAD_FLASH_BUFFER_gc
	sts NVM_CMD, r17      ; Load prepared command into NVM Command register.

	movw r0, r20          ; Load R20:R21 into R0:R1
	spm                   ; Self-program.

	sts NVM_CMD, r18      ; Restore NVM command register
	out RAMPZ, r19        ; Restore RAMPZ register.
#endif

	ret

	END_FUNC(nvm_flash_load_word_to_buffer)

	END_FILE()
#endif // __DOXYGEN__
