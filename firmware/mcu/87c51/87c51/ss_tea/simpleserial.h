// simpleserial.h

#ifndef SIMPLESERIAL_H
#define SIMPLESERIAL_H

#include <stdint.h>

// Get a message from the serial port. If we just finished receiving 
// plaintext (p01234567\n) or key (k89ABCDEF\n), then decode the ASCII buffer 
// and put the result in input[] or key[].
// Return 1 if we just received an input; otherwise, return 0.
int simpleserial_get(uint8_t* input, uint8_t* key, uint8_t size_input, uint8_t size_key);

// Encode the <size> byte array into ASCII and send it over the serial port in 
// the format "r01234567\n".
void simpleserial_put(uint8_t* output, uint8_t size);

#endif // SIMPLESERIAL_H