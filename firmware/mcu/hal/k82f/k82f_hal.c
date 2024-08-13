
#include "k82f_hal.h"
#include <stdio.h>
#include "fsl_flexio.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_clock.h"
#include "clock_config.h"
#include "fsl_lpuart.h"
#include "fsl_mmcau.h"
#include "fsl_ltc.h"
#include "fsl_trng.h"

#include <stdint.h>

#define ROUNDS_BEFORE_RESEED 50000
#define AES_KEY_SIZE 16

static uint32_t AES_KEY_SCH[44];
static char ltcAesKey[AES_KEY_SIZE];
static uint32_t maskSeed;
static uint32_t nbAesBlocks;

void platform_init(void)
{
     trng_config_t trngConfig;

#if ETM_ENABLE
     etmtrace_enable();
#endif
     BOARD_BootClockRUN();

     // Start TRNG
     TRNG_GetDefaultConfig(&trngConfig);
     TRNG_Init(TRNG0, &trngConfig);

#if USE_TRUSTED_CRYPTO
     // Start Trusted Crypto module
     LTC_Init(LTC0);
     TRNG_GetRandomData(TRNG0, &maskSeed, sizeof(maskSeed));
     LTC_SetDpaMaskSeed(LTC0, maskSeed);
     nbAesBlocks = 0;
#endif
}

void init_uart(void)
{
     lpuart_config_t config;
     CLOCK_EnableClock(kCLOCK_PortB); //enable port e clock
     /* CLOCK_EnableClock(kCLOCK_Uart1); //enable UART clock */

     LPUART_GetDefaultConfig(&config);
     config.baudRate_Bps = 38400U;
     config.enableTx = true;
     config.enableRx = true;

     PORT_SetPinMux(PORTB, 16, kPORT_MuxAlt3);
     PORT_SetPinMux(PORTB, 17, kPORT_MuxAlt3);


     /* volatile uint32_t uartclock = CLOCK_GetFreq(LPUART0_CLK_SRC); */
     LPUART_Init(LPUART0, &config, 7370000U);
}

void trigger_setup(void)
{
     PORT_SetPinMux(PORTB, 19, kPORT_MuxAsGpio);
     gpio_pin_config_t trigconf = {
          kGPIO_DigitalOutput, 0
     };

     GPIO_PinInit(GPIOB, 19, &trigconf);
     GPIO_WritePinOutput(GPIOB, 19, 0);
}

void trigger_high(void)
{
     GPIO_WritePinOutput(GPIOB, 19, 1);
}

void trigger_low(void)
{
     GPIO_WritePinOutput(GPIOB, 19, 0);
}

char getch(void)
{
     char c;
     LPUART_ReadBlocking(LPUART0, &c, 1);
     return c;
}

void putch(char c)
{
     LPUART_WriteBlocking(LPUART0, &c, 1);
}

uint32_t get_rand(void)
{
     uint32_t value;
     TRNG_GetRandomData(TRNG0, &value, sizeof(value));
     return value;
}

//nothing needed?
void HW_AES128_Init(void)
{
}
void HW_AES128_LoadKey(uint8_t* key)
{
#if USE_TRUSTED_CRYPTO
     memcpy(ltcAesKey, key, AES_KEY_SIZE);
#else
     MMCAU_AES_SetKey(key, AES_KEY_SIZE, AES_KEY_SCH);
#endif
}

void HW_AES128_Enc_pretrigger(uint8_t* pt)
{
    ;
}

void HW_AES128_Enc_posttrigger(uint8_t* pt)
{
#if USE_TRUSTED_CRYPTO
     nbAesBlocks++;
     if (nbAesBlocks >= ROUNDS_BEFORE_RESEED) {
       nbAesBlocks = 0;
       TRNG_GetRandomData(TRNG0, &maskSeed, sizeof(maskSeed));
       LTC_SetDpaMaskSeed(LTC0, maskSeed);
     }
#endif
}

void HW_AES128_Enc(uint8_t* pt)
{
#if USE_TRUSTED_CRYPTO
     LTC_AES_EncryptEcb(LTC0, pt, pt, LTC_AES_BLOCK_SIZE, ltcAesKey,
                        AES_KEY_SIZE);
#else
     MMCAU_AES_EncryptEcb(pt, AES_KEY_SCH, 10, pt);
#endif
}
void HW_AES128_Dec(uint8_t *pt)
{
#if USE_TRUSTED_CRYPTO
     LTC_AES_DecryptEcb(LTC0, pt, pt, LTC_AES_BLOCK_SIZE, ltcAesKey,
                        AES_KEY_SIZE, kLTC_DecryptKey);
     nbAesBlocks++;
     if (nbAesBlocks >= ROUNDS_BEFORE_RESEED) {
       nbAesBlocks = 0;
       TRNG_GetRandomData(TRNG0, &maskSeed, sizeof(maskSeed));
       LTC_SetDpaMaskSeed(LTC0, maskSeed);
     }
#else
     MMCAU_AES_DecryptEcb(pt, AES_KEY_SCH, 10, pt);
#endif
}
