#include <stdint.h>
#include "fsl_device_registers.h"
#include "fsl_common.h"
#include "fsl_clock.h"
#include "fsl_debug_console.h"
#include "fsl_hashcrypt.h"
#include "fsl_power.h"
#include "fsl_iocon.h"


#include "lpc55s6x_hal.h"

/* System clock frequency. */
extern uint32_t SystemCoreClock;

void platform_init(void)
{
    // switch off systick
     SysTick->CTRL = 0;

    // lets monitor busclock
    SYSCON->CLKOUTSEL = 0; // main clock
    SYSCON->CLKOUTDIV = 0; // divide by 10, reset = 0, halt = 0,

    /* Init board hardware. */
    /* attach FRO12MHz clock to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);
    BOARD_InitPins();
    //BOARD_BootClockFROHF96M();
    BOARD_InitBootClocks();
    //BOARD_BootClockPLL100M();

    RNG_Init(RNG);
}

void init_uart(void)
{
    BOARD_InitDebugConsole();
}

void putch(char c)
{
    DbgConsole_Putchar(c);
}

char getch()
{
    return DbgConsole_Getchar();
}

/* Initialize debug console. */
void BOARD_InitDebugConsole(void)
{
    RESET_ClearPeripheralReset(BOARD_DEBUG_UART_RST);

    uint32_t uartClkSrcFreq = BOARD_DEBUG_UART_CLK_FREQ;

    DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_DEBUG_UART_BAUDRATE, BOARD_DEBUG_UART_TYPE, uartClkSrcFreq);
}

void BOARD_InitDebugConsole_Core1(void)
{
    RESET_ClearPeripheralReset(BOARD_DEBUG_UART_RST_CORE1);

    uint32_t uartClkSrcFreq = BOARD_DEBUG_UART_CLK_FREQ_CORE1;

    DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE_CORE1, BOARD_DEBUG_UART_BAUDRATE_CORE1, BOARD_DEBUG_UART_TYPE_CORE1,
                    uartClkSrcFreq);
}

void BOARD_InitBootClocks(void)
{
#ifndef SDK_SECONDARY_CORE
    /*!< Set up the clock sources */
    /*!< Configure FRO192M */
    POWER_DisablePD(kPDRUNCFG_PD_FRO192M);               /*!< Ensure FRO is on  */
    CLOCK_SetupFROClocking(12000000U);                   /*!< Set up FRO to the 12 MHz, just for sure */
    CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);                /*!< Switch to FRO 12MHz first to ensure we can change the clock setting */

    CLOCK_SetupFROClocking(96000000U);                   /* Enable FRO HF(96MHz) output */

    POWER_DisablePD(kPDRUNCFG_PD_XTAL32M);                        /* Ensure XTAL32M is powered */
    POWER_DisablePD(kPDRUNCFG_PD_LDOXO32M);                       /* Ensure XTAL32M is powered */
    CLOCK_SetupExtClocking(16000000U);                            /* Enable clk_in clock */
    SYSCON->CLOCK_CTRL |= SYSCON_CLOCK_CTRL_CLKIN_ENA_MASK;       /* Enable clk_in from XTAL32M clock  */
    ANACTRL->XO32M_CTRL |= ANACTRL_XO32M_CTRL_ENABLE_SYSTEM_CLK_OUT_MASK;    /* Enable clk_in to system  */

    while((ANACTRL->XO32M_STATUS & ANACTRL_XO32M_STATUS_XO_READY_MASK) == 0);

    CLOCK_SetFLASHAccessCyclesForFreq(16000000U);          /*!< Set FLASH wait states for core */

    /*!< Set up dividers */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U, false);         /*!< Set AHBCLKDIV divider to value 1 */

    /*!< Set up clock selectors - Attach clocks to the peripheries */
    CLOCK_AttachClk(kEXT_CLK_to_MAIN_CLK);                 /*!< Switch MAIN_CLK to PLL0 */

    /*< Set SystemCoreClock variable. */
    SystemCoreClock = 16000000U;
#endif
}


void BOARD_BootClockFRO12M(void)
{
#ifndef SDK_SECONDARY_CORE
    /*!< Set up the clock sources */
    /*!< Configure FRO192M */
    POWER_DisablePD(kPDRUNCFG_PD_FRO192M);               /*!< Ensure FRO is on  */
    CLOCK_SetupFROClocking(12000000U);                   /*!< Set up FRO to the 12 MHz, just for sure */
    CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);                /*!< Switch to FRO 12MHz first to ensure we can change the clock setting */

    CLOCK_SetupFROClocking(96000000U);                   /* Enable FRO HF(96MHz) output */

    CLOCK_SetFLASHAccessCyclesForFreq(12000000U);          /*!< Set FLASH wait states for core */

    /*!< Set up dividers */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U, false);         /*!< Set AHBCLKDIV divider to value 1 */

    /*!< Set up clock selectors - Attach clocks to the peripheries */
    CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);                 /*!< Switch MAIN_CLK to FRO12M */

    /*< Set SystemCoreClock variable. */
    SystemCoreClock = BOARD_BOOTCLOCKFRO12M_CORE_CLOCK;
