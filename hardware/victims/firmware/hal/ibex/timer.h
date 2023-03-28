// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef TIMER_H__
#define TIMER_H__

#define TIMER_MTIME_REG 0x0
#define TIMER_MTIMEH_REG 0x4
#define TIMER_MTIMECMP_REG 0x8
#define TIMER_MTIMECMPH_REG 0xC

void timer_init();
uint64_t timer_read();
uint64_t get_elapsed_time();
void timer_enable(uint64_t time_base);
void timer_disable();

#endif // TIMER_H__
