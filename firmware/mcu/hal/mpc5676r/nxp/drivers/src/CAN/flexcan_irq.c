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
 
 /*!
 * @file flexcan_irq.c
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, Function not defined with external linkage.
 * The functions are not defined static because they are referenced in .s startup files.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.9, could define variable at block scope
 * The variable is defined in the common source file to make transition to other
 * platforms easier.
 */

#include "flexcan_irq.h"

#if (defined(CPU_S32K116) || defined(CPU_S32K118))

/*******************************************************************************
 * Code
 ******************************************************************************/
#if (CAN_INSTANCE_COUNT > 0U)
/* Implementation of CAN0 IRQ handler for OR'ed interrupts (Bus Off,
Transmit Warning, Receive Warning, Error, Wakeup). */
void CAN0_ORed_Err_Wakeup_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 0-31. */
void CAN0_ORed_0_31_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}
#endif /* (CAN_INSTANCE_COUNT > 0U) */

#elif (defined(CPU_S32K142) || defined(CPU_S32K144HFT0VLLT) || defined(CPU_S32K144LFT0MLLT) || \
     defined(CPU_S32K146) || defined(CPU_S32K148) || defined(CPU_S32MTV) || defined(CPU_S32K116)\
     || defined(CPU_S32K118))

/*******************************************************************************
 * Code
 ******************************************************************************/
#if (CAN_INSTANCE_COUNT > 0U)
/* Implementation of CAN0 IRQ handler for OR'ed interrupts (Bus Off,
Transmit Warning, Receive Warning). */
void CAN0_ORed_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN0_Error_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(0U);
}

#if FEATURE_CAN_HAS_WAKE_UP_IRQ

/* Implementation of CAN0 IRQ handler for interrupts indicating a wake up
event. */
void CAN0_Wake_Up_IRQHandler(void)
{
    FLEXCAN_WakeUpHandler(0U);
}

#endif /* FEATURE_CAN_HAS_WAKE_UP_IRQ */

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 0-15. */
void CAN0_ORed_0_15_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 16-31. */
void CAN0_ORed_16_31_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}
#endif /* (CAN_INSTANCE_COUNT > 0U) */

#if (CAN_INSTANCE_COUNT > 1U)
/* Implementation of CAN1 IRQ handler for OR'ed interrupts (Bus Off,
Transmit Warning, Receive Warning). */
void CAN1_ORed_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

/* Implementation of CAN1 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN1_Error_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(1U);
}

/* Implementation of CAN1 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 0-15. */
void CAN1_ORed_0_15_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

/* Implementation of CAN1 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 16-31. */
void CAN1_ORed_16_31_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}
#endif /* (CAN_INSTANCE_COUNT > 1U) */

#if (CAN_INSTANCE_COUNT > 2U)
/* Implementation of CAN2 IRQ handler for OR'ed interrupts (Bus Off,
Transmit Warning, Receive Warning). */
void CAN2_ORed_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

/* Implementation of CAN2 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN2_Error_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(2U);
}

/* Implementation of CAN2 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 0-15. */
void CAN2_ORed_0_15_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

/* Implementation of CAN2 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 16-31. */
void CAN2_ORed_16_31_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}
#endif /* (CAN_INSTANCE_COUNT > 2U) */

#elif defined(S32V23x_SERIES)

/*******************************************************************************
 * Code
 ******************************************************************************/
#if (CAN_INSTANCE_COUNT > 0U)
/* Implementation of CAN0 handler named in startup code. */
void CAN0_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

/* Implementation of CAN0 handler named in startup code. */
void CAN0_Buff_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}
#endif /* (CAN_INSTANCE_COUNT > 0U) */

#if (CAN_INSTANCE_COUNT > 1U)
/* Implementation of CAN1 handler named in startup code. */
void CAN1_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

/* Implementation of CAN1 handler named in startup code. */
void CAN1_Buff_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}
#endif /* (CAN_INSTANCE_COUNT > 1U) */