#endif
}

void BOARD_InitPins(void)
{
    /* Enables the clock for the I/O controller.: Enable Clock. */
    CLOCK_EnableClock(kCLOCK_Iocon);

    const uint32_t port0_pin29_config = (/* Pin is configured as FC0_RXD_SDA_MOSI_DATA */
                                         IOCON_PIO_FUNC1 |
                                         /* No addition pin function */
                                         IOCON_PIO_MODE_INACT |
                                         /* Standard mode, output slew rate control is enabled */
                                         IOCON_PIO_SLEW_STANDARD |
                                         /* Input function is not inverted */
                                         IOCON_PIO_INV_DI |
                                         /* Enables digital function */
                                         IOCON_PIO_DIGITAL_EN |
                                         /* Open drain is disabled */
                                         IOCON_PIO_OPENDRAIN_DI);
    /* PORT0 PIN29 (coords: 92) is configured as FC0_RXD_SDA_MOSI_DATA */
    IOCON_PinMuxSet(IOCON, 0U, 29U, port0_pin29_config);

    const uint32_t port0_pin30_config = (/* Pin is configured as FC0_TXD_SCL_MISO_WS */
                                         IOCON_PIO_FUNC1 |
                                         /* No addition pin function */
                                         IOCON_PIO_MODE_INACT |
                                         /* Standard mode, output slew rate control is enabled */
                                         IOCON_PIO_SLEW_STANDARD |
                                         /* Input function is not inverted */
                                         IOCON_PIO_INV_DI |
                                         /* Enables digital function */
                                         IOCON_PIO_DIGITAL_EN |
                                         /* Open drain is disabled */
                                         IOCON_PIO_OPENDRAIN_DI);
    /* PORT0 PIN30 (coords: 94) is configured as FC0_TXD_SCL_MISO_WS */
    IOCON_PinMuxSet(IOCON, 0U, 30U, port0_pin30_config);


    /* Trigger/GPIO4 PIN on NAE-CW308T-LPC55S6X-02 is wired into PIO0_24 */
    const uint32_t port0_pin24_config = (
										 /* Standard mode, output slew rate control is enabled */
                                         IOCON_PIO_SLEW_STANDARD |
                                         /* Input function is not inverted */
                                         IOCON_PIO_INV_DI |
                                         /* Enables digital function */
                                         IOCON_PIO_DIGITAL_EN |
                                         /* Open drain is disabled */
                                         IOCON_PIO_OPENDRAIN_DI);
    IOCON_PinMuxSet(IOCON, 0U, 24U, port0_pin24_config);


    const uint32_t port1_pin27_config = (
										 /* Standard mode, output slew rate control is enabled */
                                         IOCON_PIO_SLEW_STANDARD |
										 0x04 |
                                         /* Input function is not inverted */
                                         IOCON_PIO_INV_DI |
                                         /* Enables digital function */
                                         IOCON_PIO_DIGITAL_EN |
                                         /* Open drain is disabled */
                                         IOCON_PIO_OPENDRAIN_DI);
    IOCON_PinMuxSet(IOCON, 1U, 27U, port1_pin27_config);
}

void trigger_setup(void)
{
    gpio_pin_config_t pinconfig = { kGPIO_DigitalOutput,  0, };
    GPIO_PinInit(GPIO, 0, 24, &pinconfig);
}

void trigger_low(void)
{
    GPIO_PinWrite(GPIO, 0, 24, 0);
}

void trigger_high(void)
{
    GPIO_PinWrite(GPIO, 0, 24, 1);
}

uint32_t get_rand(void)
{
    uint32_t value;
    RNG_GetRandomData(RNG, &value, sizeof(value));
    return value;
}

void HW_AES128_Init(void)
{
}

static 	hashcrypt_handle_t hch;

void HW_AES128_LoadKey(uint8_t* key)
{
	hch.keySize = kHASHCRYPT_Aes128;
	hch.keyType = kHASHCRYPT_UserKey;
    HASHCRYPT_AES_SetKey(HASHCRYPT, &hch, key, 16);
}

void HW_AES128_Enc_pretrigger(uint8_t* pt)
{
}

void HW_AES128_Enc(uint8_t* pt)
{
    HASHCRYPT_AES_EncryptEcb(HASHCRYPT, &hch, pt, pt, 16);
}

void HW_AES128_Enc_posttrigger(uint8_t* pt)
{
}

void HW_AES128_Dec(uint8_t *pt)
{
    HASHCRYPT_AES_DecryptEcb(HASHCRYPT, &hch, pt, pt, 16);
}
