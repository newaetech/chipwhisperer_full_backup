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

#ifndef FLEXCAN_IRQ_H
#define FLEXCAN_IRQ_H

#include "MPC5676R.h"
#include "MPC5676R_features.h"



/*!
 * @brief Interrupt handler for a FlexCAN instance.
 *
 * @param   instance    The FlexCAN instance number.
 */
void FLEXCAN_IRQHandler(uint8_t instance);

/*!
 * @brief Error interrupt handler for a FlexCAN instance.
 *
 * @param   instance    The FlexCAN instance number.
 */
void FLEXCAN_Error_IRQHandler(uint8_t instance);

#if FEATURE_CAN_HAS_WAKE_UP_IRQ

/*!
 * @brief Wake up handler for a FlexCAN instance.
 *
 * @param   instance    The FlexCAN instance number.
 */
void FLEXCAN_WakeUpHandler(uint8_t instance);

#endif /* FEATURE_CAN_HAS_WAKE_UP_IRQ */

#if (defined(CPU_S32K116) || (defined(CPU_S32K118)))

/*******************************************************************************
 * Default interrupt handlers signatures
 ******************************************************************************/
void CAN0_ORed_Err_Wakeup_IRQHandler(void);
void CAN0_ORed_0_31_MB_IRQHandler(void);

#elif (defined(CPU_S32K142) || defined(CPU_S32K144HFT0VLLT) || defined(CPU_S32K144LFT0MLLT) || \
     defined(CPU_S32K146) || defined(CPU_S32K148) || defined(CPU_S32MTV) || defined(CPU_S32K116)\
     || (defined(CPU_S32K118)))

/*******************************************************************************
 * Default interrupt handlers signatures
 ******************************************************************************/
void CAN0_ORed_IRQHandler(void);
void CAN0_Error_IRQHandler(void);
#if FEATURE_CAN_HAS_WAKE_UP_IRQ
void CAN0_Wake_Up_IRQHandler(void);
#endif
void CAN0_ORed_0_15_MB_IRQHandler(void);
void CAN0_ORed_16_31_MB_IRQHandler(void);

#if (CAN_INSTANCE_COUNT > 1U)

void CAN1_ORed_IRQHandler(void);
void CAN1_Error_IRQHandler(void);
void CAN1_ORed_0_15_MB_IRQHandler(void);
void CAN1_ORed_16_31_MB_IRQHandler(void);

#endif /* (CAN_INSTANCE_COUNT > 1U) */

#if (CAN_INSTANCE_COUNT > 2U)

void CAN2_ORed_IRQHandler(void);
void CAN2_Error_IRQHandler(void);
void CAN2_ORed_0_15_MB_IRQHandler(void);
void CAN2_ORed_16_31_MB_IRQHandler(void);

#endif /* (CAN_INSTANCE_COUNT > 2U) */

#elif defined(S32V23x_SERIES)

/*******************************************************************************
 * Default interrupt handlers signatures
 ******************************************************************************/
void CAN0_IRQHandler(void);
void CAN0_Buff_IRQHandler(void);

#if (CAN_INSTANCE_COUNT > 1U)

void CAN1_IRQHandler(void);
void CAN1_Buff_IRQHandler(void);

#endif /* (CAN_INSTANCE_COUNT > 1U) */

#elif (defined(CPU_MPC5748G) || defined(CPU_MPC5746C) || defined(CPU_MPC5744B) || \
       defined(CPU_MPC5745B) || defined(CPU_MPC5746B) || defined(CPU_MPC5744C) || \
       defined(CPU_MPC5745C) || defined(CPU_MPC5747C) || defined(CPU_MPC5748C) || \
       defined(CPU_MPC5746G) || defined(CPU_MPC5747G))

/*******************************************************************************
 * Default interrupt handlers signatures
 ******************************************************************************/
void CAN0_ORed_IRQHandler(void);
void CAN0_Error_IRQHandler(void);
#if FEATURE_CAN_HAS_WAKE_UP_IRQ
void CAN0_Wake_Up_IRQHandler(void);
#endif
void CAN0_ORed_00_03_MB_IRQHandler(void);
void CAN0_ORed_04_07_MB_IRQHandler(void);
void CAN0_ORed_08_11_MB_IRQHandler(void);
void CAN0_ORed_12_15_MB_IRQHandler(void);
void CAN0_ORed_16_31_MB_IRQHandler(void);
void CAN0_ORed_32_63_MB_IRQHandler(void);
void CAN0_ORed_64_95_MB_IRQHandler(void);

