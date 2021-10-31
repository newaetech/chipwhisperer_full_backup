/*
 Copyright (C) 2015 NewAE Technology Inc. All Rights Reserved.
*/

#ifndef TASKS_H_
#define TASKS_H_

void process_events(void);
void do_task(void);

extern volatile uint32_t task_tickCnt;


#endif /* TASKS_H_ */