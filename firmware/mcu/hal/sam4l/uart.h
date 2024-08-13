#ifndef UART_SAM4L_H_
#define UART_SAM4L_H_

void uart0_init(void);
void uart0_putch(char c);
char uart0_getch(void);

#endif