#if (CAN_INSTANCE_COUNT > 1U)

void CAN1_ORed_IRQHandler(void);
void CAN1_Error_IRQHandler(void);
void CAN1_ORed_00_03_MB_IRQHandler(void);
void CAN1_ORed_04_07_MB_IRQHandler(void);
void CAN1_ORed_08_11_MB_IRQHandler(void);
void CAN1_ORed_12_15_MB_IRQHandler(void);
void CAN1_ORed_16_31_MB_IRQHandler(void);
void CAN1_ORed_32_63_MB_IRQHandler(void);
void CAN1_ORed_64_95_MB_IRQHandler(void);

#endif /* (CAN_INSTANCE_COUNT > 1U) */

#if (CAN_INSTANCE_COUNT > 2U)

void CAN2_ORed_IRQHandler(void);
void CAN2_Error_IRQHandler(void);
void CAN2_ORed_00_03_MB_IRQHandler(void);
void CAN2_ORed_04_07_MB_IRQHandler(void);
void CAN2_ORed_08_11_MB_IRQHandler(void);
void CAN2_ORed_12_15_MB_IRQHandler(void);
void CAN2_ORed_16_31_MB_IRQHandler(void);
void CAN2_ORed_32_63_MB_IRQHandler(void);
void CAN2_ORed_64_95_MB_IRQHandler(void);

#endif /* (CAN_INSTANCE_COUNT > 2U) */

#if (CAN_INSTANCE_COUNT > 3U)

void CAN3_ORed_IRQHandler(void);
void CAN3_Error_IRQHandler(void);
void CAN3_ORed_00_03_MB_IRQHandler(void);
void CAN3_ORed_04_07_MB_IRQHandler(void);
void CAN3_ORed_08_11_MB_IRQHandler(void);
void CAN3_ORed_12_15_MB_IRQHandler(void);
void CAN3_ORed_16_31_MB_IRQHandler(void);
void CAN3_ORed_32_63_MB_IRQHandler(void);
void CAN3_ORed_64_95_MB_IRQHandler(void);

#endif

#if (CAN_INSTANCE_COUNT > 4U)

void CAN4_ORed_IRQHandler(void);
void CAN4_Error_IRQHandler(void);
void CAN4_ORed_00_03_MB_IRQHandler(void);
void CAN4_ORed_04_07_MB_IRQHandler(void);
void CAN4_ORed_08_11_MB_IRQHandler(void);
void CAN4_ORed_12_15_MB_IRQHandler(void);
void CAN4_ORed_16_31_MB_IRQHandler(void);
void CAN4_ORed_32_63_MB_IRQHandler(void);
void CAN4_ORed_64_95_MB_IRQHandler(void);

#endif /* (CAN_INSTANCE_COUNT > 4U) */

#if (CAN_INSTANCE_COUNT > 5U)

void CAN5_ORed_IRQHandler(void);
void CAN5_Error_IRQHandler(void);
void CAN5_ORed_00_03_MB_IRQHandler(void);
void CAN5_ORed_04_07_MB_IRQHandler(void);
void CAN5_ORed_08_11_MB_IRQHandler(void);
void CAN5_ORed_12_15_MB_IRQHandler(void);
void CAN5_ORed_16_31_MB_IRQHandler(void);
void CAN5_ORed_32_63_MB_IRQHandler(void);
void CAN5_ORed_64_95_MB_IRQHandler(void);

#endif
#if (CAN_INSTANCE_COUNT > 6U)

void CAN6_ORed_IRQHandler(void);
void CAN6_Error_IRQHandler(void);
void CAN6_ORed_00_03_MB_IRQHandler(void);
void CAN6_ORed_04_07_MB_IRQHandler(void);
void CAN6_ORed_08_11_MB_IRQHandler(void);
void CAN6_ORed_12_15_MB_IRQHandler(void);
void CAN6_ORed_16_31_MB_IRQHandler(void);
void CAN6_ORed_32_63_MB_IRQHandler(void);
void CAN6_ORed_64_95_MB_IRQHandler(void);

#endif /* (CAN_INSTANCE_COUNT > 6U) */

#if (CAN_INSTANCE_COUNT > 7U)

