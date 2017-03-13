#ifdef SAM4L_AES_H_
#define SAM4L_AES_H_

void aes_init(void);
void aes_set_key(uint8_t * key);
void aes_encrypt(uint8_t * pt);

#endif //SAM4L_AES_H_