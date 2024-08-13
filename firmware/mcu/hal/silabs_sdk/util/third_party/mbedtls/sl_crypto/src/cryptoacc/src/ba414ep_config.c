/**
 * @file
 * @brief This file contains useful functions to configure registers of the BA4414E
 *          PointerReg, CommandReg, ControlReg, StatusReg
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#include "ba414ep_config.h"
#include <string.h>
#include <stdint.h>
#include "sxregs.h"
#include "cryptodma_internal.h"
#include "sx_memcpy.h"
#include "sx_math.h"
#include "sx_rng.h"
#include "sx_errors.h"
#include "sx_hw_cfg.h"

// #define PK_DEBUG_ENABLED

#if PK_CM_ENABLED
struct sx_rng pk_rng = {
      .param= NULL,
      .get_rand_blk = NULL
};

void ba414ep_set_rng(struct sx_rng rng)
{
   pk_rng = rng;
}
#endif
static BA414EPRegs_t * const BA414EP_REGS = (BA414EPRegs_t *)ADDR_BA414EP_REGS;
const volatile uint32_t* ADDR_BA414EP_HW_CFG = &(((BA414EPRegs_t *)ADDR_BA414EP_REGS)->HwConfigReg);

/** @brief This function starts the given PK
 *  @return The start bit of the CommandReg of the given BA414EP struct has been set
 *  to appropriate value.
 */
static void ba414ep_start(void){
   uint32_t DataReg = 0x1<<BA414EP_CTL_START_LSB;
   WR_REG32(&BA414EP_REGS->ControlReg, DataReg );
}


/**
 * @brief Function is used to get the full contents of the BA414EP status register
 * @return the contents oft he status register as uint32_t.
 */
static CHECK_RESULT uint32_t ba414ep_get_status(void)
{
    return RD_REG32(&BA414EP_REGS->StatusReg);
}


/** @brief: Function tells if a given Public Key is Busy or not (checking its status register)
 *  @return 1 when given pub key is busy, 0 otherwise
 */
#if WAIT_PK_WITH_REGISTER_POLLING
static CHECK_RESULT int ba414ep_is_busy(void)
{
   uint32_t status = ba414ep_get_status();
   return ((status >> (BA414EP_STS_BUSY_LSB)) & 0x1);
}
#endif


/**
 * @brief Function is used to wait for an interrupt, and read & return the status register
 * @return the contents of the status register as uint32_t.
 */
static CHECK_RESULT uint32_t ba414ep_wait_status(void)
{
   #if WAIT_PK_WITH_REGISTER_POLLING
      while(ba414ep_is_busy());
   #else
      #error "Interrupt not supported, polling is the only option"
   #endif


   return (ba414ep_get_status() & BA414EP_STS_ERROR_MASK);
}


void ba414ep_set_config(uint32_t PtrA, uint32_t PtrB, uint32_t PtrC, uint32_t PtrN)
{
   uint32_t DataReg = 0x00000000;
   DataReg = ((PtrC<<BA414EP_PTR_OPPTRC_LSB) & BA414EP_PTR_OPPTRC_MASK) |
             ((PtrB<<BA414EP_PTR_OPPTRB_LSB) & BA414EP_PTR_OPPTRB_MASK) |
             ((PtrA<<BA414EP_PTR_OPPTRA_LSB) & BA414EP_PTR_OPPTRA_MASK) |
             ((PtrN<<BA414EP_PTR_OPPTRN_LSB) & BA414EP_PTR_OPPTRN_MASK) ;

   WR_REG32(&BA414EP_REGS->PointerReg, DataReg);
}

