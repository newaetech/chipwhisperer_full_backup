/*
 * XPROGTimeout.h
 *
 * Created: 14/12/2014 3:28:50 PM
 *  Author: colin
 */ 


#ifndef XPROGTIMEOUT_H_
#define XPROGTIMEOUT_H_

#include <asf.h>

void setup_timeoutcnt(void);
void start_timeoutcnt(void);
void stop_timeoutcnt(void);
void disable_timeoutcnt(void);
void set_timeout(uint32_t timeoutTicks);

extern uint32_t TimeoutTicksRemaining;

#endif /* XPROGTIMEOUT_H_ */