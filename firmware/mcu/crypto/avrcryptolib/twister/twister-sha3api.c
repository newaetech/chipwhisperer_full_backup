/* twister-sha3api.c */

#include "sha3-api.h"
#include "twister-sha3api.h"
#include "twister.h"
#include "twister-small.h"
#include "twister-big.h"
#include <stdlib.h>	
#include <string.h>

#define BLOCKSIZE 512

HashReturn Init(hashState *state, int hashbitlen){
	if(!state)
		return FAIL;
	if(hashbitlen<32 || hashbitlen>512 || hashbitlen%32)
		return BAD_HASHBITLEN;
	
	if (hashbitlen>=32 && hashbitlen<=256){
		state->hashbitlen = hashbitlen;
		state->big = 0;
		state->ctx = malloc(sizeof(twister_state_t));
		twister_small_init(state->ctx, hashbitlen);
		state->buffer_fill_state=0;
		memset(state->buffer, 0, 64);
		return SUCCESS;
	}
	if (hashbitlen>256 && hashbitlen<=512){
		state->hashbitlen = hashbitlen;
		state->big = 1;
		state->ctx = malloc(sizeof(twister_big_ctx_t));
		twister_big_init(state->ctx, hashbitlen);
		state->buffer_fill_state=0;
		memset(state->buffer, 0, 64);
		return SUCCESS;
	}
	return BAD_HASHBITLEN;
}

void insertBitToBuffer(hashState *state, uint8_t bit){
	state->buffer[state->buffer_fill_state/8] |= 
		bit<<(7-(state->buffer_fill_state%8));
	state->buffer_fill_state++;
}


HashReturn Update(hashState *state, const BitSequence *data,
                  DataLength databitlen){
	if(state->buffer_fill_state+databitlen<BLOCKSIZE){
		/* move bits from data to buffer */
		uint16_t i;
		for(i=0; i<databitlen; ++i){
			insertBitToBuffer(state, 1&((data[i/8])>>(7-i%8)));
		}
		return SUCCESS;
	}                 
	if(state->buffer_fill_state==0){
		while(databitlen>=512){
			if(state->big){
				twister_big_nextBlock(state->ctx, data);
			}else{
				twister_small_nextBlock(state->ctx, data);
			}
			data += 64;
			databitlen -= 512;
		}
		if(databitlen!=0)
			Update(state, data, databitlen);
		return SUCCESS;
    }else{
    	DataLength i;
    	for(i=0; i<databitlen; ++i){
    		if(state->buffer_fill_state==512){
    			if(state->big){
    				twister_big_nextBlock(state->ctx, state->buffer);
				}else{
					twister_small_nextBlock(state->ctx, state->buffer);
				}
    			memset(state->buffer, 0, 64);
    			state->buffer_fill_state = 0;
			}
			insertBitToBuffer(state, 1&(data[i/8]>>(7-i%8)));
		}
		return SUCCESS;
	}              
	return FAIL;	
}

HashReturn Final(hashState *state, BitSequence *hashval){
	if(state->big){
		twister_big_lastBlock(state->ctx, state->buffer, state->buffer_fill_state);
		twister_big_ctx2hash(hashval, state->ctx, state->hashbitlen);
		return SUCCESS;
	}else{
		twister_small_lastBlock(state->ctx, state->buffer, state->buffer_fill_state);
		twister_small_ctx2hash(hashval, state->ctx, state->hashbitlen);
		return SUCCESS;
	}
}

HashReturn Hash(int hashbitlen, const BitSequence *data,
                DataLength databitlen, BitSequence *hashval){
	HashReturn ret;
	hashState state;
	ret = Init(&state, hashbitlen);
	if(ret!=SUCCESS)
		return ret;
	ret = Update(&state, data, databitlen);
	if(ret!=SUCCESS)
		return ret;
	return Final(&state, hashval);
}

