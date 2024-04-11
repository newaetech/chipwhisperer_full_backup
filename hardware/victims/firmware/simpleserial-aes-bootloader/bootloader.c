/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2012-2017 NewAE Technology Inc.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "aes-independant.h"
#include "hal.h"
#include "simpleserial.h"
#include <stdint.h>
#include <stdlib.h>

#undef DEFAULT_KEY
#define DEFAULT_KEY 0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c
#define HASH_KEY 0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6

uint8_t cipher_buf[0x08 + 0x10 + 0x01] = {HASH_KEY};
uint8_t key[] = {DEFAULT_KEY};
uint8_t IV[16] = {0};
uint8_t plaintext[16] = {0};
uint32_t mem_addr = 0;
uint8_t initialized = 0;

// HASH(key, ciphertext) = MAC
// buf[:16] = ciphertext
// buf[16] = MAC
void AES128_ECB_decrypt(uint8_t* input, uint8_t* key, uint8_t *output);
uint8_t ss_crc(uint8_t *, uint8_t);

//basically just getting IV
uint8_t init_bootloader(uint8_t cmd, uint8_t scmd, uint8_t len, uint8_t *buf)
{
    if (initialized) {
        return 0x12;
    }

    if (len != 16) {
        return SS_ERR_LEN;
    }

    for (uint8_t i = 0; i < 16; i++) {
        IV[i] = buf[i];
    }
    initialized = 1;
    return 0;
}

uint8_t set_addr(uint8_t cmd, uint8_t scmd, uint8_t len, uint8_t *buf)
{
    if (len != 4) {
        return SS_ERR_LEN;
    }
    mem_addr = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
    return 0;
}

uint8_t read_mem(uint8_t cmd, uint8_t scmd, uint8_t len, uint8_t *buf)
{
    return 0x20; //permission error
}

uint8_t write_plaintext(uint8_t len, uint8_t *plaintext)
{
    mem_addr += len;
    return 0;
}

uint8_t bootloader_recv(uint8_t cmd, uint8_t scmd, uint8_t len, uint8_t *buf)
{
    if (!initialized) {
        return 0x12;
    }

    if (len != 0x11) {
        return SS_ERR_LEN;
    }

    for (int i = 0; i < 16; i++) {
        cipher_buf[0x08 + i] = buf[i];
    }

    //calculate MAC
    trigger_high();
    uint8_t crc = ss_crc(cipher_buf, 0x08 + 0x10);

    // check that MAC is correct
    if (crc != buf[16]) {
        trigger_low();
        return 0x11;
    }
    trigger_low();

    //decrypt
    AES128_ECB_decrypt(buf, key, plaintext);

    //and do CBC
    for (int i = 0; i < 16; i++) {
        plaintext[i] ^= IV[i];
        IV[i] = buf[i];
    }

    write_plaintext(16, plaintext);
    return 0x00;
}


int main(void)
{
	uint8_t tmp[KEY_LENGTH] = {DEFAULT_KEY};

    platform_init();
    init_uart();
    trigger_setup();

	aes_indep_init();
	aes_indep_key(tmp);

	simpleserial_init();

    simpleserial_addcmd(0x00, 0x10, init_bootloader);
    simpleserial_addcmd(0x01, 0x11, bootloader_recv);
    simpleserial_addcmd(0x02, 0x04, set_addr);
    simpleserial_addcmd(0x03, 0x01, read_mem);
    while(1)
        simpleserial_get();
}
