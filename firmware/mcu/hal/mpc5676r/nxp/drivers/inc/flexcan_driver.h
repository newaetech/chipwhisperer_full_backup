/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FLEXCAN_DRIVER_H
#define FLEXCAN_DRIVER_H

#include "MPC5676R.h"
#include "MPC5676R_features.h"
#include "../util/status.h"
#include "osif.h"
#include <stdbool.h>

#if FEATURE_CAN_HAS_DMA_ENABLE
#include "edma_driver.h"
#endif

/* */
/* */
/* */

/*!
 * @defgroup flexcan_driver FlexCAN Driver
 * @ingroup flexcan
 * @addtogroup flexcan_driver
 * @{
 */

/* ----------------------------------------------------------------------------
   -- CAN Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CAN_Register_Masks CAN Register Masks
 * @{
 */

/* MCR Bit Fields */
#define CAN_MCR_MAXMB_MASK                       0x7Fu
#define CAN_MCR_MAXMB_SHIFT                      0u
#define CAN_MCR_MAXMB_WIDTH                      7u
#define CAN_MCR_MAXMB(x)                         (((uint32_t)(((uint32_t)(x))<<CAN_MCR_MAXMB_SHIFT))&CAN_MCR_MAXMB_MASK)
#define CAN_MCR_IDAM_MASK                        0x300u
#define CAN_MCR_IDAM_SHIFT                       8u
#define CAN_MCR_IDAM_WIDTH                       2u
#define CAN_MCR_IDAM(x)                          (((uint32_t)(((uint32_t)(x))<<CAN_MCR_IDAM_SHIFT))&CAN_MCR_IDAM_MASK)
#define CAN_MCR_AEN_MASK                         0x1000u
#define CAN_MCR_AEN_SHIFT                        12u
#define CAN_MCR_AEN_WIDTH                        1u
#define CAN_MCR_AEN(x)                           (((uint32_t)(((uint32_t)(x))<<CAN_MCR_AEN_SHIFT))&CAN_MCR_AEN_MASK)
#define CAN_MCR_LPRIOEN_MASK                     0x2000u
#define CAN_MCR_LPRIOEN_SHIFT                    13u
#define CAN_MCR_LPRIOEN_WIDTH                    1u
#define CAN_MCR_LPRIOEN(x)                       (((uint32_t)(((uint32_t)(x))<<CAN_MCR_LPRIOEN_SHIFT))&CAN_MCR_LPRIOEN_MASK)
#define CAN_MCR_IRMQ_MASK                        0x10000u
#define CAN_MCR_IRMQ_SHIFT                       16u
#define CAN_MCR_IRMQ_WIDTH                       1u
#define CAN_MCR_IRMQ(x)                          (((uint32_t)(((uint32_t)(x))<<CAN_MCR_IRMQ_SHIFT))&CAN_MCR_IRMQ_MASK)
#define CAN_MCR_SRXDIS_MASK                      0x20000u
#define CAN_MCR_SRXDIS_SHIFT                     17u
#define CAN_MCR_SRXDIS_WIDTH                     1u
#define CAN_MCR_SRXDIS(x)                        (((uint32_t)(((uint32_t)(x))<<CAN_MCR_SRXDIS_SHIFT))&CAN_MCR_SRXDIS_MASK)
#define CAN_MCR_DOZE_MASK                        0x40000u
#define CAN_MCR_DOZE_SHIFT                       18u
#define CAN_MCR_DOZE_WIDTH                       1u
#define CAN_MCR_DOZE(x)                          (((uint32_t)(((uint32_t)(x))<<CAN_MCR_DOZE_SHIFT))&CAN_MCR_DOZE_MASK)
#define CAN_MCR_LPMACK_MASK                      0x100000u
#define CAN_MCR_LPMACK_SHIFT                     20u
#define CAN_MCR_LPMACK_WIDTH                     1u
#define CAN_MCR_LPMACK(x)                        (((uint32_t)(((uint32_t)(x))<<CAN_MCR_LPMACK_SHIFT))&CAN_MCR_LPMACK_MASK)
#define CAN_MCR_WRNEN_MASK                       0x200000u
#define CAN_MCR_WRNEN_SHIFT                      21u
#define CAN_MCR_WRNEN_WIDTH                      1u
#define CAN_MCR_WRNEN(x)                         (((uint32_t)(((uint32_t)(x))<<CAN_MCR_WRNEN_SHIFT))&CAN_MCR_WRNEN_MASK)
#define CAN_MCR_SUPV_MASK                        0x800000u
#define CAN_MCR_SUPV_SHIFT                       23u
#define CAN_MCR_SUPV_WIDTH                       1u
#define CAN_MCR_SUPV(x)                          (((uint32_t)(((uint32_t)(x))<<CAN_MCR_SUPV_SHIFT))&CAN_MCR_SUPV_MASK)
#define CAN_MCR_FRZACK_MASK                      0x1000000u
#define CAN_MCR_FRZACK_SHIFT                     24u
#define CAN_MCR_FRZACK_WIDTH                     1u
#define CAN_MCR_FRZACK(x)                        (((uint32_t)(((uint32_t)(x))<<CAN_MCR_FRZACK_SHIFT))&CAN_MCR_FRZACK_MASK)
#define CAN_MCR_SOFTRST_MASK                     0x2000000u
#define CAN_MCR_SOFTRST_SHIFT                    25u
#define CAN_MCR_SOFTRST_WIDTH                    1u
#define CAN_MCR_SOFTRST(x)                       (((uint32_t)(((uint32_t)(x))<<CAN_MCR_SOFTRST_SHIFT))&CAN_MCR_SOFTRST_MASK)
#define CAN_MCR_NOTRDY_MASK                      0x8000000u
#define CAN_MCR_NOTRDY_SHIFT                     27u
#define CAN_MCR_NOTRDY_WIDTH                     1u
#define CAN_MCR_NOTRDY(x)                        (((uint32_t)(((uint32_t)(x))<<CAN_MCR_NOTRDY_SHIFT))&CAN_MCR_NOTRDY_MASK)
#define CAN_MCR_HALT_MASK                        0x10000000u
#define CAN_MCR_HALT_SHIFT                       28u
#define CAN_MCR_HALT_WIDTH                       1u
#define CAN_MCR_HALT(x)                          (((uint32_t)(((uint32_t)(x))<<CAN_MCR_HALT_SHIFT))&CAN_MCR_HALT_MASK)
#define CAN_MCR_RFEN_MASK                        0x20000000u
#define CAN_MCR_RFEN_SHIFT                       29u
#define CAN_MCR_RFEN_WIDTH                       1u
#define CAN_MCR_RFEN(x)                          (((uint32_t)(((uint32_t)(x))<<CAN_MCR_RFEN_SHIFT))&CAN_MCR_RFEN_MASK)
#define CAN_MCR_FRZ_MASK                         0x40000000u
#define CAN_MCR_FRZ_SHIFT                        30u
#define CAN_MCR_FRZ_WIDTH                        1u
#define CAN_MCR_FRZ(x)                           (((uint32_t)(((uint32_t)(x))<<CAN_MCR_FRZ_SHIFT))&CAN_MCR_FRZ_MASK)
#define CAN_MCR_MDIS_MASK                        0x80000000u
#define CAN_MCR_MDIS_SHIFT                       31u
#define CAN_MCR_MDIS_WIDTH                       1u
#define CAN_MCR_MDIS(x)                          (((uint32_t)(((uint32_t)(x))<<CAN_MCR_MDIS_SHIFT))&CAN_MCR_MDIS_MASK)
/* CTRL1 Bit Fields */
#define CAN_CTRL1_PROPSEG_MASK                   0x7u
#define CAN_CTRL1_PROPSEG_SHIFT                  0u
#define CAN_CTRL1_PROPSEG_WIDTH                  3u
#define CAN_CTRL1_PROPSEG(x)                     (((uint32_t)(((uint32_t)(x))<<CAN_CTRL1_PROPSEG_SHIFT))&CAN_CTRL1_PROPSEG_MASK)
#define CAN_CTRL1_LOM_MASK                       0x8u
#define CAN_CTRL1_LOM_SHIFT                      3u
#define CAN_CTRL1_LOM_WIDTH                      1u
#define CAN_CTRL1_LOM(x)                         (((uint32_t)(((uint32_t)(x))<<CAN_CTRL1_LOM_SHIFT))&CAN_CTRL1_LOM_MASK)
#define CAN_CTRL1_LBUF_MASK                      0x10u
#define CAN_CTRL1_LBUF_SHIFT                     4u
#define CAN_CTRL1_LBUF_WIDTH                     1u
#define CAN_CTRL1_LBUF(x)                        (((uint32_t)(((uint32_t)(x))<<CAN_CTRL1_LBUF_SHIFT))&CAN_CTRL1_LBUF_MASK)
#define CAN_CTRL1_TSYN_MASK                      0x20u
#define CAN_CTRL1_TSYN_SHIFT                     5u
#define CAN_CTRL1_TSYN_WIDTH                     1u
#define CAN_CTRL1_TSYN(x)                        (((uint32_t)(((uint32_t)(x))<<CAN_CTRL1_TSYN_SHIFT))&CAN_CTRL1_TSYN_MASK)
#define CAN_CTRL1_BOFFREC_MASK                   0x40u
#define CAN_CTRL1_BOFFREC_SHIFT                  6u
#define CAN_CTRL1_BOFFREC_WIDTH                  1u
#define CAN_CTRL1_BOFFREC(x)                     (((uint32_t)(((uint32_t)(x))<<CAN_CTRL1_BOFFREC_SHIFT))&CAN_CTRL1_BOFFREC_MASK)
#define CAN_CTRL1_SMP_MASK                       0x80u
#define CAN_CTRL1_SMP_SHIFT                      7u
#define CAN_CTRL1_SMP_WIDTH                      1u
#define CAN_CTRL1_SMP(x)                         (((uint32_t)(((uint32_t)(x))<<CAN_CTRL1_SMP_SHIFT))&CAN_CTRL1_SMP_MASK)
#define CAN_CTRL1_RWRNMSK_MASK                   0x400u
#define CAN_CTRL1_RWRNMSK_SHIFT                  10u
#define CAN_CTRL1_RWRNMSK_WIDTH                  1u
#define CAN_CTRL1_RWRNMSK(x)                     (((uint32_t)(((uint32_t)(x))<<CAN_CTRL1_RWRNMSK_SHIFT))&CAN_CTRL1_RWRNMSK_MASK)
#define CAN_CTRL1_TWRNMSK_MASK                   0x800u
#define CAN_CTRL1_TWRNMSK_SHIFT                  11u
#define CAN_CTRL1_TWRNMSK_WIDTH                  1u
#define CAN_CTRL1_TWRNMSK(x)                     (((uint32_t)(((uint32_t)(x))<<CAN_CTRL1_TWRNMSK_SHIFT))&CAN_CTRL1_TWRNMSK_MASK)
#define CAN_CTRL1_LPB_MASK                       0x1000u
#define CAN_CTRL1_LPB_SHIFT                      12u
#define CAN_CTRL1_LPB_WIDTH                      1u
#define CAN_CTRL1_LPB(x)                         (((uint32_t)(((uint32_t)(x))<<CAN_CTRL1_LPB_SHIFT))&CAN_CTRL1_LPB_MASK)
#define CAN_CTRL1_CLKSRC_MASK                    0x2000u
#define CAN_CTRL1_CLKSRC_SHIFT                   13u
#define CAN_CTRL1_CLKSRC_WIDTH                   1u
#define CAN_CTRL1_CLKSRC(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_CTRL1_CLKSRC_SHIFT))&CAN_CTRL1_CLKSRC_MASK)
#define CAN_CTRL1_ERRMSK_MASK                    0x4000u
#define CAN_CTRL1_ERRMSK_SHIFT                   14u
#define CAN_CTRL1_ERRMSK_WIDTH                   1u
#define CAN_CTRL1_ERRMSK(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_CTRL1_ERRMSK_SHIFT))&CAN_CTRL1_ERRMSK_MASK)
#define CAN_CTRL1_BOFFMSK_MASK                   0x8000u
#define CAN_CTRL1_BOFFMSK_SHIFT                  15u
#define CAN_CTRL1_BOFFMSK_WIDTH                  1u
#define CAN_CTRL1_BOFFMSK(x)                     (((uint32_t)(((uint32_t)(x))<<CAN_CTRL1_BOFFMSK_SHIFT))&CAN_CTRL1_BOFFMSK_MASK)
#define CAN_CTRL1_PSEG2_MASK                     0x70000u
#define CAN_CTRL1_PSEG2_SHIFT                    16u
#define CAN_CTRL1_PSEG2_WIDTH                    3u
#define CAN_CTRL1_PSEG2(x)                       (((uint32_t)(((uint32_t)(x))<<CAN_CTRL1_PSEG2_SHIFT))&CAN_CTRL1_PSEG2_MASK)
#define CAN_CTRL1_PSEG1_MASK                     0x380000u
#define CAN_CTRL1_PSEG1_SHIFT                    19u
#define CAN_CTRL1_PSEG1_WIDTH                    3u
#define CAN_CTRL1_PSEG1(x)                       (((uint32_t)(((uint32_t)(x))<<CAN_CTRL1_PSEG1_SHIFT))&CAN_CTRL1_PSEG1_MASK)
#define CAN_CTRL1_RJW_MASK                       0xC00000u
#define CAN_CTRL1_RJW_SHIFT                      22u
#define CAN_CTRL1_RJW_WIDTH                      2u
#define CAN_CTRL1_RJW(x)                         (((uint32_t)(((uint32_t)(x))<<CAN_CTRL1_RJW_SHIFT))&CAN_CTRL1_RJW_MASK)
#define CAN_CTRL1_PRESDIV_MASK                   0xFF000000u
#define CAN_CTRL1_PRESDIV_SHIFT                  24u
#define CAN_CTRL1_PRESDIV_WIDTH                  8u
#define CAN_CTRL1_PRESDIV(x)                     (((uint32_t)(((uint32_t)(x))<<CAN_CTRL1_PRESDIV_SHIFT))&CAN_CTRL1_PRESDIV_MASK)
/* TIMER Bit Fields */
#define CAN_TIMER_TIMER_MASK                     0xFFFFu
#define CAN_TIMER_TIMER_SHIFT                    0u
#define CAN_TIMER_TIMER_WIDTH                    16u
#define CAN_TIMER_TIMER(x)                       (((uint32_t)(((uint32_t)(x))<<CAN_TIMER_TIMER_SHIFT))&CAN_TIMER_TIMER_MASK)
/* RXMGMASK Bit Fields */
#define CAN_RXMGMASK_MG_MASK                     0xFFFFFFFFu
#define CAN_RXMGMASK_MG_SHIFT                    0u
#define CAN_RXMGMASK_MG_WIDTH                    32u
#define CAN_RXMGMASK_MG(x)                       (((uint32_t)(((uint32_t)(x))<<CAN_RXMGMASK_MG_SHIFT))&CAN_RXMGMASK_MG_MASK)
/* RX14MASK Bit Fields */
#define CAN_RX14MASK_RX14M_MASK                  0xFFFFFFFFu
#define CAN_RX14MASK_RX14M_SHIFT                 0u
#define CAN_RX14MASK_RX14M_WIDTH                 32u
#define CAN_RX14MASK_RX14M(x)                    (((uint32_t)(((uint32_t)(x))<<CAN_RX14MASK_RX14M_SHIFT))&CAN_RX14MASK_RX14M_MASK)
/* RX15MASK Bit Fields */
#define CAN_RX15MASK_RX15M_MASK                  0xFFFFFFFFu
#define CAN_RX15MASK_RX15M_SHIFT                 0u
#define CAN_RX15MASK_RX15M_WIDTH                 32u
#define CAN_RX15MASK_RX15M(x)                    (((uint32_t)(((uint32_t)(x))<<CAN_RX15MASK_RX15M_SHIFT))&CAN_RX15MASK_RX15M_MASK)
/* ECR Bit Fields */
#define CAN_ECR_TXERRCNT_MASK                    0xFFu
#define CAN_ECR_TXERRCNT_SHIFT                   0u
#define CAN_ECR_TXERRCNT_WIDTH                   8u
#define CAN_ECR_TXERRCNT(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_ECR_TXERRCNT_SHIFT))&CAN_ECR_TXERRCNT_MASK)
#define CAN_ECR_RXERRCNT_MASK                    0xFF00u
#define CAN_ECR_RXERRCNT_SHIFT                   8u
#define CAN_ECR_RXERRCNT_WIDTH                   8u
#define CAN_ECR_RXERRCNT(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_ECR_RXERRCNT_SHIFT))&CAN_ECR_RXERRCNT_MASK)
/* ESR1 Bit Fields */
#define CAN_ESR1_ERRINT_MASK                     0x2u
#define CAN_ESR1_ERRINT_SHIFT                    1u
#define CAN_ESR1_ERRINT_WIDTH                    1u
#define CAN_ESR1_ERRINT(x)                       (((uint32_t)(((uint32_t)(x))<<CAN_ESR1_ERRINT_SHIFT))&CAN_ESR1_ERRINT_MASK)
#define CAN_ESR1_BOFFINT_MASK                    0x4u
#define CAN_ESR1_BOFFINT_SHIFT                   2u
#define CAN_ESR1_BOFFINT_WIDTH                   1u
#define CAN_ESR1_BOFFINT(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_ESR1_BOFFINT_SHIFT))&CAN_ESR1_BOFFINT_MASK)
#define CAN_ESR1_RX_MASK                         0x8u
#define CAN_ESR1_RX_SHIFT                        3u
#define CAN_ESR1_RX_WIDTH                        1u
#define CAN_ESR1_RX(x)                           (((uint32_t)(((uint32_t)(x))<<CAN_ESR1_RX_SHIFT))&CAN_ESR1_RX_MASK)
#define CAN_ESR1_FLTCONF_MASK                    0x30u
#define CAN_ESR1_FLTCONF_SHIFT                   4u
#define CAN_ESR1_FLTCONF_WIDTH                   2u
#define CAN_ESR1_FLTCONF(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_ESR1_FLTCONF_SHIFT))&CAN_ESR1_FLTCONF_MASK)
#define CAN_ESR1_TX_MASK                         0x40u
#define CAN_ESR1_TX_SHIFT                        6u
#define CAN_ESR1_TX_WIDTH                        1u
#define CAN_ESR1_TX(x)                           (((uint32_t)(((uint32_t)(x))<<CAN_ESR1_TX_SHIFT))&CAN_ESR1_TX_MASK)
#define CAN_ESR1_IDLE_MASK                       0x80u
#define CAN_ESR1_IDLE_SHIFT                      7u
#define CAN_ESR1_IDLE_WIDTH                      1u
#define CAN_ESR1_IDLE(x)                         (((uint32_t)(((uint32_t)(x))<<CAN_ESR1_IDLE_SHIFT))&CAN_ESR1_IDLE_MASK)
#define CAN_ESR1_RXWRN_MASK                      0x100u
#define CAN_ESR1_RXWRN_SHIFT                     8u
#define CAN_ESR1_RXWRN_WIDTH                     1u
#define CAN_ESR1_RXWRN(x)                        (((uint32_t)(((uint32_t)(x))<<CAN_ESR1_RXWRN_SHIFT))&CAN_ESR1_RXWRN_MASK)
#define CAN_ESR1_TXWRN_MASK                      0x200u
#define CAN_ESR1_TXWRN_SHIFT                     9u
#define CAN_ESR1_TXWRN_WIDTH                     1u
#define CAN_ESR1_TXWRN(x)                        (((uint32_t)(((uint32_t)(x))<<CAN_ESR1_TXWRN_SHIFT))&CAN_ESR1_TXWRN_MASK)
#define CAN_ESR1_STFERR_MASK                     0x400u
#define CAN_ESR1_STFERR_SHIFT                    10u
#define CAN_ESR1_STFERR_WIDTH                    1u
#define CAN_ESR1_STFERR(x)                       (((uint32_t)(((uint32_t)(x))<<CAN_ESR1_STFERR_SHIFT))&CAN_ESR1_STFERR_MASK)
#define CAN_ESR1_FRMERR_MASK                     0x800u
#define CAN_ESR1_FRMERR_SHIFT                    11u
#define CAN_ESR1_FRMERR_WIDTH                    1u
#define CAN_ESR1_FRMERR(x)                       (((uint32_t)(((uint32_t)(x))<<CAN_ESR1_FRMERR_SHIFT))&CAN_ESR1_FRMERR_MASK)
#define CAN_ESR1_CRCERR_MASK                     0x1000u
#define CAN_ESR1_CRCERR_SHIFT                    12u
#define CAN_ESR1_CRCERR_WIDTH                    1u
#define CAN_ESR1_CRCERR(x)                       (((uint32_t)(((uint32_t)(x))<<CAN_ESR1_CRCERR_SHIFT))&CAN_ESR1_CRCERR_MASK)
#define CAN_ESR1_ACKERR_MASK                     0x2000u
#define CAN_ESR1_ACKERR_SHIFT                    13u
#define CAN_ESR1_ACKERR_WIDTH                    1u
#define CAN_ESR1_ACKERR(x)                       (((uint32_t)(((uint32_t)(x))<<CAN_ESR1_ACKERR_SHIFT))&CAN_ESR1_ACKERR_MASK)
#define CAN_ESR1_BIT0ERR_MASK                    0x4000u
#define CAN_ESR1_BIT0ERR_SHIFT                   14u
#define CAN_ESR1_BIT0ERR_WIDTH                   1u
#define CAN_ESR1_BIT0ERR(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_ESR1_BIT0ERR_SHIFT))&CAN_ESR1_BIT0ERR_MASK)
#define CAN_ESR1_BIT1ERR_MASK                    0x8000u
#define CAN_ESR1_BIT1ERR_SHIFT                   15u
#define CAN_ESR1_BIT1ERR_WIDTH                   1u
#define CAN_ESR1_BIT1ERR(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_ESR1_BIT1ERR_SHIFT))&CAN_ESR1_BIT1ERR_MASK)
#define CAN_ESR1_RWRNINT_MASK                    0x10000u
#define CAN_ESR1_RWRNINT_SHIFT                   16u
#define CAN_ESR1_RWRNINT_WIDTH                   1u
#define CAN_ESR1_RWRNINT(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_ESR1_RWRNINT_SHIFT))&CAN_ESR1_RWRNINT_MASK)
#define CAN_ESR1_TWRNINT_MASK                    0x20000u
#define CAN_ESR1_TWRNINT_SHIFT                   17u
#define CAN_ESR1_TWRNINT_WIDTH                   1u
#define CAN_ESR1_TWRNINT(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_ESR1_TWRNINT_SHIFT))&CAN_ESR1_TWRNINT_MASK)
#define CAN_ESR1_SYNCH_MASK                      0x40000u
#define CAN_ESR1_SYNCH_SHIFT                     18u
#define CAN_ESR1_SYNCH_WIDTH                     1u
#define CAN_ESR1_SYNCH(x)                        (((uint32_t)(((uint32_t)(x))<<CAN_ESR1_SYNCH_SHIFT))&CAN_ESR1_SYNCH_MASK)
/* IMASK2 Bit Fields */
#define CAN_IMASK2_BUFHM_MASK                    0xFFFFFFFFu
#define CAN_IMASK2_BUFHM_SHIFT                   0u
#define CAN_IMASK2_BUFHM_WIDTH                   32u
#define CAN_IMASK2_BUFHM(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_IMASK2_BUFHM_SHIFT))&CAN_IMASK2_BUFHM_MASK)
/* IMASK1 Bit Fields */
#define CAN_IMASK1_BUFLM_MASK                    0xFFFFFFFFu
#define CAN_IMASK1_BUFLM_SHIFT                   0u
#define CAN_IMASK1_BUFLM_WIDTH                   32u
#define CAN_IMASK1_BUFLM(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_IMASK1_BUFLM_SHIFT))&CAN_IMASK1_BUFLM_MASK)
/* IFLAG2 Bit Fields */
#define CAN_IFLAG2_BUFHI_MASK                    0xFFFFFFFFu
#define CAN_IFLAG2_BUFHI_SHIFT                   0u
#define CAN_IFLAG2_BUFHI_WIDTH                   32u
#define CAN_IFLAG2_BUFHI(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_IFLAG2_BUFHI_SHIFT))&CAN_IFLAG2_BUFHI_MASK)
/* IFLAG1 Bit Fields */
#define CAN_IFLAG1_BUF0I_MASK                    0x1u
#define CAN_IFLAG1_BUF0I_SHIFT                   0u
#define CAN_IFLAG1_BUF0I_WIDTH                   1u
#define CAN_IFLAG1_BUF0I(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_IFLAG1_BUF0I_SHIFT))&CAN_IFLAG1_BUF0I_MASK)
#define CAN_IFLAG1_BUF4TO1I_MASK                 0x1Eu
#define CAN_IFLAG1_BUF4TO1I_SHIFT                1u
#define CAN_IFLAG1_BUF4TO1I_WIDTH                4u
#define CAN_IFLAG1_BUF4TO1I(x)                   (((uint32_t)(((uint32_t)(x))<<CAN_IFLAG1_BUF4TO1I_SHIFT))&CAN_IFLAG1_BUF4TO1I_MASK)
#define CAN_IFLAG1_BUF5I_MASK                    0x20u
#define CAN_IFLAG1_BUF5I_SHIFT                   5u
#define CAN_IFLAG1_BUF5I_WIDTH                   1u
#define CAN_IFLAG1_BUF5I(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_IFLAG1_BUF5I_SHIFT))&CAN_IFLAG1_BUF5I_MASK)
#define CAN_IFLAG1_BUF6I_MASK                    0x40u
#define CAN_IFLAG1_BUF6I_SHIFT                   6u
#define CAN_IFLAG1_BUF6I_WIDTH                   1u
#define CAN_IFLAG1_BUF6I(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_IFLAG1_BUF6I_SHIFT))&CAN_IFLAG1_BUF6I_MASK)
#define CAN_IFLAG1_BUF7I_MASK                    0x80u
#define CAN_IFLAG1_BUF7I_SHIFT                   7u
#define CAN_IFLAG1_BUF7I_WIDTH                   1u
#define CAN_IFLAG1_BUF7I(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_IFLAG1_BUF7I_SHIFT))&CAN_IFLAG1_BUF7I_MASK)
#define CAN_IFLAG1_BUF31TO8I_MASK                0xFFFFFF00u
#define CAN_IFLAG1_BUF31TO8I_SHIFT               8u
#define CAN_IFLAG1_BUF31TO8I_WIDTH               24u
#define CAN_IFLAG1_BUF31TO8I(x)                  (((uint32_t)(((uint32_t)(x))<<CAN_IFLAG1_BUF31TO8I_SHIFT))&CAN_IFLAG1_BUF31TO8I_MASK)
/* CTRL2 Bit Fields */
#define CAN_CTRL2_EACEN_MASK                     0x10000u
#define CAN_CTRL2_EACEN_SHIFT                    16u
#define CAN_CTRL2_EACEN_WIDTH                    1u
#define CAN_CTRL2_EACEN(x)                       (((uint32_t)(((uint32_t)(x))<<CAN_CTRL2_EACEN_SHIFT))&CAN_CTRL2_EACEN_MASK)
#define CAN_CTRL2_RRS_MASK                       0x20000u
#define CAN_CTRL2_RRS_SHIFT                      17u
#define CAN_CTRL2_RRS_WIDTH                      1u
#define CAN_CTRL2_RRS(x)                         (((uint32_t)(((uint32_t)(x))<<CAN_CTRL2_RRS_SHIFT))&CAN_CTRL2_RRS_MASK)
#define CAN_CTRL2_MRP_MASK                       0x40000u
#define CAN_CTRL2_MRP_SHIFT                      18u
#define CAN_CTRL2_MRP_WIDTH                      1u
#define CAN_CTRL2_MRP(x)                         (((uint32_t)(((uint32_t)(x))<<CAN_CTRL2_MRP_SHIFT))&CAN_CTRL2_MRP_MASK)
#define CAN_CTRL2_TASD_MASK                      0xF80000u
#define CAN_CTRL2_TASD_SHIFT                     19u
#define CAN_CTRL2_TASD_WIDTH                     5u
#define CAN_CTRL2_TASD(x)                        (((uint32_t)(((uint32_t)(x))<<CAN_CTRL2_TASD_SHIFT))&CAN_CTRL2_TASD_MASK)
#define CAN_CTRL2_RFFN_MASK                      0xF000000u
#define CAN_CTRL2_RFFN_SHIFT                     24u
#define CAN_CTRL2_RFFN_WIDTH                     4u
#define CAN_CTRL2_RFFN(x)                        (((uint32_t)(((uint32_t)(x))<<CAN_CTRL2_RFFN_SHIFT))&CAN_CTRL2_RFFN_MASK)
#define CAN_CTRL2_WRMFRZ_MASK                    0x10000000u
#define CAN_CTRL2_WRMFRZ_SHIFT                   28u
#define CAN_CTRL2_WRMFRZ_WIDTH                   1u
#define CAN_CTRL2_WRMFRZ(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_CTRL2_WRMFRZ_SHIFT))&CAN_CTRL2_WRMFRZ_MASK)
#define CAN_CTRL2_ECRWRE_MASK                    0x20000000u
#define CAN_CTRL2_ECRWRE_SHIFT                   29u
#define CAN_CTRL2_ECRWRE_WIDTH                   1u
#define CAN_CTRL2_ECRWRE(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_CTRL2_ECRWRE_SHIFT))&CAN_CTRL2_ECRWRE_MASK)
/* ESR2 Bit Fields */
#define CAN_ESR2_IMB_MASK                        0x2000u
#define CAN_ESR2_IMB_SHIFT                       13u
#define CAN_ESR2_IMB_WIDTH                       1u
#define CAN_ESR2_IMB(x)                          (((uint32_t)(((uint32_t)(x))<<CAN_ESR2_IMB_SHIFT))&CAN_ESR2_IMB_MASK)
#define CAN_ESR2_VPS_MASK                        0x4000u
#define CAN_ESR2_VPS_SHIFT                       14u
#define CAN_ESR2_VPS_WIDTH                       1u
#define CAN_ESR2_VPS(x)                          (((uint32_t)(((uint32_t)(x))<<CAN_ESR2_VPS_SHIFT))&CAN_ESR2_VPS_MASK)
#define CAN_ESR2_LPTM_MASK                       0x7F0000u
#define CAN_ESR2_LPTM_SHIFT                      16u
#define CAN_ESR2_LPTM_WIDTH                      7u
#define CAN_ESR2_LPTM(x)                         (((uint32_t)(((uint32_t)(x))<<CAN_ESR2_LPTM_SHIFT))&CAN_ESR2_LPTM_MASK)
/* CRCR Bit Fields */
#define CAN_CRCR_TXCRC_MASK                      0x7FFFu
#define CAN_CRCR_TXCRC_SHIFT                     0u
#define CAN_CRCR_TXCRC_WIDTH                     15u
#define CAN_CRCR_TXCRC(x)                        (((uint32_t)(((uint32_t)(x))<<CAN_CRCR_TXCRC_SHIFT))&CAN_CRCR_TXCRC_MASK)
#define CAN_CRCR_MBCRC_MASK                      0x7F0000u
#define CAN_CRCR_MBCRC_SHIFT                     16u
#define CAN_CRCR_MBCRC_WIDTH                     7u
#define CAN_CRCR_MBCRC(x)                        (((uint32_t)(((uint32_t)(x))<<CAN_CRCR_MBCRC_SHIFT))&CAN_CRCR_MBCRC_MASK)
/* RXFGMASK Bit Fields */
#define CAN_RXFGMASK_FGM_MASK                    0xFFFFFFFFu
#define CAN_RXFGMASK_FGM_SHIFT                   0u
#define CAN_RXFGMASK_FGM_WIDTH                   32u
#define CAN_RXFGMASK_FGM(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_RXFGMASK_FGM_SHIFT))&CAN_RXFGMASK_FGM_MASK)
/* RXFIR Bit Fields */
#define CAN_RXFIR_IDHIT_MASK                     0x1FFu
#define CAN_RXFIR_IDHIT_SHIFT                    0u
#define CAN_RXFIR_IDHIT_WIDTH                    9u
#define CAN_RXFIR_IDHIT(x)                       (((uint32_t)(((uint32_t)(x))<<CAN_RXFIR_IDHIT_SHIFT))&CAN_RXFIR_IDHIT_MASK)
/* RAMn Bit Fields */
#define CAN_RAMn_DATA_BYTE_3_MASK                0xFFu
#define CAN_RAMn_DATA_BYTE_3_SHIFT               0u
#define CAN_RAMn_DATA_BYTE_3_WIDTH               8u
#define CAN_RAMn_DATA_BYTE_3(x)                  (((uint32_t)(((uint32_t)(x))<<CAN_RAMn_DATA_BYTE_3_SHIFT))&CAN_RAMn_DATA_BYTE_3_MASK)
#define CAN_RAMn_DATA_BYTE_2_MASK                0xFF00u
#define CAN_RAMn_DATA_BYTE_2_SHIFT               8u
#define CAN_RAMn_DATA_BYTE_2_WIDTH               8u
#define CAN_RAMn_DATA_BYTE_2(x)                  (((uint32_t)(((uint32_t)(x))<<CAN_RAMn_DATA_BYTE_2_SHIFT))&CAN_RAMn_DATA_BYTE_2_MASK)
#define CAN_RAMn_DATA_BYTE_1_MASK                0xFF0000u
#define CAN_RAMn_DATA_BYTE_1_SHIFT               16u
#define CAN_RAMn_DATA_BYTE_1_WIDTH               8u
#define CAN_RAMn_DATA_BYTE_1(x)                  (((uint32_t)(((uint32_t)(x))<<CAN_RAMn_DATA_BYTE_1_SHIFT))&CAN_RAMn_DATA_BYTE_1_MASK)
#define CAN_RAMn_DATA_BYTE_0_MASK                0xFF000000u
#define CAN_RAMn_DATA_BYTE_0_SHIFT               24u
#define CAN_RAMn_DATA_BYTE_0_WIDTH               8u
#define CAN_RAMn_DATA_BYTE_0(x)                  (((uint32_t)(((uint32_t)(x))<<CAN_RAMn_DATA_BYTE_0_SHIFT))&CAN_RAMn_DATA_BYTE_0_MASK)
/* RXIMR Bit Fields */
#define CAN_RXIMR_MI_MASK                        0xFFFFFFFFu
#define CAN_RXIMR_MI_SHIFT                       0u
#define CAN_RXIMR_MI_WIDTH                       32u
#define CAN_RXIMR_MI(x)                          (((uint32_t)(((uint32_t)(x))<<CAN_RXIMR_MI_SHIFT))&CAN_RXIMR_MI_MASK)
/* MECR Bit Fields */
#define CAN_MECR_NCEFAFRZ_MASK                   0x80u
#define CAN_MECR_NCEFAFRZ_SHIFT                  7u
#define CAN_MECR_NCEFAFRZ_WIDTH                  1u
#define CAN_MECR_NCEFAFRZ(x)                     (((uint32_t)(((uint32_t)(x))<<CAN_MECR_NCEFAFRZ_SHIFT))&CAN_MECR_NCEFAFRZ_MASK)
#define CAN_MECR_ECCDIS_MASK                     0x100u
#define CAN_MECR_ECCDIS_SHIFT                    8u
#define CAN_MECR_ECCDIS_WIDTH                    1u
#define CAN_MECR_ECCDIS(x)                       (((uint32_t)(((uint32_t)(x))<<CAN_MECR_ECCDIS_SHIFT))&CAN_MECR_ECCDIS_MASK)
#define CAN_MECR_RERRDIS_MASK                    0x200u
#define CAN_MECR_RERRDIS_SHIFT                   9u
#define CAN_MECR_RERRDIS_WIDTH                   1u
#define CAN_MECR_RERRDIS(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_MECR_RERRDIS_SHIFT))&CAN_MECR_RERRDIS_MASK)
#define CAN_MECR_EXTERRIE_MASK                   0x2000u
#define CAN_MECR_EXTERRIE_SHIFT                  13u
#define CAN_MECR_EXTERRIE_WIDTH                  1u
#define CAN_MECR_EXTERRIE(x)                     (((uint32_t)(((uint32_t)(x))<<CAN_MECR_EXTERRIE_SHIFT))&CAN_MECR_EXTERRIE_MASK)
#define CAN_MECR_FAERRIE_MASK                    0x4000u
#define CAN_MECR_FAERRIE_SHIFT                   14u
#define CAN_MECR_FAERRIE_WIDTH                   1u
#define CAN_MECR_FAERRIE(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_MECR_FAERRIE_SHIFT))&CAN_MECR_FAERRIE_MASK)
#define CAN_MECR_HAERRIE_MASK                    0x8000u
#define CAN_MECR_HAERRIE_SHIFT                   15u
#define CAN_MECR_HAERRIE_WIDTH                   1u
#define CAN_MECR_HAERRIE(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_MECR_HAERRIE_SHIFT))&CAN_MECR_HAERRIE_MASK)
#define CAN_MECR_CEI_MSK_MASK                    0x10000u
#define CAN_MECR_CEI_MSK_SHIFT                   16u
#define CAN_MECR_CEI_MSK_WIDTH                   1u
#define CAN_MECR_CEI_MSK(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_MECR_CEI_MSK_SHIFT))&CAN_MECR_CEI_MSK_MASK)
#define CAN_MECR_FANCEI_MSK_MASK                 0x40000u
#define CAN_MECR_FANCEI_MSK_SHIFT                18u
#define CAN_MECR_FANCEI_MSK_WIDTH                1u
#define CAN_MECR_FANCEI_MSK(x)                   (((uint32_t)(((uint32_t)(x))<<CAN_MECR_FANCEI_MSK_SHIFT))&CAN_MECR_FANCEI_MSK_MASK)
#define CAN_MECR_HANCEI_MSK_MASK                 0x80000u
#define CAN_MECR_HANCEI_MSK_SHIFT                19u
#define CAN_MECR_HANCEI_MSK_WIDTH                1u
#define CAN_MECR_HANCEI_MSK(x)                   (((uint32_t)(((uint32_t)(x))<<CAN_MECR_HANCEI_MSK_SHIFT))&CAN_MECR_HANCEI_MSK_MASK)
#define CAN_MECR_ECRWRDIS_MASK                   0x80000000u
#define CAN_MECR_ECRWRDIS_SHIFT                  31u
#define CAN_MECR_ECRWRDIS_WIDTH                  1u
#define CAN_MECR_ECRWRDIS(x)                     (((uint32_t)(((uint32_t)(x))<<CAN_MECR_ECRWRDIS_SHIFT))&CAN_MECR_ECRWRDIS_MASK)
/* ERRIAR Bit Fields */
#define CAN_ERRIAR_INJADDR_MASK                  0x3FFFu
#define CAN_ERRIAR_INJADDR_SHIFT                 0u
#define CAN_ERRIAR_INJADDR_WIDTH                 14u
#define CAN_ERRIAR_INJADDR(x)                    (((uint32_t)(((uint32_t)(x))<<CAN_ERRIAR_INJADDR_SHIFT))&CAN_ERRIAR_INJADDR_MASK)
/* ERRIDPR Bit Fields */
#define CAN_ERRIDPR_DFLIP_MASK                   0xFFFFFFFFu
#define CAN_ERRIDPR_DFLIP_SHIFT                  0u
#define CAN_ERRIDPR_DFLIP_WIDTH                  32u
#define CAN_ERRIDPR_DFLIP(x)                     (((uint32_t)(((uint32_t)(x))<<CAN_ERRIDPR_DFLIP_SHIFT))&CAN_ERRIDPR_DFLIP_MASK)
/* ERRIPPR Bit Fields */
#define CAN_ERRIPPR_PFLIP0_MASK                  0x1Fu
#define CAN_ERRIPPR_PFLIP0_SHIFT                 0u
#define CAN_ERRIPPR_PFLIP0_WIDTH                 5u
#define CAN_ERRIPPR_PFLIP0(x)                    (((uint32_t)(((uint32_t)(x))<<CAN_ERRIPPR_PFLIP0_SHIFT))&CAN_ERRIPPR_PFLIP0_MASK)
#define CAN_ERRIPPR_PFLIP1_MASK                  0x1F00u
#define CAN_ERRIPPR_PFLIP1_SHIFT                 8u
#define CAN_ERRIPPR_PFLIP1_WIDTH                 5u
#define CAN_ERRIPPR_PFLIP1(x)                    (((uint32_t)(((uint32_t)(x))<<CAN_ERRIPPR_PFLIP1_SHIFT))&CAN_ERRIPPR_PFLIP1_MASK)
#define CAN_ERRIPPR_PFLIP2_MASK                  0x1F0000u
#define CAN_ERRIPPR_PFLIP2_SHIFT                 16u
#define CAN_ERRIPPR_PFLIP2_WIDTH                 5u
#define CAN_ERRIPPR_PFLIP2(x)                    (((uint32_t)(((uint32_t)(x))<<CAN_ERRIPPR_PFLIP2_SHIFT))&CAN_ERRIPPR_PFLIP2_MASK)
#define CAN_ERRIPPR_PFLIP3_MASK                  0x1F000000u
#define CAN_ERRIPPR_PFLIP3_SHIFT                 24u
#define CAN_ERRIPPR_PFLIP3_WIDTH                 5u
#define CAN_ERRIPPR_PFLIP3(x)                    (((uint32_t)(((uint32_t)(x))<<CAN_ERRIPPR_PFLIP3_SHIFT))&CAN_ERRIPPR_PFLIP3_MASK)
/* RERRAR Bit Fields */
#define CAN_RERRAR_ERRADDR_MASK                  0x3FFFu
#define CAN_RERRAR_ERRADDR_SHIFT                 0u
#define CAN_RERRAR_ERRADDR_WIDTH                 14u
#define CAN_RERRAR_ERRADDR(x)                    (((uint32_t)(((uint32_t)(x))<<CAN_RERRAR_ERRADDR_SHIFT))&CAN_RERRAR_ERRADDR_MASK)
#define CAN_RERRAR_SAID_MASK                     0x70000u
#define CAN_RERRAR_SAID_SHIFT                    16u
#define CAN_RERRAR_SAID_WIDTH                    3u
#define CAN_RERRAR_SAID(x)                       (((uint32_t)(((uint32_t)(x))<<CAN_RERRAR_SAID_SHIFT))&CAN_RERRAR_SAID_MASK)
#define CAN_RERRAR_NCE_MASK                      0x1000000u
#define CAN_RERRAR_NCE_SHIFT                     24u
#define CAN_RERRAR_NCE_WIDTH                     1u
#define CAN_RERRAR_NCE(x)                        (((uint32_t)(((uint32_t)(x))<<CAN_RERRAR_NCE_SHIFT))&CAN_RERRAR_NCE_MASK)
/* RERRDR Bit Fields */
#define CAN_RERRDR_RDATA_MASK                    0xFFFFFFFFu
#define CAN_RERRDR_RDATA_SHIFT                   0u
#define CAN_RERRDR_RDATA_WIDTH                   32u
#define CAN_RERRDR_RDATA(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_RERRDR_RDATA_SHIFT))&CAN_RERRDR_RDATA_MASK)
/* RERRSYNR Bit Fields */
#define CAN_RERRSYNR_SYND0_MASK                  0x1Fu
#define CAN_RERRSYNR_SYND0_SHIFT                 0u
#define CAN_RERRSYNR_SYND0_WIDTH                 5u
#define CAN_RERRSYNR_SYND0(x)                    (((uint32_t)(((uint32_t)(x))<<CAN_RERRSYNR_SYND0_SHIFT))&CAN_RERRSYNR_SYND0_MASK)
#define CAN_RERRSYNR_BE0_MASK                    0x80u
#define CAN_RERRSYNR_BE0_SHIFT                   7u
#define CAN_RERRSYNR_BE0_WIDTH                   1u
#define CAN_RERRSYNR_BE0(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_RERRSYNR_BE0_SHIFT))&CAN_RERRSYNR_BE0_MASK)
#define CAN_RERRSYNR_SYND1_MASK                  0x1F00u
#define CAN_RERRSYNR_SYND1_SHIFT                 8u
#define CAN_RERRSYNR_SYND1_WIDTH                 5u
#define CAN_RERRSYNR_SYND1(x)                    (((uint32_t)(((uint32_t)(x))<<CAN_RERRSYNR_SYND1_SHIFT))&CAN_RERRSYNR_SYND1_MASK)
#define CAN_RERRSYNR_BE1_MASK                    0x8000u
#define CAN_RERRSYNR_BE1_SHIFT                   15u
#define CAN_RERRSYNR_BE1_WIDTH                   1u
#define CAN_RERRSYNR_BE1(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_RERRSYNR_BE1_SHIFT))&CAN_RERRSYNR_BE1_MASK)
#define CAN_RERRSYNR_SYND2_MASK                  0x1F0000u
#define CAN_RERRSYNR_SYND2_SHIFT                 16u
#define CAN_RERRSYNR_SYND2_WIDTH                 5u
#define CAN_RERRSYNR_SYND2(x)                    (((uint32_t)(((uint32_t)(x))<<CAN_RERRSYNR_SYND2_SHIFT))&CAN_RERRSYNR_SYND2_MASK)
#define CAN_RERRSYNR_BE2_MASK                    0x800000u
#define CAN_RERRSYNR_BE2_SHIFT                   23u
#define CAN_RERRSYNR_BE2_WIDTH                   1u
#define CAN_RERRSYNR_BE2(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_RERRSYNR_BE2_SHIFT))&CAN_RERRSYNR_BE2_MASK)
#define CAN_RERRSYNR_SYND3_MASK                  0x1F000000u
#define CAN_RERRSYNR_SYND3_SHIFT                 24u
#define CAN_RERRSYNR_SYND3_WIDTH                 5u
#define CAN_RERRSYNR_SYND3(x)                    (((uint32_t)(((uint32_t)(x))<<CAN_RERRSYNR_SYND3_SHIFT))&CAN_RERRSYNR_SYND3_MASK)
#define CAN_RERRSYNR_BE3_MASK                    0x80000000u
#define CAN_RERRSYNR_BE3_SHIFT                   31u
#define CAN_RERRSYNR_BE3_WIDTH                   1u
#define CAN_RERRSYNR_BE3(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_RERRSYNR_BE3_SHIFT))&CAN_RERRSYNR_BE3_MASK)
/* ERRSR Bit Fields */
#define CAN_ERRSR_CEIOF_MASK                     0x1u
#define CAN_ERRSR_CEIOF_SHIFT                    0u
#define CAN_ERRSR_CEIOF_WIDTH                    1u
#define CAN_ERRSR_CEIOF(x)                       (((uint32_t)(((uint32_t)(x))<<CAN_ERRSR_CEIOF_SHIFT))&CAN_ERRSR_CEIOF_MASK)
#define CAN_ERRSR_FANCEIOF_MASK                  0x4u
#define CAN_ERRSR_FANCEIOF_SHIFT                 2u
#define CAN_ERRSR_FANCEIOF_WIDTH                 1u
#define CAN_ERRSR_FANCEIOF(x)                    (((uint32_t)(((uint32_t)(x))<<CAN_ERRSR_FANCEIOF_SHIFT))&CAN_ERRSR_FANCEIOF_MASK)
#define CAN_ERRSR_HANCEIOF_MASK                  0x8u
#define CAN_ERRSR_HANCEIOF_SHIFT                 3u
#define CAN_ERRSR_HANCEIOF_WIDTH                 1u
#define CAN_ERRSR_HANCEIOF(x)                    (((uint32_t)(((uint32_t)(x))<<CAN_ERRSR_HANCEIOF_SHIFT))&CAN_ERRSR_HANCEIOF_MASK)
#define CAN_ERRSR_CEIF_MASK                      0x10000u
#define CAN_ERRSR_CEIF_SHIFT                     16u
#define CAN_ERRSR_CEIF_WIDTH                     1u
#define CAN_ERRSR_CEIF(x)                        (((uint32_t)(((uint32_t)(x))<<CAN_ERRSR_CEIF_SHIFT))&CAN_ERRSR_CEIF_MASK)
#define CAN_ERRSR_FANCEIF_MASK                   0x40000u
#define CAN_ERRSR_FANCEIF_SHIFT                  18u
#define CAN_ERRSR_FANCEIF_WIDTH                  1u
#define CAN_ERRSR_FANCEIF(x)                     (((uint32_t)(((uint32_t)(x))<<CAN_ERRSR_FANCEIF_SHIFT))&CAN_ERRSR_FANCEIF_MASK)
#define CAN_ERRSR_HANCEIF_MASK                   0x80000u
#define CAN_ERRSR_HANCEIF_SHIFT                  19u
#define CAN_ERRSR_HANCEIF_WIDTH                  1u
#define CAN_ERRSR_HANCEIF(x)                     (((uint32_t)(((uint32_t)(x))<<CAN_ERRSR_HANCEIF_SHIFT))&CAN_ERRSR_HANCEIF_MASK)

