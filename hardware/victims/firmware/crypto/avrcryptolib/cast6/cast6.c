/* 
 * File:	cast6.c
 * Author:	Daniel Otte
 * Date: 	09.09.2006
 * License: GPL
 * Description: Implementation of the CAST6 (aka CAST-256) cipher algorithm as described in RFC 2612
 * 
 */

#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "cast6.h"
#include "cast6-sbox.h"

#define CHANGE_ENDIAN32(x) (((x)<<24 | (x)>>24 | ((x)&0xff00)<<8 | ((x)&0xff0000)>>8)&0xffffffff)


static
uint8_t kr(uint8_t i, const cast6_ctx_t* ctx){
	uint8_t ret;
	ret = ctx->krx[i/2];
	if(i&1){
		ret >>= 4;
	}else{
		ret &= 0x0f;
	}
	/* now get the high bit */
	ret |= ((ctx->krx[24+i/8])&(1<<(i%8)))?0x10:0x00;
	return ret;
}

static
void set_kr(uint8_t value, uint8_t i, cast6_ctx_t* ctx){
	value &= 0x1F;
	
	(ctx->krx[i/2]) &= 0xF0>>((i&1)*4); /* clear the location where v should go */
	(ctx->krx[i/2]) |= (value&0x0f)<<((i&1)*4); 
	
	/* now set the high bit */
	(ctx->krx[24+i/8]) &= ~(1<<(i%8)); /* clear the location where v should go */
	(ctx->krx[24+i/8]) |= (value>>4)<<(i%8); 
}

#define ROTL32(a,n) (((a)<<(n))|((a)>>(32-(n))))
#define ROTR32(a,n) (((a)>>(n))|((a)<<(32-(n))))

#define S1(a) (pgm_read_dword(&(s1[(a)])))
#define S2(a) (pgm_read_dword(&(s2[(a)])))
#define S3(a) (pgm_read_dword(&(s3[(a)])))
#define S4(a) (pgm_read_dword(&(s4[(a)])))

#define A ((uint8_t)(v>>(8*3)))
#define B ((uint8_t)(v>>(8*2)))
#define C ((uint8_t)(v>>(8*1)))
#define D ((uint8_t)(v>>(8*0)))


static
uint32_t f1(uint32_t v, uint8_t kri, uint32_t kmi){
	uint32_t o;
	kri &= 0x1F;
	v  = ROTL32(kmi+v, kri);
	o  = S1(A);
	o ^= S2(B);
	o -= S3(C);
	o += S4(D);
	return o;
}

static
uint32_t f2(uint32_t v, uint8_t kri, uint32_t kmi){
	uint32_t o;
	kri &= 0x1F;
	v  = ROTL32(kmi^v, kri);
	o  = S1(A);
	o -= S2(B);
	o += S3(C);
	o ^= S4(D);
	return o;
}

static
uint32_t f3(uint32_t v, uint8_t kri, uint32_t kmi){
	uint32_t o;
	kri &= 0x1F;
	v  = ROTL32(kmi-v, kri);
	o  = S1(A);
	o += S2(B);
	o ^= S3(C);
	o -= S4(D);
	return o;
}

#undef A
#undef B
#undef C
#undef D

#define A (((uint32_t*)buffer)[0])
#define B (((uint32_t*)buffer)[1])
#define C (((uint32_t*)buffer)[2])
#define D (((uint32_t*)buffer)[3])

static
void q(void* buffer, uint8_t i, const cast6_ctx_t* ctx){
	C ^= f1(D, kr(i*4+0, ctx), ctx->km[i][0]);
	B ^= f2(C, kr(i*4+1, ctx), ctx->km[i][1]);
	A ^= f3(B, kr(i*4+2, ctx), ctx->km[i][2]);
	D ^= f1(A, kr(i*4+3, ctx), ctx->km[i][3]);
}

static
void qbar(void* buffer, uint8_t i, const cast6_ctx_t* ctx){
	D ^= f1(A, kr(i*4+3, ctx), ctx->km[i][3]);
	A ^= f3(B, kr(i*4+2, ctx), ctx->km[i][2]);
	B ^= f2(C, kr(i*4+1, ctx), ctx->km[i][1]);
	C ^= f1(D, kr(i*4+0, ctx), ctx->km[i][0]);
}

void cast6_enc(void* buffer, const cast6_ctx_t* ctx){
	uint8_t i;
	for(i=0; i<32/4; ++i){
		((uint32_t*)buffer)[i] = CHANGE_ENDIAN32(((uint32_t*)buffer)[i]);
	}
	for(i=0; i<6; ++i){
		q(buffer, i, ctx);
	}
	for(i=6; i<12; ++i){
		qbar(buffer, i, ctx);
	}
	for(i=0; i<32/4; ++i){
		((uint32_t*)buffer)[i] = CHANGE_ENDIAN32(((uint32_t*)buffer)[i]);
	}
}

