
#include "stm32f4_hal.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_uart.h"

UART_HandleTypeDef UartHandle;


void platform_init(void)
{
	//HAL_Init();

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSEState       = RCC_HSE_BYPASS;
	RCC_OscInitStruct.HSIState       = RCC_HSI_OFF;
	RCC_OscInitStruct.PLL.PLLSource  = RCC_PLL_NONE;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_ClkInitStruct.ClockType      = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSE;
	RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_ACR_LATENCY_5WS);
}

void init_uart(void)
{
	GPIO_InitTypeDef GpioInit;
	GpioInit.Pin       = GPIO_PIN_9 | GPIO_PIN_10;
	GpioInit.Mode      = GPIO_MODE_AF_PP;
	GpioInit.Pull      = GPIO_PULLUP;
	GpioInit.Speed     = GPIO_SPEED_FREQ_HIGH;
	GpioInit.Alternate = GPIO_AF7_USART1;
	__GPIOA_CLK_ENABLE();
	HAL_GPIO_Init(GPIOA, &GpioInit);

	UartHandle.Instance        = USART1;
	UartHandle.Init.BaudRate   = 38400;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits   = UART_STOPBITS_1;
	UartHandle.Init.Parity     = UART_PARITY_NONE;
	UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode       = UART_MODE_TX_RX;
	__USART1_CLK_ENABLE();
	HAL_UART_Init(&UartHandle);
}

void aes_indep_init(void){
	return;
}
void aes_indep_key(uint8_t * key){
	return;
}

void aes_indep_enc(uint8_t * key){
	return;
}

char getch(void)
{
	uint8_t d;
	HAL_UART_Receive(&UartHandle, &d, 1, 5000);
	return d;
}

void putch(char c)
{
	uint8_t d  = c;
	HAL_UART_Transmit(&UartHandle,  &d, 1, 5000);
}