#elif (defined(CPU_MPC5748G) || defined(CPU_MPC5746C) || defined(CPU_MPC5744B) || \
       defined(CPU_MPC5745B) || defined(CPU_MPC5746B) || defined(CPU_MPC5744C) || \
       defined(CPU_MPC5745C) || defined(CPU_MPC5747C) || defined(CPU_MPC5748C) || \
       defined(CPU_MPC5746G) || defined(CPU_MPC5747G))

/*******************************************************************************
 * Code
 ******************************************************************************/
#if (CAN_INSTANCE_COUNT > 0U)
/* Implementation of CAN0 IRQ handler for OR'ed interrupts (Bus Off,
Transmit Warning, Receive Warning). */
void CAN0_ORed_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(0U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN0_Error_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(0U);
}

#if FEATURE_CAN_HAS_WAKE_UP_IRQ

/* Implementation of CAN0 IRQ handler for interrupts indicating a wake up
event. */
void CAN0_Wake_Up_IRQHandler(void)
{
    FLEXCAN_WakeUpHandler(0U);
}

#endif /* FEATURE_CAN_HAS_WAKE_UP_IRQ */

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 0-3. */
void CAN0_ORed_00_03_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 4-7. */
void CAN0_ORed_04_07_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 08-11. */
void CAN0_ORed_08_11_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 12-15. */
void CAN0_ORed_12_15_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 16-31. */
void CAN0_ORed_16_31_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 32-63. */
void CAN0_ORed_32_63_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 64-95. */
void CAN0_ORed_64_95_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}
#endif /* (CAN_INSTANCE_COUNT > 0U) */

#if (CAN_INSTANCE_COUNT > 1U)
/* Implementation of CAN1 IRQ handler for OR'ed interrupts (Bus Off,
Transmit Warning, Receive Warning). */
void CAN1_ORed_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(1U);
}

/* Implementation of CAN1 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN1_Error_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(1U);
}

/* Implementation of CAN1 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 0-3. */
void CAN1_ORed_00_03_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

/* Implementation of CAN1 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 4-7. */
void CAN1_ORed_04_07_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

/* Implementation of CAN1 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 08-11. */
void CAN1_ORed_08_11_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

/* Implementation of CAN1 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 12-15. */
void CAN1_ORed_12_15_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

/* Implementation of CAN1 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 16-31. */
void CAN1_ORed_16_31_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

/* Implementation of CAN1 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 32-63. */
void CAN1_ORed_32_63_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

/* Implementation of CAN1 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 64-95. */
void CAN1_ORed_64_95_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}
#endif /* (CAN_INSTANCE_COUNT > 1U) */

#if (CAN_INSTANCE_COUNT > 2U)
/* Implementation of CAN2 IRQ handler for OR'ed interrupts (Bus Off,
Transmit Warning, Receive Warning). */
void CAN2_ORed_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(2U);
}

/* Implementation of CAN2 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN2_Error_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(2U);
}

/* Implementation of CAN2 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 0-3. */
void CAN2_ORed_00_03_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

/* Implementation of CAN2 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 4-7. */
void CAN2_ORed_04_07_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

/* Implementation of CAN2 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 08-11. */
void CAN2_ORed_08_11_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

/* Implementation of CAN2 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 12-15. */
void CAN2_ORed_12_15_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

/* Implementation of CAN2 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 16-31. */
void CAN2_ORed_16_31_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

/* Implementation of CAN2 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 32-63. */
void CAN2_ORed_32_63_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

/* Implementation of CAN2 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 64-95. */
void CAN2_ORed_64_95_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}
#endif /* (CAN_INSTANCE_COUNT > 2U) */

#if (CAN_INSTANCE_COUNT > 3U)
/* Implementation of CAN3 IRQ handler for OR'ed interrupts (Bus Off,
Transmit Warning, Receive Warning). */
void CAN3_ORed_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(3U);
}

