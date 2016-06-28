// decryption.h
// Super-secret decryption used in bootloader
//
// Authors:
//   Greg d'Eon <gdeon@newae.com>

#include <stdint.h>

// Decrypt len bytes in the buffer
void decrypt_data(uint8_t* buffer, uint8_t len);