/*!
 * @}
 */ /* end of group CAN_Register_Masks */


/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*! @brief The type of the RxFIFO transfer (interrupts/DMA).
 * Implements : flexcan_rxfifo_transfer_type_t_Class
 */
typedef enum {
    FLEXCAN_RXFIFO_USING_INTERRUPTS,    /*!< Use interrupts for RxFIFO. */
#if FEATURE_CAN_HAS_DMA_ENABLE
    FLEXCAN_RXFIFO_USING_DMA            /*!< Use DMA for RxFIFO. */
#endif
} flexcan_rxfifo_transfer_type_t;

/*! @brief The type of the event which occurred when the callback was invoked.
 * Implements : flexcan_event_type_t_Class
 */
typedef enum {
    FLEXCAN_EVENT_RX_COMPLETE,     /*!< A frame was received in the configured Rx MB. */
    FLEXCAN_EVENT_RXFIFO_COMPLETE, /*!< A frame was received in the Rx FIFO. */
    FLEXCAN_EVENT_RXFIFO_WARNING,  /*!< Rx FIFO is almost full (5 frames). */
    FLEXCAN_EVENT_RXFIFO_OVERFLOW, /*!< Rx FIFO is full (incoming message was lost). */
    FLEXCAN_EVENT_TX_COMPLETE,     /*!< A frame was sent from the configured Tx MB. */
#if FEATURE_CAN_HAS_WAKE_UP_IRQ
    FLEXCAN_EVENT_WAKEUP_TIMEOUT,  /*!< An wake up event occurred due to timeout. */
    FLEXCAN_EVENT_WAKEUP_MATCH,    /*!< An wake up event occurred due to matching. */
#endif /* FEATURE_CAN_HAS_WAKE_UP_IRQ */
#if FEATURE_CAN_HAS_DMA_ENABLE
	FLEXCAN_EVENT_DMA_COMPLETE,	  /*!< A complete transfer occurred on DMA */
	FLEXCAN_EVENT_DMA_ERROR,	  /*!< A DMA transfer fail, because of a DMA channel error */
#endif /* FEATURE_CAN_HAS_DMA_ENABLE */
    FLEXCAN_EVENT_ERROR
} flexcan_event_type_t;