/* Implementation of CAN3 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN3_Error_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(3U);
}

/* Implementation of CAN3 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 0-3. */
void CAN3_ORed_00_03_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

/* Implementation of CAN3 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 4-7. */
void CAN3_ORed_04_07_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

/* Implementation of CAN3 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 08-11. */
void CAN3_ORed_08_11_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

/* Implementation of CAN3 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 12-15. */
void CAN3_ORed_12_15_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

/* Implementation of CAN3 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 16-31. */
void CAN3_ORed_16_31_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

/* Implementation of CAN3 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 32-63. */
void CAN3_ORed_32_63_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

/* Implementation of CAN3 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 64-95. */
void CAN3_ORed_64_95_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}
#endif /* (CAN_INSTANCE_COUNT > 3) */

#if (CAN_INSTANCE_COUNT > 4U)
/* Implementation of CAN4 IRQ handler for OR'ed interrupts (Bus Off,
Transmit Warning, Receive Warning). */
void CAN4_ORed_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(4U);
}

/* Implementation of CAN4 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN4_Error_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(4U);
}

/* Implementation of CAN4 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 0-3. */
void CAN4_ORed_00_03_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(4U);
}

/* Implementation of CAN4 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 4-7. */
void CAN4_ORed_04_07_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(4U);
}

/* Implementation of CAN4 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 08-11. */
void CAN4_ORed_08_11_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(4U);
}

/* Implementation of CAN4 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 12-15. */
void CAN4_ORed_12_15_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(4U);
}

/* Implementation of CAN4 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 16-31. */
void CAN4_ORed_16_31_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(4U);
}

/* Implementation of CAN4 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 32-63. */
void CAN4_ORed_32_63_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(4U);
}

/* Implementation of CAN4 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 64-95. */
void CAN4_ORed_64_95_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(4U);
}
#endif /* (CAN_INSTANCE_COUNT > 4) */

#if (CAN_INSTANCE_COUNT > 5U)
/* Implementation of CAN5 IRQ handler for OR'ed interrupts (Bus Off,
Transmit Warning, Receive Warning). */
void CAN5_ORed_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(5U);
}

/* Implementation of CAN5 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN5_Error_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(5U);
}

/* Implementation of CAN5 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 0-3. */
void CAN5_ORed_00_03_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(5U);
}

/* Implementation of CAN5 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 4-7. */
void CAN5_ORed_04_07_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(5U);
}

/* Implementation of CAN5 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 08-11. */
void CAN5_ORed_08_11_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(5U);
}

/* Implementation of CAN5 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 12-15. */
void CAN5_ORed_12_15_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(5U);
}

/* Implementation of CAN5 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 16-31. */
void CAN5_ORed_16_31_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(5U);
}

/* Implementation of CAN5 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 32-63. */
void CAN5_ORed_32_63_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(5U);
}

/* Implementation of CAN5 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 64-95. */
void CAN5_ORed_64_95_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(5U);
}
#endif /* (CAN_INSTANCE_COUNT > 5) */

#if (CAN_INSTANCE_COUNT > 6U)
/* Implementation of CAN6 IRQ handler for OR'ed interrupts (Bus Off,
Transmit Warning, Receive Warning). */
void CAN6_ORed_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(6U);
}

/* Implementation of CAN6 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN6_Error_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(6U);
}

/* Implementation of CAN6 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 0-3. */
void CAN6_ORed_00_03_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(6U);
}

/* Implementation of CAN6 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 4-7. */
void CAN6_ORed_04_07_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(6U);
}

/* Implementation of CAN6 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 08-11. */
void CAN6_ORed_08_11_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(6U);
}

/* Implementation of CAN6 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 12-15. */
void CAN6_ORed_12_15_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(6U);
}

/* Implementation of CAN6 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 16-31. */
void CAN6_ORed_16_31_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(6U);
}

