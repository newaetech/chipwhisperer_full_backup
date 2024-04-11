
#include "./MPC5676R_hwinit.h"
#include "./MPC5676R_intrinit.h"
#include "../device/MPC5676R.h"
#include "../device/MPC5676R_features.h"
//#undef TURN_ON_CPU1




void hw_init(void)
{
#if defined(TURN_ON_CPU1)
    /* Start the second core */
#if defined(START_FROM_FLASH)
    SIU->RSTVEC1 = (unsigned long)0x300800 | 0x00000001;
#else
    SIU->RSTVEC1 = (unsigned long)0x40030000 | 0x00000001;
#endif /* defined(START_FROM_FLASH) */     
    
#endif
    xcptn_xmpl();
}
