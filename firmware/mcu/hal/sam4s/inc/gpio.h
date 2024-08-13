/**
 * \file
 *
 * \brief Common GPIO API.
 *
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
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
#ifndef _GPIO_H_
#define _GPIO_H_

#include <parts.h>

#if (SAM3S || SAM3U || SAM3N || SAM3XA || SAM4S || SAM4E || SAM4N || SAM4C || SAMG || SAM4CP || SAM4CM || SAMV71 || SAMV70 || SAME70 || SAMS70)
# include "sam_gpio/sam_gpio.h"
#elif XMEGA
# include "xmega_gpio/xmega_gpio.h"
#elif MEGA || MEGA_RF
# include "mega_gpio/mega_gpio.h"
#else
# error Unsupported chip type
#endif

/**
 * \defgroup gpio_group General Purpose Input/Output
 *
 * This is the common API for GPIO. Additional features are available
 * in the documentation of the specific modules.
 *
 * \section io_group_platform Platform Dependencies
 *
 * The following functions are available on all platforms, but there may
 * be variations in the function signature (i.e. parameters) and
 * behaviour. These functions are typically called by platform-specific
 * parts of drivers, and applications that aren't intended to be
 * portable:
 *   - gpio_pin_is_low()
 *   - gpio_pin_is_high()
 *   - gpio_set_pin_high()
 *   - gpio_set_pin_group_high()
 *   - gpio_set_pin_low()
 *   - gpio_set_pin_group_low()
 *   - gpio_toggle_pin()
 *   - gpio_toggle_pin_group()
 *   - gpio_configure_pin()
 *   - gpio_configure_group()
 */

#endif  /* _GPIO_H_ */
