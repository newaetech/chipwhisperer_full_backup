#ifndef STM32F3_HAL_SECCAN_H
#define STM32F3_HAL_SECCAN_H

#include <stdint.h>

int init_can(void);
int init_adc(void);
int init_pwm(void);

int write_can(uint32_t addr, uint8_t *data, int len);
int read_can(uint8_t *data, uint32_t *addr, int len);

int read_adc(uint16_t *val);

void change_pwm(uint16_t duty_cycle);

#endif