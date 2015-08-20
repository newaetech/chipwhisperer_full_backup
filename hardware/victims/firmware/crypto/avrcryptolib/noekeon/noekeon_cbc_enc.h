#ifndef NOEKEON_CBC_ENC_H_
#define NOEKEON_CBC_ENC_H_

#include <stdint.h>
#include "noekeon.h"

void noekeon_cbc_enc(void* buffer, uint8_t block_cnt, const void* key);

#endif /*NOEKEON_CBC_ENC_H_*/