void CAN7_ORed_IRQHandler(void);
void CAN7_Error_IRQHandler(void);
void CAN7_ORed_00_03_MB_IRQHandler(void);
void CAN7_ORed_04_07_MB_IRQHandler(void);
void CAN7_ORed_08_11_MB_IRQHandler(void);
void CAN7_ORed_12_15_MB_IRQHandler(void);
void CAN7_ORed_16_31_MB_IRQHandler(void);
void CAN7_ORed_32_63_MB_IRQHandler(void);
void CAN7_ORed_64_95_MB_IRQHandler(void);

#endif /* (CAN_INSTANCE_COUNT > 7U) */

#elif (defined(CPU_MPC5741P) || defined(CPU_MPC5742P) || defined(CPU_MPC5743P) || \
       defined(CPU_MPC5744P) || defined(CPU_S32R372) || defined(CPU_S32R274))

/*******************************************************************************
 * Default interrupt handlers signatures
 ******************************************************************************/
void CAN0_ORed_IRQHandler(void);
void CAN0_Error_IRQHandler(void);
#if FEATURE_CAN_HAS_WAKE_UP_IRQ
void CAN0_Wake_Up_IRQHandler(void);
#endif
void CAN0_ORed_00_03_MB_IRQHandler(void);
void CAN0_ORed_04_07_MB_IRQHandler(void);
void CAN0_ORed_08_11_MB_IRQHandler(void);
void CAN0_ORed_12_15_MB_IRQHandler(void);
void CAN0_ORed_16_31_MB_IRQHandler(void);
void CAN0_ORed_32_39_MB_IRQHandler(void);
void CAN0_ORed_40_47_MB_IRQHandler(void);
void CAN0_ORed_48_55_MB_IRQHandler(void);
void CAN0_ORed_56_63_MB_IRQHandler(void);
#if (defined(CPU_S32R372) || defined(CPU_S32R274))
void CAN0_ORed_64_79_MB_IRQHandler(void);
void CAN0_ORed_80_95_MB_IRQHandler(void);
#endif

#if (CAN_INSTANCE_COUNT > 1U)

#if( defined(CPU_MPC5741P) || defined(CPU_MPC5742P) || defined(CPU_MPC5743P) || \
     defined(CPU_MPC5744P) || defined(CPU_S32R274))
void CAN1_ORed_IRQHandler(void);
void CAN1_Error_IRQHandler(void);
void CAN1_ORed_00_03_MB_IRQHandler(void);
void CAN1_ORed_04_07_MB_IRQHandler(void);
void CAN1_ORed_08_11_MB_IRQHandler(void);
void CAN1_ORed_12_15_MB_IRQHandler(void);
void CAN1_ORed_16_31_MB_IRQHandler(void);
void CAN1_ORed_32_39_MB_IRQHandler(void);
void CAN1_ORed_40_47_MB_IRQHandler(void);
void CAN1_ORed_48_55_MB_IRQHandler(void);
void CAN1_ORed_56_63_MB_IRQHandler(void);
#else // is defined(CPU_S32R372)
void CAN2_ORed_IRQHandler(void);
void CAN2_Error_IRQHandler(void);
void CAN2_ORed_00_03_MB_IRQHandler(void);
void CAN2_ORed_04_07_MB_IRQHandler(void);
void CAN2_ORed_08_11_MB_IRQHandler(void);
void CAN2_ORed_12_15_MB_IRQHandler(void);
void CAN2_ORed_16_31_MB_IRQHandler(void);
void CAN2_ORed_32_39_MB_IRQHandler(void);
void CAN2_ORed_40_47_MB_IRQHandler(void);
void CAN2_ORed_48_55_MB_IRQHandler(void);
void CAN2_ORed_56_63_MB_IRQHandler(void);
void CAN2_ORed_64_79_MB_IRQHandler(void);
void CAN2_ORed_80_95_MB_IRQHandler(void);
#endif //endif cpu defined
#endif /* (CAN_INSTANCE_COUNT > 1U) */

#if (CAN_INSTANCE_COUNT > 2U)

