// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef GPIO_H__
#define GPIO_H__

#include <stdint.h>

#define GPIO_OUT_REG 0x0
#define GPIO_IN_REG 0x4
#define GPIO_IN_DBNC_REG 0x8
#define GPIO_OUT_SHIFT_REG 0xC

#define GPIO_OUT_MASK 0xF // Support 4-bit output

typedef void* gpio_t;

#define GPIO_FROM_BASE_ADDR(addr)((gpio_t)addr)

void set_outputs(gpio_t gpio, uint32_t outputs);
uint32_t read_gpio(gpio_t gpio);

void set_output_bit(gpio_t gpio, uint32_t output_bit_index,
    uint32_t output_bit);

uint32_t get_output_bit(gpio_t gpio, uint32_t output_bit_index);

#endif
