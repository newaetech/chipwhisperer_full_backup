// modules.h
// Exposes all of the module main() methods

#ifndef MODULES_H
#define MODULES_H

#include <stdint.h>

void main_print();
void main_passcheck();
void main_glitchloop();
void main_ss(uint8_t mode);

// Modes for SimpleSerial driver
#define SS_AES 0
#define SS_TEA 1
#define SS_XOR 2


#endif // MODULES_H