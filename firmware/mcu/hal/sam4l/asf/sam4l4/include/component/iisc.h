/**
 * \file
 *
 * \brief Component description for IISC
 *
 * Copyright (c) 2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAM4L_IISC_COMPONENT_
#define _SAM4L_IISC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR IISC */
/* ========================================================================== */
/** \addtogroup SAM4L_IISC Inter-IC Sound (I2S) Controller */
/*@{*/

#define IISC_I7560
#define REV_IISC                    0x100

/* -------- IISC_CR : (IISC Offset: 0x00) ( /W 32) Control Register -------- */
#define IISC_CR_OFFSET              0x00         /**< \brief (IISC_CR offset) Control Register */

#define IISC_CR_RXEN_Pos            0            /**< \brief (IISC_CR) Receive Enable */
#define IISC_CR_RXEN                (0x1u << IISC_CR_RXEN_Pos)
#define   IISC_CR_RXEN_OFF_Val            0x0u   /**< \brief (IISC_CR) No effect */
#define   IISC_CR_RXEN_ON_Val             0x1u   /**< \brief (IISC_CR) Enables Data Receive if RXDIS is not set */
#define IISC_CR_RXEN_OFF            (IISC_CR_RXEN_OFF_Val          << IISC_CR_RXEN_Pos)
#define IISC_CR_RXEN_ON             (IISC_CR_RXEN_ON_Val           << IISC_CR_RXEN_Pos)
#define IISC_CR_RXDIS_Pos           1            /**< \brief (IISC_CR) Receive Disable */
#define IISC_CR_RXDIS               (0x1u << IISC_CR_RXDIS_Pos)
#define   IISC_CR_RXDIS_OFF_Val           0x0u   /**< \brief (IISC_CR) No effect */
#define   IISC_CR_RXDIS_ON_Val            0x1u   /**< \brief (IISC_CR) Disables Data Receive */
#define IISC_CR_RXDIS_OFF           (IISC_CR_RXDIS_OFF_Val         << IISC_CR_RXDIS_Pos)
#define IISC_CR_RXDIS_ON            (IISC_CR_RXDIS_ON_Val          << IISC_CR_RXDIS_Pos)
#define IISC_CR_CKEN_Pos            2            /**< \brief (IISC_CR) Clocks Enable */
#define IISC_CR_CKEN                (0x1u << IISC_CR_CKEN_Pos)
#define   IISC_CR_CKEN_OFF_Val            0x0u   /**< \brief (IISC_CR) No effect */
#define   IISC_CR_CKEN_ON_Val             0x1u   /**< \brief (IISC_CR) Enables clocks if CKDIS is not set */
#define IISC_CR_CKEN_OFF            (IISC_CR_CKEN_OFF_Val          << IISC_CR_CKEN_Pos)
#define IISC_CR_CKEN_ON             (IISC_CR_CKEN_ON_Val           << IISC_CR_CKEN_Pos)
#define IISC_CR_CKDIS_Pos           3            /**< \brief (IISC_CR) Clocks Disable */
#define IISC_CR_CKDIS               (0x1u << IISC_CR_CKDIS_Pos)
#define   IISC_CR_CKDIS_OFF_Val           0x0u   /**< \brief (IISC_CR) No effect */
#define   IISC_CR_CKDIS_ON_Val            0x1u   /**< \brief (IISC_CR) Disables clocks */
#define IISC_CR_CKDIS_OFF           (IISC_CR_CKDIS_OFF_Val         << IISC_CR_CKDIS_Pos)
#define IISC_CR_CKDIS_ON            (IISC_CR_CKDIS_ON_Val          << IISC_CR_CKDIS_Pos)
#define IISC_CR_TXEN_Pos            4            /**< \brief (IISC_CR) Transmit Enable */
#define IISC_CR_TXEN                (0x1u << IISC_CR_TXEN_Pos)
#define   IISC_CR_TXEN_OFF_Val            0x0u   /**< \brief (IISC_CR) No effect */
#define   IISC_CR_TXEN_ON_Val             0x1u   /**< \brief (IISC_CR) Enables Data Transmit if TXDIS is not set */
#define IISC_CR_TXEN_OFF            (IISC_CR_TXEN_OFF_Val          << IISC_CR_TXEN_Pos)
#define IISC_CR_TXEN_ON             (IISC_CR_TXEN_ON_Val           << IISC_CR_TXEN_Pos)
#define IISC_CR_TXDIS_Pos           5            /**< \brief (IISC_CR) Transmit Disable */
#define IISC_CR_TXDIS               (0x1u << IISC_CR_TXDIS_Pos)
#define   IISC_CR_TXDIS_OFF_Val           0x0u   /**< \brief (IISC_CR) No effect */
#define   IISC_CR_TXDIS_ON_Val            0x1u   /**< \brief (IISC_CR) Disables Data Transmit */
#define IISC_CR_TXDIS_OFF           (IISC_CR_TXDIS_OFF_Val         << IISC_CR_TXDIS_Pos)
#define IISC_CR_TXDIS_ON            (IISC_CR_TXDIS_ON_Val          << IISC_CR_TXDIS_Pos)
#define IISC_CR_SWRST_Pos           7            /**< \brief (IISC_CR) Software Reset */
#define IISC_CR_SWRST               (0x1u << IISC_CR_SWRST_Pos)
#define   IISC_CR_SWRST_OFF_Val           0x0u   /**< \brief (IISC_CR) No effect */
#define   IISC_CR_SWRST_ON_Val            0x1u   /**< \brief (IISC_CR) Performs a software reset. Has priority on any other bit in CR */
#define IISC_CR_SWRST_OFF           (IISC_CR_SWRST_OFF_Val         << IISC_CR_SWRST_Pos)
#define IISC_CR_SWRST_ON            (IISC_CR_SWRST_ON_Val          << IISC_CR_SWRST_Pos)
#define IISC_CR_MASK                0x000000BFu  /**< \brief (IISC_CR) MASK Register */

