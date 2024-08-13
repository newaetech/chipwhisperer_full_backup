#include "sx_memcmp.h"
#include "sx_memcpy.h"


uint32_t memcmp_time_cst(uint8_t *in1, uint8_t *in2, uint32_t size)
{
  //Don't try to optimise this function for performance, it's time constant for security reasons
   uint32_t diff = 0;
   uint32_t i = 0;
   for(i = 0; i < size; i++) {
      diff |= (*(in1 + i)^(*(in2 + i)));
   }

   return (diff > 0);
}

uint32_t memcmp32_blk_time_cst(block_t in1, block_t in2, uint32_t size)
{
  //Don't try to optimise this function for performance, it's time constant for security reasons
   uint32_t diff = 0;
   uint32_t in1_value, in2_value;
   uint32_t i = 0;
   for(i = 0; i < size/4; i++) {
      if (in1.flags & BLOCK_S_CONST_ADDR) {
         in1_value = *(volatile uint32_t *) in1.addr;
      } else {
         in1_value = *(volatile uint32_t *) (in1.addr + 4*i);
      }
      if (in2.flags & BLOCK_S_CONST_ADDR) {
         in2_value = *(volatile uint32_t *) in2.addr;
      } else {
         in2_value = *(volatile uint32_t *) (in2.addr + 4*i);
      }
      diff |= in1_value ^ in2_value;
   }

   return (diff > 0);
}
