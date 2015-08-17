/******************************************************************
 * S-BOX for Twister
 *
 *
 *******************************************************************/

#include "config.h"
#include <stdint.h>

#ifndef TWISTER_TABLES_H_
#define TWISTER_TABLES_H_
extern const uint8_t twister_sbox[256];

#ifndef TWISTER_MUL_TABLE
  
/* Twister MDS matrix */
extern const uint8_t twister_mds[8][8];
  
#else

extern const uint8_t twister_mds[8][8];
extern const uint8_t twister_multab[8][256];

#endif

#endif

