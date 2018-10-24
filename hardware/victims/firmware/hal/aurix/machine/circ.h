/*
  EABI 2.1.3.1: Circular Buffer Pointers.

  circ.h -- C interface to use the circular buffer addressing on the TriCore

  This module define access Macros for all scalar data types up to 8 byte
  using circular addressing mode.
*/

#ifndef __circ_h__
#define __circ_h__

/* The following typedef defines the circular buffer type.
   __circ and __circ64 are tricore-gcc built-in type with the
   following layout:

   union
   {
       struct
       {
           void *buf
           unsigned short index; 
           unsigned short length; 
       };
       __circ64 circ64;
   };

   __circ64 is a 64-bit integer.

   The __circ and __circ64 types will be passed in address registers
   if applicable.
*/

typedef __circ circ_t;

#define opPd(type, fname, tg, tp, s,A )         \
  __opPd (type, fname, tg, tp, s, A)

/*
   The following macro defines the get and put functions using the circular
   buffer addressing mode according to the defined type.
   The parameters are:

   type	 the scalar access type
   tg	 tricore data type for get
   tp	 tricore data type for put
   s	 the size of the data type in bytes
   A	 use doubel register names inside the functions

   opPd(long,w,w,4,) defines the following funcitons for the long data type

   circ_t get_circ_long (circ_t aa, long *ptrlong);

   get_circ_...  reads the element from the actual index out of the buffer
   and stores the value at ptrlong.  The modified circular buffer address aa
   will be returned.

   circ_t get_circ_long_incr (circ_t aa, long *ptrlong,const int incr);

   get_circ_...  reads the element from the actual index out of the buffer
   and stores the value at ptrlong.  The circular buffer will be modified by
   incr * sizeof(type) bytes and returned.  incr must be a constant.

   circ_t put_circ_long (circ_t aa, long value);

   put_circ_... writes the value at the actual index into the buffer and returns
   the modified circular buffer address aa;

   circ_t put_circ_long_incr (circ_t aa, long value, const int incr);

   put_circ_... writes the value at the actual index into the buffer.
   The circular buffer will be modified by incr*sizeof(type) bytes and
   returned. incr must be a constant.

   circ_t init_circ_long (circ_t aa, long *buf,
                          unsigned short length, unsigned short index);

   init_circ_... initialize the circular buffer address with the base address
   BUF, and the length LENGTH.  The index will be initialized with 0.

   All defined function get as there first argument the circular buffer
   address and return the modified circular buffer address.
*/

#define __opPd(type, fname, tg, tp, s, A)                               \
    static __inline__ __attribute__((__always_inline__)) circ_t         \
    get_circ_##fname (circ_t aa, type *pret)                            \
    {                                                                   \
        __asm__ volatile ("ld."#tg"\t%"#A"0,[%1+c]"#s""                 \
                          : "=d" (*pret), "+a" (aa));                   \
        return aa;                                                      \
    }                                                                   \
                                                                        \
    static __inline__ __attribute__((__always_inline__)) circ_t         \
    get_circ_##fname##_incr (circ_t aa, type *pret, const int incr)     \
    {                                                                   \
        __asm__ volatile ("ld."#tg"\t%"#A"0,[%1+c](%2*"#s")"            \
                          : "=d" (*pret), "+a" (aa)                     \
                          : "n" (incr));                                \
        return aa;                                                      \
    }                                                                   \
                                                                        \
    static __inline__ __attribute__((__always_inline__)) circ_t         \
    put_circ_##fname (circ_t aa, type value)                            \
    {                                                                   \
        __asm__ volatile ("st."#tp"\t[%0+c]"#s", %"#A"1"                \
                          : "+a" (aa)                                   \
                          : "d" (value)                                 \
                          : "memory");                                  \
        return aa;                                                      \
    }                                                                   \
                                                                        \
    static __inline__ __attribute__((__always_inline__)) circ_t         \
    put_circ_##fname##_incr (circ_t aa, type value, const int incr)     \
    {                                                                   \
        __asm__ volatile ("st."#tg"\t[%0+c](%2*"#s"), %"#A"1"           \
                          : "+a" (aa)                                   \
                          : "d" (value), "n" (incr)                     \
                          : "memory");                                  \
        return aa;                                                      \
    }                                                                   \
                                                                        \
    static __inline__  __attribute__((__always_inline__))circ_t         \
    init_circ_##fname (circ_t aa, type *buf,                            \
                       unsigned short len, unsigned short idx)          \
    {                                                                   \
      aa.buf = (void*) buf;                                             \
      aa.index = idx;                                                   \
      aa.length = len;                                                  \
      return aa;                                                        \
    }

opPd (long, long, w, w, 4, /**/)
opPd (int, int, w, w, 4, /**/)
opPd (short, short, h, h, 2, /**/)
opPd (char, char, b, b, 1, /**/)
opPd (unsigned long, ulong, w, w, 4, /**/)
opPd (unsigned int, uint, w, w, 4, /**/)
opPd (unsigned short, ushort, hu, h, 2, /**/)
opPd (unsigned char, uchar, bu, b, 1, /**/)
opPd (long long, llong, d, d, 8, A)
#if __SIZEOF_DOUBLE__ == 4
opPd (double, double, w, w, 4, /**/)
#else
opPd (double, double, d, d, 8, A)
#endif /* sizeof (double) */
opPd (float, float, w, w, 4, /**/)

#undef opPd
#undef __opPd
#endif /* __circ_h__ */
