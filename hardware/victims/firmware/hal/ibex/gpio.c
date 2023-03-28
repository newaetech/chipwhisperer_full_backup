// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "gpio.h"
#include "dev_access.h"
#include "demo_system.h"

void set_outputs(gpio_t gpio, uint32_t outputs) {
  DEV_WRITE(gpio, outputs);
}

uint32_t read_gpio(gpio_t gpio) {
  return DEV_READ(gpio);
}

void set_output_bit(gpio_t gpio, uint32_t output_bit_index,
    uint32_t output_bit) {
  output_bit &= 1;

  uint32_t output_bits = read_gpio(gpio);
  output_bits &= ~(1 << output_bit_index);
  output_bits |= (output_bit << output_bit_index);

  set_outputs(gpio, output_bits);
}

uint32_t get_output_bit(gpio_t gpio, uint32_t output_bit_index) {
  uint32_t output_bits = read_gpio(gpio);
  output_bits >>= output_bit_index;
  output_bits &= 1;

  return output_bits;
}
