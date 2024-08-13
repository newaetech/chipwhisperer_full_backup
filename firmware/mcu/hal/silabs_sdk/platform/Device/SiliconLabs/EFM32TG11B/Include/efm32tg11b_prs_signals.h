/***************************************************************************//**
 * @file
 * @brief EFM32TG11B_PRS_SIGNALS register and bit field definitions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#if defined(__ICCARM__)
#pragma system_include       /* Treat file as system include file. */
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma clang system_header  /* Treat file as system include file. */
#endif

/***************************************************************************//**
 * @addtogroup Parts
 * @{
 ******************************************************************************/
/***************************************************************************//**
 * @addtogroup EFM32TG11B_PRS
 * @{
 * @addtogroup EFM32TG11B_PRS_Signals PRS Signals
 * @{
 * @brief PRS Signal names
 ******************************************************************************/
#define PRS_PRS_CH0                  ((1 << 8) + 0)  /**< PRS PRS channel 0 */
#define PRS_PRS_CH1                  ((1 << 8) + 1)  /**< PRS PRS channel 1 */
#define PRS_PRS_CH2                  ((1 << 8) + 2)  /**< PRS PRS channel 2 */
#define PRS_PRS_CH3                  ((1 << 8) + 3)  /**< PRS PRS channel 3 */
#define PRS_PRS_CH4                  ((1 << 8) + 4)  /**< PRS PRS channel 4 */
#define PRS_PRS_CH5                  ((1 << 8) + 5)  /**< PRS PRS channel 5 */
#define PRS_PRS_CH6                  ((1 << 8) + 6)  /**< PRS PRS channel 6 */
#define PRS_PRS_CH7                  ((1 << 8) + 7)  /**< PRS PRS channel 7 */
#define PRS_ACMP0_OUT                ((2 << 8) + 0)  /**< PRS Analog comparator output */
#define PRS_ACMP1_OUT                ((3 << 8) + 0)  /**< PRS Analog comparator output */
#define PRS_ADC0_SINGLE              ((4 << 8) + 0)  /**< PRS ADC single conversion done */
#define PRS_ADC0_SCAN                ((4 << 8) + 1)  /**< PRS ADC scan conversion done */
#define PRS_RTCC_CCV0                ((5 << 8) + 1)  /**< PRS RTCC Compare 0 */
#define PRS_RTCC_CCV1                ((5 << 8) + 2)  /**< PRS RTCC Compare 1 */
#define PRS_RTCC_CCV2                ((5 << 8) + 3)  /**< PRS RTCC Compare 2 */
#define PRS_GPIO_PIN0                ((6 << 8) + 0)  /**< PRS GPIO pin 0 */
#define PRS_GPIO_PIN1                ((6 << 8) + 1)  /**< PRS GPIO pin 1 */
#define PRS_GPIO_PIN2                ((6 << 8) + 2)  /**< PRS GPIO pin 2 */
#define PRS_GPIO_PIN3                ((6 << 8) + 3)  /**< PRS GPIO pin 3 */
#define PRS_GPIO_PIN4                ((6 << 8) + 4)  /**< PRS GPIO pin 4 */
#define PRS_GPIO_PIN5                ((6 << 8) + 5)  /**< PRS GPIO pin 5 */
#define PRS_GPIO_PIN6                ((6 << 8) + 6)  /**< PRS GPIO pin 6 */
#define PRS_GPIO_PIN7                ((6 << 8) + 7)  /**< PRS GPIO pin 7 */
#define PRS_GPIO_PIN8                ((7 << 8) + 0)  /**< PRS GPIO pin 8 */
#define PRS_GPIO_PIN9                ((7 << 8) + 1)  /**< PRS GPIO pin 9 */
#define PRS_GPIO_PIN10               ((7 << 8) + 2)  /**< PRS GPIO pin 10 */
#define PRS_GPIO_PIN11               ((7 << 8) + 3)  /**< PRS GPIO pin 11 */
#define PRS_GPIO_PIN12               ((7 << 8) + 4)  /**< PRS GPIO pin 12 */
#define PRS_GPIO_PIN13               ((7 << 8) + 5)  /**< PRS GPIO pin 13 */
#define PRS_GPIO_PIN14               ((7 << 8) + 6)  /**< PRS GPIO pin 14 */
#define PRS_GPIO_PIN15               ((7 << 8) + 7)  /**< PRS GPIO pin 15 */
#define PRS_LETIMER0_CH0             ((8 << 8) + 0)  /**< PRS LETIMER CH0 Out */
#define PRS_LETIMER0_CH1             ((8 << 8) + 1)  /**< PRS LETIMER CH1 Out */
#define PRS_PCNT0_TCC                ((9 << 8) + 0)  /**< PRS Triggered compare match */
#define PRS_PCNT0_UFOF               ((9 << 8) + 1)  /**< PRS Counter overflow or underflow */
#define PRS_PCNT0_DIR                ((9 << 8) + 2)  /**< PRS Counter direction */
#define PRS_CRYOTIMER_PERIOD         ((10 << 8) + 0) /**< PRS CRYOTIMER Output */
#define PRS_CMU_CLKOUT0              ((11 << 8) + 0) /**< PRS Clock Output 0 */
#define PRS_CMU_CLKOUT1              ((11 << 8) + 1) /**< PRS Clock Output 1 */
#define PRS_CMU_CLKOUT2              ((11 << 8) + 7) /**< PRS Clock Output 2 */
#define PRS_VDAC0_CH0                ((17 << 8) + 0) /**< PRS DAC ch0 conversion done */
#define PRS_VDAC0_CH1                ((17 << 8) + 1) /**< PRS DAC ch1 conversion done */
#define PRS_VDAC0_OPA0               ((17 << 8) + 2) /**< PRS OPA0 warmed up. output is valid. */
#define PRS_VDAC0_OPA1               ((17 << 8) + 3) /**< PRS OPA1 warmed up. output is valid. */
#define PRS_VDAC0_OPA2               ((17 << 8) + 4) /**< PRS OPA2 warmed up. output is valid. */
#define PRS_VDAC0_OPA3               ((17 << 8) + 5) /**< PRS OPA3 warmed up. output is valid. */
#define PRS_LESENSE_SCANRES0         ((18 << 8) + 0) /**< PRS LESENSE SCANRES register, bit 0 */
#define PRS_LESENSE_SCANRES1         ((18 << 8) + 1) /**< PRS LESENSE SCANRES register, bit 1 */
#define PRS_LESENSE_SCANRES2         ((18 << 8) + 2) /**< PRS LESENSE SCANRES register, bit 2 */
#define PRS_LESENSE_SCANRES3         ((18 << 8) + 3) /**< PRS LESENSE SCANRES register, bit 3 */
#define PRS_LESENSE_SCANRES4         ((18 << 8) + 4) /**< PRS LESENSE SCANRES register, bit 4 */
#define PRS_LESENSE_SCANRES5         ((18 << 8) + 5) /**< PRS LESENSE SCANRES register, bit 5 */
#define PRS_LESENSE_SCANRES6         ((18 << 8) + 6) /**< PRS LESENSE SCANRES register, bit 6 */
#define PRS_LESENSE_SCANRES7         ((18 << 8) + 7) /**< PRS LESENSE SCANRES register, bit 7 */
#define PRS_LESENSE_SCANRES8         ((19 << 8) + 0) /**< PRS LESENSE SCANRES register, bit 8 */
#define PRS_LESENSE_SCANRES9         ((19 << 8) + 1) /**< PRS LESENSE SCANRES register, bit 9 */
#define PRS_LESENSE_SCANRES10        ((19 << 8) + 2) /**< PRS LESENSE SCANRES register, bit 10 */
#define PRS_LESENSE_SCANRES11        ((19 << 8) + 3) /**< PRS LESENSE SCANRES register, bit 11 */
#define PRS_LESENSE_SCANRES12        ((19 << 8) + 4) /**< PRS LESENSE SCANRES register, bit 12 */
#define PRS_LESENSE_SCANRES13        ((19 << 8) + 5) /**< PRS LESENSE SCANRES register, bit 13 */
#define PRS_LESENSE_SCANRES14        ((19 << 8) + 6) /**< PRS LESENSE SCANRES register, bit 14 */
#define PRS_LESENSE_SCANRES15        ((19 << 8) + 7) /**< PRS LESENSE SCANRES register, bit 15 */
#define PRS_LESENSE_DEC0             ((20 << 8) + 0) /**< PRS LESENSE Decoder PRS out 0 */
#define PRS_LESENSE_DEC1             ((20 << 8) + 1) /**< PRS LESENSE Decoder PRS out 1 */
#define PRS_LESENSE_DEC2             ((20 << 8) + 2) /**< PRS LESENSE Decoder PRS out 2 */
#define PRS_LESENSE_DECCMP           ((20 << 8) + 3) /**< PRS LESENSE Decoder PRS compare value match channel */
#define PRS_LESENSE_MEASACT          ((21 << 8) + 0) /**< PRS LESENSE Measurement active */
#define PRS_USART0_IRTX              ((32 << 8) + 0) /**< PRS  */
#define PRS_USART0_TXC               ((32 << 8) + 1) /**< PRS  */
#define PRS_USART0_RXDATAV           ((32 << 8) + 2) /**< PRS  */
#define PRS_USART0_RTS               ((32 << 8) + 3) /**< PRS  */
#define PRS_USART0_TX                ((32 << 8) + 5) /**< PRS  */
#define PRS_USART0_CS                ((32 << 8) + 6) /**< PRS  */
#define PRS_USART1_TXC               ((33 << 8) + 1) /**< PRS  */
#define PRS_USART1_RXDATAV           ((33 << 8) + 2) /**< PRS  */
#define PRS_USART1_RTS               ((33 << 8) + 3) /**< PRS  */
#define PRS_USART1_TX                ((33 << 8) + 5) /**< PRS  */
#define PRS_USART1_CS                ((33 << 8) + 6) /**< PRS  */
#define PRS_USART2_IRTX              ((34 << 8) + 0) /**< PRS USART 2 IRDA out */
#define PRS_USART2_TXC               ((34 << 8) + 1) /**< PRS  */
#define PRS_USART2_RXDATAV           ((34 << 8) + 2) /**< PRS  */
#define PRS_USART2_RTS               ((34 << 8) + 3) /**< PRS  */
#define PRS_USART2_TX                ((34 << 8) + 5) /**< PRS  */
#define PRS_USART2_CS                ((34 << 8) + 6) /**< PRS  */
#define PRS_USART3_TXC               ((35 << 8) + 1) /**< PRS  */
#define PRS_USART3_RXDATAV           ((35 << 8) + 2) /**< PRS  */
#define PRS_USART3_RTS               ((35 << 8) + 3) /**< PRS  */
#define PRS_USART3_TX                ((35 << 8) + 5) /**< PRS  */
#define PRS_USART3_CS                ((35 << 8) + 6) /**< PRS  */
#define PRS_UART0_TXC                ((36 << 8) + 1) /**< PRS  */
#define PRS_UART0_RXDATAV            ((36 << 8) + 2) /**< PRS  */
#define PRS_UART0_RTS                ((36 << 8) + 3) /**< PRS  */
#define PRS_UART0_TX                 ((36 << 8) + 5) /**< PRS  */
#define PRS_UART0_CS                 ((36 << 8) + 6) /**< PRS  */
#define PRS_TIMER0_UF                ((37 << 8) + 0) /**< PRS  */
#define PRS_TIMER0_OF                ((37 << 8) + 1) /**< PRS  */
#define PRS_TIMER0_CC0               ((37 << 8) + 2) /**< PRS  */
#define PRS_TIMER0_CC1               ((37 << 8) + 3) /**< PRS  */
#define PRS_TIMER0_CC2               ((37 << 8) + 4) /**< PRS  */
#define PRS_TIMER1_UF                ((38 << 8) + 0) /**< PRS  */
#define PRS_TIMER1_OF                ((38 << 8) + 1) /**< PRS  */
#define PRS_TIMER1_CC0               ((38 << 8) + 2) /**< PRS  */
#define PRS_TIMER1_CC1               ((38 << 8) + 3) /**< PRS  */
#define PRS_TIMER1_CC2               ((38 << 8) + 4) /**< PRS  */
#define PRS_TIMER1_CC3               ((38 << 8) + 5) /**< PRS  */
#define PRS_WTIMER0_UF               ((39 << 8) + 0) /**< PRS  */
#define PRS_WTIMER0_OF               ((39 << 8) + 1) /**< PRS  */
#define PRS_WTIMER0_CC0              ((39 << 8) + 2) /**< PRS  */
#define PRS_WTIMER0_CC1              ((39 << 8) + 3) /**< PRS  */
#define PRS_WTIMER0_CC2              ((39 << 8) + 4) /**< PRS  */
#define PRS_WTIMER1_UF               ((40 << 8) + 0) /**< PRS  */
#define PRS_WTIMER1_OF               ((40 << 8) + 1) /**< PRS  */
#define PRS_WTIMER1_CC0              ((40 << 8) + 2) /**< PRS  */
#define PRS_WTIMER1_CC1              ((40 << 8) + 3) /**< PRS  */
#define PRS_WTIMER1_CC2              ((40 << 8) + 4) /**< PRS  */
#define PRS_WTIMER1_CC3              ((40 << 8) + 5) /**< PRS  */
#define PRS_CM0P_TXEV                ((41 << 8) + 0) /**< PRS  */
#define PRS_CM0P_ICACHEPCHITSOF      ((41 << 8) + 1) /**< PRS  */
#define PRS_CM0P_ICACHEPCMISSESOF    ((41 << 8) + 2) /**< PRS  */

/** @} */
/** @} End of group EFM32TG11B_PRS */
/** @} End of group Parts */
