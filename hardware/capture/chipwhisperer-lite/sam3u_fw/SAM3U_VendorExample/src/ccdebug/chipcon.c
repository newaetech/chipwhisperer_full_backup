/* This file is based on the GOODFet source-code,
  Copyright (C) Travis Goodspeed, 2013.
  
  Note it has been modified for the SAM3U by NewAE Technology Inc.,
  changes Copyright (C) NewAE Technology Inc, 2015. Changes have
  generally basterdized and reduced the functionality of the original
  code, so please see original code if using this to port.
  
  Full programming does not yet work with this file, only dumping
  of SRAM memory us currently tested.  
*/
#include <asf.h>
#include <string.h>
#include "ccdebug/chipcon.h"

// Timing is just fixed now
#define CCSPEED 3
#define CCDELAY(x) delay_us(1)

// SPI is bit-banged in this example
#define SETMOSI gpio_set_pin_high(PIN_PDIDTX_GPIO)
#define CLRMOSI gpio_set_pin_low(PIN_PDIDTX_GPIO)
#define SETCLK gpio_set_pin_high(PIN_PDIC_GPIO)
#define CLRCLK gpio_set_pin_low(PIN_PDIC_GPIO)
#define READMISO (gpio_pin_is_high(PIN_PDIDRX_GPIO))

#define CCWRITE gpio_configure_pin(PIN_PDIDTX_GPIO, PIN_PDIDTX_OUT_FLAGS)
#define CCREAD gpio_configure_pin(PIN_PDIDTX_GPIO, PIN_PDIDTX_IN_FLAGS)

/* todo: fixme */
#define CMDDATA_BUF_SIZE 64
static uint8_t cmddata[CMDDATA_BUF_SIZE];
static uint16_t * cmddataword = (uint16_t *)cmddata;

#define CC_STATUS_MAXPAYLOAD 32
uint8_t cc_status_payload[CC_STATUS_MAXPAYLOAD];
uint16_t cc_status_payload_size;

unsigned char cctrans8(unsigned char byte);
void cccmd(unsigned char len);
void ccread(unsigned char len);
void ccsetup(void);
void ccdebuginit(void);

//! Set up the pins for CC mode.  Does not init debugger.
void ccsetup(void){
	/* Turn on clock */
	//sysclk_enable_peripheral_clock(USART_PDI_ID);
	/* Set Tx and XCK as outputs, Rx as input */
	gpio_configure_pin(PIN_PDIDTX_GPIO, PIN_PDIDTX_OUT_FLAGS);
	gpio_configure_pin(PIN_PDIDRX_GPIO, PIN_PDIDRX_FLAGS|PIO_PULLUP);
	gpio_configure_pin(PIN_PDIC_GPIO, PIN_PDIC_OUT_FLAGS);
	gpio_configure_pin(PIN_TARG_NRST_GPIO, (PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT));
}


/* 33 cycle critical region
0000000e <ccdebuginit>:
   e:	f2 d0 0d 00 	bis.b	#13,	&0x0031	;5 cycles
  12:	31 00
  14:	f2 c2 31 00 	bic.b	#8,	&0x0031	;4 cycles
  18:	d2 c3 31 00 	bic.b	#1,	&0x0031	;4
  1c:	f2 e2 31 00 	xor.b	#8,	&0x0031	;4
  20:	f2 e2 31 00 	xor.b	#8,	&0x0031	;4
  24:	f2 e2 31 00 	xor.b	#8,	&0x0031	;4
  28:	f2 e2 31 00 	xor.b	#8,	&0x0031	;4
  2c:	d2 d3 31 00 	bis.b	#1,	&0x0031	;4
  30:	30 41       	ret
*/



/** Enables the target's CC interface, which sets target into debug mode. */
void ccdebuginit(void)
{	
	
	/* Perform debug init */
	gpio_set_pin_low(PIN_PDIC_GPIO);
	delay_us(20);
	gpio_set_pin_low(PIN_TARG_NRST_GPIO);
	delay_us(10);
	
	SETCLK;
	delay_cycles(10);
	CLRCLK;
	delay_cycles(10);
	SETCLK;
	delay_cycles(10);
	CLRCLK;
	
	delay_us(5);
	gpio_set_pin_high(PIN_TARG_NRST_GPIO);	

#if 0
	/* Set up the synchronous USART for XMEGA communications - 8 data bits, even parity, 2 stop bits */
	const sam_usart_opt_t usart_pdid_settings = {
		PDI_BAUD_RATE,
		US_MR_CHRL_8_BIT,
		US_MR_PAR_EVEN,
		US_MR_NBSTOP_2_BIT,
		US_MR_CHMODE_NORMAL
	};
	usart_init_sync_master(USART_PDI, &usart_pdid_settings, sysclk_get_main_hz());

	/* Turn on clock */
	gpio_configure_pin(PIN_PDIC_GPIO, PIN_PDIC_USART_FLAGS);
#endif
}


