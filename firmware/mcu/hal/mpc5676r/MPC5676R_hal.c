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

#include "nxp/config/Cpu.h"
#include "nxp/config/canCom1.h"
#include "nxp/config/pin_mux.h"
#include "nxp/config/uart_pal1.h"
#include "nxp/drivers/inc/uart_esci_callback.h"
#include "MPC5676R_hal.h"
#define TIMEOUT (200U)
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#if 0 
#include "board.h"
#include "fsl_dcp.h"
#include "fsl_lpuart.h"
#include "fsl_snvs_lp.h"
#include "pin_mux.h"
#include "system_MPC5676R.h"
#include "clock_config.h"
#endif 

#define LED_PORT        PTD
#define LED0            0U
#define LED1            1U
#define BTN_PORT        PTE
#define BTN0_PIN        6U
#define BTN1_PIN        7U
#define BTN0_EIRQ       8U
#define BTN1_EIRQ       9U

#define FLASH_REG   FLASH_A.BIUCR.R
#define FLASH_DATA  0x00016B35
#define FLASH_REG2  FLASH_A.BIUCR3.R
#define FLASH_DATA2 0x00020015

/* This function is defined in some other functions too */
__attribute__ ((weak)) void uart_puts(char * s){
    while(*s){
        putch(*(s++));
    }
}

void init_uart(void)
{
    UART_Init(&uart_pal1_instance, &uart_pal1_Config0);
}

void putch(char c)
{
    /* Ensure all the data in the transmit buffer are sent out to bus. */
    char local = c;
    uint8_t* mssg = &local;
    // UART_SendData(&uart_pal1_instance, (uint8_t *)mssg, 1);
    ESCI_HW_SendCharacter(0, c);
    // while (!ESCI_HW_GetSendReadyFlag(0));
    // ESCI_HW_ClearSendReadyFlag(0);
}

char getch(void)
{
    char mssg;
    while (!ESCI_HW_GetReceiveReadyFlag(0));
    ESCI_HW_ClearReceiveReadyFlag(0);
    UART_ReceiveDataBlocking(&uart_pal1_instance, &mssg, 1,TIMEOUT);
    return mssg;
}


