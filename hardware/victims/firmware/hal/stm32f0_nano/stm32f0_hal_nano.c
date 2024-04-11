
#include "stm32f0_hal.h"
#include "stm32f0_hal_lowlevel.h"
#include "stm32f0xx_hal_rcc.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_dma.h"
#include "stm32f0xx_hal_uart.h"
#include "stm32f0xx_hal_flash.h"

UART_HandleTypeDef UartHandle;


void platform_init(void)
{
    /* LEDs */
    GPIO_InitTypeDef GpioInit;
	GpioInit.Pin       = GPIO_PIN_2;
	GpioInit.Mode      = GPIO_MODE_OUTPUT_PP;
	GpioInit.Pull      = GPIO_NOPULL;
	GpioInit.Speed     = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GpioInit);
    
	GpioInit.Pin       = GPIO_PIN_4;
	GpioInit.Mode      = GPIO_MODE_OUTPUT_PP;
	GpioInit.Pull      = GPIO_NOPULL;
	GpioInit.Speed     = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GpioInit);

#ifdef USE_INTERNAL_CLK
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSEState       = RCC_HSE_OFF;
  RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLSource  = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_ClkInitStruct.ClockType      = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  uint32_t flash_latency = 0;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, flash_latency);
#else
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSEState       = RCC_HSE_BYPASS;
	RCC_OscInitStruct.HSIState       = RCC_HSI_OFF;
	RCC_OscInitStruct.PLL.PLLSource  = RCC_PLL_NONE;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_ClkInitStruct.ClockType      = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
	RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSE;
	RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
#endif
}

void init_uart(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef GpioInit;
	GpioInit.Pin       = GPIO_PIN_9 | GPIO_PIN_10;
	GpioInit.Mode      = GPIO_MODE_AF_PP;
	GpioInit.Pull      = GPIO_PULLUP;
	GpioInit.Speed     = GPIO_SPEED_FREQ_HIGH;
	GpioInit.Alternate = GPIO_AF1_USART1;
	HAL_GPIO_Init(GPIOA, &GpioInit);

	__HAL_RCC_USART1_CLK_ENABLE();
	__HAL_RCC_USART1_CONFIG(RCC_USART1CLKSOURCE_SYSCLK);
	UartHandle.Instance        = USART1;
  #if SS_VER==SS_VER_2_1
  UartHandle.Init.BaudRate   = 230400;
  #else
  UartHandle.Init.BaudRate   = 38400;
  #endif
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits   = UART_STOPBITS_1;
	UartHandle.Init.Parity     = UART_PARITY_NONE;
	UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode       = UART_MODE_TX_RX;
	HAL_UART_Init(&UartHandle);
}

void trigger_setup(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_InitTypeDef GpioInit;
	GpioInit.Pin       = GPIO_PIN_7;
	GpioInit.Mode      = GPIO_MODE_OUTPUT_PP;
	GpioInit.Pull      = GPIO_NOPULL;
	GpioInit.Speed     = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GpioInit);

	GpioInit.Pin       = GPIO_PIN_4;
	GpioInit.Mode      = GPIO_MODE_OUTPUT_PP;
	GpioInit.Pull      = GPIO_NOPULL;
	GpioInit.Speed     = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GpioInit);

	GpioInit.Pin       = GPIO_PIN_2;
	GpioInit.Mode      = GPIO_MODE_OUTPUT_PP;
	GpioInit.Pull      = GPIO_NOPULL;
	GpioInit.Speed     = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GpioInit);
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, SET);
}

void trigger_high(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, SET);
}

void trigger_low(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, RESET);
}   

char getch(void)
{
	uint8_t d;
	while(HAL_UART_Receive(&UartHandle, &d, 1, 50) != HAL_OK)
		USART1->ICR |= (1 << 3); // make sure overrun error is cleared, otherwise can stall here
	return d;
}

void putch(char c)
{
	uint8_t d  = c;
	HAL_UART_Transmit(&UartHandle,  &d, 1, 5000);
}


void led_error(unsigned int status)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, status);
}

void led_ok(unsigned int status)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, status);
}
