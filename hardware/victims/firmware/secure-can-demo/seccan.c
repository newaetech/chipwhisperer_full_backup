#include "seccan.h"
#include "aes-independant.h"
#include <string.h>

uint8_t Kenc[] = {0x21, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};
uint8_t Kauth[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
uint8_t IV[] = {0x01, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

//encrypts the data in in and puts the finished product in out
void encrypt_can_packet(seccan_packet *out, can_input *in)
{
	uint8_t nonce_enc[16] = {0};
	uint8_t nonce_auth[16] = {0};

	nonce_enc[0] = (in->msgnum >> 16) & 0xFF;
	nonce_enc[1] = (in->msgnum >> 8) & 0xFF;
	nonce_enc[2] = in->msgnum & 0xFF;
	nonce_enc[3] = (in->baseid >> 8) & 0xFF;
	nonce_enc[4] = (in->baseid & 0xFF);

	memcpy(nonce_auth, nonce_enc, 5); //copy nonce over to enc
	//nonce_enc all done
	memcpy(nonce_auth + 12, in->data, 4); //copy data over

	//do XORing with IV for auth
	int i = 0;
	for (i = 0; i < 16; i++) {
		nonce_auth[i] ^= IV[i];
	}

	aes_indep_key(Kenc);
	aes_indep_enc(nonce_enc);

	aes_indep_key(Kauth);
	aes_indep_enc(nonce_auth);

	for (i = 8; i < 12; i++) {
		nonce_enc[i] ^= in->data[i - 8];
	}
	for (i = 0; i < 4; i++) {
		nonce_enc[i + 12] ^= nonce_auth[i];
	}

	out->ext_id = (in->baseid) & 0x7FF;
	out->ext_id |= (in->msgnum << 11) & 0x1FFFF800;
	memcpy(out->payload, nonce_enc + 8, 8);
}

//decrypts in, putting the baseid, msgnum, and data into out
//returns 0 upon successful authentication of the message
//and nonzero for unsuccessful authentication of the message
int decrypt_can_packet(can_input *out, seccan_packet *in)
{
	out->baseid = in->ext_id & 0x7FF;
	out->msgnum = (in->ext_id >> 11) & 0x3FFFF;
	uint8_t nonce_enc[16] = {0};
	uint8_t nonce_auth[16] = {0};
	//first need output from Kenc AES, so first steps are the same

	nonce_enc[0] = (out->msgnum >> 16) & 0xFF;
	nonce_enc[1] = (out->msgnum >> 8) & 0xFF;
	nonce_enc[2] = out->msgnum & 0xFF;
	nonce_enc[3] = (out->baseid >> 8) & 0xFF;
	nonce_enc[4] = (out->baseid & 0xFF);

	memcpy(nonce_auth, nonce_enc, 5); //copy nonce over to enc

	aes_indep_key(Kenc);
	aes_indep_enc(nonce_enc);

	//can now get data by XORing output of Kenc AES with data part of packet
	int i = 0;
	for (; i < 4; i++) {
		out->data[i] = in->payload[i] ^ nonce_enc[i + 8];
	}

	//now check authentication by running through Kauth procedure
	memcpy(nonce_auth + 12, out->data, 4); //copy data over

	for (i = 0; i < 16; i++) {
		nonce_auth[i] ^= IV[i];
	}

	aes_indep_key(Kauth);
	aes_indep_enc(nonce_auth);

	for (i = 0; i < 4; i++) {
		nonce_auth[i] ^= nonce_enc[i + 12];
	}

	//returns 0 if auth matches, nonzero if auth doesn't match
	return memcmp(nonce_auth, in->payload + 4, 4);
}
