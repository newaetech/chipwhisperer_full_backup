/*
 * naeusb_nano.h
 *
 * Created: 6/28/2021 9:50:51 PM
 *  Author: adewa
 */ 


#ifndef NAEUSB_NANO_H_
#define NAEUSB_NANO_H_

#define REQ_MEMREAD_BULK 0x10
#define REQ_MEMWRITE_BULK 0x11
#define REQ_MEMREAD_CTRL 0x12
#define REQ_MEMWRITE_CTRL 0x13

#define REQ_GPIO_DIR 0x24
#define REQ_GPIO_OUT 0x25
#define REQ_GPIO_IN  0x26
#define REQ_CLK_OUT  0x27
#define REQ_ADCCLK_OUT 0x28
#define REQ_ARM 0x29
#define REQ_SAMPLES 0x2A
#define REQ_BUFSIZE 0x2B
#define REQ_GLITCHSET 0x2C
#define REQ_GLITCHGO 0x2D

void nano_register_handlers(void);

#endif /* NAEUSB_NANO_H_ */