void CAN2_ORed_IRQHandler(void);
void CAN2_Error_IRQHandler(void);
void CAN2_ORed_00_03_MB_IRQHandler(void);
void CAN2_ORed_04_07_MB_IRQHandler(void);
void CAN2_ORed_08_11_MB_IRQHandler(void);
void CAN2_ORed_12_15_MB_IRQHandler(void);
void CAN2_ORed_16_31_MB_IRQHandler(void);
void CAN2_ORed_32_39_MB_IRQHandler(void);
void CAN2_ORed_40_47_MB_IRQHandler(void);
void CAN2_ORed_48_55_MB_IRQHandler(void);
void CAN2_ORed_56_63_MB_IRQHandler(void);
#if (defined(CPU_S32R372) || defined(CPU_S32R274))
void CAN2_ORed_64_79_MB_IRQHandler(void);
void CAN2_ORed_80_95_MB_IRQHandler(void);
#endif
#endif /* (CAN_INSTANCE_COUNT > 2U) */

#elif (defined(CPU_MPC5777C) || defined(CPU_MPC5775B) || defined(CPU_MPC5775E))
#if (CAN_INSTANCE_COUNT > 0U)
    
/* Implementation of CAN0 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN0_ESR1_IRQHandler(void);
void CAN0_ESR2_IRQHandler(void);
/* Implementation of CAN0 handler for message buffers in startup code. */
void CAN0_BUF0_IRQHandler(void);
void CAN0_BUF1_IRQHandler(void);
void CAN0_BUF2_IRQHandler(void);
void CAN0_BUF3_IRQHandler(void);
void CAN0_BUF4_IRQHandler(void);
void CAN0_BUF5_IRQHandler(void);
void CAN0_BUF6_IRQHandler(void);
void CAN0_BUF7_IRQHandler(void);
void CAN0_BUF8_IRQHandler(void);
void CAN0_BUF9_IRQHandler(void);
void CAN0_BUF10_IRQHandler(void);
void CAN0_BUF11_IRQHandler(void);
void CAN0_BUF12_IRQHandler(void);
void CAN0_BUF13_IRQHandler(void);
void CAN0_BUF14_IRQHandler(void);
void CAN0_BUF15_IRQHandler(void);
void CAN0_BUF16_31_IRQHandler(void);
void CAN0_BUF32_63_IRQHandler(void);
#endif /* (CAN_INSTANCE_COUNT > 0U) */

#if (CAN_INSTANCE_COUNT > 1U)
/* Implementation of CAN1 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN1_ESR1_IRQHandler(void);
void CAN1_ESR2_IRQHandler(void);
/* Implementation of CAN1 handler for message buffers in startup code. */
void CAN1_BUF0_IRQHandler(void);
void CAN1_BUF1_IRQHandler(void);
void CAN1_BUF2_IRQHandler(void);
void CAN1_BUF3_IRQHandler(void);
void CAN1_BUF4_IRQHandler(void);
void CAN1_BUF5_IRQHandler(void);
void CAN1_BUF6_IRQHandler(void);
void CAN1_BUF7_IRQHandler(void);
void CAN1_BUF8_IRQHandler(void);
void CAN1_BUF9_IRQHandler(void);
void CAN1_BUF10_IRQHandler(void);
void CAN1_BUF11_IRQHandler(void);
void CAN1_BUF12_IRQHandler(void);
void CAN1_BUF13_IRQHandler(void);
void CAN1_BUF14_IRQHandler(void);
void CAN1_BUF15_IRQHandler(void);
void CAN1_BUF16_31_IRQHandler(void);
void CAN1_BUF32_63_IRQHandler(void);    
#endif   /* (CAN_INSTANCE_COUNT > 1U) */  

#if (CAN_INSTANCE_COUNT > 2U)
/* Implementation of CAN2 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN2_ESR1_IRQHandler(void);
void CAN2_ESR2_IRQHandler(void);
/* Implementation of CAN2 handler for message buffers in startup code. */
void CAN2_BUF0_IRQHandler(void);
void CAN2_BUF1_IRQHandler(void);
void CAN2_BUF2_IRQHandler(void);
void CAN2_BUF3_IRQHandler(void);
void CAN2_BUF4_IRQHandler(void);
void CAN2_BUF5_IRQHandler(void);
void CAN2_BUF6_IRQHandler(void);
void CAN2_BUF7_IRQHandler(void);
void CAN2_BUF8_IRQHandler(void);
void CAN2_BUF9_IRQHandler(void);
void CAN2_BUF10_IRQHandler(void);
void CAN2_BUF11_IRQHandler(void);
void CAN2_BUF12_IRQHandler(void);
void CAN2_BUF13_IRQHandler(void);
void CAN2_BUF14_IRQHandler(void);
void CAN2_BUF15_IRQHandler(void);
void CAN2_BUF16_31_IRQHandler(void);
void CAN2_BUF32_63_IRQHandler(void);    
#endif   /* (CAN_INSTANCE_COUNT > 2U) */  