void ba414ep_set_command(uint32_t op, uint32_t operandsize, uint32_t swap, uint32_t curve_flags)
{
   uint32_t DataReg = 0x80000000;
   uint32_t NumberOfBytes;

   if (operandsize > 0) {
      NumberOfBytes = operandsize - 1;
   } else {
      NumberOfBytes = 0;
   }

   // Data ram is erased automatically after reset in PK engine.
   // Wait until erasing is finished before writing any data
   // (this routine is called before any data transfer)
   #if WAIT_PK_WITH_REGISTER_POLLING
      while(ba414ep_is_busy());
   #else
      #error "Interrupt not supported, polling is the only option"
   #endif

   DataReg = DataReg | op;
   DataReg = DataReg | (NumberOfBytes  << BA414EP_CMD_OPSIZE_LSB);
   #if PK_CM_ENABLED
      // Counter-Measures for the Public Key
      if (BA414EP_IS_OP_WITH_SECRET_ECC(op)) {
         // ECC operation
         DataReg = DataReg | (BA414EP_CMD_RANDPR(PK_CM_RANDPROJ_ECC));
         DataReg = DataReg | (BA414EP_CMD_RANDKE(PK_CM_RANDKE_ECC));
      } else if (BA414EP_IS_OP_WITH_SECRET_MOD(op)) {
         // Modular operations
         DataReg = DataReg | (BA414EP_CMD_RANDPR(PK_CM_RANDPROJ_MOD));
         DataReg = DataReg | (BA414EP_CMD_RANDKE(PK_CM_RANDKE_MOD));
      }
   #endif
   DataReg = DataReg | (BA414EP_CMD_SWAP_MASK & (swap << BA414EP_CMD_SWAP_LSB));
   DataReg = DataReg | curve_flags;

   #ifdef PK_DEBUG_ENABLED
   CRYPTOLIB_PRINTF("Set command PK %08x\n", DataReg);
   #endif

   WR_REG32(&BA414EP_REGS->CommandReg, DataReg );


   #if PK_CM_ENABLED
   CRYPTOLIB_ASSERT_NM(pk_rng.get_rand_blk != NULL);
   // Copy random value in the CryptoRAM for the counter-measures
   if ((BA414EP_IS_OP_WITH_SECRET_ECC(op) && (PK_CM_RANDKE_ECC || PK_CM_RANDPROJ_ECC)) ||
       (BA414EP_IS_OP_WITH_SECRET_MOD(op) && (PK_CM_RANDKE_MOD || PK_CM_RANDPROJ_MOD))) {
      uint8_t pk_cm_rand[8] = { 0xb5, 0xb5, 0xb5, 0xb5, 0xb5, 0xb5, 0xb5, 0xb5 };
      block_t rnd_blk = block_t_convert(pk_cm_rand, PK_CM_RAND_SIZE);

      // Generate non-null random value
      do {
#if !CRYPTOLIB_TEST_ENABLED
         pk_rng.get_rand_blk(pk_rng.param, rnd_blk);
#endif //!CRYPTOLIB_TEST_ENABLED
      } while(!sx_math_array_is_not_null(rnd_blk.addr, PK_CM_RAND_SIZE));

      // Random has to be odd
      if (BA414EP_IS_OP_WITH_SECRET_MOD(op) && PK_CM_RANDPROJ_MOD) {
         pk_cm_rand[PK_CM_RAND_SIZE-1] |= 1;
      }

      // Copy random to cryptoRAM (pad with 0 if smaller than operandsize)
      if (swap) {
         mem2CryptoRAM_rev(rnd_blk, roundup_32(operandsize), BA414EP_MEMLOC_15);
      } else {
         mem2CryptoRAM(rnd_blk, roundup_32(operandsize), BA414EP_MEMLOC_15);
      }
   }
   #endif
}


void mem2CryptoRAM_rev(block_t src, uint32_t size, uint32_t offset)
{
  if(src.len > size) {
    #ifdef PK_DEBUG_ENABLED
    CRYPTOLIB_PRINTF("mem2CryptoRAM_rev: Src longer (=%d) than size (=%d) (cutting) !\n",
                     src.len, size);
    #endif

    src.len = size;
  }

  if(!src.len || !size){
    #ifdef PK_DEBUG_ENABLED
    CRYPTOLIB_PRINTF("mem2CryptoRAM_rev: Src or size is null (skip) !\n");
    #endif

    return;
  }

  #ifdef PK_DEBUG_ENABLED
  CRYPTOLIB_PRINTF("Debug BA414EP: Write (rev) %d bytes padded to %d to loc %d:\n", src.len, size, offset);
  #endif

   block_t dst;
   dst.flags   = BLOCK_S_INCR_ADDR;
   dst.len     = size;
   dst.addr    = BA414EP_ADDR_MEMLOC(offset, src.len);
   memcpy_blk(dst, src, src.len);

   if(src.len < size) {
      dst.addr = BA414EP_ADDR_MEMLOC(offset, size);
      sx_memset(dst.addr, 0, size - src.len);
   }
   #ifdef PK_DEBUG_ENABLED
   dst.addr = BA414EP_ADDR_MEMLOC(offset, size);
   SX_PRINT_BLK(dst);
   #endif
}

