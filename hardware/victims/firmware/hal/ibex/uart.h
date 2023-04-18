// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef UART_H__
#define UART_H__

#define UART_RX_REG 0
#define UART_TX_REG 4
#define UART_STATUS_REG 8

#define UART_STATUS_RX_EMPTY 1
#define UART_STATUS_TX_FULL 2

#define UART_EOF -1

typedef void* uart_t;

#define UART_FROM_BASE_ADDR(addr)((uart_t)(addr))

void uart_enable_rx_int(void);
int uart_in(uart_t uart);
void uart_out(uart_t uart, char c);

#endif // UART_H__

