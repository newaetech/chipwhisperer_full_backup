/*
 * Copyright (c) 2016, NewAE Technology Inc.
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

#include <qm_common.h>
#include <qm_uart.h>
#include <qm_pinmux.h>
#include <qm_gpio.h>
#include "simpleserial.h"
#include "aes.h"

void uart_putchar(char c){
	qm_uart_write(QM_UART_0, c);
}

char uart_getchar(void){
	uint8_t c;
	while (qm_uart_read(QM_UART_0, &c, NULL) != 0);
	return (char)c;
}

#define TRIGGER_PIN QM_PIN_ID_10

/* SimpleSerial Example Application */
int main(void)
{
	uint8_t key[16];
	uint8_t pt[16];
	uint8_t ct[16];

	qm_gpio_port_config_t cfg;

    cfg.direction = BIT(TRIGGER_PIN);
    qm_gpio_set_config(QM_GPIO_0, &cfg);

    qm_gpio_clear_pin(QM_GPIO_0, TRIGGER_PIN);

	qm_pmux_select(QM_PIN_ID_12, QM_PMUX_FN_2); /* configure UART_A_TXD */
    qm_pmux_select(QM_PIN_ID_13, QM_PMUX_FN_2); /* configure UART_A_RXD */
    qm_pmux_input_en(QM_PIN_ID_13, true);       /* UART_A_RXD is an input */

    qm_pmux_select(QM_PIN_ID_5, QM_PMUX_FN_2); /* Set clock out */

    QM_PRINTF("D2000 SimpleSerial Test\n");

	struct tc_aes_key_sched_struct ks;
	TCAesKeySched_t ksp = &ks;

	while(1){
		if (simpleserial_get(pt, key, 16, 16)){

			tc_aes128_set_encrypt_key(ksp, key);

			qm_gpio_set_pin(QM_GPIO_0, TRIGGER_PIN);
			tc_aes_encrypt(ct, pt, ksp);
			qm_gpio_clear_pin(QM_GPIO_0, TRIGGER_PIN);

			simpleserial_put(ct, 16);
		}

	}

	return 0;
}
