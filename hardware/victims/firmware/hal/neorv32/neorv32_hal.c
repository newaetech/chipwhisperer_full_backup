#include "neorv32.h"

// #define BAUD_RATE 19200

void blink_led_c(void);

void platform_init(void)
{
//   neorv32_uart0_setup(BAUD_RATE, PARITY_NONE, FLOW_CONTROL_NONE);

//   // check if GPIO unit is implemented at all
//   if (neorv32_gpio_available() == 0) {
//     neorv32_uart0_print("Error! No GPIO unit synthesized!\n");
//     return 1; // nope, no GPIO unit synthesized
//   }
}

/**********************************************************************//**
 * C-version of blinky LED counter
 **************************************************************************/
void blink_led_c(void) {

  neorv32_gpio_port_set(0); // clear gpio output

  int cnt = 0;

  while (1) {
    neorv32_gpio_port_set(cnt++ & 0xFF); // increment counter and mask for lowest 8 bit
    neorv32_cpu_delay_ms(200); // wait 200ms using busy wait
  }
}

void init_uart(void)
{
  #if SS_VER==SS_VER_2_1
  #define BAUD_RATE 230400
  #else
  #define BAUD_RATE 38400
  #endif
  neorv32_uart0_setup(BAUD_RATE, PARITY_NONE, FLOW_CONTROL_NONE);

  // capture all exceptions and give debug info via UART
  // this is not required, but keeps us safe
  neorv32_rte_setup();
}

void trigger_setup(void)
{
    neorv32_gpio_pin_clr(1);
}

void trigger_high(void)
{
    neorv32_gpio_pin_set(1);
}

void trigger_low(void)
{
    neorv32_gpio_pin_clr(1);
}

char getch(void)
{
    return neorv32_uart0_getc();
}

void putch(char c)
{
    neorv32_uart0_putc(c);
}