/*! @brief The state of a given MB (idle/Rx busy/Tx busy).
 * Implements : flexcan_mb_state_t_Class
 */
typedef enum {
    FLEXCAN_MB_IDLE,      /*!< The MB is not used by any transfer. */
    FLEXCAN_MB_RX_BUSY,   /*!< The MB is used for a reception. */
    FLEXCAN_MB_TX_BUSY,   /*!< The MB is used for a transmission. */
#if FEATURE_CAN_HAS_DMA_ENABLE
	FLEXCAN_MB_DMA_ERROR /*!< The MB is used as DMA source and fail to transfer */
#endif
} flexcan_mb_state_t;

/*! @brief FlexCAN Message Buffer ID type
 * Implements : flexcan_msgbuff_id_type_t_Class
 */
typedef enum {
    FLEXCAN_MSG_ID_STD,         /*!< Standard ID*/
    FLEXCAN_MSG_ID_EXT          /*!< Extended ID*/
} flexcan_msgbuff_id_type_t;

#if FEATURE_CAN_HAS_PE_CLKSRC_SELECT
/*! @brief FlexCAN PE clock sources
 * Implements : flexcan_clk_source_t_Class
 */
typedef enum {
    FLEXCAN_CLK_SOURCE_OSC    = 0U,  /*!< The CAN engine clock source is the oscillator clock. */
    FLEXCAN_CLK_SOURCE_PERIPH = 1U   /*!< The CAN engine clock source is the peripheral clock. */
} flexcan_clk_source_t;
#endif

