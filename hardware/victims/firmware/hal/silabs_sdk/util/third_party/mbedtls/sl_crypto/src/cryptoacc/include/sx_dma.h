/**
 * @file
 * @brief Describes the DMA registers
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#ifndef SX_DMA_H
#define SX_DMA_H

#include <stdint.h>
#include "cryptolib_def.h"
/**
* @brief Defines for accessing DMA registers
*/
#define ADDR_DMA_FETCH_ADDR     (ADDR_CRYPTOMASTER_REGS + 0x0000) /**< Start address of data block */
#define ADDR_DMA_FETCH_LEN      (ADDR_CRYPTOMASTER_REGS + 0x0008) /**< Length of data block */
#define ADDR_DMA_FETCH_TAG      (ADDR_CRYPTOMASTER_REGS + 0x000C) /**< User tag */
#define ADDR_DMA_PUSH_ADDR      (ADDR_CRYPTOMASTER_REGS + 0x0010) /**< Start address of data block */
#define ADDR_DMA_PUSH_LEN       (ADDR_CRYPTOMASTER_REGS + 0x0018) /**< Length of data block */
#define ADDR_DMA_INT_EN         (ADDR_CRYPTOMASTER_REGS + 0x001C) /**< Interrupt enable */
#define ADDR_DMA_INT_EN_SET     (ADDR_CRYPTOMASTER_REGS + 0x0020) /**< Interrupt enable set */
#define ADDR_DMA_INT_EN_CLR     (ADDR_CRYPTOMASTER_REGS + 0x0024) /**< Interrupt enable clear */
#define ADDR_DMA_INT_STAT_RAW   (ADDR_CRYPTOMASTER_REGS + 0x0028) /**< Interrupt raw status */
#define ADDR_DMA_INT_STAT       (ADDR_CRYPTOMASTER_REGS + 0x002C) /**< Interrupt status */
#define ADDR_DMA_INT_STAT_CLR   (ADDR_CRYPTOMASTER_REGS + 0x0030) /**< Interrupt status clear */
#define ADDR_DMA_CONFIG         (ADDR_CRYPTOMASTER_REGS + 0x0034) /**< Configuration */
#define ADDR_DMA_START          (ADDR_CRYPTOMASTER_REGS + 0x0038) /**< Start fetch & push */
#define ADDR_DMA_STATUS         (ADDR_CRYPTOMASTER_REGS + 0x003C) /**< Status */

/**
* @brief Structure that represent a descriptor for the DMA module (in scatter-gather mode).
*/
struct dma_sg_descr_s {
   volatile void * addr; /**< Address of the first byte of data to be fetched/pushed */
   volatile struct dma_sg_descr_s * next_descr; /**< Pointer to the next valid
                                                     descriptor or indicates
                                                     that the current descriptor
                                                     is the last.
      - \c next_descr[31:2] address of next descriptor, they are ignored on
        last descriptor.\n The 2 LSB are always zero as DMA handles 32 bits word
      - \c next_descr[1] is not used
      - \c next_descr[0] is the stop bit. If high, the DMA will not fetch
      another descriptor, the stop indicates the last descriptor */
   volatile uint32_t length_irq; /**< length and flags associated to a descriptor.
   - \c length_irq[31] is not used
   - \c length_irq[30] allows data discarding. When high, data is fetched
     but not written to the destination
   - \c length_irq[29] allows data realignment on 32 bits when high.\n Padding is discarded
     from output data. \n The realignment \e must be used:
       - with the last descriptor of a list of descriptors
       - to realign different items in a list of descriptors
       .
     The realignment <em> must not </em> be used when a message is split in
     multiple unaligned blocks to concatenate on output.
   - \c length_irq[28] indicates if data are coming from FIFO
     (does not increment address between words) or not.
   - \c length_irq[27-0] is the length of the data expressed in bytes.
   */

   volatile uint32_t tag; /**< Indicates the engine to select.
                            Could be any of ::dma_sg_EngineSelect_e */
};

/** @brief value of ::dma_sg_regs_s.config to disable fecther's scatter/gather mode */
#define DMA_AXI_CONFIGREG_FETCHER_DIRECT           0x00000000

/** @brief value of ::dma_sg_regs_s.config to disable pusher's scatter/gather mode */
#define DMA_AXI_CONFIGREG_PUSHER_DIRECT            0x00000000

/** @brief value of ::dma_sg_regs_s.config to enable fecther's scatter/gather mode */
#define DMA_AXI_CONFIGREG_FETCHER_INDIRECT         0x00000001

/** @brief value of ::dma_sg_regs_s.config to enable pusher's scatter/gather mode */
#define DMA_AXI_CONFIGREG_PUSHER_INDIRECT          0x00000002

/** @brief value of ::dma_sg_regs_s.config to stop pusher at the end of the current block  */
#define DMA_AXI_CONFIGREG_STOP_FETCHER             0x00000004

/** @brief value of ::dma_sg_regs_s.config to stop pusher at the end of the current block  */
#define DMA_AXI_CONFIGREG_STOP_PUSHER              0x00000008

/** @brief value of ::dma_sg_regs_s.config to softreset the processing module  */
#define DMA_AXI_CONFIGREG_SOFTRESET                0x00000010


/** @brief value of ::dma_sg_regs_s.config to enable pusher's scatter/gather mode */
#define DMA_AXI_CONFIGREG_INDIRECT                 0x00000002


/** @brief value of ::dma_sg_regs_s.start to start fetcher */
#define DMA_AXI_STARTREG_FETCH                     0x00000001

/** @brief value of ::dma_sg_regs_s.start to start pusher */
#define DMA_AXI_STARTREG_PUSH                      0x00000002


