/**
 * @file
 * @brief Provides the API to access the cryptographic engine through the cryptoDMA
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */

#ifndef CRYPTODMA_INTERNAL_H
#define CRYPTODMA_INTERNAL_H
#include "sx_dma.h"
#include "cryptolib_types.h"

/**
* @brief Select which core the DMA will use. To set in descriptor ::dma_sg_descr_s.tag.
*/
enum dma_sg_EngineSelect_e
{
   DMA_SG_ENGINESELECT_BYPASS = 0x00, /**< direct bypass from input to output */
   DMA_SG_ENGINESELECT_BA411E = 0x01, /**< data flow through BA411E AES */
   DMA_SG_ENGINESELECT_BA412  = 0x02, /**< data flow through BA412 DES */
   DMA_SG_ENGINESELECT_BA413  = 0x03, /**< data flow through BA413 Hash */
   DMA_SG_ENGINESELECT_BA417  = 0x04, /**< data flow through BA417 ChaChaPoly */
   DMA_SG_ENGINESELECT_BA418  = 0x05, /**< data flow through BA418 SHA3	*/

   DMA_SG_ENGINESELECT_BA419  = 0x0B, /**< data flow through BA419 SM4 */
   DMA_SG_ENGINESELECT_BA424  = 0x0F, /**< data flow through BA424 ARIA */


   DMA_SG_ENGINESELECT_MASK   = 0xF   /**< Mask on the engine */
};

/** @brief value for ::dma_sg_descr_s.tag or ::dma_sg_regs32_t.fetch_tag, to direct data to parameters */
#define DMA_SG_TAG_ISCONFIG 0x00000010
/** @brief value for ::dma_sg_descr_s.tag or ::dma_sg_regs32_t.fetch_tag, to direct data to processing */
#define DMA_SG_TAG_ISDATA 0x00000000
/** @brief value for ::dma_sg_descr_s.tag or ::dma_sg_regs32_t.fetch_tag,specifying data as last */
#define DMA_SG_TAG_ISLAST  0x00000020

/** @brief macro to set the offset in the configuration for ::dma_sg_descr_s.tag or ::dma_sg_regs32_t.fetch_tag */
#define DMA_SG_TAG_SETCFGOFFSET(a) ((((a)&0xFF)<<8))

/** @brief value for ::dma_sg_descr_s.tag or ::dma_sg_regs32_t.fetch_tag, specifying data type to message */
#define DMA_SG_TAG_DATATYPE_HASHMSG     0x00000000
/** @brief value for ::dma_sg_descr_s.tag or ::dma_sg_regs32_t.fetch_tag, specifying data type to initialization state */
#define DMA_SG_TAG_DATATYPE_HASHINIT    0x00000040
/** @brief value for ::dma_sg_descr_s.tag or ::dma_sg_regs32_t.fetch_tag, specifying data type to HMAC key */
#define DMA_SG_TAG_DATATYPE_HASHKEY     0x00000080
/** @brief value for ::dma_sg_descr_s.tag or ::dma_sg_regs32_t.fetch_tag, specifying data type payload (will be encrypted/decrypted and authenticated) */
#define DMA_SG_TAG_DATATYPE_BLK_CIPHER_PAYLOAD    0x00000000
/** @brief value for ::dma_sg_descr_s.tag or ::dma_sg_regs32_t.fetch_tag, specifying data type header (will only be authenticated, not encrypted/decrypted) */
#define DMA_SG_TAG_DATATYPE_BLK_CIPHER_HEADER     0x00000040

#define DMA_SG_TAG_PADDING_MASK  0x1F
#define DMA_SG_TAG_PADDING_OFFSET   8
 /** @brief macro to set the amount of invalid bytes in for ::dma_sg_descr_s.tag or ::dma_sg_regs_t.fetch_tag */
#define DMA_SG_TAG_SETINVALIDBYTES(a) ((((a) & DMA_SG_TAG_PADDING_MASK) \
         << DMA_SG_TAG_PADDING_OFFSET))

