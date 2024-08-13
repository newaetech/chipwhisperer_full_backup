/**
 * @file
 * @brief This file provides an interface to the BA414EP module
 *
 * A generic high level PK operation will require to:
 *   - Select the operation to issue, with a chosen size of element, endianness
 *     thanks to ::ba414ep_set_command
 *   - If allowed, select the memory locations thanks to ::ba414ep_set_config
 *   - If required (by ECC), load curve parameter thanks to ::ba414ep_load_curve
 *   - Move required inputs in the PK memory thanks to ::mem2CryptoRAM family
 *   - Start the PK computation with ::ba414ep_start_wait_status()
 *   - Read back the computed value if expected thanks to ::CryptoRAM2mem
 *
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#ifndef BA414EP_CONFIG_H
#define BA414EP_CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include "compiler_extentions.h"
#include "cryptolib_def.h"
#include "cryptolib_types.h"
#include "sx_errors.h"
#if PK_CM_ENABLED
#include "sx_rng.h"
#endif


/** @struct BA414EPRegs_t
 *  @brief This structure represents the accessible registers of a BA414EP entity.
 *  See BA414EP's datasheet for details.
 */
typedef struct BA414EPRegs_t
{
   volatile uint32_t PointerReg;              /* BASE_ADDR + 0x0000 */
   volatile uint32_t CommandReg;              /* BASE_ADDR + 0x0004 */
   volatile uint32_t ControlReg;              /* BASE_ADDR + 0x0008 */
   volatile uint32_t StatusReg;               /* BASE_ADDR + 0x000C */
   volatile uint32_t VersionReg;              /* BASE_ADDR + 0x0010 */
   volatile uint32_t TimerReg;                /* BASE_ADDR + 0x0014 */
   volatile uint32_t HwConfigReg;             /* BASE_ADDR + 0x0018 */
} BA414EPRegs_t;

/** @struct BA414EP_ucode_t
 *  @brief This structure represents the accessible microcode of a BA414EP entity.
 *  See BA414EP's datasheet for details.
 */
typedef union BA414EP_ucode_t
{
   volatile uint16_t   content[4096];
   struct ucode_info {
      volatile uint16_t   pointers[128];
      volatile uint16_t   ucodesize;
   } info;
} BA414EP_ucode_t;

#define BA414EP_UCODE_MAX_LENGTH 4094  // ucode content: memory size (4096) - redundancy value size (2)


struct cryptolib_status forward_error_to_cryptosoc(uint16_t ba414ep_error);

/**
 * @brief This function configures the BA414EP memory locations to use in the
 *        primitives arithmetic operations
 * @param PtrA is the memory location of operand A
 *        (from ::BA414EP_MEMLOC_0 to ::BA414EP_MEMLOC_15)
 * @param PtrB is the memory location of operand B
 *        (from ::BA414EP_MEMLOC_0 to ::BA414EP_MEMLOC_15)
 * @param PtrC is the memory location of operand C
 *        (from ::BA414EP_MEMLOC_0 to ::BA414EP_MEMLOC_15)
 * @param PtrN is the memory location of operand N
 *        (from ::BA414EP_MEMLOC_0 to ::BA414EP_MEMLOC_15)
 */
void ba414ep_set_config(uint32_t PtrA, uint32_t PtrB, uint32_t PtrC, uint32_t PtrN);

/**
 * @brief This function configure the BA414EP to issue one PK operation.
 *
 * @param op          The PK operation to issue. Should be one of \c BA414EP_OPTYPE_* family
 * @param operandsize the operands size in bytes (up to 8KB + 64B in RSA + counter-measures)
 * @param swap        swap the bytes on AHB/AXI4 interface (endianness conversion).
 *                    Should be ::BA414EP_BIGEND or ::BA414EP_LITTLEEND
 * @param curve_flags enable accelerator for specific curve modulus
 */
void ba414ep_set_command(uint32_t op, uint32_t operandsize, uint32_t swap, uint32_t curve_flags);

/**
 * @brief Transfer data from memory to the end of a CryptoRAM location.
 *        Should be used when big-endianness is considered.
 *        Data from src are padded from src.len to size with zeros.
 * @param src is a data block of the source that has to be copied (byte-addressing)
 * @param size is the size of the data to copy expressed in bytes
 * @param offset is the offset of the memory location that has to be copied
 *        (from ::BA414EP_MEMLOC_0 to ::BA414EP_MEMLOC_15)
 */
