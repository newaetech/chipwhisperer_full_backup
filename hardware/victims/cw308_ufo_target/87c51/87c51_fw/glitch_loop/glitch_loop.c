// glitch_loop.c

#include <8052.h>
#include <stdio.h>

#include "glitch_loop.h"

#define trigger_high() P1 |= 0x01
#define trigger_low()  P1 &= ~(0x01)

void glitch_loop()
{
	unsigned int i;
	unsigned int j;
	unsigned int cnt;
	unsigned char inc=0;

	while (1) 
	{
        cnt = 0;
        trigger_high();     		    
        for(i = 0; i < 200; i++)
		{
            for(j = 0; j < 200; j++)
			{
                cnt++;
            }
        }
        trigger_low();
        inc++;
		
		
		printf_tiny("%u: %u %u %u\n", inc, i, j, cnt);
    }
}