/*
 Copyright (C) 2015 NewAE Technology Inc. All Rights Reserved.
*/

#include <asf.h>
#include <string.h>
#include "tasks.h"

volatile uint32_t task_tickCnt;
bool timerEnabled;

/*
TODO: Need to have a proper event loop not this crappy hack
*/

void process_events(void)
{
	static uint32_t nextCnt = 0;
	
	/* Hack for now - fixed at 100 mS, one event */	
	if (task_tickCnt > nextCnt){
		
		do_task();
						
		nextCnt += 100;
		if (nextCnt < task_tickCnt){
			task_tickCnt = 0;
			nextCnt = 100;
		}
	}
}
