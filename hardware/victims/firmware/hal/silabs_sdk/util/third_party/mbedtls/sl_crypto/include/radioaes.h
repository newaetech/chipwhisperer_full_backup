/**************************************************************************/ /**
 * @file
 * @brief Radio AES
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: APACHE-2.0
 *
 * This software is subject to an open source license and is distributed by
 * Silicon Laboratories Inc. pursuant to the terms of the Apache License,
 * Version 2.0 available at https://www.apache.org/licenses/LICENSE-2.0.
 * Such terms and conditions may be further supplemented by the Silicon Labs
 * Master Software License Agreement (MSLA) available at www.silabs.com and its
 * sections applicable to open source software.
 *
 ******************************************************************************/

#ifndef RADIOAES_H
#define RADIOAES_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <stdlib.h>

#include "em_device.h"

#if defined( RADIOAES_PRESENT )

/***************************************************************************//**
 * \addtogroup sl_radioaes
 * \{
 ******************************************************************************/

/**
 * @brief Structure that represent a descriptor for the DMA module (in scatter-gather mode).
 */
struct radioaesDmaSgDescr {
  volatile uint32_t address;
  volatile uint32_t nextDescr;
  volatile uint32_t lengthAndIrq;
  volatile uint32_t tag;
};

/**
 * @brief Select which IP core the DMA will use. To set in descriptor ::radioaesDmaSgDescr.tag.
 */
enum radioaesDmaEngineSelect {
  DMA_SG_ENGINESELECT_BYPASS = 0x00,  /**< direct bypass from input to output */
  DMA_SG_ENGINESELECT_BA411E = 0x01,  /**< data flow through BA411E AES */
  DMA_SG_ENGINESELECT_BA412  = 0x02,  /**< data flow through BA412 DES */
  DMA_SG_ENGINESELECT_BA413  = 0x03,  /**< data flow through BA413 Hash */
  DMA_SG_ENGINESELECT_BA417  = 0x04   /**< data flow through BA417 ChaChaPoly */
};

/** @brief value for ::radioaesDmaSgDescr.tag or ::dma_sg_regs_s.FetchTagReg, to direct data to parameters */
#define DMA_SG_TAG_ISCONFIG 0x00000010
/** @brief value for ::radioaesDmaSgDescr.tag or ::dma_sg_regs_s.FetchTagReg, to direct data to processing */
#define DMA_SG_TAG_ISDATA 0x00000000
/** @brief value for ::radioaesDmaSgDescr.tag or ::dma_sg_regs_s.FetchTagReg,specifying data as last */
#define DMA_SG_TAG_ISLAST  0x00000020

/** @brief macro to set the offset in the configuration for ::radioaesDmaSgDescr.tag or ::dma_sg_regs_s.FetchTagReg */
#define DMA_SG_TAG_SETCFGOFFSET(a) ((((a) & 0xFF) << 8))

/** @brief value for ::radioaesDmaSgDescr.tag or ::dma_sg_regs_s.FetchTagReg, specifying data type payload (will be encrypted/decrypted and authenticated) */
#define DMA_SG_TAG_DATATYPE_AESPAYLOAD    0x00000000
/** @brief value for ::radioaesDmaSgDescr.tag or ::dma_sg_regs_s.FetchTagReg, specifying data type header (will only be authenticated, not encrypted/decrypted) */
#define DMA_SG_TAG_DATATYPE_AESHEADER     0x00000040

/** @brief macro to set the amount of invalid bytes in for ::radioaesDmaSgDescr.tag or ::dma_sg_regs_s.FetchTagReg */
#define DMA_SG_TAG_SETINVALIDBYTES(a) ((((a) & 0x1F) << 8))

/** @brief ::dma_axi_descr_t.lengthAndIrq mask for data length */
#define DMA_AXI_DESCR_MASK_LENGTH       0x3FFFFFFF

/** @brief ::dma_axi_descr_t.lengthAndIrq right shift for data length */
#define DMA_AXI_DESCR_LSB_LENGTH        0

#define DMA_AXI_DESCR_CONST_ADDR       0x10000000
#define DMA_AXI_DESCR_REALIGN          0x20000000
#define DMA_AXI_DESCR_DISCARD          0x40000000
#define DMA_AXI_DESCR_INT_ENABLE       0x80000000
#define DMA_AXI_DESCR_INT_DISABLE      0x00000000

#define DMA_AXI_DESCR_NEXT_STOP        0x00000001
#define DMA_AXI_DESCR_NEXT_CONTINUE    0x00000000
#define DMA_AXI_DESCR_MASK_NEXT_ADD    0xFFFFFFFC

