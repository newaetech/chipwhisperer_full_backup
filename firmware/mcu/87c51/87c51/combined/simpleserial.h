// simpleserial.h

#ifndef SIMPLESERIAL_H
#define SIMPLESERIAL_H

#include <stdint.h>

// Possible return values for simpleserial_get
#define ss_get_bad 0 // bad command - no ack
#define ss_get_ok  1 // for k and t - just ack
#define ss_get_enc 2 // for p - encrypt then ack


// Get a message from the serial port. If we just finished receiving 
// plaintext (p01234567\n), key (k89ABCDEF\n), or expected (t00112233\n),
// then decode the ASCII buffer and put the result in input[], key[], or exp[]
// Return 1 if we just received an input; otherwise, return 0.
int simpleserial_get(uint8_t* input, uint8_t* key, uint8_t* exp, uint8_t size_input, uint8_t size_key);

// Encode the <size> byte array into ASCII and send it over the serial port in 
// the format "r01234567\n".
void simpleserial_put(uint8_t* output, uint8_t size);

// Respond to a command with z00\n
void simpleserial_ack(void);


#endif // SIMPLESERIAL_H