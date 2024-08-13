#include "hal.h" // HAL function definitions
#include "simpleserial.h" // include simpleserial
#include <stdint.h>
#include <stdlib.h>
static uint8_t key[16] = {0}; // initialize key as all 0's

// Read key in
uint8_t get_key(uint8_t cmd, uint8_t scmd, uint8_t len, uint8_t *buf)
{
    if (len != 16) {
        return 0x04; // SS_ERR_LEN
    }
    for (uint8_t i = 0; i < 16; i++) {
        key[i] = buf[i]; // set key
    }

    return 0x00; //SS_ERR_OK
}

// read plaintext, do encryption, then send ciphertext back
uint8_t xor_inc(uint8_t cmd, uint8_t scmd, uint8_t len, uint8_t *buf)
{
    if (len != 16) {
        return 0x04; // SS_ERR_LEN
    }

    trigger_high();
    for (uint8_t i = 0; i < 16; i++) {
        buf[i] ^= key[i]; // do "encryption"
    }
    trigger_low();

    simpleserial_put('r', 16, buf); // send ciphertext back

    return 0x00; //SS_ERR_OK
}

int main(void)
{
    // setup
    platform_init();
    init_uart();
    trigger_setup();
    simpleserial_init();

    // tell simpleserial_get to look for 'p' and 'k' packets
    // and to do these two functions when they're received
    simpleserial_addcmd('p', 16, xor_inc);
    simpleserial_addcmd('k', 16, get_key);

    // look for simpleserial packets
    while(1)
        simpleserial_get();
}
