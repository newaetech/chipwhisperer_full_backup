/**
 * \file
 *
 * \brief SAM4S-XPLAINED LEDs support package.
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include "board.h"
#include "led.h"
#include "gpio.h"

#define ACTIVE_LEVEL_IS_HIGH 1

typedef const struct
{
    uint32_t ul_port_id;      //!< LED GPIO port.
    uint32_t ul_active_level; //!< Active level of the LED.
} tLED_DESCRIPTOR;

//! Hardware descriptors of all LEDs.
static tLED_DESCRIPTOR LED_DESCRIPTOR[BOARD_NUM_OF_LED] =
{
#define INSERT_LED_DESCRIPTOR(LED_NO, unused)      \
  {                                                \
    LED##LED_NO##_GPIO, LED##LED_NO##_ACTIVE_LEVEL \
  },
  MREPEAT(BOARD_NUM_OF_LED, INSERT_LED_DESCRIPTOR, ~)
#undef INSERT_LED_DESCRIPTOR
};

/*! \brief Turns off the specified LEDs.
 *
 * \param led_gpio LED to turn off (LEDx_GPIO).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
void LED_Off(uint32_t led_gpio)
{
	uint32_t i;
	for (i = 0; i < BOARD_NUM_OF_LED; i++) {
		if (led_gpio == LED_DESCRIPTOR[i].ul_port_id) {
			if (LED_DESCRIPTOR[i].ul_active_level == ACTIVE_LEVEL_IS_HIGH) {
				gpio_set_pin_low(led_gpio);
			} else {
				gpio_set_pin_high(led_gpio);
			}
		}
	}
}

/*! \brief Turns on the specified LEDs.
 *
 * \param led_gpio LED to turn on (LEDx_GPIO).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
void LED_On(uint32_t led_gpio)
{
	uint32_t i;
	for (i = 0; i < BOARD_NUM_OF_LED; i++) {
		if (led_gpio == LED_DESCRIPTOR[i].ul_port_id) {
			if (LED_DESCRIPTOR[i].ul_active_level == ACTIVE_LEVEL_IS_HIGH) {
				gpio_set_pin_high(led_gpio);
			} else {
				gpio_set_pin_low(led_gpio);
			}
		}
	}
}