/* -------- IISC_MR : (IISC Offset: 0x04) (R/W 32) Mode Register -------- */
#define IISC_MR_OFFSET              0x04         /**< \brief (IISC_MR offset) Mode Register */
#define IISC_MR_RESETVALUE          0x00000000   /**< \brief (IISC_MR reset_value) Mode Register */

#define IISC_MR_MODE_Pos            0            /**< \brief (IISC_MR) Master/Slave/Controller Mode */
#define IISC_MR_MODE                (0x1u << IISC_MR_MODE_Pos)
#define   IISC_MR_MODE_SLAVE_Val          0x0u   /**< \brief (IISC_MR) Slave mode (only serial data handled, clocks received from external master or controller) */
#define   IISC_MR_MODE_MASTER_Val         0x1u   /**< \brief (IISC_MR) Master mode (clocks generated and output by IISC, serial data handled if CR.RXEN and/or CR.TXEN written to 1) */
#define IISC_MR_MODE_SLAVE          (IISC_MR_MODE_SLAVE_Val        << IISC_MR_MODE_Pos)
#define IISC_MR_MODE_MASTER         (IISC_MR_MODE_MASTER_Val       << IISC_MR_MODE_Pos)
#define IISC_MR_DATALENGTH_Pos      2            /**< \brief (IISC_MR) Data Word Length */
#define IISC_MR_DATALENGTH_Msk      (0x7u << IISC_MR_DATALENGTH_Pos)
#define IISC_MR_DATALENGTH(value)   ((IISC_MR_DATALENGTH_Msk & ((value) << IISC_MR_DATALENGTH_Pos)))
#define   IISC_MR_DATALENGTH_32_Val       0x0u   /**< \brief (IISC_MR) 32 bits */
#define   IISC_MR_DATALENGTH_24_Val       0x1u   /**< \brief (IISC_MR) 24 bits */
#define   IISC_MR_DATALENGTH_20_Val       0x2u   /**< \brief (IISC_MR) 20 bits */
#define   IISC_MR_DATALENGTH_18_Val       0x3u   /**< \brief (IISC_MR) 18 bits */
#define   IISC_MR_DATALENGTH_16_Val       0x4u   /**< \brief (IISC_MR) 16 bits */
#define   IISC_MR_DATALENGTH_16C_Val      0x5u   /**< \brief (IISC_MR) 16 bits compact stereo */
#define   IISC_MR_DATALENGTH_8_Val        0x6u   /**< \brief (IISC_MR) 8 bits */
#define   IISC_MR_DATALENGTH_8C_Val       0x7u   /**< \brief (IISC_MR) 8 bits compact stereo */
#define IISC_MR_DATALENGTH_32       (IISC_MR_DATALENGTH_32_Val     << IISC_MR_DATALENGTH_Pos)
#define IISC_MR_DATALENGTH_24       (IISC_MR_DATALENGTH_24_Val     << IISC_MR_DATALENGTH_Pos)
#define IISC_MR_DATALENGTH_20       (IISC_MR_DATALENGTH_20_Val     << IISC_MR_DATALENGTH_Pos)
#define IISC_MR_DATALENGTH_18       (IISC_MR_DATALENGTH_18_Val     << IISC_MR_DATALENGTH_Pos)
#define IISC_MR_DATALENGTH_16       (IISC_MR_DATALENGTH_16_Val     << IISC_MR_DATALENGTH_Pos)
#define IISC_MR_DATALENGTH_16C      (IISC_MR_DATALENGTH_16C_Val    << IISC_MR_DATALENGTH_Pos)
#define IISC_MR_DATALENGTH_8        (IISC_MR_DATALENGTH_8_Val      << IISC_MR_DATALENGTH_Pos)
#define IISC_MR_DATALENGTH_8C       (IISC_MR_DATALENGTH_8C_Val     << IISC_MR_DATALENGTH_Pos)
#define IISC_MR_RXMONO_Pos          8            /**< \brief (IISC_MR) Receiver Mono */
#define IISC_MR_RXMONO              (0x1u << IISC_MR_RXMONO_Pos)
#define   IISC_MR_RXMONO_STEREO_Val       0x0u   /**< \brief (IISC_MR) Normal mode */
#define   IISC_MR_RXMONO_MONO_Val         0x1u   /**< \brief (IISC_MR) Left channel data is duplicated to right channel */
#define IISC_MR_RXMONO_STEREO       (IISC_MR_RXMONO_STEREO_Val     << IISC_MR_RXMONO_Pos)
#define IISC_MR_RXMONO_MONO         (IISC_MR_RXMONO_MONO_Val       << IISC_MR_RXMONO_Pos)
#define IISC_MR_RXDMA_Pos           9            /**< \brief (IISC_MR) Single or Multiple DMA Channels for Receiver */
#define IISC_MR_RXDMA               (0x1u << IISC_MR_RXDMA_Pos)
#define   IISC_MR_RXDMA_SINGLE_Val        0x0u   /**< \brief (IISC_MR) Single DMA channel */
#define   IISC_MR_RXDMA_MULTIPLE_Val      0x1u   /**< \brief (IISC_MR) One DMA channel per data channel */
#define IISC_MR_RXDMA_SINGLE        (IISC_MR_RXDMA_SINGLE_Val      << IISC_MR_RXDMA_Pos)
#define IISC_MR_RXDMA_MULTIPLE      (IISC_MR_RXDMA_MULTIPLE_Val    << IISC_MR_RXDMA_Pos)
#define IISC_MR_RXLOOP_Pos          10           /**< \brief (IISC_MR) Loop-back Test Mode */
#define IISC_MR_RXLOOP              (0x1u << IISC_MR_RXLOOP_Pos)
#define   IISC_MR_RXLOOP_OFF_Val          0x0u   /**< \brief (IISC_MR) Normal mode */
#define   IISC_MR_RXLOOP_ON_Val           0x1u   /**< \brief (IISC_MR) ISDO internally connected to ISDI */
#define IISC_MR_RXLOOP_OFF          (IISC_MR_RXLOOP_OFF_Val        << IISC_MR_RXLOOP_Pos)
#define IISC_MR_RXLOOP_ON           (IISC_MR_RXLOOP_ON_Val         << IISC_MR_RXLOOP_Pos)
#define IISC_MR_TXMONO_Pos          12           /**< \brief (IISC_MR) Transmitter Mono */
#define IISC_MR_TXMONO              (0x1u << IISC_MR_TXMONO_Pos)
#define   IISC_MR_TXMONO_STEREO_Val       0x0u   /**< \brief (IISC_MR) Normal mode */
#define   IISC_MR_TXMONO_MONO_Val         0x1u   /**< \brief (IISC_MR) Left channel data is duplicated to right channel */
#define IISC_MR_TXMONO_STEREO       (IISC_MR_TXMONO_STEREO_Val     << IISC_MR_TXMONO_Pos)
#define IISC_MR_TXMONO_MONO         (IISC_MR_TXMONO_MONO_Val       << IISC_MR_TXMONO_Pos)
#define IISC_MR_TXDMA_Pos           13           /**< \brief (IISC_MR) Single or Multiple DMA Channels for Transmitter */
#define IISC_MR_TXDMA               (0x1u << IISC_MR_TXDMA_Pos)
#define   IISC_MR_TXDMA_SINGLE_Val        0x0u   /**< \brief (IISC_MR) Single DMA channel */
#define   IISC_MR_TXDMA_MULTIPLE_Val      0x1u   /**< \brief (IISC_MR) One DMA channel per data channel */
#define IISC_MR_TXDMA_SINGLE        (IISC_MR_TXDMA_SINGLE_Val      << IISC_MR_TXDMA_Pos)
#define IISC_MR_TXDMA_MULTIPLE      (IISC_MR_TXDMA_MULTIPLE_Val    << IISC_MR_TXDMA_Pos)
#define IISC_MR_TXSAME_Pos          14           /**< \brief (IISC_MR) Transmit Data when Underrun */
#define IISC_MR_TXSAME              (0x1u << IISC_MR_TXSAME_Pos)
#define   IISC_MR_TXSAME_ZERO_Val         0x0u   /**< \brief (IISC_MR) Zero data transmitted in case of underrun */
#define   IISC_MR_TXSAME_SAME_Val         0x1u   /**< \brief (IISC_MR) Last data transmitted in case of underrun */
#define IISC_MR_TXSAME_ZERO         (IISC_MR_TXSAME_ZERO_Val       << IISC_MR_TXSAME_Pos)
#define IISC_MR_TXSAME_SAME         (IISC_MR_TXSAME_SAME_Val       << IISC_MR_TXSAME_Pos)
#define IISC_MR_IMCKFS_Pos          24           /**< \brief (IISC_MR) Master Clock to fs Ratio */
#define IISC_MR_IMCKFS_Msk          (0x3Fu << IISC_MR_IMCKFS_Pos)
#define IISC_MR_IMCKFS(value)       ((IISC_MR_IMCKFS_Msk & ((value) << IISC_MR_IMCKFS_Pos)))
#define   IISC_MR_IMCKFS_16_Val           0x0u   /**< \brief (IISC_MR) 16 fs */
#define   IISC_MR_IMCKFS_32_Val           0x1u   /**< \brief (IISC_MR) 32 fs */
#define   IISC_MR_IMCKFS_64_Val           0x3u   /**< \brief (IISC_MR) 64 fs */
#define   IISC_MR_IMCKFS_128_Val          0x7u   /**< \brief (IISC_MR) 128 fs */
#define   IISC_MR_IMCKFS_256_Val          0xFu   /**< \brief (IISC_MR) 256 fs */
#define   IISC_MR_IMCKFS_384_Val          0x17u   /**< \brief (IISC_MR) 384 fs */
#define   IISC_MR_IMCKFS_512_Val          0x1Fu   /**< \brief (IISC_MR) 512 fs */
#define   IISC_MR_IMCKFS_768_Val          0x2Fu   /**< \brief (IISC_MR) 768 fs */
#define   IISC_MR_IMCKFS_1024_Val         0x3Fu   /**< \brief (IISC_MR) 1024 fs */
#define IISC_MR_IMCKFS_16           (IISC_MR_IMCKFS_16_Val         << IISC_MR_IMCKFS_Pos)
#define IISC_MR_IMCKFS_32           (IISC_MR_IMCKFS_32_Val         << IISC_MR_IMCKFS_Pos)
#define IISC_MR_IMCKFS_64           (IISC_MR_IMCKFS_64_Val         << IISC_MR_IMCKFS_Pos)
#define IISC_MR_IMCKFS_128          (IISC_MR_IMCKFS_128_Val        << IISC_MR_IMCKFS_Pos)
#define IISC_MR_IMCKFS_256          (IISC_MR_IMCKFS_256_Val        << IISC_MR_IMCKFS_Pos)
#define IISC_MR_IMCKFS_384          (IISC_MR_IMCKFS_384_Val        << IISC_MR_IMCKFS_Pos)
#define IISC_MR_IMCKFS_512          (IISC_MR_IMCKFS_512_Val        << IISC_MR_IMCKFS_Pos)
#define IISC_MR_IMCKFS_768          (IISC_MR_IMCKFS_768_Val        << IISC_MR_IMCKFS_Pos)
#define IISC_MR_IMCKFS_1024         (IISC_MR_IMCKFS_1024_Val       << IISC_MR_IMCKFS_Pos)
#define IISC_MR_IMCKMODE_Pos        30           /**< \brief (IISC_MR) Master Clock Mode */
#define IISC_MR_IMCKMODE            (0x1u << IISC_MR_IMCKMODE_Pos)
#define   IISC_MR_IMCKMODE_NO_IMCK_Val    0x0u   /**< \brief (IISC_MR) No IMCK generated */
#define   IISC_MR_IMCKMODE_IMCK_Val       0x1u   /**< \brief (IISC_MR) IMCK generated */
#define IISC_MR_IMCKMODE_NO_IMCK    (IISC_MR_IMCKMODE_NO_IMCK_Val  << IISC_MR_IMCKMODE_Pos)
#define IISC_MR_IMCKMODE_IMCK       (IISC_MR_IMCKMODE_IMCK_Val     << IISC_MR_IMCKMODE_Pos)
#define IISC_MR_IWS24_Pos           31           /**< \brief (IISC_MR) IWS Data Slot Width */
#define IISC_MR_IWS24               (0x1u << IISC_MR_IWS24_Pos)
#define   IISC_MR_IWS24_32_Val            0x0u   /**< \brief (IISC_MR) IWS Data Slot is 32-bit wide for DATALENGTH=18/20/24-bit */
#define   IISC_MR_IWS24_24_Val            0x1u   /**< \brief (IISC_MR) IWS Data Slot is 24-bit wide for DATALENGTH=18/20/24-bit */
#define IISC_MR_IWS24_32            (IISC_MR_IWS24_32_Val          << IISC_MR_IWS24_Pos)
#define IISC_MR_IWS24_24            (IISC_MR_IWS24_24_Val          << IISC_MR_IWS24_Pos)
#define IISC_MR_MASK                0xFF00771Du  /**< \brief (IISC_MR) MASK Register */

