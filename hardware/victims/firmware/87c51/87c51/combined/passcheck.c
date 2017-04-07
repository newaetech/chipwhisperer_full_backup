// passcheck.c

#include "modules.h"

#include <stdio.h>
#include <8052.h>

#include "secret_pass.h"
#include "utility.h"

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

void main_passcheck()
{
	char typed_password[MAX_PASS_LENGTH];
	int i;
	int success;


	// Print a welcome message
	printf_tiny("Welcome to your 87C51 login, [username]\n");
	printf_tiny("Please enter high-entropy password:\n");

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
		printf_tiny("Password check OK\n");
		printf_tiny("Last login: [time]\n");
		led1_high();
		led2_low();
	}
	// If failed comparison, tell them and turn on red light
	else
	{
		printf_tiny("Password check FAILED\n");
		printf_tiny("Reporting incident to police...\n");
		led1_low();
		led2_high();
	}
	
	
	// Wait here forever
	while(1);
}