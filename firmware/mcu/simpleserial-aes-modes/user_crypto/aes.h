#include <stdint.h>
typedef uint8_t byte;
byte * aes(byte *in, byte *skey);
void addRoundKey(void);
void subBytes(void);
void shiftRows(void);
byte xtime(byte x);
void mixColumns(void);
void computeKey(byte rcon);
