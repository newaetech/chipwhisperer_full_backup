#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#if defined(__CC_ARM)       /* Keil */
#define NULL 0
#define INLINE __inline
#elif defined (__GNUC__)    // GCC
#define INLINE inline
#else
#error "Sorry: unsupported compiler (please use GCC or ARMCC)!"
#endif

void cortex_m4_cycles_reset(void);

unsigned int get_cortex_m4_cycles(void);

int _putchar(char character);

int _getchar(char *character);

int _getbuffer(char *buffer, unsigned int len);

void _comm_byte_received(void);

int _comm_putchar(char character);

int _comm_getchar(char *character);

int _comm_getbuffer(char *buffer, unsigned int len);


int get_random(unsigned char *buff, unsigned int bufflen);

#endif /* __PLATFORM_H__ */
