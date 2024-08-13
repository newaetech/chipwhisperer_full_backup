#include <stdint.h>
#include <string.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"

#include "efm32gg11_hal.h"

//make flashing device lock bootloader
//const uint32_t __attribute__((section (".debug_lock"))) user_lock_word[1] = {0xFFFFFFFD};

void platform_init(void)
{
    CHIP_Init();
    
    CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_EXTERNAL_CLOCK;
    CMU_HFXOInit(&hfxoInit);
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
    CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);
    
    CMU_ClockEnable(cmuClock_GPIO, true);
    CMU_ClockEnable(cmuClock_USART1, true);
}

void init_uart(void)
{
    GPIO_PinModeSet(gpioPortE, 10, gpioModePushPull, 1); //PE10 = Tx
    GPIO_PinModeSet(gpioPortE, 11, gpioModeInput, 0);    //PE11 = RX
    
#error "Unfinished HAL - need to enable IO pin mux here"
    
    USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;
    init.baudrate = 38400;
    init.refFreq = 7.3728E6;
    
    USART_InitAsync(USART0, &init);
}

void putch(char c)
{
    USART_Tx(USART0, c);
}

char getch(void)
{
    return USART_Rx(USART0);
}

void trigger_setup(void)
{
     GPIO_PinModeSet(gpioPortB, 8, gpioModePushPull, 0);
}
void trigger_low(void)
{
     GPIO_PinOutClear(gpioPortB, 8);
}
void trigger_high(void)
{
     GPIO_PinOutSet(gpioPortB, 8);
}