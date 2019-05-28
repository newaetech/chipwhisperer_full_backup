#include <machine/intrinsics.h>
#include <machine/wdtcon.h>
#include <stdlib.h>
#include <stdint.h>

#include "IfxScu_reg.h"
#include "IfxScu_bf.h"
#include "IfxCpu_reg.h"
#include "IfxCpu_bf.h"
#include "IfxStm_reg.h"
#include "IfxStm_bf.h"
#include "IfxQspi_reg.h"
#include "IfxStm_reg.h"
#include "IfxStm_bf.h"

# define BOARD_NAME				"TriBoard-TC233A"
# define BOARD_TITLE			"TC233A TriBoard"
# define MCU_NAME				"TC233A"

#include "IfxPort_reg.h"
#include "IfxPort_bf.h"
#include "IfxAsclin_reg.h"
#include "IfxAsclin_bf.h"
#include "aurix_hal_sys.h"

static Ifx_P * const PORT15 = (Ifx_P *)&MODULE_P15;
static Ifx_P * const PORT14 = (Ifx_P *)&MODULE_P14;
static Ifx_P * const PORT11 = (Ifx_P *)&MODULE_P11;
static Ifx_ASCLIN * const UART = (Ifx_ASCLIN *)&MODULE_ASCLIN0;

/* UART primitives */
#define RX_CLEAR(u)				((u)->FLAGSCLEAR.U = (IFX_ASCLIN_FLAGSCLEAR_RFLC_MSK << IFX_ASCLIN_FLAGSCLEAR_RFLC_OFF))
#define TX_CLEAR(u)				((u)->FLAGSCLEAR.U = (IFX_ASCLIN_FLAGSCLEAR_TFLC_MSK << IFX_ASCLIN_FLAGSCLEAR_TFLC_OFF))
#define PUT_CHAR(u, c)			((u)->TXDATA.U = (c))
#define GET_CHAR(u)				((u)->RXDATA.U)
#define GET_ERROR_STATUS(u)		(((u)->FLAGS.U) & ASC_ERROR_MASK)
#define RESET_ERROR(u)			((u)->FLAGSCLEAR.U = ASC_CLRERR_MASK)
#define TX_START(u)				((u)->FLAGSSET.U   = (IFX_ASCLIN_FLAGSSET_TFLS_MSK << IFX_ASCLIN_FLAGSSET_TFLS_OFF))
#define TX_READY(u)				((u)->FLAGS.B.TFL != 0)				/* Transmit FIFO Level */
#define RX_READY(u)				((u)->FLAGS.B.RFL != 0)				/* Receive FIFO Level */

/* baud rate values at 100 MHz */
#define BAUD_9600				(48 * 1)
#define BAUD_19200				(48 * 2)
#define BAUD_38400				(48 * 4)
#define BAUD_57600				(48 * 6)
#define BAUD_115200				(48 * 12)

/* Port Modes */
#define IN_NOPULL0				0x00	/* Port Input No Pull Device */
#define IN_PULLDOWN				0x01	/* Port Input Pull Down Device */
#define IN_PULLUP				0x02	/* Port Input Pull Up Device */
#define IN_NOPULL3				0x03	/* Port Input No Pull Device */
#define OUT_PPGPIO				0x10	/* Port Output General Purpose Push/Pull */
#define OUT_PPALT1				0x11	/* Port Output Alternate 1 Function Push/Pull */
#define OUT_PPALT2				0x12	/* Port Output Alternate 2 Function Push/Pull */
#define OUT_PPALT3				0x13	/* Port Output Alternate 3 Function Push/Pull */
#define OUT_PPALT4				0x14	/* Port Output Alternate 4 Function Push/Pull */
#define OUT_PPALT5				0x15	/* Port Output Alternate 5 Function Push/Pull */
#define OUT_PPALT6				0x16	/* Port Output Alternate 6 Function Push/Pull */
#define OUT_PPALT7				0x17	/* Port Output Alternate 7 Function Push/Pull */
#define OUT_ODGPIO				0x18	/* Port Output General Purpose Open Drain */
#define OUT_ODALT1				0x19	/* Port Output Alternate 1 Function Open Drain */
#define OUT_ODALT2				0x1A	/* Port Output Alternate 2 Function Open Drain */
#define OUT_ODALT3				0x1B	/* Port Output Alternate 3 Function Open Drain */
#define OUT_ODALT4				0x1C	/* Port Output Alternate 4 Function Open Drain */
#define OUT_ODALT5				0x1D	/* Port Output Alternate 5 Function Open Drain */
#define OUT_ODALT6				0x1E	/* Port Output Alternate 6 Function Open Drain */
#define OUT_ODALT7				0x1F	/* Port Output Alternate 7 Function Open Drain */