void cast6_dec(void* buffer, const cast6_ctx_t* ctx){
	uint8_t i;
	for(i=0; i<32/4; ++i){
		((uint32_t*)buffer)[i] = CHANGE_ENDIAN32(((uint32_t*)buffer)[i]);
	}
	for(i=12; i>6; --i){
		q(buffer, i-1, ctx);
	}
	for(i=6; i>0; --i){
		qbar(buffer, i-1, ctx);
	}
	for(i=0; i<32/4; ++i){
		((uint32_t*)buffer)[i] = CHANGE_ENDIAN32(((uint32_t*)buffer)[i]);
	}
}


#undef A
#undef B
#undef C
#undef D

#define A (((uint32_t*)buffer)[0])
#define B (((uint32_t*)buffer)[1])
#define C (((uint32_t*)buffer)[2])
#define D (((uint32_t*)buffer)[3])
#define E (((uint32_t*)buffer)[4])
#define F (((uint32_t*)buffer)[5])
#define G (((uint32_t*)buffer)[6])
#define H (((uint32_t*)buffer)[7])

/*
 * we might later make it optional to use this small thing
static
void w(void* buffer, uint8_t* tr, uint32_t* tm){
	G ^= f1(H, (tr[0]&0x0f)+(tr[5]&0x01)?0x10:0x00, tm[0]);
	F ^= f2(G, (tr[0]>>4)  +(tr[5]&0x02)?0x10:0x00, tm[1]);
	E ^= f3(F, (tr[1]&0x0f)+(tr[5]&0x04)?0x10:0x00, tm[2]);
	D ^= f1(E, (tr[1]>>4)  +(tr[5]&0x08)?0x10:0x00, tm[3]);
	C ^= f2(D, (tr[2]&0x0f)+(tr[5]&0x10)?0x10:0x00, tm[4]);
	B ^= f3(C, (tr[2]>>4)  +(tr[5]&0x20)?0x10:0x00, tm[5]);
	A ^= f1(B, (tr[3]&0x0f)+(tr[5]&0x40)?0x10:0x00, tm[6]);
	H ^= f2(A, (tr[3]>>4)  +(tr[5]&0x80)?0x10:0x00, tm[7]);
}
*/
static
void w(void* buffer, uint8_t* tr, uint32_t* tm){
	G ^= f1(H, tr[0], tm[0]);
	F ^= f2(G, tr[1], tm[1]);
	E ^= f3(F, tr[2], tm[2]);
	D ^= f1(E, tr[3], tm[3]);
	C ^= f2(D, tr[4], tm[4]);
	B ^= f3(C, tr[5], tm[5]);
	A ^= f1(B, tr[6], tm[6]);
	H ^= f2(A, tr[7], tm[7]);
}

/*
void dump_ctx(const cast6_ctx_t* ctx){
	uint8_t i,t;
	cli_putstr_P(PSTR("\r\n DBG:"));
	for(i=0; i<12; ++i){
		cli_putstr_P(PSTR("\r\n DBG:"));
		cli_putstr_P(PSTR(" rotk1="));
		t=kr(i*4+0, ctx);
		cli_hexdump(&t,1);
		cli_putstr_P(PSTR("          rotk2="));
		t=kr(i*4+1, ctx);
		cli_hexdump(&t,1);
		cli_putstr_P(PSTR("          rotk3="));
		t=kr(i*4+2, ctx);
		cli_hexdump(&t,1);
		cli_putstr_P(PSTR("          rotk4="));
		t=kr(i*4+3, ctx);
		cli_hexdump(&t,1);
		cli_putstr_P(PSTR("\r\n     "));
		cli_putstr_P(PSTR(" mask1="));
		cli_hexdump(&(ctx->km[i][0]),4);
		cli_putstr_P(PSTR(" mask2="));
		cli_hexdump(&(ctx->km[i][1]),4);
		cli_putstr_P(PSTR(" mask3="));
		cli_hexdump(&(ctx->km[i][2]),4);
		cli_putstr_P(PSTR(" mask4="));
		cli_hexdump(&(ctx->km[i][3]),4);
		cli_putstr_P(PSTR("\r\n;-----"));
	}
}
*/

#define CR 19
#define CM 0x5A827999
#define MR 17 
#define MM 0x6ED9EBA1

void cast6_init(const void* key, uint16_t keysize_b, cast6_ctx_t* ctx){
	uint8_t  buffer[32];
	uint8_t  cr=CR, tr[8];
	uint32_t cm=CM, tm[8];
	uint8_t i,j;
	
	memset(buffer, 0, 32);
	memcpy(buffer, key, (keysize_b+7)/8);
	for(i=0; i<32/4; ++i){
		((uint32_t*)buffer)[i] = CHANGE_ENDIAN32(((uint32_t*)buffer)[i]);
	}

	for(i=0; i<24; ++i){
		for(j=0; j<8; ++j){
			tm[j] = cm;
			cm += MM;
			tr[j] = cr&0x1F;
			cr += MR;
		}
		w(buffer, tr, tm);

		if(i&1){
			j=i/2;
			ctx->km[j][0]=H;
			ctx->km[j][1]=F;
			ctx->km[j][2]=D;
			ctx->km[j][3]=B;
			set_kr(buffer[0*4],j*4+0,ctx);
			set_kr(buffer[2*4],j*4+1,ctx);
			set_kr(buffer[4*4],j*4+2,ctx);
			set_kr(buffer[6*4],j*4+3,ctx);
		}
	}
}



