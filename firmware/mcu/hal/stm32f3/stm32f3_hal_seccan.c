#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f3xx_hal_can.h"
#include "stm32f3xx_hal_rcc.h"
#include "stm32f3xx_hal_gpio.h"
#include "stm32f3_hal_seccan.h"
#include "stm32f3xx_hal_adc.h"
#include "stm32f3xx_hal_tim.h"

static CAN_HandleTypeDef myhcan;
static CanTxMsgTypeDef msg;
static CanRxMsgTypeDef RxMessage;

typedef enum {
	CAN_RET_BAD_ADDRESS = -1,
	CAN_RET_TOO_MUCH_DATA = -2,
	CAN_RET_TIMEOUT = -3,
	CAN_RET_BUSY = -4,
	CAN_RET_ERROR = -5,
	CAN_RET_ERROR_UNKNOWN = -6
} can_return_t;

typedef enum {
	ADC_RET_OK = 0,
	ADC_RET_ADC_INIT = -1,
	ADC_RET_CHANNEL_INIT = -2,
	ADC_RET_ADC_START = -3,
	ADC_RET_ADC_TIMEOUT = -4,
	ADC_RET_ADC_STOP = -5,
	ADC_RET_PIN_INIT = -6
} adc_return_t;

//////////////////////////CAN/////////////////////////////////
static can_return_t can_return_error(HAL_StatusTypeDef canError) {
	can_return_t r_error;
	switch (canError) {
	case HAL_TIMEOUT:
		r_error = CAN_RET_TIMEOUT;
		break;
	case HAL_BUSY:
		r_error = CAN_RET_BUSY;
		break;
	case HAL_ERROR:
		r_error = CAN_RET_ERROR;
		break;
	default:
		r_error = CAN_RET_ERROR_UNKNOWN;
	}
	return r_error;
}

int init_can(void)
{
	HAL_StatusTypeDef rval;
	CAN_FilterConfTypeDef sFilterConfig;

	myhcan.Instance = CAN;
	myhcan.Init.Prescaler = 8;
	myhcan.Init.Mode = CAN_MODE_NORMAL;
	myhcan.Init.SJW = CAN_SJW_1TQ;
	myhcan.Init.BS1 = CAN_BS1_2TQ;
	myhcan.Init.BS2 = CAN_BS2_1TQ;
	myhcan.Init.TTCM = DISABLE;
	myhcan.Init.ABOM = DISABLE;
	myhcan.Init.AWUM = DISABLE;
	myhcan.Init.NART = DISABLE;
	myhcan.Init.RFLM = DISABLE;
	myhcan.Init.TXFP = DISABLE;
	
	GPIO_InitTypeDef GPIO_InitStruct;
	/* Peripheral clock enable */
	__HAL_RCC_CAN1_CLK_ENABLE()
	;
	__HAL_RCC_GPIOB_CLK_ENABLE()
	;

	/**CAN GPIO Configuration
	 PB8     ------> CAN_RX
	 PB9     ------> CAN_TX
	 */
	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF9_CAN;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	rval = HAL_CAN_Init(&myhcan);
	if (rval != HAL_OK)
		return -1;


	/*##-2- Configure the CAN Filter ###########################################*/
	sFilterConfig.FilterNumber = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = 0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.BankNumber = 14;

	if (HAL_CAN_ConfigFilter(&myhcan, &sFilterConfig) != HAL_OK) {
		/* Filter configuration Error */
		//Error_Handler();
		return -1;
	}
	msg.StdId = 0x124; //1BC;
	msg.ExtId = 0x12345ABA;
	msg.IDE = CAN_ID_EXT;
	msg.RTR = CAN_RTR_DATA;

	myhcan.pTxMsg = &msg;
	myhcan.pRxMsg = &RxMessage;
	return 0;
}

int write_can(uint32_t addr, uint8_t *data, int len)
{
	HAL_StatusTypeDef canError;
	int index;

	if (len > 8) {
		return (CAN_RET_TOO_MUCH_DATA);
	}
	msg.DLC = len;
	for (index = 0; index <= len; index++) {
		if (index >= 8) {
			break;
		}
		msg.Data[index] = *data++;
	}
	if (addr <= 0x7FF) {
		msg.StdId = addr;
		msg.IDE = CAN_ID_STD;
	} else if (addr <= 0x1FFFFFFF) {
		msg.ExtId = addr;
		msg.IDE = CAN_ID_EXT;
	} else {
		return (CAN_RET_BAD_ADDRESS);
	}

	// Transmit the data.
	canError = HAL_CAN_Transmit(&myhcan, 100);

	if (myhcan.State == HAL_CAN_STATE_TIMEOUT) {
		//restart bus?
		HAL_CAN_DeInit(&myhcan);
		init_can();
	}
	if (canError == HAL_OK) {
		return (index);
	}
	return (can_return_error(canError));
}

int read_can(uint8_t *data, uint32_t *addr, int len)
{
	HAL_StatusTypeDef canError;

	if (len < 8) {
		return (CAN_RET_TOO_MUCH_DATA);
	}
	canError = HAL_CAN_Receive(&myhcan, CAN_FIFO0, 0);

	if (canError == HAL_OK) {
		memcpy(data, RxMessage.Data, 8);
		if (RxMessage.IDE == CAN_ID_EXT) {
			*addr = RxMessage.ExtId;
		} else {
			*addr = RxMessage.StdId;
		}
		return ((int) RxMessage.DLC);
	}
	return (can_return_error(canError));
}

