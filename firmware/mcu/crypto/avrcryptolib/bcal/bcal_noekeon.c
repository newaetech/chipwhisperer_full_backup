/* bcal_noekeon.c */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "blockcipher_descriptor.h"
#include "noekeon.h"
#include "keysize_descriptor.h"

const char noekeon_direct_str[]   PROGMEM = "Noekeon-Direct";
const char noekeon_indirect_str[] PROGMEM = "Noekeon-Indirect"; 

const uint8_t noekeon_keysize_desc[] PROGMEM = { KS_TYPE_LIST, 1, KS_INT(128), 
                                                 KS_TYPE_TERMINATOR    };

const bcdesc_t noekeon_direct_desc PROGMEM = {
	BCDESC_TYPE_BLOCKCIPHER,
	BC_ENC_TYPE_1,
	noekeon_direct_str,
	16,
	128,
	{(void_fpt)NULL},
	{(void_fpt)noekeon_enc},
	{(void_fpt)noekeon_dec},
	(bc_free_fpt)NULL,
	noekeon_keysize_desc
};

const bcdesc_t noekeon_indirect_desc PROGMEM = {
	BCDESC_TYPE_BLOCKCIPHER,
	BC_INIT_TYPE_1 | BC_ENC_TYPE_1,
	noekeon_indirect_str,
	16,
	128,
	{(void_fpt)noekeon_init},
	{(void_fpt)noekeon_enc},
	{(void_fpt)noekeon_dec},
	(bc_free_fpt)NULL,
	noekeon_keysize_desc
};


