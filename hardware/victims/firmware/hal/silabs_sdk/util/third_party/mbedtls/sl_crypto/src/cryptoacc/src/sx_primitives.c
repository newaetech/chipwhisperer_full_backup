/**
 * @file
 * @brief Implements the procedures to make primitive operations with
 *          the BA414EP pub key
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#include "sx_primitives.h"
#include <stddef.h>
#include "cryptolib_def.h"
#include "sx_memcpy.h"
#include "ba414ep_config.h"
#include "sx_ecc_curves.h"
#include "sx_errors.h"
#include "string.h"

/* PRIMITIVE OPERATIONS */
static uint32_t primitive_operation(uint32_t op,
                              const uint8_t *a,
                              const uint8_t *b,
                              const uint8_t *modulo,
                              uint8_t *result,
                             uint32_t  size,
                              uint32_t PtrA,
                              uint32_t PtrB,
                              uint32_t PtrN,
                              uint32_t PtrC);


/* Modular Addition: C = A + B mod N */
uint32_t modular_addition(const uint8_t *a,
                          const uint8_t *b,
                          const uint8_t *modulo,
                           uint8_t *result,
                          uint32_t  size)
{

   return primitive_operation((uint8_t) BA414EP_OPTYPE_MOD_ADD , a, b, modulo, result, size, 0xA, 0x8, 0x0, 0x6);
}

/* Modular Subtraction: C = A - B mod N */
uint32_t modular_substraction(const uint8_t *a,
                              const uint8_t *b,
                              const uint8_t *modulo,
                               uint8_t *result,
                              uint32_t  size)
{

   return primitive_operation((uint8_t) BA414EP_OPTYPE_MOD_SUB, a, b, modulo, result, size, 0xA, 0x8, 0x0, 0x6);
}

/* Modular Multiplication: C = A * B mod N */
uint32_t modular_multiplication(const uint8_t *a,
                                const uint8_t *b,
                                const uint8_t *modulo,
                                 uint8_t *result,
                                uint32_t  size)
{

   return primitive_operation((uint8_t) BA414EP_OPTYPE_MOD_MULT_ODD, a, b, modulo, result, size, 0xA, 0x8, 0x0, 0x6);
}

/* Modular Reduction: C = A mod N */
uint32_t modular_reduction(const uint8_t *a,
                           const uint8_t *modulo,
                           uint8_t *result,
                           uint32_t  size)
{

   return primitive_operation((uint8_t) BA414EP_OPTYPE_MOD_RED_ODD, NULL, a, modulo, result, size, 0xA, 0x8, 0x0, 0x6);
}

/* Modular Division: C = A/B mod N */
uint32_t modular_division(const uint8_t *a,
                          const uint8_t *b,
                          const uint8_t *modulo,
                           uint8_t *result,
                          uint32_t  size)
{

   return primitive_operation((uint8_t) BA414EP_OPTYPE_MOD_DIV_ODD, b, a, modulo, result, size, 0xA, 0x8, 0x0, 0x6);
}

/* Modular Inversion: C = 1/A mod N */
uint32_t  modular_inversion(const uint8_t *a,
                            const uint8_t *modulo,
                             uint8_t *result,
                            uint32_t  size)
{

   return primitive_operation((uint8_t) BA414EP_OPTYPE_MOD_INV_ODD , NULL, a, modulo, result, size, 0xA, 0x8, 0x0, 0x6);
}

/* Multiplication: C = A * B */
uint32_t multiplication(const uint8_t *a,
                        const uint8_t *b,
                         uint8_t *result,
                        uint32_t  size)
{
   return primitive_operation((uint8_t) BA414EP_OPTYPE_MULT, a, b, NULL, result, size, 0xA, 0x8, 0x0, 0x6);
}


/* Modular Exponentiation    C = B^A mod N */
uint32_t modular_exponentiation(const uint8_t *a,
                           const uint8_t *b,
                           const uint8_t *modulo,
                           uint8_t *result,
                          uint32_t  size)
{

   return primitive_operation((uint8_t) BA414EP_OPTYPE_MOD_EXP, b, a, modulo, result, size, 0xA, 0x8, 0x0, 0x6);
}