void mem2CryptoRAM_rev(block_t src, uint32_t size, uint32_t offset);

/**
 * @brief Transfer data from the end of a CryptoRAM location to memory.
 * Should be used when big-endianness is considered.
 * @param dst is a block_t where the source has to be copied to (byte-addressing)
 * @param size is the size of the data to copy expressed in bytes
 * @param offset is the offset of the memory location that has to be copied
 *        (from ::BA414EP_MEMLOC_0 to ::BA414EP_MEMLOC_15)
 */
void CryptoRAM2mem_rev(block_t dst, uint32_t size, uint32_t offset);

/**
 * @brief Transfer data from memory to CryptoRAM starting at the beginning of
 * the CryptoRAM location. Should be used when little endianness is
 * considered.
 * @param src is a data block of the source that has to be copied (byte-addressing)
 * @param size is a the size of the data to copy expressed in bytes
 * @param offset is the offset of the memory location that has to be copied
 *        (from ::BA414EP_MEMLOC_0 to ::BA414EP_MEMLOC_15)
 */
void mem2CryptoRAM(block_t src, uint32_t size, uint32_t offset);

/**
 * @brief Function transferring data from CryptoRAM to memory starting from the
 * beginning of the CryptoRAM location. Should be used when little endianness is
 * considered.
 * @param dst is a block_t where the source has to be copied to (byte-addressing)
 * @param size is the size of the data to copy expressed in bytes
 * @param offset is the offset of the memory location that has to be copied
 *        (from ::BA414EP_MEMLOC_0 to ::BA414EP_MEMLOC_15)
 */
void CryptoRAM2mem(block_t dst, uint32_t size, uint32_t offset);

/**
 * @brief Function is used to start the PK, wait for completion,
 *        read and return the status register value
 * @return the contents of the status register as uint32_t.
 */
uint32_t ba414ep_start_wait_status(void) CHECK_RESULT;

/**
 * Copy the BA414EP ECC curve parameters to CryptoRAM
 * @param curve      ecc curve to copy to the cryptoRAM (See ::sx_ecc_curve_t.params)
 * @param size       Size of an element of the curve expressed in bytes
 * @param byte_swap  1 if byte_swap enabled in BA414EP (for AHB/AXI4 endianness), 0 otherwise
 * @param gen        1 if we want to load the generator from the curve parameters
 */
void ba414ep_load_curve(block_t curve, uint32_t size, uint32_t byte_swap, uint32_t gen);


/**
 * @brief Compute \p out = \p in mod N.
 *
 * The modulo will be copied back in \p out but also let in \p outloc memory location
 * for further PK operations.
 *
 * \warning N must be already loaded in \p nloc
 * @param outloc      memory location to place \p out
 * @param nloc        memory location for modulo N
 * @param size        size of \p in expressed in bytes
 * @param in          input value
 * @param out         block_t containing the computed modulo
 * @param curve_flags enable accelerator for specific curve modulus
  @return CRYPTOLIB_SUCCESS if successful
 */
uint32_t ba414ep_load_and_modN(
      uint8_t outloc,
      uint8_t nloc,
      uint32_t size,
      block_t in,
      block_t out,
      uint32_t curve_flags) CHECK_RESULT;

#if PK_CM_ENABLED
/**
 * Set the random number generator to be used for the PK countermeasures.
 * @param rng a structure containing a pointer to a function to generate
          the mandatory random for the countermeasures.
 */
void ba414ep_set_rng(struct sx_rng rng);
#endif

/**
 * Copy an ECC point (X and Y coordinates) to CryptoRAM at two consecutive
 * locations. The last \p size bytes from each of the two CryptoRAM locations
 * will be written. Should be used when big-endianness is considered.
 *
 * @param src point on the curve: X coordinate followed by Y coordinate,
 *            each having  \p size bytes
 * @param size size of the operand/coordinate in bytes
 * @param offset position of the first coordinate in cryptoRAM where the
 *               X coordinate will be copied; the Y coordinate will be
 *               written at location offset + 1
 *        (from ::BA414EP_MEMLOC_0 to ::BA414EP_MEMLOC_14)
 */
