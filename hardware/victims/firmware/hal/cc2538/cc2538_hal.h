#ifndef CC2538_HAL_H
#define CC2538_HAL_H

void init_uart(void);
void putch(char c);
char getch(void);

void trigger_setup(void);
void trigger_low(void);
void trigger_high(void);

#endif //CC2538_HAL_H