//! Read and write a CC bit.
unsigned char cctrans8(unsigned char byte){
  unsigned int bit;
  //This function came from the SPI Wikipedia article.
  //Minor alterations.

  for (bit = 0; bit < 8; bit++) {
    CCDELAY(CCSPEED>>2);
    /* write MOSI on trailing edge of previous clock */
    if (byte & 0x80)
      SETMOSI;
    else
      CLRMOSI;
    byte <<= 1;

    /* half a clock cycle before leading/rising edge */
    CCDELAY(CCSPEED>>2);
    SETCLK;

    /* half a clock cycle before trailing/falling edge */
    CCDELAY(CCSPEED>>2);

    /* read MISO on trailing edge */
    byte |= READMISO;
    CLRCLK;
  }

  return byte;
}

//! Send a command from txbytes.
void cccmd(unsigned char len){
  unsigned char i;
  CCWRITE;
  for(i=0;i<len;i++)
    cctrans8(cmddata[i]);
}

//! Fetch a reply, usually 1 byte.
void ccread(unsigned char len){
  unsigned char i;
  unsigned int timeout = 10;
  CCREAD;
  delay_us(2);
  
  /* Check if pin is high, if so do another try */  
  while (READMISO) {
	  cctrans8(0);
	  timeout--;
	  
	  if(timeout == 0){
		  printf("Timeout in chipcon ccread()\n");
		  return;
	  }
  }
  
  for(i=0;i<len;i++)
    cmddata[i]=cctrans8(0);
}

/** Handler for the ChipCon Command
 */