void point2CryptoRAM_rev(block_t src, uint32_t size, uint32_t offset);

/**
 * Copy ECC point (X and Y coordinates) from two consecutive locations in the
 * CryptoRAM. The last \p size bytes from each of the two memory locations will
 * be transferred. Should be used when big-endianness is considered.
 *
 * @param dst location where the X coordinate followed by the Y coordinate will
 *            be stored
 * @param size size of the operand/coordinate in bytes
 * @param offset position of the first coordinate in cryptoRAM where the X
 *               coordinate is stored; the Y coordinate will be copied from
 *               the location offset + 1
 *        (from ::BA414EP_MEMLOC_0 to ::BA414EP_MEMLOC_14)
 */
void CryptoRAM2point_rev(block_t dst, uint32_t size, uint32_t offset);

/**
 * Copy an ECC point (X and Y coordinates) to two consecutive CryptoRAM
 * locations. The first \p size bytes of each location will be written. Should
 * be used when little-endianness is considered.
 *
 * @param src point on the curve: X coordinate followed by Y coordinate,
 *            each having  \p size bytes
 * @param size size of the operand/coordinate in bytes
 * @param offset position of the first coordinate in cryptoRAM where the
 *               X coordinate will be copied; the Y coordinate will be
 *               written at location offset + 1
 *        (from ::BA414EP_MEMLOC_0 to ::BA414EP_MEMLOC_14)
 */
void point2CryptoRAM(block_t src, uint32_t size, uint32_t offset);

/**
 * Copy ECC point (X and Y coordinates) from two consecutive locations in the
 * CryptoRAM. The first \p size bytes of the two memory locations will be
 * transferred. Should be used when little-endianness is considered.
 *
 * @param dst location where the X coordinate followed by the Y coordinate will
 *            be stored
 * @param size size of the operand/coordinate in bytes
 * @param offset position of the first coordinate in cryptoRAM where the X
 *               coordinate is stored; the Y coordinate will be copied from
 *               the location offset + 1
 *        (from ::BA414EP_MEMLOC_0 to ::BA414EP_MEMLOC_14)
 */
void CryptoRAM2point(block_t dst, uint32_t size, uint32_t offset);


/* For register CFG */
#define BA414EP_PTR_OPPTRN_LSB  (24)
#define BA414EP_PTR_OPPTRN_MASK (0x0F000000)
#define BA414EP_PTR_OPPTRC_LSB  (16)
#define BA414EP_PTR_OPPTRC_MASK (0x000F0000)
#define BA414EP_PTR_OPPTRB_LSB  (8)
#define BA414EP_PTR_OPPTRB_MASK (0x00000F00)
#define BA414EP_PTR_OPPTRA_LSB  (0)
#define BA414EP_PTR_OPPTRA_MASK (0x0000000F)

/* For register CMD */
#define BA414EP_CMD_CALCR2_LSB      (31)
#define BA414EP_CMD_FLAGB_LSB       (30)
#define BA414EP_CMD_FLAGA_LSB       (29)
#define BA414EP_CMD_SWAP_LSB        (28)
#define BA414EP_CMD_EDWARDS_LSB     (26)
#define BA414EP_CMD_RANDPR_LSB      (25)
#define BA414EP_CMD_RANDKE_LSB      (24)
#define BA414EP_CMD_SELCUR_LSB      (20)
#define BA414EP_CMD_OPSIZE_LSB      (8)
#define BA414EP_CMD_OPFLD_LSB       (7)
#define BA414EP_CMD_OPTYPE_LSB      (0)
#define BA414EP_CMD_CALCR2_MASK     (0x80000000)
#define BA414EP_CMD_FLAGB_MASK      (0x40000000)
#define BA414EP_CMD_FLAGA_MASK      (0x20000000)
#define BA414EP_CMD_SWAP_MASK       (0x10000000)
#define BA414EP_CMD_EDWARDS_MASK    (0x04000000)
#define BA414EP_CMD_RANDPR_MASK     (0x02000000)
#define BA414EP_CMD_RANDKE_MASK     (0x01000000)
#define BA414EP_CMD_SELCUR_MASK     (0x00F00000)
#define BA414EP_CMD_OPSIZE_MASK     (0x0001FF00)
#define BA414EP_CMD_OPFLD_MASK      (0x00000080)
#define BA414EP_CMD_OPTYPE_MASK     (0x0000007F)