/* -------- IISC_SR : (IISC Offset: 0x08) (R/  32) Status Register -------- */
#define IISC_SR_OFFSET              0x08         /**< \brief (IISC_SR offset) Status Register */
#define IISC_SR_RESETVALUE          0x00000000   /**< \brief (IISC_SR reset_value) Status Register */

#define IISC_SR_RXEN_Pos            0            /**< \brief (IISC_SR) Receive Enable */
#define IISC_SR_RXEN                (0x1u << IISC_SR_RXEN_Pos)
#define   IISC_SR_RXEN_OFF_Val            0x0u   /**< \brief (IISC_SR) Receiver is effectively disabled, following a CR.RXDIS or CR.SWRST request */
#define   IISC_SR_RXEN_ON_Val             0x1u   /**< \brief (IISC_SR) Receiver is effectively enabled, following a CR.RXEN request */
#define IISC_SR_RXEN_OFF            (IISC_SR_RXEN_OFF_Val          << IISC_SR_RXEN_Pos)
#define IISC_SR_RXEN_ON             (IISC_SR_RXEN_ON_Val           << IISC_SR_RXEN_Pos)
#define IISC_SR_RXRDY_Pos           1            /**< \brief (IISC_SR) Receive Ready */
#define IISC_SR_RXRDY               (0x1u << IISC_SR_RXRDY_Pos)
#define   IISC_SR_RXRDY_EMPTY_Val         0x0u   /**< \brief (IISC_SR) The register RHR is empty and can't be read */
#define   IISC_SR_RXRDY_FULL_Val          0x1u   /**< \brief (IISC_SR) The register RHR is full and is ready to be read */
#define IISC_SR_RXRDY_EMPTY         (IISC_SR_RXRDY_EMPTY_Val       << IISC_SR_RXRDY_Pos)
#define IISC_SR_RXRDY_FULL          (IISC_SR_RXRDY_FULL_Val        << IISC_SR_RXRDY_Pos)
#define IISC_SR_RXOR_Pos            2            /**< \brief (IISC_SR) Receive Overrun */
#define IISC_SR_RXOR                (0x1u << IISC_SR_RXOR_Pos)
#define   IISC_SR_RXOR_NO_Val             0x0u   /**< \brief (IISC_SR) No overrun */
#define   IISC_SR_RXOR_YES_Val            0x1u   /**< \brief (IISC_SR) The previous received data has not been read. This data is lost */
#define IISC_SR_RXOR_NO             (IISC_SR_RXOR_NO_Val           << IISC_SR_RXOR_Pos)
#define IISC_SR_RXOR_YES            (IISC_SR_RXOR_YES_Val          << IISC_SR_RXOR_Pos)
#define IISC_SR_TXEN_Pos            4            /**< \brief (IISC_SR) Transmit Enable */
#define IISC_SR_TXEN                (0x1u << IISC_SR_TXEN_Pos)
#define   IISC_SR_TXEN_OFF_Val            0x0u   /**< \brief (IISC_SR) Transmitter is effectively disabled, following a CR.TXDIS or CR.SWRST request */
#define   IISC_SR_TXEN_ON_Val             0x1u   /**< \brief (IISC_SR) Transmitter is effectively enabled, following a CR.TXEN request */
#define IISC_SR_TXEN_OFF            (IISC_SR_TXEN_OFF_Val          << IISC_SR_TXEN_Pos)
#define IISC_SR_TXEN_ON             (IISC_SR_TXEN_ON_Val           << IISC_SR_TXEN_Pos)
#define IISC_SR_TXRDY_Pos           5            /**< \brief (IISC_SR) Transmit Ready */
#define IISC_SR_TXRDY               (0x1u << IISC_SR_TXRDY_Pos)
#define   IISC_SR_TXRDY_FULL_Val          0x0u   /**< \brief (IISC_SR) The register THR is full and can't be written */
#define   IISC_SR_TXRDY_EMPTY_Val         0x1u   /**< \brief (IISC_SR) The register THR is empty and is ready to be written */
#define IISC_SR_TXRDY_FULL          (IISC_SR_TXRDY_FULL_Val        << IISC_SR_TXRDY_Pos)
#define IISC_SR_TXRDY_EMPTY         (IISC_SR_TXRDY_EMPTY_Val       << IISC_SR_TXRDY_Pos)
#define IISC_SR_TXUR_Pos            6            /**< \brief (IISC_SR) Transmit Underrun */
#define IISC_SR_TXUR                (0x1u << IISC_SR_TXUR_Pos)
#define   IISC_SR_TXUR_NO_Val             0x0u   /**< \brief (IISC_SR) No underrun */
#define   IISC_SR_TXUR_YES_Val            0x1u   /**< \brief (IISC_SR) The last bit of the last data written to the register THR has been set. Until the next write to THR, data will be sent according to MR.TXSAME field */
#define IISC_SR_TXUR_NO             (IISC_SR_TXUR_NO_Val           << IISC_SR_TXUR_Pos)
#define IISC_SR_TXUR_YES            (IISC_SR_TXUR_YES_Val          << IISC_SR_TXUR_Pos)
#define IISC_SR_RXORCH_Pos          8            /**< \brief (IISC_SR) Receive Overrun Channels */
#define IISC_SR_RXORCH_Msk          (0x3u << IISC_SR_RXORCH_Pos)
#define IISC_SR_RXORCH(value)       ((IISC_SR_RXORCH_Msk & ((value) << IISC_SR_RXORCH_Pos)))
#define   IISC_SR_RXORCH_LEFT_Val         0x0u   /**< \brief (IISC_SR) Overrun first occurred on left channel */
#define   IISC_SR_RXORCH_RIGHT_Val        0x1u   /**< \brief (IISC_SR) Overrun first occurred on right channel */
#define IISC_SR_RXORCH_LEFT         (IISC_SR_RXORCH_LEFT_Val       << IISC_SR_RXORCH_Pos)
#define IISC_SR_RXORCH_RIGHT        (IISC_SR_RXORCH_RIGHT_Val      << IISC_SR_RXORCH_Pos)
#define IISC_SR_TXURCH_Pos          20           /**< \brief (IISC_SR) Transmit Underrun Channels */
#define IISC_SR_TXURCH_Msk          (0x3u << IISC_SR_TXURCH_Pos)
#define IISC_SR_TXURCH(value)       ((IISC_SR_TXURCH_Msk & ((value) << IISC_SR_TXURCH_Pos)))
#define   IISC_SR_TXURCH_LEFT_Val         0x0u   /**< \brief (IISC_SR) Underrun first occurred on left channel */
#define   IISC_SR_TXURCH_RIGHT_Val        0x1u   /**< \brief (IISC_SR) Underrun first occurred on right channel */
#define IISC_SR_TXURCH_LEFT         (IISC_SR_TXURCH_LEFT_Val       << IISC_SR_TXURCH_Pos)
#define IISC_SR_TXURCH_RIGHT        (IISC_SR_TXURCH_RIGHT_Val      << IISC_SR_TXURCH_Pos)
#define IISC_SR_MASK                0x00300377u  /**< \brief (IISC_SR) MASK Register */

