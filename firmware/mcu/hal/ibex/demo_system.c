// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "demo_system.h"
#include "dev_access.h"
#include "uart.h"
#include "dev_access.h"

int putchar(int c) {
#ifdef SIM_CTRL_OUTPUT
  DEV_WRITE(SIM_CTRL_BASE + SIM_CTRL_OUT, c);
#else
  if (c == '\n') {
    uart_out(DEFAULT_UART, '\r');
  }

  uart_out(DEFAULT_UART, c);
#endif

  return c;
}

int getchar(void) {
  return uart_in(DEFAULT_UART);
}

int puts(const char *str) {
  while (*str) {
    putchar(*str++);
  }

  return 0;
}

void puthex(uint32_t h) {
  int cur_digit;
  // Iterate through h taking top 4 bits each time and outputting ASCII of hex
  // digit for those 4 bits
  for (int i = 0; i < 8; i++) {
    cur_digit = h >> 28;

    if (cur_digit < 10)
      putchar('0' + cur_digit);
    else
      putchar('A' - 10 + cur_digit);

    h <<= 4;
  }
}

void sim_halt() { DEV_WRITE(SIM_CTRL_BASE + SIM_CTRL_CTRL, 1); }

unsigned int get_mepc() {
  uint32_t result;
  __asm__ volatile("csrr %0, mepc;" : "=r"(result));
  return result;
}

unsigned int get_mcause() {
  uint32_t result;
  __asm__ volatile("csrr %0, mcause;" : "=r"(result));
  return result;
}

unsigned int get_mtval() {
  uint32_t result;
  __asm__ volatile("csrr %0, mtval;" : "=r"(result));
  return result;
}

uint32_t get_mcycle(void) {
  uint32_t result;
  __asm__ volatile("csrr %0, mcycle;" : "=r"(result));
  return result;
}

void reset_mcycle(void) {
  __asm__ volatile("csrw mcycle, x0");
}

extern uint32_t _vectors_start;
volatile uint32_t* exc_vectors = &_vectors_start;

int install_exception_handler(uint32_t vector_num, void(*handler_fn)(void)) {
  if (vector_num >= 32)
    return 1;

  volatile uint32_t* handler_jmp_loc = exc_vectors + vector_num;
  int32_t offset = (uint32_t)handler_fn - (uint32_t)handler_jmp_loc;

  if ((offset  >= (1 << 19)) || (offset  < -(1 << 19))) {
    return 2;
  }

  uint32_t offset_uimm = offset;

  uint32_t jmp_ins =
    ((offset_uimm & 0x7fe) << 20) | // imm[10:1] -> 21
    ((offset_uimm & 0x800) << 9) | // imm[11] -> 20
    (offset_uimm & 0xff000) | // imm[19:12] -> 12
    ((offset_uimm & 0x100000) << 11) | // imm[20] -> 31
    0x6f; // J opcode

  *handler_jmp_loc = jmp_ins;

  __asm__ volatile("fence.i;");

  return 0;
}

void enable_interrupts(uint32_t enable_mask) {
  asm volatile("csrs mie, %0\n" : : "r"(enable_mask));
}

void disable_interrupts(uint32_t disable_mask) {
  asm volatile("csrc mie, %0\n" : : "r"(disable_mask));
}

void set_global_interrupt_enable(uint32_t enable) {
  if (enable) {
    asm volatile("csrs mstatus, %0\n" : : "r"(1<<3));
  } else {
    asm volatile("csrc mstatus, %0\n" : : "r"(1<<3));
  }
}

void simple_exc_handler(void) {
  puts("EXCEPTION!!!\n");
  puts("============\n");
  puts("MEPC:   0x");
  puthex(get_mepc());
  puts("\nMCAUSE: 0x");
  puthex(get_mcause());
  puts("\nMTVAL:  0x");
  puthex(get_mtval());
  putchar('\n');

  while(1);
}