void point2CryptoRAM_rev(block_t src, uint32_t size, uint32_t offset) {
   struct dma_sg_descr_s desc_from;
   struct dma_sg_descr_s desc_to[2];

#ifdef PK_DEBUG_ENABLED
   CRYPTOLIB_PRINTF("Debug BA414EP: DMA transfer point (rev): "
                    "%d bytes to loc %d & %d with operand size = %d\n",
                    src.len, offset, offset+1, size);
#endif
   CRYPTOLIB_ASSERT((src.len >= size*2), "source length less then point size.");

   write_desc(
         &desc_from,
         src.addr,
         (src.flags & DMA_AXI_DESCR_CONST_ADDR ? roundup_32(src.len) : 2 * size), //XXX the source length can be bigger then the transfer size
         src.flags,
         DMA_SG_ENGINESELECT_BYPASS);
   set_last_desc(&desc_from);

   write_desc(
         &desc_to[0],
         BA414EP_ADDR_MEMLOC(offset, size),
         size,
         BLOCK_S_INCR_ADDR,
         DMA_SG_ENGINESELECT_BYPASS);

   write_desc(
         &desc_to[1],
         BA414EP_ADDR_MEMLOC(offset + 1, size),
         size,
         BLOCK_S_INCR_ADDR,
         DMA_SG_ENGINESELECT_BYPASS);
   set_last_desc(&desc_to[1]);

   cryptodma_run_sg(&desc_from, desc_to);
}

void point2CryptoRAM(block_t src, uint32_t size, uint32_t offset) {
   struct dma_sg_descr_s desc_from;
   struct dma_sg_descr_s desc_to[2];

#ifdef PK_DEBUG_ENABLED
   CRYPTOLIB_PRINTF("Debug BA414EP: DMA transfer point: "
                    "%d bytes to loc %d & %d with operand size = %d\n",
                    src.len, offset, offset + 1, size);
#endif
   CRYPTOLIB_ASSERT((src.len >= size*2), "source length less then point size.");

   write_desc(
         &desc_from,
         src.addr,
         (src.flags & DMA_AXI_DESCR_CONST_ADDR ? roundup_32(src.len) : 2 * size), //XXX the source length can be bigger then the transfer size
         src.flags,
         DMA_SG_ENGINESELECT_BYPASS);
   set_last_desc(&desc_from);

   write_desc(
         &desc_to[0],
         BA414EP_ADDR_MEMLOC(offset - 1, 0),
         size,
         BLOCK_S_INCR_ADDR,
         DMA_SG_ENGINESELECT_BYPASS);

   write_desc(
         &desc_to[1],
         BA414EP_ADDR_MEMLOC(offset, 0),
         size,
         BLOCK_S_INCR_ADDR,
         DMA_SG_ENGINESELECT_BYPASS);
   set_last_desc(&desc_to[1]);

   cryptodma_run_sg(&desc_from, desc_to);
}

void CryptoRAM2point_rev(block_t dst, uint32_t size, uint32_t offset) {
   struct dma_sg_descr_s desc_from[2];
   struct dma_sg_descr_s desc_to;

#ifdef PK_DEBUG_ENABLED
   CRYPTOLIB_PRINTF("Debug BA414EP: DMA transfer point (rev): "
                    "%d bytes from loc %d & %d with operand size = %d\n",
                    dst.len, offset, offset+1, size);
#endif
   CRYPTOLIB_ASSERT((dst.len >= size*2), "destination length smaller then point size.");

   write_desc(
         &desc_from[0],
         BA414EP_ADDR_MEMLOC(offset, size),
         size,
         BLOCK_S_INCR_ADDR,
         DMA_SG_ENGINESELECT_BYPASS);

   write_desc(
         &desc_from[1],
         BA414EP_ADDR_MEMLOC(offset + 1, size),
         size,
         BLOCK_S_INCR_ADDR,
         DMA_SG_ENGINESELECT_BYPASS);
   set_last_desc(&desc_from[1]);

   write_desc(
         &desc_to,
         dst.addr,
         (dst.flags & DMA_AXI_DESCR_CONST_ADDR ? roundup_32(dst.len) : 2 * size), //XXX the destination length can be bigger then the transfer size
         dst.flags,
         DMA_SG_ENGINESELECT_BYPASS);
   set_last_desc(&desc_to);

   cryptodma_run_sg(desc_from, &desc_to);
}

