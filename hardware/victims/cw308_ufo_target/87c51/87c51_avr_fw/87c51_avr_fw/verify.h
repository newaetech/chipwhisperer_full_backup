/*
 * verify.h
 *
 * Created: 09/08/2016 11:51:41 AM
 *  Author: greg
 */ 


#ifndef VERIFY_H_
#define VERIFY_H_

#include <stdint.h>

void verify_init();
uint8_t verify_byte(uint16_t address);

#endif /* VERIFY_H_ */