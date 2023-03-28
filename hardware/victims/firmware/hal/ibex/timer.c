// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "demo_system.h"
#include "dev_access.h"
#include "timer.h"

volatile uint64_t time_elapsed;
uint64_t time_increment;

void timecmp_update(uint64_t new_time) {
  DEV_WRITE(TIMER_BASE + TIMER_MTIMECMP_REG, -1);
  DEV_WRITE(TIMER_BASE + TIMER_MTIMECMPH_REG, new_time >> 32);
  DEV_WRITE(TIMER_BASE + TIMER_MTIMECMP_REG, new_time);
}

inline static void increment_timecmp(uint64_t time_base) {
  uint64_t current_time = timer_read();
  current_time += time_base;
  timecmp_update(current_time);
}

void simple_timer_handler(void) __attribute__((interrupt));

void simple_timer_handler(void) {
  increment_timecmp(time_increment);
  time_elapsed++;
}

void timer_init(void) {
  install_exception_handler(7, &simple_timer_handler);
}

uint64_t timer_read() {
  uint32_t current_timeh;
  uint32_t current_time;
  // check if time overflowed while reading and try again
  do {
    current_timeh = DEV_READ(TIMER_BASE + TIMER_MTIMEH_REG);
    current_time = DEV_READ(TIMER_BASE + TIMER_MTIME_REG);
  } while (current_timeh != DEV_READ(TIMER_BASE + TIMER_MTIMEH_REG));
  uint64_t final_time = ((uint64_t)current_timeh << 32) | current_time;
  return final_time;
}

uint64_t get_elapsed_time(void) { return time_elapsed; }

void timer_enable(uint64_t time_base) {
  time_elapsed = 0;
  time_increment = time_base;
  // Set timer values
  increment_timecmp(time_base);
  enable_interrupts(TIMER_IRQ);
  set_global_interrupt_enable(1);
}

void timer_disable(void) { asm volatile("csrc  mie, %0\n" : : "r"(0x80)); }