/*! @brief FlexCAN message buffer structure
 * Implements : flexcan_msgbuff_t_Class
 */
typedef struct {
    uint32_t cs;                        /*!< Code and Status*/
    uint32_t msgId;                     /*!< Message Buffer ID*/
    uint8_t data[64];                   /*!< Data bytes of the FlexCAN message*/
    uint8_t dataLen;                    /*!< Length of data in bytes */
} flexcan_msgbuff_t;

/*! @brief Information needed for internal handling of a given MB.
 * Implements : flexcan_mb_handle_t_Class
 */
typedef struct {
    flexcan_msgbuff_t *mb_message;       /*!< The FlexCAN MB structure */
    semaphore_t mbSema;                  /*!< Semaphore used for signaling completion of a blocking transfer */
    volatile flexcan_mb_state_t state;   /*!< The state of the current MB (idle/Rx busy/Tx busy) */
    bool isBlocking;                     /*!< True if the transfer is blocking */
    bool isRemote;                       /*!< True if the frame is a remote frame */
} flexcan_mb_handle_t;

/*!
 * @brief Internal driver state information.
 *
 * @note The contents of this structure are internal to the driver and should not be
 *      modified by users. Also, contents of the structure are subject to change in
 *      future releases.
 * Implements : flexcan_state_t_Class
 */
