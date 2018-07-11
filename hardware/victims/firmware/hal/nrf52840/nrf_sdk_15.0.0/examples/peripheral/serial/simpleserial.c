// simpleserial.c

#include "simpleserial.h"
#include <stdint.h>

typedef struct ss_cmd
{
	char c;
	unsigned int len;
	uint8_t (*fp)(uint8_t*);
} ss_cmd;

#define MAX_SS_CMDS 10
static ss_cmd commands[MAX_SS_CMDS];
static int num_commands = 0;

#define MAX_SS_LEN 64

#define SS_VER_1_0 0
#define SS_VER_1_1 1

#define SS_VER 1

void putch(char c);
char getch(void);

static char hex_lookup[16] =
{
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

int hex_decode(int len, char* ascii_buf, uint8_t* data_buf)
{
	for(int i = 0; i < len; i++)
	{
		char n_hi = ascii_buf[2*i];
		char n_lo = ascii_buf[2*i+1];

		if(n_lo >= '0' && n_lo <= '9')
			data_buf[i] = n_lo - '0';
		else if(n_lo >= 'A' && n_lo <= 'F')
			data_buf[i] = n_lo - 'A' + 10;
		else if(n_lo >= 'a' && n_lo <= 'f')
			data_buf[i] = n_lo - 'a' + 10;
		else
			return 1;

		if(n_hi >= '0' && n_hi <= '9')
			data_buf[i] |= (n_hi - '0') << 4;
		else if(n_hi >= 'A' && n_hi <= 'F')
			data_buf[i] |= (n_hi - 'A' + 10) << 4;
		else if(n_hi >= 'a' && n_hi <= 'f')
			data_buf[i] |= (n_hi - 'a' + 10) << 4;
		else
			return 1;
	}

	return 0;
}

// Callback function for "v" command.
// This can exist in v1.0 as long as we don't actually send back an ack ("z")
uint8_t check_version(uint8_t* v)
{
	return 0x00;
}

// Set up the SimpleSerial module by preparing internal commands
// This just adds the "v" command for now...
void simpleserial_init()
{
	simpleserial_addcmd('v', 0, check_version);
}

int simpleserial_addcmd(char c, unsigned int len, uint8_t (*fp)(uint8_t*))
{
	if(num_commands >= MAX_SS_CMDS)
		return 1;

	if(len >= MAX_SS_LEN)
		return 1;

	commands[num_commands].c   = c;
	commands[num_commands].len = len;
	commands[num_commands].fp  = fp;
	num_commands++;

	return 0;
}

void simpleserial_get(void)
{
	char ascii_buf[2*MAX_SS_LEN];
	uint8_t data_buf[MAX_SS_LEN];
	char c;

	// Find which command we're receiving
	c = getch();

	int cmd;
	for(cmd = 0; cmd < num_commands; cmd++)
	{
		if(commands[cmd].c == c)
			break;
	}

	// If we didn't find a match, give up right away
	if(cmd == num_commands)
		return;

	// Receive characters until we fill the ASCII buffer
	for(int i = 0; i < 2*commands[cmd].len; i++)
	{
		c = getch();

		// Check for early \n
		if(c == '\n' || c == '\r')
			return;

		ascii_buf[i] = c;
	}

	// Assert that last character is \n or \r
	c = getch();
	if(c != '\n' && c != '\r')
		return;

	// ASCII buffer is full: convert to bytes 
	// Check for illegal characters here
	if(hex_decode(commands[cmd].len, ascii_buf, data_buf))
		return;

	// Callback
	uint8_t ret[1];
	ret[0] = commands[cmd].fp(data_buf);
	
	// Acknowledge (if version is 1.1)
#if SS_VER == SS_VER_1_1
	simpleserial_put('z', 1, ret);
#endif
}

void simpleserial_put(char c, int size, uint8_t* output)
{
	// Write first character
	putch(c);

	// Write each byte as two nibbles
	for(int i = 0; i < size; i++)
	{
		putch(hex_lookup[output[i] >> 4 ]);
		putch(hex_lookup[output[i] & 0xF]);
	}

	// Write trailing '\n'
	putch('\n');
}