#define CC_BUF_SIZE 256
bool CCProtocol_ProcessCommand(void)
{
	long i;

	uint8_t command = udd_g_ctrlreq.req.wValue & 0xff;
	uint8_t arg1 = (udd_g_ctrlreq.req.wValue >> 8) & 0xff;	
	
	unsigned short blocklen;
	unsigned short blockadr;
	
	uint32_t addr32;
	
	int len = 1;
	
	switch(command){
    
		case CC_READBACK:
			udd_g_ctrlreq.payload_size = cc_status_payload_size;
			cc_status_payload_size = 0;
			if (udd_g_ctrlreq.payload_size){
				udd_g_ctrlreq.payload = cmddata;
				return true;
			}
			break;			
	
		case CC_PEEK:
			cmddata[0]=cc_peekirambyte(arg1);
			udd_g_ctrlreq.payload = cmddata;
			udd_g_ctrlreq.payload_size = 1;
			return true;
			break;
			
		case CC_POKE:
			cmddata[0]=cc_pokeirambyte(udd_g_ctrlreq.payload[0],udd_g_ctrlreq.payload[1]);
			break;
			
		case CC_READ:  //Write a command and optionally return 1-byte reply.
			len = udd_g_ctrlreq.payload_size;	
			memcpy(cmddata, udd_g_ctrlreq.payload, udd_g_ctrlreq.payload_size);	
			cccmd(len);
			if(cmddata[0] & 0x4) {
				ccread(1);
				cc_status_payload_size = 1;
			}
			break;
			
		case CC_WRITE: //Write a command with no reply.
			len =  udd_g_ctrlreq.payload_size;
			memcpy(cmddata, udd_g_ctrlreq.payload, udd_g_ctrlreq.payload_size);
			cccmd(len);
			break;
			
		case CC_START://enter debugger
			ccdebuginit();
			break;
			
		case CC_STOP://exit debugger
			//Take RST low, then high.
			gpio_set_pin_low(PIN_TARG_NRST_GPIO);
			CCDELAY(CCSPEED);
			gpio_set_pin_high(PIN_TARG_NRST_GPIO);
			break;

		case CC_SETUP:
			ccsetup();
			break;
			
		case CC_SET_FLASH_WORD_SIZE:
			flash_word_size = arg1;
			break;

		//Micro commands!
		case CC_CHIP_ERASE:
		case CC_MASS_ERASE_FLASH:
			cc_chip_erase();
			udd_g_ctrlreq.payload = cmddata;
			udd_g_ctrlreq.payload_size = 1;
			return true;
			break;
			
		case CC_WR_CONFIG:
			cc_wr_config(arg1);
			udd_g_ctrlreq.payload = cmddata;
			udd_g_ctrlreq.payload_size = 1;
			return true;
			break;
			
		case CC_RD_CONFIG:
			cc_rd_config();
			udd_g_ctrlreq.payload = cmddata;
			udd_g_ctrlreq.payload_size = 1;
			return true;
			break;
			
		case CC_GET_PC:
			cc_get_pc();
			udd_g_ctrlreq.payload = cmddata;
			udd_g_ctrlreq.payload_size = 2;
			return true;
			break;
 
		case CC_LOCKCHIP:
			cc_lockchip();
			
			//no break, return status
		case CC_READ_STATUS:
			cc_read_status();
			udd_g_ctrlreq.payload = cmddata;
			udd_g_ctrlreq.payload_size = 1;
			return true;
			break;
  
		case CC_SET_HW_BRKPNT:
			//NOT implemented
			return false;
			break;
			
		case CC_HALT:
			cc_halt();
			udd_g_ctrlreq.payload = cmddata;
			udd_g_ctrlreq.payload_size = 1;
			return true;
			break;

		case CC_RESUME:
			cc_resume();
			udd_g_ctrlreq.payload = cmddata;
			udd_g_ctrlreq.payload_size = 1;
			return true;
			break;
			
		case CC_DEBUG_INSTR:
			len =  udd_g_ctrlreq.payload_size;
			memcpy(cmddata, udd_g_ctrlreq.payload, udd_g_ctrlreq.payload_size);
			cc_debug_instr(len);
			udd_g_ctrlreq.payload = cmddata;
			cc_status_payload_size = 1;
			break;
			
		case CC_STEP_INSTR:
			cc_step_instr();
			udd_g_ctrlreq.payload = cmddata;
			udd_g_ctrlreq.payload_size = 1;
			return true;
			break;
			
		case CC_GET_CHIP_ID:
			cc_get_chip_id();
			udd_g_ctrlreq.payload = cmddata;
			udd_g_ctrlreq.payload_size = 2;
			return true;
			break;

		//Macro commands
		case CC_READ_CODE_MEMORY:		
			blockadr = (udd_g_ctrlreq.payload[1] << 8) | (udd_g_ctrlreq.payload[0]);			
			cmddata[0]=cc_peekcodebyte(blockadr);
			cc_status_payload_size = 1;
			break;

		case CC_READ_XDATA_MEMORY:
			//Read the length.
			blocklen = (udd_g_ctrlreq.payload[1] << 8) | (udd_g_ctrlreq.payload[0]);		
			blockadr = (udd_g_ctrlreq.payload[3] << 8) | (udd_g_ctrlreq.payload[2]);	
			
			if (blocklen > CMDDATA_BUF_SIZE) {
				cc_status_payload_size = 0;
				return false;
			}	

			//Return that many bytes.
			for(i=0;i<blocklen;i++)
				cmddata[i]=cc_peekdatabyte(blockadr+i);
			
			cc_status_payload_size = blocklen;
			break;

		case CC_WRITE_XDATA_MEMORY:
			blockadr = (udd_g_ctrlreq.payload[1] << 8) | (udd_g_ctrlreq.payload[0]);	
			cmddata[0]=cc_pokedatabyte(blockadr, udd_g_ctrlreq.payload[2]);
			cc_status_payload_size = 1;
			break;
  
		case CC_SET_PC:
			addr32 = (udd_g_ctrlreq.payload[3] << 24) | (udd_g_ctrlreq.payload[2] << 16) | (udd_g_ctrlreq.payload[1] << 8) | (udd_g_ctrlreq.payload[0]);
			cc_set_pc(addr32);
			break;
  
		case CC_WRITE_FLASH_PAGE:
			addr32 = (udd_g_ctrlreq.payload[3] << 24) | (udd_g_ctrlreq.payload[2] << 16) | (udd_g_ctrlreq.payload[1] << 8) | (udd_g_ctrlreq.payload[0]);
			cc_write_flash_page(addr32);
			break;

		case CC_WIPEFLASHBUFFER:
			for(i=0xf000;i<0xf800;i++)
				cc_pokedatabyte(i,0xFF);
			break;

		case CC_CLOCK_INIT:
		case CC_PROGRAM_FLASH:
		default:
   			printf("This Chipcon command is not yet implemented.\n");
			break;
  }
	
	return false;
}