typedef struct FlexCANState {
    flexcan_mb_handle_t mbs[FEATURE_CAN_MAX_MB_NUM];           /*!< Array containing information
                                                                    related to each MB */
    void (*callback)(uint8_t instance,
                     flexcan_event_type_t eventType,
                     uint32_t buffIdx,
                     struct FlexCANState *driverState);        /*!< IRQ handler callback function. */
    void *callbackParam;                                       /*!< Parameter used to pass user data
                                                                    when invoking the callback
                                                                    function. */
    void (*error_callback)(uint8_t instance,
                           flexcan_event_type_t eventType,
                           struct FlexCANState *driverState);  /*!< Error IRQ handler callback
                                                                    function. */
    void *errorCallbackParam;                                  /*!< Parameter used to pass user data
                                                                    when invoking the error callback
                                                                    function. */
#if FEATURE_CAN_HAS_DMA_ENABLE
    uint8_t rxFifoDMAChannel;                                  /*!< DMA channel number used for
                                                                    transfers. */
#endif
    flexcan_rxfifo_transfer_type_t transferType;               /*!< Type of RxFIFO transfer. */
} flexcan_state_t;

/*! @brief FlexCAN data info from user
 * Implements : flexcan_data_info_t_Class
 */
typedef struct {
    flexcan_msgbuff_id_type_t msg_id_type;  /*!< Type of message ID (standard or extended)*/
    uint32_t data_length;                   /*!< Length of Data in Bytes*/
#if FEATURE_CAN_HAS_FD
    bool fd_enable;                         /*!< Enable or disable FD*/
    uint8_t fd_padding;                     /*!< Set a value for padding. It will be used when the data length code (DLC)
                                                 specifies a bigger payload size than data_length to fill the MB */
    bool enable_brs;                        /*!< Enable bit rate switch inside a CAN FD format frame*/
#endif
    bool is_remote;                         /*!< Specifies if the frame is standard or remote */
} flexcan_data_info_t;