void CryptoRAM2point(block_t dst, uint32_t size, uint32_t offset) {
   struct dma_sg_descr_s desc_from[2];
   struct dma_sg_descr_s desc_to;

#ifdef PK_DEBUG_ENABLED
   CRYPTOLIB_PRINTF("Debug BA414EP: DMA transfer point (rev): "
                    "%d bytes from loc %d & %d with operand size = %d\n",
                    dst.len, offset, offset+1, size);
#endif
   CRYPTOLIB_ASSERT((dst.len >= size*2), "destination length smaller then point size.");

   write_desc(
         &desc_from[0],
         BA414EP_ADDR_MEMLOC(offset - 1, 0),
         size,
         BLOCK_S_INCR_ADDR,
         DMA_SG_ENGINESELECT_BYPASS);

   write_desc(
         &desc_from[1],
         BA414EP_ADDR_MEMLOC(offset, 0),
         size,
         BLOCK_S_INCR_ADDR,
         DMA_SG_ENGINESELECT_BYPASS);
   set_last_desc(&desc_from[1]);

   write_desc(
         &desc_to,
         dst.addr,
         (dst.flags & DMA_AXI_DESCR_CONST_ADDR ? roundup_32(dst.len) : 2 * size), //XXX the destination length can be bigger then the transfer size
         dst.flags,
         DMA_SG_ENGINESELECT_BYPASS);
   set_last_desc(&desc_to);

   cryptodma_run_sg(desc_from, &desc_to);
}


void CryptoRAM2mem_rev(block_t dst, uint32_t size, uint32_t offset)
{
   block_t src;

   src.addr    = BA414EP_ADDR_MEMLOC(offset, size);
   src.len     = size;
   src.flags   = BLOCK_S_INCR_ADDR;
   memcpy_blk(dst, src, size);

   #ifdef PK_DEBUG_ENABLED
   CRYPTOLIB_PRINTF("Debug BA414EP: Read %d bytes from loc %d:\n", size, offset);
   SX_PRINT_BLK(src);
   #endif
}

void mem2CryptoRAM(block_t src, uint32_t size, uint32_t offset)
{
   block_t dst;

   dst.addr    = BA414EP_ADDR_MEMLOC(offset - 1, 0);
   dst.len     = size;
   dst.flags   = BLOCK_S_INCR_ADDR;
   memcpy_blk(dst, src, size);

   #ifdef PK_DEBUG_ENABLED
   CRYPTOLIB_PRINTF("Debug BA414EP: Write %d bytes to loc %d:\n", size, offset);
   SX_PRINT_BLK(dst);
   #endif
}

void CryptoRAM2mem(block_t dst, uint32_t size, uint32_t offset)
{
   block_t src;

   src.addr    = BA414EP_ADDR_MEMLOC(offset - 1, 0);
   src.len     = size;
   src.flags   = BLOCK_S_INCR_ADDR;
   memcpy_blk(dst, src, size);

   #ifdef PK_DEBUG_ENABLED
   CRYPTOLIB_PRINTF("Debug BA414EP: Read %d bytes from loc %d:\n", size, offset);
   SX_PRINT_BLK(src);
   #endif
}


uint32_t ba414ep_start_wait_status(void)
{
   uint32_t status;
   ba414ep_start();
   status = ba414ep_wait_status();
   if (status&~(BA414EP_STS_BADSIGNATURE_MASK|BA414EP_STS_NOTPRIME_MASK|BA414EP_STS_NOTINVERTIBLE_MASK)) {
      CRYPTOLIB_PRINTF("BA414EP: error status: %08X\n", status);
   }
   return status;
}


void ba414ep_load_curve(
      block_t curve,
      uint32_t size,
      uint32_t byte_swap,
      uint32_t gen)
{
   uint32_t i;

   block_t param;
   param.addr  = curve.addr;
   param.len   = size;
   param.flags = curve.flags;

   /* Load ECC parameters */
   for (i=0; i * size < curve.len; i++) {
      if (gen || (i != 2 && i != 3)) {
         if (byte_swap)
            mem2CryptoRAM_rev(param, size, i);
         else
            mem2CryptoRAM(param, size, i);
      }

      if (!(param.flags & BLOCK_S_CONST_ADDR))
         param.addr += param.len;
   }
}



uint32_t ba414ep_load_and_modN(uint8_t outloc, uint8_t nloc, uint32_t size, block_t in, block_t out, uint32_t flags)
{
   ba414ep_set_command(BA414EP_OPTYPE_MOD_RED_ODD, size, BA414EP_BIGEND, flags);
   ba414ep_set_config(outloc, outloc, outloc, nloc);
   mem2CryptoRAM_rev(in, size, outloc);
   if(ba414ep_start_wait_status())
      return CRYPTOLIB_CRYPTO_ERR;

   CryptoRAM2mem_rev(out, size, outloc);

   return CRYPTOLIB_SUCCESS;
}

