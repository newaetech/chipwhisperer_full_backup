/* This file combines several STM32F4 HAL Functions into one file. This was done
   for space reasons, to avoid having several MB of HAL functions that most people
   will not use. In addition this HAL is slightly less demanding (no interrupts),
   but less robust as doesn't implement the timeouts.
   
   The original HAL files are COPYRIGHT STMicroelectronics, as shown below:
*/

/*
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
*/ 


#include "stm32f4_hal.h"
#include "stm32f4_hal_lowlevel.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_cryp.h"

#define assert_param(expr) ((void)0U)

#define CLOCKSWITCH_TIMEOUT_VALUE  5000U /* 5 s */


uint32_t HAL_GetTick(void)
{
	static uint32_t tick;
	return tick++;;
}

/**
  * @brief  Enables or disables the AHB2 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it. 
  * @param  RCC_AHBPeriph: specifies the AHB2 peripheral to gates its clock.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_AHB2Periph_DCMI:   DCMI clock
  *            @arg RCC_AHB2Periph_CRYP:   CRYP clock
  *            @arg RCC_AHB2Periph_HASH:   HASH clock
  *            @arg RCC_AHB2Periph_RNG:    RNG clock
  *            @arg RCC_AHB2Periph_OTG_FS: USB OTG FS clock
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHB2PeriphClockCmd(uint32_t RCC_AHB2Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB2_PERIPH(RCC_AHB2Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    RCC->AHB2ENR |= RCC_AHB2Periph;
  }
  else
  {
    RCC->AHB2ENR &= ~RCC_AHB2Periph;
  }
}

/**
  * @brief  Initializes the RCC Oscillators according to the specified parameters in the
  *         RCC_OscInitTypeDef.
  * @param  RCC_OscInitStruct: pointer to an RCC_OscInitTypeDef structure that
  *         contains the configuration information for the RCC Oscillators.
  * @note   The PLL is not disabled when used as system clock.
  * @note   Transitions LSE Bypass to LSE On and LSE On to LSE Bypass are not
  *         supported by this API. User should request a transition to LSE Off
  *         first and then LSE On or LSE Bypass.
  * @note   Transition HSE Bypass to HSE On and HSE On to HSE Bypass are not
  *         supported by this API. User should request a transition to HSE Off
  *         first and then HSE On or HSE Bypass.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef  *RCC_OscInitStruct)
{
  uint32_t tickstart = 0U;

  /* Check the parameters */
  assert_param(IS_RCC_OSCILLATORTYPE(RCC_OscInitStruct->OscillatorType));
  /*------------------------------- HSE Configuration ------------------------*/
  if(((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_HSE) == RCC_OSCILLATORTYPE_HSE)
  {
    /* Check the parameters */
    assert_param(IS_RCC_HSE(RCC_OscInitStruct->HSEState));
    /* When the HSE is used as system clock or clock source for PLL in these cases HSE will not disabled */
    if((__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_CFGR_SWS_HSE) ||\
      ((__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_CFGR_SWS_PLL) && ((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) == RCC_PLLCFGR_PLLSRC_HSE)))
    {
      if((__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) != RESET) && (RCC_OscInitStruct->HSEState == RCC_HSE_OFF))
      {
        return HAL_ERROR;
      }
    }
    else
    {
      /* Set the new HSE configuration ---------------------------------------*/
      __HAL_RCC_HSE_CONFIG(RCC_OscInitStruct->HSEState);

      /* Check the HSE State */
      if((RCC_OscInitStruct->HSEState) != RCC_HSE_OFF)
      {
        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till HSE is ready */
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET)
        {
          if((HAL_GetTick() - tickstart ) > HSE_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
      else
      {
        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till HSE is bypassed or disabled */
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) != RESET)
        {
          if((HAL_GetTick() - tickstart ) > HSE_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
    }
  }
  /*----------------------------- HSI Configuration --------------------------*/
  if(((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_HSI) == RCC_OSCILLATORTYPE_HSI)
  {
    /* Check the parameters */
    assert_param(IS_RCC_HSI(RCC_OscInitStruct->HSIState));
    assert_param(IS_RCC_CALIBRATION_VALUE(RCC_OscInitStruct->HSICalibrationValue));

    /* Check if HSI is used as system clock or as PLL source when PLL is selected as system clock */
    if((__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_CFGR_SWS_HSI) ||\
      ((__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_CFGR_SWS_PLL) && ((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) == RCC_PLLCFGR_PLLSRC_HSI)))
    {
      /* When HSI is used as system clock it will not disabled */
      if((__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) != RESET) && (RCC_OscInitStruct->HSIState != RCC_HSI_ON))
      {
        return HAL_ERROR;
      }
      /* Otherwise, just the calibration is allowed */
      else
      {
        /* Adjusts the Internal High Speed oscillator (HSI) calibration value.*/
        __HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(RCC_OscInitStruct->HSICalibrationValue);
      }
    }
    else
    {
      /* Check the HSI State */
      if((RCC_OscInitStruct->HSIState)!= RCC_HSI_OFF)
      {
        /* Enable the Internal High Speed oscillator (HSI). */
        __HAL_RCC_HSI_ENABLE();

        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till HSI is ready */
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) == RESET)
        {
          if((HAL_GetTick() - tickstart ) > HSI_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }

        /* Adjusts the Internal High Speed oscillator (HSI) calibration value.*/
        __HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(RCC_OscInitStruct->HSICalibrationValue);
      }
      else
      {
        /* Disable the Internal High Speed oscillator (HSI). */
        __HAL_RCC_HSI_DISABLE();

        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till HSI is ready */
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) != RESET)
        {
          if((HAL_GetTick() - tickstart ) > HSI_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
    }
  }
  /*------------------------------ LSI Configuration -------------------------*/
  if(((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_LSI) == RCC_OSCILLATORTYPE_LSI)
  {
    /* Check the parameters */
    assert_param(IS_RCC_LSI(RCC_OscInitStruct->LSIState));

    /* Check the LSI State */
    if((RCC_OscInitStruct->LSIState)!= RCC_LSI_OFF)
    {
      /* Enable the Internal Low Speed oscillator (LSI). */
      __HAL_RCC_LSI_ENABLE();

      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Wait till LSI is ready */
      while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET)
      {
        if((HAL_GetTick() - tickstart ) > LSI_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
    else
    {
      /* Disable the Internal Low Speed oscillator (LSI). */
      __HAL_RCC_LSI_DISABLE();

      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Wait till LSI is ready */
      while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) != RESET)
      {
        if((HAL_GetTick() - tickstart ) > LSI_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
  }
  /*------------------------------ LSE Configuration -------------------------*/
  if(((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_LSE) == RCC_OSCILLATORTYPE_LSE)
  {
    /* Check the parameters */
    assert_param(IS_RCC_LSE(RCC_OscInitStruct->LSEState));

    /* Enable Power Clock*/
    __HAL_RCC_PWR_CLK_ENABLE();

    /* Enable write access to Backup domain */
    PWR->CR |= PWR_CR_DBP;

    /* Wait for Backup domain Write protection enable */
    tickstart = HAL_GetTick();

    while((PWR->CR & PWR_CR_DBP) == RESET)
    {
      if((HAL_GetTick() - tickstart ) > RCC_DBP_TIMEOUT_VALUE)
      {
        return HAL_TIMEOUT;
      }
    }

    /* Set the new LSE configuration -----------------------------------------*/
    __HAL_RCC_LSE_CONFIG(RCC_OscInitStruct->LSEState);
    /* Check the LSE State */
    if((RCC_OscInitStruct->LSEState) != RCC_LSE_OFF)
    {
      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Wait till LSE is ready */
      while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET)
      {
        if((HAL_GetTick() - tickstart ) > RCC_LSE_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
    else
    {
      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Wait till LSE is ready */
      while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) != RESET)
      {
        if((HAL_GetTick() - tickstart ) > RCC_LSE_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
  }
  /*-------------------------------- PLL Configuration -----------------------*/
  /* Check the parameters */
  assert_param(IS_RCC_PLL(RCC_OscInitStruct->PLL.PLLState));
  if ((RCC_OscInitStruct->PLL.PLLState) != RCC_PLL_NONE)
  {
    /* Check if the PLL is used as system clock or not */
    if(__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_CFGR_SWS_PLL)
    {
      if((RCC_OscInitStruct->PLL.PLLState) == RCC_PLL_ON)
      {
        /* Check the parameters */
        assert_param(IS_RCC_PLLSOURCE(RCC_OscInitStruct->PLL.PLLSource));
        assert_param(IS_RCC_PLLM_VALUE(RCC_OscInitStruct->PLL.PLLM));
        assert_param(IS_RCC_PLLN_VALUE(RCC_OscInitStruct->PLL.PLLN));
        assert_param(IS_RCC_PLLP_VALUE(RCC_OscInitStruct->PLL.PLLP));
        assert_param(IS_RCC_PLLQ_VALUE(RCC_OscInitStruct->PLL.PLLQ));

        /* Disable the main PLL. */
        __HAL_RCC_PLL_DISABLE();

        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till PLL is ready */
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) != RESET)
        {
          if((HAL_GetTick() - tickstart ) > PLL_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }

        /* Configure the main PLL clock source, multiplication and division factors. */
        WRITE_REG(RCC->PLLCFGR, (RCC_OscInitStruct->PLL.PLLSource                                            | \
                                 RCC_OscInitStruct->PLL.PLLM                                                 | \
                                 (RCC_OscInitStruct->PLL.PLLN << POSITION_VAL(RCC_PLLCFGR_PLLN))             | \
                                 (((RCC_OscInitStruct->PLL.PLLP >> 1U) - 1U) << POSITION_VAL(RCC_PLLCFGR_PLLP)) | \
                                 (RCC_OscInitStruct->PLL.PLLQ << POSITION_VAL(RCC_PLLCFGR_PLLQ))));
        /* Enable the main PLL. */
        __HAL_RCC_PLL_ENABLE();

        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till PLL is ready */
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET)
        {
          if((HAL_GetTick() - tickstart ) > PLL_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
      else
      {
        /* Disable the main PLL. */
        __HAL_RCC_PLL_DISABLE();

        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till PLL is ready */
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) != RESET)
        {
          if((HAL_GetTick() - tickstart ) > PLL_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
    }
    else
    {
      return HAL_ERROR;
    }
  }
  return HAL_OK;
}

/**
  * @brief  Initializes the CPU, AHB and APB busses clocks according to the specified
  *         parameters in the RCC_ClkInitStruct.
  * @param  RCC_ClkInitStruct: pointer to an RCC_OscInitTypeDef structure that
  *         contains the configuration information for the RCC peripheral.
  * @param  FLatency: FLASH Latency, this parameter depend on device selected
  *
  * @note   The SystemCoreClock CMSIS variable is used to store System Clock Frequency
  *         and updated by HAL_RCC_GetHCLKFreq() function called within this function
  *
  * @note   The HSI is used (enabled by hardware) as system clock source after
  *         startup from Reset, wake-up from STOP and STANDBY mode, or in case
  *         of failure of the HSE used directly or indirectly as system clock
  *         (if the Clock Security System CSS is enabled).
  *
  * @note   A switch from one clock source to another occurs only if the target
  *         clock source is ready (clock stable after startup delay or PLL locked).
  *         If a clock source which is not yet ready is selected, the switch will
  *         occur when the clock source will be ready.
  *
  * @note   Depending on the device voltage range, the software has to set correctly
  *         HPRE[3:0] bits to ensure that HCLK not exceed the maximum allowed frequency
  *         (for more details refer to section above "Initialization/de-initialization functions")
  * @retval None
  */
HAL_StatusTypeDef HAL_RCC_ClockConfig(RCC_ClkInitTypeDef  *RCC_ClkInitStruct, uint32_t FLatency)
{
  uint32_t tickstart = 0U;

  /* Check the parameters */
  assert_param(IS_RCC_CLOCKTYPE(RCC_ClkInitStruct->ClockType));
  assert_param(IS_FLASH_LATENCY(FLatency));

  /* To correctly read data from FLASH memory, the number of wait states (LATENCY)
    must be correctly programmed according to the frequency of the CPU clock
    (HCLK) and the supply voltage of the device. */

  /* Increasing the number of wait states because of higher CPU frequency */
  if(FLatency > (FLASH->ACR & FLASH_ACR_LATENCY))
  {
    /* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */
    __HAL_FLASH_SET_LATENCY(FLatency);

    /* Check that the new number of wait states is taken into account to access the Flash
    memory by reading the FLASH_ACR register */
    if((FLASH->ACR & FLASH_ACR_LATENCY) != FLatency)
    {
      return HAL_ERROR;
    }
  }

  /*-------------------------- HCLK Configuration --------------------------*/
  if(((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_HCLK) == RCC_CLOCKTYPE_HCLK)
  {
    assert_param(IS_RCC_HCLK(RCC_ClkInitStruct->AHBCLKDivider));
    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_ClkInitStruct->AHBCLKDivider);
  }

  /*------------------------- SYSCLK Configuration ---------------------------*/
  if(((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_SYSCLK) == RCC_CLOCKTYPE_SYSCLK)
  {
    assert_param(IS_RCC_SYSCLKSOURCE(RCC_ClkInitStruct->SYSCLKSource));

    /* HSE is selected as System Clock Source */
    if(RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_HSE)
    {
      /* Check the HSE ready flag */
      if(__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET)
      {
        return HAL_ERROR;
      }
    }
    /* PLL is selected as System Clock Source */
    else if((RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_PLLCLK)   ||
            (RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_PLLRCLK))
    {
      /* Check the PLL ready flag */
      if(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET)
      {
        return HAL_ERROR;
      }
    }
    /* HSI is selected as System Clock Source */
    else
    {
      /* Check the HSI ready flag */
      if(__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) == RESET)
      {
        return HAL_ERROR;
      }
    }

    __HAL_RCC_SYSCLK_CONFIG(RCC_ClkInitStruct->SYSCLKSource);
    /* Get Start Tick*/
    tickstart = HAL_GetTick();

    if(RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_HSE)
    {
      while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_HSE)
      {
        if((HAL_GetTick() - tickstart ) > CLOCKSWITCH_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
    else if(RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_PLLCLK)
    {
      while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_PLLCLK)
      {
        if((HAL_GetTick() - tickstart ) > CLOCKSWITCH_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
    else if(RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_PLLRCLK)
    {
      while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_PLLRCLK)
      {
        if((HAL_GetTick() - tickstart ) > CLOCKSWITCH_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
    else
    {
      while(__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_HSI)
      {
        if((HAL_GetTick() - tickstart ) > CLOCKSWITCH_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
  }

  /* Decreasing the number of wait states because of lower CPU frequency */
  if(FLatency < (FLASH->ACR & FLASH_ACR_LATENCY))
  {
     /* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */
    __HAL_FLASH_SET_LATENCY(FLatency);

    /* Check that the new number of wait states is taken into account to access the Flash
    memory by reading the FLASH_ACR register */
    if((FLASH->ACR & FLASH_ACR_LATENCY) != FLatency)
    {
      return HAL_ERROR;
    }
 }

  /*-------------------------- PCLK1 Configuration ---------------------------*/
  if(((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK1) == RCC_CLOCKTYPE_PCLK1)
  {
    assert_param(IS_RCC_PCLK(RCC_ClkInitStruct->APB1CLKDivider));
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_ClkInitStruct->APB1CLKDivider);
  }

  /*-------------------------- PCLK2 Configuration ---------------------------*/
  if(((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK2) == RCC_CLOCKTYPE_PCLK2)
  {
    assert_param(IS_RCC_PCLK(RCC_ClkInitStruct->APB2CLKDivider));
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, ((RCC_ClkInitStruct->APB2CLKDivider) << 3U));
  }

  /* Update the SystemCoreClock global variable */
  //SystemCoreClock = HAL_RCC_GetSysClockFreq() >> AHBPrescTable[(RCC->CFGR & RCC_CFGR_HPRE)>> POSITION_VAL(RCC_CFGR_HPRE)];

  /* Configure the source of time base considering new system clocks settings*/
  //HAL_InitTick (TICK_INT_PRIORITY);

  return HAL_OK;
}

/**
  * @brief  Returns the PCLK1 frequency
  * @note   Each time PCLK1 changes, this function must be called to update the
  *         right PCLK1 value. Otherwise, any configuration based on this function will be incorrect.
  * @retval PCLK1 frequency
  */
uint32_t HAL_RCC_GetPCLK1Freq(void)
{
  /* Get HCLK source and Compute PCLK1 frequency ---------------------------*/
  //return (HAL_RCC_GetHCLKFreq() >> APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE1)>> POSITION_VAL(RCC_CFGR_PPRE1)]);
  return 7.37E6;
}

/**
  * @brief  Returns the PCLK2 frequency
  * @note   Each time PCLK2 changes, this function must be called to update the
  *         right PCLK2 value. Otherwise, any configuration based on this function will be incorrect.
  * @retval PCLK2 frequency
  */
uint32_t HAL_RCC_GetPCLK2Freq(void)
{
  /* Get HCLK source and Compute PCLK2 frequency ---------------------------*/
  //return (HAL_RCC_GetHCLKFreq()>> APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE2)>> POSITION_VAL(RCC_CFGR_PPRE2)]);
  return 7.37E6;
}



#define GPIO_MODE             0x00000003U
#define EXTI_MODE             0x10000000U
#define GPIO_MODE_IT          0x00010000U
#define GPIO_MODE_EVT         0x00020000U
#define RISING_EDGE           0x00100000U
#define FALLING_EDGE          0x00200000U
#define GPIO_OUTPUT_TYPE      0x00000010U

#define GPIO_NUMBER           16U

/**
  * @brief  Initializes the GPIOx peripheral according to the specified parameters in the GPIO_Init.
  * @param  GPIOx: where x can be (A..K) to select the GPIO peripheral for STM32F429X device or
  *                      x can be (A..I) to select the GPIO peripheral for STM32F40XX and STM32F427X devices.
  * @param  GPIO_Init: pointer to a GPIO_InitTypeDef structure that contains
  *         the configuration information for the specified GPIO peripheral.
  * @retval None
  */
void HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
  uint32_t position;
  uint32_t ioposition = 0x00U;
  uint32_t iocurrent = 0x00U;
  uint32_t temp = 0x00U;

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Init->Pin));
  assert_param(IS_GPIO_MODE(GPIO_Init->Mode));
  assert_param(IS_GPIO_PULL(GPIO_Init->Pull));

  /* Configure the port pins */
  for(position = 0U; position < GPIO_NUMBER; position++)
  {
    /* Get the IO position */
    ioposition = 0x01U << position;
    /* Get the current IO position */
    iocurrent = (uint32_t)(GPIO_Init->Pin) & ioposition;

    if(iocurrent == ioposition)
    {
      /*--------------------- GPIO Mode Configuration ------------------------*/
      /* In case of Alternate function mode selection */
      if((GPIO_Init->Mode == GPIO_MODE_AF_PP) || (GPIO_Init->Mode == GPIO_MODE_AF_OD))
      {
        /* Check the Alternate function parameter */
        assert_param(IS_GPIO_AF(GPIO_Init->Alternate));
        /* Configure Alternate function mapped with the current IO */
        temp = GPIOx->AFR[position >> 3U];
        temp &= ~(0xFU << ((uint32_t)(position & 0x07U) * 4U)) ;
        temp |= ((uint32_t)(GPIO_Init->Alternate) << (((uint32_t)position & 0x07U) * 4U));
        GPIOx->AFR[position >> 3U] = temp;
      }

      /* Configure IO Direction mode (Input, Output, Alternate or Analog) */
      temp = GPIOx->MODER;
      temp &= ~(GPIO_MODER_MODER0 << (position * 2U));
      temp |= ((GPIO_Init->Mode & GPIO_MODE) << (position * 2U));
      GPIOx->MODER = temp;

      /* In case of Output or Alternate function mode selection */
      if((GPIO_Init->Mode == GPIO_MODE_OUTPUT_PP) || (GPIO_Init->Mode == GPIO_MODE_AF_PP) ||
         (GPIO_Init->Mode == GPIO_MODE_OUTPUT_OD) || (GPIO_Init->Mode == GPIO_MODE_AF_OD))
      {
        /* Check the Speed parameter */
        assert_param(IS_GPIO_SPEED(GPIO_Init->Speed));
        /* Configure the IO Speed */
        temp = GPIOx->OSPEEDR;
        temp &= ~(GPIO_OSPEEDER_OSPEEDR0 << (position * 2U));
        temp |= (GPIO_Init->Speed << (position * 2U));
        GPIOx->OSPEEDR = temp;

        /* Configure the IO Output Type */
        temp = GPIOx->OTYPER;
        temp &= ~(GPIO_OTYPER_OT_0 << position) ;
        temp |= (((GPIO_Init->Mode & GPIO_OUTPUT_TYPE) >> 4U) << position);
        GPIOx->OTYPER = temp;
      }

      /* Activate the Pull-up or Pull down resistor for the current IO */
      temp = GPIOx->PUPDR;
      temp &= ~(GPIO_PUPDR_PUPDR0 << (position * 2U));
      temp |= ((GPIO_Init->Pull) << (position * 2U));
      GPIOx->PUPDR = temp;

      /*--------------------- EXTI Mode Configuration ------------------------*/
      /* Configure the External Interrupt or event for the current IO */
      if((GPIO_Init->Mode & EXTI_MODE) == EXTI_MODE)
      {
        /* Enable SYSCFG Clock */
        __HAL_RCC_SYSCFG_CLK_ENABLE();

        temp = SYSCFG->EXTICR[position >> 2U];
        temp &= ~(0x0FU << (4U * (position & 0x03U)));
        temp |= ((uint32_t)(GPIO_GET_INDEX(GPIOx)) << (4U * (position & 0x03U)));
        SYSCFG->EXTICR[position >> 2U] = temp;

        /* Clear EXTI line configuration */
        temp = EXTI->IMR;
        temp &= ~((uint32_t)iocurrent);
        if((GPIO_Init->Mode & GPIO_MODE_IT) == GPIO_MODE_IT)
        {
          temp |= iocurrent;
        }
        EXTI->IMR = temp;

        temp = EXTI->EMR;
        temp &= ~((uint32_t)iocurrent);
        if((GPIO_Init->Mode & GPIO_MODE_EVT) == GPIO_MODE_EVT)
        {
          temp |= iocurrent;
        }
        EXTI->EMR = temp;

        /* Clear Rising Falling edge configuration */
        temp = EXTI->RTSR;
        temp &= ~((uint32_t)iocurrent);
        if((GPIO_Init->Mode & RISING_EDGE) == RISING_EDGE)
        {
          temp |= iocurrent;
        }
        EXTI->RTSR = temp;

        temp = EXTI->FTSR;
        temp &= ~((uint32_t)iocurrent);
        if((GPIO_Init->Mode & FALLING_EDGE) == FALLING_EDGE)
        {
          temp |= iocurrent;
        }
        EXTI->FTSR = temp;
      }
    }
  }
}

/**
  * @brief  Sets or clears the selected data port bit.
  *
  * @note   This function uses GPIOx_BSRR register to allow atomic read/modify
  *         accesses. In this way, there is no risk of an IRQ occurring between
  *         the read and the modify access.
  *
  * @param  GPIOx: where x can be (A..K) to select the GPIO peripheral for STM32F429X device or
  *                      x can be (A..I) to select the GPIO peripheral for STM32F40XX and STM32F427X devices.
  * @param  GPIO_Pin: specifies the port bit to be written.
  *          This parameter can be one of GPIO_PIN_x where x can be (0..15).
  * @param  PinState: specifies the value to be written to the selected bit.
  *          This parameter can be one of the GPIO_PinState enum values:
  *            @arg GPIO_PIN_RESET: to clear the port pin
  *            @arg GPIO_PIN_SET: to set the port pin
  * @retval None
  */
void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
  /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Pin));
  assert_param(IS_GPIO_PIN_ACTION(PinState));

  if(PinState != GPIO_PIN_RESET)
  {
    GPIOx->BSRR = GPIO_Pin;
  }
  else
  {
    GPIOx->BSRR = (uint32_t)GPIO_Pin << 16U;
  }
}


/**
  * @brief  Configures the UART peripheral.
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
static void UART_SetConfig(UART_HandleTypeDef *huart)
{
  uint32_t tmpreg = 0x00U;

  /* Check the parameters */
  assert_param(IS_UART_BAUDRATE(huart->Init.BaudRate));
  assert_param(IS_UART_STOPBITS(huart->Init.StopBits));
  assert_param(IS_UART_PARITY(huart->Init.Parity));
  assert_param(IS_UART_MODE(huart->Init.Mode));

  /*-------------------------- USART CR2 Configuration -----------------------*/
  tmpreg = huart->Instance->CR2;

  /* Clear STOP[13:12] bits */
  tmpreg &= (uint32_t)~((uint32_t)USART_CR2_STOP);

  /* Configure the UART Stop Bits: Set STOP[13:12] bits according to huart->Init.StopBits value */
  tmpreg |= (uint32_t)huart->Init.StopBits;

  /* Write to USART CR2 */
  WRITE_REG(huart->Instance->CR2, (uint32_t)tmpreg);

  /*-------------------------- USART CR1 Configuration -----------------------*/
  tmpreg = huart->Instance->CR1;

  /* Clear M, PCE, PS, TE and RE bits */
  tmpreg &= (uint32_t)~((uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | \
                                   USART_CR1_RE | USART_CR1_OVER8));

  /* Configure the UART Word Length, Parity and mode:
     Set the M bits according to huart->Init.WordLength value
     Set PCE and PS bits according to huart->Init.Parity value
     Set TE and RE bits according to huart->Init.Mode value
     Set OVER8 bit according to huart->Init.OverSampling value */
  tmpreg |= (uint32_t)huart->Init.WordLength | huart->Init.Parity | huart->Init.Mode | huart->Init.OverSampling;

  /* Write to USART CR1 */
  WRITE_REG(huart->Instance->CR1, (uint32_t)tmpreg);

  /*-------------------------- USART CR3 Configuration -----------------------*/
  tmpreg = huart->Instance->CR3;

  /* Clear CTSE and RTSE bits */
  tmpreg &= (uint32_t)~((uint32_t)(USART_CR3_RTSE | USART_CR3_CTSE));

  /* Configure the UART HFC: Set CTSE and RTSE bits according to huart->Init.HwFlowCtl value */
  tmpreg |= huart->Init.HwFlowCtl;

  /* Write to USART CR3 */
  WRITE_REG(huart->Instance->CR3, (uint32_t)tmpreg);

  /* Check the Over Sampling */
  if(huart->Init.OverSampling == UART_OVERSAMPLING_8)
  {
    /*-------------------------- USART BRR Configuration ---------------------*/
#if defined(USART6)
    if((huart->Instance == USART1) || (huart->Instance == USART6))
    {
      huart->Instance->BRR = UART_BRR_SAMPLING8(HAL_RCC_GetPCLK2Freq(), huart->Init.BaudRate);
    }
#else
    if(huart->Instance == USART1)
    {
      huart->Instance->BRR = UART_BRR_SAMPLING8(HAL_RCC_GetPCLK2Freq(), huart->Init.BaudRate);
    }
#endif /* USART6 */
    else
    {
      huart->Instance->BRR = UART_BRR_SAMPLING8(HAL_RCC_GetPCLK1Freq(), huart->Init.BaudRate);
    }
  }
  else
  {
    /*-------------------------- USART BRR Configuration ---------------------*/
#if defined(USART6)
    if((huart->Instance == USART1) || (huart->Instance == USART6))
    {
      huart->Instance->BRR = UART_BRR_SAMPLING16(HAL_RCC_GetPCLK2Freq(), huart->Init.BaudRate);
    }
#else
    if(huart->Instance == USART1)
    {
      huart->Instance->BRR = UART_BRR_SAMPLING16(HAL_RCC_GetPCLK2Freq(), huart->Init.BaudRate);
    }
#endif /* USART6 */
    else
    {
      huart->Instance->BRR = UART_BRR_SAMPLING16(HAL_RCC_GetPCLK1Freq(), huart->Init.BaudRate);
    }
  }
}

/**
  * @brief  Initializes the UART mode according to the specified parameters in
  *         the UART_InitTypeDef and create the associated handle.
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart)
{
  /* Check the UART handle allocation */
  if(huart == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  if(huart->Init.HwFlowCtl != UART_HWCONTROL_NONE)
  {
    /* The hardware flow control is available only for USART1, USART2, USART3 and USART6 */
    assert_param(IS_UART_HWFLOW_INSTANCE(huart->Instance));
    assert_param(IS_UART_HARDWARE_FLOW_CONTROL(huart->Init.HwFlowCtl));
  }
  else
  {
    assert_param(IS_UART_INSTANCE(huart->Instance));
  }
  assert_param(IS_UART_WORD_LENGTH(huart->Init.WordLength));
  assert_param(IS_UART_OVERSAMPLING(huart->Init.OverSampling));

  if(huart->gState == HAL_UART_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    huart->Lock = HAL_UNLOCKED;
    /* Init the low level hardware */
    //HAL_UART_MspInit(huart);
  }

  huart->gState = HAL_UART_STATE_BUSY;

  /* Disable the peripheral */
  __HAL_UART_DISABLE(huart);

  /* Set the UART Communication parameters */
  UART_SetConfig(huart);

  /* In asynchronous mode, the following bits must be kept cleared:
     - LINEN and CLKEN bits in the USART_CR2 register,
     - SCEN, HDSEL and IREN  bits in the USART_CR3 register.*/
  CLEAR_BIT(huart->Instance->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
  CLEAR_BIT(huart->Instance->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));

  /* Enable the peripheral */
  __HAL_UART_ENABLE(huart);

  /* Initialize the UART state */
  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->gState= HAL_UART_STATE_READY;
  huart->RxState= HAL_UART_STATE_READY;

  return HAL_OK;
}

static HAL_StatusTypeDef UART_WaitOnFlagForever(UART_HandleTypeDef *huart, uint32_t Flag, FlagStatus Status)
{
  /* Wait until flag is set */
  while((__HAL_UART_GET_FLAG(huart, Flag) ? SET : RESET) == Status);
  return HAL_OK;
}

/**
  * @brief  Sends an amount of data in blocking mode.
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint16_t* tmp;
  //uint32_t tickstart = 0U;

  /* Check that a Tx process is not already ongoing */
  if(huart->gState == HAL_UART_STATE_READY)
  {
    if((pData == NULL ) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(huart);

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->gState = HAL_UART_STATE_BUSY_TX;

    /* Init tickstart for timeout managment */
    //tickstart = HAL_GetTick();

    huart->TxXferSize = Size;
    huart->TxXferCount = Size;
    while(huart->TxXferCount > 0U)
    {
      huart->TxXferCount--;
      if(huart->Init.WordLength == UART_WORDLENGTH_9B)
      {
        if(UART_WaitOnFlagForever(huart, UART_FLAG_TXE, RESET) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        tmp = (uint16_t*) pData;
        huart->Instance->DR = (*tmp & (uint16_t)0x01FF);
        if(huart->Init.Parity == UART_PARITY_NONE)
        {
          pData +=2U;
        }
        else
        {
          pData +=1U;
        }
      }
      else
      {
        if(UART_WaitOnFlagForever(huart, UART_FLAG_TXE, RESET) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        huart->Instance->DR = (*pData++ & (uint8_t)0xFF);
      }
    }

    if(UART_WaitOnFlagForever(huart, UART_FLAG_TC, RESET) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    /* At end of Tx process, restore huart->gState to Ready */
      huart->gState = HAL_UART_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(huart);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



/**
  * @brief  Receives an amount of data in blocking mode.
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be received
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint16_t* tmp;
  //uint32_t tickstart = 0U;

  /* Check that a Rx process is not already ongoing */
  if(huart->RxState == HAL_UART_STATE_READY)
  {
    if((pData == NULL ) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(huart);

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->RxState = HAL_UART_STATE_BUSY_RX;

    /* Init tickstart for timeout managment */
    //tickstart = HAL_GetTick();

    huart->RxXferSize = Size;
    huart->RxXferCount = Size;

    /* Check the remain data to be received */
    while(huart->RxXferCount > 0U)
    {
      huart->RxXferCount--;
      if(huart->Init.WordLength == UART_WORDLENGTH_9B)
      {
        if(UART_WaitOnFlagForever(huart, UART_FLAG_RXNE, RESET) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        tmp = (uint16_t*) pData;
        if(huart->Init.Parity == UART_PARITY_NONE)
        {
          *tmp = (uint16_t)(huart->Instance->DR & (uint16_t)0x01FF);
          pData +=2U;
        }
        else
        {
          *tmp = (uint16_t)(huart->Instance->DR & (uint16_t)0x00FF);
          pData +=1U;
        }

      }
      else
      {
        if(UART_WaitOnFlagForever(huart, UART_FLAG_RXNE, RESET) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        if(huart->Init.Parity == UART_PARITY_NONE)
        {
          *pData++ = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
        }
        else
        {
          *pData++ = (uint8_t)(huart->Instance->DR & (uint8_t)0x007F);
        }

      }
    }

    /* At end of Rx process, restore huart->RxState to Ready */
    huart->RxState = HAL_UART_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(huart);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

///// ----- From stm32f4xx_cryp.c ------------------------------------------------
//#define FLAG_MASK     ((uint8_t)0x20)

///**
  //* @brief  Fills each CRYP_KeyInitStruct member with its default value.
  //* @param  CRYP_KeyInitStruct: pointer to a CRYP_KeyInitTypeDef structure 
  //*         which will be initialized.
  //* @retval None
  //*/
//void CRYP_KeyStructInit(CRYP_KeyInitTypeDef* CRYP_KeyInitStruct)
//{
  //CRYP_KeyInitStruct->CRYP_Key0Left  = 0;
  //CRYP_KeyInitStruct->CRYP_Key0Right = 0;
  //CRYP_KeyInitStruct->CRYP_Key1Left  = 0;
  //CRYP_KeyInitStruct->CRYP_Key1Right = 0;
  //CRYP_KeyInitStruct->CRYP_Key2Left  = 0;
  //CRYP_KeyInitStruct->CRYP_Key2Right = 0;
  //CRYP_KeyInitStruct->CRYP_Key3Left  = 0;
  //CRYP_KeyInitStruct->CRYP_Key3Right = 0;
//}

///**
  //* @brief  Flushes the IN and OUT FIFOs (that is read and write pointers of the 
  //*         FIFOs are reset)
  //* @note   The FIFOs must be flushed only when BUSY flag is reset.  
  //* @param  None
  //* @retval None
  //*/
//void CRYP_FIFOFlush(void)
//{
  ///* Reset the read and write pointers of the FIFOs */
  //CRYP->CR |= CRYP_CR_FFLUSH;
//}

///**
  //* @brief  Initializes the CRYP peripheral according to the specified parameters
  //*         in the CRYP_InitStruct.
  //* @param  CRYP_InitStruct: pointer to a CRYP_InitTypeDef structure that contains
  //*         the configuration information for the CRYP peripheral.
  //* @retval None
  //*/
//void CRYP_Init(CRYP_InitTypeDef* CRYP_InitStruct)
//{
  ///* Check the parameters */
  //assert_param(IS_CRYP_ALGOMODE(CRYP_InitStruct->CRYP_AlgoMode));
  //assert_param(IS_CRYP_DATATYPE(CRYP_InitStruct->CRYP_DataType));
  //assert_param(IS_CRYP_ALGODIR(CRYP_InitStruct->CRYP_AlgoDir));

  ///* Select Algorithm mode*/  
  //CRYP->CR &= ~CRYP_CR_ALGOMODE;
  //CRYP->CR |= CRYP_InitStruct->CRYP_AlgoMode;

  ///* Select dataType */ 
  //CRYP->CR &= ~CRYP_CR_DATATYPE;
  //CRYP->CR |= CRYP_InitStruct->CRYP_DataType;

  ///* select Key size (used only with AES algorithm) */
  //if ((CRYP_InitStruct->CRYP_AlgoMode != CRYP_AlgoMode_TDES_ECB) &&
      //(CRYP_InitStruct->CRYP_AlgoMode != CRYP_AlgoMode_TDES_CBC) &&
      //(CRYP_InitStruct->CRYP_AlgoMode != CRYP_AlgoMode_DES_ECB) &&
      //(CRYP_InitStruct->CRYP_AlgoMode != CRYP_AlgoMode_DES_CBC))
  //{
    //assert_param(IS_CRYP_KEYSIZE(CRYP_InitStruct->CRYP_KeySize));
    //CRYP->CR &= ~CRYP_CR_KEYSIZE;
    //CRYP->CR |= CRYP_InitStruct->CRYP_KeySize; /* Key size and value must be 
                                                  //configured once the key has 
                                                  //been prepared */
  //}

  ///* Select data Direction */ 
  //CRYP->CR &= ~CRYP_CR_ALGODIR;
  //CRYP->CR |= CRYP_InitStruct->CRYP_AlgoDir;
//}

///**
  //* @brief  Initializes the CRYP Keys according to the specified parameters in
  //*         the CRYP_KeyInitStruct.
  //* @param  CRYP_KeyInitStruct: pointer to a CRYP_KeyInitTypeDef structure that
  //*         contains the configuration information for the CRYP Keys.
  //* @retval None
  //*/
//void CRYP_KeyInit(CRYP_KeyInitTypeDef* CRYP_KeyInitStruct)
//{
  ///* Key Initialisation */
  //CRYP->K0LR = CRYP_KeyInitStruct->CRYP_Key0Left;
  //CRYP->K0RR = CRYP_KeyInitStruct->CRYP_Key0Right;
  //CRYP->K1LR = CRYP_KeyInitStruct->CRYP_Key1Left;
  //CRYP->K1RR = CRYP_KeyInitStruct->CRYP_Key1Right;
  //CRYP->K2LR = CRYP_KeyInitStruct->CRYP_Key2Left;
  //CRYP->K2RR = CRYP_KeyInitStruct->CRYP_Key2Right;
  //CRYP->K3LR = CRYP_KeyInitStruct->CRYP_Key3Left;
  //CRYP->K3RR = CRYP_KeyInitStruct->CRYP_Key3Right;
//}

///**
  //* @brief  Enables or disables the CRYP peripheral.
  //* @param  NewState: new state of the CRYP peripheral.
  //*          This parameter can be: ENABLE or DISABLE.
  //* @retval None
  //*/
//void CRYP_Cmd(FunctionalState NewState)
//{
  ///* Check the parameters */
  //assert_param(IS_FUNCTIONAL_STATE(NewState));

  //if (NewState != DISABLE)
  //{
    ///* Enable the Cryptographic processor */
    //CRYP->CR |= CRYP_CR_CRYPEN;
  //}
  //else
  //{
    ///* Disable the Cryptographic processor */
    //CRYP->CR &= ~CRYP_CR_CRYPEN;
  //}
//}

///**
  //* @brief  Returns whether CRYP peripheral is enabled or disabled.
  //* @param  none.
  //* @retval Current state of the CRYP peripheral (ENABLE or DISABLE).
  //*/
//FunctionalState CRYP_GetCmdStatus(void)
//{
  //FunctionalState state = DISABLE;

  //if ((CRYP->CR & CRYP_CR_CRYPEN) != 0)
  //{
    ///* CRYPEN bit is set */
    //state = ENABLE;
  //}
  //else
  //{
    ///* CRYPEN bit is reset */
    //state = DISABLE;
  //}
  //return state;
//}

///**
  //* @brief  Writes data in the Data Input register (DIN).
  //* @note   After the DIN register has been read once or several times, 
  //*         the FIFO must be flushed (using CRYP_FIFOFlush() function).  
  //* @param  Data: data to write in Data Input register
  //* @retval None
  //*/
//void CRYP_DataIn(uint32_t Data)
//{
  //CRYP->DR = Data;
//}

///**
  //* @brief  Returns the last data entered into the output FIFO.
  //* @param  None
  //* @retval Last data entered into the output FIFO.
  //*/
//uint32_t CRYP_DataOut(void)
//{
  //return CRYP->DOUT;
//}

///**
  //* @brief  Checks whether the specified CRYP flag is set or not.
  //* @param  CRYP_FLAG: specifies the CRYP flag to check.
  //*          This parameter can be one of the following values:
  //*            @arg CRYP_FLAG_IFEM: Input FIFO Empty flag.
  //*            @arg CRYP_FLAG_IFNF: Input FIFO Not Full flag.
  //*            @arg CRYP_FLAG_OFNE: Output FIFO Not Empty flag.
  //*            @arg CRYP_FLAG_OFFU: Output FIFO Full flag.
  //*            @arg CRYP_FLAG_BUSY: Busy flag.
  //*            @arg CRYP_FLAG_OUTRIS: Output FIFO raw interrupt flag.
  //*            @arg CRYP_FLAG_INRIS: Input FIFO raw interrupt flag.
  //* @retval The new state of CRYP_FLAG (SET or RESET).
  //*/
//FlagStatus CRYP_GetFlagStatus(uint8_t CRYP_FLAG)
//{
  //FlagStatus bitstatus = RESET;
  //uint32_t tempreg = 0;

  ///* Check the parameters */
  //assert_param(IS_CRYP_GET_FLAG(CRYP_FLAG));

  ///* check if the FLAG is in RISR register */
  //if ((CRYP_FLAG & FLAG_MASK) != 0x00) 
  //{
    //tempreg = CRYP->RISR;
  //}
  //else  /* The FLAG is in SR register */
  //{
    //tempreg = CRYP->SR;
  //}


  ///* Check the status of the specified CRYP flag */
  //if ((tempreg & CRYP_FLAG ) != (uint8_t)RESET)
  //{
    ///* CRYP_FLAG is set */
    //bitstatus = SET;
  //}
  //else
  //{
    ///* CRYP_FLAG is reset */
    //bitstatus = RESET;
  //}

  ///* Return the CRYP_FLAG status */
  //return  bitstatus;
//}

///// ----- From stm32f4xx_cryp_aes.c --------------------------------------------
//#define AESBUSY_TIMEOUT    ((uint32_t) 0x00010000)

///**
  //* @brief  Encrypt and decrypt using AES in ECB Mode
  //* @param  Mode: encryption or decryption Mode.
  //*          This parameter can be one of the following values:
  //*            @arg MODE_ENCRYPT: Encryption
  //*            @arg MODE_DECRYPT: Decryption
  //* @param  Key: Key used for AES algorithm.
  //* @param  Keysize: length of the Key, must be a 128, 192 or 256.
  //* @param  Input: pointer to the Input buffer.
  //* @param  Ilength: length of the Input buffer, must be a multiple of 16.
  //* @param  Output: pointer to the returned buffer.
  //* @retval An ErrorStatus enumeration value:
  //*          - SUCCESS: Operation done
  //*          - ERROR: Operation failed
  //*/
//ErrorStatus CRYP_AES_ECB(uint8_t Mode, uint8_t* Key, uint16_t Keysize,
                         //uint8_t* Input, uint32_t Ilength, uint8_t* Output)
//{
  //CRYP_InitTypeDef AES_CRYP_InitStructure;
  //CRYP_KeyInitTypeDef AES_CRYP_KeyInitStructure;
  //__IO uint32_t counter = 0;
  //uint32_t busystatus = 0;
  //ErrorStatus status = SUCCESS;
  //uint32_t keyaddr    = (uint32_t)Key;
  //uint32_t inputaddr  = (uint32_t)Input;
  //uint32_t outputaddr = (uint32_t)Output;
  //uint32_t i = 0;

  ///* Crypto structures initialisation*/
  //CRYP_KeyStructInit(&AES_CRYP_KeyInitStructure);

  //switch(Keysize)
  //{
    //case 128:
    //AES_CRYP_InitStructure.CRYP_KeySize = CRYP_KeySize_128b;
    //AES_CRYP_KeyInitStructure.CRYP_Key2Left = __REV(*(uint32_t*)(keyaddr));
    //keyaddr+=4;
    //AES_CRYP_KeyInitStructure.CRYP_Key2Right= __REV(*(uint32_t*)(keyaddr));
    //keyaddr+=4;
    //AES_CRYP_KeyInitStructure.CRYP_Key3Left = __REV(*(uint32_t*)(keyaddr));
    //keyaddr+=4;
    //AES_CRYP_KeyInitStructure.CRYP_Key3Right= __REV(*(uint32_t*)(keyaddr));
    //break;
    //case 192:
    //AES_CRYP_InitStructure.CRYP_KeySize  = CRYP_KeySize_192b;
    //AES_CRYP_KeyInitStructure.CRYP_Key1Left = __REV(*(uint32_t*)(keyaddr));
    //keyaddr+=4;
    //AES_CRYP_KeyInitStructure.CRYP_Key1Right= __REV(*(uint32_t*)(keyaddr));
    //keyaddr+=4;
    //AES_CRYP_KeyInitStructure.CRYP_Key2Left = __REV(*(uint32_t*)(keyaddr));
    //keyaddr+=4;
    //AES_CRYP_KeyInitStructure.CRYP_Key2Right= __REV(*(uint32_t*)(keyaddr));
    //keyaddr+=4;
    //AES_CRYP_KeyInitStructure.CRYP_Key3Left = __REV(*(uint32_t*)(keyaddr));
    //keyaddr+=4;
    //AES_CRYP_KeyInitStructure.CRYP_Key3Right= __REV(*(uint32_t*)(keyaddr));
    //break;
    //case 256:
    //AES_CRYP_InitStructure.CRYP_KeySize  = CRYP_KeySize_256b;
    //AES_CRYP_KeyInitStructure.CRYP_Key0Left = __REV(*(uint32_t*)(keyaddr));
    //keyaddr+=4;
    //AES_CRYP_KeyInitStructure.CRYP_Key0Right= __REV(*(uint32_t*)(keyaddr));
    //keyaddr+=4;
    //AES_CRYP_KeyInitStructure.CRYP_Key1Left = __REV(*(uint32_t*)(keyaddr));
    //keyaddr+=4;
    //AES_CRYP_KeyInitStructure.CRYP_Key1Right= __REV(*(uint32_t*)(keyaddr));
    //keyaddr+=4;
    //AES_CRYP_KeyInitStructure.CRYP_Key2Left = __REV(*(uint32_t*)(keyaddr));
    //keyaddr+=4;
    //AES_CRYP_KeyInitStructure.CRYP_Key2Right= __REV(*(uint32_t*)(keyaddr));
    //keyaddr+=4;
    //AES_CRYP_KeyInitStructure.CRYP_Key3Left = __REV(*(uint32_t*)(keyaddr));
    //keyaddr+=4;
    //AES_CRYP_KeyInitStructure.CRYP_Key3Right= __REV(*(uint32_t*)(keyaddr));
    //break;
    //default:
    //break;
  //}
  
  ///*------------------ AES Decryption ------------------*/
  //if(Mode == MODE_DECRYPT) /* AES decryption */
  //{
    ///* Flush IN/OUT FIFOs */
    //CRYP_FIFOFlush();

    ///* Crypto Init for Key preparation for decryption process */
    //AES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Decrypt;
    //AES_CRYP_InitStructure.CRYP_AlgoMode = CRYP_AlgoMode_AES_Key;
    //AES_CRYP_InitStructure.CRYP_DataType = CRYP_DataType_32b;
    //CRYP_Init(&AES_CRYP_InitStructure);

    ///* Key Initialisation */
    //CRYP_KeyInit(&AES_CRYP_KeyInitStructure);

    ///* Enable Crypto processor */
    //CRYP_Cmd(ENABLE);

    ///* wait until the Busy flag is RESET */
    //do
    //{
      //busystatus = CRYP_GetFlagStatus(CRYP_FLAG_BUSY);
      //counter++;
    //}while ((counter != AESBUSY_TIMEOUT) && (busystatus != RESET));

    //if (busystatus != RESET)
   //{
       //status = ERROR;
    //}
    //else
    //{
      ///* Crypto Init for decryption process */  
      //AES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Decrypt;
    //}
  //}
  ///*------------------ AES Encryption ------------------*/
  //else /* AES encryption */
  //{

    //CRYP_KeyInit(&AES_CRYP_KeyInitStructure);

    ///* Crypto Init for Encryption process */
    //AES_CRYP_InitStructure.CRYP_AlgoDir  = CRYP_AlgoDir_Encrypt;
  //}

  //AES_CRYP_InitStructure.CRYP_AlgoMode = CRYP_AlgoMode_AES_ECB;
  //AES_CRYP_InitStructure.CRYP_DataType = CRYP_DataType_8b;
  //CRYP_Init(&AES_CRYP_InitStructure);

  ///* Flush IN/OUT FIFOs */
  //CRYP_FIFOFlush();

  ///* Enable Crypto processor */
  //CRYP_Cmd(ENABLE);

  //if(CRYP_GetCmdStatus() == DISABLE)
  //{
    ///* The CRYP peripheral clock is not enabled or the device doesn't embed 
       //the CRYP peripheral (please check the device sales type. */
    //return(ERROR);
  //}
  
  //for(i=0; ((i<Ilength) && (status != ERROR)); i+=16)
  //{

    ///* Write the Input block in the IN FIFO */
    //CRYP_DataIn(*(uint32_t*)(inputaddr));
    //inputaddr+=4;
    //CRYP_DataIn(*(uint32_t*)(inputaddr));
    //inputaddr+=4;
    //CRYP_DataIn(*(uint32_t*)(inputaddr));
    //inputaddr+=4;
    //CRYP_DataIn(*(uint32_t*)(inputaddr));
    //inputaddr+=4;

    ///* Wait until the complete message has been processed */
    //counter = 0;
    //do
    //{
      //busystatus = CRYP_GetFlagStatus(CRYP_FLAG_BUSY);
      //counter++;
    //}while ((counter != AESBUSY_TIMEOUT) && (busystatus != RESET));

    //if (busystatus != RESET)
   //{
       //status = ERROR;
    //}
    //else
    //{

      ///* Read the Output block from the Output FIFO */
      //*(uint32_t*)(outputaddr) = CRYP_DataOut();
      //outputaddr+=4;
      //*(uint32_t*)(outputaddr) = CRYP_DataOut();
      //outputaddr+=4;
      //*(uint32_t*)(outputaddr) = CRYP_DataOut();
      //outputaddr+=4;
      //*(uint32_t*)(outputaddr) = CRYP_DataOut(); 
      //outputaddr+=4;
    //}
  //}

  ///* Disable Crypto */
  //CRYP_Cmd(DISABLE);

  //return status; 
//}

/**
  * @brief  Writes the Key in Key registers. 
  * @param  hcryp pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  Key Pointer to Key buffer
  * @param  KeySize Size of Key
  * @retval None
  */
static void CRYP_SetKey(CRYP_HandleTypeDef *hcryp, uint8_t *Key, uint32_t KeySize)
{
  uint32_t keyaddr = (uint32_t)Key;
  
  switch(KeySize)
  {
  case CRYP_KEYSIZE_256B:
    /* Key Initialisation */
    hcryp->Instance->K0LR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4U;
    hcryp->Instance->K0RR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4U;
    hcryp->Instance->K1LR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4U;
    hcryp->Instance->K1RR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4U;
    hcryp->Instance->K2LR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4U;
    hcryp->Instance->K2RR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4U;
    hcryp->Instance->K3LR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4U;
    hcryp->Instance->K3RR = __REV(*(uint32_t*)(keyaddr));
    break;
  case CRYP_KEYSIZE_192B:
    hcryp->Instance->K1LR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4U;
    hcryp->Instance->K1RR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4U;
    hcryp->Instance->K2LR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4U;
    hcryp->Instance->K2RR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4U;
    hcryp->Instance->K3LR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4U;
    hcryp->Instance->K3RR = __REV(*(uint32_t*)(keyaddr));
    break;
  case CRYP_KEYSIZE_128B:       
    hcryp->Instance->K2LR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4U;
    hcryp->Instance->K2RR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4U;
    hcryp->Instance->K3LR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4U;
    hcryp->Instance->K3RR = __REV(*(uint32_t*)(keyaddr));
    break;
  default:
    break;
  }
}

static HAL_StatusTypeDef CRYP_ProcessData(CRYP_HandleTypeDef *hcryp, uint8_t* Input, uint16_t Ilength, uint8_t* Output, uint32_t Timeout);

/**
  * @brief  Process Data: Writes Input data in polling mode and read the output data
  * @param  hcryp pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  Input Pointer to the Input buffer
  * @param  Ilength Length of the Input buffer, must be a multiple of 16.
  * @param  Output Pointer to the returned buffer
  * @param  Timeout Timeout value
  * @retval None
  */
static HAL_StatusTypeDef CRYP_ProcessData(CRYP_HandleTypeDef *hcryp, uint8_t* Input, uint16_t Ilength, uint8_t* Output, uint32_t Timeout)
{
  uint32_t tickstart = 0U;
  
  uint32_t i = 0U;
  uint32_t inputaddr  = (uint32_t)Input;
  uint32_t outputaddr = (uint32_t)Output;
  
  for(i=0U; (i < Ilength); i+=16U)
  {
    /* Write the Input block in the IN FIFO */
    hcryp->Instance->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4U;
    hcryp->Instance->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4U;
    hcryp->Instance->DR  = *(uint32_t*)(inputaddr);
    inputaddr+=4U;
    hcryp->Instance->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4U;
    
    /* Get tick */
    tickstart = HAL_GetTick();

    while(HAL_IS_BIT_CLR(hcryp->Instance->SR, CRYP_FLAG_OFNE))
    {    
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0U)||((HAL_GetTick() - tickstart ) > Timeout))
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */
          __HAL_UNLOCK(hcryp);
        
          return HAL_TIMEOUT;
        }
      }
    }
    /* Read the Output block from the Output FIFO */
    *(uint32_t*)(outputaddr) = hcryp->Instance->DOUT;
    outputaddr+=4U;
    *(uint32_t*)(outputaddr) = hcryp->Instance->DOUT;
    outputaddr+=4U;
    *(uint32_t*)(outputaddr) = hcryp->Instance->DOUT;
    outputaddr+=4U;
    *(uint32_t*)(outputaddr) = hcryp->Instance->DOUT;
    outputaddr+=4U;
  }
  /* Return function status */
  return HAL_OK;
}

HAL_StatusTypeDef HAL_CRYP_Init(CRYP_HandleTypeDef *hcryp)
{ 
  /* Check the CRYP handle allocation */
  if(hcryp == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_CRYP_KEYSIZE(hcryp->Init.KeySize));
  assert_param(IS_CRYP_DATATYPE(hcryp->Init.DataType));
    
  if(hcryp->State == HAL_CRYP_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hcryp->Lock = HAL_UNLOCKED;
    /* Init the low level hardware */
    //HAL_CRYP_MspInit(hcryp);
  }
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Set the key size and data type*/
  CRYP->CR = (uint32_t) (hcryp->Init.KeySize | hcryp->Init.DataType);
  
  /* Reset CrypInCount and CrypOutCount */
  hcryp->CrypInCount = 0U;
  hcryp->CrypOutCount = 0U;
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Set the default CRYP phase */
  hcryp->Phase = HAL_CRYP_PHASE_READY;
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  DeInitializes the CRYP peripheral. 
  * @param  hcryp pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_DeInit(CRYP_HandleTypeDef *hcryp)
{
  /* Check the CRYP handle allocation */
  if(hcryp == NULL)
  {
    return HAL_ERROR;
  }
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Set the default CRYP phase */
  hcryp->Phase = HAL_CRYP_PHASE_READY;
  
  /* Reset CrypInCount and CrypOutCount */
  hcryp->CrypInCount = 0U;
  hcryp->CrypOutCount = 0U;
  
  /* Disable the CRYP Peripheral Clock */
  __HAL_CRYP_DISABLE(hcryp);
  
  /* DeInit the low level hardware: CLOCK, NVIC.*/
  //HAL_CRYP_MspDeInit(hcryp);
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hcryp);

  /* Return function status */
  return HAL_OK;
}

HAL_StatusTypeDef HAL_CRYP_AESECB_Encrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData, uint32_t Timeout)
{
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Check if initialization phase has already been performed */
  if(hcryp->Phase == HAL_CRYP_PHASE_READY)
  {
    /* Set the key */
    CRYP_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
    
    /* Set the CRYP peripheral in AES ECB mode */
    __HAL_CRYP_SET_MODE(hcryp, CRYP_CR_ALGOMODE_AES_ECB);
    
    /* Flush FIFO */
    __HAL_CRYP_FIFO_FLUSH(hcryp);
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE(hcryp);
    
    /* Set the phase */
    hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
  }
  
    /* Write Plain Data and Get Cypher Data */
    if(CRYP_ProcessData(hcryp, pPlainData, Size, pCypherData, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in AES ECB decryption mode
  *         then decrypted pCypherData. The cypher data are available in pPlainData
  * @param  hcryp pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pCypherData Pointer to the cyphertext buffer
  * @param  Size Length of the plaintext buffer, must be a multiple of 16.
  * @param  pPlainData Pointer to the plaintext buffer
  * @param  Timeout Specify Timeout value  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_AESECB_Decrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData, uint32_t Timeout)
{
   uint32_t tickstart = 0U;
  
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Check if initialization phase has already been performed */
  if(hcryp->Phase == HAL_CRYP_PHASE_READY)
  {
    /* Set the key */
    CRYP_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
    
    /* Set the CRYP peripheral in AES Key mode */
    __HAL_CRYP_SET_MODE(hcryp, CRYP_CR_ALGOMODE_AES_KEY | CRYP_CR_ALGODIR);
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE(hcryp);
    
    /* Get tick */ 
    tickstart = HAL_GetTick();

    while(HAL_IS_BIT_SET(hcryp->Instance->SR, CRYP_FLAG_BUSY))
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0U)||((HAL_GetTick() - tickstart ) > Timeout))
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */          
          __HAL_UNLOCK(hcryp);
        
          return HAL_TIMEOUT;
        }
      }
    }
    
    /* Disable CRYP */
    __HAL_CRYP_DISABLE(hcryp);
    
    /* Reset the ALGOMODE bits*/
    CRYP->CR &= (uint32_t)(~CRYP_CR_ALGOMODE);
    
    /* Set the CRYP peripheral in AES ECB decryption mode */
    __HAL_CRYP_SET_MODE(hcryp, CRYP_CR_ALGOMODE_AES_ECB | CRYP_CR_ALGODIR);
    /* Flush FIFO */
    __HAL_CRYP_FIFO_FLUSH(hcryp);
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE(hcryp);
    
    /* Set the phase */
    hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
  }
    
    /* Write Plain Data and Get Cypher Data */
    if(CRYP_ProcessData(hcryp, pCypherData, Size, pPlainData, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}
