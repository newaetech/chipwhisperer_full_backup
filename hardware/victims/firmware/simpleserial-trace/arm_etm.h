/* 2015 Petteri Aimonen <jpa@git.mail.kapsi.fi>
 * Public domain. */
 
#ifndef ARM_ETM_H
#define ARM_ETM_H

//#include "core_cm3.h"
#include "core_cm4.h"

/* ETM Peripheral Register definitions.
 * See here for register documentation:
 * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ihi0014q/Chdfiagc.html
 *
 * Not all features are supported on Cortex-M3, see here for details:
 * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0337i/CHDBGEED.html
 *
 * The ETM peripheral has a lot of registers, but these are the main ones:
 * - LAR:       Allow write access to other ETM registers
 * - CR:        Enable/disable tracing
 * - TRIGGER:   Select tracing trigger event
 * - SR:        Current status
 * - TECR1      Select areas of code where to enable trace
 * - TECR2      Select comparator for trace enable
 * - TEEVR      Select event for trace enable
 */
typedef struct
{
    __IO uint32_t CR;           /* Main Control Register */
    __IO uint32_t CCR;          /* Configuration Code Register */
    __IO uint32_t TRIGGER;      /* Trigger Event */
    __IO uint32_t ASICCR;       /* ASIC Control Register */
    __IO uint32_t SR;           /* ETM Status Register */
    __IO uint32_t SCR;          /* System Configuration Register */
    __IO uint32_t TSSCR;        /* TraceEnable Start/Stop Control Register */
    __IO uint32_t TECR2;        /* TraceEnable Control 2 */
    __IO uint32_t TEEVR;        /* TraceEnable Event Register */
    __IO uint32_t TECR1;        /* TraceEnable Control 1 */
    __IO uint32_t FFRR;         /* FIFOFULL Region Register */
    __IO uint32_t FFLR;         /* FIFOFULL Level Register */
    __IO uint32_t VDEVR;        /* ViewData Event Register */
    __IO uint32_t VDCR1;        /* ViewData Control 1 */
    __IO uint32_t VDCR2;        /* ViewData Control 2 */
    __IO uint32_t VDCR3;        /* ViewData Control 3 */
    __IO uint32_t ACVR[16];     /* Address Comparator Value Registers */
    __IO uint32_t ACTR[16];     /* Address Comparator Access Type Registers */
    __IO uint32_t DCVR[16];     /* Data Comparator Value Registers */
    __IO uint32_t DCMR[16];     /* Data Comparator Mask Registers */
    __IO uint32_t CNTRLDVR[4];  /* Counter Reload Value Registers */
    __IO uint32_t CNTENR[4];    /* Counter Enable Registers */
    __IO uint32_t CNTRLDEVR[4]; /* Counter Reload Event Registers */
    __IO uint32_t CNTVR[4];     /* Counter Value Registers */
    __IO uint32_t SQabEVR[6];   /* Sequencer State Transition Event Registers */
    __IO uint32_t RESERVED0;
    __IO uint32_t SQR;          /* Current Sequencer State Register */
    __IO uint32_t EXTOUTEVR[4]; /* External Output Event Registers */
    __IO uint32_t CIDCVR[3];    /* Context ID Comparator Value Registers */
    __IO uint32_t CIDCMR;       /* Context ID Comparator Mask Register */
    __IO uint32_t IMPL[8];      /* Implementation specific registers */
    __IO uint32_t SYNCFR;       /* Synchronization Frequency Register */
    __IO uint32_t IDR;          /* ETM ID Register */
    __IO uint32_t CCER;         /* Configuration Code Extension Register */
    __IO uint32_t EXTINSELR;    /* Extended External Input Selection Register */
    __IO uint32_t TESSEICR;     /* TraceEnable Start/Stop EmbeddedICE Control Register */
    __IO uint32_t EIBCR;        /* EmbeddedICE Behavior Control Register */
    __IO uint32_t TSEVR;        /* Timestamp Event Register, ETMv3.5 */
    __IO uint32_t AUXCR;        /* Auxiliary Control Register, ETMv3.5 */
    __IO uint32_t TRACEIDR;     /* CoreSight Trace ID Register */
    __IO uint32_t RESERVED1;
    __IO uint32_t IDR2;         /* ETM ID Register 2 */
    __IO uint32_t RESERVED2[13];
    __IO uint32_t VMIDCVR;      /* VMID Comparator Value Register, ETMv3.5 */
    __IO uint32_t RESERVED3[47];
    __IO uint32_t OSLAR;        /* OS Lock Access Register */
    __IO uint32_t OSLSR;        /* OS Lock Status Register */
    __IO uint32_t OSSRR;        /* OS Save and Restore Register */
    __IO uint32_t RESERVED4;
    __IO uint32_t PDCR;         /* Power Down Control Register, ETMv3.5 */
    __IO uint32_t PDSR;         /* Device Power-Down Status Register */
    __IO uint32_t RESERVED5[762];
    __IO uint32_t ITCTRL;       /* Integration Mode Control Register */
    __IO uint32_t RESERVED6[39];
    __IO uint32_t CLAIMSET;     /* Claim Tag Set Register */
    __IO uint32_t CLAIMCLR;     /* Claim Tag Clear Register */
    __IO uint32_t RESERVED7[2];
    __IO uint32_t LAR;          /* Lock Access Register */
    __IO uint32_t LSR;          /* Lock Status Register */
    __IO uint32_t AUTHSTATUS;   /* Authentication Status Register */
    __IO uint32_t RESERVED8[3];
    __IO uint32_t DEVID;        /* CoreSight Device Configuration Register */
    __IO uint32_t DEVTYPE;      /* CoreSight Device Type Register */
    __IO uint32_t PIDR4;        /* Peripheral ID4 */
    __IO uint32_t PIDR5;        /* Peripheral ID5 */
    __IO uint32_t PIDR6;        /* Peripheral ID6 */
    __IO uint32_t PIDR7;        /* Peripheral ID7 */
    __IO uint32_t PIDR0;        /* Peripheral ID0 */
    __IO uint32_t PIDR1;        /* Peripheral ID1 */
    __IO uint32_t PIDR2;        /* Peripheral ID2 */
    __IO uint32_t PIDR3;        /* Peripheral ID3 */
    __IO uint32_t CIDR0;        /* Component ID0 */
    __IO uint32_t CIDR1;        /* Component ID1 */
    __IO uint32_t CIDR2;        /* Component ID2 */
    __IO uint32_t CIDR3;        /* Component ID3 */
} ETM_Type;