/* -------- IISC_SCR : (IISC Offset: 0x0C) ( /W 32) Status Clear Register -------- */
#define IISC_SCR_OFFSET             0x0C         /**< \brief (IISC_SCR offset) Status Clear Register */

#define IISC_SCR_RXOR_Pos           2            /**< \brief (IISC_SCR) Receive Overrun */
#define IISC_SCR_RXOR               (0x1u << IISC_SCR_RXOR_Pos)
#define   IISC_SCR_RXOR_NO_Val            0x0u   /**< \brief (IISC_SCR) No effect */
#define   IISC_SCR_RXOR_CLEAR_Val         0x1u   /**< \brief (IISC_SCR) Clears the corresponding SR bit */
#define IISC_SCR_RXOR_NO            (IISC_SCR_RXOR_NO_Val          << IISC_SCR_RXOR_Pos)
#define IISC_SCR_RXOR_CLEAR         (IISC_SCR_RXOR_CLEAR_Val       << IISC_SCR_RXOR_Pos)
#define IISC_SCR_TXUR_Pos           6            /**< \brief (IISC_SCR) Transmit Underrun */
#define IISC_SCR_TXUR               (0x1u << IISC_SCR_TXUR_Pos)
#define   IISC_SCR_TXUR_NO_Val            0x0u   /**< \brief (IISC_SCR) No effect */
#define   IISC_SCR_TXUR_CLEAR_Val         0x1u   /**< \brief (IISC_SCR) Clears the corresponding SR bit */
#define IISC_SCR_TXUR_NO            (IISC_SCR_TXUR_NO_Val          << IISC_SCR_TXUR_Pos)
#define IISC_SCR_TXUR_CLEAR         (IISC_SCR_TXUR_CLEAR_Val       << IISC_SCR_TXUR_Pos)
#define IISC_SCR_RXORCH_Pos         8            /**< \brief (IISC_SCR) Receive Overrun Channels */
#define IISC_SCR_RXORCH_Msk         (0x3u << IISC_SCR_RXORCH_Pos)
#define IISC_SCR_RXORCH(value)      ((IISC_SCR_RXORCH_Msk & ((value) << IISC_SCR_RXORCH_Pos)))
#define IISC_SCR_TXURCH_Pos         20           /**< \brief (IISC_SCR) Transmit Underrun Channels */
#define IISC_SCR_TXURCH_Msk         (0x3u << IISC_SCR_TXURCH_Pos)
#define IISC_SCR_TXURCH(value)      ((IISC_SCR_TXURCH_Msk & ((value) << IISC_SCR_TXURCH_Pos)))
#define IISC_SCR_MASK               0x00300344u  /**< \brief (IISC_SCR) MASK Register */