#define BA414EP_CMD_CALCR2(v)  (((v) << BA414EP_CMD_CALCR2_LSB)& BA414EP_CMD_CALCR2_MASK)
#define BA414EP_CMD_FLAGB(v)   (((v) <<  BA414EP_CMD_FLAGB_LSB)&  BA414EP_CMD_FLAGB_MASK)
#define BA414EP_CMD_FLAGA(v)   (((v) <<  BA414EP_CMD_FLAGA_LSB)&  BA414EP_CMD_FLAGA_MASK)
#define BA414EP_CMD_SWAP(v)    (((v) <<   BA414EP_CMD_SWAP_LSB)&   BA414EP_CMD_SWAP_MASK)
#define BA414EP_CMD_EDWARDS(v) (((v) << BA414EP_CMD_EDWARDS_LSB)&BA414EP_CMD_EDWARDS_MASK)
#define BA414EP_CMD_RANDPR(v)  (((v) << BA414EP_CMD_RANDPR_LSB)& BA414EP_CMD_RANDPR_MASK)
#define BA414EP_CMD_RANDKE(v)  (((v) << BA414EP_CMD_RANDKE_LSB)& BA414EP_CMD_RANDKE_MASK)
#define BA414EP_CMD_OPSIZE(v)  (((v) << BA414EP_CMD_OPSIZE_LSB)& BA414EP_CMD_OPSIZE_MASK)
#define BA414EP_CMD_OPFLD(v)   (((v) <<  BA414EP_CMD_OPFLD_LSB)&  BA414EP_CMD_OPFLD_MASK)
#define BA414EP_CMD_OPTYPE(v)  (((v) << BA414EP_CMD_OPTYPE_LSB)& BA414EP_CMD_OPTYPE_MASK)


/* For operation type */
/*--- Primitive arithmetic operations GF(p) and GF(2^m) ---*/
/** @brief BA414EP operation type: modular addition */
#define BA414EP_OPTYPE_MOD_ADD                 (0x01)      /**< A+B mod N */
/** @brief BA414EP operation type: modular subtraction */
#define BA414EP_OPTYPE_MOD_SUB                 (0x02)      /**< A-B mod N */
/** @brief BA414EP operation type: modular multiplication (odd N) */
#define BA414EP_OPTYPE_MOD_MULT_ODD            (0x03)      /**< A*B mod N */
/** @brief BA414EP operation type: modular reduction (odd N) */
#define BA414EP_OPTYPE_MOD_RED_ODD             (0x04)      /**< B mod N */
/** @brief BA414EP operation type: modular division (odd N) */
#define BA414EP_OPTYPE_MOD_DIV_ODD             (0x05)      /**< A/B mod N */
/** @brief BA414EP operation type: modular inversion (odd N) */
#define BA414EP_OPTYPE_MOD_INV_ODD             (0x06)      /**< 1/B mod N */
/** @brief BA414EP operation type: multiplication */
#define BA414EP_OPTYPE_MULT                    (0x08)      /**< A*B */
/** @brief BA414EP operation type: modular inversion (even N and GF(p) only) */
#define BA414EP_OPTYPE_MOD_INV_EVEN            (0x09)      /**< 1/B mod N */
/** @brief BA414EP operation type: modular reduction (even N and GF(p) only) */
#define BA414EP_OPTYPE_MOD_RED_EVEN            (0x0A)      /**< B mod N */
/** @brief BA414EP operation type: clear data memory */
#define BA414EP_OPTYPE_CLEAR_MEM               (0x0F)