/*! @brief FlexCAN Rx FIFO filters number
 * Implements : flexcan_rx_fifo_id_filter_num_t_Class
 */
typedef enum {
    FLEXCAN_RX_FIFO_ID_FILTERS_8   = 0x0,         /*!<   8 Rx FIFO Filters. @internal gui name="8 Rx FIFO Filters" */
    FLEXCAN_RX_FIFO_ID_FILTERS_16  = 0x1,         /*!<  16 Rx FIFO Filters. @internal gui name="16 Rx FIFO Filters" */
    FLEXCAN_RX_FIFO_ID_FILTERS_24  = 0x2,         /*!<  24 Rx FIFO Filters. @internal gui name="24 Rx FIFO Filters" */
    FLEXCAN_RX_FIFO_ID_FILTERS_32  = 0x3,         /*!<  32 Rx FIFO Filters. @internal gui name="32 Rx FIFO Filters" */
    FLEXCAN_RX_FIFO_ID_FILTERS_40  = 0x4,         /*!<  40 Rx FIFO Filters. @internal gui name="40 Rx FIFO Filters" */
    FLEXCAN_RX_FIFO_ID_FILTERS_48  = 0x5,         /*!<  48 Rx FIFO Filters. @internal gui name="48 Rx FIFO Filters" */
    FLEXCAN_RX_FIFO_ID_FILTERS_56  = 0x6,         /*!<  56 Rx FIFO Filters. @internal gui name="56 Rx FIFO Filters" */
    FLEXCAN_RX_FIFO_ID_FILTERS_64  = 0x7,         /*!<  64 Rx FIFO Filters. @internal gui name="64 Rx FIFO Filters" */
    FLEXCAN_RX_FIFO_ID_FILTERS_72  = 0x8,         /*!<  72 Rx FIFO Filters. @internal gui name="72 Rx FIFO Filters" */
    FLEXCAN_RX_FIFO_ID_FILTERS_80  = 0x9,         /*!<  80 Rx FIFO Filters. @internal gui name="80 Rx FIFO Filters" */
    FLEXCAN_RX_FIFO_ID_FILTERS_88  = 0xA,         /*!<  88 Rx FIFO Filters. @internal gui name="88 Rx FIFO Filters" */
    FLEXCAN_RX_FIFO_ID_FILTERS_96  = 0xB,         /*!<  96 Rx FIFO Filters. @internal gui name="96 Rx FIFO Filters" */
    FLEXCAN_RX_FIFO_ID_FILTERS_104 = 0xC,         /*!< 104 Rx FIFO Filters. @internal gui name="104 Rx FIFO Filters" */
    FLEXCAN_RX_FIFO_ID_FILTERS_112 = 0xD,         /*!< 112 Rx FIFO Filters. @internal gui name="112 Rx FIFO Filters" */
    FLEXCAN_RX_FIFO_ID_FILTERS_120 = 0xE,         /*!< 120 Rx FIFO Filters. @internal gui name="120 Rx FIFO Filters" */
    FLEXCAN_RX_FIFO_ID_FILTERS_128 = 0xF          /*!< 128 Rx FIFO Filters. @internal gui name="128 Rx FIFO Filters" */
} flexcan_rx_fifo_id_filter_num_t;

/*! @brief FlexCAN Rx mask type.
 * Implements : flexcan_rx_mask_type_t_Class
 */
typedef enum {
    FLEXCAN_RX_MASK_GLOBAL,      /*!< Rx global mask*/
    FLEXCAN_RX_MASK_INDIVIDUAL   /*!< Rx individual mask*/
} flexcan_rx_mask_type_t;

/*! @brief ID formats for Rx FIFO
 * Implements : flexcan_rx_fifo_id_element_format_t_Class
 */
typedef enum {
    FLEXCAN_RX_FIFO_ID_FORMAT_A, /*!< One full ID (standard and extended) per ID Filter Table element.*/
    FLEXCAN_RX_FIFO_ID_FORMAT_B, /*!< Two full standard IDs or two partial 14-bit (standard and
                                      extended) IDs per ID Filter Table element.*/
    FLEXCAN_RX_FIFO_ID_FORMAT_C, /*!< Four partial 8-bit Standard IDs per ID Filter Table element.*/
    FLEXCAN_RX_FIFO_ID_FORMAT_D  /*!< All frames rejected.*/
} flexcan_rx_fifo_id_element_format_t;

/*! @brief FlexCAN Rx FIFO ID filter table structure
 * Implements : flexcan_id_table_t_Class
 */
typedef struct {
    bool isRemoteFrame;      /*!< Remote frame*/
    bool isExtendedFrame;    /*!< Extended frame*/
    uint32_t id;             /*!< Rx FIFO ID filter element*/
} flexcan_id_table_t;

/*! @brief FlexCAN operation modes
 * Implements : flexcan_operation_modes_t_Class
 */
typedef enum {
    FLEXCAN_NORMAL_MODE,        /*!< Normal mode or user mode @internal gui name="Normal" */
    FLEXCAN_LISTEN_ONLY_MODE,   /*!< Listen-only mode @internal gui name="Listen-only" */
    FLEXCAN_LOOPBACK_MODE,      /*!< Loop-back mode @internal gui name="Loop back" */
    FLEXCAN_FREEZE_MODE,        /*!< Freeze mode @internal gui name="Freeze" */
    FLEXCAN_DISABLE_MODE        /*!< Module disable mode @internal gui name="Disabled" */
} flexcan_operation_modes_t;

#if FEATURE_CAN_HAS_FD
/*! @brief FlexCAN payload sizes
 * Implements : flexcan_fd_payload_size_t_Class
 */
typedef enum {
    FLEXCAN_PAYLOAD_SIZE_8 = 0,  /*!< FlexCAN message buffer payload size in bytes*/
    FLEXCAN_PAYLOAD_SIZE_16 ,    /*!< FlexCAN message buffer payload size in bytes*/
    FLEXCAN_PAYLOAD_SIZE_32 ,    /*!< FlexCAN message buffer payload size in bytes*/
    FLEXCAN_PAYLOAD_SIZE_64      /*!< FlexCAN message buffer payload size in bytes*/
} flexcan_fd_payload_size_t;
#endif

/*! @brief FlexCAN bitrate related structures
 * Implements : flexcan_time_segment_t_Class
 */
typedef struct {
    uint32_t propSeg;         /*!< Propagation segment*/
    uint32_t phaseSeg1;       /*!< Phase segment 1*/
    uint32_t phaseSeg2;       /*!< Phase segment 2*/
    uint32_t preDivider;      /*!< Clock prescaler division factor*/
    uint32_t rJumpwidth;      /*!< Resync jump width*/
} flexcan_time_segment_t;

/*! @brief FlexCAN configuration
 * @internal gui name="Common configuration" id="flexcanCfg"
 * Implements : flexcan_user_config_t_Class
 */
typedef struct {
    uint32_t max_num_mb;                            /*!< The maximum number of Message Buffers
                                                         @internal gui name="Maximum number of message buffers" id="max_num_mb" */
    flexcan_rx_fifo_id_filter_num_t num_id_filters; /*!< The number of RX FIFO ID filters needed
                                                         @internal gui name="Number of RX FIFO ID filters" id="num_id_filters" */
    bool is_rx_fifo_needed;                         /*!< 1 if needed; 0 if not. This controls whether the Rx FIFO feature is enabled or not.
                                                         @internal gui name="Use rx fifo" id="is_rx_fifo_needed" */
    flexcan_operation_modes_t flexcanMode;          /*!< User configurable FlexCAN operation modes.
                                                         @internal gui name="Flexcan Operation Mode" id="flexcanMode"*/
#if FEATURE_CAN_HAS_FD
    flexcan_fd_payload_size_t payload;              /*!< The payload size of the mailboxes specified in bytes. */
    bool fd_enable;                                 /*!< Enable/Disable the Flexible Data Rate feature. */
#endif
#if FEATURE_CAN_HAS_PE_CLKSRC_SELECT
    flexcan_clk_source_t pe_clock;                  /*!< The clock source of the CAN Protocol Engine (PE). */
#endif
    flexcan_time_segment_t bitrate;                 /*!< The bitrate used for standard frames or for the arbitration phase of FD frames. */
#if FEATURE_CAN_HAS_FD
    flexcan_time_segment_t bitrate_cbt;             /*!< The bitrate used for the data phase of FD frames. */
#endif
    flexcan_rxfifo_transfer_type_t transfer_type;   /*!< Specifies if the Rx FIFO uses interrupts or DMA. */
#if FEATURE_CAN_HAS_DMA_ENABLE
    uint8_t rxFifoDMAChannel;                       /*!< Specifies the DMA channel number to be used for DMA transfers. */
#endif
} flexcan_user_config_t;

#if FEATURE_CAN_HAS_PRETENDED_NETWORKING

/*! @brief Pretended Networking ID filter */
typedef struct {
    bool extendedId;    /*!< Specifies if the ID is standard or extended. */
    bool remoteFrame;   /*!< Specifies if the frame is standard or remote. */
    uint32_t id;        /*!< Specifies the ID value. */
} flexcan_pn_id_filter_t;

/*! @brief Pretended Networking payload filter */
typedef struct {
    uint8_t dlcLow;       /*!< Specifies the lower limit of the payload size. */
    uint8_t dlcHigh;      /*!< Specifies the upper limit of the payload size. */
    uint8_t payload1[8U]; /*!< Specifies the payload to be matched (for MATCH_EXACT), the lower limit
                              (for MATCH_GEQ and MATCH_RANGE) or the upper limit (for MATCH_LEQ). */
    uint8_t payload2[8U]; /*!< Specifies the mask (for MATCH_EXACT) or the upper limit (for MATCH_RANGE). */
} flexcan_pn_payload_filter_t;

/*! @brief Pretended Networking filtering combinations */
typedef enum {
    FLEXCAN_FILTER_ID,                  /*!< Message ID filtering only */
    FLEXCAN_FILTER_ID_PAYLOAD,          /*!< Message ID and payload filtering */
    FLEXCAN_FILTER_ID_NTIMES,           /*!< Message ID filtering occurring a specified number of times */
    FLEXCAN_FILTER_ID_PAYLOAD_NTIMES    /*!< Message ID and payload filtering  occurring a specified number of times */
} flexcan_pn_filter_combination_t;