//! Set the Chipcon's Program Counter
void cc_set_pc(uint32_t adr){
  cmddata[0]=0x02;             //SetPC
  cmddata[1]=((adr>>8)&0xff);  //HIBYTE
  cmddata[2]=adr&0xff;         //LOBYTE
  cc_debug_instr(3);
  return;
}

//! Erase all of a Chipcon's memory.
void cc_chip_erase(void){
  cmddata[0]=CCCMD_CHIP_ERASE; //0x14
  cccmd(1);
  ccread(1);
}
//! Write the configuration byte.
void cc_wr_config(unsigned char config){
  cmddata[0]=CCCMD_WR_CONFIG; //0x1D
  cmddata[1]=config;
  cccmd(2);
  ccread(1);
}

//! Locks the chip.
void cc_lockchip(void){
  register int i;

  printf("Locking chip.\n");
  cc_wr_config(1);//Select Info Flash
  if(!(cc_rd_config()&1))
    printf("Config forgotten!\n");

  //Clear config page.
  for(i=0;i<2048;i++)
    cc_pokedatabyte(0xf000+i,0);
  cc_write_flash_page(0);
  if(cc_peekcodebyte(0))
    printf("Failed to clear info flash byte.\n");

  cc_wr_config(0);
  if(cc_rd_config()&1)
    printf("Stuck in info flash mode!\n");
	
}

//! Read the configuration byte.
unsigned char cc_rd_config(){
  cmddata[0]=CCCMD_RD_CONFIG; //0x24
  cccmd(1);
  ccread(1);
  return cmddata[0];
}


//! Read the status register
unsigned char cc_read_status(){
  cmddata[0]=CCCMD_READ_STATUS; //0x3f
  cccmd(1);
  ccread(1);
  return cmddata[0];
}

//! Read the CHIP ID bytes.
unsigned short cc_get_chip_id(){
  cmddata[0]=CCCMD_GET_CHIP_ID; //0x68
  cccmd(1);
  ccread(2);


  //Find the flash word size.
  switch(cmddata[0]){
  case 0x01://CC1110
  case 0x11://CC1111
  case 0x81://CC2510
  case 0x91://CC2511
    //printf("2 bytes/flash word\n");
    flash_word_size=0x02;
    break;
  case 0x85://CC2430
  case 0x89://CC2431	
	//printf("4 bytes/flash word\n");
    flash_word_size = 0x04;
	break;

  case 0xA5: //CC2530
  case 0xB5: //CC2531
  case 0x95: //CC2533
  case 0x8D: //CC2540
  case 0x41: //CC2541
	flash_word_size = 0x02;
	printf("WARNING: CC not supported for flash programming\n");
	break;

  default:
    //debugstr("Warning: Guessing flash word size.");
    //flash_word_size=0;
    break;
  }

  //Return the word.
  return cmddataword[0];
}

//! Populates flash buffer in xdata.
void cc_write_flash_buffer(uint8_t *data, uint16_t len){
  cc_write_xdata(0xf000, data, len);
}
//! Populates flash buffer in xdata.
void cc_write_xdata(uint16_t adr, uint8_t *data, uint16_t len){
  uint8_t i;
  for(i=0; i<len; i++){
    cc_pokedatabyte(adr+i,
		    data[i]);
  }
}


//32-bit words, 2KB pages
//0x20 0x00 for CC2430, CC1110
#define HIBYTE_WORDS_PER_FLASH_PAGE 0x02
#define LOBYTE_WORDS_PER_FLASH_PAGE 0x00

/** Ugh, this varies by chip.
    0x800 for CC2430
    0x400 for CC1110
*/
//#define FLASHPAGE_SIZE 0x400
#define MAXFLASHPAGE_SIZE 0x800
#define MINFLASHPAGE_SIZE 0x400


//32 bit words on CC2430
//16 bit words on CC1110
//#define FLASH_WORD_SIZE 0x2
uint8_t flash_word_size = 0; //0x02;


/* Flash Write Timing
   MHZ | FWT (0xAB)
   12  | 0x10
   13  | 0x11
   16  | 0x15
   24  | 0x20
   26  | 0x23  (IM ME)
   32  | 0x2A  (Modula.si)
*/
//#define FWT 0x23

