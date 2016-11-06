/*
 * Copyright (c) 2016, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the Intel Corporation nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
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

#ifndef __SOC_BOOT_H__
#define __SOC_BOOT_H__

#include "interrupt/mvic.h"

/* Lakemont application's entry point */
#define LMT_APP_ADDR (0x00180000)

/* TODO: consider changing the following macros with 'alias' __attribute__ */
/* Init the interrupt controller (MVIC for Quark D2000) */
#define soc_boot_init_interrupt_controller(...) mvic_init(__VA_ARGS__)

/* CPU halt function (used by the double fault handler) */
#define soc_boot_halt_cpu(...) power_cpu_halt(__VA_ARGS__)

/* Sleep function called when there is no application to run */
/*
 * TODO: change this to a function that enables the JTAG pin as input and calls
 * power_soc_deep_sleep(POWER_WAKE_FROM_GPIO_COMP).
 */
#define soc_boot_sleep(...) power_cpu_halt(__VA_ARGS__)

#endif /* __SOC_BOOT_H__ */
