#ifndef STM32F3_HAL_H
#define STM32F3_HAL_H
#include <stdint.h>

//You probably don't need this from rest of code
//#include "stm32f4_hal_lowlevel.h"

#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *         which reports the name of the source file and the source
  *         line number of the call that failed.
  *         If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  //#define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

void init_uart(void);
void putch(char c);
char getch(void);

void trigger_setup(void);
void trigger_low(void);
void trigger_high(void);


#if (PLATFORM==CWLITEARM)
void change_err_led(unsigned int x);
void change_ok_led(unsigned int x);
#define led_error(X) (change_err_led(X))
#define led_ok(X) (change_ok_led(X))
#endif

#endif // STM32F3_HAL_H