/*--- High-level RSA, CRT & DSA Operations - GF(p) only ---*/
/** @brief BA414EP operation type: modular exponentiation */
#define BA414EP_OPTYPE_MOD_EXP                 (0x10)
/** @brief BA414EP operation type: RSA priv key generation */
#define BA414EP_OPTYPE_RSA_PK_GEN              (0x11)
/** @brief BA414EP operation type: RSA CRT key parameter generation */
#define BA414EP_OPTYPE_RSA_CRT_GEN             (0x12)
/** @brief BA414EP operation type: RSA CRT decryption */
#define BA414EP_OPTYPE_RSA_CRT_DEC             (0x13)
/** @brief BA414EP operation type: RSA encryption */
#define BA414EP_OPTYPE_RSA_ENC                 (0x14)
/** @brief BA414EP operation type: RSA decryption */
#define BA414EP_OPTYPE_RSA_DEC                 (0x15)
/** @brief BA414EP operation type: RSA signature generation */
#define BA414EP_OPTYPE_RSA_SIGN_GEN            (0x16)
/** @brief BA414EP operation type: RSA signature verification */
#define BA414EP_OPTYPE_RSA_SIGN_VERIF          (0x17)
/** @brief BA414EP operation type: DSA key generation */
#define BA414EP_OPTYPE_DSA_KEY_GEN             (0x18)
/** @brief BA414EP operation type: DSA signature generation */
#define BA414EP_OPTYPE_DSA_SIGN_GEN            (0x19)
/** @brief BA414EP operation type: DSA signature verification */
#define BA414EP_OPTYPE_DSA_SIGN_VERIF          (0x1A)
/** @brief BA414EP operation type: SRP server session key */
#define BA414EP_OPTYPE_SRP_SERVER_KEY          (0x1B)
/** @brief BA414EP operation type: SRP client session key */
#define BA414EP_OPTYPE_SRP_CLIENT_KEY          (0x1C)
/** @brief BA414EP operation type: RSA half CRT recombination */
#define BA414EP_OPTYPE_RSA_HALFCRT_RECOMBI     (0x1D)
/** @brief BA414EP operation type: SRP server pub key */
#define BA414EP_OPTYPE_SRP_SERVER_PK           (0x1E)
/** @brief BA414EP operation type: RSA half CRT decryption */
#define BA414EP_OPTYPE_RSA_HALFCRT_DEC         (0x1F)

/*--- Primitive ECC & Check Point Operations GF(p) & GF(2m) ---*/
/** @brief BA414EP operation type: ECC point doubling */
#define BA414EP_OPTYPE_ECC_POINT_DOUBLE        (0x20)
/** @brief BA414EP operation type: ECC point addition */
#define BA414EP_OPTYPE_ECC_POINT_ADD           (0x21)
/** @brief BA414EP operation type: ECC point multiplication */
#define BA414EP_OPTYPE_ECC_POINT_MULT          (0x22)
/** @brief BA414EP operation type: ECC check a & b */
#define BA414EP_OPTYPE_ECC_CHECK_AB            (0x23)
/** @brief BA414EP operation type: ECC check n!=q */
#define BA414EP_OPTYPE_ECC_CHECK_NNOTQ         (0x24)
/** @brief BA414EP operation type: ECC check x<q, y<q */
#define BA414EP_OPTYPE_ECC_CHECK_XY            (0x25)
/** @brief BA414EP operation type: ECC check point on curve */
#define BA414EP_OPTYPE_ECC_CHECK_POINTONCURVE  (0x26)
/** @brief BA414EP operation type: ECC point decompression */
#define BA414EP_OPTYPE_ECC_POINT_DECOMP        (0x27)
/** @brief BA414EP operation type: Montgomery curve point multiplication */
#define BA414EP_OPTYPE_MONTGOMERY_POINT_MULT   (0x28)

/** @brief BA414EP operation type: SM2 signature generation */
#define BA414EP_OPTYPE_SM2_SIGN_GEN            (0x2D)
/** @brief BA414EP operation type: SM2 signature verification */
#define BA414EP_OPTYPE_SM2_SIGN_VERIF          (0x2E)
/** @brief BA414EP operation type: SM2 key exchange */
#define BA414EP_OPTYPE_SM2_SIGN_KEY_EXCHANGE   (0x2F)

