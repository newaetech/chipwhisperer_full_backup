
/* for Keil */
#if defined __C51__
#include "n76s003_keil.h"

/* for IAR */
#elif defined __ICC8051__
#include "n76s003_iar.h"

/* for SDCC */
#elif defined __SDCC__
#include "n76s003_sdcc.h"

#endif
