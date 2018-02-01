/*
 * simpleserial.h
 *
 * Created: 03/08/2016 4:43:45 PM
 *  Author: greg
 */ 


#ifndef SIMPLESERIAL_H_
#define SIMPLESERIAL_H_

#include <stdint.h>

uint8_t* simpleserial_getaddress(void);
int simpleserial_get(uint8_t size);
void simpleserial_put(uint8_t* output, uint8_t size);

#endif /* SIMPLESERIAL_H_ */