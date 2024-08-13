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

/* Initialize bare-metal application environment and jump to application's
 * entry point.
 *
 * Standard system startup files are not used.
 * The 'main' function receives no parameters and the return code is ignored.
 */

#include <string.h>
#include "qm_common.h"
#include "qm_uart.h"

extern void main(void);
#if (QM_SENSOR)
void sensor_init(void);
#else
void __attribute__((section(".text.entry"))) _start(void);
#endif

void _start(void)
{
	extern uint32_t __bss_start[];
	extern uint32_t __data_vma[];
	extern uint32_t __data_lma[];
	extern uint32_t __data_size[];
	extern uint32_t __bss_end[];

	/* Zero out bss */
	memset(__bss_start, 0x00, (uint32_t)__bss_end - (uint32_t)__bss_start);

	/* Copy initialised variables */
	memcpy(__data_vma, __data_lma, (size_t)__data_size);

/* Now, Sensor subsystem is prepared for C code */

#if (QM_SENSOR)
	/* Initialize the Sensor Subsystem */
	sensor_init();
#endif /* QM_SENSOR */

#if (STDOUT_UART_INIT)
#if (PRINTF_ENABLE || PUTS_ENABLE)
	stdout_uart_setup(BOOTROM_UART_115200);
#endif /* PRINTF_ENABLE || PUTS_ENABLE */
#endif /* STDOUT_UART_INIT */

	main();
}
