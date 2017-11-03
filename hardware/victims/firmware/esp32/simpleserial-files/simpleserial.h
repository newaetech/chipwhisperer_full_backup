// simpleserial.h
// Generic module for interpreting SimpleSerial commands

#ifndef SIMPLESERIAL_H
#define SIMPLESERIAL_H

#include <stdint.h>

// Set up the SimpleSerial module
// This prepares any internal commands
void simpleserial_init(void);

// Add a command to the SimpleSerial module
// Args:
// - c:   The character designating this command
// - len: The number of bytes expected
// - fp:  A pointer to a callback, which is called after receiving data
// Example: simpleserial_addcmd('p', 16, encrypt_text)
// - Calls encrypt_text() with a 16 byte array after receiving a line 
//   like p00112233445566778899AABBCCDDEEFF\n
// Notes:
// - Maximum of 10 active commands
// - Maximum length of 64 bytes
// - Returns 1 if either of these fail; otherwise 0
// - The callback function returns a number in [0x00, 0xFF] as a status code;
//   in protocol v1.1, this status code is returned through a "z" message
int simpleserial_addcmd(char c, unsigned int len, uint8_t (*fp)(uint8_t*));

// Attempt to process a command 
// If a full string is found, the relevant callback function is called
// Might return without calling a callback for several reasons:
// - First character didn't match any known commands
// - One of the characters wasn't in [0-9|A-F|a-f]
// - Data was too short or too long
void simpleserial_get(void);

// Write some data to the serial port
// Prepends the character c to the start of the line
// Example: simpleserial_put('r', 16, ciphertext)
void simpleserial_put(char c, int size, uint8_t* output);

#endif // SIMPLESERIAL_H