/**
 * @brief Configure fetch and push operations in scatter-gather mode on internal DMA
 * @param first_fetch_descriptor physical address of the first fetcher descriptor to be configured
 * @param first_push_descriptor physical address of the first pusher descriptor to be configured
 */
void cryptodma_config_sg(
      struct dma_sg_descr_s * first_fetch_descriptor,
      struct dma_sg_descr_s * first_push_descriptor);

/**
 * @brief Configure fetch and push operations in direct mode on internal DMA
 * @param src block_t to the source data to transfer
 * @param dest block_t to the destination location
 * @param length the length in bytes to transfer (from src to dest)
 */
void cryptodma_config_direct(block_t dest, block_t src, uint32_t length);

/**
 * @brief Check cryptodma status
 * Trigger a hardfault if any error occured
 */
void cryptodma_check_status(void) ;

/** @brief Reset the internal DMA */
void cryptodma_reset(void);

/** @brief Start internal DMA transfer */
void cryptodma_start(void);

/** @brief Wait until internal DMA is done */
void cryptodma_wait(void);

/**
 * @brief Check cryptodma error flag
 * @return CRYPTOLIB_DMA_ERR if fifo's are not empty, CRYPTOLIB_SUCCESS otherwise
 */
uint32_t cryptodma_check_bus_error(void);

/**
 * @brief Issues an internal DMA transfer command in indirect mode
 *
 * It configures the internal DMA to issue a data transfer in indirect mode.
 * After that, it waits for the completion (interrupt or polling) and in case
 * of errors on the bus will trigger an hard fault.
 *
 * @param first_fetch_descriptor list of descriptors to fetch from
 * @param first_push_descriptor  list of descriptors to push to
 */
void cryptodma_run_sg(struct dma_sg_descr_s * first_fetch_descriptor, struct dma_sg_descr_s * first_push_descriptor);


/**
 * @brief Map software descriptors and buffers to the hardware
 *
 * \warning \c unmap_descriptors should be called to uninitialize after transfer.
 *
 * @param first_fetch_descriptor DMA input descriptors list
 * @param first_push_descriptor DMA output descriptors list
 * @param mapped_in Pointer to the address of the mapped input descriptors list
 * @param mapped_out Pointer to the address of the mapped output descriptors list
 */
void map_descriptors(struct dma_sg_descr_s *first_fetch_descriptor,
   struct dma_sg_descr_s *first_push_descriptor,
   struct dma_sg_descr_s **mapped_in,
   struct dma_sg_descr_s **mapped_out);

/** @brief Unmap descriptors and buffers to the hardware
 * @param out_descs Output DMA descriptors list
 */
void unmap_descriptors(struct dma_sg_descr_s *out_descs);

/**
 * @brief Write a descriptor and return the next updated address
 *
 * Fill the descriptor (even in case of null length) with the address to
 * fetch/push data, the amount of bytes to fetch/push, the additional flags
 * required by the DMA (like fetch in a fifo mode or discard data) and the tag
 * which selects the crypto engine and extra flags for this specific
 * crypto engine.
 *
 * @param descr  pointer to a descriptor to fill with others parameters
 * @param addr   the address where data will be fetched/pushed
 * @param length amount of bytes to fetch/push
 * @param flags  the extra flags describing if data are coming read like
 *               from a fifo (::DMA_AXI_DESCR_CONST_ADDR), if it is needed
 *               to realign on the width of the DMA bus
 *               (::DMA_AXI_DESCR_REALIGN) or if data can be discarded
 *               (::DMA_AXI_DESCR_DISCARD)
 * @param tag    contains the engine from/to fetch/push data
 *               (see ::dma_sg_EngineSelect_e), indicates
 *               if descriptor contains data or configuration is the last
 *               and specific additional information per crypto-engine.
 *
 * @return the address of the next descriptor available (it supposes a large
 *         enough array of descriptors is passed as first parameter).
 */