/*! @brief Pretended Networking matching schemes */
typedef enum {
    FLEXCAN_FILTER_MATCH_EXACT,   /*!< Match an exact target value. */
    FLEXCAN_FILTER_MATCH_GEQ,     /*!< Match greater than or equal to a specified target value. */
    FLEXCAN_FILTER_MATCH_LEQ,     /*!< Match less than or equal to a specified target value. */
    FLEXCAN_FILTER_MATCH_RANGE    /*!< Match inside a range, greater than or equal to a specified lower limit and smaller than or
                                      equal to a specified upper limit. */
} flexcan_pn_filter_selection_t;

/*! @brief Pretended Networking configuration structure
 * Implements : flexcan_pn_config_t_Class
 */
typedef struct {
    bool wakeUpTimeout;                               /*!< Specifies if an wake up event is triggered on timeout. */
    bool wakeUpMatch;                                 /*!< Specifies if an wake up event is triggered on match. */
    uint16_t numMatches;                              /*!< The number of matches needed before generating an wake up event. */
    uint16_t matchTimeout;                            /*!< Defines a timeout value that generates an wake up event if wakeUpTimeout is true. */
    flexcan_pn_filter_combination_t filterComb;       /*!< Defines the filtering scheme used. */
    flexcan_pn_id_filter_t idFilter1;                 /*!< The configuration of the first ID filter (match exact / lower limit / upper limit). */
    flexcan_pn_id_filter_t idFilter2;                 /*!< The configuration of the second ID filter (mask / upper limit). */
    flexcan_pn_filter_selection_t idFilterType;       /*!< Defines the ID filtering scheme. */
    flexcan_pn_filter_selection_t payloadFilterType;  /*!< Defines the payload filtering scheme. */
    flexcan_pn_payload_filter_t payloadFilter;        /*!< The configuration of the payload filter. */
} flexcan_pn_config_t;

#endif /* FEATURE_CAN_HAS_PRETENDED_NETWORKING */

/*! @brief FlexCAN Driver callback function type
 * Implements : flexcan_callback_t_Class
 */
typedef void (*flexcan_callback_t)(uint8_t instance, flexcan_event_type_t eventType,
                                   uint32_t buffIdx, flexcan_state_t *flexcanState);

/*! @brief FlexCAN Driver error callback function type
 * Implements : flexcan_error_callback_t_Class
 */