/** @brief ::dma_sg_regs_s.status mask for fetcher busy bit */
#define DMA_AXI_STATUSREG_MASK_FETCHER_BUSY        0x00000001

/** @brief ::dma_sg_regs_s.status mask for pusher busy bit */
#define DMA_AXI_STATUSREG_MASK_PUSHER_BUSY         0x00000002

/** @brief ::dma_sg_regs_s.status mask for input fifo almost full bit */
#define DMA_AXI_STATUSREG_MASK_FIFOIN_AF           0x00000004

/** @brief ::dma_sg_regs_s.status mask for core module busy bit */
#define DMA_AXI_STATUSREG_MASK_CORE_BUSY           0x00000008

/** @brief ::dma_sg_regs_s.status mask for input fifo not empty bit */
#define DMA_AXI_STATUSREG_MASK_FIFOIN_NOT_EMPTY    0x00000010

/** @brief ::dma_sg_regs_s.status mask for pusher waiting FIFO bit */
#define DMA_AXI_STATUSREG_MASK_PUSHER_WAIT         0x00000020

/** @brief ::dma_sg_regs_s.status mask for soft reset bit */
#define DMA_AXI_STATUSREG_MASK_SOFT_RESET          0x00000040

/** @brief ::dma_sg_regs_s.status mask for number of data in output fifo */
#define DMA_AXI_STATUSREG_MASK_FIFOOUT_NDATA       0xFFFF0000

/** @brief ::dma_sg_regs_s.status right shift for number of data in output fifo */
#define DMA_AXI_STATUSREG_LSB_FIFOOUT_NDATA        16


/** @brief ::dma_sg_regs_s.Rawstatus mask for fetcher error bit */
#define DMA_AXI_RAWSTATREG_MASK_FETCHER_ERROR      0x00000004

/** @brief ::dma_sg_regs_s.Rawstatus mask for pusher error bit */
#define DMA_AXI_RAWSTATREG_MASK_PUSHER_ERROR       0x00000020


/** @brief value of ::dma_sg_regs_s.int_en to enable the fetcher triggered at the end of each block (if enabled in the descriptor - scatter-gather only)
    Must be set via ::dma_sg_regs_s.int_en_set and clear via ::dma_sg_regs_s.int_en_clr
*/
#define DMA_AXI_INTENSETREG_FETCHER_ENDOFBLOCK_EN  0X00000001

/** @brief value of ::dma_sg_regs_s.int_en to enable the fetcher triggered when reaching a block with Stop=1 (or end of direct transfer)
    Must be set via ::dma_sg_regs_s.int_en_set and clear via ::dma_sg_regs_s.int_en_clr
*/
#define DMA_AXI_INTENSETREG_FETCHER_STOPPED_EN     0X00000002

/** @brief value of ::dma_sg_regs_s.int_en to enable the fetcher triggered when an error response is received from AXI
    Must be set via ::dma_sg_regs_s.int_en_set and clear via ::dma_sg_regs_s.int_en_clr
*/
#define DMA_AXI_INTENSETREG_FETCHER_ERROR_EN       0X00000004

/** @brief value of ::dma_sg_regs_s.int_en to enable the pusher triggered at the end of each block (if enabled in the descriptor - scatter-gather only)
    Must be set via ::dma_sg_regs_s.int_en_set and clear via ::dma_sg_regs_s.int_en_clr
*/
#define DMA_AXI_INTENSETREG_PUSHER_ENDBLOCK_EN     0X00000008

/** @brief value of ::dma_sg_regs_s.int_en to enable the pusher triggered when reaching a block with Stop=1 (or end of direct transfer)
    Must be set via ::dma_sg_regs_s.int_en_set and clear via ::dma_sg_regs_s.int_en_clr
*/
#define DMA_AXI_INTENSETREG_PUSHER_STOPPED_EN      0X00000010

/** @brief value of ::dma_sg_regs_s.int_en to enable the pusher triggered when an error response is received from AXI
    Must be set via ::dma_sg_regs_s.int_en_set and clear via ::dma_sg_regs_s.int_en_clr
*/
#define DMA_AXI_INTENSETREG_PUSHER_ERROR_EN        0X00000020

/** @brief value of ::dma_sg_regs_s.int_en to enable all the enable lines
    Must be set via ::dma_sg_regs_s.int_en_set and clear via ::dma_sg_regs_s.int_en_clr
*/
#define DMA_AXI_INTENSETREG_ALL_EN        0X0000003F



/** @brief ::dma_axi_descr_t.length_irq mask for data length */
#define DMA_AXI_DESCR_MASK_LENGTH       0x0FFFFFFF

/** @brief ::dma_axi_descr_t.length_irq right shift for data length */
#define DMA_AXI_DESCR_LSB_LENGTH        0


/** @brief Indicates to the DMA that addressing in constant mode (pointing to a FIFO) */
#define DMA_AXI_DESCR_CONST_ADDR       0x10000000
/** @brief Indicates to the DMA to realign data on 32 bits words */
#define DMA_AXI_DESCR_REALIGN          0x20000000
/** @brief Indicates to the DMA to discard fetched data */
#define DMA_AXI_DESCR_DISCARD          0x40000000
#define DMA_AXI_DESCR_INT_ENABLE       0x80000000
#define DMA_AXI_DESCR_INT_DISABLE      0x00000000

/** @brief Indicates to the DMA to not fetch another descriptor */
#define DMA_AXI_DESCR_NEXT_STOP        ((struct dma_sg_descr_s*)0x00000001)

#endif /* DMA_SG_CONFIG_H */
