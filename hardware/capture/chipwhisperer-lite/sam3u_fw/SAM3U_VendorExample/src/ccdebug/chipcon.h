/* This file is based on the GOODFet source-code,
  Copyright (C) Travis Goodspeed, 2013.
  
  Note it has been modified for the SAM3U by NewAE Technology Inc.,
  changes Copyright (C) NewAE Technology Inc, 2015. Changes have
  generally basterdized and reduced the functionality of the original
  code, so please see original code if using this to port.
*/

#ifndef CHIPCON_H
#define CHIPCON_H

#define CHIPCON 0x30

//Chipcon command definitions.
#define CCCMD_CHIP_ERASE 0x14

//1D or 19?
#define CCCMD_WR_CONFIG 0x1D
#define CCCMD_RD_CONFIG 0x24
#define CCCMD_READ_STATUS 0x34
#define CCCMD_GET_CHIP_ID 0x68
#define CCCMD_GET_PC 0x28
#define CCCMD_HALT 0x44
#define CCCMD_RESUME 0x4C
#define CCCMD_STEP_INSTR 0x5C
#define CCCMD_DEBUG_INSTR 0x54

//! Flash Word Size
extern uint8_t flash_word_size;

void ccsetup(void);
void ccdebuginit(void);

//! Erase a chipcon chip.
void cc_chip_erase(void);
//! Write the configuration byte.
void cc_wr_config(unsigned char config);
//! Read the configuration byte.
unsigned char cc_rd_config(void);
//! Read the status register.
unsigned char cc_read_status(void);
//! Read the CHIP ID bytes.
unsigned short cc_get_chip_id(void);
//! Get the PC
unsigned short cc_get_pc(void);
//! Set a hardware breakpoint.
void cc_set_hw_brkpnt(unsigned short);
//! Debug an instruction, for remote use.
void cc_debug_instr(unsigned char);
//!Read a byte of code memory.
unsigned char cc_peekcodebyte(unsigned long adr);
//!Read a byte of data memory.
unsigned char cc_peekdatabyte(unsigned int adr);
//! Fetch a byte of IRAM.
uint8_t cc_peekirambyte(uint8_t adr);
//! Write a byte of IRAM.
uint8_t cc_pokeirambyte(uint8_t adr, uint8_t val);
//! Set a byte of data memory.
unsigned char cc_pokedatabyte(unsigned int adr,
			      unsigned char val);
//! Debug an instruction, for local use.
unsigned char cc_debug(unsigned char len,
		       unsigned char a,
		       unsigned char b,
		       unsigned char c);

//! Populates flash buffer in xdata.
void cc_write_flash_buffer(uint8_t *data, uint16_t len);
//! Populates flash buffer in xdata.
void cc_write_xdata(uint16_t adr, uint8_t *data, uint16_t len);
//! Copies flash buffer to flash.
void cc_write_flash_page(uint32_t adr);
//! Set the Chipcon's Program Counter
void cc_set_pc(uint32_t adr);

//! Halt the CPU.
void cc_halt(void);
//! Resume the CPU.
void cc_resume(void);
//! Step an instruction
void cc_step_instr(void);
//! Locks the chip.
void cc_lockchip(void);

bool CCProtocol_ProcessCommand(void);

#define CC_STATUS_ERASED 0x80
#define CC_STATUS_PCONIDLE 0x40
#define CC_STATUS_CPUHALTED 0x20
#define CC_STATUS_PM0 0x10
#define CC_STATUS_HALTSTATUS 0x08
#define CC_STATUS_LOCKED 0x04
#define CC_STATUS_OSCSTABLE 0x02
#define CC_STATUS_OVERFLOW 0x01

//GOODFet commands
#define CC_READ  0x00
#define CC_WRITE 0x01
#define CC_PEEK  0x02
#define CC_POKE  0x03
#define CC_SETUP 0x10
#define CC_START 0x20
#define CC_STOP  0x21

//CHIPCON commands
#define CC_CHIP_ERASE 0x80
#define CC_WR_CONFIG 0x81
#define CC_RD_CONFIG 0x82
#define CC_GET_PC 0x83
#define CC_READ_STATUS 0x84
#define CC_SET_HW_BRKPNT 0x85
#define CC_HALT 0x86
#define CC_RESUME 0x87
#define CC_DEBUG_INSTR 0x88
#define CC_STEP_INSTR 0x89
#define CC_STEP_REPLACE 0x8a
#define CC_GET_CHIP_ID 0x8b
//CHIPCON macros
#define CC_READ_CODE_MEMORY 0x90
#define CC_READ_XDATA_MEMORY 0x91
#define CC_WRITE_XDATA_MEMORY 0x92
#define CC_SET_PC 0x93
#define CC_CLOCK_INIT 0x94
#define CC_WRITE_FLASH_PAGE 0x95
#define CC_READ_FLASH_PAGE 0x96
#define CC_MASS_ERASE_FLASH 0x97
#define CC_PROGRAM_FLASH 0x98
#define CC_WIPEFLASHBUFFER 0x99
#define CC_LOCKCHIP 0x9A

//Extra commands (not in original)
#define CC_SET_FLASH_WORD_SIZE 0xA0
#define CC_READBACK            0xA1

#endif // CHIPCON_H