const uint8_t flash_routine[] = {
  //0:
  //MOV FADDRH, #imm;
  0x75, 0xAD,
  0x00,//#imm=((address >> 8) / FLASH_WORD_SIZE) & 0x7E,

  //0x75, 0xAB, 0x23, //Set FWT per clock
  0x75, 0xAC, 0x00,                                          //                 MOV FADDRL, #00;
  /* Erase page. */
  0x75, 0xAE, 0x01,                                          //                 MOV FLC, #01H; // ERASE
                                                             //                 ; Wait for flash erase to complete
  0xE5, 0xAE,                                                // eraseWaitLoop:  MOV A, FLC;
  0x20, 0xE7, 0xFB,                                          //                 JB ACC_BUSY, eraseWaitLoop;

  /* End erase page. */
                                                             //                 ; Initialize the data pointer
  0x90, 0xF0, 0x00,                                          //                 MOV DPTR, #0F000H;
                                                             //                 ; Outer loops
  0x7F, HIBYTE_WORDS_PER_FLASH_PAGE,                         //                 MOV R7, #imm;
  0x7E, LOBYTE_WORDS_PER_FLASH_PAGE,                         //                 MOV R6, #imm;
  0x75, 0xAE, 0x02,                                          //                 MOV FLC, #02H; // WRITE
                                                             //                     ; Inner loops
  //24:
  0x7D, 0xde /*FLASH_WORD_SIZE*/,                                     // writeLoop:          MOV R5, #imm;
  0xE0,                                                      // writeWordLoop:          MOVX A, @DPTR;
  0xA3,                                                      //                         INC DPTR;
  0xF5, 0xAF,                                                //                         MOV FWDATA, A;
  0xDD, 0xFA,                                                //                     DJNZ R5, writeWordLoop;
                                                             //                     ; Wait for completion
  0xE5, 0xAE,                                                // writeWaitLoop:      MOV A, FLC;
  0x20, 0xE6, 0xFB,                                          //                     JB ACC_SWBSY, writeWaitLoop;
  0xDE, 0xF1,                                                //                 DJNZ R6, writeLoop;
  0xDF, 0xEF,                                                //                 DJNZ R7, writeLoop;
                                                             //                 ; Done, fake a breakpoint
  0xA5                                                       //                 DB 0xA5;
};


//! Copies flash buffer to flash.
void cc_write_flash_page(uint32_t adr){
  //Assumes that page has already been written to XDATA 0xF000
  //debugstr("Flashing 2kb at 0xF000 to given adr.");

  if(adr&(MINFLASHPAGE_SIZE-1)){
    printf("Flash page address is not on a page boundary.  Aborting.\n");
    return;
  }

  if(flash_word_size!=2 && flash_word_size!=4){
    printf("Flash word size is wrong, aborting write to 0x%x\n", (unsigned int)adr);
    return;
  }

  //Routine comes next
  //WRITE_XDATA_MEMORY(IN: 0xF000 + FLASH_PAGE_SIZE, sizeof(routine), routine);
  cc_write_xdata(0xF000+MAXFLASHPAGE_SIZE,
		 (uint8_t*) flash_routine, sizeof(flash_routine));
  //Patch routine's third byte with
  //((address >> 8) / FLASH_WORD_SIZE) & 0x7E
  cc_pokedatabyte(0xF000+MAXFLASHPAGE_SIZE+2,
		  ((adr>>8)/flash_word_size)&0x7E);
  //Patch routine to define FLASH_WORD_SIZE
  if(flash_routine[25]!=0xde)
    printf("Ugly patching code failing in chipcon.c\n");
  cc_pokedatabyte(0xF000+MAXFLASHPAGE_SIZE+25,
		  flash_word_size);

  //debugstr("Wrote flash routine.");

  //MOV MEMCTR, (bank * 16) + 1;
  cmddata[0]=0x75;
  cmddata[1]=0xc7;
  cmddata[2]=0x51;
  cc_debug_instr(3);
  //debugstr("Loaded bank info.");

  cc_set_pc(0xf000+MAXFLASHPAGE_SIZE);//execute code fragment
  cc_resume();

  //debugstr("Executing.");


  while(!(cc_read_status()&CC_STATUS_CPUHALTED)){
    ;//led_toggle();//blink LED while flashing
	printf("%02x\n", cc_read_status());
	delay_ms(250);
  }


  //debugstr("Done flashing.");

  ;//led_off();
}

//! Read the PC
unsigned short cc_get_pc(void){
  cmddata[0]=CCCMD_GET_PC; //0x28
  cccmd(1);
  ccread(2);

  //Return the word.
  return cmddataword[0];
}

