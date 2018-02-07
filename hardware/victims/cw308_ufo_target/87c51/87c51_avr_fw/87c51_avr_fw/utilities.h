/*
 * utilities.h
 *
 * Created: 29/07/2016 4:20:49 PM
 *  Author: greg
 */ 


#ifndef UTILITIES_H_
#define UTILITIES_H_

void utilities_init();

void led2_high();
void led2_low();
void led2_toggle();

void heartbeat_enable();
void heartbeat_disable();

void trigger_high();
void trigger_low();

void target_enable();

#endif /* UTILITIES_H_ */