/////////////////////////ADC/////////////////////////////
#define ADC_READ_TIMEOUT 500
static ADC_HandleTypeDef myadc;
__weak void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);

  /* NOTE : This function should not be modified. When the callback is needed,
            function HAL_ADC_MspInit must be implemented in the user file.
   */ 
}

int init_adc(void)
{
	GPIO_InitTypeDef gpio;
	gpio.Pin = GPIO_PIN_14;
	gpio.Mode = GPIO_MODE_ANALOG;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &gpio);
	
	__HAL_RCC_ADC34_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	//init_ADC_pin();
	myadc.Instance = ADC4;

	myadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4; //div by 4 rec for 12 and 10 bit
	myadc.Init.Resolution = ADC_RESOLUTION_12B;
	myadc.Init.DataAlign = ADC_DATAALIGN_RIGHT; //lsb at bit 0
	myadc.Init.ScanConvMode = ADC_SCAN_ENABLE; //just one channel needed, so no scanning
	myadc.Init.EOCSelection = DISABLE;//ADC_EOC_SINGLE_CONV; //does this matter with one channel?
	myadc.Init.LowPowerAutoWait = DISABLE;
	myadc.Init.ContinuousConvMode = ENABLE; //single mode now
	myadc.Init.NbrOfConversion = 1; //does this matter for single channel?
	myadc.Init.DiscontinuousConvMode = DISABLE; //discarded with continuous mode
	myadc.Init.NbrOfDiscConversion = 1; //discarded without Discont mode
	myadc.Init.ExternalTrigConv = ADC_SOFTWARE_START; //start ADC with software, not ext trigger
	myadc.Init.ExternalTrigConvEdge = 0; //discarded with software start
	myadc.Init.DMAContinuousRequests = DISABLE;
	myadc.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN; //overwrite if data not read before next conversion is done

	if (HAL_ADC_Init(&myadc) != HAL_OK) {
		//send error message
		return ADC_RET_ADC_INIT;
	}

	ADC_ChannelConfTypeDef channel;
	channel.Channel = ADC_CHANNEL_4; //PB14
	channel.Rank = ADC_REGULAR_RANK_1; //single conversion, does this matter?
	channel.SamplingTime = ADC_SAMPLETIME_19CYCLES_5;
	channel.SingleDiff = ADC_SINGLE_ENDED;
	channel.OffsetNumber = ADC_OFFSET_NONE;
	channel.Offset = 0;

	if (HAL_ADC_ConfigChannel(&myadc, &channel) != HAL_OK) {
		//send error
		return ADC_RET_CHANNEL_INIT;
	}

	if (HAL_ADC_Start(&myadc) != HAL_OK) {
			return ADC_RET_ADC_START;
	}

	return 0;
}

int read_adc(uint16_t *val)
{
	if (HAL_ADC_PollForConversion(&myadc, ADC_READ_TIMEOUT) == HAL_OK) {
		//good to read
		*val = HAL_ADC_GetValue(&myadc);
	} else {
		//error
		return ADC_RET_ADC_TIMEOUT;
	}

	return 0;
}

/////////////////////PWM//////////////////////////////
static TIM_OC_InitTypeDef pwm;

static TIM_HandleTypeDef tim;

int init_pwm(void)
{
	HAL_StatusTypeDef rtn;
	__HAL_RCC_TIM1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef GpioInit;
	GpioInit.Pin       = GPIO_PIN_11;
	GpioInit.Mode      = GPIO_MODE_AF_PP;
	GpioInit.Pull      = GPIO_NOPULL;
	GpioInit.Speed     = GPIO_SPEED_FREQ_HIGH;
	GpioInit.Alternate = GPIO_AF11_TIM1;
	HAL_GPIO_Init(GPIOA, &GpioInit);


	tim.Instance = TIM1;
	tim.Init.CounterMode = TIM_COUNTERMODE_DOWN;
	tim.Init.Prescaler = 0;
	tim.Init.Period = 0xFFFF;
	tim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	tim.Init.RepetitionCounter = 0x0;
	tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;


	pwm.OCMode = TIM_OCMODE_PWM1;
	pwm.Pulse = 0;
	pwm.OCPolarity = TIM_OCPOLARITY_HIGH;
	pwm.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	pwm.OCFastMode = TIM_OCFAST_ENABLE; //s;nic
	pwm.OCIdleState = TIM_OCIDLESTATE_RESET;
	pwm.OCNIdleState = TIM_OCNIDLESTATE_RESET;

	tim.Channel = HAL_TIM_ACTIVE_CHANNEL_4;

	rtn = HAL_TIM_PWM_Init(&tim);

	rtn = HAL_TIM_PWM_ConfigChannel(&tim, &pwm, TIM_CHANNEL_4);
	rtn = HAL_TIM_PWM_Start(&tim, TIM_CHANNEL_4);

	if (rtn != HAL_OK) {
		return -1;
	}

	return 0;
}

void change_pwm(uint16_t duty_cycle)
{
	pwm.Pulse = duty_cycle;
	HAL_TIM_PWM_ConfigChannel(&tim, &pwm, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&tim, TIM_CHANNEL_4);
}