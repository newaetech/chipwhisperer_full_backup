#ifndef MICKEY128_H_
#define MICKEY128_H_

#include <stdint.h>

typedef struct mickey128_ctx_st{
	uint8_t r[20];
	uint8_t s[20];
} mickey128_ctx_t;

void debug_clock_r(void);
void debug_clock_s(void);
void mickey128_init(void* key, uint16_t keysize_b, 
                    void* iv,  uint16_t ivsize_b, 
                    mickey128_ctx_t* ctx);
uint8_t mickey128_getbit(mickey128_ctx_t* ctx);
uint8_t mickey128_getbyte(mickey128_ctx_t* ctx);

#endif /*MICKEY128_H_*/
