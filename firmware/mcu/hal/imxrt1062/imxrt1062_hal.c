/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2019-2020 NewAE Technology Inc.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "imxrt1062_hal.h"
#include "board.h"
#include "fsl_dcp.h"
#include "fsl_lpuart.h"
#include "fsl_snvs_lp.h"
#include "pin_mux.h"
#include "system_MIMXRT1062.h"
#include "clock_config.h"

/* This function is defined in some other functions too */
__attribute__ ((weak)) void uart_puts(char * s){
    while(*s){
        putch(*(s++));
    }
}

void init_uart(void)
{
    lpuart_config_t lpuartConfig;
    LPUART_GetDefaultConfig(&lpuartConfig);
    lpuartConfig.baudRate_Bps = 38400U;
    lpuartConfig.enableTx = 1;
    lpuartConfig.enableRx = 1;
    LPUART_Init(LPUART1, &lpuartConfig, 80000000U);
}

void putch(char c)
{
    while (0U == (LPUART1->STAT & LPUART_STAT_TDRE_MASK)){;}
    
    LPUART1->DATA = c;
    
    /* Ensure all the data in the transmit buffer are sent out to bus. */
    /*
    while (0U == (LPUART1->STAT & LPUART_STAT_TC_MASK))
    {
    }
    */
}

char getch(void)
{
    while (0U == (LPUART1->STAT & LPUART_STAT_RDRF_MASK)){;}
    
    return LPUART1->DATA;
}


void trigger_setup(void)
{
    //Setup is done in pin_mux file
    ;
}
void trigger_high(void)
{
    GPIO_PinWrite(GPIO1, (14U), 1U);
}
void trigger_low(void)
{
    GPIO_PinWrite(GPIO1, (14U), 0U);
}

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_LED_GPIO BOARD_USER_LED_GPIO
#define EXAMPLE_LED_GPIO_PIN BOARD_USER_LED_PIN


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile uint32_t g_systickCounter;
/* The PIN status */
volatile bool g_pinSet = false;

/*******************************************************************************
 * Code
 ******************************************************************************/
void SysTick_Handler(void)
{
    if (g_systickCounter != 0U)
    {
        g_systickCounter--;
    }
}

void SysTick_DelayTicks(uint32_t n)
{
    g_systickCounter = n;
    while (g_systickCounter != 0U)
    {
    }
}

static dcp_handle_t _dcp_handle;

void HW_AES128_Init(void)
{
    dcp_config_t config;
    DCP_GetDefaultConfig(&config);
    DCP_Init(DCP, &config);
    
    _dcp_handle.channel = kDCP_Channel0;
    _dcp_handle.keySlot = 0;
} 

void HW_AES128_LoadKey(uint8_t * key)
{
    DCP_AES_SetKey(DCP, &_dcp_handle, key, 16);
}

void HW_AES128_Enc_pretrigger(uint8_t * pt)
{
    ;
}

void HW_AES128_Enc(uint8_t * pt)
{
    DCP_AES_EncryptEcb(DCP, &_dcp_handle, pt, pt, 16);
}

void HW_AES128_Enc_posttrigger(uint8_t * pt)
{
    ;
}

void platform_init(void)
{
    /* Board pin init */
    BOARD_InitPins();
    BOARD_InitBootClocks();
    
    /* Update the core clock */
    SystemCoreClockUpdate();
    
    init_uart();
    
    /* Check if from previous boot we had a power glitch */
    if(hal_glitch_detected()) {
        uart_puts("BOOT-GLITCH\n");
    }
    
    
    /* Power glitch detector enabled */
    SNVS_LP_Init(SNVS);

    /*
    if (SysTick_Config(SystemCoreClock / 1000U))
    {
        while (1)
        {
        }
    }

    while (1)
    {
        SysTick_DelayTicks(1000U);
        if (g_pinSet)
        {
            GPIO_PinWrite(EXAMPLE_LED_GPIO, EXAMPLE_LED_GPIO_PIN, 0U);
            g_pinSet = false;
        }
        else
        {
            GPIO_PinWrite(EXAMPLE_LED_GPIO, EXAMPLE_LED_GPIO_PIN, 1U);
            g_pinSet = true;
        }
    }
    */
}

/* Power glitch detection - appears to only be detected if caused reset */
int hal_glitch_detected(void)
{
    return SNVS->LPSR & SNVS_LPSR_PGD_MASK;
}

/* Reset power glitch flag */
void hal_glitch_detect_reset(void)
{
    SNVS->LPSR = SNVS_LPSR_PGD_MASK;
}