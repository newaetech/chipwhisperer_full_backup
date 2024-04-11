#include "platform.h"
extern void _INITSCT(void);
//extern void R_SET_USP(void *); //init u stack pointer
//extern void R_SET_INTB(void *); //init interrupt table register
//extern void R_SET_EXTB(void *); //init exception table register

extern void bsp_ram_initialize(void);
extern int main(void);

//do floating point init

//set operating frequency

// init c runtime

// bsp_ram_initialize

// bsp_interrupt_open

// bsp_register_protect_open

// hardware_setup()

// R_SET_PSW() //enable interrupt and select I or U stack



//void startup(void)
void PowerON_Reset_PC_Prg(void)
{

    R_SET_USP(R_SECEND_USTACK);

    R_SET_INTB(R_SECTOP_INTVECTTBL);

    R_SET_EXTB(R_SECTOP_EXCEPTVECTTBL);
    operating_frequency_set();
    _INITSCT();

    //R_SET_FPSW(FPSW_init | FPU_ROUND | FPU_DENOM);

    //bsp_interrupt_open();
    //bsp_register_protect_open();
    hardware_setup();
    //R_SET_PSW(PSW_init);
    //R_BSP_Change_PSW_PM_to_UserMode(); //maybe???

    int rv = main();

    for (;;) {

    }

}