/** @brief Generic function to load the Public Key to compute primitive
 * arithmetic operations. It is reserved for internal use and SHOULD NOT be used
 * via the API as it can corrupt the CryptoRAM.
 *
 *  @param op is the [6:0] bits of the CommandReg register, describing the
 *            operation asked to the Public Key
 *  @param a is a pointer to the first operand of the operation (this must point
 *           to a location in the System Memory, not in the CryptoRAM).
 *  @param b is a pointer to the second operand of the operation (this must
 *           point to a location in the System Memory, not in the CryptoRAM).
 *  @param modulo is a pointer to the modulo of the operation (this must point
 *                to a location in the System Memory, not in the CryptoRAM).
 *  @param result is a pointer where the result has to be copied (this must
 *                point to a location in the System Memory, not in the
 *                CryptoRAM).
 *  @param size represents the operands size in bytes.
 *  @param PtrA is a memory location of the CryptoRAM where the first operand
 *              has to be copied
 *  @param PtrB is a memory location of the CryptoRAM where the second operand
 *              has to be copied
 *  @param PtrC is a memory location of the CryptoRAM from where the result has
 *              to be copied
 *  @param PtrN is a memory location of the CryptoRAM from where the modulo has
 *              to be copied
 *  @return CRYPTOLIB_SUCCESS when no error occurs
 */
static uint32_t primitive_operation(uint32_t op,
                              const uint8_t *a,
                              const uint8_t *b,
                              const uint8_t *modulo,
                              uint8_t *result,
                             uint32_t  size,
                              uint32_t PtrA,
                              uint32_t PtrB,
                              uint32_t PtrN,
                              uint32_t PtrC){
   uint32_t status;

   /* Set command register with operation, operand size, swap and field
    * This is done before transferring data into cryptoRAM because
    * endianness needs to be known before transferring data */
    ba414ep_set_command(op, size, BA414EP_BIGEND, BA414EP_SELCUR_NO_ACCELERATOR);

    if(modulo != NULL) {
      mem2CryptoRAM_rev(block_t_convert(modulo, size), size, PtrN);
   }

   if(a != NULL) {
      mem2CryptoRAM_rev(block_t_convert(a, size), size, PtrA);
   }

   if(b != NULL) {
      mem2CryptoRAM_rev(block_t_convert(b, size), size, PtrB);
   }

   ba414ep_set_config(PtrA, PtrB, PtrC, PtrN);

   /* Start BA414EP */
   status = ba414ep_start_wait_status();
   if (status)
      return CRYPTOLIB_CRYPTO_ERR;

   CryptoRAM2mem_rev(block_t_convert(result, size), size, PtrC);

   return CRYPTOLIB_SUCCESS;
}



/* Modular Exponentiation    C = B^A mod N */
uint32_t modular_exponentiation_blk(block_t a,
                                 block_t b,
                                 block_t modulo,
                                 block_t result,
                                uint32_t  size)
{
   uint32_t status;
   uint32_t size_adapt = size;

   #if PK_CM_ENABLED
      if (PK_CM_RANDPROJ_MOD) {
         size_adapt += PK_CM_RAND_SIZE;
      }
   #endif

   ba414ep_set_command(BA414EP_OPTYPE_MOD_EXP, size_adapt, BA414EP_BIGEND, BA414EP_SELCUR_NO_ACCELERATOR);

   ba414ep_set_config(BA414EP_MEMLOC_10, BA414EP_MEMLOC_11, BA414EP_MEMLOC_12, BA414EP_MEMLOC_0);

   mem2CryptoRAM_rev(modulo, size_adapt, BA414EP_MEMLOC_0);
   mem2CryptoRAM_rev(a, size_adapt, BA414EP_MEMLOC_10);
   mem2CryptoRAM_rev(b, size_adapt, BA414EP_MEMLOC_11);

   /* Start BA414EP */
   status = ba414ep_start_wait_status();
   if (status)
      return CRYPTOLIB_CRYPTO_ERR;

   CryptoRAM2mem_rev(result, size, BA414EP_MEMLOC_12);

   return CRYPTOLIB_SUCCESS;
}



