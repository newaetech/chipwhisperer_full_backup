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

#include <string.h>

#include "qm_soc_regs.h"
#include "qm_gpio.h"
#include "qm_init.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_pinmux.h"

#include "fw-manager.h"
#include "fw-manager_config.h"
#include "fm_entry.h"
#include "dfu/qda/qda.h"

#if FM_CONFIG_USE_AON_GPIO_PORT
#define FM_GPIO_PORT QM_AON_GPIO_0
#else
#define FM_GPIO_PORT QM_GPIO_0
#endif

void fm_entry_uart(void)
{
	qm_gpio_state_t state;

	/*
	 * qda_init() implicitly init the HW required by XMODEM (i.e., UART and
	 * PIC timer) and the DFU state machine
	 */
	qda_init();
	do {
		/*
		 * The following function returns only when no data is received
		 * for 10 seconds.
		 */
		qda_receive_loop();
		qm_gpio_read_pin(FM_GPIO_PORT, FM_CONFIG_GPIO_PIN, &state);
	} while (state == QM_GPIO_LOW);
	/*
	 * Cold reboot in order to restore the default system configuration
	 * (thus getting rid of all the changes done by the FM mode, like the
	 * UART configuration).
	 */
	qm_soc_reset(QM_COLD_RESET);
}
