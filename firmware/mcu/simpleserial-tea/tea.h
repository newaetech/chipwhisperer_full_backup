// Definitions for TEA encryption
// 
// Authors:
//   Greg d'Eon <gdeon@newae.com>

#include <stdint.h>

// TEA uses 64 bit (8 byte) blocks and 128 bit (16 byte) keys
// Here, a word is 32 bits = 4 bytes
#define TEA_BLOCK_WORDS 2
#define TEA_BLOCK_BYTES 8
#define TEA_KEY_WORDS 4
#define TEA_KEY_BYTES 16

// Encrypt the plaintext v (in-place) using the key k
void tea_encrypt(uint32_t* v, uint32_t* k);