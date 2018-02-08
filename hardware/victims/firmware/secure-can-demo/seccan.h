#ifndef SECCAN_H
#define SECCAN_H
#include <stdint.h>
typedef struct can_input {
	uint32_t msgnum;
	uint32_t baseid;
	uint8_t data[4];
} can_input;

typedef struct seccan_packet {
	uint32_t ext_id;
	uint8_t payload[8];
} seccan_packet;

//encrypts the data in in and puts the finished product in out
void encrypt_can_packet(seccan_packet *out, can_input *in);

//decrypts in, putting the baseid, msgnum, and data into out
//returns 0 upon successful authentication of the message
//and nonzero for unsuccessful authentication of the message
int decrypt_can_packet(can_input *out, seccan_packet *in);
#endif
