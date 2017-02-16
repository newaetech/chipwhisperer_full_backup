// simpleserial.h
// Generic module for interpreting SimpleSerial commands

#ifndef SIMPLESERIAL_H
#define SIMPLESERIAL_H

#include <stdint.h>

int simpleserial_addcmd(char c, unsigned int len, void (*fp)(uint8_t*));
void simpleserial_get(void);
void simpleserial_put(char c, int size, uint8_t* output);

#endif // SIMPLESERIAL_H