#define ETM_CR_POWERDOWN                0x00000001
#define ETM_CR_MONITORCPRT              0x00000002
#define ETM_CR_TRACE_DATA               0x00000004
#define ETM_CR_TRACE_ADDR               0x00000008
#define ETM_CR_PORTSIZE_1BIT            0x00200000
#define ETM_CR_PORTSIZE_2BIT            0x00200010
#define ETM_CR_PORTSIZE_4BIT            0x00000000
#define ETM_CR_PORTSIZE_8BIT            0x00000010
#define ETM_CR_PORTSIZE_16BIT           0x00000020
#define ETM_CR_STALL_PROCESSOR          0x00000080
#define ETM_CR_BRANCH_OUTPUT            0x00000100
#define ETM_CR_DEBUGREQ                 0x00000200
#define ETM_CR_PROGRAMMING              0x00000400
#define ETM_CR_ETMEN                    0x00000800
#define ETM_CR_CYCLETRACE               0x00001000
#define ETM_CR_CONTEXTID_8BIT           0x00004000
#define ETM_CR_CONTEXTID_16BIT          0x00008000
#define ETM_CR_CONTEXTID_32BIT          0x0000C000
#define ETM_CR_CONTEXTID_8BIT           0x00004000
#define ETM_CR_PORTMODE_ONCHIP          0x00000000
#define ETM_CR_PORTMODE_2_1             0x00010000
#define ETM_CR_PORTMODE_IMPL            0x00030000
#define ETM_CR_PORTMODE_1_1             0x00002000
#define ETM_CR_PORTMODE_1_2             0x00022000
#define ETM_CR_PORTMODE_1_3             0x00012000
#define ETM_CR_PORTMODE_1_4             0x00032000
#define ETM_CR_SUPPRESS_DATA            0x00040000
#define ETM_CR_FILTER_CPRT              0x00080000
#define ETM_CR_DATA_ONLY                0x00100000
#define ETM_CR_BLOCK_DEBUGGER           0x00400000
#define ETM_CR_BLOCK_SOFTWARE           0x00800000
#define ETM_CR_ACCESS                   0x01000000
#define ETM_CR_PROCSEL_Pos              25
#define ETM_CR_TIMESTAMP                0x10000000
#define ETM_CR_VMID                     0x40000000

#define ETM_SR_PROGSTATUS               0x00000002
#define ETM_SR_TRIGSTATUS               0x00000008

#define ETM_TECR1_EXCLUDE               0x01000000
#define ETM_TECR1_TSSEN                 0x02000000

#define ETM_FFRR_EXCLUDE                0x01000000

#define ETM_LAR_KEY                     0xC5ACCE55

#define ETM_TraceMode() ETM->CR &= ~ETM_CR_PROGRAMMING
#define ETM_SetupMode() ETM->CR |= ETM_CR_PROGRAMMING

#define ETM_BASE 0xE0041000
#define ETM ((ETM_Type*)ETM_BASE)

#endif
