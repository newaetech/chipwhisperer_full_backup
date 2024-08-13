#include <stdbool.h>
#include <string.h>
#include "sx_math.h"
#include "sx_rng.h"
#include "sx_errors.h"
#include "cryptolib_def.h"
/**
 * @brief Random Number Generator Abstraction for CryptoSoC
 * @copyright Copyright (c) 2018 Silex Insight. All Rights reserved
 * @file
 */

/* This function implements one of the methods approved by FIPS 186-4 to
 * generate a random number k with 1 <= k < n.
 *
 * Get a random value rnd of the appropriate length.
 * If rnd > n -2
 *    Try another value
 * Else
 *    k = rnd + 1
 */
uint32_t sx_rng_get_rand_lt_n_blk(block_t dst, block_t n, struct sx_rng rng)
{
   CRYPTOLIB_ASSERT_NM(dst.flags == BLOCK_S_INCR_ADDR);
   CRYPTOLIB_ASSERT_NM(n.flags == BLOCK_S_INCR_ADDR);

   if ((dst.len != n.len) || !(n.addr[n.len - 1] & 0x01)) /* n must be odd */
      return CRYPTOLIB_INVALID_PARAM;

   bool rnd_invalid;

   /* Check what is the most significant bit of n and compute an index of and a
    * mask for the most significant byte that can be used to remove the leading
    * zeros.
    */
   uint32_t index;
   uint8_t msb_mask;
   for (index = 0; !n.addr[index]; index++); /* since n is odd, at minimum the
                                              * least significant byte should be
                                              * different from 0 */
   for (msb_mask = 0xFF; n.addr[index] & msb_mask; msb_mask <<= 1);

   /* Create container for random value from RNG, pointing to the same buffer as
    * dst but referring only to [MSB-1:0] instead of [len-1:0].
    * Force the leading, non-significant bytes to zero.
    */
   memset(dst.addr, 0, index);
   block_t rnd = block_t_convert(dst.addr + index, dst.len - index);

   do {
      /* Get a random value */
      rng.get_rand_blk(rng.param, rnd);

      /* Mask off the leading non-significant bits. Keep only the bits that are
       * relevant according to msb_mask. This is done to speed up the process of
       * finding a proper random value.
       * For example:
       * If the highest byte of n is 0x06, the chance that we get a random with
       * a highest byte <= 0x06 is only 7/256 without masking.
       * With the masking process (msb_mask = 0xF8, ~msb_mask = 0x07) we
       * significantly increase the chances of immediately finding a suitable
       * random.
       */
      dst.addr[index] &= ~msb_mask; /*Note that dst.addr[index] = rnd.addr[0]*/

      /* Check if rnd > n-2 (constant time comparison) */
      bool gt = false;
      bool eq = true;
      bool n_gt_1 = false;

      for (uint32_t i = 0; i < dst.len; i++) {
         uint32_t leftop = dst.addr[i];
         uint32_t rightop = n.addr[i];

         /* We rephrase rnd > n-2 as rnd >= n-1. Since n is odd, n-1 is obtained
          * by masking 1 bit.
          */
         if (i == dst.len - 1) {
            rightop &= 0xFE;
         }

         /* We use a trick to determine whether leftop >= rightop to avoid
          * possible time dependency in the implementations of ">", "<" and "==".
          * If leftop > rightop then (rightop - leftop) will be 0xFFFFFFxx.
          * If leftop <= rightop then (rightop - leftop) will be 0x000000xx.
          * By shifting out the xx, we can determine the relation between left
          * and right.
          *
          * A similar trick is used to determine whether leftop == rightop.
          * If leftop == rightop then (rightop ^ leftop) - 1 will be 0xFFFFFFFF.
          * If leftop != rightop then (rightop ^ leftop) - 1 will be 0x000000xx.
          *
          * By muxing eq with eq, we ensure that eq will be zero from the first
          * different byte onwards.
          * By muxing the leftop >= rightop check with eq, we ensure that it
          * only has an effect when executed on the first most significant byte
          * that is different between the arrays.
          */
         gt |= (bool) ((rightop - leftop) >> 8) & eq;
         eq &= (bool) (((rightop ^ leftop) - 1) >> 8);

         /* We check that the number is greather than 1 (n_gt_1 ) in the loop
          * to avoid to have a big buffer with a value equal to one
          * (ex : 0x000001).
          * Using this loop instead of create another loop for this check
          * allow to not lose time for a check that should very rarely passed.
          */
         n_gt_1 |= (bool)rightop;
      }
      if (n_gt_1 == false)
         return CRYPTOLIB_INVALID_PARAM;

      rnd_invalid = gt | eq;

   } while (rnd_invalid);

   /* Compute k = rnd + 1 (constant time increment) */
   sx_math_array_incr(dst.addr, dst.len, 1);

   return CRYPTOLIB_SUCCESS;
}
