// tea.h

#ifndef TEA_H
#define TEA_H

#include <stdint.h>

// Encrypt (or decrypt) the state matrix with TEA
// Encryption is done in-place and result is stored in state[]
void tea_encrypt(uint8_t* state, uint8_t* key);

#endif // TEA