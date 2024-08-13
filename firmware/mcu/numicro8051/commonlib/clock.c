#include "numicro_8051.h"
#include "Common.h"
#include "pin_defines.h"

void get_hircmap_16mhz_vals(uint8_t *hircmap)
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
    hircmap[0] = IAPFD;
    IAPAL = 0x31;
    set_IAPTRG_IAPGO;
    hircmap[1] = IAPFD;
    if (!IAPEN_val)
    {
        clr_CHPCON_IAPEN;
    }
}

void set_hircmap(uint8_t *hircmap){
    // doing this to ensure that these are near accesses
    // it screws up the timing of writing to TA-protected registers if we have to get a far pointer
    uint8_t hircmap0, hircmap1;
    hircmap0 = hircmap[0];
    hircmap1 = hircmap[1];
    BIT_TMP = EA;
    EA = 0;
    TA = 0XAA;
    TA = 0X55;
    RCTRIM0 = hircmap0;
    TA = 0XAA;
    TA = 0X55;
    RCTRIM1 = hircmap1;
    EA = BIT_TMP;
}

#ifdef NUM51_CPU24MHZ_SUPPORTED
void get_hircmap_24mhz_vals(uint8_t *hircmap)
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
    hircmap[0] = IAPFD;
    IAPAL = 0x39;
    set_IAPTRG_IAPGO;
    hircmap[1] = IAPFD;
    if (!IAPEN_val)
    {
        clr_CHPCON_IAPEN;
    }
}

void MODIFY_HIRC_24(void) // Modify HIRC to 24MHz
{
    UINT8 hircmap[2];
    get_hircmap_24mhz_vals(hircmap);
    set_hircmap(hircmap);
    /* Clear power on flag */
    PCON &= CLR_BIT4;
}
#endif
void MODIFY_HIRC_166(void) // Modify HIRC to 16.6MHz, more detail please see datasheet V1.02
{
    UINT8 hircmap[2];
    UINT16 trimvalue16bit;
    get_hircmap_16mhz_vals(hircmap);
    trimvalue16bit = ((hircmap[0] << 1) + (hircmap[1] & 0x01));
    trimvalue16bit = trimvalue16bit - 14;
    hircmap[1] = trimvalue16bit & 0x01;
    hircmap[0] = trimvalue16bit >> 1;
    set_hircmap(hircmap);
    /* Clear power on flag */
    PCON &= CLR_BIT4;
}

void MODIFY_HIRC_16(void)
{
    UINT8 hircmap[2];
    get_hircmap_16mhz_vals(hircmap);
    set_hircmap(hircmap);
}

#define TRIM_VALUE_UNIT 40000 // about 40kHz

// Use this with caution! The internal oscillator(s) are not guaranteed to support arbitrary frequencies
// It doesn't seem like it will support much else other than 16, 16.6, and 24MHz (if the CPU supports 24MHz)
void MODIFY_HIRC_OTHER(void)
{
    UINT8 hircmap[2];
    UINT16 trimvalue16bit;
#if FCPU > 20000000
    get_hircmap_24mhz_vals(hircmap);
#define trimvalue16bit_adj DIV_ROUND_CLOSEST((24000000 - F_CPU), TRIM_VALUE_UNIT)
#else
    get_hircmap_16mhz_vals(hircmap);
#define trimvalue16bit_adj DIV_ROUND_CLOSEST((16000000 - F_CPU), TRIM_VALUE_UNIT)
#endif
    trimvalue16bit = ((hircmap[0] << 1) + (hircmap[1] & 0x01));
    trimvalue16bit = trimvalue16bit - trimvalue16bit_adj;
    hircmap[1] = trimvalue16bit & 0x01;
    hircmap[0] = trimvalue16bit >> 1;
    set_hircmap(hircmap);
}

void enable_output_clock()
{
    PIN_CLK_OUT_PUSHPULL_MODE;
    set_CKCON_CLOEN;         // Enable clock out pin
}

void disable_output_clock()
{
    PIN_CLK_OUT_QUASI_MODE;
    clr_CKCON_CLOEN;
}

void use_external_clock(void)
{
    set_CKEN_EXTEN1;
    set_CKEN_EXTEN0;
    while(!(CKSWT&SET_BIT3));
    clr_CKSWT_OSC1; // step3: switching system clock source if needed
    set_CKSWT_OSC0;
    clr_CKEN_HIRCEN;
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
    uint8_t hircmap[2];
#if (F_CPU == 16000000)
    MODIFY_HIRC_16();
#elif (F_CPU == 16600000)
    MODIFY_HIRC_166();
#elif (F_CPU == 24000000)
#ifndef NUM51_CPU24MHZ_SUPPORTED
    #error "This CPU does not support 24MHz!"
#endif
    MODIFY_HIRC_24();
#else
    MODIFY_HIRC_OTHER();
#endif
}