void trigger_setup(void)
{
    //Setup is done in pin_mux file
    ;
}
void trigger_high(void)
{
    // PINS_DRV_WritePin(SIU, 160, 1);
    pin_settings_config_t trig_config = {
        .base              = SIU,
        .pinPortIdx        = 160u,
        .mux               = PORT_MUX_AS_GPIO,
        .outputBuffer      = PORT_OUTPUT_BUFFER_ENABLED,
        .slewRateCtrl      = HALF_STRENGTH_WITH_SLEWRATE_CONTROL,
        .openDrain         = PORT_OPEN_DRAIN_DISABLED,
        .hysteresis        = PORT_HYSTERESYS_DISABLED,
        .driveSelect       = PORT_MINIMUM_DRIVE_STRENGTH,
        .inputBuffer       = PORT_INPUT_BUFFER_DISABLED,
        .pullConfig        = PORT_INTERNAL_PULL_NOT_ENABLED,
        .initValue         = 1u,
    };
    PINS_DRV_Init(1, &trig_config);
   ;
}
void trigger_low(void)
{
    pin_settings_config_t trig_config = {
        .base              = SIU,
        .pinPortIdx        = 160u,
        .mux               = PORT_MUX_AS_GPIO,
        .outputBuffer      = PORT_OUTPUT_BUFFER_ENABLED,
        .slewRateCtrl      = HALF_STRENGTH_WITH_SLEWRATE_CONTROL,
        .openDrain         = PORT_OPEN_DRAIN_DISABLED,
        .hysteresis        = PORT_HYSTERESYS_DISABLED,
        .driveSelect       = PORT_MINIMUM_DRIVE_STRENGTH,
        .inputBuffer       = PORT_INPUT_BUFFER_DISABLED,
        .pullConfig        = PORT_INTERNAL_PULL_NOT_ENABLED,
        .initValue         = 0u,
    };
    // PINS_DRV_WritePin(SIU, 160, 0);
    PINS_DRV_Init(1, &trig_config);
   ;
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
Function Name : Flash_Init
Engineer      : b55689
Date          : Jan-12-2018
Parameters    :
Modifies      :
Returns       :
Notes         : necessary for correct functionality of core 1
Issues        :
*******************************************************************************/
static void FlashInit(void)
{
	unsigned int mem_write_code [] =
	{

			/*for processors which support VLE only or for 'VLE on' option*/
			0xD0344400, /* stw r3,(0)r4 machine code: writes r3 contents to addr in r4 then se_nop*/
			0x7C0006AC, /* mbar machine code: ensure prior store completed */
			0x44000004  /* blr machine code: branches to return address in link register */
    };


typedef void (*mem_write_code_ptr_t)(unsigned int, unsigned int);

  #if defined(FLASH_REG)
  #if FLASH_DATA > 0
  (*((mem_write_code_ptr_t)mem_write_code)) 	/* cast mem_write_code as func ptr*/
                                 /* * de-references func ptr, i.e. converts to func*/
     (FLASH_DATA,            /* which passes integer (in r3) */
      (unsigned int)&FLASH_REG);
  #endif
  #endif

  #if defined(FLASH_REG2)
  	(*((mem_write_code_ptr_t)mem_write_code)) 	/* cast mem_write_code as func ptr*/
                                 /* * de-references func ptr, i.e. converts to func*/
     (FLASH_DATA2,            /* which passes integer (in r3) */
      (unsigned int)&FLASH_REG2);
  #endif

}


/*******************************************************************************
Function Name : XBAR_Init
Engineer      : b55689
Date          : Jan-12-2018
Parameters    :
Modifies      :
Returns       :
Notes         :
Issues        :
*******************************************************************************/
static void XbarInit(void)
{
	/* set round robin for all slaves */
	XBAR.SGPCR0.B.ARB = 1;
	XBAR.SGPCR1.B.ARB = 1;
	XBAR.SGPCR2.B.ARB = 1;
	XBAR.SGPCR3.B.ARB = 1;
	XBAR.SGPCR6.B.ARB = 1;
	XBAR.SGPCR7.B.ARB = 1;
}


/*******************************************************************************
Function Name : PIT3_Init
Engineer      : b05111
Date          : Mar-04-2012
Parameters    :
Modifies      :
Returns       :
Notes         : expecting fsys = 180MHz, Init PIT for 1 second period
Issues        :
*******************************************************************************/
static void PIT3Init(void)
{
    /* 30: MDIS = 0 to enable clock for PITs. */
    /* 31: FRZ = 1 for Timers stopped in debug mode */
    PIT->MCR = 0x00000001;
    PIT->TIMER[3].LDVAL = 18000000 - 1;

    /* clear the TIF flag */
    PIT->TIMER[3].TFLG = 0x00000001;

    /* 30: TIE = 1 for interrupt request enabled */
    // 31: TEN = 1 for timer active */
    PIT->TIMER[3].TCTRL = 0x00000003;
}


/*
 * @brief : Initialize clocks, pins and power modes
 */
void BoardInit(void)
{
    PINS_DRV_Init(NUM_OF_CONFIGURED_PINS, g_pin_mux_InitConfigArr);

    // FMPLL.ESYNCR1.B.CLKCFG = 7;         /* Normal mode with crystal osc */
    // FMPLL.ESYNCR1.B.CLKCFG = 0;         /* Normal mode with crystal osc */
    
    /* Programming PLL to 60 MHz */
    /* Fpll = (10Mhz* (EMFD+16)) / ((ERFD+1) * (EPREDIV+1) * DIV2) */
    /* Fpll = (10 * 54 / 9)  = 60MHz */
    // FMPLL.ESYNCR2.R = 0x00000002;           /* Output divide ratio: 2+1=3 */
    // FMPLL.ESYNCR1.B.EPREDIV = 5;        /* Input divide ratio: 5+1=6 */
    // FMPLL.ESYNCR1.B.EMFD = 38;          /* Feedback divide ratio: 38+16=54 */ 
    
    // while(!FMPLL.SYNSR.B.LOCK) {;}      /* Wait for FMPLL to lock */

    // VERY IMPORTANT:
    // The endianness of the datasheet is opposite
    // To the C representation

    // So bit 0 on the datasheet is bit 31 here
    // Probably powerpc bs
    if (SIU->SYSDIV & (1 << 31)) {
        SIU->SYSDIV &= ~(1 << 31);
    }
    uint32_t sysdiv = SIU->SYSDIV;

    
    //sysclock = xosc
    sysdiv &= ~(0b11 << 12);
    sysdiv |= 0b01 << 12; //10 for ext oscillator

    // turn off bypass
    // sysdiv &= ~(0b1 << 4);

    sysdiv &= ~(0b11 << 2); // sysclock/2 for m_clk

    SIU->SYSDIV = sysdiv;
    pin_settings_config_t clkokconfig = {
        .base              = SIU,
        .pinPortIdx        = 199,
        .mux               = PORT_MUX_AS_GPIO,
        .outputBuffer      = PORT_OUTPUT_BUFFER_ENABLED,
        .slewRateCtrl      = HALF_STRENGTH_WITH_SLEWRATE_CONTROL,
        .openDrain         = PORT_OPEN_DRAIN_DISABLED,
        .hysteresis        = PORT_HYSTERESYS_DISABLED,
        .driveSelect       = PORT_MINIMUM_DRIVE_STRENGTH,
        .inputBuffer       = PORT_INPUT_BUFFER_DISABLED,
        .pullConfig        = PORT_INTERNAL_PULL_NOT_ENABLED,
        .initValue         = 1u,
    };
    // PINS_DRV_WritePin(SIU, 160, 0);
    PINS_DRV_Init(1, &clkokconfig);

    // SIU_SYSDIV_BYPASS_MASK
    // SIU->SYSDIV = (SIU->SYSDIV & ~SIU_SYSDIV_BYPASS_MASK) | SIU_SYSDIV_BYPASS(1U);
    //SIU->SYSDIV = (SIU->SYSDIV & ~SIU_SYSDIV_SYSCLKSEL_MASK) | SIU_SYSDIV_SYSCLKSEL(2U);
    
}


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


void platform_init(void)
{
    //FlashInit();
    //XbarInit();
    BoardInit();
        /*
     * Initialize FlexCAN driver
     *  - 8 byte payload size
     *  - FD disabled
     *  - Oscillator clock as peripheral engine clock
     */
    FLEXCAN_DRV_Init(INST_CANCOM1, &canCom1_State, &canCom1_InitConfig0);
}


// const uint32_t  __attribute__((section (".rchw")))user_rchw[] = {
//     0x005A0000,
//     0x1000
// };

#define MPC56xx_ID    0x005A0000  /* RCHW boot ID for MPC56xx devices     */
#define VLE_ENABLE    0x01000000  /* VLE is enabled                       */

extern void _start(void);
#define ENTRY_POINT  _start
#define RCHW_VAL (VLE_ENABLE | MPC56xx_ID)

const uint32_t __attribute__ ((section(".rchw"))) RCHW[] = {RCHW_VAL, (uint32_t)ENTRY_POINT};