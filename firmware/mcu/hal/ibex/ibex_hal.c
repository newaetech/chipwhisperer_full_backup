// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "ibex_hal.h"
#include "uart.h"
#include "gpio.h"
#include "demo_system.h"

void trigger_high (void) {
    set_outputs(GPIO_OUT, 0x01);
}

void trigger_low (void) {
    set_outputs(GPIO_OUT, 0x00);
}

int getch (void) {
    int uart_in_char;
    while ((uart_in_char = uart_in(DEFAULT_UART)) == -1) {
    }
    return uart_in_char;
}

void putch (char c) {
    uart_out(DEFAULT_UART, c);
}

void print (const char *ptr) {
    while (*ptr != 0) {
        uart_out(DEFAULT_UART, *ptr);
        ptr++;
    }
}

void platform_init(void) {
}

void init_uart(void) {
    //uart_enable_rx_int(); note: not needed as this is for when using interrupts
}

void trigger_setup(void) {
    set_outputs(GPIO_OUT, 0x00);
}
