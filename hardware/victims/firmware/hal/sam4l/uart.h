#ifndef UART_SAM4L_H_
#define UART_SAM4L_H_

void uart1_init(void);
void uart1_putch(char c);
char uart1_getch(void);

#endif