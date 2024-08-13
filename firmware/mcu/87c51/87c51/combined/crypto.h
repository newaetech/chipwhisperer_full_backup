// crypto.h
// Encryption and authentication functions
// These are implemented in aes.c, tea.c, and xor.c

#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdint.h>

// Encrypt (or decrypt) the state matrix with AES-128
// Encryption is done in-place and result is stored in state[]
void aes_encrypt(uint8_t* state, uint8_t* key);

// Encrypt state[] with key[] and compare to exp[]
void aes_auth(uint8_t* state, uint8_t* key, uint8_t* exp);

// Encrypt v[] with k[] using TEA
void tea_encrypt(uint8_t* v, uint8_t* k);

// Encrypt v[] with k[] using TEA and compare to e[]
void tea_auth(uint8_t* v, uint8_t* k, uint8_t* e);

// XOR state[] with key[]
void xor_encrypt(uint8_t* state, uint8_t* key);

// XOR state[] with key[] and compare to exp[]
void xor_auth(uint8_t* state, uint8_t* key, uint8_t* exp);

#endif // CRYPTO_H