/* Implementation of CAN6 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 32-63. */
void CAN6_ORed_32_63_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(6U);
}

/* Implementation of CAN6 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 64-95. */
void CAN6_ORed_64_95_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(6U);
}
#endif /* (CAN_INSTANCE_COUNT > 6) */

#if (CAN_INSTANCE_COUNT > 7U)
/* Implementation of CAN7 IRQ handler for OR'ed interrupts (Bus Off,
Transmit Warning, Receive Warning). */
void CAN7_ORed_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(7U);
}

/* Implementation of CAN7 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN7_Error_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(7U);
}

/* Implementation of CAN7 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 0-3. */
void CAN7_ORed_00_03_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(7U);
}

/* Implementation of CAN7 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 4-7. */
void CAN7_ORed_04_07_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(7U);
}

/* Implementation of CAN7 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 08-11. */
void CAN7_ORed_08_11_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(7U);
}

/* Implementation of CAN7 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 12-15. */
void CAN7_ORed_12_15_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(7U);
}

/* Implementation of CAN7 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 16-31. */
void CAN7_ORed_16_31_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(7U);
}

/* Implementation of CAN7 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 32-63. */
void CAN7_ORed_32_63_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(7U);
}

/* Implementation of CAN7 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 64-95. */
void CAN7_ORed_64_95_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(7U);
}
#endif /* (CAN_INSTANCE_COUNT > 7) */

#elif (defined(CPU_MPC5741P) || defined(CPU_MPC5742P) || defined(CPU_MPC5743P) || \
       defined(CPU_MPC5744P) || defined(CPU_S32R372) || defined(CPU_S32R274))

/*******************************************************************************
 * Code
 ******************************************************************************/
#if (CAN_INSTANCE_COUNT > 0U)
/* Implementation of CAN0 IRQ handler for OR'ed interrupts (Bus Off,
Transmit Warning, Receive Warning). */
void CAN0_ORed_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(0U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN0_Error_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(0U);
}

#if FEATURE_CAN_HAS_WAKE_UP_IRQ

/* Implementation of CAN0 IRQ handler for interrupts indicating a wake up
event. */
void CAN0_Wake_Up_IRQHandler(void)
{
    FLEXCAN_WakeUpHandler(0U);
}

#endif /* FEATURE_CAN_HAS_WAKE_UP_IRQ */

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 0-3. */
void CAN0_ORed_00_03_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 4-7. */
void CAN0_ORed_04_07_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 08-11. */
void CAN0_ORed_08_11_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 12-15. */
void CAN0_ORed_12_15_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 16-31. */
void CAN0_ORed_16_31_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 32-39. */
void CAN0_ORed_32_39_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 40-47. */
void CAN0_ORed_40_47_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 48-55. */
void CAN0_ORed_48_55_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 56-63. */
void CAN0_ORed_56_63_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

#if (defined(CPU_S32R372) || defined(CPU_S32R274))
/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 64-79. */
void CAN0_ORed_64_79_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 80-95. */
void CAN0_ORed_80_95_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}
#endif /* (CPU_S32R372) */
#endif /* (CAN_INSTANCE_COUNT > 0U) */


#if (CAN_INSTANCE_COUNT > 1U)
/* Implementation of CAN1 IRQ handler for OR'ed interrupts (Bus Off,
Transmit Warning, Receive Warning). For S32R372 the instance is no is 1 
and the can as peripheral is CAN2*/
#if defined(CPU_S32R372)
void CAN2_ORed_IRQHandler(void)
#else
void CAN1_ORed_IRQHandler(void)
#endif
{
    FLEXCAN_Error_IRQHandler(1U);
}

/* Implementation of CAN1 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. For S32R372 the instance is no is 1 
and the can as peripheral is CAN2*/
#if defined(CPU_S32R372)
void CAN2_Error_IRQHandler(void)
#else
void CAN1_Error_IRQHandler(void)
#endif
{
    FLEXCAN_Error_IRQHandler(1U);
}

