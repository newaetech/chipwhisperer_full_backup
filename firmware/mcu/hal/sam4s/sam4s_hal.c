#include "sam4s_hal.h"
#include "genclk.h"
#include "osc.h"
#include "pio.h"
#include "uart.h"
#include "sysclk.h"
#include "gpio.h"

void platform_init(void)
{
	WDT->WDT_MR = WDT_MR_WDDIS;
    sysclk_enable_peripheral_clock(ID_SUPC);
    sysclk_enable_peripheral_clock(ID_RSTC);
    sysclk_enable_peripheral_clock(ID_PMC);
    pmc_set_writeprotect(0);
    system_init_flash(CHIP_FREQ_CPU_MAX);
    pmc_switch_mck_to_sclk(SYSCLK_PRES_1);
    osc_enable(OSC_MAINCK_BYPASS);
    osc_wait_ready(OSC_MAINCK_BYPASS);
    pmc_switch_mck_to_mainck(SYSCLK_PRES_1);
    SystemCoreClockUpdate();
    system_init_flash(7.37E6);
    EFC0->EEFC_FMR &= ~(0b100 << 24); // disable code loop optimizations (causes trace desyncs)
    // EFC0->EEFC_FMR &= ~(0b001 << 16); // disable code loop optimizations (causes trace desyncs)

    sysclk_enable_peripheral_clock(ID_PIOA);
    gpio_configure_pin(PIO_PA15_IDX, PIO_OUTPUT_1 | PIO_DEFAULT);
    // while (1) {
    //     gpio_configure_pin(PIO_PA16_IDX, PIO_OUTPUT_0 | PIO_DEFAULT);
    //     for (volatile uint32_t i = 0; i < 100000; i++) {
    //     }
    //     gpio_configure_pin(PIO_PA16_IDX, PIO_OUTPUT_1 | PIO_DEFAULT);
    //     for (volatile uint32_t i = 0; i < 100000; i++) {
    //     }
    // }
    // gpio_configure_pin(PIO_PA10_IDX, PIO_PERIPH_A | PIO_DEFAULT);
}

void init_uart(void)
{
    // todo: just set these registers manually, doing so is simple and avoids asf rounding errors
    sam_uart_opt_t uopts;
    #if SS_VER == SS_VER_2_1
        uopts.ul_baudrate = 230400;
    #else
        uopts.ul_baudrate = 38400;
    #endif
    uopts.ul_mck = 7.45E6; //overestimate clock a little to avoid large baud error from calc rounding down, 38400/7.37E6 results in baud of 41k, not 38400

    uopts.ul_mode = (0x00 << 14) | (0x04 << 9);

    sysclk_enable_peripheral_clock(ID_UART0);


    uart_init(UART0, &uopts);
    uart_enable_rx(UART0);
    uart_enable_tx(UART0);
    gpio_configure_pin(PIO_PA9_IDX, PIO_PERIPH_A | PIO_DEFAULT | PIO_PULLUP);
    gpio_configure_pin(PIO_PA10_IDX, PIO_PERIPH_A | PIO_DEFAULT);
}

void putch(char c)
{
    // while (!uart_is_tx_buf_end(UART0));
    while(uart_write(UART0, c));
}

char getch(void)
{
    uint8_t c;
    while(uart_read(UART0, &c));
    return c;
}

void trigger_setup(void)
{
    sysclk_enable_peripheral_clock(ID_PIOA);
    gpio_configure_pin(PIO_PA7_IDX, PIO_OUTPUT_0 | PIO_DEFAULT);
}

void trigger_low(void)
{
    gpio_set_pin_low(PIO_PA7_IDX);
    gpio_configure_pin(PIO_PA7_IDX, PIO_OUTPUT_0 | PIO_DEFAULT);
}

void trigger_high(void)
{
    gpio_set_pin_high(PIO_PA7_IDX);
    gpio_configure_pin(PIO_PA7_IDX, PIO_OUTPUT_1 | PIO_DEFAULT);
}