/** @brief value of ::block_s.flags to discard the data */
#define BLOCK_S_DISCARD_DATA           0x40000000
/** @brief value of ::block_s.flags to realign the data */
#define BLOCK_S_REALIGN_DATA           0x20000000
/** @brief value of ::block_s.flags to set addressing in constant mode (pointing to a FIFO) */
#define BLOCK_S_CONST_ADDR             0x10000000
/** @brief value of ::block_s.flags to set addressing in increment mode (pointing to a buffer) */
#define BLOCK_S_INCR_ADDR              0x00000000
/** @brief mask for ::block_s.flags to only get DMA-related options */
#define BLOCK_S_FLAG_MASK_DMA_PROPS    0x70000000
/** @brief value of ::block_s.flags mask for fetcher location destination */
#define BLOCK_S_MASK_LOC_DEST          0x00FFFFFF

/// Config ///

/** @brief BA411E offset for Configuration word in DMA Scatter-Gather Tag */
#define AES_OFFSET_CFG        0
/** @brief BA411E offset for Configuration word in DMA Scatter-Gather Tag */
#define AES_OFFSET_KEY        8
/** @brief BA411E offset for Configuration word in DMA Scatter-Gather Tag */
#define AES_OFFSET_IV        40
/** @brief BA411E offset for Configuration word in DMA Scatter-Gather Tag */
#define AES_OFFSET_IV2       56
/** @brief BA411E offset for Configuration word in DMA Scatter-Gather Tag */
#define AES_OFFSET_KEY2      72

/** @brief BA411E Mode Register value for ECB mode of operation */
#define AES_MODEID_ECB        0x00000100
/** @brief BA411E Mode Register value for CBC mode of operation */
#define AES_MODEID_CBC        0x00000200
/** @brief BA411E Mode Register value for CTR mode of operation */
#define AES_MODEID_CTR        0x00000400
/** @brief BA411E Mode Register value for CCM mode of operation */
#define AES_MODEID_CCM        0x00002000
/** @brief BA411E Mode Register value for CMAC mode of operation */
#define AES_MODEID_CMA        0x00010000

/** @brief BA411E Mode Register value for AES context saving */
#define AES_MODEID_CX_SAVE    0x00000020
/** @brief BA411E Mode Register value for AES context loading */
#define AES_MODEID_CX_LOAD    0x00000010
/** @brief BA411E Mode Register value for AES no context */
#define AES_MODEID_NO_CX      0x00000000

/** @brief BA411E Mode Register value for AES keysize of 128 bits */
#define AES_MODEID_AES128     0x00000000
/** @brief BA411E Mode Register value for AES keysize of 256 bits */
#define AES_MODEID_AES256     0x00000004
/** @brief BA411E Mode Register value for AES keysize of 192 bits */
#define AES_MODEID_AES192     0x00000008

/** @brief BA411E Mode Register value for encryption mode */
#define AES_MODEID_ENCRYPT    0x00000000
/** @brief BA411E Mode Register value for decryption mode */
#define AES_MODEID_DECRYPT    0x00000001

/** @brief BA411E Size for IV in GCM mode */
#define AES_IV_GCM_SIZE       12
/** @brief BA411E Size for IV in all modes except GCM */
#define AES_IV_SIZE           16
/** @brief BA411E Size for Context in GCM and CCM modes */
#define AES_CTX_xCM_SIZE      32
/** @brief BA411E Size for Context in all modes except GCM and CCM */
#define AES_CTX_SIZE          16

typedef struct {
	uint32_t FETCHADDR; 	/**< Fetcher Address   	*/
#if defined(_AES_FETCHDESCR_MASK)
	uint32_t FETCHDESCR;	/**< Fetcher Descriptor	*/
#endif
	uint32_t FETCHLEN;  	/**< Fetcher Length    	*/
	uint32_t FETCHTAG;  	/**< Fetcher Tag       	*/
	uint32_t PUSHADDR;  	/**< Pusher Address    	*/
#if defined(_AES_PUSHDESCR_MASK)
	uint32_t PUSHDESCR; 	/**< Pusher Descriptor 	*/
#endif
	uint32_t PUSHLEN;   	/**< Pusher Length     	*/
	uint32_t CTRL;      	/**< Control register  	*/
} radioaes_state_t;

/**
 * \brief          Acquire RADIOAES access
 *
 * \return         0 if successful, positive if preempted, negative on error
 */
int radioaes_acquire ( void );

/**
 * \brief          Release RADIOAES access
 *
 * \return         0 if successful, negative on error
 */
int radioaes_release ( void );

/**
 * \brief          Save RADIOAES register state to RAM
 *
 * \param ctx      Context struct to save register state into
 *
 * \return         0 if successful, negative on error
 */
int radioaes_saveState (radioaes_state_t *ctx);

/**
 * \brief          Restore RADIOAES register state from RAM
 *
 * \param ctx      Context struct to restore register state from
 *
 * \return         0 if successful, negative on error
 */
int radioaes_restoreState (radioaes_state_t *ctx);

#ifdef __cplusplus
}
#endif

/** \} (end addtogroup sl_radioaes) */

#endif /* RADIOAES_PRESENT */
#endif /* RADIOAES_H */
