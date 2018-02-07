// utility.h

#ifndef UTILITY_H
#define UTILITY_H

#include <8052.h>

// Macros that we want to use in a few places
#define trigger_high() P1 |= 0x01
#define trigger_low()  P1 &= ~(0x01)

#define led1_high()    P2 |= (1<<5)
#define led1_low()     P2 &= ~(1<<5)

#define led2_high()    P2 |= (1<<4)
#define led2_low()     P2 &= ~(1<<4)

#define get_mode()     (P3 >> 3) & 0x07

#endif // UTILITY_H