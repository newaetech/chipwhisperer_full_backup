/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2012-2020 NewAE Technology Inc.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "aes-independant.h"
#include "hal.h"
#include "simpleserial.h"

#if HAL_TYPE == HAL_k82f
#include "MK82F25615.h"
#include "core_cm4.h"
#endif

#if HAL_TYPE == HAL_stm32f3
#include "stm32f303x8.h"
#include "core_cm4.h"
#endif

#if HAL_TYPE == HAL_sam4s
#include "asf.h"
#include "core_cm4.h"
#endif

#include "arm_etm.h"
#include <stdint.h>
#include <stdlib.h>

uint8_t pcsamp_enable;
static uint16_t num_encryption_rounds = 10;

uint8_t setreg(uint8_t* x, uint8_t len)
{
        uint32_t val;
        val = x[4] + (x[3] << 8) + (x[2] << 16) + (x[1] << 24);
// Must match capture/trace/TraceWhisperer.py:
//0:  DWT->CTRL
//1:  DWT->COMP0
//2:  DWT->COMP1
//3:  ETM->CR
//4:  ETM->TESSEICR
//5:  ETM->TEEVR
//6:  ETM->TECR1
//7:  ETM->TRACEIDR
//8:  TPI->ACPR
//9:  TPI->SPPR
//10: TPI->FFCR
//11: TPI->CSPSR
//12: ITM->TCR
        if       (x[0] == 0)    {DWT->CTRL = val;}
        else if  (x[0] == 1)    {DWT->COMP0 = val;}
        else if  (x[0] == 2)    {DWT->COMP1 = val;}
        else if  (x[0] == 3)    {ETM_SetupMode(); ETM->CR = val; ETM_TraceMode();}
        else if  (x[0] == 4)    {ETM_SetupMode(); ETM->TESSEICR = val; ETM_TraceMode();}
        else if  (x[0] == 5)    {ETM_SetupMode(); ETM->TEEVR    = val; ETM_TraceMode();}
        else if  (x[0] == 6)    {ETM_SetupMode(); ETM->TECR1    = val; ETM_TraceMode();}
        else if  (x[0] == 7)    {ETM_SetupMode(); ETM->TRACEIDR = val; ETM_TraceMode();}
        else if  (x[0] == 8)    {TPI->ACPR    = val;}
        else if  (x[0] == 9)    {TPI->SPPR    = val;}
        else if  (x[0] == 10)   {TPI->FFCR    = val;}
        else if  (x[0] == 11)   {TPI->CSPSR   = val;}
        else if  (x[0] == 12)   {ITM->TCR     = val;}

	return 0x00;
}


uint8_t getreg(uint8_t* x, uint8_t len)
{
        uint32_t val;
        if       (x[0] == 0)    {val = DWT->CTRL;}
        else if  (x[0] == 1)    {val = DWT->COMP0;}
        else if  (x[0] == 2)    {val = DWT->COMP1 ;}
        else if  (x[0] == 3)    {val = ETM->CR;}
        else if  (x[0] == 4)    {val = ETM->TESSEICR;}
        else if  (x[0] == 5)    {val = ETM->TEEVR;}
        else if  (x[0] == 6)    {val = ETM->TECR1;}
        else if  (x[0] == 7)    {val = ETM->TRACEIDR;}
        else if  (x[0] == 8)    {val = TPI->ACPR;}
        else if  (x[0] == 9)    {val = TPI->SPPR;}
        else if  (x[0] == 10)   {val = TPI->FFCR;}
        else if  (x[0] == 11)   {val = TPI->CSPSR;}
        else if  (x[0] == 12)   {val = ITM->TCR;}
        else {val = 0;}

        x[3] = val & 0xff;
        x[2] = (val >> 8) & 0xff;
        x[1] = (val >> 16) & 0xff;
        x[0] = (val >> 24) & 0xff;
	simpleserial_put('r', 4, x);
	return 0x00;
}

