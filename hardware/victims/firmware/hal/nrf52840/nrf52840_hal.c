
#include "nrf52840_hal.h"
#include <stdio.h>

#include "nrf.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_rng.h"
#include "nrf_drv_power.h"
#include "nrf_serial.h"
#include "app_timer.h"

#include "ssi_pal_types.h"
#include "ssi_pal_mem.h"
#include "sns_silib.h"
#include "ssi_aes.h"
#include "crys_aesccm.h"

#include "app_error.h"
#include "app_util.h"
#include "boards.h"

#include <stdint.h>

SaSiAesUserContext_t    ContextID;

void platform_init(void)
{
    ret_code_t ret;

    ret = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret);
    ret = nrf_drv_power_init(NULL);
    APP_ERROR_CHECK(ret);

    nrf_drv_clock_lfclk_request(NULL);
    nrf_drv_clock_hfclk_request(NULL);

    ret = app_timer_init();
    APP_ERROR_CHECK(ret);

    //while (false == nrf_drv_clock_hfclk_is_running());

    NRF_GPIOTE->CONFIG[0] = GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos |
                            GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos |
                            XTAL_OUT << GPIOTE_CONFIG_PSEL_Pos |
                            GPIOTE_CONFIG_OUTINIT_Low << GPIOTE_CONFIG_OUTINIT_Pos;
    NRF_TIMER1->PRESCALER = 0;
    NRF_TIMER1->CC[0] = 1;
    NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;
    NRF_TIMER1->TASKS_START = 1;

    NRF_PPI->CH[0].EEP = (uint32_t) &NRF_TIMER1->EVENTS_COMPARE[0];
    NRF_PPI->CH[0].TEP = (uint32_t) &NRF_GPIOTE->TASKS_OUT[0];

    NRF_PPI->CHENSET = PPI_CHENSET_CH0_Enabled << PPI_CHENSET_CH0_Pos;

    NRF_CLOCK->TRACECONFIG = (CLOCK_TRACECONFIG_TRACEMUX_Parallel << CLOCK_TRACECONFIG_TRACEMUX_Pos);

    // Initialize LEDs and buttons.
    bsp_board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);

    NVIC_EnableIRQ(CRYPTOCELL_IRQn);

    NRF_CRYPTOCELL->ENABLE = 1;

    // Start TRNG
    nrf_rng_shorts_disable(NRF_RNG_SHORT_VALRDY_STOP_MASK);
    nrf_rng_error_correction_enable();
    nrf_rng_event_clear(NRF_RNG_EVENT_VALRDY);
    nrf_rng_task_trigger(NRF_RNG_TASK_START);

    SaSi_LibInit();

}


NRF_SERIAL_DRV_UART_CONFIG_DEF(m_uart0_drv_config,
                      RX_PIN_NUMBER, TX_PIN_NUMBER,
                      0, 0,
                      NRF_UART_HWFC_DISABLED, NRF_UART_PARITY_EXCLUDED,
                      NRF_UART_BAUDRATE_38400,
                      UART_DEFAULT_CONFIG_IRQ_PRIORITY);

NRF_SERIAL_CONFIG_DEF(serial_config, NRF_SERIAL_MODE_POLLING,
                      NULL, NULL, NULL, NULL);


NRF_SERIAL_UART_DEF(serial_uart, 0);


void init_uart(void)
{
    ret_code_t ret;
    ret = nrf_serial_init(&serial_uart, &m_uart0_drv_config, &serial_config);
    APP_ERROR_CHECK(ret);
}

void trigger_setup(void)
{
    nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(0,29));
}

void trigger_high(void)
{
    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(0,29), 1);
}

void trigger_low(void)
{
    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(0,29), 0);
}

char getch(void)
{
    char c;
    nrf_serial_read(&serial_uart, &c, sizeof(c), NULL, 1000);
    return c;
}

void putch(char c)
{
    nrf_serial_write(&serial_uart,
                 &c,
                 1,
                 NULL,
                 NRF_SERIAL_MAX_TIMEOUT);
}

uint32_t get_rand(void)
{
    uint32_t value;
    int i;
    for (i = 0; i < 4; i++)
    {
        while (!nrf_rng_event_get(NRF_RNG_EVENT_VALRDY)) ;
        nrf_rng_event_clear(NRF_RNG_EVENT_VALRDY);
        value <<= 8;
        value |= nrf_rng_random_value_get();
    }
    return value;
}

void HW_AES128_Init(void)
{
    SaSi_AesInit(&ContextID,SASI_AES_ENCRYPT,SASI_AES_MODE_ECB,SASI_AES_PADDING_NONE);
}

void HW_AES128_LoadKey(uint8_t* key)
{
    SaSiAesUserKeyData_t    keyData;
    keyData.keySize = 16;
    keyData.pKey = key;
	SaSi_AesSetKey(&ContextID, SASI_AES_USER_KEY, &keyData, sizeof(keyData) );
}

void HW_AES128_Enc_pretrigger(uint8_t* pt)
{
}

void HW_AES128_Enc(uint8_t* pt)
{
    SaSi_AesBlock(&ContextID, pt, 16, pt);
}

void HW_AES128_Enc_posttrigger(uint8_t* pt)
{
}

void HW_AES128_Dec(uint8_t *pt)
{
    ;
}
