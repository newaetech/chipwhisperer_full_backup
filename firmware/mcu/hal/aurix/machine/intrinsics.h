/*  intrinsics.h -- C interface for TriCore special machine instructions.

    Copyright (C) 1999-2014 HighTec EDV-Systeme GmbH.
    
    This file is part of GCC.

    GCC is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3, or (at your option)
    any later version.

    GCC is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    Under Section 7 of GPL version 3, you are granted additional
    permissions described in the GCC Runtime Library Exception, version
    3.1, as published by the Free Software Foundation.

    You should have received a copy of the GNU General Public License and
    a copy of the GCC Runtime Library Exception along with this program;
    see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
    <http://www.gnu.org/licenses/>.  */


#ifndef __INTRINSICS_H__
#define __INTRINSICS_H__

#undef  __STRINGIFY
#define __STRINGIFY(x)    #x

/* Some of the TriCore instructions require an immediate operand.
   If the compiler cannot prove that the respective operand is a compile
   time constant, we get diagnostics like

      warning: asm operand <n> probably doesn't match constraints
      error: impossible constraint in 'asm'

   Therefore, we supply these instructions in two flavors:

   __XXXX:  This is a macro that puts together the assembler template:
            stringifying the macro argument and then gluing it together
            to get the static constant string as required by __asm__.
            Some of the __XXXX macros are also supplied for backward
            compatibility with 3.x legacy code.

   _xxxx:   An inline function that allows to feed in 0L for example.
            In some cases and where the argument does not matter (or there
            is no argument at all) this might also be a macro.  Again, this
            is for compatibility.

   Neither of the two forms cover all use cases so that we need both.
   The __tric_xxxx macros are only used internally for macro double-expansion.

   Examples:
   =========
   
            int x;

            x = _mfcr (0);
            x = _mfcr (1L);
            x = __MFCR (0);
            x = __MFCR ($ICR);

            #define ADDR 0x123

            x = _mfcr (ADDR);
            x = __MFCR (ADDR);

            #define ADDR1 ADDR + 1

            x = _mfcr (ADDR1);
            x = __MFCR (ADDR1);
*/


/*********************************************************************
 * BISR
 **********************************************************************/

#define __bisr(irq_level) __tric_bisr(irq_level)
#define __BISR(irq_level) __tric_bisr(irq_level)

#define __tric_bisr(irq_level)                                          \
  __asm__ volatile ("bisr " __STRINGIFY (irq_level) ::: "memory")

static __inline__ __attribute__((__always_inline__))
void _bisr (const unsigned __irq_level)
{
  __asm__ volatile ("bisr %0" :: "i" (__irq_level) : "memory");
}


/*********************************************************************
 * MFCR
 **********************************************************************/

#define __MFCR(regaddr) __tric_mfcr (regaddr)
#define __mfcr(regaddr) __tric_mfcr (regaddr)

#define __tric_mfcr(regaddr)                                            \
  (__extension__({                                                      \
      unsigned __res;                                                   \
      __asm__ volatile ("mfcr %0, LO:" __STRINGIFY (regaddr)            \
                        : "=d" (__res) :: "memory");                    \
      __res;                                                            \
    }))

static __inline__ __attribute__((__always_inline__))
unsigned _mfcr (const unsigned __regaddr)
{
  unsigned __res;
  __asm__ volatile ("mfcr %0, LO:%1"
                    : "=d" (__res) : "i" (__regaddr) : "memory");
  return __res;
}


/*********************************************************************
 * MTCR
 **********************************************************************/

#define __MTCR(regaddr, val) __tric_mtcr (regaddr, val)
#define __mtcr(regaddr, val) __tric_mtcr (regaddr, val)

#define __tric_mtcr(regaddr, val)                                       \
  do {                                                                  \
    unsigned __newval = (unsigned) (val);                               \
    __asm__ volatile ("mtcr LO:" __STRINGIFY (regaddr) ", %0"           \
                      :: "d" (__newval) : "memory");                    \
  } while (0)

static __inline__ __attribute__((__always_inline__))
void _mtcr (const unsigned __regaddr, const unsigned __val)
{
  __asm__ volatile ("mtcr LO:%0, %1"
                    :: "i" (__regaddr), "d" (__val) : "memory");
}


/*********************************************************************
 * SYSCALL
 **********************************************************************/

#define __syscall(service) __tric_syscall (service)
#define __SYSCALL(service) __tric_syscall (service)

#define __tric_syscall(service)                                         \
  __asm__ volatile ("syscall "__STRINGIFY (service) ::: "memory")

static __inline__ __attribute__((__always_inline__))
void _syscall (const unsigned __service)
{
  __asm__ volatile ("syscall %0" :: "i" (__service) : "memory");
}


/*********************************************************************
 * Misc, without operands
 **********************************************************************/

static __inline__ __attribute__((__always_inline__))
void _disable (void)
{
  __asm__ volatile ("disable" ::: "memory");
}

static __inline__ __attribute__((__always_inline__))
void _enable (void)
{
  __asm__ volatile ("enable" ::: "memory");
}

static __inline__ __attribute__((__always_inline__))
void _debug (void)
{
  __asm__ volatile ("debug" ::: "memory");
}

static __inline__ __attribute__((__always_inline__))
void _isync (void)
{
  __asm__ volatile ("isync" ::: "memory");
}

static __inline__ __attribute__((__always_inline__))
void _dsync (void)
{
  __asm__ volatile ("dsync" ::: "memory");
}

static __inline__ __attribute__((__always_inline__))
void _rstv (void)
{
  __asm__ volatile ("rstv" ::: "memory");
}

static __inline__ __attribute__((__always_inline__))
void _rslcx (void)
{
    __asm__ volatile ("rslcx" ::: "memory", 
                      "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7",
                      "a2", "a3", "a4", "a5", "a6", "a7", "a11");
}


static __inline__ __attribute__((__always_inline__))
void _svlcx (void)
{
  __asm__ volatile ("svlcx" ::: "memory");
}

static __inline__ __attribute__((__always_inline__))
void _nop (void)
{
  __asm__ volatile ("nop" ::: "memory");
}

/*********************************************************************
 * More Fiddling with Interrupt Enable / Disable
 **********************************************************************/

/* Restore interrupt state.  Directly supported for TriCore 1.6.
   Emulated on TC1.3.  */

static __inline__ __attribute__((__always_inline__))
void _restore (const int irqs_on)
{
#if defined(__TC16__) || defined(__TC161__)
  __asm__ volatile ("restore %0" :: "d" (irqs_on) : "memory");
#else
  if (irqs_on)
    _enable();
  else
    _disable();
#endif
}

/*********************************************************************
 * Some compatibility defines with name mess of 3.x.
 * In 3.x, these names served to indicate wether or not a specific
 * built-in is available, but users started to use the marker macros
 * as function calls...
 * FIXME: We should clean this up.
 **********************************************************************/

#define __CLZ(val)      __builtin_clz (val)

#define __CTZ(val)      __builtin_ctz (val)

#define __ABS(val)      __builtin_abs (val)

#endif /* __INTRINSICS_H__ */
