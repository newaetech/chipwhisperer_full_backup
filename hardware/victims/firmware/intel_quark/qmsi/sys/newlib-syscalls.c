/*
 * Copyright (c) 2016, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the Intel Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * newlib syscalls implementation
 *
 * @defgroup groupSyscalls Syscalls
 * @{
 */

/*
 * Implement newlib system calls and syscall-related functions.
 *
 * Implementation notes:
 * - file descriptor support is limited to stdout/stderr, both targeting UART
 */

#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>

#include "qm_common.h"
#include "qm_interrupt.h"
#include "qm_pinmux.h"
#include "clk.h"
#include "qm_uart.h"

#define ASSERT_STR_HEAD ("\nAssertion failed: file ")
#define ASSERT_STR_TRAIL (", expr: ")

#define PICO_PRINTF_D 1
#define PICO_PRINTF_U 1
#define PICO_PRINTF_X 1
#define PICO_PRINTF_S 1
#define PICO_PRINTF_ESC 1
#define FORMAT_BUF_SIZE 32

static __inline__ int pico_putchar(int c)
{
	qm_uart_write(STDOUT_UART, c);
	return 1;
}

#if (PICO_PRINTF_D || PICO_PRINTF_U || PICO_PRINTF_X)
static __inline__ char last_digit_to_char(unsigned int n, int base, bool upcase)
{
	char c;

	c = n % base;
	if (c <= 9)
		return c + '0';

	return upcase ? c + ('A' - 10) : c + ('a' - 10);
}

static __inline__ int putuint(unsigned int n, int base, bool upcase)
{
	static char format_buf[FORMAT_BUF_SIZE];
	char *s = format_buf;
	int k;
	int i = 0;

	do {
		s[i++] = last_digit_to_char(n, base, upcase);
		n /= base;
	} while (n > 0);

	for (k = i - 1; k >= 0; k--)
		pico_putchar(s[k]);

	return i;
}
#endif

#if (PICO_PRINTF_S)
static __inline__ int pico_putchars(const char *s)
{
	int len = 0;

	for (; *s; s++)
		len += pico_putchar(*s);

	return len;
}
#endif

/**
 * @brief This is an minimally useful subset of the POSIX printf() function.
 *
 * To reduce code size, this pico_printf() implementation only supports a few
 * conversion specifiers:
 *  - @a 'd', @a 'u': for signed and unsigned decimal numbers, respectively;
 *  - @a 'x', @a 'X': for hexadecimal numbers, downcase and upcase;
 *  - @a 's': for NULL terminated strings.
 *
 * Other limitations:
 *  - No flag specifier is implemented;
 *  - No field width specifier is implemented;
 *  - The only supported length modifier is 'l', which is parsed and ignored,
 *    on supported archictetures 'int' and 'long int' are both 32 bits long.
 *  - 32 digits maximum length for formatted numbers.
 */
int pico_printf(const char *format, ...)
{
	const char *s = format;
	int len = 0;
	va_list ap;
	va_start(ap, format);

	while (*s) {
		char c = *s++;
		if (c == '%') {
			c = *s++;
			/*
			 * Ignore 'l' length sub-specifier.
			 * This has no effect in ILP32 (4/4/4).
			 * In i586, 'int' and 'long int' are both 4 bytes long.
			 */
			if (c == 'l') {
				c = *s++;
			}
			switch (c) {
#if (PICO_PRINTF_D)
			case 'd': {
				int n;
				n = va_arg(ap, int);
				if (n < 0) {
					len += pico_putchar('-');
					n = -n;
				}
				len += putuint(n, 10, false);
				break;
			}
#endif
#if (PICO_PRINTF_U)
			case 'u': {
				unsigned int u;
				u = va_arg(ap, unsigned int);
				len += putuint(u, 10, false);
				break;
			}
#endif
#if (PICO_PRINTF_X)
			case 'X':
			case 'x': {
				unsigned int u;
				u = va_arg(ap, unsigned int);
				len += putuint(u, 16, c == 'X');
				break;
			}
#endif
#if (PICO_PRINTF_S)
			case 's': {
				const char *str;
				str = va_arg(ap, const char *);
				len += pico_putchars(str);
				break;
			}
#endif
#if (PICO_PRINTF_ESC)
			default:
				len += pico_putchar('%');
			case '%':
				len += pico_putchar(c);
				break;
#endif
			}
			continue;
		}

		len += pico_putchar(c);
	}

	va_end(ap);
	return len;
}

static qm_uart_config_t stdout_uart_cfg;
#define QM_AON_GPIO ((qm_gpio_reg_t *)QM_AON_GPIO_BASE)

