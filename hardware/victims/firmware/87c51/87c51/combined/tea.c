// tea.c
// Implementation of crypto with TEA (ECB)

// test:
// k000102030405060708090A0B0C0D0E0F  \r
// p0102030405060708  \r
// rB1A1AB198C45FA5B  \r

#include "modules.h"

#include <stdio.h>
#include <stdint.h>
#include <8052.h>

#define TEA_BLOCK_SIZE 8
#define TEA_KEY_SIZE   16

void tea_encrypt(uint8_t* v, uint8_t* k)
{
	uint32_t vp[2];
	uint32_t kp[4];
	
	uint8_t i, j;
    uint32_t sum=0;                             /* set up */
    uint32_t delta=0x9e3779b9;                     /* a key schedule constant */
	
	// Move 8 bit arrays into 32 bits
	for(i = 0; i < 2; i++){
		vp[i] = 0;
		for(j = 0; j < 4; j++)
			vp[i] = (vp[i] << 8) | v[4*i + j]; 
	}
	
	for(i = 0; i < 4; i++){
		kp[i] = 0;
		for(j = 0; j < 4; j++)
			kp[i] = (kp[i] << 8) | k[4*i + j]; 
	}
	
    for (i=0; i < 32; i++) {                       /* basic cycle start */
		uint32_t a, b, c;
		
        sum += delta;
		
//        v[0] += ((v[1]<<4) + k[0]) ^ (v[1] + sum) ^ ((v[1]>>5) + k[1]);
		a = (vp[1] << 4) + kp[0];
		b = (vp[1] + sum);
		c = (vp[1] >> 5) + kp[1];
		vp[0] += (a ^ b ^ c);
		
//        v[1] += ((v[0]<<4) + k[2]) ^ (v[0] + sum) ^ ((v[0]>>5) + k[3]);
		a = (vp[0] << 4) + kp[2];
		b = (vp[0] + sum);
		c = (vp[0] >> 5) + kp[3];
		vp[1] += (a ^ b ^ c);
    }  
	
	// Move output back into 8 bits
	for(i = 0; i < 8; i++)
	{
		uint8_t shift = 24 - 8 * (i%4);
		v[i] = (vp[i/4] >> shift) & 0xff;
	}
}

void tea_auth(uint8_t* v, uint8_t* k, uint8_t* e)
{
	uint8_t res = 0x10;
	uint8_t i;
	tea_encrypt(v, k);
	for(i = 0; i < TEA_BLOCK_SIZE; i++)
	{
		if(v[i] != e[i])
			res = 0;
		v[i] = 0;
	}
	v[0] = res;
}