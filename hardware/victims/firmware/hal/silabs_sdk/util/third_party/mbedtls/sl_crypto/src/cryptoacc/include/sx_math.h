/**
 * @file
 * @brief Mathematic functions
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#ifndef SX_MATH_H
#define SX_MATH_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "cryptolib_types.h"

/**
 * @brief Transform a 64-bit unsigned integer into a byte arrays.
 * @param in input value
 * @param out result destination. 8 bytes will be written.
 * @param endian endianness of data (see ::sx_endianness)
 */
void sx_math_u64_to_u8array(uint64_t in, uint8_t *out, enum sx_endianness endian);

/**
 * @brief Verify if a byte array is different than 0.
 * @param in      input value
 * @param length  length of the array
 * @return 1      if the array is not all zeroes, 0 otherwise
 */
uint32_t sx_math_array_is_not_null(uint8_t *in, uint32_t length);


/**
 * @brief Get bit size of a value in an array of bytes
 * @param a array of bytes
 * @param length  length of the \p a
 * @return number of bits in \p a
 */
uint32_t sx_math_array_nbits(uint8_t *a, const size_t length);

/**
 * @brief a = a + value (value can be positive or negative).
 * Increment (time constant) of an unsigned value stored as byte array a.
 * @param a unsigned integer to increment, stored as a big endian byte array
 * @param length size of \p a
 * @param value  the value to add to \p a
 */
int32_t sx_math_array_incr(uint8_t *a, const size_t length, int8_t value);

/**
 * @brief A = A + B. Add two unsigned values stored as byte arrays.
 * @param a unsigned integer to add, as a big endian byte array. Result will be placed here.
 * @param alen size of \p a.
 * @param b unsigned integer to add, as a big endian byte array.
 * @param blen size of \p b. Must be smaller or equal to \p alen.
 */
void sx_math_array_add(uint8_t *a, const size_t alen,
      const uint8_t *b, const size_t blen);


/**
 * @brief Determine if a \c lhs (representing a big number) is less than \c rhs
 * @param lhs Big number to compare
 * @param rhs Big number against to compare
 * @param len length size of both \c lhs and \c rhs
 * @param endian endianness of data (see ::sx_endianness)
 * @return true if \c lhs is inferior to \c rhs, otherwise false
 *
 * @warning This function is NOT constant time !
 */
bool sx_math_less_than(
      const uint8_t *lhs,
      const uint8_t *rhs,
      const size_t len);

/**
 * @brief Compute the absolute difference of two big numbers: result = abs(a - b)
 * @param a unsigned integer as a big endian byte array
 * @param b unsigned integer as a big endian byte array
 * @param result unsigned integer as a big endian byte array
 * @param size size of \p a, \p b and \p result.
 */
void sx_math_abs_diff(
      const uint8_t *a,
      const uint8_t *b,
      uint8_t *result,
      size_t size);

#endif
