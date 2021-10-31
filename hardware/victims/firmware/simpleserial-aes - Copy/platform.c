#include "platform.h"
#include "string.h"

unsigned int round = 0;
int get_random(unsigned char *buff, unsigned int bufflen){
        int i = 0;
        
	buff[0] = 0xAB+round;
        for(i = 1; i < bufflen; i++){
                buff[i] = 0xCD+buff[i-1]+round;
        }
        buff[bufflen-1] = (0xEF+round)| 1;

	round++;
	return 0;
}

#ifdef MEASURE_PERFORMANCE

#ifdef USE_QEMU_PLATFORM
#error "Sorry: QEMU does not have a proper support of time measurement, please do not use it!"
#endif
#ifdef USE_KEIL_PLATFORM
#error "Sorry: KEIL does not have a proper support of time measurement, please do not use it!"
#endif

#define DEMCR_TRCENA    0x01000000
/* Core Debug registers */
#define SCB_DEMCR       (*((volatile unsigned int *)0xE000EDFC))
#define DWT_CTRL        (*(volatile unsigned int *)0xE0001000)
#define CYCCNTENA       (1<<0)
#define DWT_CYCCNT      ((volatile unsigned int *)0xE0001004)

void cortex_m4_cycles_reset(void)
{
    /* Enable DWT */
    SCB_DEMCR |= DEMCR_TRCENA;
    *DWT_CYCCNT = 0;
    /* Enable CPU cycle counter */
    DWT_CTRL |= CYCCNTENA;
}

unsigned int get_cortex_m4_cycles(void){
	return *((volatile unsigned int *)DWT_CYCCNT);
}
#else
void cortex_m4_cycles_reset(void)
{
	return;
}

unsigned int get_cortex_m4_cycles(void){
	return 0;
}

#endif



#ifdef USE_QEMU_PLATFORM
/******* QEMU platform *******************/

/* Override the _start ELF entry point in emulation mode:
 * no need to do unnecessary stuff that could generate errors. 
 */
int main(void);
static inline int exit(int exitcode);
void _start(void){
	main();
	exit(0);
	return;
}


#define __NR_read  3
#define __NR_write 4
#define __NR_exit  1

#if defined (__GNUC__)    // GCC
/* Note: this is an ugly workaround to have a syscall in thumb2 mode ...
 * But this works!
 */
#pragma GCC push_options
#pragma GCC optimize ("-fomit-frame-pointer")
#pragma GCC optimize ("-O0")

static inline int write(int fd, void *buf, int len)
{
    int rval;
        asm volatile ("mov      r0, %1\n\t"
                "mov    r1, %2\n\t"
                "mov    r2, %3\n\t"
                "mov    r7, %4\n\t"
                "svc    #7\n\t"
                "mov    %0, r0\n\t"
                : "=r" (rval)
                : "r" (fd),
                  "r" (buf),
                  "r" (len),
                  "Ir" (__NR_write)
                : "r0", "r1", "r2", "r7");
    return rval;
}

static inline int read(int fd, void *buf, int len)
{
    int rval;
        asm volatile ("mov      r0, %1\n\t"
                "mov    r1, %2\n\t"
                "mov    r2, %3\n\t"
                "mov    r7, %4\n\t"
                "svc    #7\n\t"
                "mov    %0, r0\n\t"
                : "=r" (rval)
                : "r" (fd),
                  "r" (buf),
                  "r" (len),
                  "Ir" (__NR_read)
                : "r0", "r1", "r2", "r7");
    return rval;
}

static inline int exit(int exitcode)
{
    int rval;
        asm volatile ("mov      r0, %1\n\t"
                "mov    r7, %2\n\t"
                "svc    #7\n\t"
                "mov    %0, r0\n\t"
                : "=r" (rval)
                : "r" (exitcode),
                  "Ir" (__NR_exit)
                : "r0", "r7");
    return rval;
}

#pragma GCC pop_options

#elif defined(__CC_ARM) // ARMCC

static INLINE int write(int fd, void *buf, int len)
{
    /* FIXME */
    int rval;
        __asm
	{
		mov    r0, fd
                mov    r1, buf
                mov    r2, len
                mov    r7, __NR_write
                svc    #7
                mov    rval, r0
	}
    return rval;
}

static INLINE int read(int fd, void *buf, int len)
{
    /* FIXME */
    int rval;
        __asm
	{
		mov    r0, fd
                mov    r1, buf
                mov    r2, len
                mov    r7, __NR_read
                svc    #7
                mov    rval, r0
	}
    return rval;
}

#else
#error "Sorry: unsupported compiler (please use GCC or ARMCC)!"
#endif


int _putchar(char character)
{
	if(write(2, &character, 1) != 1){
		return -1;
	}
	return 0;
}

int _getchar(char *character)
{
	if(read(2, character, 1) != 1){
		return -1;
	}
	return 0;
}

int _getbuffer(char *buffer, unsigned int len)
{
	unsigned int i;

	for(i = 0; i < len; i++){
		if(_getchar(&buffer[i])){
			goto err;
		}
	}

	return 0;
err:
	return -1;
}

void _comm_byte_received(void){
	return;
}