void enable_trace(void)
{
    // Enable SWO pin (not required on K82)
    #if (HAL_TYPE == HAL_stm32f3)
       DBGMCU->CR |= DBGMCU_CR_TRACE_IOEN_Msk;
    #endif

    // Configure TPI
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Enable access to registers
    TPI->ACPR = 0; // SWO trace baud rate = cpu clock / (ACPR+1)

    #if (HAL_TYPE == HAL_stm32f3) || (HAL_TYPE == HAL_sam4s)
       TPI->SPPR = 2; // default to SWO with NRZ encoding
       //TPI->SPPR = 1; // SWO with Manchester encoding
    #else
       TPI->SPPR = 0; // default to parallel trace mode
    #endif

    TPI->FFCR = 0x102; // packet framing enabled
    //TPI->FFCR = 0x100; // no framing: for DWT/ITM only, no ETM
    TPI->CSPSR =0x00000008; // 4 trace lanes

    // Configure ITM:
    ITM->LAR = 0xC5ACCE55;
    ITM->TCR = (1 << ITM_TCR_TraceBusID_Pos) // Trace bus ID for TPIU
             | (1 << ITM_TCR_DWTENA_Pos) // Enable events from DWT
             | (0 << ITM_TCR_SYNCENA_Pos) // Disable sync packets!
             | (1 << ITM_TCR_ITMENA_Pos); // Main enable for ITM
    ITM->TER = 0xFFFFFFFF; // Enable all stimulus ports
    ITM->TPR = 0x00000000; // allow unpriviledged access

    // Configure DWT:
    DWT->CTRL = (0xf << DWT_CTRL_POSTINIT_Pos);// countdown counter for PC sampling, must be written
                                               // before enabling PC sampling
    DWT->CTRL |=(1 << DWT_CTRL_CYCTAP_Pos)     // Prescaler for PC sampling: 0 = x32, 1 = x512
              | (8 << DWT_CTRL_POSTPRESET_Pos) // PC sampling postscaler
              | (0 << DWT_CTRL_PCSAMPLENA_Pos) // disable PC sampling
              | (1 << DWT_CTRL_SYNCTAP_Pos)    // sync packets every 16M cycles
              | (0 << DWT_CTRL_EXCTRCENA_Pos)  // disable exception trace
              | (1 << DWT_CTRL_CYCCNTENA_Pos); // enable cycle counter

    // Configure DWT PC comparator 0:
    DWT->COMP0 = 0x00001d60; // AES subbytes
    DWT->MASK0 = 0;
    DWT->FUNCTION0 = (0 << DWT_FUNCTION_DATAVMATCH_Pos) // address match
                   | (0 << DWT_FUNCTION_CYCMATCH_Pos)
                   | (0 << DWT_FUNCTION_EMITRANGE_Pos)
                   | (8 << DWT_FUNCTION_FUNCTION_Pos); // Iaddr CMPMATCH event

    // Configure DWT PC comparator 1:
    DWT->COMP1 = 0x00001d68; // AES mixcolumns
    DWT->MASK1 = 0;
    DWT->FUNCTION1 = (0 << DWT_FUNCTION_DATAVMATCH_Pos) // address match
                   | (0 << DWT_FUNCTION_CYCMATCH_Pos)
                   | (0 << DWT_FUNCTION_EMITRANGE_Pos)
                   | (8 << DWT_FUNCTION_FUNCTION_Pos); // Iaddr CMPMATCH event


    // Configure ETM:
    ETM->LAR = 0xC5ACCE55;
    ETM_SetupMode();
    ETM->CR = ETM_CR_ETMEN; // Enable ETM output port
    ETM->TRACEIDR = 1; // Trace bus ID for TPIU
    ETM->FFLR = 0; // Stall processor when FIFO is full
    ETM->TEEVR = 0x000150a0;    // EmbeddedICE comparator 0 or 1 (DWT->COMP0 or DWT->COMP1)
    //ETM->TEEVR = 0x00000020;    // EmbeddedICE comparator 0 only
    //ETM->TEEVR = 0x00000021;    // EmbeddedICE comparator 1 only
    ETM->TESSEICR = 0xf; // set EmbeddedICE watchpoint 0 as a TraceEnable start resource.
    ETM->TECR1 = 0; // tracing is unaffected by the trace start/stop logic
    ETM_TraceMode();
}

void print(const char *ptr)
{
    while (*ptr != (char)0)
    {
        putch(*ptr);
        ptr++;
    }
}


// Print a given string to ITM with 8-bit writes.
void ITM_Print(int port, const char *p)
{
    if ((ITM->TCR & ITM_TCR_ITMENA_Msk) && (ITM->TER & (1UL << port)))
    {
        while (*p)
        {
            while (ITM->PORT[port].u32 == 0);
            ITM->PORT[port].u8 = *p++;
        }
        print("ITM alive!\n");
    }
    else {print("Couldn't print!\n");}
}


uint8_t test_itm(uint8_t* x, uint8_t len)
{
    ITM_Print(x[0], "ITM alive!\n");
    return 0x00;
}


