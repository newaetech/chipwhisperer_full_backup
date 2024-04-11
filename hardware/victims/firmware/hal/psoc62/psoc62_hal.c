/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2019 NewAE Technology Inc.

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
    
    Portions of this HAL are based on Atmel ASF.
*/

#include "psoc62_hal.h"
#include "cy_crypto_common.h"
#include "cy_crypto_core_aes.h"

cy_stc_scb_uart_context_t uart_ctx;

void platform_init(void)
{
    init_cycfg_all();
    
    /* Enable the Crypto block */
    Cy_Crypto_Core_Enable(CRYPTO);
}

void init_uart(void)
{
	Cy_SCB_UART_Init(SCB1, &scb_1_config, &uart_ctx);
	Cy_SysClk_PeriphAssignDivider(PCLK_SCB1_CLOCK, CY_SYSCLK_DIV_16_BIT, 0);
	Cy_SCB_UART_Enable(SCB1);
}


void putch(char c)
{
	Cy_SCB_UART_Put(SCB1, c);
	while (!Cy_SCB_UART_IsTxComplete(SCB1));
}

char getch(void)
{
    uint32_t val = CY_SCB_UART_RX_NO_DATA;
    
    while(val == CY_SCB_UART_RX_NO_DATA) {
        val = Cy_SCB_UART_Get(SCB1);
    }
    
    return (char)val;
}


cy_stc_crypto_aes_state_t aesState;

/* Key used for AES encryption*/
CY_ALIGN(4) uint8_t AES_Key[16]={};
CY_ALIGN(4) uint8_t message[16];
CY_ALIGN(4) uint8_t encrypted_msg[16];

void HW_AES128_Init(void)
{
    ;//Init is done in the main function instead
}

void HW_AES128_Enc_pretrigger(uint8_t * pt)
{
    for(int i = 0; i < 16; i++){
        message[i] = pt[i];
    }
    
    Cy_Crypto_Core_Aes_Init(CRYPTO, AES_Key, CY_CRYPTO_KEY_AES_128, &aesState);
}

void HW_AES128_Enc(uint8_t * pt)
{
    Cy_Crypto_Core_Aes_Ecb(CRYPTO, CY_CRYPTO_ENCRYPT, encrypted_msg, message, &aesState);
    
    /* Wait for Crypto Block to be available */
    Cy_Crypto_Core_WaitForReady(CRYPTO);
}

void HW_AES128_Enc_posttrigger(uint8_t* pt)
{
    for(int i = 0; i < 16; i++){
        pt[i] = encrypted_msg[i];
    }
}

void HW_AES128_LoadKey(uint8_t * key)
{
    for(int i = 0; i < 16; i++){
        AES_Key[i] = key[i];
    }
}