uint32_t ecc_montgomery_mult(block_t curve, uint32_t curve_flags, uint32_t size, block_t scalar, block_t point, block_t result)
{
   uint32_t status;
   uint8_t buffer_sc[ECC_MAX_KEY_SIZE];
   uint8_t buffer_pt[ECC_MAX_KEY_SIZE];

   if (size > ECC_MAX_KEY_SIZE)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   if (curve.len != 3 * size ||
         scalar.len != size ||
         (point.len && point.len != size) ||
         result.len != size)
      return CRYPTOLIB_INVALID_PARAM;

   if(point.addr == NULL_blk.addr)
      point = block_t_convert(curve.addr + 2*size, size);

   /* copy and process keys */
   memcpy_blkIn(buffer_pt, point, point.len);
   memcpy_blkIn(buffer_sc, scalar, scalar.len);
   if(curve.addr == sx_ecc_curve_curve25519.params.addr)
   {
      buffer_pt[size - 1] &= 127;     //decodeUCoordinate
      buffer_sc[0] &= 248;      //decodeScalar25519 (clamp)
      buffer_sc[size - 1] &= 127;     //decodeScalar25519 (clamp)
      buffer_sc[size - 1] |= 64;      //decodeScalar25519 (clamp)
   }
   else if(curve.addr == sx_ecc_curve_curve448.params.addr)
   {
      buffer_sc[0] &= 252;       //decodeScalar448 (clamp)
      buffer_sc[size - 1] |= 128;      //decodeScalar448 (clamp)
   }
   else
   {
      return CRYPTOLIB_INVALID_PARAM;
   }


   /* Set Command register - size 32 bytes - G(p) field */
   ba414ep_set_command(BA414EP_OPTYPE_MONTGOMERY_POINT_MULT, size, BA414EP_LITTLEEND, curve_flags);
   mem2CryptoRAM(block_t_convert(buffer_pt, ECC_MAX_KEY_SIZE), size, BA414EP_MEMLOC_2);
   mem2CryptoRAM(block_t_convert(buffer_sc, ECC_MAX_KEY_SIZE), size, BA414EP_MEMLOC_3);

   /* Load parameters */
   mem2CryptoRAM(curve, size, BA414EP_MEMLOC_0);
   if(!(curve.flags & BLOCK_S_CONST_ADDR)) curve.addr += size;
   mem2CryptoRAM(curve, size, BA414EP_MEMLOC_1);

   /* Set Configuration register */
   ba414ep_set_config(BA414EP_MEMLOC_2, BA414EP_MEMLOC_3, BA414EP_MEMLOC_4, 0x0);

   // Start BA414EP & get status when finished
   status = ba414ep_start_wait_status();
   if (status)
      return CRYPTOLIB_CRYPTO_ERR;

   CryptoRAM2mem(result, size, BA414EP_MEMLOC_4);
   return CRYPTOLIB_SUCCESS;
}

uint32_t ecc_point_inverse(block_t curve, block_t point, block_t result,
      uint32_t size)
{
   uint32_t error;

   if (point.len != size * 2 || result.len != size * 2)
      return CRYPTOLIB_INVALID_PARAM;
   // result = (x, -y)
   // take the opposite value of y coordinate
   error = modular_substraction(curve.addr, point.addr + size, curve.addr, result.addr + size,
      size);
   if (error)
      return CRYPTOLIB_CRYPTO_ERR;
   // copy x coordinate
   memcpy(result.addr, point.addr, size);

   return CRYPTOLIB_SUCCESS;
}

