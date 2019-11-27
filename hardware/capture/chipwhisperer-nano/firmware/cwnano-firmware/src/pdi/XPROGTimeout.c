/*
     Timeout Count
     Copyright (C) NewAE Technology Inc., 2014
*/

/*
  Copyright 2014  NewAE Technology Inc. Based on LUFA which is:
  Copyright 2014  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/


#include "XPROGTimeout.h"
#include "conf_usb.h"
#include "tc.h"
#include <string.h>

#define TC_CHANNEL_TICKCNT 0
#define TC_WAVEFORM_DIVISOR 8
#define TC_WAVEFORM_FREQUENCY 2500

//Timeout counter for XMEGA Protocol
uint32_t TimeoutTicksRemaining;
static uint32_t defaultTimeoutTicks = 500;
static bool timerEnabled = false;

void set_timeout(uint32_t timeoutTicks)
{
	defaultTimeoutTicks = timeoutTicks;
	//printf("Timeout = %d\n", defaultTimeoutTicks);
}

void setup_timeoutcnt(void)
{
	sysclk_enable_peripheral_clock(ID_TC0);
	
	/* Configure TC interrupts for TC TC_CHANNEL_CAPTURE only */
	irq_register_handler(TC0_IRQn, 1);
	
	// Init TC to waveform mode.
	tc_init(TC0, TC_CHANNEL_TICKCNT,
	1 /* Clock = 96MHz / 8 */
	| TC_CMR_WAVE       // Waveform mode is enabled
	| TC_CMR_CPCTRG
	);
	
	// Configure waveform frequency and duty cycle.
	uint32_t rc = (sysclk_get_peripheral_bus_hz(TC0) /
	TC_WAVEFORM_DIVISOR /
	TC_WAVEFORM_FREQUENCY);
	tc_write_rc(TC0, TC_CHANNEL_TICKCNT, rc);
	tc_enable_interrupt(TC0, TC_CHANNEL_TICKCNT, TC_IER_CPCS);
	
	timerEnabled = true;
}

void disable_timeoutcnt(void)
{
	tc_disable_interrupt(TC0, TC_CHANNEL_TICKCNT, TC_IER_CPCS);
	sysclk_disable_peripheral_clock(ID_TC0);
	
	timerEnabled = false;
}

void start_timeoutcnt(void)
{
	if(timerEnabled == false){
		setup_timeoutcnt();
	}
	tc_stop(TC0, TC_CHANNEL_TICKCNT);
	TimeoutTicksRemaining = defaultTimeoutTicks; /* x / 2500 = timeout in seconds  */
	tc_start(TC0, TC_CHANNEL_TICKCNT);
}

void stop_timeoutcnt(void)
{
	tc_stop(TC0, TC_CHANNEL_TICKCNT);
}

ISR(TC0_Handler)
{
	if(TimeoutTicksRemaining){
		TimeoutTicksRemaining--;
	}
	tc_get_status(TC0, TC_CHANNEL_TICKCNT);
}
