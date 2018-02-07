// pass_check.c

// See main.c for a note on why this file isn't included.
//#include <mcs51/serial_IO.h>

#include <8052.h>
#include <stdio.h>

#include "pass_check.h"
#include "secret_pass.h"

#define trigger_high() P1 |= 0x01
#define trigger_low()  P1 &= ~(0x01)

#define led1_high()    P2 |= (1<<5)
#define led1_low()     P2 &= ~(1<<5)

#define led2_high()    P2 |= (1<<4)
#define led2_low()     P2 &= ~(1<<4)

#define MAX_PASS_LENGTH 32

int get_password(char* str)
{
	int n = 0;
	while(1)
	{
		str[n] = getchar();
		if(str[n] == '\r')
		{
			str[n] = '\0';
			break;
		}
		n++;
	}
	return n;
}

void check_password()
{
	char typed_password[MAX_PASS_LENGTH];
	int i;
	int success;
	
	trigger_low();
	
	// Print a welcome message
	printf_small("Welcome to your 87C51 login, [username]\n");
	printf_small("Please enter high-entropy password:\n");

	// Read password from serial port
	get_password(typed_password);
	
	// Compare password to stored one
	trigger_high();
	success = 1;
	for(i = 0; i < MAX_PASS_LENGTH; i++)
	{
		if(stored_password[i] == '\0')
			break;
		if(typed_password[i] != stored_password[i])
		{
			success = 0;
			break;
		}
	}
	trigger_low();
	
	// If everything is good, tell them and turn on green light
	if(success)
	{
		printf_small("Password check OK\n");
		printf_small("Last login: [time]\n");
		led1_high();
		led2_low();
	}
	// If failed comparison, tell them and turn on red light
	else
	{
		printf_small("Password check FAILED\n");
		printf_small("Reporting incident to police...\n");
		led1_low();
		led2_high();
	}
	
	return;
}