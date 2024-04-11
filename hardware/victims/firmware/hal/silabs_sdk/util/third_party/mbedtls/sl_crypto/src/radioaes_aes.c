/***************************************************************************//**
 * @file
 * @brief AES block cipher implementation optimized for Silicon Labs devices
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
#include "em_device.h"
#include "em_core.h"

#if defined(RADIOAES_PRESENT)
#include "radioaes.h"
#include "crypto_ble.h"
#include "mbedtls/aes.h"

#include <stddef.h>
#include <stdbool.h>

#define AES_BLOCK_BYTES (16)

int mbedtls_aes_crypt_ecb_radio(bool                   encrypt,
                                const unsigned char   *key,
                                unsigned int           keybits,
                                const unsigned char    input[AES_BLOCK_BYTES],
                                volatile unsigned char output[AES_BLOCK_BYTES])
{
  radioaes_state_t aes_ctx;
  uint32_t aesConfig;

  switch (keybits) {
    case 256:
      aesConfig = AES_MODEID_ECB | AES_MODEID_NO_CX | AES_MODEID_AES256;
      break;
    case 192:
      return MBEDTLS_ERR_AES_FEATURE_UNAVAILABLE;
    case 128:
      aesConfig = AES_MODEID_ECB | AES_MODEID_NO_CX | AES_MODEID_AES128;
      break;
    default:
      return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
  }

  aesConfig |= encrypt ? AES_MODEID_ENCRYPT : AES_MODEID_DECRYPT;

  struct radioaesDmaSgDescr aesDescPusherData =
  {
    .address       = (uint32_t) output,
    .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISLAST
  };

  struct radioaesDmaSgDescr aesDescFetcherData =
  {
    .address       = (uint32_t) input,
    .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISLAST | DMA_SG_TAG_ISDATA | DMA_SG_TAG_DATATYPE_AESPAYLOAD
  };

  struct radioaesDmaSgDescr aesDescFetcherConfig =
  {
    .address       = (uint32_t) &aesConfig,
    .nextDescr     = (uint32_t) &aesDescFetcherData,
    .lengthAndIrq  = sizeof(aesConfig),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_CFG)
  };

  struct radioaesDmaSgDescr aesDescFetcherKey =
  {
    .address       = (uint32_t) key,
    .nextDescr     = (uint32_t) &aesDescFetcherConfig,
    .lengthAndIrq  = (uint32_t) (keybits / 8) | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
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
  RADIOAES->FETCHADDR = (uint32_t) &aesDescFetcherKey;
  RADIOAES->PUSHADDR  = (uint32_t) &aesDescPusherData;

  RADIOAES->CMD = AES_CMD_STARTPUSHER | AES_CMD_STARTFETCHER;
  while (RADIOAES->STATUS & (AES_STATUS_FETCHERBSY | AES_STATUS_PUSHERBSY)) ;

  if (aq > 0) {
    radioaes_restoreState(&aes_ctx);
  }

  return radioaes_release();
}

int mbedtls_aes_crypt_ctr_radio(const unsigned char   *key,
                                unsigned int           keybits,
                                const unsigned char    input[AES_BLOCK_BYTES],
                                const unsigned char    iv_in[AES_BLOCK_BYTES],
                                volatile unsigned char iv_out[AES_BLOCK_BYTES],
                                volatile unsigned char output[AES_BLOCK_BYTES])
{
  radioaes_state_t aes_ctx;
  uint32_t aesConfig;
  static const uint32_t zero = 0;

  switch (keybits) {
    case 256:
      aesConfig = AES_MODEID_CTR | AES_MODEID_CX_LOAD | (((uint32_t)iv_out != 0) ? AES_MODEID_CX_SAVE : 0) | AES_MODEID_AES256;
      break;
    case 192:
      return MBEDTLS_ERR_AES_FEATURE_UNAVAILABLE;
    case 128:
      aesConfig = AES_MODEID_CTR | AES_MODEID_CX_LOAD | (((uint32_t)iv_out != 0) ? AES_MODEID_CX_SAVE : 0) | AES_MODEID_AES128;
      break;
    default:
      return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
  }

  struct radioaesDmaSgDescr aesDescPusherCtx =
  {
    .address       = (uint32_t) iv_out,
    .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISLAST
  };

  struct radioaesDmaSgDescr aesDescPusherData =
  {
    .address       = (uint32_t) output,
    .nextDescr     = (((uint32_t)iv_out != 0) ? (uint32_t) &aesDescPusherCtx : DMA_AXI_DESCR_NEXT_STOP),
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA
  };

  struct radioaesDmaSgDescr aesDescFetcherData =
  {
    .address       = (uint32_t) input,
    .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISLAST | DMA_SG_TAG_ISDATA | DMA_SG_TAG_DATATYPE_AESPAYLOAD
  };

  struct radioaesDmaSgDescr aesDescFetcherNoCtx =
  {
    .address       = (uint32_t) &zero,
    .nextDescr     = (uint32_t) &aesDescFetcherData,
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_CONST_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_IV)
  };

  struct radioaesDmaSgDescr aesDescFetcherCtx =
  {
    .address       = (uint32_t) iv_in,
    .nextDescr     = (uint32_t) &aesDescFetcherData,
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_IV)
  };

  struct radioaesDmaSgDescr aesDescFetcherConfig =
  {
    .address       = (uint32_t) &aesConfig,
    .nextDescr     = (((uint32_t)iv_in != 0) ? (uint32_t) &aesDescFetcherCtx : (uint32_t) &aesDescFetcherNoCtx),
    .lengthAndIrq  = sizeof(aesConfig),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_CFG)
  };

  struct radioaesDmaSgDescr aesDescFetcherKey =
  {
    .address       = (uint32_t) key,
    .nextDescr     = (uint32_t) &aesDescFetcherConfig,
    .lengthAndIrq  = (uint32_t) (keybits / 8) | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
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
  RADIOAES->FETCHADDR = (uint32_t) &aesDescFetcherKey;
  RADIOAES->PUSHADDR  = (uint32_t) &aesDescPusherData;

  RADIOAES->CMD = AES_CMD_STARTPUSHER | AES_CMD_STARTFETCHER;
  while (RADIOAES->STATUS & (AES_STATUS_FETCHERBSY | AES_STATUS_PUSHERBSY)) ;

  if (aq > 0) {
    radioaes_restoreState(&aes_ctx);
  }

  return radioaes_release();
}

int mbedtls_aes_crypt_ctr_ble(const unsigned char   *key,
                              unsigned int           keybits,
                              const unsigned char    input[AES_BLOCK_BYTES],
                              const unsigned char    iv_in[AES_BLOCK_BYTES],
                              volatile unsigned char iv_out[AES_BLOCK_BYTES],
                              volatile unsigned char output[AES_BLOCK_BYTES])
{
  radioaes_state_t aes_ctx;
  uint32_t aesConfig;
  static const uint32_t zero = 0;

  switch (keybits) {
    case 256:
      aesConfig = AES_MODEID_CTR | AES_MODEID_CX_LOAD | (((uint32_t)iv_out != 0) ? AES_MODEID_CX_SAVE : 0) | AES_MODEID_AES256;
      break;
    case 192:
      return MBEDTLS_ERR_AES_FEATURE_UNAVAILABLE;
    case 128:
      aesConfig = AES_MODEID_CTR | AES_MODEID_CX_LOAD | (((uint32_t)iv_out != 0) ? AES_MODEID_CX_SAVE : 0) | AES_MODEID_AES128;
      break;
    default:
      return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
  }

  struct radioaesDmaSgDescr aesDescPusherCtx =
  {
    .address       = (uint32_t) iv_out,
    .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISLAST
  };

  struct radioaesDmaSgDescr aesDescPusherData =
  {
    .address       = (uint32_t) output,
    .nextDescr     = (((uint32_t)iv_out != 0) ? (uint32_t) &aesDescPusherCtx : DMA_AXI_DESCR_NEXT_STOP),
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA
  };

  struct radioaesDmaSgDescr aesDescFetcherData =
  {
    .address       = (uint32_t) input,
    .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISLAST | DMA_SG_TAG_ISDATA | DMA_SG_TAG_DATATYPE_AESPAYLOAD
  };

  struct radioaesDmaSgDescr aesDescFetcherNoCtx =
  {
    .address       = (uint32_t) &zero,
    .nextDescr     = (uint32_t) &aesDescFetcherData,
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_CONST_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_IV)
  };

  struct radioaesDmaSgDescr aesDescFetcherCtx =
  {
    .address       = (uint32_t) iv_in,
    .nextDescr     = (uint32_t) &aesDescFetcherData,
    .lengthAndIrq  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_IV)
  };

  struct radioaesDmaSgDescr aesDescFetcherConfig =
  {
    .address       = (uint32_t) &aesConfig,
    .nextDescr     = (((uint32_t)iv_in != 0) ? (uint32_t) &aesDescFetcherCtx : (uint32_t) &aesDescFetcherNoCtx),
    .lengthAndIrq  = sizeof(aesConfig),
    .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_CFG)
  };

  struct radioaesDmaSgDescr aesDescFetcherKey =
  {
    .address       = (uint32_t) key,
    .nextDescr     = (uint32_t) &aesDescFetcherConfig,
    .lengthAndIrq  = (uint32_t) (keybits / 8) | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
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
  RADIOAES->FETCHADDR = (uint32_t) &aesDescFetcherKey;
  RADIOAES->PUSHADDR  = (uint32_t) &aesDescPusherData;

  RADIOAES->CMD = AES_CMD_STARTPUSHER | AES_CMD_STARTFETCHER;
  while (RADIOAES->STATUS & (AES_STATUS_FETCHERBSY | AES_STATUS_PUSHERBSY)) ;

  if (aq > 0) {
    radioaes_restoreState(&aes_ctx);
  }

  return radioaes_release();
}

#endif //defined(RADIOAES_PRESENT)