#if (CAN_INSTANCE_COUNT > 3U)
/* Implementation of CAN3 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN3_ESR1_IRQHandler(void);
void CAN3_ESR2_IRQHandler(void);
/* Implementation of CAN3 handler for message buffers in startup code. */
void CAN3_BUF0_IRQHandler(void);
void CAN3_BUF1_IRQHandler(void);
void CAN3_BUF2_IRQHandler(void);
void CAN3_BUF3_IRQHandler(void);
void CAN3_BUF4_IRQHandler(void);
void CAN3_BUF5_IRQHandler(void);
void CAN3_BUF6_IRQHandler(void);
void CAN3_BUF7_IRQHandler(void);
void CAN3_BUF8_IRQHandler(void);
void CAN3_BUF9_IRQHandler(void);
void CAN3_BUF10_IRQHandler(void);
void CAN3_BUF11_IRQHandler(void);
void CAN3_BUF12_IRQHandler(void);
void CAN3_BUF13_IRQHandler(void);
void CAN3_BUF14_IRQHandler(void);
void CAN3_BUF15_IRQHandler(void);
void CAN3_BUF16_31_IRQHandler(void);
void CAN3_BUF32_63_IRQHandler(void);    
#endif   /* (CAN_INSTANCE_COUNT > 3U) */  
/* (CPU_MPC5777C) */
#elif (defined(CPU_MPC5746R) || defined(CPU_MPC5743R) || defined(CPU_MPC5745R))
#if (CAN_INSTANCE_COUNT > 0U)
void CAN0_0_IRQHandler(void);
void CAN0_1_IRQHandler(void);
void CAN0_5_IRQHandler(void);
void CAN0_2_IRQHandler(void);
void CAN0_3_IRQHandler(void);
void CAN0_4_IRQHandler(void);
#endif /* (CAN_INSTANCE_COUNT > 0U) */
#if (CAN_INSTANCE_COUNT > 1U)
void CAN1_0_IRQHandler(void);
void CAN1_1_IRQHandler(void);
void CAN1_5_IRQHandler(void);
void CAN1_2_IRQHandler(void);
void CAN1_3_IRQHandler(void);
void CAN1_4_IRQHandler(void);   
#endif /* (CAN_INSTANCE_COUNT > 1U) */
#if (CAN_INSTANCE_COUNT > 2U)
void CAN2_0_IRQHandler(void);
void CAN2_1_IRQHandler(void);
void CAN2_5_IRQHandler(void);
void CAN2_2_IRQHandler(void);
void CAN2_3_IRQHandler(void);
void CAN2_4_IRQHandler(void);   
#endif /* (CAN_INSTANCE_COUNT > 2U) */
#if (CAN_INSTANCE_COUNT > 3U)
void CAN3_0_IRQHandler(void);
void CAN3_1_IRQHandler(void);
void CAN3_5_IRQHandler(void);
void CAN3_2_IRQHandler(void);
void CAN3_3_IRQHandler(void);
void CAN3_4_IRQHandler(void);   
#endif /* (CAN_INSTANCE_COUNT > 3U) */

#elif defined(MPC5676R_SERIES)

#if (CAN_INSTANCE_COUNT > 0U)
    
/* Implementation of CAN0 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN0_ESR1_IRQHandler(void);
void CAN0_ESR2_IRQHandler(void);
/* Implementation of CAN0 handler for message buffers in startup code. */
void CAN0_BUF0_IRQHandler(void);
void CAN0_BUF1_IRQHandler(void);
void CAN0_BUF2_IRQHandler(void);
void CAN0_BUF3_IRQHandler(void);
void CAN0_BUF4_IRQHandler(void);
void CAN0_BUF5_IRQHandler(void);
void CAN0_BUF6_IRQHandler(void);
void CAN0_BUF7_IRQHandler(void);
void CAN0_BUF8_IRQHandler(void);
void CAN0_BUF9_IRQHandler(void);
void CAN0_BUF10_IRQHandler(void);
void CAN0_BUF11_IRQHandler(void);
void CAN0_BUF12_IRQHandler(void);
void CAN0_BUF13_IRQHandler(void);
void CAN0_BUF14_IRQHandler(void);
void CAN0_BUF15_IRQHandler(void);
void CAN0_BUF16_31_IRQHandler(void);
void CAN0_BUF32_63_IRQHandler(void);
#endif /* (CAN_INSTANCE_COUNT > 0U) */