uint32_t ecc_point_decompress(block_t curve, block_t point_x, uint8_t y_bit,
         block_t point_y, uint32_t size, uint32_t curve_flags)
{
   uint32_t error;
   if (y_bit != 0 && y_bit != 1)
      return CRYPTOLIB_INVALID_PARAM;
   curve_flags |= BA414EP_CMD_FLAGA(y_bit);

   if (point_x.len != size || point_y.len != size)
      return CRYPTOLIB_INVALID_PARAM;

   ba414ep_set_command(BA414EP_OPTYPE_ECC_POINT_DECOMP, size, BA414EP_BIGEND, curve_flags);
   ba414ep_set_config(BA414EP_MEMLOC_6, 0x00, BA414EP_MEMLOC_7, 0x00);

   ba414ep_load_curve(curve, size, BA414EP_BIGEND, 1);

   mem2CryptoRAM_rev(point_x, size, BA414EP_MEMLOC_6);

   error = ba414ep_start_wait_status();
   if (error)
      return CRYPTOLIB_CRYPTO_ERR;

   CryptoRAM2mem_rev(point_y, size, BA414EP_MEMLOC_7);

   return CRYPTOLIB_SUCCESS;
}


uint32_t ecc_point_compute_y2(block_t curve, block_t point_x, block_t point_y2,
         uint32_t size)
{
   uint32_t error;

   if (point_x.len != size || point_y2.len != size)
      return CRYPTOLIB_INVALID_PARAM;

   // Y^2 = x^3 + ax + b = (x * x + a) * x + b
   // Y^2 = (x * x)
   error = modular_multiplication(point_x.addr, point_x.addr, curve.addr, point_y2.addr, size);
   if (error)
      return CRYPTOLIB_CRYPTO_ERR;
   // Y^2 = (x * x) + a
   error = modular_addition(point_y2.addr, curve.addr + (4*size), curve.addr, point_y2.addr, size);
   if (error)
      return CRYPTOLIB_CRYPTO_ERR;
   // Y^2 = ((x * x) + a) * x
   error = modular_multiplication(point_y2.addr, point_x.addr, curve.addr, point_y2.addr, size);
   if (error)
      return CRYPTOLIB_CRYPTO_ERR;
   // Y^2 = ((x * x) + a) * x + b
   error = modular_addition(point_y2.addr, curve.addr + (5*size), curve.addr, point_y2.addr, size);
   if (error)
      return CRYPTOLIB_CRYPTO_ERR;

   return CRYPTOLIB_SUCCESS;
}


uint32_t ecc_point_mul(block_t curve, block_t scalar, block_t point,
      block_t result, uint32_t size, uint32_t curve_flags)
{
   uint32_t error;

 if (point.len != size * 2 || result.len != size * 2)
      return CRYPTOLIB_INVALID_PARAM;

   ba414ep_set_command(BA414EP_OPTYPE_ECC_POINT_MULT, size, BA414EP_BIGEND, curve_flags);
   ba414ep_set_config(BA414EP_MEMLOC_12, BA414EP_MEMLOC_14, BA414EP_MEMLOC_6, 0x0);

   ba414ep_load_curve(curve, size, BA414EP_BIGEND, 1);

   mem2CryptoRAM_rev(scalar, scalar.len, BA414EP_MEMLOC_14);

   point2CryptoRAM_rev(point, size, BA414EP_MEMLOC_12);

   error = ba414ep_start_wait_status();
   if (error)
      return CRYPTOLIB_CRYPTO_ERR;

   CryptoRAM2point_rev(result, size, BA414EP_MEMLOC_6);
   return CRYPTOLIB_SUCCESS;
}

uint32_t ecc_point_add(block_t curve, block_t point_a, block_t point_b,
       block_t result, uint32_t size, uint32_t curve_flags)
{
   uint32_t error;

   if (point_a.len != size * 2 || point_b.len != size * 2 || result.len != size * 2)
      return CRYPTOLIB_INVALID_PARAM;

   ba414ep_set_command(BA414EP_OPTYPE_ECC_POINT_ADD, size, BA414EP_BIGEND, curve_flags);
   ba414ep_set_config(BA414EP_MEMLOC_6, BA414EP_MEMLOC_8, BA414EP_MEMLOC_10, 0x0);

    // Load curve parameters
   ba414ep_load_curve(curve, size, BA414EP_BIGEND, 1);

   point2CryptoRAM_rev(point_a, size, BA414EP_MEMLOC_6);
   point2CryptoRAM_rev(point_b, size, BA414EP_MEMLOC_8);

   error = ba414ep_start_wait_status();
   if (error)
      return CRYPTOLIB_CRYPTO_ERR;
    // Fetch the results
   CryptoRAM2point_rev(result, size, BA414EP_MEMLOC_10);
   return CRYPTOLIB_SUCCESS;
}