uint8_t set_pcsample_params(uint8_t* x, uint8_t len)
{
    uint8_t postinit;
    uint8_t postreset;
    uint8_t cyctap;
    pcsamp_enable = x[0] & 1;
    cyctap = x[1] & 1;
    postinit  = x[2] & 0xf;
    postreset = x[3] & 0xf;

    // must clear everything before updating postinit field:
    DWT->CTRL = 0;
    // then update postinit:
    DWT->CTRL = (postinit << DWT_CTRL_POSTINIT_Pos);
    // then update the reset, but don't turn on PC sampling yet;
    // that will be handled in trigger_high_pcsamp
    DWT->CTRL = (cyctap << DWT_CTRL_CYCTAP_Pos)
              | (postreset << DWT_CTRL_POSTPRESET_Pos)
              | (postinit << DWT_CTRL_POSTINIT_Pos)
              | (1 << DWT_CTRL_SYNCTAP_Pos)
              | (1 << DWT_CTRL_CYCCNTENA_Pos);
    simpleserial_put('r', 4, x);
    return 0x00;
}


// in order for PC sample packets to be easily parsed, PC sampling must
// begin *after* we start capturing trace data
void trigger_high_pcsamp(void)
{
    if (pcsamp_enable == 1)
    {
        DWT->CTRL |= (1 << DWT_CTRL_PCSAMPLENA_Pos); // enable PC sampling
    }
    trigger_high();
}


void trigger_low_pcsamp(void)
{
    trigger_low();
    DWT->CTRL &= ~(1 << DWT_CTRL_PCSAMPLENA_Pos); // disable PC sampling
}


uint8_t get_mask(uint8_t* m, uint8_t len)
{
  aes_indep_mask(m, len);
  return 0x00;
}


uint8_t get_key(uint8_t* k, uint8_t len)
{
    aes_indep_key(k);
    return 0x00;
}


uint8_t get_pt(uint8_t* pt, uint8_t len)
{
    aes_indep_enc_pretrigger(pt);

    trigger_high_pcsamp();

    #ifdef ADD_JITTER
    for (volatile uint8_t k = 0; k < (*pt & 0x0F); k++);
    #endif

    aes_indep_enc(pt); /* encrypting the data block */
    trigger_low_pcsamp();

    aes_indep_enc_posttrigger(pt);

    simpleserial_put('r', 16, pt);
    return 0x00;
}

uint8_t enc_multi_getpt(uint8_t* pt, uint8_t len)
{
    aes_indep_enc_pretrigger(pt);

    for(unsigned int i = 0; i < num_encryption_rounds; i++){
        trigger_high_pcsamp();
        aes_indep_enc(pt);
        trigger_low_pcsamp();
    }

    aes_indep_enc_posttrigger(pt);
	simpleserial_put('r', 16, pt);
    return 0;
}

uint8_t enc_multi_setnum(uint8_t* t, uint8_t len)
{
    //Assumes user entered a number like [0, 200] to mean "200"
    //which is most sane looking for humans I think
    num_encryption_rounds = t[1];
    num_encryption_rounds |= t[0] << 8;
    return 0;
}


uint8_t info(uint8_t* x, uint8_t len)
{
        print("ChipWhisperer simpleserial-trace, compiled ");
        print(__DATE__);
        print(", ");
        print(__TIME__);
        print("\n");
	return 0x00;
}


uint8_t reenable_trace(uint8_t* x, uint8_t len)
{
        enable_trace();
	return 0x00;
}


uint8_t reset(uint8_t* x, uint8_t len)
{
    // Reset key here if needed
    return 0x00;
}

int main(void)
{
    uint8_t tmp[KEY_LENGTH] = {DEFAULT_KEY};

    platform_init();
    init_uart();
    trigger_setup();

    aes_indep_init();
    aes_indep_key(tmp);

    simpleserial_init();
    simpleserial_addcmd('k', 16, get_key);
    simpleserial_addcmd('p', 16, get_pt);
    simpleserial_addcmd('x', 0, reset);
    simpleserial_addcmd_flags('m', 18, get_mask, CMD_FLAG_LEN);
    simpleserial_addcmd('i', 0, info);
    simpleserial_addcmd('e', 0, reenable_trace);
    simpleserial_addcmd('t', 1, test_itm);
    simpleserial_addcmd('s', 5, setreg);
    simpleserial_addcmd('g', 5, getreg);
    simpleserial_addcmd('c', 4, set_pcsample_params);
    simpleserial_addcmd('n', 2, enc_multi_setnum);
    simpleserial_addcmd('f', 16, enc_multi_getpt);

    enable_trace();

    while(1)
        simpleserial_get();
}