struct dma_sg_descr_s* write_desc_always(
      struct dma_sg_descr_s *descr,
      volatile void *addr,
      const uint32_t length,
      const uint32_t flags,
      const uint32_t tag);

 /**
 * @brief Write a descriptor and returns the next updated address only if required
 *
 * Works like ::write_desc_always but start first by checking if there is any
 * data to fetch/push (length > 0). If not, does not fill the descriptor
 *
 * @param descr  pointer to a descriptor to fill with others parameters
 * @param addr   the address where data will be fetched/pushed
 * @param length amount of bytes to fetch/push
 * @param flags  the extra flags like realign/const address, discard
 * @param tag    contains crypto engines, is last or not, config or data, ...
 *
 * @return the address of the next descriptor available (it supposes a large
 *         enough array of descriptors is passed as first parameter). It could
 *         be the same as ::descr
 */
static inline struct dma_sg_descr_s* write_desc(
      struct dma_sg_descr_s *descr,
      volatile void *addr,
      const uint32_t length,
      const uint32_t flags,
      const uint32_t tag)
{
   if (length)
      return write_desc_always(descr, addr, length, flags, tag);
   return descr;
}


/**
 * @brief Write a descriptor and returns the next updated address
 *
 * Works like ::write_desc_always but dedicated for block_t
 *
 * @param descr  pointer to a descriptor to fill with others parameters
 * @param blk    a ::block_t containing an address, a length and flags
 * @param flags  the extra flags like realign or discard
 * @param tag    contains crypto engines, is last or not, config or data, ...
 *
 * @return the address of the next descriptor available
 */
static inline struct dma_sg_descr_s *write_desc_always_blk(
      struct dma_sg_descr_s *descr,
      const block_t *blk,
      const uint32_t flags,
      const uint32_t tag)
{
   return write_desc_always(
         descr,
         blk->addr,
         blk->len,
         blk->flags | flags,
         tag);
}

/**
 * @brief Write a descriptor and returns the next updated address
 *
 * Works like ::write_desc but dedicated for block_t
 *
 * @param descr  pointer to a descriptor to fill with others parameters
 * @param blk    a ::block_t containing an address, a length and flags
 * @param flags  the extra flags like realign or discard
 * @param tag    contains crypto engines, is last or not, config or data, ...
 *
 * @return the address of the next descriptor available (could be the same is ::descr)
 */
static inline struct dma_sg_descr_s *write_desc_blk(
      struct dma_sg_descr_s *descr,
      const block_t *blk,
      const uint32_t flags,
      const uint32_t tag)
{
   return write_desc(
         descr,
         blk->addr,
         blk->len,
         blk->flags | flags,
         tag);
}

/**
 * @brief Mark input descriptor as needing to be realigned by the DMA
 * @param d address of descriptor
 */
void realign_desc(struct dma_sg_descr_s * d);

/**
 * @brief Mark input descriptor as last of a list of descriptors
 * @param d address of last descriptor
 */
void set_last_desc(struct dma_sg_descr_s * d);

/**
 * @brief Update the last data descriptor with the extra invalid bytes.
 *
 * For the currently supported engines in software (AES, ChaCha, SHA(1-2-3)),
 * the corresponding HW field holds invalid bytes, meaning padding after
 * the last data (see the CryptoMaster Datasheet, Table 13).
 *
 * This function ensures that the invalid bytes are set on a descriptor \d
 * already marked as ::DMA_SG_TAG_ISLAST, ::DMA_SG_TAG_ISDATA and also
 * verifies that the selected engine is currently supported before updating
 * the descriptor.
 *
 * @param d       the descriptor to update
 * @param n_bytes the extra padding to append ( < 32, 5 bits in HW).
 */
void set_desc_invalid_bytes(
      struct dma_sg_descr_s *d,
      const uint32_t n_bytes);


#endif