uint32_t ecc_is_point_on_curve(block_t curve, block_t point, uint32_t size,
       uint32_t curve_flags)
{
   uint32_t error;
   if (point.len != size * 2)
      return CRYPTOLIB_INVALID_PARAM;

   ba414ep_set_command(BA414EP_OPTYPE_ECC_CHECK_POINTONCURVE, size, BA414EP_BIGEND, curve_flags);
   ba414ep_load_curve(curve, size, BA414EP_BIGEND, 1);
   point2CryptoRAM_rev(point, size , BA414EP_MEMLOC_6);

   ba414ep_set_config(BA414EP_MEMLOC_6, 0x00, 0x00, 0x0);
   error = ba414ep_start_wait_status();
   if (error)
      return CRYPTOLIB_INVALID_POINT;
   return CRYPTOLIB_SUCCESS;
}

uint32_t ecc_pt_muladd(block_t curve, block_t scalar_m, block_t point1,
    block_t scalar_n, block_t point2, block_t result, uint32_t size, uint32_t curve_flags)
{
   uint32_t error;

    /* first multiply 'point1' with the 'scalar_m' */
   ba414ep_set_command(BA414EP_OPTYPE_ECC_POINT_MULT, size, BA414EP_BIGEND, curve_flags);
   ba414ep_set_config(BA414EP_MEMLOC_12, BA414EP_MEMLOC_14, BA414EP_MEMLOC_6, 0x0);
   // Load curve parameters
   ba414ep_load_curve(curve, size, BA414EP_BIGEND, 1);
   // Location 14 -> scalar_m
   mem2CryptoRAM_rev(scalar_m, size, BA414EP_MEMLOC_14);
   // Location 12 -> point1 x, Location 13 -> point1 y
   point2CryptoRAM_rev(point1, size, BA414EP_MEMLOC_12);
   // Start BA414EP
   error = ba414ep_start_wait_status();
   if (error)
      return CRYPTOLIB_CRYPTO_ERR;

   /* next multiply 'point2' with the 'scalar_m' */
   // Already done: ba414ep_set_command(BA414EP_OPTYPE_ECC_POINT_MULT, size, BA414EP_BIGEND, curve_flags);
   ba414ep_set_config(BA414EP_MEMLOC_12, BA414EP_MEMLOC_14, BA414EP_MEMLOC_8, 0x0);
   // Location 14 -> scalar_n
   mem2CryptoRAM_rev(scalar_n, size, BA414EP_MEMLOC_14);
   // Location 12 -> point2 x, Location 13 -> point2 y
   point2CryptoRAM_rev(point2, size, BA414EP_MEMLOC_12);
   // Start BA414EP
   error = ba414ep_start_wait_status();
   if (error)
      return CRYPTOLIB_CRYPTO_ERR;

   /* Add the results of the 2 products*/
   ba414ep_set_command(BA414EP_OPTYPE_ECC_POINT_ADD, size, BA414EP_BIGEND, curve_flags);
   ba414ep_set_config(BA414EP_MEMLOC_6, BA414EP_MEMLOC_8, BA414EP_MEMLOC_10, 0x0);
   // Start BA414EP
   error = ba414ep_start_wait_status();
   if (error)
      return CRYPTOLIB_CRYPTO_ERR;

   // Fetch the results
   CryptoRAM2point_rev(result, size, BA414EP_MEMLOC_10);

   return CRYPTOLIB_SUCCESS;
}