typedef void (*flexcan_error_callback_t)(uint8_t instance, flexcan_event_type_t eventType,
                                         flexcan_state_t *flexcanState);

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Bit rate
 * @{
 */

/*!
 * @brief Sets the FlexCAN bit rate for standard frames or the arbitration phase of FD frames.
 *
 * @param   instance    A FlexCAN instance number
 * @param   bitrate     A pointer to the FlexCAN bit rate settings.
 */
void FLEXCAN_DRV_SetBitrate(uint8_t instance, const flexcan_time_segment_t *bitrate);

#if FEATURE_CAN_HAS_FD
/*!
 * @brief Sets the FlexCAN bit rate for the data phase of FD frames (BRS enabled).
 *
 * @param   instance    A FlexCAN instance number
 * @param   bitrate     A pointer to the FlexCAN bit rate settings.
 */
void FLEXCAN_DRV_SetBitrateCbt(uint8_t instance, const flexcan_time_segment_t *bitrate);
#endif

/*!
 * @brief Gets the FlexCAN bit rate for standard frames or the arbitration phase of FD frames.
 *
 * @param   instance    A FlexCAN instance number
 * @param   bitrate     A pointer to a variable for returning the FlexCAN bit rate settings
 */
void FLEXCAN_DRV_GetBitrate(uint8_t instance, flexcan_time_segment_t *bitrate);

#if FEATURE_CAN_HAS_FD
/*!
 * @brief Gets the FlexCAN bit rate for the data phase of FD frames (BRS enabled).
 *
 * @param   instance    A FlexCAN instance number
 * @param   bitrate     A pointer to a variable for returning the FlexCAN bit rate settings
 */
void FLEXCAN_DRV_GetBitrateFD(uint8_t instance, flexcan_time_segment_t *bitrate);
#endif

/*@}*/

/*!
 * @name Rx MB and Rx FIFO masks
 * @{
 */

/*!
 * @brief Sets the Rx masking type.
 *
 * @param   instance     A FlexCAN instance number
 * @param   type         The FlexCAN RX mask type
 */
void FLEXCAN_DRV_SetRxMaskType(uint8_t instance, flexcan_rx_mask_type_t type);

/*!
 * @brief Sets the FlexCAN Rx FIFO global mask (standard or extended).
 * This mask is applied to all filters ID regardless the ID Filter format.
 *
 * @param   instance    A FlexCAN instance number
 * @param   id_type     Standard ID or extended ID mask type
 * @param   mask        Mask Value. In FIFO mode, when using ID Format A or B, 
                        bit 31 encodes RTR check and bit 30 encodes IDE check respectively. 
                        For ID Format C, bits 31 and 30 are ignored.
 */
void FLEXCAN_DRV_SetRxFifoGlobalMask(
    uint8_t instance,
    flexcan_msgbuff_id_type_t id_type,
    uint32_t mask);

/*!
 * @brief Sets the FlexCAN Rx MB global mask (standard or extended).
 *
 * @param   instance    A FlexCAN instance number
 * @param   id_type     Standard ID or extended ID
 * @param   mask        Mask value
 */
void FLEXCAN_DRV_SetRxMbGlobalMask(
    uint8_t instance,
    flexcan_msgbuff_id_type_t id_type,
    uint32_t mask);

/*!
 * @brief Sets the FlexCAN Rx MB 14 mask (standard or extended).
 *
 * @param   instance    A FlexCAN instance number
 * @param   id_type     Standard ID or extended ID
 * @param   mask        Mask value
 */
void FLEXCAN_DRV_SetRxMb14Mask(
    uint8_t instance,
    flexcan_msgbuff_id_type_t id_type,
    uint32_t mask);

/*!
 * @brief Sets the FlexCAN Rx MB 15 mask (standard or extended).
 *
 * @param   instance    A FlexCAN instance number
 * @param   id_type     Standard ID or extended ID
 * @param   mask        Mask value
 */
void FLEXCAN_DRV_SetRxMb15Mask(
    uint8_t instance,
    flexcan_msgbuff_id_type_t id_type,
    uint32_t mask);

/*!
 * @brief Sets the FlexCAN Rx individual mask (standard or extended).
 *
 * @param   instance  A FlexCAN instance number
 * @param   id_type   A standard ID or an extended ID
 * @param   mb_idx    Index of the message buffer
 * @param   mask      Mask Value. In FIFO mode, when using ID Format A or B, 
                      bit 31 encodes RTR check and bit 30 encodes IDE check respectively. 
                      For ID Format C, bits 31 and 30 are ignored.
 * @return  STATUS_SUCCESS if successful;
 *          STATUS_CAN_BUFF_OUT_OF_RANGE if the index of the
 *          message buffer is invalid.
 */
status_t FLEXCAN_DRV_SetRxIndividualMask(
    uint8_t instance,
    flexcan_msgbuff_id_type_t id_type,
    uint8_t mb_idx,
    uint32_t mask);
    
/*@}*/
 
/*!
 * @name Initialization and Shutdown
 * @{
 */

/*!
 * @brief Gets the default configuration structure
 *
 * This function gets the default configuration structure, with the following settings:
 * - 16 message buffers
 * - flexible data rate disabled
 * - Rx FIFO disabled
 * - normal operation mode
 * - 8 byte payload size
 * - Protocol Engine clock = Oscillator clock
 * - bitrate of 500 Kbit/s (computed for sample point = 87.5)
 *
 * @param[out] config The configuration structure
 */
void FLEXCAN_DRV_GetDefaultConfig(flexcan_user_config_t *config);

/*!
 * @brief Initializes the FlexCAN peripheral.
 *
 * This function initializes
 * @param   instance                   A FlexCAN instance number
 * @param   state                      Pointer to the FlexCAN driver state structure.
 * @param   data                       The FlexCAN platform data
 * @return  STATUS_SUCCESS if successful;
 *          STATUS_CAN_BUFF_OUT_OF_RANGE if the index of a message buffer is invalid;
 *          STATUS_ERROR if other error occurred
 */
status_t FLEXCAN_DRV_Init(
       uint8_t instance,
       flexcan_state_t *state,
       const flexcan_user_config_t *data);

/*!
 * @brief Shuts down a FlexCAN instance.
 *
 * @param   instance    A FlexCAN instance number
 * @return  STATUS_SUCCESS if successful;
 *          STATUS_ERROR if failed
 */
status_t FLEXCAN_DRV_Deinit(uint8_t instance);

#if FEATURE_CAN_HAS_FD
/*!
 * @brief Enables/Disables the Transceiver Delay Compensation feature and sets
 * the Transceiver Delay Compensation Offset (offset value to be added to the
 * measured transceiver's loop delay in order to define the position of the
 * delayed comparison point when bit rate switching is active).
 *
 * @param   instance    A FlexCAN instance number
 * @param   enable Enable/Disable Transceiver Delay Compensation
 * @param   offset Transceiver Delay Compensation Offset
 */
void FLEXCAN_DRV_SetTDCOffset(uint8_t instance, bool enable, uint8_t offset);

/*!
 * @brief Gets the value of the Transceiver Delay Compensation.
 *
 * @param   base  The FlexCAN base address
 * @return  The value of the transceiver loop delay measured from the transmitted
 * EDL to R0 transition edge to the respective received one added to the TDCOFF
 * value specified by FLEXCAN_HAL_SetTDCOffset.
 */
uint8_t FLEXCAN_DRV_GetTDCValue(uint8_t instance);

/*!
 * @brief Gets the value of the TDC Fail flag.
 *
 * @param   base  The FlexCAN base address
 * @return  If true, indicates that the TDC mechanism is out of range, unable to
 * compensate the transceiver's loop delay and successfully compare the delayed
 * received bits to the transmitted ones.
 */
bool FLEXCAN_DRV_GetTDCFail(uint8_t instance);

/*!
 * @brief Clears the TDC Fail flag.
 *
 * @param   base  The FlexCAN base address
 */
void FLEXCAN_DRV_ClearTDCFail(uint8_t instance);
#endif

/*@}*/

/*!
 * @name Send configuration
 * @{
 */

/*!
 * @brief FlexCAN transmit message buffer field configuration.
 *
 * @param   instance                   A FlexCAN instance number
 * @param   mb_idx                     Index of the message buffer
 * @param   tx_info                    Data info
 * @param   msg_id                     ID of the message to transmit
 * @return  STATUS_SUCCESS if successful;
 *          STATUS_CAN_BUFF_OUT_OF_RANGE if the index of the message buffer is invalid
 */
status_t FLEXCAN_DRV_ConfigTxMb(
    uint8_t instance,
    uint8_t mb_idx,
    const flexcan_data_info_t *tx_info,
    uint32_t msg_id);

/*!
 * @brief Configures a transmit message buffer for remote frame response.
 *
 * @param   instance                   A FlexCAN instance number
 * @param   mb_idx                     Index of the message buffer
 * @param   tx_info                    Data info
 * @param   msg_id                     ID of the message to transmit
 * @param   mb_data                    Bytes of the FlexCAN message
 * @return  STATUS_SUCCESS if successful;
 *          STATUS_CAN_BUFF_OUT_OF_RANGE if the index of the message buffer
 *          is invalid
 */
status_t FLEXCAN_DRV_ConfigRemoteResponseMb(
    uint8_t instance,
    uint8_t mb_idx,
    const flexcan_data_info_t *tx_info,
    uint32_t msg_id,
    const uint8_t *mb_data);

/*!
 * @brief Sends a CAN frame using the specified message buffer, in a blocking manner.
 *
 * This function sends a CAN frame using a configured message buffer. The function
 * blocks until either the frame was sent, or the specified timeout expired.
 *
 * @param   instance   A FlexCAN instance number
 * @param   mb_idx     Index of the message buffer
 * @param   tx_info    Data info
 * @param   msg_id     ID of the message to transmit
 * @param   mb_data    Bytes of the FlexCAN message
 * @param   timeout_ms A timeout for the transfer in milliseconds.
 * @return  STATUS_SUCCESS if successful;
 *          STATUS_CAN_BUFF_OUT_OF_RANGE if the index of a message buffer is invalid;
 *          STATUS_BUSY if a resource is busy;
 *          STATUS_TIMEOUT if the timeout is reached
 */
status_t FLEXCAN_DRV_SendBlocking(
    uint8_t instance,
    uint8_t mb_idx,
    const flexcan_data_info_t *tx_info,
    uint32_t msg_id,
    const uint8_t *mb_data,
    uint32_t timeout_ms);

/*!
 * @brief Sends a CAN frame using the specified message buffer.
 *
 * This function sends a CAN frame using a configured message buffer. The function
 * returns immediately. If a callback is installed, it will be invoked after
 * the frame was sent.
 *
 * @param   instance   A FlexCAN instance number
 * @param   mb_idx     Index of the message buffer
 * @param   tx_info    Data info
 * @param   msg_id     ID of the message to transmit
 * @param   mb_data    Bytes of the FlexCAN message.
 * @return  STATUS_SUCCESS if successful;
 *          STATUS_CAN_BUFF_OUT_OF_RANGE if the index of a message buffer is invalid;
 *          STATUS_BUSY if a resource is busy
 */
status_t FLEXCAN_DRV_Send(
    uint8_t instance,
    uint8_t mb_idx,
    const flexcan_data_info_t *tx_info,
    uint32_t msg_id,
    const uint8_t *mb_data);

/*@}*/

/*!
 * @name Receive configuration
 * @{
 */

/*!
 * @brief FlexCAN receive message buffer field configuration
 *
 * @param   instance                   A FlexCAN instance number
 * @param   mb_idx                     Index of the message buffer
 * @param   rx_info                    Data info
 * @param   msg_id                     ID of the message to transmit
 * @return  STATUS_SUCCESS if successful;
 *          STATUS_CAN_BUFF_OUT_OF_RANGE if the index of a message buffer is invalid;
 */
status_t FLEXCAN_DRV_ConfigRxMb(
    uint8_t instance,
    uint8_t mb_idx,
    const flexcan_data_info_t *rx_info,
    uint32_t msg_id);

/*!
 * @brief FlexCAN Rx FIFO field configuration
 *
 * @note The number of elements in the ID filter table is defined by the
 *       following formula:
 *       - for format A: the number of Rx FIFO ID filters
 *       - for format B: twice the number of Rx FIFO ID filters
 *       - for format C: four times the number of Rx FIFO ID filters
 *       The user must provide the exact number of elements in order to avoid
 *       any misconfiguration.
 *
 *       Each element in the ID filter table specifies an ID to be used as
 *       acceptance criteria for the FIFO as follows:
 *       - for format A: In the standard frame format, bits 10 to 0 of the ID
 *         are used for frame identification. In the extended frame format, bits
 *         28 to 0 are used.
 *       - for format B: In the standard frame format, bits 10 to 0 of the ID
 *         are used for frame identification. In the extended frame format, only
 *         the 14 most significant bits (28 to 15) of the ID are compared to the
 *         14 most significant bits (28 to 15) of the received ID.
 *       - for format C: In both standard and extended frame formats, only the 8
 *         most significant bits (7 to 0 for standard, 28 to 21 for extended) of
 *         the ID are compared to the 8 most significant bits (7 to 0 for
 *         standard, 28 to 21 for extended) of the received ID.
 *
 * @param   instance           A FlexCAN instance number
 * @param   id_format          The format of the Rx FIFO ID Filter Table Elements
 * @param   id_filter_table    The ID filter table elements which contain RTR
 *                             bit, IDE bit, and Rx message ID
 */
void FLEXCAN_DRV_ConfigRxFifo(
    uint8_t instance,
    flexcan_rx_fifo_id_element_format_t id_format,
    const flexcan_id_table_t *id_filter_table);

/*!
 * @brief Receives a CAN frame using the specified message buffer, in a blocking manner.
 *
 * This function receives a CAN frame using a configured message buffer. The function
 * blocks until either a frame was received, or the specified timeout expired.
 *
 * @param   instance   A FlexCAN instance number
 * @param   mb_idx     Index of the message buffer
 * @param   data       The FlexCAN receive message buffer data.
 * @param   timeout_ms A timeout for the transfer in milliseconds.
 * @return  STATUS_SUCCESS if successful;
 *          STATUS_CAN_BUFF_OUT_OF_RANGE if the index of a message buffer is invalid;
 *          STATUS_BUSY if a resource is busy;
 *          STATUS_TIMEOUT if the timeout is reached
 */
status_t FLEXCAN_DRV_ReceiveBlocking(
    uint8_t instance,
    uint8_t mb_idx,
    flexcan_msgbuff_t *data,
    uint32_t timeout_ms);

/*!
 * @brief Receives a CAN frame using the specified message buffer.
 *
 * This function receives a CAN frame using a configured message buffer. The function
 * returns immediately. If a callback is installed, it will be invoked after
 * the frame was received and read into the specified buffer.
 *
 * @param   instance   A FlexCAN instance number
 * @param   mb_idx     Index of the message buffer
 * @param   data       The FlexCAN receive message buffer data.
 * @return  STATUS_SUCCESS if successful;
 *          STATUS_CAN_BUFF_OUT_OF_RANGE if the index of a message buffer is invalid;
 *          STATUS_BUSY if a resource is busy
 */
status_t FLEXCAN_DRV_Receive(
    uint8_t instance,
    uint8_t mb_idx,
    flexcan_msgbuff_t *data);

/*!
 * @brief Receives a CAN frame using the message FIFO, in a blocking manner.
 *
 * This function receives a CAN frame using the Rx FIFO. The function blocks until
 * either a frame was received, or the specified timeout expired.
 *
 * @param   instance    A FlexCAN instance number
 * @param   data        The FlexCAN receive message buffer data.
 * @param   timeout_ms  A timeout for the transfer in milliseconds.
 * @return  STATUS_SUCCESS if successful;
 *          STATUS_BUSY if a resource is busy;
 *          STATUS_TIMEOUT if the timeout is reached;
 *          STATUS_ERROR if other error occurred
 */
status_t FLEXCAN_DRV_RxFifoBlocking(
    uint8_t instance,
    flexcan_msgbuff_t *data,
    uint32_t timeout_ms);

/*!
 * @brief Receives a CAN frame using the message FIFO.
 *
 * This function receives a CAN frame using the Rx FIFO. The function returns
 * immediately. If a callback is installed, it will be invoked after the frame
 * was received and read into the specified buffer.
 *
 * @param   instance    A FlexCAN instance number
 * @param   data        The FlexCAN receive message buffer data.
 * @return  STATUS_SUCCESS if successful;
 *          STATUS_BUSY if a resource is busy;
 *          STATUS_ERROR if other error occurred
 */
status_t FLEXCAN_DRV_RxFifo(
    uint8_t instance,
    flexcan_msgbuff_t *data);

/*@}*/

/*!
 * @name Transfer status
 * @{
 */

/*!
 * @brief Ends a non-blocking FlexCAN transfer early.
 *
 * @param   instance   A FlexCAN instance number
 * @param   mb_idx     The index of the message buffer
 * @return  STATUS_SUCCESS if successful;
 *          STATUS_CAN_NO_TRANSFER_IN_PROGRESS if no transfer was running
 */
status_t FLEXCAN_DRV_AbortTransfer(uint8_t instance, uint8_t mb_idx);

/*!
 * @brief Returns whether the previous FlexCAN transfer has finished.
 *
 * When performing an async transfer, call this function to ascertain the state of the
 * current transfer: in progress (or busy) or complete (success).
 *
 * @param instance The FlexCAN instance number.
 * @param mb_idx The index of the message buffer.
 * @return STATUS_SUCCESS if successful;
 *         STATUS_BUSY if a resource is busy;
 *         STATUS_ERROR in case of a DMA error transfer;
 */
status_t FLEXCAN_DRV_GetTransferStatus(uint8_t instance, uint8_t mb_idx);

/*!
 * @brief Returns reported error conditions.
 *
 * Reports various error conditions detected in the reception and transmission of a CAN frame
 * and some general status of the device.
 *
 * @param instance The FlexCAN instance number.
 * @return value of the Error and Status 1 register;
 */
uint32_t FLEXCAN_DRV_GetErrorStatus(uint8_t instance);

/*@}*/

/*!
 * @name IRQ handler callback
 * @{
 */

/*!
 * @brief Installs a callback function for the IRQ handler.
 *
 * @param instance The FlexCAN instance number.
 * @param callback The callback function.
 * @param callbackParam User parameter passed to the callback function through the state parameter.
 */
void FLEXCAN_DRV_InstallEventCallback(uint8_t instance,
                                      flexcan_callback_t callback,
                                      void *callbackParam);

/*!
 * @brief Installs an error callback function for the IRQ handler and enables error interrupts.
 *
 * @param instance The FlexCAN instance number.
 * @param callback The error callback function.
 * @param callbackParam User parameter passed to the error callback function through the state
 *                      parameter.
 */
void FLEXCAN_DRV_InstallErrorCallback(uint8_t instance,
                                      flexcan_error_callback_t callback,
                                      void *callbackParam);

/*@}*/

#if FEATURE_CAN_HAS_PRETENDED_NETWORKING

/*!
 * @name Pretended Networking
 * @{
 */

/*!
 * @brief Configures Pretended Networking settings.
 *
 * @param instance The FlexCAN instance number.
 * @param enable Enable/Disable Pretended Networking mode.
 * @param pnConfig Pointer to the Pretended Networking configuration structure.
 */
void FLEXCAN_DRV_ConfigPN(uint8_t instance, bool enable, const flexcan_pn_config_t *pnConfig);

/*!
 * @brief Extracts one of the frames which triggered the wake up event.
 *
 * @param   instance    The FlexCAN instance number.
 * @param   wmbIndex  The index of the message buffer to be extracted.
 * @param   wmb  Pointer to the message buffer structure where the frame will be saved.
 */
void FLEXCAN_DRV_GetWMB(uint8_t instance, uint8_t wmbIndex, flexcan_msgbuff_t *wmb);

/*@}*/

#endif /* FEATURE_CAN_HAS_PRETENDED_NETWORKING */

#ifdef __cplusplus
}
#endif

/*! @}*/

#endif /* FLEXCAN_DRIVER_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/
