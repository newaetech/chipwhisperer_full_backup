/***************************************************************************//**
 * @file
 * @brief BLE-specific cipher implementations optimized for Silicon Labs devices
 *        with a Radio AES peripheral.
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

#include "crypto_ble.h"

#include "em_device.h"

#if defined(RADIOAES_PRESENT)
#include "radioaes.h"

#include <stddef.h>
#include <stdbool.h>

#define AES_BLOCK_BYTES       16U

#define BLE_CCM_NONCE_BYTES   13U
#define BLE_CCM_KEY_BYTES     16U
#define BLE_CCM_B_BYTES       16U
#define BLE_CCM_TAG_BYTES      4U
#define BLE_CCM_VER_BYTES      4U
#define BLE_CCM_B0_FLAGS    0x49U
#define BLE_CCM_AUTH_BLOCKS    1U

#define BLE_RPA_KEY_BYTES     16U
#define BLE_RPA_DATA_BYTES    16U

#define RADIOAES_CONFIG_BYTES  4U

#ifndef RADIOAES_BLE_RPA_MAX_KEYS
#define RADIOAES_BLE_RPA_MAX_KEYS 32
#endif

// Local CCM variables
static const uint32_t aesCcmConfigEncrypt = AES_MODEID_CCM
                                          | AES_MODEID_NO_CX
                                          | AES_MODEID_AES128
                                          | AES_MODEID_ENCRYPT;

static const uint32_t aesCcmConfigDecrypt = AES_MODEID_CCM
                                          | AES_MODEID_NO_CX
                                          | AES_MODEID_AES128
                                          | AES_MODEID_DECRYPT;
static const uint32_t zeros = 0;

// CONST FETCHERS
static const struct radioaesDmaSgDescr ccmDescFetcherTagPadding =
{
  .address       = (uint32_t) &zeros,
  .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
  .lengthAndIrq  = (AES_BLOCK_BYTES - BLE_CCM_TAG_BYTES) | (BLOCK_S_CONST_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
  .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA | DMA_SG_TAG_ISLAST | DMA_SG_TAG_DATATYPE_AESPAYLOAD | DMA_SG_TAG_SETINVALIDBYTES(AES_BLOCK_BYTES - BLE_CCM_TAG_BYTES)
};

// CONST PUSHERS
static const struct radioaesDmaSgDescr ccmDescPusherVerPadding =
{
  .address       = (uint32_t) NULL,
  .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
  .lengthAndIrq  = (AES_BLOCK_BYTES - BLE_CCM_VER_BYTES) | DMA_AXI_DESCR_DISCARD
};

static int aes_ccm_ble(bool                 encrypt,
                       unsigned char       *data,
                       size_t               length,
                       const unsigned char *key,
                       const unsigned char *iv,
                       unsigned char        header,
                       unsigned char       *tag)

{
  volatile uint32_t verFailed;
  volatile uint8_t b0b1[BLE_CCM_B_BYTES * 2] = { 0 };

  size_t dataPadBytes = AES_BLOCK_BYTES - 1 - ((length - 1) % AES_BLOCK_BYTES);

  // fetchers
  struct radioaesDmaSgDescr ccmDescFetcherTag =
  {
    .address       = (uint32_t) tag,
    .nextDescr     = (uint32_t) &ccmDescFetcherTagPadding,
    .lengthAndIrq  = BLE_CCM_TAG_BYTES,
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA | DMA_SG_TAG_DATATYPE_AESPAYLOAD
  };

  struct radioaesDmaSgDescr ccmDescFetcherDataPadding =
  {
    .address       = (uint32_t) &zeros,
    .nextDescr     = (encrypt ? DMA_AXI_DESCR_NEXT_STOP : (uint32_t) &ccmDescFetcherTag),
    .lengthAndIrq  = (uint32_t) dataPadBytes | (BLOCK_S_CONST_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA | DMA_SG_TAG_DATATYPE_AESPAYLOAD | (encrypt ? DMA_SG_TAG_ISLAST : 0) | DMA_SG_TAG_SETINVALIDBYTES(dataPadBytes),
  };

  struct radioaesDmaSgDescr ccmDescFetcherData =
  {
    .address       = (uint32_t) data,
    .nextDescr     = ((dataPadBytes != 0) ? (uint32_t) &ccmDescFetcherDataPadding : (encrypt ? DMA_AXI_DESCR_NEXT_STOP : (uint32_t) &ccmDescFetcherTag)),
    .lengthAndIrq  = length | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA | DMA_SG_TAG_DATATYPE_AESPAYLOAD | (((dataPadBytes == 0) && encrypt) ? DMA_SG_TAG_ISLAST : 0),
  };

  struct radioaesDmaSgDescr ccmDescFetcherB0B1 =
  {
    .address       = (uint32_t) b0b1,
    .nextDescr     = (uint32_t) &ccmDescFetcherData,
    .lengthAndIrq  = BLE_CCM_B_BYTES * 2 | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA | DMA_SG_TAG_DATATYPE_AESHEADER
  };

  struct radioaesDmaSgDescr ccmDescFetcherKey =
  {
    .address       = (uint32_t) key,
    .nextDescr     = (uint32_t) &ccmDescFetcherB0B1,
    .lengthAndIrq  = BLE_CCM_KEY_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_KEY)
  };

  struct radioaesDmaSgDescr ccmDescFetcherConfig =
  {
    .address       = (uint32_t) (encrypt ? &aesCcmConfigEncrypt : &aesCcmConfigDecrypt),
    .nextDescr     = (uint32_t) &ccmDescFetcherKey,
    .lengthAndIrq  = RADIOAES_CONFIG_BYTES,
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_CFG)
  };

  // Pushers
  struct radioaesDmaSgDescr ccmDescPusherVer =
  {
    .address       = (uint32_t) &verFailed,
    .nextDescr     = (uint32_t) &ccmDescPusherVerPadding,
    .lengthAndIrq  = BLE_CCM_VER_BYTES
  };

  struct radioaesDmaSgDescr ccmDescPusherTagPadding =
  {
    .address       = (uint32_t) NULL,
    .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
    .lengthAndIrq  = (AES_BLOCK_BYTES - BLE_CCM_TAG_BYTES) | DMA_AXI_DESCR_DISCARD
  };

  struct radioaesDmaSgDescr ccmDescPusherTag =
  {
    .address       = (uint32_t) tag,
    .nextDescr     = (uint32_t) &ccmDescPusherTagPadding,
    .lengthAndIrq  = BLE_CCM_TAG_BYTES
  };

  struct radioaesDmaSgDescr ccmDescPusherDataPadding =
  {
    .address       = (uint32_t) NULL,
    .nextDescr     = (uint32_t) (encrypt ? &ccmDescPusherTag : &ccmDescPusherVer),
    .lengthAndIrq  = (uint32_t) dataPadBytes | DMA_AXI_DESCR_DISCARD,
  };

  struct radioaesDmaSgDescr ccmDescPusherData =
  {
    .address       = (uint32_t) data,
    .nextDescr     = (uint32_t) ((dataPadBytes != 0) ? &ccmDescPusherDataPadding : (encrypt ? &ccmDescPusherTag : &ccmDescPusherVer)),
    .lengthAndIrq  = (uint32_t) length | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
  };

  struct radioaesDmaSgDescr ccmDescPusherB0B1 =
  {
    .address       = (uint32_t) NULL,
    .nextDescr     = (uint32_t) &ccmDescPusherData,
    .lengthAndIrq  = (BLE_CCM_B_BYTES * 2) | DMA_AXI_DESCR_DISCARD
  };

  // Fill in B0 block according to BLE spec
  b0b1[0] = BLE_CCM_B0_FLAGS;

  for (size_t i = 0; i < BLE_CCM_NONCE_BYTES; i++) {
    b0b1[i + 1] = iv[i];
  }

  b0b1[14] = (uint8_t) length >> 8;
  b0b1[15] = (uint8_t) length;

  // Fill in B1 block according to BLE spec
  for (size_t i = BLE_CCM_B_BYTES; i < BLE_CCM_B_BYTES * 2; i++) {
    b0b1[i] = 0;
  }
  b0b1[BLE_CCM_B_BYTES + 1] = BLE_CCM_AUTH_BLOCKS;
  b0b1[BLE_CCM_B_BYTES + 2] = header;

  radioaes_state_t aes_ctx;
  int aq = radioaes_acquire();
  if (aq > 0) {
    radioaes_saveState(&aes_ctx);
  } else if (aq < 0) {
    return aq;
  }

  RADIOAES->CTRL = AES_CTRL_FETCHERSCATTERGATHER | AES_CTRL_PUSHERSCATTERGATHER;
  RADIOAES->FETCHADDR = (uint32_t) &ccmDescFetcherConfig;
  RADIOAES->PUSHADDR  = (uint32_t) &ccmDescPusherB0B1;

  RADIOAES->CMD = AES_CMD_STARTPUSHER | AES_CMD_STARTFETCHER;
  while (RADIOAES->STATUS & (AES_STATUS_FETCHERBSY | AES_STATUS_PUSHERBSY)) ;

  if (aq > 0) {
    radioaes_restoreState(&aes_ctx);
  }

  aq = radioaes_release();

  // Check MIC
  if (!encrypt && (verFailed != 0)) {
    return MBEDTLS_ERR_CCM_AUTH_FAILED;
  }
  return aq;
}

/*
 * CCM buffer encryption optimized for BLE
 */
