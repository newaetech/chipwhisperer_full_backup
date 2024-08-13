// esp-hal.h
// HAL interface for ESP32 devices

#ifndef ESP_HAL_H
#define ESP_HAL_H

#include <stdint.h>

void platform_init();

// Set up the UART. Call this once.
void init_uart();

// Write a single character to the UART.
void putch(char c);

// Get a single character from the UART.
char getch();

void trigger_setup();
void trigger_high();
void trigger_low();

// TODO: LED functions

void aes128_enc(uint8_t* pt, uint8_t* k);

#endif // ESP_HAL_H