/* definitions for RX error conditions */
#define ASC_ERROR_MASK			((IFX_ASCLIN_FLAGS_PE_MSK << IFX_ASCLIN_FLAGS_PE_OFF) | \
								 (IFX_ASCLIN_FLAGS_FE_MSK << IFX_ASCLIN_FLAGS_FE_OFF) | \
								 (IFX_ASCLIN_FLAGS_RFO_MSK << IFX_ASCLIN_FLAGS_RFO_OFF))

#define ASC_CLRERR_MASK			((IFX_ASCLIN_FLAGSCLEAR_PEC_MSK << IFX_ASCLIN_FLAGSCLEAR_PEC_OFF) | \
								 (IFX_ASCLIN_FLAGSCLEAR_FEC_MSK << IFX_ASCLIN_FLAGSCLEAR_FEC_OFF) | \
								 (IFX_ASCLIN_FLAGSCLEAR_RFOC_MSK << IFX_ASCLIN_FLAGSCLEAR_RFOC_OFF))


void platform_init(void)
{
     unlock_wdtcon();
     unlock_safety_wdtcon(); //EVR13CON is safety_endinit protected ("SE" in user manual)

     SCU_EVR13CON.U |= 0b11 << 28; //shut off internal regulator

     lock_wdtcon();
     lock_safety_wdtcon();

     SYSTEM_Init();
     PORT11->IOCR8.U = (0b10000 << 19) | (0b10000 << 11); //P11.10 as GPO Push pull
     PORT11->OMR.U = (1 << 10) | (1 << 26); //P11.10 High
}

//trigger later
void trigger_setup(void)
{
     PORT14->IOCR4.U = (0b10000 << 3); //P14.4 -> output
     PORT14->OMR.B.PCL4 = 1; //trigger low
}

void trigger_high(void)
{
     PORT14->OMR.B.PS4 = 1; //trigger high
}

void trigger_low(void)
{
     PORT14->OMR.B.PCL4 = 1; //trigger low
}

void init_uart(void)
{
	PORT15->IOCR0.U = (0b10010 << 19); //P15.3 -> input, P15.2 -> UART TX PP
	PORT15->OMR.B.PS2 = 1;

	unlock_wdtcon();
	UART->CLC.U = 0;

	PORT15->PDR0.B.PD2 = 0;
	PORT15->PDR0.B.PD3 = 0;

	lock_wdtcon();

	(void)UART->CLC.U; //?
	UART->IOCR.B.ALTI = 1; //P15.3 for as RX

	UART->CSR.U = 0; //turn off UART clock to allow registers to be modified

	//1 byte into fifo per write, enable tx fifo, flush tx fifo
	UART->TXFIFOCON.U = (1 << 6) | (1 << 1) | (1 << 0);

	//same as tx fifo stuff
	UART->RXFIFOCON.U = (1 << 6) | (1 << 1) | (1 << 0);

	//HighTec UART assumes 100MHz clock and uses prescale of 9+1, so we have prescale of 1+1
	//to keep things the same
	//prescale 9+1, oversample 16, sample position 7,8,9, 3 samples per bit
	UART->BITCON.U = (1) | (15 << 16) | (9 << 24) | (1 << 31);

	//8n1 UART
	UART->FRAMECON.U = (1 << 9) | (0 << 16) | (0 << 30);

	UART->DATCON.U = 7; //8bit data length

/* #define BAUD_NUM (48 * 40) */
/* #define BAUD_DEN (3125) */
#define BAUD_NUM (1001)
#define BAUD_DEN (3002)
/* #define BAUD_NUM (200) */
/* #define BAUD_DEN (120) */
  //NOTE: DEN>NUM

	/*
	 * fosc * num / ((prescale + 1) * den * (oversample + 1))
	 * = 100MHz * 48 * 4 / (10 * 3125 * 16)
	 * = 38400
	 */
	UART->BRG.U = (BAUD_DEN << 0) | (BAUD_NUM << 16);

	UART->FRAMECON.B.MODE = 1; //asc mode
	UART->CSR.U = 1; //CLC as clock source

	TX_START(UART); //macro from bspconfig, expand later
}

int poll_uart(char *c)
{
     unsigned char ret;
     int res = 0;

     if (RX_READY(UART))
     {
          ret = (unsigned char)GET_CHAR(UART);
          /* acknowledge receive */
          RX_CLEAR(UART);
          /* check for error condition */
          if (GET_ERROR_STATUS(UART))
          {
               /* reset error flags */
               RESET_ERROR(UART);
               /* ignore this character */
          }
          else
          {
               /* this is a valid character */
               *c = ret;
               res = 1;
          }
     }

     return res;
}
void putch(char c)
{
     while (!TX_READY(UART));
     TX_CLEAR(UART);
     PUT_CHAR(UART, c);
}
char getch(void)
{
     char ch;
     while (!poll_uart(&ch));
     return ch;
}