/*--- High-level ECC - ECDSA Operations GF(p) & GF(2m) ---*/
/** @brief BA414EP operation type: ECDSA signature generation */
#define BA414EP_OPTYPE_ECDSA_SIGN_GEN          (0x30)
/** @brief BA414EP operation type: ECDSA signature verification */
#define BA414EP_OPTYPE_ECDSA_SIGN_VERIF        (0x31)
/** @brief BA414EP operation type: ECDSA domain parameters validation */
#define BA414EP_OPTYPE_ECDSA_PARAM_EVAL        (0x32)
/** @brief BA414EP operation type: ECKCDSA pub key generation */
#define BA414EP_OPTYPE_ECKCDSA_PK_GEN          (0x33)
/** @brief BA414EP operation type: ECKCDSA signature generation */
#define BA414EP_OPTYPE_ECKCDSA_SIGN_GEN        (0x34)
/** @brief BA414EP operation type: ECKCDSA signature verification */
#define BA414EP_OPTYPE_ECKCDSA_SIGN_VERIF      (0x35)
/** @brief BA414EP operation type: JPAKE ZKP generation*/
#define BA414EP_OPTYPE_JPAKE_ZKP_GEN           (0x36)
/** @brief BA414EP operation type: JPAKE ZKP verification */
#define BA414EP_OPTYPE_JPAKE_ZKP_VERIF         (0x37)
/** @brief BA414EP operation type: JPAKE 3-point addition */
#define BA414EP_OPTYPE_JPAKE_3POINT_ADD        (0x38)
/** @brief BA414EP operation type: JPAKE session key */
#define BA414EP_OPTYPE_JPAKE_SESSION_KEY       (0x39)
/** @brief BA414EP operation type: JPAKE step 2 */
#define BA414EP_OPTYPE_JPAKE_STEP2             (0x3A)
/** @brief BA414EP operation type: EdDSA point multiplication */
#define BA414EP_OPTYPE_EDDSA_POINT_MULT        (0x3B)
/** @brief BA414EP operation type: EdDSA signature generation */
#define BA414EP_OPTYPE_EDDSA_SIGN_GEN          (0x3C)
/** @brief BA414EP operation type: EdDSA signature verification */
#define BA414EP_OPTYPE_EDDSA_SIGN_VERIF        (0x3D)
/** @brief BA414EP operation type: ECMQV session key */
#define BA414EP_OPTYPE_ECMQV_SESSION_KEY       (0x3E)

/*--- Primality Test - Rabin-Miller ---*/
/** @brief BA414EP operation type: Miller-Rabin primality test */
#define BA414EP_OPTYPE_MILLER_RABIN            (0x42)


#if PK_CM_ENABLED
   /** @brief BA414EP modular operations involving a secret exponent */
   #define BA414EP_IS_OP_WITH_SECRET_MOD(op)      ((op == BA414EP_OPTYPE_MOD_EXP)              || \
                                                  (op == BA414EP_OPTYPE_RSA_CRT_DEC)          || \
                                                  (op == BA414EP_OPTYPE_RSA_DEC)              || \
                                                  (op == BA414EP_OPTYPE_RSA_SIGN_GEN)         || \
                                                  (op == BA414EP_OPTYPE_RSA_HALFCRT_RECOMBI)  || \
                                                  (op == BA414EP_OPTYPE_RSA_HALFCRT_DEC)      || \
                                                  (op == BA414EP_OPTYPE_SRP_SERVER_KEY)       || \
                                                  (op == BA414EP_OPTYPE_SRP_CLIENT_KEY)       || \
                                                  (op == BA414EP_OPTYPE_SRP_SERVER_PK)        || \
                                                  (op == BA414EP_OPTYPE_DSA_KEY_GEN)          || \
                                                  (op == BA414EP_OPTYPE_DSA_SIGN_GEN))

   /** @brief BA414EP ECC operations involving point mult */
   #define BA414EP_IS_OP_WITH_SECRET_ECC(op)      ((op == BA414EP_OPTYPE_ECC_POINT_MULT)          || \
                                                  (op == BA414EP_OPTYPE_MONTGOMERY_POINT_MULT)   || \
                                                  (op == BA414EP_OPTYPE_ECDSA_SIGN_GEN)          || \
                                                  (op == BA414EP_OPTYPE_ECKCDSA_PK_GEN)          || \
                                                  (op == BA414EP_OPTYPE_ECKCDSA_SIGN_GEN)        || \
                                                  (op == BA414EP_OPTYPE_JPAKE_ZKP_GEN)           || \
                                                  (op == BA414EP_OPTYPE_JPAKE_ZKP_VERIF)         || \
                                                  (op == BA414EP_OPTYPE_JPAKE_3POINT_ADD)        || \
                                                  (op == BA414EP_OPTYPE_JPAKE_SESSION_KEY)       || \
                                                  (op == BA414EP_OPTYPE_JPAKE_STEP2)             || \
                                                  (op == BA414EP_OPTYPE_ECMQV_SESSION_KEY)       || \
                                                  (op == BA414EP_OPTYPE_EDDSA_POINT_MULT)        || \
                                                  (op == BA414EP_OPTYPE_EDDSA_SIGN_GEN))