/* -------- IISC_SSR : (IISC Offset: 0x10) ( /W 32) Status Set Register -------- */
#define IISC_SSR_OFFSET             0x10         /**< \brief (IISC_SSR offset) Status Set Register */

#define IISC_SSR_RXOR_Pos           2            /**< \brief (IISC_SSR) Receive Overrun */
#define IISC_SSR_RXOR               (0x1u << IISC_SSR_RXOR_Pos)
#define   IISC_SSR_RXOR_NO_Val            0x0u   /**< \brief (IISC_SSR) No effect */
#define   IISC_SSR_RXOR_SET_Val           0x1u   /**< \brief (IISC_SSR) Sets corresponding SR bit */
#define IISC_SSR_RXOR_NO            (IISC_SSR_RXOR_NO_Val          << IISC_SSR_RXOR_Pos)
#define IISC_SSR_RXOR_SET           (IISC_SSR_RXOR_SET_Val         << IISC_SSR_RXOR_Pos)
#define IISC_SSR_TXUR_Pos           6            /**< \brief (IISC_SSR) Transmit Underrun */
#define IISC_SSR_TXUR               (0x1u << IISC_SSR_TXUR_Pos)
#define   IISC_SSR_TXUR_NO_Val            0x0u   /**< \brief (IISC_SSR) No effect */
#define   IISC_SSR_TXUR_SET_Val           0x1u   /**< \brief (IISC_SSR) Sets corresponding SR bit */
#define IISC_SSR_TXUR_NO            (IISC_SSR_TXUR_NO_Val          << IISC_SSR_TXUR_Pos)
#define IISC_SSR_TXUR_SET           (IISC_SSR_TXUR_SET_Val         << IISC_SSR_TXUR_Pos)
#define IISC_SSR_RXORCH_Pos         8            /**< \brief (IISC_SSR) Receive Overrun Channels */
#define IISC_SSR_RXORCH_Msk         (0x3u << IISC_SSR_RXORCH_Pos)
#define IISC_SSR_RXORCH(value)      ((IISC_SSR_RXORCH_Msk & ((value) << IISC_SSR_RXORCH_Pos)))
#define IISC_SSR_TXURCH_Pos         20           /**< \brief (IISC_SSR) Transmit Underrun Channels */
#define IISC_SSR_TXURCH_Msk         (0x3u << IISC_SSR_TXURCH_Pos)
#define IISC_SSR_TXURCH(value)      ((IISC_SSR_TXURCH_Msk & ((value) << IISC_SSR_TXURCH_Pos)))
#define IISC_SSR_MASK               0x00300344u  /**< \brief (IISC_SSR) MASK Register */