/* Implementation of CAN1 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 0-3. For S32R372 the instance is no is 1 
and the can as peripheral is CAN2*/
#if defined(CPU_S32R372)
void CAN2_ORed_00_03_MB_IRQHandler(void)
#else
void CAN1_ORed_00_03_MB_IRQHandler(void)
#endif
{
    FLEXCAN_IRQHandler(1U);
}


/* Implementation of CAN1 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 4-7. For S32R372 the instance is no is 1 
and the can as peripheral is CAN2*/
#if defined(CPU_S32R372)
void CAN2_ORed_04_07_MB_IRQHandler(void)
#else
void CAN1_ORed_04_07_MB_IRQHandler(void)
#endif
{
    FLEXCAN_IRQHandler(1U);
}


/* Implementation of CAN1 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 08-11. For S32R372 the instance is no is 1 
and the can as peripheral is CAN2*/
#if defined(CPU_S32R372)
void CAN2_ORed_08_11_MB_IRQHandler(void)
#else
void CAN1_ORed_08_11_MB_IRQHandler(void)
#endif
{
    FLEXCAN_IRQHandler(1U);
}


/* Implementation of CAN1 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 12-15. For S32R372 the instance is no is 1 
and the can as peripheral is CAN2*/
#if defined(CPU_S32R372)
void CAN2_ORed_12_15_MB_IRQHandler(void)
#else
void CAN1_ORed_12_15_MB_IRQHandler(void)
#endif
{
    FLEXCAN_IRQHandler(1U);
}


/* Implementation of CAN1 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 16-31. For S32R372 the instance is no is 1 
and the can as peripheral is CAN2*/
#if defined(CPU_S32R372)
void CAN2_ORed_16_31_MB_IRQHandler(void)
#else
void CAN1_ORed_16_31_MB_IRQHandler(void)
#endif
{
    FLEXCAN_IRQHandler(1U);
}


/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 32-39. For S32R372 the instance is no is 1 
and the can as peripheral is CAN2*/
#if defined(CPU_S32R372)
void CAN2_ORed_32_39_MB_IRQHandler(void)
#else
void CAN1_ORed_32_39_MB_IRQHandler(void)
#endif
{
    FLEXCAN_IRQHandler(1U);
}


/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 40-47. For S32R372 the instance is no is 1 
and the can as peripheral is CAN2*/
#if defined(CPU_S32R372)
void CAN2_ORed_40_47_MB_IRQHandler(void)
#else
void CAN1_ORed_40_47_MB_IRQHandler(void)
#endif
{
    FLEXCAN_IRQHandler(1U);
}


/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 48-55. For S32R372 the instance is no is 1 
and the can as peripheral is CAN2*/
#if defined(CPU_S32R372)
void CAN2_ORed_48_55_MB_IRQHandler(void)
#else
void CAN1_ORed_48_55_MB_IRQHandler(void)
#endif
{
    FLEXCAN_IRQHandler(1U);
}


/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 56-63. For S32R372 the instance is no is 1 
and the can as peripheral is CAN2*/
#if defined(CPU_S32R372)
void CAN2_ORed_56_63_MB_IRQHandler(void)
#else
void CAN1_ORed_56_63_MB_IRQHandler(void)
#endif
{
    FLEXCAN_IRQHandler(1U);
}

#if defined(CPU_S32R372)
/* Implementation of CAN2 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 64-79. For S32R372 the instance is no is 1 
and the can as peripheral is CAN2*/
void CAN2_ORed_64_79_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

/* Implementation of CAN2 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 80-95. For S32R372 the instance is no is 1 
and the can as peripheral is CAN2*/
void CAN2_ORed_80_95_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}
#endif /* (CPU_S32R372) */
#endif /* (CAN_INSTANCE_COUNT > 1U) */

