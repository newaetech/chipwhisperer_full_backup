/*
 * cint.h -- C interface for TriCore trap and interrupt handlers.
 *
 * Copyright (C) 1998 HighTec EDV-Systeme GmbH.
 *
 */

#ifndef __cint_h
#define __cint_h

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_TRAPS 8
#define MAX_INTRS 256

/* The following two functions install the vector tables and initialize
   chained interrupt handlers, respectively.  There is usually no need
   to call these functions, as they are declared as "constructors" so
   that they will be automatically called by __main (), which in turn
   is automatically called as part of the function prologue of main ().  */

extern void _init_vectab (void);
extern void _init_hnd_chain (void);

/* Install an interrupt handler for interrupt number intno.  If this
   interrupt occurs, the handler will be called with the given argument.
   A non-zero return value indicates success, zero indicates an error
   occurred and the handler couldn't be installed successfully.  */

extern int _install_int_handler (int intno, void (*handler) (int), int arg);

/* Install a chained handler for interrupt number intno.  If this
   interrupt occurs, all handlers registered for it will be called
   with their given argument.  The return value for the function below
   is a handle that can be used to remove the handler at a later time.
   A return value of NULL indicates an error.  */

extern void *_install_chained_int_handler (int intno, void (*handler) (int),
					   int arg);

/* Remove a chained handler for interrupt number intno.  Ptr is the
   handle returned by _install_chained_int_handler.  A return value
   of zero indicates success, non-zero indicates an error occurred.  */

extern int _remove_chained_int_handler (int intno, void *ptr);

/* Install a trap handler for trap number trapno.  If this trap occurs,
   the handler is called with the TIN (trap identification number) as
   its only argument.  */

extern int _install_trap_handler (int trapno, void (*handler) (int));

/* Definitions for compatibility with previous versions of this interface.  */

#define TrapInit _init_vectab
#define cintsetup _init_vectab
#define cinthandler _install_int_handler
#define ccintsetup _init_hnd_chain
#define ccinthandler _install_chained_int_handler
#define freechain_hnd _remove_chained_int_handler
#define ctraphandler _install_trap_handler

#ifdef __cplusplus
}
#endif

#endif /* __cint_h */
