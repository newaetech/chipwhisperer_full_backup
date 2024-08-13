// TEA encryption methods
//
// Authors: 
//   Greg d'Eon <gdeon@newae.com>

#include <stdint.h>

void tea_encrypt(uint32_t* v, uint32_t* k)
{
    uint32_t sum=0, i;                             /* set up */
    uint32_t delta=0x9e3779b9;                     /* a key schedule constant */
    for (i=0; i < 32; i++) {                       /* basic cycle start */
        sum += delta;
		
//        v[0] += ((v[1]<<4) + k[0]) ^ (v[1] + sum) ^ ((v[1]>>5) + k[1]);
		uint32_t a = (v[1] << 4) + k[0];
		uint32_t b = (v[1] + sum);
		uint32_t c = (v[1] >> 5) + k[1];
		v[0] += (a ^ b ^ c);
		
//        v[1] += ((v[0]<<4) + k[2]) ^ (v[0] + sum) ^ ((v[0]>>5) + k[3]);
		a = (v[0] << 4) + k[2];
		b = (v[0] + sum);
		c = (v[0] >> 5) + k[3];
		v[1] += (a ^ b ^ c);
    }  
}