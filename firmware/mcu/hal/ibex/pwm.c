// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "pwm.h"
#include "dev_access.h"

void set_pwm(pwm_t pwm, uint32_t counter, uint32_t pulse_width) {
  DEV_WRITE(&pwm[1], counter);
  DEV_WRITE(&pwm[0], pulse_width);
}
