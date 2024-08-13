// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef PWM_H__
#define PWM_H__

#include <stdint.h>

typedef uint32_t* pwm_t;

#define PWM_FROM_ADDR_AND_INDEX(addr, index)(&(((pwm_t) addr)[2*index]))

void set_pwm(pwm_t pwm, uint32_t counter, uint32_t pulse_width);

#endif
