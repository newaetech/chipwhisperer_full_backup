// esp-hal.c
// Implementation of ESP32 HAL functions

#include "esp-hal.h"

// TODO: can we remove these?
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "soc/uart_struct.h"
#include "hwcrypto/aes.h"
#include "driver/gpio.h"

#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"

#define ECHO_TEST_TXD  (1)
#define ECHO_TEST_RXD  (3)

#define BUF_SIZE (1024)
#define UART_NUM UART_NUM_1

static void clock_configure(void)
{
    /* Set CPU to 80MHz. Keep other clocks unmodified. */
    //uart_tx_wait_idle(0);
    rtc_clk_config_t clk_cfg = RTC_CLK_CONFIG_DEFAULT();
    clk_cfg.xtal_freq = RTC_XTAL_FREQ_26M;
    clk_cfg.cpu_freq_mhz = 26;//same clk speed as osc?
    clk_cfg.slow_freq = rtc_clk_slow_freq_get();
    clk_cfg.fast_freq = rtc_clk_fast_freq_get();
    rtc_clk_init(clk_cfg);
    /* As a slight optimization, if 32k XTAL was enabled in sdkconfig, we enable
     * it here. Usually it needs some time to start up, so we amortize at least
     * part of the start up time by enabling 32k XTAL early.
     * App startup code will wait until the oscillator has started up.
     */
#ifdef CONFIG_ESP32_RTC_CLOCK_SOURCE_EXTERNAL_CRYSTAL
    if (!rtc_clk_32k_enabled()) {
        rtc_clk_32k_bootstrap();
    }
#endif
}

void platform_init()
{
	clock_configure();
}

void init_uart()
{
    uart_config_t uart_config = {
        .baud_rate = 59000, //??? 38400->25000
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    //Configure UART1 parameters
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, 0, 0);
    //Install UART driver (we don't need an event queue here)
    //In this example we don't even use a buffer for sending data.
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
	//putch('\n');
}

void putch(char c)
{
    uart_write_bytes(UART_NUM, (const char*)&c, 1);
}

char getch()
{
    int len = 0;
    uint8_t ret;
    while(!len)
        len = uart_read_bytes(UART_NUM, &ret, 1, 20 / portTICK_RATE_MS);
    return ret;
}

void trigger_setup()
{
	gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1 << 4;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
}

void trigger_high()
{
	gpio_set_level(4, 1);
}

void trigger_low()
{
	gpio_set_level(4, 0);
}

void aes128_enc(uint8_t* pt, uint8_t* k)
{
	esp_aes_context ctx;
	esp_aes_init(&ctx);
	esp_aes_setkey(&ctx, k, 128);
	esp_aes_crypt_ecb(&ctx, ESP_AES_ENCRYPT, pt, pt);
}