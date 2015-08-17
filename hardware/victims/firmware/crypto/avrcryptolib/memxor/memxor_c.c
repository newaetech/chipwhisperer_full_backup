#include <stdint.h>

#include "memxor.h"

void memxor(void* dest, const void* src, uint16_t n){
  while(n--){
    *((uint8_t*)dest) ^= *((uint8_t*)src);
    dest = (uint8_t*)dest +1;
    src  = (uint8_t*)src  +1;
  }
}

