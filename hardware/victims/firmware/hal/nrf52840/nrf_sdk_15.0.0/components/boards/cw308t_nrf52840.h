#ifndef CW308T_NRF52840_H
#define CW308T_NRF52840_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

// LEDs definitions for CW308T_NRF52840_H
#define LEDS_NUMBER    3

#define LED_1          NRF_GPIO_PIN_MAP(0,24)
#define LED_2          NRF_GPIO_PIN_MAP(0,22)
#define LED_3          NRF_GPIO_PIN_MAP(0,20)
#define LED_START      LED_1
#define LED_STOP       LED_3
#define XTAL_OUT       NRF_GPIO_PIN_MAP(1,10)

#define LEDS_ACTIVE_STATE 1

#define LEDS_LIST { LED_1, LED_2, LED_3 }

#define LEDS_INV_MASK  LEDS_MASK

#define BSP_LED_0      24
#define BSP_LED_1      22
#define BSP_LED_2      20

#define BUTTONS_NUMBER 0

#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST { }

#define RX_PIN_NUMBER  NRF_GPIO_PIN_MAP(1,15)
#define TX_PIN_NUMBER  NRF_GPIO_PIN_MAP(1,13)
#define CTS_PIN_NUMBER UART_PIN_DISCONNECTED
#define RTS_PIN_NUMBER UART_PIN_DISCONNECTED
#define HWFC           false

// serialization APPLICATION board - temp. setup for running serialized MEMU tests
#define SER_APP_RX_PIN              NRF_GPIO_PIN_MAP(1,15)    // UART RX pin number.
#define SER_APP_TX_PIN              NRF_GPIO_PIN_MAP(1,13)    // UART TX pin number.

// serialization CONNECTIVITY board
#define SER_CON_RX_PIN              NRF_GPIO_PIN_MAP(1,15)    // UART RX pin number.
#define SER_CON_TX_PIN              NRF_GPIO_PIN_MAP(1,13)    // UART TX pin number.

#ifdef __cplusplus
}
#endif

#endif // PCA10056_H