#if (CAN_INSTANCE_COUNT > 1U)
/* Implementation of CAN1 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN1_ESR1_IRQHandler(void);
void CAN1_ESR2_IRQHandler(void);
/* Implementation of CAN1 handler for message buffers in startup code. */
void CAN1_BUF0_IRQHandler(void);
void CAN1_BUF1_IRQHandler(void);
void CAN1_BUF2_IRQHandler(void);
void CAN1_BUF3_IRQHandler(void);
void CAN1_BUF4_IRQHandler(void);
void CAN1_BUF5_IRQHandler(void);
void CAN1_BUF6_IRQHandler(void);
void CAN1_BUF7_IRQHandler(void);
void CAN1_BUF8_IRQHandler(void);
void CAN1_BUF9_IRQHandler(void);
void CAN1_BUF10_IRQHandler(void);
void CAN1_BUF11_IRQHandler(void);
void CAN1_BUF12_IRQHandler(void);
void CAN1_BUF13_IRQHandler(void);
void CAN1_BUF14_IRQHandler(void);
void CAN1_BUF15_IRQHandler(void);
void CAN1_BUF16_31_IRQHandler(void);
void CAN1_BUF32_63_IRQHandler(void);    
#endif   /* (CAN_INSTANCE_COUNT > 1U) */  

#if (CAN_INSTANCE_COUNT > 2U)
/* Implementation of CAN2 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN2_ESR1_IRQHandler(void);
void CAN2_ESR2_IRQHandler(void);
/* Implementation of CAN2 handler for message buffers in startup code. */
void CAN2_BUF0_IRQHandler(void);
void CAN2_BUF1_IRQHandler(void);
void CAN2_BUF2_IRQHandler(void);
void CAN2_BUF3_IRQHandler(void);
void CAN2_BUF4_IRQHandler(void);
void CAN2_BUF5_IRQHandler(void);
void CAN2_BUF6_IRQHandler(void);
void CAN2_BUF7_IRQHandler(void);
void CAN2_BUF8_IRQHandler(void);
void CAN2_BUF9_IRQHandler(void);
void CAN2_BUF10_IRQHandler(void);
void CAN2_BUF11_IRQHandler(void);
void CAN2_BUF12_IRQHandler(void);
void CAN2_BUF13_IRQHandler(void);
void CAN2_BUF14_IRQHandler(void);
void CAN2_BUF15_IRQHandler(void);
void CAN2_BUF16_31_IRQHandler(void);
void CAN2_BUF32_63_IRQHandler(void);    
#endif   /* (CAN_INSTANCE_COUNT > 2U) */  

#if (CAN_INSTANCE_COUNT > 3U)
/* Implementation of CAN3 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN3_ESR1_IRQHandler(void);
void CAN3_ESR2_IRQHandler(void);
/* Implementation of CAN3 handler for message buffers in startup code. */
void CAN3_BUF0_IRQHandler(void);
void CAN3_BUF1_IRQHandler(void);
void CAN3_BUF2_IRQHandler(void);
void CAN3_BUF3_IRQHandler(void);
void CAN3_BUF4_IRQHandler(void);
void CAN3_BUF5_IRQHandler(void);
void CAN3_BUF6_IRQHandler(void);
void CAN3_BUF7_IRQHandler(void);
void CAN3_BUF8_IRQHandler(void);
void CAN3_BUF9_IRQHandler(void);
void CAN3_BUF10_IRQHandler(void);
void CAN3_BUF11_IRQHandler(void);
void CAN3_BUF12_IRQHandler(void);
void CAN3_BUF13_IRQHandler(void);
void CAN3_BUF14_IRQHandler(void);
void CAN3_BUF15_IRQHandler(void);
void CAN3_BUF16_31_IRQHandler(void);
void CAN3_BUF32_63_IRQHandler(void);    
#endif   /* (CAN_INSTANCE_COUNT > 3U) */  
/* (CPU_MPC5777C) */

#else
    #error "No valid CPU defined!"
#endif

#endif /* FLEXCAN_IRQ_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/