#endif

/* For BA414EP accelerator curve */
#define BA414EP_SELCUR_NO_ACCELERATOR   (0<<BA414EP_CMD_SELCUR_LSB)
#define BA414EP_SELCUR_ACCEL_P256       (1<<BA414EP_CMD_SELCUR_LSB)
#define BA414EP_SELCUR_ACCEL_P384       (2<<BA414EP_CMD_SELCUR_LSB)
#define BA414EP_SELCUR_ACCEL_P521       (3<<BA414EP_CMD_SELCUR_LSB)
#define BA414EP_SELCUR_ACCEL_P192       (4<<BA414EP_CMD_SELCUR_LSB)
#define BA414EP_SELCUR_ACCEL_X25519     (5<<BA414EP_CMD_SELCUR_LSB)
#define BA414EP_SELCUR_ACCEL_ED25519    (6<<BA414EP_CMD_SELCUR_LSB)

#define BA414EP_OPFLD_PRIME      (0)
#define BA414EP_OPFLD_BINARY     (1)

/** @brief Indicate to use little endian on AHB/AXI4 interface */
#define BA414EP_LITTLEEND        0

/** @brief Indicate to use big endian on AHB/AXI4 interface */
#define BA414EP_BIGEND           1

/* For register CTL */
#define BA414EP_CTL_CLIRQ_LSB  (1)
#define BA414EP_CTL_CLIRQ_MASK (0x00000002)
#define BA414EP_CTL_START_LSB  (0)
#define BA414EP_CTL_START_MASK (0x00000001)

/* For register STS */
#define BA414EP_STS_IRQ_LSB            (17)
#define BA414EP_STS_IRQ_MASK           (0x00020000)
#define BA414EP_STS_BUSY_LSB           (16)
#define BA414EP_STS_BUSY_MASK          (0x00010000)
#define BA414EP_STS_NOTPRIME_LSB       (12)
#define BA414EP_STS_NOTPRIME_MASK      (0x00001000)
#define BA414EP_STS_NOTINVERTIBLE_LSB  (11)
#define BA414EP_STS_NOTINVERTIBLE_MASK (0x00000800)
#define BA414EP_STS_BADCURVE_LSB       (10)
#define BA414EP_STS_BADCURVE_MASK      (0x00000400)
#define BA414EP_STS_BADSIGNATURE_LSB   (9)
#define BA414EP_STS_BADSIGNATURE_MASK  (0x00000200)
#define BA414EP_STS_NOTIMPL_LSB        (8)
#define BA414EP_STS_NOTIMPL_MASK       (0x00000100)
#define BA414EP_STS_BADMODULUS_LSB     (7)
#define BA414EP_STS_BADMODULUS_MASK    (0x00000080)
#define BA414EP_STS_OUTOFRANGE_LSB     (6)
#define BA414EP_STS_OUTOFRANGE_MASK    (0x00000040)
#define BA414EP_STS_NOTONCURVE_LSB     (4)
#define BA414EP_STS_NOTONCURVE_MASK    (0x00000010)
#define BA414EP_STS_FAILPTR_LSB        (0)
#define BA414EP_STS_FAILPTR_MASK       (0x0000000F)

#define BA414EP_STS_ERROR_MASK         (0x0000FFF0)

