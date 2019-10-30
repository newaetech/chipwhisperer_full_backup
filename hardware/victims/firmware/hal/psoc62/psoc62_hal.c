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


cy_stc_scb_uart_context_t uart_ctx;

void platform_init(void)
{
    init_cycfg_all();
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