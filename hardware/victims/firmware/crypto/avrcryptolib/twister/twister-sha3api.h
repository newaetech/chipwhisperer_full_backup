/* twister-sha3api.h */

#include "sha3-api.h"
#include <stdint.h>

typedef struct{
	uint16_t  hashbitlen;
	uint8_t   big;
	void*     ctx; /* points either to twister_state_t or twister_big_ctx_t */
	uint8_t   buffer[64];
	uint16_t  buffer_fill_state;
}hashState;

HashReturn Init(hashState *state, int hashbitlen);
HashReturn Update(hashState *state, const BitSequence *data,
                  DataLength databitlen);
HashReturn Final(hashState *state, BitSequence *hashval);
HashReturn Hash(int hashbitlen, const BitSequence *data,
                DataLength databitlen, BitSequence *hashval);

