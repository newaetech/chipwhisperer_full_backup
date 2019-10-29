/*******************************************************************************
* File Name: cycfg_pins.h
*
* Description:
* Pin configuration
* This file was automatically generated and should not be modified.
* 
********************************************************************************
* Copyright 2017-2019 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
********************************************************************************/

#if !defined(CYCFG_PINS_H)
#define CYCFG_PINS_H

#include "cycfg_notices.h"
#include "cy_gpio.h"
#include "cycfg_routing.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define ioss_0_port_12_pin_6_PORT GPIO_PRT12
#define ioss_0_port_12_pin_6_PIN 6U
#define ioss_0_port_12_pin_6_NUM 6U
#define ioss_0_port_12_pin_6_DRIVEMODE CY_GPIO_DM_ANALOG
#define ioss_0_port_12_pin_6_INIT_DRIVESTATE 1
#ifndef ioss_0_port_12_pin_6_HSIOM
	#define ioss_0_port_12_pin_6_HSIOM HSIOM_SEL_GPIO
#endif
#define ioss_0_port_12_pin_6_IRQ ioss_interrupts_gpio_12_IRQn
#define ioss_0_port_12_pin_7_PORT GPIO_PRT12
#define ioss_0_port_12_pin_7_PIN 7U
#define ioss_0_port_12_pin_7_NUM 7U
#define ioss_0_port_12_pin_7_DRIVEMODE CY_GPIO_DM_ANALOG
#define ioss_0_port_12_pin_7_INIT_DRIVESTATE 1
#ifndef ioss_0_port_12_pin_7_HSIOM
	#define ioss_0_port_12_pin_7_HSIOM HSIOM_SEL_GPIO
#endif
#define ioss_0_port_12_pin_7_IRQ ioss_interrupts_gpio_12_IRQn
#define ioss_0_port_2_pin_0_PORT GPIO_PRT2
#define ioss_0_port_2_pin_0_PIN 0U
#define ioss_0_port_2_pin_0_NUM 0U
#define ioss_0_port_2_pin_0_DRIVEMODE CY_GPIO_DM_HIGHZ
#define ioss_0_port_2_pin_0_INIT_DRIVESTATE 1
#ifndef ioss_0_port_2_pin_0_HSIOM
	#define ioss_0_port_2_pin_0_HSIOM HSIOM_SEL_GPIO
#endif
#define ioss_0_port_2_pin_0_IRQ ioss_interrupts_gpio_2_IRQn
#define ioss_0_port_2_pin_1_PORT GPIO_PRT2
#define ioss_0_port_2_pin_1_PIN 1U
#define ioss_0_port_2_pin_1_NUM 1U
#define ioss_0_port_2_pin_1_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define ioss_0_port_2_pin_1_INIT_DRIVESTATE 1
#ifndef ioss_0_port_2_pin_1_HSIOM
	#define ioss_0_port_2_pin_1_HSIOM HSIOM_SEL_GPIO
#endif
#define ioss_0_port_2_pin_1_IRQ ioss_interrupts_gpio_2_IRQn
#define ioss_0_port_2_pin_7_PORT GPIO_PRT2
#define ioss_0_port_2_pin_7_PIN 7U
#define ioss_0_port_2_pin_7_NUM 7U
#define ioss_0_port_2_pin_7_DRIVEMODE CY_GPIO_DM_HIGHZ
#define ioss_0_port_2_pin_7_INIT_DRIVESTATE 1
#ifndef ioss_0_port_2_pin_7_HSIOM
	#define ioss_0_port_2_pin_7_HSIOM HSIOM_SEL_GPIO
#endif
#define ioss_0_port_2_pin_7_IRQ ioss_interrupts_gpio_2_IRQn
#define ioss_0_port_3_pin_2_PORT GPIO_PRT3
#define ioss_0_port_3_pin_2_PIN 2U
#define ioss_0_port_3_pin_2_NUM 2U
#define ioss_0_port_3_pin_2_DRIVEMODE CY_GPIO_DM_STRONG
#define ioss_0_port_3_pin_2_INIT_DRIVESTATE 1
#ifndef ioss_0_port_3_pin_2_HSIOM
	#define ioss_0_port_3_pin_2_HSIOM HSIOM_SEL_GPIO
#endif
#define ioss_0_port_3_pin_2_IRQ ioss_interrupts_gpio_3_IRQn
#define SWO_PORT GPIO_PRT6
#define SWO_PIN 4U
#define SWO_NUM 4U
#define SWO_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define SWO_INIT_DRIVESTATE 1
#ifndef ioss_0_port_6_pin_4_HSIOM
	#define ioss_0_port_6_pin_4_HSIOM HSIOM_SEL_GPIO
#endif
#define SWO_HSIOM ioss_0_port_6_pin_4_HSIOM
#define SWO_IRQ ioss_interrupts_gpio_6_IRQn
#define SWDIO_PORT GPIO_PRT6
#define SWDIO_PIN 6U
#define SWDIO_NUM 6U
#define SWDIO_DRIVEMODE CY_GPIO_DM_PULLUP
#define SWDIO_INIT_DRIVESTATE 1
#ifndef ioss_0_port_6_pin_6_HSIOM
	#define ioss_0_port_6_pin_6_HSIOM HSIOM_SEL_GPIO
#endif
#define SWDIO_HSIOM ioss_0_port_6_pin_6_HSIOM
#define SWDIO_IRQ ioss_interrupts_gpio_6_IRQn
#define SWCLK_PORT GPIO_PRT6
#define SWCLK_PIN 7U
#define SWCLK_NUM 7U
#define SWCLK_DRIVEMODE CY_GPIO_DM_PULLDOWN
#define SWCLK_INIT_DRIVESTATE 1
#ifndef ioss_0_port_6_pin_7_HSIOM
	#define ioss_0_port_6_pin_7_HSIOM HSIOM_SEL_GPIO
#endif
#define SWCLK_HSIOM ioss_0_port_6_pin_7_HSIOM
#define SWCLK_IRQ ioss_interrupts_gpio_6_IRQn

extern const cy_stc_gpio_pin_config_t ioss_0_port_12_pin_6_config;
extern const cy_stc_gpio_pin_config_t ioss_0_port_12_pin_7_config;
extern const cy_stc_gpio_pin_config_t ioss_0_port_2_pin_0_config;
extern const cy_stc_gpio_pin_config_t ioss_0_port_2_pin_1_config;
extern const cy_stc_gpio_pin_config_t ioss_0_port_2_pin_7_config;
extern const cy_stc_gpio_pin_config_t ioss_0_port_3_pin_2_config;
extern const cy_stc_gpio_pin_config_t SWO_config;
extern const cy_stc_gpio_pin_config_t SWDIO_config;
extern const cy_stc_gpio_pin_config_t SWCLK_config;

void init_cycfg_pins(void);

#if defined(__cplusplus)
}
#endif


#endif /* CYCFG_PINS_H */