int mbedtls_ccm_encrypt_and_tag_ble(unsigned char       *data,
                                    size_t               length,
                                    const unsigned char *key,
                                    const unsigned char *iv,
                                    unsigned char        header,
                                    unsigned char       *tag)
{
  return aes_ccm_ble(true,
                     data,
                     length,
                     key,
                     iv,
                     header,
                     tag);
}

/*
 * CCM buffer authenticated decryption optimized for BLE
 */
int mbedtls_ccm_auth_decrypt_ble(unsigned char       *data,
                                 size_t               length,
                                 const unsigned char *key,
                                 const unsigned char *iv,
                                 unsigned char        header,
                                 unsigned char *tag)
{
  return aes_ccm_ble(false,
                     data,
                     length,
                     key,
                     iv,
                     header,
                     (uint8_t *) tag);
}

/*
 * Process a table of BLE RPA device keys and look for a
 * match against the supplied hash
 */
int mbedtls_process_ble_rpa(const unsigned char   keytable[],
                            uint32_t              keymask,
                            uint32_t              prand,
                            uint32_t              hash)
{
  int block;
  int previousBlock = -1, result = -1;
  static const uint32_t  aesRpaConfig = AES_MODEID_ECB
                                      | AES_MODEID_NO_CX
                                      | AES_MODEID_AES128
                                      | AES_MODEID_ENCRYPT;

  uint32_t rpaDataIn[BLE_RPA_DATA_BYTES / 4] = { 0 };
  volatile uint32_t rpaDataOut[BLE_RPA_DATA_BYTES / 4];
  radioaes_state_t aes_ctx;

  rpaDataIn[3] = __REV(prand);

  struct radioaesDmaSgDescr aesDescPusherData =
  {
    .address       = (uint32_t) rpaDataOut,
    .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISLAST
  };

  struct radioaesDmaSgDescr aesDescFetcherData =
  {
    .address       = (uint32_t) rpaDataIn,
    .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISLAST | DMA_SG_TAG_ISDATA | DMA_SG_TAG_DATATYPE_AESPAYLOAD
  };

  struct radioaesDmaSgDescr aesDescFetcherConfig =
  {
    .address       = (uint32_t) &aesRpaConfig,
    .nextDescr     = (uint32_t) &aesDescFetcherData,
    .lengthAndIrq  = sizeof(aesRpaConfig),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_CFG)
  };

  volatile struct radioaesDmaSgDescr aesDescFetcherKey =
  {
    .address       = (uint32_t) NULL, // Filled out in each round of RPA check
    .nextDescr     = (uint32_t) &aesDescFetcherConfig,
    .lengthAndIrq  = (uint32_t) BLE_RPA_KEY_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_KEY)
  };

  // Start operation
  int aq = radioaes_acquire();
  if (aq > 0) {
    radioaes_saveState(&aes_ctx);
  } else if (aq < 0) {
    return aq;
  }

  RADIOAES->CTRL = AES_CTRL_FETCHERSCATTERGATHER | AES_CTRL_PUSHERSCATTERGATHER;

  // Data output contains hash in the most significant word (WORD3).
  // Descriptors for blocks that are not included in key mask will be skipped.
  for (block = 0; block < RADIOAES_BLE_RPA_MAX_KEYS; block++) {
    if ( keymask & (1U << block) ) {  // Skip masked keys
      // Write key address and start operation
      while (RADIOAES->STATUS & AES_STATUS_FETCHERBSY) ;
      aesDescFetcherKey.address = (uint32_t) &keytable[block * BLE_RPA_KEY_BYTES];
      RADIOAES->FETCHADDR = (uint32_t) &aesDescFetcherKey;

      RADIOAES->CMD = AES_CMD_STARTFETCHER;

      // Wait for pusher from previous round to finish
      while (RADIOAES->STATUS & AES_STATUS_PUSHERBSY) ;
      RADIOAES->PUSHADDR  = (uint32_t) &aesDescPusherData;

      // Check previous results while AES is processing
      if ((previousBlock >= 0) && ((rpaDataOut[3] & 0xFFFFFF00) == __REV(hash)) ) {
        // Make sure AES is finished before returning
        RADIOAES->CMD = AES_CMD_STARTPUSHER;
        result = previousBlock;
        break;
      }

      // Start pusher so it is ready to push results when encryption is done
      RADIOAES->CMD = AES_CMD_STARTPUSHER;
      previousBlock = block;
    }
  }

  // Wait for last data and check it
  while (RADIOAES->STATUS & AES_STATUS_PUSHERBSY) ;

  if (aq > 0) {
    radioaes_restoreState(&aes_ctx);
  }

  radioaes_release();

  if (result >= 0) {
    return result;
  }

  if ((rpaDataOut[3] & 0xFFFFFF00) == __REV(hash) ) {
    return previousBlock;
  }

  // No match
  return -1;
}
#endif //defined(RADIOAES_PRESENT)