int _comm_putchar(char character)
{
	if(write(1, &character, 1) != 1){
		return -1;
	}
	return 0;
}

int _comm_getchar(char *character)
{
	if(read(0, character, 1) != 1){
		return -1;
	}
	return 0;
}

int _comm_getbuffer(char *buffer, unsigned int len)
{
	unsigned int i;

	for(i = 0; i < len; i++){
		if(_comm_getchar((char*)&buffer[i])){
			goto err;
		}
	}

	return 0;
err:
	return -1;
}

#elif defined(USE_STM32_DISCO_PLATFORM)
/******* STM32 DISCO platform *******************/

typedef unsigned short (*cb_getc_t)(void);
typedef void (*cb_putc_t)(unsigned short);

extern cb_putc_t console_putc;
int _putchar(char character)
{
	console_putc((unsigned short)character);
	if(character == '\n'){
		console_putc((unsigned short)'\r');
	}
	return 0;
}

extern cb_getc_t console_getc;
int _getchar(char *character)
{
	if(character == NULL){
		goto err;
	}
	*character = (char)(console_getc() & 0xff);

	return 0;

err:
	return -1;
}

int _getbuffer(char *buffer, unsigned int len)
{
	unsigned int i;

	for(i = 0; i < len; i++){
		if(_getchar(&buffer[i])){
			goto err;
		}
	}

	return 0;
err:
	return -1;
}


/* In order to avoid desynchronization when receiving, we use a ring buffer */
typedef enum { false, true } bool;
#define BUF_SIZE        4096
#define BUF_MAX         (BUF_SIZE - 1)

struct s_ring {
    unsigned int start;
    unsigned int end;
    bool     full;
    char buf[BUF_SIZE];
};
static volatile struct s_ring ring_buffer;

static void init_ring_buffer(void)
{
    /* init flags */
    int i = 0;
    ring_buffer.end = 0;
    ring_buffer.start = ring_buffer.end;
    ring_buffer.full = false;

    /* memsetting buffer
     */
    for (i = 0; i < BUF_MAX; i++) {
        ring_buffer.buf[i] = '\0';
    }
}
static volatile bool ring_buffer_initialized = false;
static inline int ring_buffer_write_char(const char c)
{
    if(ring_buffer_initialized == false){
        init_ring_buffer();
        ring_buffer_initialized = true;
    }
    /* if the ring buffer is full when we try to put char in it,
     * the car is discared, waiting for the ring buffer to be flushed.
     */
    if (ring_buffer.full) {
        goto err;
    }
    ring_buffer.buf[ring_buffer.end] = c;
    if (((ring_buffer.end + 1) % BUF_MAX) != ring_buffer.start) {
        ring_buffer.end++;
        ring_buffer.end %= BUF_MAX;
    } else {
        /* full buffer detection */
        ring_buffer.full = true;
    }
    return 0;
err:
    return -1;
}
static inline int ring_buffer_read_char(char *c)
{
    if(c == NULL){
        goto err;
    }
    if(ring_buffer_initialized == false){
        init_ring_buffer();
        ring_buffer_initialized = true;
        goto err;
    }
    /* Is ring buffer empty? If yes, this is not normal */
    if(ring_buffer.start == ring_buffer.end){
        goto err;
    }
    *c = ring_buffer.buf[ring_buffer.start];
    ring_buffer.start = (ring_buffer.start + 1) % BUF_MAX;
    ring_buffer.full = false;

    return 0;
err:
    return -1;
}

extern cb_getc_t protocol_getc;
void _comm_byte_received(void){
	if(ring_buffer.full == true){
		return;
	}
	char c = protocol_getc();
	ring_buffer_write_char(c);

	return;
}

static int protocol_local_getc(char *c){
	if(c == NULL){
		goto err;
	}
	if(ring_buffer_read_char(c)){
		goto err;
	}
	return 0;
err:
	return -1;	
}

int _comm_getchar(char *character)
{
	if(character == NULL){
		goto err;
	}
	if(protocol_local_getc(character)){
		goto err;
	}
	return 0;
err:
	return -1;
}

int _comm_getbuffer(char *buffer, unsigned int len)
{
	unsigned int i;
	for(i = 0; i < len; i++){
		while(_comm_getchar(&buffer[i])){};
	}
	return 0;
}

extern cb_putc_t protocol_putc;
int _comm_putchar(char character)
{
	protocol_putc((unsigned short)character);
	return 0;
}

#elif defined(USE_KEIL_PLATFORM)
/******* Keil platform *******************/

/* For the Keil platform, we only use stubs */
int _putchar(char character){
	return 0;
}

int _getchar(char *character){
	return 0;
}

int _getbuffer(char *buffer, unsigned int len){
	return 0;
}

void _comm_byte_received(void){
	return;
}

int _comm_putchar(char character){
	return 0;
}

int _comm_getchar(char *character){
	return 0;
}

int _comm_getbuffer(char *buffer, unsigned int len){
	return 0;
}

#else

#error "Uknown platform! You must define either USE_QEMU_PLATFORM, USE_KEIL_PLATFORM or USE_STM32_DISCO_PLATFORM"

#endif