/* -------- IISC_IER : (IISC Offset: 0x14) ( /W 32) Interrupt Enable Register -------- */
#define IISC_IER_OFFSET             0x14         /**< \brief (IISC_IER offset) Interrupt Enable Register */

#define IISC_IER_RXRDY_Pos          1            /**< \brief (IISC_IER) Receiver Ready Interrupt Enable */
#define IISC_IER_RXRDY              (0x1u << IISC_IER_RXRDY_Pos)
#define   IISC_IER_RXRDY_OFF_Val          0x0u   /**< \brief (IISC_IER) No effect */
#define   IISC_IER_RXRDY_ON_Val           0x1u   /**< \brief (IISC_IER) Enables the corresponding interrupt */
#define IISC_IER_RXRDY_OFF          (IISC_IER_RXRDY_OFF_Val        << IISC_IER_RXRDY_Pos)
#define IISC_IER_RXRDY_ON           (IISC_IER_RXRDY_ON_Val         << IISC_IER_RXRDY_Pos)
#define IISC_IER_RXOR_Pos           2            /**< \brief (IISC_IER) Receive Overrun Interrupt Enable */
#define IISC_IER_RXOR               (0x1u << IISC_IER_RXOR_Pos)
#define   IISC_IER_RXOR_OFF_Val           0x0u   /**< \brief (IISC_IER) No effect */
#define   IISC_IER_RXOR_ON_Val            0x1u   /**< \brief (IISC_IER) Enables the corresponding interrupt */
#define IISC_IER_RXOR_OFF           (IISC_IER_RXOR_OFF_Val         << IISC_IER_RXOR_Pos)
#define IISC_IER_RXOR_ON            (IISC_IER_RXOR_ON_Val          << IISC_IER_RXOR_Pos)
#define IISC_IER_TXRDY_Pos          5            /**< \brief (IISC_IER) Transmit Ready Interrupt Enable */
#define IISC_IER_TXRDY              (0x1u << IISC_IER_TXRDY_Pos)
#define   IISC_IER_TXRDY_OFF_Val          0x0u   /**< \brief (IISC_IER) No effect */
#define   IISC_IER_TXRDY_ON_Val           0x1u   /**< \brief (IISC_IER) Enables the corresponding interrupt */
#define IISC_IER_TXRDY_OFF          (IISC_IER_TXRDY_OFF_Val        << IISC_IER_TXRDY_Pos)
#define IISC_IER_TXRDY_ON           (IISC_IER_TXRDY_ON_Val         << IISC_IER_TXRDY_Pos)
#define IISC_IER_TXUR_Pos           6            /**< \brief (IISC_IER) Transmit Underrun Interrupt Enable */
#define IISC_IER_TXUR               (0x1u << IISC_IER_TXUR_Pos)
#define   IISC_IER_TXUR_OFF_Val           0x0u   /**< \brief (IISC_IER) No effect */
#define   IISC_IER_TXUR_ON_Val            0x1u   /**< \brief (IISC_IER) Enables the corresponding interrupt */
#define IISC_IER_TXUR_OFF           (IISC_IER_TXUR_OFF_Val         << IISC_IER_TXUR_Pos)
#define IISC_IER_TXUR_ON            (IISC_IER_TXUR_ON_Val          << IISC_IER_TXUR_Pos)
#define IISC_IER_MASK               0x00000066u  /**< \brief (IISC_IER) MASK Register */

/* -------- IISC_IDR : (IISC Offset: 0x18) ( /W 32) Interrupt Disable Register -------- */
#define IISC_IDR_OFFSET             0x18         /**< \brief (IISC_IDR offset) Interrupt Disable Register */

