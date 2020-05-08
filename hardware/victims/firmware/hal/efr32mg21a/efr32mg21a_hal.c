#include <stdint.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_se.h"

#include "efr32mg21a_hal.h"

//make flashing device lock bootloader
//const uint32_t __attribute__((section (".debug_lock"))) user_lock_word[1] = {0xFFFFFFFD};

void platform_init(void)
{
    CHIP_Init();
    
    CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;
    hfxoInit.mode = _HFXO_CFG_MODE_EXTCLK;
    CMU_HFXOInit(&hfxoInit);
    CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_HFXO);
    CMU_OscillatorEnable(cmuOsc_HFRCODPLL, false, false);
    
    CMU_ClockEnable(cmuClock_GPIO, true);
    CMU_ClockEnable(cmuClock_USART1, true);
}

void init_uart(void)
{
    GPIO_PinModeSet(gpioPortC, 0, gpioModePushPull, 1); //PC0 = Tx
    GPIO_PinModeSet(gpioPortC, 1, gpioModeInput, 0);    //PC1 = RX

    GPIO->USARTROUTE[0].TXROUTE = (gpioPortC << _GPIO_USART_TXROUTE_PORT_SHIFT)
     | (0 << _GPIO_USART_TXROUTE_PIN_SHIFT);
    GPIO->USARTROUTE[0].RXROUTE = (gpioPortC << _GPIO_USART_RXROUTE_PORT_SHIFT)
     | (1 << _GPIO_USART_RXROUTE_PIN_SHIFT);

    GPIO->USARTROUTE[0].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN | GPIO_USART_ROUTEEN_TXPEN;

    
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
     GPIO_PinModeSet(gpioPortC, 3, gpioModePushPull, 0);
}
void trigger_low(void)
{
     GPIO_PinOutClear(gpioPortC, 3);
}
void trigger_high(void)
{
     GPIO_PinOutSet(gpioPortC, 3);
}

void HW_AES128_Init(void)
{
     ;
}

static uint8_t hw_aes_key[16];

void HW_AES128_LoadKey(uint8_t* key)
{
    for(unsigned int i = 0; i < 16; i++){
        hw_aes_key[i] = key[i];
    }
}

void HW_AES128_Enc(uint8_t* pt)
{
    SE_Response_t command_status;
    uint8_t output[16] = {0};
    unsigned int keysize = 16;

    SE_Command_t command = SE_COMMAND_DEFAULT(SE_COMMAND_AES_ENCRYPT | SE_COMMAND_OPTION_MODE_ECB | SE_COMMAND_OPTION_CONTEXT_WHOLE);
    SE_DataTransfer_t key = SE_DATATRANSFER_DEFAULT(hw_aes_key, keysize);
    SE_DataTransfer_t in = SE_DATATRANSFER_DEFAULT((void*)pt, 16);
    SE_DataTransfer_t out = SE_DATATRANSFER_DEFAULT(output, 16);

    SE_addDataInput(&command, &key);
    SE_addDataInput(&command, &in);
    SE_addDataOutput(&command, &out);
    SE_addParameter(&command, keysize);
    SE_addParameter(&command, 16);

    SE_executeCommand(&command);
    command_status = SE_readCommandResponse();
    
    for(unsigned int i = 0; i < 16; i++){
        pt[i] = output[i];
    }
    

    if ( command_status == SE_RESPONSE_OK ) {
        return;
    } else {
        //We don't have way to report back, just block for now!
        putch('F');
        putch('A');
        putch('I');
        putch('L');
        //while(1);
        return;
    }
}

//not working for now, need cypher key to decrypt, so it needs to be stored
void HW_AES128_Dec(uint8_t *pt)
{
     ;
}