void stdout_uart_setup(uint32_t baud_divisors)
{
/* Mux out TX pin */

#if (QUARK_SE)
#if (STDOUT_UART_0)
	qm_pmux_select(QM_PIN_ID_19, QM_PMUX_FN_0);
#else
	qm_pmux_select(QM_PIN_ID_16, QM_PMUX_FN_2);

#if (UART1_FTDI)
	/* Set AON_GPIO 3 to get UART1 routed to USB/FTDI */
	QM_AON_GPIO->gpio_swporta_dr |= BIT(3);
#else
	QM_AON_GPIO->gpio_swporta_dr &= ~BIT(3);
#endif /* UART1_FTDI */
	QM_AON_GPIO->gpio_swporta_ddr |= BIT(3);
#endif /* STDOUT_UART_0 */
#else
#if (STDOUT_UART_0)
	qm_pmux_select(QM_PIN_ID_12, QM_PMUX_FN_2);
#else
	qm_pmux_select(QM_PIN_ID_20, QM_PMUX_FN_2);
#endif /* STDOUT_UART_0 */
#endif /* QUARK_SE */

	stdout_uart_cfg.baud_divisor = baud_divisors;
	stdout_uart_cfg.line_control = QM_UART_LC_8N1;

#if (STDOUT_UART_0)
	clk_periph_enable(CLK_PERIPH_CLK | CLK_PERIPH_UARTA_REGISTER);
#else
	clk_periph_enable(CLK_PERIPH_CLK | CLK_PERIPH_UARTB_REGISTER);
#endif
	qm_uart_set_config(STDOUT_UART, &stdout_uart_cfg);
}

/*
 * Custom puts implementation providing a lightweight alternative to the
 * standard stream-compliant newlib implementation.
 * Transmit a pre-formatted string through UART and append a newline to it.
 */
int puts(const char *s)
{
	for (; *s; s++) {
		qm_uart_write(STDOUT_UART, *s);
	}

	qm_uart_write(STDOUT_UART, '\n');
	return 0;
}

/*
 * Custom lightweight implementation that avoids the need for
 * kill/abort/exit/getpid syscalls. It is also optimised for size (no string
 * formatting, targeting UART directly).
 */
void __assert_func(const char *file, int line, const char *func,
		   const char *failedexpr)
{
	(void)file;
	(void)line;

	qm_irq_disable();

#if (PUTS_ENABLE || PRINTF_ENABLE)
	/* No point in checking for TX outcome at this stage */
	qm_uart_write_buffer(STDOUT_UART, (uint8_t *)ASSERT_STR_HEAD,
			     sizeof(ASSERT_STR_HEAD));
	int i;
	for (i = 0; func[i]; i++) {
		qm_uart_write(STDOUT_UART, (uint8_t)func[i]);
	}
	qm_uart_write_buffer(STDOUT_UART, (uint8_t *)ASSERT_STR_TRAIL,
			     sizeof(ASSERT_STR_TRAIL));
	for (i = 0; failedexpr[i]; i++) {
		qm_uart_write(STDOUT_UART, (uint8_t)failedexpr[i]);
	}
#else
	(void)func;
	(void)failedexpr;
#endif /* PUTS_ENABLED || PRINTF_ENABLED */

	while (1) {
#if (ASSERT_ACTION_HALT)
#if (QM_SENSOR)
		__builtin_arc_brk();
#else
		__asm__ __volatile__("hlt");
#endif
#elif(ASSERT_ACTION_RESET)
		qm_soc_reset(QM_WARM_RESET);
#else
#error "Undefined assert action"
#endif /* ASSERT_ACTION_* */
	}
}

/* System call not supported */
int close(int file)
{
	(void)file;

	errno = ENOTSUP;
	return -1;
}

/* System call not supported */
int isatty(int file)
{
	(void)file;

	errno = ENOTTY;
	return 0;
}

/* System call not supported */
int read(int file, char *buf, int len)
{
	(void)file;
	(void)buf;
	(void)len;

	errno = ENOTSUP;
	return -1;
}

/*
 * Back-end of printf.  Directs output and error streams to UART.
 */
int write(int file, const char *buf, int len)
{
	int ret = -1;

	switch (file) {
	case 1:
	case 2:
		if (0 ==
		    qm_uart_write_buffer(STDOUT_UART, (uint8_t *)buf, len)) {
			ret = len;
		} else {
			errno = EIO;
		}
		break;

	default:
		errno = ENOTSUP;
		break;
	}

	return ret;
}

/* System call not supported */
int lseek(int file, int p, int dir)
{
	(void)file;
	(void)p;
	(void)dir;

	errno = ENOTSUP;
	return -1;
}

/* Returns file status.  stdout/stderr set to character devices (needed by the
 * printf family).  */
int fstat(int file, struct stat *st)
{
	if (file == 0 || file > 2) {
		errno = ENOTSUP;
		return -1;
	}

	/* The stream is a character device (needed by printf functions) */
	st->st_mode = S_IFCHR;
	return 0;
}

/*
 * Expands/shrinks the heap.  This is the back-end of malloc/free.
 */
caddr_t sbrk(int incr)
{
	extern char __heap, __heap_end;
	static char *prog_break = &__heap;

	char *prev_prog_break;

	if (prog_break + incr >= &__heap_end || prog_break + incr < &__heap) {
		errno = ENOMEM;
		return (void *)-1;
	}

	prev_prog_break = prog_break;

	prog_break += incr;

	return prev_prog_break;
}

/**
 * @}
 */
