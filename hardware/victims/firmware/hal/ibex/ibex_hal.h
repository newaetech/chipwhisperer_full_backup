// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef IBEX_HAL_H
#define IBEX_HAL_H

void trigger_high (void);

void trigger_low (void);

int getch (void);

void putch (char c);

void print (const char *ptr);

void init_uart(void);

void trigger_setup(void);

void platform_init(void);

#endif