#if (CAN_INSTANCE_COUNT > 2U)
/* Implementation of CAN2 IRQ handler for OR'ed interrupts (Bus Off,
Transmit Warning, Receive Warning). */
void CAN2_ORed_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(2U);
}

/* Implementation of CAN2 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN2_Error_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(2U);
}

/* Implementation of CAN2 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 0-3. */
void CAN2_ORed_00_03_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

/* Implementation of CAN2 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 4-7. */
void CAN2_ORed_04_07_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

/* Implementation of CAN2 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 08-11. */
void CAN2_ORed_08_11_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

/* Implementation of CAN2 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 12-15. */
void CAN2_ORed_12_15_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

/* Implementation of CAN2 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 16-31. */
void CAN2_ORed_16_31_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 32-39. */
void CAN2_ORed_32_39_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 40-47. */
void CAN2_ORed_40_47_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 48-55. */
void CAN2_ORed_48_55_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 56-63. */
void CAN2_ORed_56_63_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

#if defined(CPU_S32R274)
/* Implementation of CAN2 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 64-79. */
void CAN2_ORed_64_79_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

/* Implementation of CAN2 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 80-95. */
void CAN2_ORed_80_95_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}
#endif /* (CPU_S32R274) */
#endif /* (CAN_INSTANCE_COUNT > 2U) */
#elif (defined(CPU_MPC5777C) || defined(CPU_MPC5775B) || defined(CPU_MPC5775E))
#if (CAN_INSTANCE_COUNT > 0U)
    
/* Implementation of CAN0 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN0_ESR1_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(0U);
}

void CAN0_ESR2_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(0U);
}
/* Implementation of CAN0 handler named in startup code. */
void CAN0_BUF0_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF1_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF2_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF3_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF4_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF5_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF6_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF7_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF8_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF9_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF10_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF11_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF12_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF13_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF14_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF15_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF16_31_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF32_63_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}
#endif /* (CAN_INSTANCE_COUNT > 0U) */

#if (CAN_INSTANCE_COUNT > 1U)
/* Implementation of CAN1 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN1_ESR1_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(1U);
}

void CAN1_ESR2_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(1U);
}
/* Implementation of CAN1 handler named in startup code. */
void CAN1_BUF0_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF1_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF2_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF3_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF4_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF5_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF6_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF7_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF8_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF9_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF10_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF11_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF12_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF13_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF14_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF15_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF16_31_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF32_63_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}    
#endif /* (CAN_INSTANCE_COUNT > 1U) */

#if (CAN_INSTANCE_COUNT > 2U)
/* Implementation of CAN2 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN2_ESR1_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(2U);
}

void CAN2_ESR2_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(2U);
}
/* Implementation of CAN2 handler named in startup code. */
void CAN2_BUF0_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF1_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF2_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF3_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF4_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF5_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF6_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF7_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF8_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF9_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF10_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF11_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF12_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF13_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF14_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF15_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF16_31_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF32_63_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}    
#endif /* (CAN_INSTANCE_COUNT > 2U) */