#define IISC_IDR_RXRDY_Pos          1            /**< \brief (IISC_IDR) Receive Ready Interrupt Disable */
#define IISC_IDR_RXRDY              (0x1u << IISC_IDR_RXRDY_Pos)
#define   IISC_IDR_RXRDY_OFF_Val          0x0u   /**< \brief (IISC_IDR) No effect */
#define   IISC_IDR_RXRDY_ON_Val           0x1u   /**< \brief (IISC_IDR) Disables the corresponding interrupt */
#define IISC_IDR_RXRDY_OFF          (IISC_IDR_RXRDY_OFF_Val        << IISC_IDR_RXRDY_Pos)
#define IISC_IDR_RXRDY_ON           (IISC_IDR_RXRDY_ON_Val         << IISC_IDR_RXRDY_Pos)
#define IISC_IDR_RXOR_Pos           2            /**< \brief (IISC_IDR) Receive Overrun Interrupt Disable */
#define IISC_IDR_RXOR               (0x1u << IISC_IDR_RXOR_Pos)
#define   IISC_IDR_RXOR_OFF_Val           0x0u   /**< \brief (IISC_IDR) No effect */
#define   IISC_IDR_RXOR_ON_Val            0x1u   /**< \brief (IISC_IDR) Disables the corresponding interrupt */
#define IISC_IDR_RXOR_OFF           (IISC_IDR_RXOR_OFF_Val         << IISC_IDR_RXOR_Pos)
#define IISC_IDR_RXOR_ON            (IISC_IDR_RXOR_ON_Val          << IISC_IDR_RXOR_Pos)
#define IISC_IDR_TXRDY_Pos          5            /**< \brief (IISC_IDR) Transmit Ready Interrupt Disable */
#define IISC_IDR_TXRDY              (0x1u << IISC_IDR_TXRDY_Pos)
#define   IISC_IDR_TXRDY_OFF_Val          0x0u   /**< \brief (IISC_IDR) No effect */
#define   IISC_IDR_TXRDY_ON_Val           0x1u   /**< \brief (IISC_IDR) Disables the corresponding interrupt */
#define IISC_IDR_TXRDY_OFF          (IISC_IDR_TXRDY_OFF_Val        << IISC_IDR_TXRDY_Pos)
#define IISC_IDR_TXRDY_ON           (IISC_IDR_TXRDY_ON_Val         << IISC_IDR_TXRDY_Pos)
#define IISC_IDR_TXUR_Pos           6            /**< \brief (IISC_IDR) Transmit Underrun Interrupt Disable */
#define IISC_IDR_TXUR               (0x1u << IISC_IDR_TXUR_Pos)
#define   IISC_IDR_TXUR_OFF_Val           0x0u   /**< \brief (IISC_IDR) No effect */
#define   IISC_IDR_TXUR_ON_Val            0x1u   /**< \brief (IISC_IDR) Disables the corresponding interrupt */
#define IISC_IDR_TXUR_OFF           (IISC_IDR_TXUR_OFF_Val         << IISC_IDR_TXUR_Pos)
#define IISC_IDR_TXUR_ON            (IISC_IDR_TXUR_ON_Val          << IISC_IDR_TXUR_Pos)
#define IISC_IDR_MASK               0x00000066u  /**< \brief (IISC_IDR) MASK Register */

/* -------- IISC_IMR : (IISC Offset: 0x1C) (R/  32) Interrupt Mask Register -------- */
#define IISC_IMR_OFFSET             0x1C         /**< \brief (IISC_IMR offset) Interrupt Mask Register */
#define IISC_IMR_RESETVALUE         0x00000000   /**< \brief (IISC_IMR reset_value) Interrupt Mask Register */

#define IISC_IMR_RXRDY_Pos          1            /**< \brief (IISC_IMR) Receive Ready Interrupt Mask */
#define IISC_IMR_RXRDY              (0x1u << IISC_IMR_RXRDY_Pos)
#define   IISC_IMR_RXRDY_DISABLED_Val     0x0u   /**< \brief (IISC_IMR) The corresponding interrupt is disabled */
#define   IISC_IMR_RXRDY_ENABLED_Val      0x1u   /**< \brief (IISC_IMR) The corresponding interrupt is enabled */
#define IISC_IMR_RXRDY_DISABLED     (IISC_IMR_RXRDY_DISABLED_Val   << IISC_IMR_RXRDY_Pos)
#define IISC_IMR_RXRDY_ENABLED      (IISC_IMR_RXRDY_ENABLED_Val    << IISC_IMR_RXRDY_Pos)
#define IISC_IMR_RXOR_Pos           2            /**< \brief (IISC_IMR) Receive Overrun Interrupt Mask */
#define IISC_IMR_RXOR               (0x1u << IISC_IMR_RXOR_Pos)
#define   IISC_IMR_RXOR_DISABLED_Val      0x0u   /**< \brief (IISC_IMR) The corresponding interrupt is disabled */
#define   IISC_IMR_RXOR_ENABLED_Val       0x1u   /**< \brief (IISC_IMR) The corresponding interrupt is enabled */
#define IISC_IMR_RXOR_DISABLED      (IISC_IMR_RXOR_DISABLED_Val    << IISC_IMR_RXOR_Pos)
#define IISC_IMR_RXOR_ENABLED       (IISC_IMR_RXOR_ENABLED_Val     << IISC_IMR_RXOR_Pos)
#define IISC_IMR_TXRDY_Pos          5            /**< \brief (IISC_IMR) Transmit Ready Interrupt Mask */
#define IISC_IMR_TXRDY              (0x1u << IISC_IMR_TXRDY_Pos)
#define   IISC_IMR_TXRDY_DISABLED_Val     0x0u   /**< \brief (IISC_IMR) The corresponding interrupt is disabled */
#define   IISC_IMR_TXRDY_ENABLED_Val      0x1u   /**< \brief (IISC_IMR) The corresponding interrupt is enabled */
#define IISC_IMR_TXRDY_DISABLED     (IISC_IMR_TXRDY_DISABLED_Val   << IISC_IMR_TXRDY_Pos)
#define IISC_IMR_TXRDY_ENABLED      (IISC_IMR_TXRDY_ENABLED_Val    << IISC_IMR_TXRDY_Pos)
#define IISC_IMR_TXUR_Pos           6            /**< \brief (IISC_IMR) Transmit Underrun Interrupt Mask */
#define IISC_IMR_TXUR               (0x1u << IISC_IMR_TXUR_Pos)
#define   IISC_IMR_TXUR_DISABLED_Val      0x0u   /**< \brief (IISC_IMR) The corresponding interrupt is disabled */
#define   IISC_IMR_TXUR_ENABLED_Val       0x1u   /**< \brief (IISC_IMR) The corresponding interrupt is enabled */
#define IISC_IMR_TXUR_DISABLED      (IISC_IMR_TXUR_DISABLED_Val    << IISC_IMR_TXUR_Pos)
#define IISC_IMR_TXUR_ENABLED       (IISC_IMR_TXUR_ENABLED_Val     << IISC_IMR_TXUR_Pos)
#define IISC_IMR_MASK               0x00000066u  /**< \brief (IISC_IMR) MASK Register */

