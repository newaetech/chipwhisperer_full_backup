/*
  specific_instructions.h - RX specific functions
  Copyright (c) 2014 Nozomu Fujita.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
/*
  Modified 12 July 2018 by Yuuki Okamiya for GR-ROSE
*/

#ifndef _SPECIFIC_INSTRUCTIONS_H_
#define _SPECIFIC_INSTRUCTIONS_H_

#if 0
#define sei() \
do { \
  __asm __volatile("setpsw i\n"); \
} while (0)

#define cli() \
do { \
  __asm __volatile("clrpsw i\n"); \
} while (0)


#define isNoInterrupts() \
({ \
  bool ret; \
  __asm __volatile( \
    "mvfc psw, %0\n" \
    "btst #16, %0\n" \
    "sceq.l %0\n" \
    : "=r" (ret) \
    : \
    : \
  ); \
  ret; \
})

#else
#define sei() __builtin_rx_setpsw('I')
#define cli() __builtin_rx_clrpsw('I')
#define isNoInterrupts() (!(__builtin_rx_mvfc(0x0) && 0x00010000))
#endif

#define pushi() \
{ \
  bool _di = isNoInterrupts();

#define popi() \
  if (_di) { \
    cli(); \
  } else { \
    sei(); \
  } \
}

#if 0
#define BSET(port, bit) \
do { \
  volatile byte* _port = (port); \
  int _bit = (bit); \
  __asm __volatile( \
    "bset %1, [%0].b\n" \
    : \
    : "r" (_port), "r" (_bit) \
    : \
  ); \
} while (0)

#define BCLR(port, bit) \
do { \
  volatile byte* _port = (port); \
  int _bit = (bit); \
  __asm __volatile( \
    "bclr %1, [%0].b\n" \
    : \
    : "r" (_port), "r" (_bit) \
    : \
  ); \
} while (0)

#define BTST(port, bit) \
({ \
  volatile byte* _port = (port); \
  int _bit = (bit); \
  int ret; \
  __asm __volatile( \
    "btst %2, [%1].b\n" \
    "scnz.l %0\n" \
    : "=r" (ret) \
    : "r" (_port), "r" (_bit) \
    : \
  ); \
  ret; \
})
#else
#define BSET(port, bit) \
do { \
	*port = *port | (1 << bit); \
} while (0)

#define BCLR(port, bit) \
do { \
	*port = *port & ~(1 << bit); \
} while (0)
#endif

#define sbi(port, bit) BSET((port), (bit))
#define cbi(port, bit) BCLR((port), (bit))

#endif/*_SPECIFIC_INSTRUCTIONS_H_*/
