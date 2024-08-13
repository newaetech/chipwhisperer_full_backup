// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "uart.h"
#include "dev_access.h"
#include "demo_system.h"

void uart_enable_rx_int(void) {
  enable_interrupts(UART_IRQ);
  set_global_interrupt_enable(1);
}

int uart_in(uart_t uart) {
  int res = UART_EOF;

  if (!(DEV_READ(uart + UART_STATUS_REG) & UART_STATUS_RX_EMPTY)) {
    res = DEV_READ(uart + UART_RX_REG);
  }

  return res;
}

void uart_out(uart_t uart, char c) {
  while(DEV_READ(uart + UART_STATUS_REG) & UART_STATUS_TX_FULL);

  DEV_WRITE(uart + UART_TX_REG, c);
}
