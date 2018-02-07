// glitchloop.c

#include "modules.h"

#include <8052.h>
#include <stdio.h>

#include "utility.h"

void main_glitchloop()
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