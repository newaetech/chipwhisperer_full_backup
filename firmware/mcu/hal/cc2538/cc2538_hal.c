
#include "hw_memmap.h"
#include "hw_types.h"
#include "hw_ioc.h"

#include "gpio.h"
#include "sys_ctrl.h"
#include "uart.h"
#include "gpio.h"
#include "ioc.h"
#include <stdint.h>
#include <stdlib.h>

//#define TRIG_PIN 0x07
#define TRIG_PIN GPIO_PIN_2 
#define TRIG_PORT GPIO_B_BASE

#define UART_RX_PIN GPIO_PIN_4
#define UART_TX_PIN GPIO_PIN_5

#define UART_PORT GPIO_B_BASE

void platform_init(void)
{
	SysCtrlClockSet(false, true, SYS_CTRL_SYSDIV_16MHZ); //32MHz ext clock
	SysCtrlIOClockSet(SYS_CTRL_SYSDIV_16MHZ); //io clk same as sys clk
}

void init_uart(void)
{
	SysCtrlPeripheralEnable(SYS_CTRL_PERIPH_UART1); //enable uart
	UARTDisable(UART1_BASE); //disable UART
	UARTIntDisable(UART1_BASE, 0x1FFF); //disable uart interrupts
	UARTClockSourceSet(UART1_BASE, UART_CLOCK_PIOSC); //IO clk as uart clock source

	IOCPinConfigPeriphOutput(UART_PORT, UART_TX_PIN, IOC_MUX_OUT_SEL_UART1_TXD); //PB5 as TX
	GPIOPinTypeUARTOutput(UART_PORT, UART_TX_PIN);

	IOCPinConfigPeriphInput(UART_PORT, UART_RX_PIN, IOC_UARTRXD_UART1); //PB4 as RX
	GPIOPinTypeUARTInput(UART_PORT, UART_RX_PIN);

	//UART 38400 baud 8-N-1 operation
	UARTConfigSetExpClk(UART1_BASE, SysCtrlClockGet(), 38400, 
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	UARTEnable(UART1_BASE);
}


void putch(char c)
{
	UARTCharPut(UART1_BASE, c);
}

char getch(void)
{
	char c = UARTCharGet(UART1_BASE);
	return c;
}


void trigger_setup(void)
{
	GPIOPinTypeGPIOOutput(TRIG_PORT, TRIG_PIN); //let's try this
	GPIOPinTypeGPIOOutput(GPIO_C_BASE, GPIO_PIN_0);
}

void trigger_low(void)
{
	GPIOPinWrite(TRIG_PORT, TRIG_PIN, 0); //should leave rest of pins alone
	GPIOPinWrite(GPIO_C_BASE, GPIO_PIN_0, 0);
	
}

void trigger_high(void)
{
	GPIOPinWrite(TRIG_PORT, TRIG_PIN, 0xFF); //should leave rest of pins alone
	GPIOPinWrite(GPIO_C_BASE, GPIO_PIN_0, 1);
}
/*
int main(void)
{
	GPIOPinTypeGPIOOutput(GPIO_C_BASE, GPIO_PIN_0);
	GPIOPinTypeGPIOOutput(GPIO_C_BASE, GPIO_PIN_1);
	GPIOPinWrite(GPIO_C_BASE, GPIO_PIN_0, 0);
	GPIOPinWrite(GPIO_C_BASE, GPIO_PIN_1, 1);

	while(1) {
		volatile uint32_t a;
		for (a = 0; a < 500000; a++);
		
		GPIOPinWrite(GPIO_C_BASE, GPIO_PIN_0, 0);
		for (a = 0; a < 500000; a++);
		
		GPIOPinWrite(GPIO_C_BASE, GPIO_PIN_0, 1);
	}
	return 0;
}*/
