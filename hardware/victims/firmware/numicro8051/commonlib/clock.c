#include "numicro_8051.h"
#include "Common.h"

void get_hircmap_16mhz_vals(uint8_t *hircmap0, uint8_t *hircmap1)
{
    __bit IAPEN_val = CHPCON & SET_BIT0;
    if (!IAPEN_val)
    {
        set_CHPCON_IAPEN;
    }
    IAPAH = 0x00;
    IAPAL = 0x30;
    IAPCN = READ_UID;
    set_IAPTRG_IAPGO;
    *hircmap0 = IAPFD;
    IAPAL = 0x31;
    set_IAPTRG_IAPGO;
    *hircmap1 = IAPFD;
    if (!IAPEN_val)
    {
        clr_CHPCON_IAPEN;
    }
}
#ifdef NUM51_CPU24MHZ_SUPPORTED
void get_hircmap_24mhz_vals(uint8_t *hircmap0, uint8_t *hircmap1)
{
    __bit IAPEN_val = CHPCON & SET_BIT0;
    if (!IAPEN_val)
    {
        set_CHPCON_IAPEN;
    }
    IAPAH = 0x00;
    IAPAL = 0x38;
    IAPCN = READ_UID;
    set_IAPTRG_IAPGO;
    *hircmap0 = IAPFD;
    IAPAL = 0x39;
    set_IAPTRG_IAPGO;
    *hircmap1 = IAPFD;
    if (!IAPEN_val)
    {
        clr_CHPCON_IAPEN;
    }
}

void MODIFY_HIRC_24(void) // Modify HIRC to 24MHz, this isn't in the datasheet!
{
    UINT8 hircmap0, hircmap1;
    get_hircmap_24mhz_vals(&hircmap0, &hircmap1);
    TA = 0XAA;
    TA = 0X55;
    RCTRIM0 = hircmap0;
    TA = 0XAA;
    TA = 0X55;
    RCTRIM1 = hircmap1;
}
#endif
void MODIFY_HIRC_166(void) // Modify HIRC to 16.6MHz, more detail please see datasheet V1.02
{
    UINT8 hircmap0, hircmap1;
    UINT16 trimvalue16bit;
    get_hircmap_16mhz_vals(&hircmap0, &hircmap1);
    trimvalue16bit = ((hircmap0 << 1) + (hircmap1 & 0x01));
    trimvalue16bit = trimvalue16bit - 14;
    hircmap1 = trimvalue16bit & 0x01;
    hircmap0 = trimvalue16bit >> 1;
    TA = 0XAA;
    TA = 0X55;
    RCTRIM0 = hircmap0;
    TA = 0XAA;
    TA = 0X55;
    RCTRIM1 = hircmap1;
    /* Clear power on flag */
    PCON &= CLR_BIT4;
}

void MODIFY_HIRC_16(void)
{
    UINT8 hircmap0, hircmap1;
    get_hircmap_16mhz_vals(&hircmap0, &hircmap1);
    TA = 0XAA;
    TA = 0X55;
    RCTRIM0 = hircmap0;
    TA = 0XAA;
    TA = 0X55;
    RCTRIM1 = hircmap1;
}

void enable_output_clock()
{
    P11_PUSHPULL_MODE; // Set P1.1 to push-pull mode
    set_CKCON_CLOEN;         // Enable clock out pin
}

void disable_output_clock()
{
    P11_QUASI_MODE;
    clr_CKCON_CLOEN;
}

void use_external_clock(void)
{
    set_CKEN_EXTEN1;
    set_CKEN_EXTEN0;
    clr_CKSWT_OSC1; // step3: switching system clock source if needed
    set_CKSWT_OSC0;
    clr_CKEN_HIRCEN;
    // TODO: Make sure changing this from set_CT_T0 doesn't break anything
    ENABLE_TIMER0_MODE0; // Timer0 Clock source = OSCIN (external clock)
}

void use_internal_clock(void)
{
    set_CKEN_HIRCEN; // step1: enable HIRC clock source run
    while ((CKSWT & SET_BIT5) == 0)
        ;     // step2: check ready
    clr_CKSWT_OSC1; // step3: switching system clock source if needed
    clr_CKSWT_OSC0;
    while ((CKEN & SET_BIT0) == 1)
        ; // step4: check system clock switching OK or NG
#ifdef FOSC_166000
    MODIFY_HIRC_166();
#elif (defined(FOSC_240000))
#ifndef NUM51_CPU24MHZ_SUPPORTED
    #error "FOSC_240000 is not supported on this CPU"
#endif
    MODIFY_HIRC_24();
#else
    MODIFY_HIRC_16();
#endif
}
