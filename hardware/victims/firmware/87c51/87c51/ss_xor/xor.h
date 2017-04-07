// xor.h

#ifndef XOR_H
#define XOR_H

#include <stdint.h>

// Encrypt (or decrypt) the state matrix with an XOR operation
// Assumes that state[] and key[] are 16 bytes long
void xor_encrypt(uint8_t* state, uint8_t* key);

#endif // XOR