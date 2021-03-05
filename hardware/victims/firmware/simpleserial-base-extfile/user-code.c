#include "simpleserial.h"
#include "hal.h"

static uint8_t known_password[SECRET_KEY_LEN] = {'p', 'a', 's', 's', 'w', 'o', 'r', 'd'};

uint8_t process_secret_key(uint8_t* key, uint8_t len)
{
    //Do something with key if needed here (store to local variable for example)
    for(uint8_t i = 0; i < SECRET_KEY_LEN; i++){
        ;
    }

    return 0x00;
}

uint8_t process_input(uint8_t* pt, uint8_t len)
{
    uint8_t pass_ok = 0x00;

    //Example - check password time-invariant method
    trigger_high();
    for(uint8_t i = 0; i < INPUT_LEN; i++){
        pass_ok |= (pt[i] ^ known_password[i]);
    }
    trigger_low();

    return pass_ok;
}