#if (CAN_INSTANCE_COUNT > 3U)
/* Implementation of CAN3 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN3_ESR1_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(3U);
}

void CAN3_ESR2_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(3U);
}
/* Implementation of CAN3 handler named in startup code. */
void CAN3_BUF0_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF1_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF2_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF3_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF4_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF5_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF6_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF7_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF8_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF9_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF10_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF11_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF12_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF13_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF14_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF15_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF16_31_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF32_63_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}    
#endif /* (CAN_INSTANCE_COUNT > 3U) */
/* (CPU_MPC5777C) */
#elif (defined(CPU_MPC5746R) || defined(CPU_MPC5743R) || defined(CPU_MPC5745R))
#if (CAN_INSTANCE_COUNT > 0U)
void CAN0_0_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(0U);
}    
void CAN0_1_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(0U);
}
void CAN0_5_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}
void CAN0_2_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}
void CAN0_3_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}
void CAN0_4_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}
#endif /*CAN_INSTANCE_COUNT > 0U */
#if (CAN_INSTANCE_COUNT > 1U)
void CAN1_0_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(1U);
}    
void CAN1_1_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(1U);
}
void CAN1_5_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}
void CAN1_2_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}
void CAN1_3_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}
void CAN1_4_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}
#endif /* (CAN_INSTANCE_COUNT > 1U) */
#if (CAN_INSTANCE_COUNT > 2U)
void CAN2_0_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(2U);
}    
void CAN2_1_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(2U);
}
void CAN2_5_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}
void CAN2_2_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}
void CAN2_3_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}
void CAN2_4_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}
#endif /* (CAN_INSTANCE_COUNT > 2U) */
#if (CAN_INSTANCE_COUNT > 3U)
void CAN3_0_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(3U);
}    
void CAN3_1_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(3U);
}
void CAN3_5_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}
void CAN3_2_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}
void CAN3_3_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}
void CAN3_4_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}
#endif /* (CAN_INSTANCE_COUNT > 2U) */

#elif defined(MPC5676R_SERIES)
#if (CAN_INSTANCE_COUNT > 0U)
    
/* Implementation of CAN0 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN0_ESR1_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(0U);
}

void CAN0_ESR2_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(0U);
}
/* Implementation of CAN0 handler named in startup code. */
void CAN0_BUF0_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF1_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF2_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF3_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF4_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF5_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF6_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF7_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF8_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF9_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF10_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF11_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF12_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF13_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF14_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF15_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF16_31_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}

void CAN0_BUF32_63_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U);
}
#endif /* (CAN_INSTANCE_COUNT > 0U) */

#if (CAN_INSTANCE_COUNT > 1U)
/* Implementation of CAN1 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN1_ESR1_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(1U);
}

void CAN1_ESR2_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(1U);
}
/* Implementation of CAN1 handler named in startup code. */
void CAN1_BUF0_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF1_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF2_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF3_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF4_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF5_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF6_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF7_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF8_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF9_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF10_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF11_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF12_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF13_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF14_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF15_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF16_31_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}

void CAN1_BUF32_63_IRQHandler(void)
{
    FLEXCAN_IRQHandler(1U);
}    
#endif /* (CAN_INSTANCE_COUNT > 1U) */

#if (CAN_INSTANCE_COUNT > 2U)
/* Implementation of CAN2 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN2_ESR1_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(2U);
}

void CAN2_ESR2_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(2U);
}
/* Implementation of CAN2 handler named in startup code. */
void CAN2_BUF0_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF1_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF2_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF3_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF4_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF5_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF6_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF7_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF8_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF9_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF10_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF11_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF12_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF13_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF14_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF15_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF16_31_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}

void CAN2_BUF32_63_IRQHandler(void)
{
    FLEXCAN_IRQHandler(2U);
}    
#endif /* (CAN_INSTANCE_COUNT > 2U) */

#if (CAN_INSTANCE_COUNT > 3U)
/* Implementation of CAN3 IRQ handler for interrupts indicating that errors were
detected on the CAN bus. */
void CAN3_ESR1_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(3U);
}

void CAN3_ESR2_IRQHandler(void)
{
    FLEXCAN_Error_IRQHandler(3U);
}
/* Implementation of CAN3 handler named in startup code. */
void CAN3_BUF0_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF1_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF2_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF3_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF4_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF5_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF6_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF7_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF8_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF9_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF10_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF11_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF12_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF13_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF14_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF15_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF16_31_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}

void CAN3_BUF32_63_IRQHandler(void)
{
    FLEXCAN_IRQHandler(3U);
}    
#endif /* (CAN_INSTANCE_COUNT > 3U) */
/* (CPU_MPC5777C) */
#else
    #error "No valid CPU defined!"
#endif

/*******************************************************************************
 * EOF
 ******************************************************************************/
