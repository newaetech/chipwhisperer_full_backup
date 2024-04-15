// decryption.c
// Decrypt data using rot-13
// TODO: Improve security by using double-rot-13
//
// Authors:
//   Greg d'Eon <gdeon@newae.com>

#include "decryption.h"
#include <stdint.h>

int num_chars = 2*26;
char __xdata input[]  = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
char __xdata output[] = "nopqrstuvwxyzabcdefghijklmNOPQRSTUVWXYZABCDEFGHIJKLM";

// Decrypt len bytes in the buffer
void decrypt_data(uint8_t* buffer, uint8_t len)
{
	// Apply rot-13 to each byte
	int i, j;
	for(i = 0; i < len; i++)
	{
		// Search for character inside input array
		for(j = 0; j < num_chars; j++)
		{
			if(buffer[i] == input[j])
			{
				buffer[i] = output[j];
				break;
			}
		}
	}
}