/* -------- IISC_RHR : (IISC Offset: 0x20) (R/  32) Receive Holding Register -------- */
#define IISC_RHR_OFFSET             0x20         /**< \brief (IISC_RHR offset) Receive Holding Register */
#define IISC_RHR_RESETVALUE         0x00000000   /**< \brief (IISC_RHR reset_value) Receive Holding Register */

#define IISC_RHR_RDAT_Pos           0            /**< \brief (IISC_RHR) Receive Data */
#define IISC_RHR_RDAT_Msk           (0xFFFFFFFFu << IISC_RHR_RDAT_Pos)
#define IISC_RHR_RDAT(value)        ((IISC_RHR_RDAT_Msk & ((value) << IISC_RHR_RDAT_Pos)))
#define IISC_RHR_MASK               0xFFFFFFFFu  /**< \brief (IISC_RHR) MASK Register */

/* -------- IISC_THR : (IISC Offset: 0x24) ( /W 32) Transmit Holding Register -------- */
#define IISC_THR_OFFSET             0x24         /**< \brief (IISC_THR offset) Transmit Holding Register */

#define IISC_THR_TDAT_Pos           0            /**< \brief (IISC_THR) Transmit Data */
#define IISC_THR_TDAT_Msk           (0xFFFFFFFFu << IISC_THR_TDAT_Pos)
#define IISC_THR_TDAT(value)        ((IISC_THR_TDAT_Msk & ((value) << IISC_THR_TDAT_Pos)))
#define IISC_THR_MASK               0xFFFFFFFFu  /**< \brief (IISC_THR) MASK Register */

/* -------- IISC_VERSION : (IISC Offset: 0x28) (R/  32) Version Register -------- */
#define IISC_VERSION_OFFSET         0x28         /**< \brief (IISC_VERSION offset) Version Register */
#define IISC_VERSION_RESETVALUE     0x00000100   /**< \brief (IISC_VERSION reset_value) Version Register */

#define IISC_VERSION_VERSION_Pos    0            /**< \brief (IISC_VERSION) Reserved. Value subject to change. No functionality associated. This is the Atmel internal version of the macrocell. */
#define IISC_VERSION_VERSION_Msk    (0xFFFu << IISC_VERSION_VERSION_Pos)
#define IISC_VERSION_VERSION(value) ((IISC_VERSION_VERSION_Msk & ((value) << IISC_VERSION_VERSION_Pos)))
#define IISC_VERSION_VARIANT_Pos    16           /**< \brief (IISC_VERSION) Reserved. Value subject to change. No functionality associated. */
#define IISC_VERSION_VARIANT_Msk    (0xFu << IISC_VERSION_VARIANT_Pos)
#define IISC_VERSION_VARIANT(value) ((IISC_VERSION_VARIANT_Msk & ((value) << IISC_VERSION_VARIANT_Pos)))
#define IISC_VERSION_MASK           0x000F0FFFu  /**< \brief (IISC_VERSION) MASK Register */

/* -------- IISC_PARAMETER : (IISC Offset: 0x2C) (R/  32) Parameter Register -------- */
#define IISC_PARAMETER_OFFSET       0x2C         /**< \brief (IISC_PARAMETER offset) Parameter Register */
#define IISC_PARAMETER_RESETVALUE   0x00010000   /**< \brief (IISC_PARAMETER reset_value) Parameter Register */

#define IISC_PARAMETER_FORMAT_Pos   7            /**< \brief (IISC_PARAMETER) Data protocol format */
#define IISC_PARAMETER_FORMAT       (0x1u << IISC_PARAMETER_FORMAT_Pos)
#define   IISC_PARAMETER_FORMAT_I2S_Val   0x0u   /**< \brief (IISC_PARAMETER) I2S format, stereo with IWS low for left channel */
#define IISC_PARAMETER_FORMAT_I2S   (IISC_PARAMETER_FORMAT_I2S_Val << IISC_PARAMETER_FORMAT_Pos)
#define IISC_PARAMETER_NBCHAN_Pos   16           /**< \brief (IISC_PARAMETER) Maximum number of channels - 1 */
#define IISC_PARAMETER_NBCHAN_Msk   (0x1Fu << IISC_PARAMETER_NBCHAN_Pos)
#define IISC_PARAMETER_NBCHAN(value) ((IISC_PARAMETER_NBCHAN_Msk & ((value) << IISC_PARAMETER_NBCHAN_Pos)))
#define IISC_PARAMETER_MASK         0x001F0080u  /**< \brief (IISC_PARAMETER) MASK Register */

/** \brief IISC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  WoReg   IISC_CR;            /**< \brief (IISC Offset: 0x00) Control Register */
  RwReg   IISC_MR;            /**< \brief (IISC Offset: 0x04) Mode Register */
  RoReg   IISC_SR;            /**< \brief (IISC Offset: 0x08) Status Register */
  WoReg   IISC_SCR;           /**< \brief (IISC Offset: 0x0C) Status Clear Register */
  WoReg   IISC_SSR;           /**< \brief (IISC Offset: 0x10) Status Set Register */
  WoReg   IISC_IER;           /**< \brief (IISC Offset: 0x14) Interrupt Enable Register */
  WoReg   IISC_IDR;           /**< \brief (IISC Offset: 0x18) Interrupt Disable Register */
  RoReg   IISC_IMR;           /**< \brief (IISC Offset: 0x1C) Interrupt Mask Register */
  RoReg   IISC_RHR;           /**< \brief (IISC Offset: 0x20) Receive Holding Register */
  WoReg   IISC_THR;           /**< \brief (IISC Offset: 0x24) Transmit Holding Register */
  RoReg   IISC_VERSION;       /**< \brief (IISC Offset: 0x28) Version Register */
  RoReg   IISC_PARAMETER;     /**< \brief (IISC Offset: 0x2C) Parameter Register */
} Iisc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAM4L_IISC_COMPONENT_ */
