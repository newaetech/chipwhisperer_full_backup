
/* for Keil */
#if defined __C51__
#include "n76e003_keil.h"

/* for IAR */
#elif defined __ICC8051__
#include "n76e003_iar.h"

/* for SDCC */
#elif defined __SDCC__
#include "n76e003_sdcc.h"

#endif