/* Hardware configuration register */
#define BA414EP_HW_CFG_DISABLE_CM_LSB                       31
#define BA414EP_HW_CFG_DISABLE_CM_MASK                      (1UL<<BA414EP_HW_CFG_DISABLE_CM_LSB)
#define BA414EP_HW_CFG_SUPPORT_255119_ACCEL_LSB             24
#define BA414EP_HW_CFG_SUPPORT_255119_ACCEL_MASK            (1UL<<BA414EP_HW_CFG_SUPPORT_255119_ACCEL_LSB)
#define BA414EP_HW_CFG_SUPPORT_ECC_P192_ACCEL_LSB           23
#define BA414EP_HW_CFG_SUPPORT_ECC_P192_ACCEL_MASK          (1UL<<BA414EP_HW_CFG_SUPPORT_ECC_P192_ACCEL_LSB)
#define BA414EP_HW_CFG_SUPPORT_ECC_P521_ACCEL_LSB           22
#define BA414EP_HW_CFG_SUPPORT_ECC_P521_ACCEL_MASK          (1UL<<BA414EP_HW_CFG_SUPPORT_ECC_P521_ACCEL_LSB)
#define BA414EP_HW_CFG_SUPPORT_ECC_P384_ACCEL_LSB           21
#define BA414EP_HW_CFG_SUPPORT_ECC_P384_ACCEL_MASK          (1UL<<BA414EP_HW_CFG_SUPPORT_ECC_P384_ACCEL_LSB)
#define BA414EP_HW_CFG_SUPPORT_ECC_P256_ACCEL_LSB           20
#define BA414EP_HW_CFG_SUPPORT_ECC_P256_ACCEL_MASK          (1UL<<BA414EP_HW_CFG_SUPPORT_ECC_P256_ACCEL_LSB)
#define BA414EP_HW_CFG_SUPPORT_ECC_BINARY_LSB               17
#define BA414EP_HW_CFG_SUPPORT_ECC_BINARY_MASK              (1UL<<BA414EP_HW_CFG_SUPPORT_ECC_BINARY_LSB)
#define BA414EP_HW_CFG_SUPPORT_ECC_PRIME_LSB                16
#define BA414EP_HW_CFG_SUPPORT_ECC_PRIME_MASK               (1UL<<BA414EP_HW_CFG_SUPPORT_ECC_PRIME_LSB)
#define BA414EP_HW_CFG_NUM_MULT_LSB                         12
#define BA414EP_HW_CFG_NUM_MULT_MASK                        (0xF<<BA414EP_HW_CFG_NUM_MULT_LSB)
#define BA414EP_HW_CFG_MAX_OPSIZE_LSB                       0
#define BA414EP_HW_CFG_MAX_OPSIZE_MASK                      (0xFFF<<BA414EP_HW_CFG_MAX_OPSIZE_LSB)

extern const volatile uint32_t* ADDR_BA414EP_HW_CFG;
#define BA414EP_HW_CFG (*ADDR_BA414EP_HW_CFG)

// BA414EP memory locations
#define BA414EP_MEMLOC_0    0x0 /**< Used for storing domain or message hash */
#define BA414EP_MEMLOC_1    0x1 /**< Used for storing domain or message hash */
#define BA414EP_MEMLOC_2    0x2 /**< Used for storing domain or message hash */
#define BA414EP_MEMLOC_3    0x3 /**< Used for storing domain or message hash */
#define BA414EP_MEMLOC_4    0x4 /**< Used for storing domain or message hash */
#define BA414EP_MEMLOC_5    0x5 /**< Used for storing domain or message hash */
#define BA414EP_MEMLOC_6    0x6 /**< Used for storing private key */
#define BA414EP_MEMLOC_7    0x7 /**< Used for storing private key */
#define BA414EP_MEMLOC_8    0x8 /**< Used for storing public key */
#define BA414EP_MEMLOC_9    0x9 /**< Used for storing public key */
#define BA414EP_MEMLOC_10   0xA /**< Used for signature */
#define BA414EP_MEMLOC_11   0xB /**< Used for signature */
#define BA414EP_MEMLOC_12   0xC /**< Used for storing hash */
#define BA414EP_MEMLOC_13   0xD /**< Used for hash */
#define BA414EP_MEMLOC_14   0xE /**< Used for other usage */
#define BA414EP_MEMLOC_15   0xF /**< Used for other usage */

#define BA414EP_MEMLOC_SIZE 512

#define BA414EP_ADDR_MEMLOC(loc, size) ((uint8_t*)(size_t)(ADDR_BA414EP_CRYPTORAM + \
         (((loc) + 1) * BA414EP_MEMLOC_SIZE) - (size) ))
#endif