//! Halt the CPU.
void cc_halt(void){
  cmddata[0]=CCCMD_HALT; //0x44
  cccmd(1);
  ccread(1);
  return;
}
//! Resume the CPU.
void cc_resume(void){
  cmddata[0]=CCCMD_RESUME; //0x4C
  cccmd(1);
  ccread(1);
  return;
}


//! Step an instruction
void cc_step_instr(void){
  cmddata[0]=CCCMD_STEP_INSTR; //0x5C
  cccmd(1);
  ccread(1);
  return;
}

//! Debug an instruction.
void cc_debug_instr(unsigned char len){
  //Bottom two bits of command indicate length.
  unsigned char cmd=CCCMD_DEBUG_INSTR+(len&0x3); //0x54+len
  CCWRITE;
  cctrans8(cmd);  //Second command code
  cccmd(len&0x3); //Command itself.
  ccread(1);
  return;
}

//! Debug an instruction, for local use.
unsigned char cc_debug(unsigned char len,
	      unsigned char a,
	      unsigned char b,
	      unsigned char c){
  unsigned char cmd=CCCMD_DEBUG_INSTR+(len&0x3);//0x54+len
  CCWRITE;
  cctrans8(cmd);
  if(len>0)
    cctrans8(a);
  if(len>1)
    cctrans8(b);
  if(len>2)
    cctrans8(c);
  CCREAD;
  delay_us(2);
  
  /* Check if pin is high, if so do another try */  
  int timeout = 10;
  while (READMISO) {
	  cctrans8(0);
	  timeout--;
	  
	  if(timeout == 0){
		  printf("Timeout in chipcon cc_debug()\n");
		  return 0x00;
	  }
  }
  
  return cctrans8(0x00);
}

//! Fetch a byte of code memory.
unsigned char cc_peekcodebyte(unsigned long adr){
  /** See page 9 of SWRA124 */
  unsigned char bank=adr>>15,
    lb=adr&0xFF,
    hb=(adr>>8)&0x7F,
    toret=0;
  adr&=0x7FFF;

  //MOV MEMCTR, (bank*16)+1
  cc_debug(3, 0x75, 0xC7, (bank<<4) + 1);
  //MOV DPTR, address
  cc_debug(3, 0x90, hb, lb);

  //for each byte
  //CLR A
  cc_debug(2, 0xE4, 0, 0);
  //MOVC A, @A+DPTR;
  toret=cc_debug(3, 0x93, 0, 0);
  //INC DPTR
  //cc_debug(1, 0xA3, 0, 0);

  return toret;
}


//! Set a byte of data memory.
unsigned char cc_pokedatabyte(unsigned int adr,
			   unsigned char val){
  unsigned char
    hb=(adr&0xFF00)>>8,
    lb=adr&0xFF;

  //MOV DPTR, adr
  cc_debug(3, 0x90, hb, lb);
  //MOV A, val
  cc_debug(2, 0x74, val, 0);
  //MOVX @DPTR, A
  cc_debug(1, 0xF0, 0, 0);

  return 0;
  /*
DEBUG_INSTR(IN: 0x90, HIBYTE(address), LOBYTE(address), OUT: Discard);
for (n = 0; n < count; n++) {
    DEBUG_INSTR(IN: 0x74, inputArray[n], OUT: Discard);
    DEBUG_INSTR(IN: 0xF0, OUT: Discard);
    DEBUG_INSTR(IN: 0xA3, OUT: Discard);
}
   */
}

//! Fetch a byte of data memory.
unsigned char cc_peekdatabyte(unsigned int adr){
  unsigned char
    hb=(adr&0xFF00)>>8,
    lb=adr&0xFF;

  //MOV DPTR, adr
  cc_debug(3, 0x90, hb, lb);
  //MOVX A, @DPTR
  //Must be 2, perhaps for clocking?
  return cc_debug(3, 0xE0, 0, 0);
}


//! Fetch a byte of IRAM.
uint8_t cc_peekirambyte(uint8_t adr){
  //CLR A
  cc_debug(2, 0xE4, 0, 0);
  //MOV A, #iram
  return cc_debug(3, 0xE5, adr, 0);
}

//! Write a byte of IRAM.
uint8_t cc_pokeirambyte(uint8_t adr, uint8_t val){
  //CLR A
  cc_debug(2, 0xE4, 0, 0);
  //MOV #iram, #val
  return cc_debug(3, 0x75, adr, val);
  //return cc_debug(3, 0x75, val, adr);
}


