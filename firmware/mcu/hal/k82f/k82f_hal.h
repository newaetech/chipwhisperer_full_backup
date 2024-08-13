#ifndef K82F_HAL_H
#define K82F_HAL_H

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
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#include <stdint.h>

void etmtrace_enable(void);

void init_uart(void);
void putch(char c);
char getch(void);

void trigger_setup(void);
void trigger_low(void);
void trigger_high(void);

void HW_AES128_Init(void);
void HW_AES128_LoadKey(uint8_t* key);
void HW_AES128_Enc(uint8_t* pt);
void HW_AES128_Enc_pretrigger(uint8_t* pt);
void HW_AES128_Enc_posttrigger(uint8_t* pt);
void HW_AES128_Dec(uint8_t *pt);

#endif // K82F_HAL_H
