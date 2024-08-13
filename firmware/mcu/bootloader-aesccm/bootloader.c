/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2016 NewAE Technology Inc <coflynn@newae.com>

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

#include "hal.h"

#include "aes128_enc.h"
#include "aes_keyschedule.h"

// Nobody will ever find this, right?
#include "supersecret.h"	

//If building on something besides avr-gcc target, need to replace this
#include <util/crc16.h>

// CRC check replies
#define COMM_BADCHECKSUM 0xA1
#define COMM_OK 0xA4

// Signature check replies
#define COMM_BADSIG 0xB1
#define COMM_OKSIG 0xB4

uint8_t read_block(uint8_t * data_ptr)
{
    uint8_t c;
    uint16_t crc;
    
    //Initial Value
    crc = 0x0000;
    
    //Read 16 Bytes now            
    for(uint8_t i = 0; i < 16; i++){
        c = (uint8_t)getch();
        crc = _crc_xmodem_update(crc, c);
        data_ptr[i] = c;
    }
    
    //Validate CRC-16
    uint16_t inpcrc = (uint16_t)getch() << 8;
    inpcrc |= (uint8_t)getch();
    
    if (inpcrc == crc){
        return 1;
    } else {
        return 0;
    }
}

void xor16(uint8_t * dst, uint8_t * src)
{
    for(uint8_t i = 0; i < 16;i++){
        *(dst + i) ^= *(src + i);
    }
}

void copy16(uint8_t * dst, uint8_t * src)
{
    for(uint8_t i = 0; i < 16;i++){
        *(dst + i) = *(src + i);
    }
}

void clear16(uint8_t * dst)
{
    for(uint8_t i = 0; i < 16;i++){
        *(dst + i) = 0;
    }
}

/* Time-independant compare for 16 bytes */
uint8_t cmp16_isdifferent(uint8_t * dst, uint8_t * src)
{
    uint8_t differ = 0;
    for(uint8_t i = 0; i < 16;i++){
        differ |= *(dst+i) ^ *(src+i);
    }
    return differ;
}

int main
	(
	void
	)
	{
    platform_init();
	init_uart();	
	trigger_setup();
	
 	/* Uncomment this to get a HELLO message for debug */
	/*
	putch('h');
	putch('e');
	putch('l');
	putch('l');
	putch('o');
	putch('\n');
	*/
	    
	//Load Super-Secret key
    aes128_ctx_t ctx;
    uint8_t input_buffer[16] = SECRET_KEY;
    aes128_init(input_buffer, &ctx);
    clear16(input_buffer);

    //Load super-secret IV
    uint8_t iv[16] = AUTH_IV;
       
    //Load AES-CTR ctr
    uint8_t ctr[16] = CTR_NONCE;
    uint8_t ctr_dec[16];
       
    //Expected encrypted MAC Auth-Tag
    uint8_t encrypted_authtag[16];
    uint8_t state_authtag[16];
       
    //Expected length
    uint16_t expected_blocklen = 0;
       
    //Valid flag used to check movement through states
    uint8_t bootloader_valid = 0;
       
    //Do this forever (we don't actually have a jump to bootloader)
    uint8_t c;
    
    uint16_t aes_ctr_temp = 0;
    
    while(1){
        c = (uint8_t)getch();
        if (c == 1){
            /** FRAME TYPE #1: Header / Length **/
        
            bootloader_valid = 0;
            if (read_block(input_buffer)){
                //Uncomment following to get trigger on first block
                //trigger_high();
            
                expected_blocklen = input_buffer[15] | ((uint16_t)input_buffer[14] << 8);
                //Invalid length - just silently disable bootloader
                if (expected_blocklen > 0x2FFF){
                    expected_blocklen = 0;
                } else {
                    bootloader_valid = 1;
                    
                    //AES-CBC, initial state
                    copy16(state_authtag, iv);
                    aes128_enc(state_authtag, &ctx);
                    
                    trigger_low();

                    //AES-CBC, add auth header
                    xor16(state_authtag, input_buffer);
                    aes128_enc(state_authtag, &ctx);                    
                }
                putch(COMM_OK);
            } else {
                putch(COMM_BADCHECKSUM);
            }
            
            trigger_low();
        
        } else if (c == 2){
            /** FRAME TYPE #2: Encrypted Auth Tag **/
            
            if (bootloader_valid != 1){
                bootloader_valid = 0;
                continue;
            }
            
            //Uncomment following to get trace on second block
            //trigger_high();
            
            //Store encrypted auth-tag, we'll check it much later on
            if (read_block(encrypted_authtag)){
                bootloader_valid = 2;
                aes_ctr_temp = 0;
                putch(COMM_OK);
            } else {
                bootloader_valid = 0;
                putch(COMM_BADCHECKSUM);
            }
            
            trigger_low();

        } else if (c == 3){
            /** FRAME TYPE #3: Data **/
            if (bootloader_valid != 2){
                bootloader_valid = 0;
                continue;
            }
            
            //Receive data
            if (read_block(input_buffer)){
                trigger_high();
            
                //Increment counter
                aes_ctr_temp++;
                copy16(ctr_dec, ctr);
                ctr_dec[14] = (aes_ctr_temp >> 8) & 0xff;
                ctr_dec[15] = aes_ctr_temp & 0xff;
                
                //Generate counter "pad"
                aes128_enc(ctr_dec, &ctx); 
                xor16(ctr_dec, input_buffer);
                
                trigger_low();
                
                //ctr_dec is now decrypted
                //perform AES-CBC mode
                xor16(state_authtag, ctr_dec);
                aes128_enc(state_authtag, &ctx);
                
                putch(COMM_OK);
                
                //Check if last block
                if (aes_ctr_temp == expected_blocklen){
                    //Encrypt the MAC, see if we get a match
                    copy16(ctr_dec, ctr);
                    ctr_dec[14] = 0;
                    ctr_dec[15] = 0;
                    aes128_enc(ctr_dec, &ctx);
                    xor16(state_authtag, ctr_dec);
                    
                    if(cmp16_isdifferent(state_authtag, encrypted_authtag)){
                        putch(COMM_BADSIG);
                    } else {
                        putch(COMM_OKSIG);
                    }
                    
                } 
            } else {
                putch(COMM_BADCHECKSUM);
            }
            trigger_low();
        } else {
            //Invalid response, drop everything
            aes_ctr_temp = 0;
            expected_blocklen = 0;
            bootloader_valid= 0;
            ctr[14] = 0xff;
            ctr[15] = 0xff;        
        }
    }
	 
	return 1;
	}
	
	