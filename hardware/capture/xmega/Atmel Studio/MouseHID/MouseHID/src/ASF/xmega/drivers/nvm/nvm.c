/**
 * \file
 *
 * \brief Non Volatile Memory controller driver
 *
 * Copyright (C) 2010-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#include "compiler.h"
#include "ccp.h"
#include "nvm.h"
#include <string.h>

/**
 * \weakgroup nvm_signature_group
 * @{
 */

/**
 * \brief Read the device serial
 *
 * This function returns the device serial stored in the device.
 *
 * \note This function is modifying the NVM.CMD register.
 *       If the application are using program space access in interrupts
 *       (__flash pointers in IAR EW or pgm_read_byte in GCC) interrupts
 *       needs to be disabled when running EEPROM access functions. If not
 *       the program space reads will be corrupted.
 *
 * \retval storage Pointer to the structure where to store the device serial
 */
void nvm_read_device_serial(struct nvm_device_serial *storage)
{
	storage->lotnum0 = nvm_read_production_signature_row(
			nvm_get_production_signature_row_offset(LOTNUM0));
	storage->lotnum1 = nvm_read_production_signature_row(
			nvm_get_production_signature_row_offset(LOTNUM1));
	storage->lotnum2 = nvm_read_production_signature_row(
			nvm_get_production_signature_row_offset(LOTNUM2));
	storage->lotnum3 = nvm_read_production_signature_row(
			nvm_get_production_signature_row_offset(LOTNUM3));
	storage->lotnum4 = nvm_read_production_signature_row(
			nvm_get_production_signature_row_offset(LOTNUM4));
	storage->lotnum5 = nvm_read_production_signature_row(
			nvm_get_production_signature_row_offset(LOTNUM5));

	storage->wafnum  = nvm_read_production_signature_row(
			nvm_get_production_signature_row_offset(WAFNUM));

	storage->coordx0 = nvm_read_production_signature_row(
			nvm_get_production_signature_row_offset(COORDX0));
	storage->coordx1 = nvm_read_production_signature_row(
			nvm_get_production_signature_row_offset(COORDX1));
	storage->coordy0 = nvm_read_production_signature_row(
			nvm_get_production_signature_row_offset(COORDY0));
	storage->coordy1 = nvm_read_production_signature_row(
			nvm_get_production_signature_row_offset(COORDY1));
}

//! @}

/**
 * \weakgroup nvm_eeprom_group
 * @{
 */

/**
 * \brief Read one byte from EEPROM using mapped access.
 *
 * This function reads one byte from EEPROM using mapped access.
 *
 * \param  addr       EEPROM address, between 0 and EEPROM_SIZE
 *
 *  \return  Byte value read from EEPROM.
 */
uint8_t nvm_eeprom_read_byte(eeprom_addr_t addr)
{
	uint8_t data;
	Assert(addr <= EEPROM_SIZE);

	/* Wait until NVM is ready */
	nvm_wait_until_ready();
	eeprom_enable_mapping();
	data = *(uint8_t*)(addr + MAPPED_EEPROM_START),
	eeprom_disable_mapping();
	return data;
}

/**
 * \brief Read buffer within the eeprom
 *
 * \param address   the address to where to read
 * \param buf       pointer to the data
 * \param len       the number of bytes to read
 */
void nvm_eeprom_read_buffer(eeprom_addr_t address, void *buf, uint16_t len)
{
	nvm_wait_until_ready();
	eeprom_enable_mapping();
	memcpy( buf,(void*)(address+MAPPED_EEPROM_START), len );
	eeprom_disable_mapping();
}


/**
 * \brief Write one byte to EEPROM using IO mapping.
 *
 * This function writes one byte to EEPROM using IO-mapped access.
 * This function will cancel all ongoing EEPROM page buffer loading
 * operations, if any.
 *
 * \param  address    EEPROM address (max EEPROM_SIZE)
 * \param  value      Byte value to write to EEPROM.
 */
void nvm_eeprom_write_byte(eeprom_addr_t address, uint8_t value)
{
	uint8_t old_cmd;

	Assert(address <= EEPROM_SIZE);
	/*  Flush buffer to make sure no unintentional data is written and load
	 *  the "Page Load" command into the command register.
	 */
	old_cmd = NVM.CMD;
	nvm_eeprom_flush_buffer();
	// Wait until NVM is ready
	nvm_wait_until_ready();
	nvm_eeprom_load_byte_to_buffer(address, value);

	// Set address to write to
	NVM.ADDR2 = 0x00;
	NVM.ADDR1 = (address >> 8) & 0xFF;
	NVM.ADDR0 = address & 0xFF;

	/*  Issue EEPROM Atomic Write (Erase&Write) command. Load command, write
	 *  the protection signature and execute command.
	 */
	NVM.CMD = NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc;
	nvm_exec();
	NVM.CMD = old_cmd;
}

/**
 * \brief Write buffer within the eeprom
 *
 * \param address   the address to where to write
 * \param buf       pointer to the data
 * \param len       the number of bytes to write
 */
void nvm_eeprom_erase_and_write_buffer(eeprom_addr_t address, const void *buf, uint16_t len)
{
	while (len) {
		if (((address%EEPROM_PAGE_SIZE)==0) && (len>=EEPROM_PAGE_SIZE)) {
			// A full page can be written
			nvm_eeprom_load_page_to_buffer((uint8_t*)buf);
			nvm_eeprom_atomic_write_page(address/EEPROM_PAGE_SIZE);
			address += EEPROM_PAGE_SIZE;
			buf = (uint8_t*)buf + EEPROM_PAGE_SIZE;
			len -= EEPROM_PAGE_SIZE;
		} else {
			nvm_eeprom_write_byte(address++, *(uint8_t*)buf);
			buf = (uint8_t*)buf + 1;
			len--;
		}
	}
}


/**
 * \brief Flush temporary EEPROM page buffer.
 *
 * This function flushes the EEPROM page buffers. This function will cancel
 * any ongoing EEPROM page buffer loading operations, if any.
 * This function also works for memory mapped EEPROM access.
 *
 * \note An EEPROM write operations will automatically flush the buffer for you.
 * \note The function does not preserve the value of the NVM.CMD register
 */
void nvm_eeprom_flush_buffer(void)
{
	// Wait until NVM is ready
	nvm_wait_until_ready();

	// Flush EEPROM page buffer if necessary
	if ((NVM.STATUS & NVM_EELOAD_bm) != 0) {
		NVM.CMD = NVM_CMD_ERASE_EEPROM_BUFFER_gc;
		nvm_exec();
	}
}

/**
 * \brief Load single byte into temporary page buffer.
 *
 * This function loads one byte into the temporary EEPROM page buffers.
 * If memory mapped EEPROM is enabled, this function will not work.
 * Make sure that the buffer is flushed before starting to load bytes.
 * Also, if multiple bytes are loaded into the same location, they will
 * be ANDed together, thus 0x55 and 0xAA will result in 0x00 in the buffer.
 *
 * \note Only one page buffer exist, thus only one page can be loaded with
 *       data and programmed into one page. If data needs to be written to
 *       different pages, the loading and writing needs to be repeated.
 *
 * \param  byte_addr EEPROM Byte address, between 0 and EEPROM_PAGE_SIZE.
 * \param  value     Byte value to write to buffer.
 */
void nvm_eeprom_load_byte_to_buffer(uint8_t byte_addr, uint8_t value)
{
	// Wait until NVM is ready
	nvm_wait_until_ready();

	eeprom_enable_mapping();
	*(uint8_t*)(byte_addr + MAPPED_EEPROM_START) = value;
	eeprom_disable_mapping();
}


/**
 * \brief Load entire page into temporary EEPROM page buffer.
 *
 * This function loads an entire EEPROM page from an SRAM buffer to
 * the EEPROM page buffers. If memory mapped EEPROM is enabled, this
 * function will not work. Make sure that the buffer is flushed before
 * starting to load bytes.
 *
 * \note Only the lower part of the address is used to address the buffer.
 *       Therefore, no address parameter is needed. In the end, the data
 *       is written to the EEPROM page given by the address parameter to the
 *       EEPROM write page operation.
 *
 * \param  values   Pointer to SRAM buffer containing an entire page.
 */
void nvm_eeprom_load_page_to_buffer(const uint8_t *values)
{
	// Wait until NVM is ready
	nvm_wait_until_ready();

	// Load multiple bytes into page buffer
	uint8_t i;
	for (i = 0; i < EEPROM_PAGE_SIZE; ++i) {
		nvm_eeprom_load_byte_to_buffer(i, *values);
		++values;
	}
}

/**
 * \brief Erase and write bytes from page buffer into EEPROM.
 *
 * This function writes the contents of an already loaded EEPROM page
 * buffer into EEPROM memory.
 *
 * As this is an atomic write, the page in EEPROM will be erased
 * automatically before writing. Note that only the page buffer locations
 * that have been loaded will be used when writing to EEPROM. Page buffer
 * locations that have not been loaded will be left untouched in EEPROM.
 *
 * \param  page_addr  EEPROM Page address, between 0 and EEPROM_SIZE/EEPROM_PAGE_SIZE
 */
void nvm_eeprom_atomic_write_page(uint8_t page_addr)
{
	// Wait until NVM is ready
	nvm_wait_until_ready();

	// Calculate page address
	uint16_t address = (uint16_t)(page_addr * EEPROM_PAGE_SIZE);

	Assert(address <= EEPROM_SIZE);

	// Set address
	NVM.ADDR2 = 0x00;
	NVM.ADDR1 = (address >> 8) & 0xFF;
	NVM.ADDR0 = address & 0xFF;

	// Issue EEPROM Atomic Write (Erase&Write) command
	nvm_issue_command(NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc);
}

/**
 * \brief Write (without erasing) EEPROM page.
 *
 * This function writes the contents of an already loaded EEPROM page
 * buffer into EEPROM memory.
 *
 * As this is a split write, the page in EEPROM will _not_ be erased
 * before writing.
 *
 * \param  page_addr  EEPROM Page address, between 0 and EEPROM_SIZE/EEPROM_PAGE_SIZE
 */
void nvm_eeprom_split_write_page(uint8_t page_addr)
{
	// Wait until NVM is ready
	nvm_wait_until_ready();

	// Calculate page address
	uint16_t address = (uint16_t)(page_addr * EEPROM_PAGE_SIZE);

	Assert(address <= EEPROM_SIZE);

	// Set address
	NVM.ADDR2 = 0x00;
	NVM.ADDR1 = (address >> 8) & 0xFF;
	NVM.ADDR0 = address & 0xFF;

	// Issue EEPROM Split Write command
	nvm_issue_command(NVM_CMD_WRITE_EEPROM_PAGE_gc);
}

/**
 * \brief Fill temporary EEPROM page buffer with value.
 *
 * This fills the the EEPROM page buffers with a given value.
 * If memory mapped EEPROM is enabled, this function will not work.
 *
 * \note Only the lower part of the address is used to address the buffer.
 *       Therefore, no address parameter is needed. In the end, the data
 *       is written to the EEPROM page given by the address parameter to the
 *       EEPROM write page operation.
 *
 * \param  value Value to copy to the page buffer.
 */
void nvm_eeprom_fill_buffer_with_value(uint8_t value)
{
	nvm_eeprom_flush_buffer();
	// Wait until NVM is ready
	nvm_wait_until_ready();
	// Load multiple bytes into page buffer
	uint8_t i;
	for (i = 0; i < EEPROM_PAGE_SIZE; ++i) {
		nvm_eeprom_load_byte_to_buffer(i, value);
	}
}

/**
 * \brief Erase bytes from EEPROM page.
 *
 * This function erases bytes from one EEPROM page, so that every location
 * written to in the page buffer reads 0xFF.
 *
 * \param page_addr EEPROM Page address, between 0 and EEPROM_SIZE/EEPROM_PAGE_SIZE
 */
void nvm_eeprom_erase_bytes_in_page(uint8_t page_addr)
{
	// Wait until NVM is ready
	nvm_wait_until_ready();

	// Calculate page address
	uint16_t address = (uint16_t)(page_addr * EEPROM_PAGE_SIZE);

	Assert(address <= EEPROM_SIZE);

	// Set address
	NVM.ADDR2 = 0x00;
	NVM.ADDR1 = (address >> 8) & 0xFF;
	NVM.ADDR0 = address & 0xFF;

	// Issue EEPROM Erase command
	nvm_issue_command(NVM_CMD_ERASE_EEPROM_PAGE_gc);
}

/**
 * \brief Erase EEPROM page.
 *
 * This function erases one EEPROM page, so that every location reads 0xFF.
 *
 * \param page_addr EEPROM Page address, between 0 and EEPROM_SIZE/EEPROM_PAGE_SIZE
 */
void nvm_eeprom_erase_page(uint8_t page_addr)
{
	// Mark all addresses to be deleted
	nvm_eeprom_fill_buffer_with_value(0xff);
	// Erase bytes
	nvm_eeprom_erase_bytes_in_page(page_addr);
}


/**
 * \brief Erase bytes from all EEPROM pages.
 *
 * This function erases bytes from all EEPROM pages, so that every location
 * written to in the page buffer reads 0xFF.
 */
void nvm_eeprom_erase_bytes_in_all_pages(void)
{
	// Wait until NVM is ready
	nvm_wait_until_ready();

	// Issue EEPROM Erase All command
	nvm_issue_command(NVM_CMD_ERASE_EEPROM_gc);
}

/**
 * \brief Erase entire EEPROM memory.
 *
 * This function erases the entire EEPROM memory block to 0xFF.
 */
void nvm_eeprom_erase_all(void)
{
	// Mark all addresses to be deleted
	nvm_eeprom_fill_buffer_with_value(0xff);
	// Erase all pages
	nvm_eeprom_erase_bytes_in_all_pages();
}

//! @}


//! @}


/**
 * \weakgroup nvm_flash_group
 * @{
 */

/**
 * \brief Issue flash range CRC command
 *
 * This function sets the FLASH range CRC command in the NVM.CMD register.
 * It then loads the start and end byte address of the part of FLASH to
 * generate a CRC-32 for into the ADDR and DATA registers and finally performs
 * the execute command.
 *
 * \note Should only be called from the CRC module. The function saves and
 *       restores the NVM.CMD register, but if this
 *       function is called from an interrupt, interrupts must be disabled
 *       before this function is called.
 *
 * \param start_addr  end byte address
 * \param end_addr    start byte address
 */
void nvm_issue_flash_range_crc(flash_addr_t start_addr, flash_addr_t end_addr)
{
	uint8_t old_cmd;
	// Save current nvm command
	old_cmd = NVM.CMD;

	// Load the NVM CMD register with the Flash Range CRC command
	NVM.CMD = NVM_CMD_FLASH_RANGE_CRC_gc;

	// Load the start byte address in the NVM Address Register
	NVM.ADDR0 = start_addr & 0xFF;
	NVM.ADDR1 = (start_addr >> 8) & 0xFF;
#if (FLASH_SIZE >= 0x10000UL)
	NVM.ADDR2 = (start_addr >> 16) & 0xFF;
#endif

	// Load the end byte address in NVM Data Register
	NVM.DATA0 = end_addr & 0xFF;
	NVM.DATA1 = (end_addr >> 8) & 0xFF;
#if (FLASH_SIZE >= 0x10000UL)
	NVM.DATA2 = (end_addr >> 16) & 0xFF;
#endif

	// Execute command
	ccp_write_io((uint8_t *)&NVM.CTRLA, NVM_CMDEX_bm);

	// Restore command register
	NVM.CMD = old_cmd;
}

/**
 * \brief Read buffer within the application section
 *
 * \param address	the address to where to read
 * \param buf		pointer to the data
 * \param len		the number of bytes to read
 */
void nvm_flash_read_buffer(flash_addr_t address, void *buf, uint16_t len)
{
#if (FLASH_SIZE>0x10000)
	uint32_t opt_address = address;
#else
	uint16_t opt_address = (uint16_t)address;
#endif
	nvm_wait_until_ready();
	while ( len ) {
		*(uint8_t*)buf = nvm_flash_read_byte(opt_address);
		buf=(uint8_t*)buf+1;
		opt_address++;
		len--;
	}
}

/**
 * \brief Read buffer within the user section
 *
 * \param address	the address to where to read
 * \param buf		pointer to the data
 * \param len		the number of bytes to read
 */
void nvm_user_sig_read_buffer(flash_addr_t address, void *buf, uint16_t len)
{
	uint16_t opt_address = (uint16_t)address&(FLASH_PAGE_SIZE-1);
	while ( len ) {
		*(uint8_t*)buf = nvm_read_user_signature_row(opt_address);
		buf=(uint8_t*)buf+1;
		opt_address++;
		len--;
	}
}

/**
 * \brief Write specific parts of user flash section
 *
 * \param address        the address to where to write
 * \param buf            pointer to the data
 * \param len            the number of bytes to write
 * \param b_blank_check  if True then the page flash is checked before write
 *                       to run or not the erase page command.
 *
 * Set b_blank_check to false if all application flash is erased before.
 */
void nvm_user_sig_write_buffer(flash_addr_t address, const void *buf,
	uint16_t len, bool b_blank_check)
{
	uint16_t w_value;
	uint16_t page_pos;
	uint16_t opt_address = (uint16_t)address;
	bool b_flag_erase = false;

	while ( len ) {
		for (page_pos=0; page_pos<FLASH_PAGE_SIZE; page_pos+=2 ) {
			if (b_blank_check) {
				// Read flash to know if the erase command is mandatory
				LSB(w_value) = nvm_read_user_signature_row(page_pos);
				MSB(w_value) = nvm_read_user_signature_row(page_pos+1);
				if (w_value!=0xFFFF) {
					b_flag_erase = true; // The page is not empty
				}
			}else{
				w_value = 0xFFFF;
			}
			// Update flash buffer
			if (len) {
				if (opt_address == page_pos) {
					// The MSB of flash word must be changed
					// because the address is even
					len--;
					opt_address++;
					LSB(w_value)=*(uint8_t*)buf;
					buf=(uint8_t*)buf+1;
				}
			}
			if (len) {
				if (opt_address == (page_pos+1)) {
					// The LSB of flash word must be changed
					// because the user buffer is not empty
					len--;
					opt_address++;
					MSB(w_value)=*(uint8_t*)buf;
					buf=(uint8_t*)buf+1;
				}
			}
			// Load flash buffer
			nvm_flash_load_word_to_buffer(page_pos,w_value);
		}
	}
	// Write flash buffer
	if (b_flag_erase) {
		nvm_flash_erase_user_section();
	}
	nvm_flash_write_user_page();
}

/**
 * \brief Erase and write specific parts of application flash section
 *
 * \param address        the address to where to write
 * \param buf            pointer to the data
 * \param len            the number of bytes to write
 * \param b_blank_check  if True then the page flash is checked before write
 *                       to run or not the erase page command.
 *
 * Set b_blank_check to false if all application flash is erased before.
 */
void nvm_flash_erase_and_write_buffer(flash_addr_t address, const void *buf,
	uint16_t len, bool b_blank_check)
{
	uint16_t w_value;
	uint16_t page_pos;
	bool b_flag_erase;
#if (FLASH_SIZE>0x10000)
	uint32_t page_address;
	uint32_t opt_address = address;
#else
	uint16_t page_address;
	uint16_t opt_address = (uint16_t)address;
#endif

	// Compute the start of the page to be modified
	page_address = opt_address-(opt_address%FLASH_PAGE_SIZE);

	// For each page
	while ( len ) {
		b_flag_erase = false;

		nvm_wait_until_ready();
		for (page_pos=0; page_pos<FLASH_PAGE_SIZE; page_pos+=2 ) {
			if (b_blank_check) {
				// Read flash to know if the erase command is mandatory
				w_value = nvm_flash_read_word(page_address);
				if (w_value!=0xFFFF) {
					b_flag_erase = true; // The page is not empty
				}
			}else{
				w_value = 0xFFFF;
			}

			// Update flash buffer
			if (len) {
				if (opt_address == page_address) {
					// The MSB of flash word must be changed
					// because the address is even
					len--;
					opt_address++;
					LSB(w_value)=*(uint8_t*)buf;
					buf=(uint8_t*)buf+1;
				}
			}
			if (len) {
				if (opt_address == (page_address+1)) {
					// The LSB of flash word must be changed
					// because the user buffer is not empty
					len--;
					opt_address++;
					MSB(w_value)=*(uint8_t*)buf;
					buf=(uint8_t*)buf+1;
				}
			}
			// Load flash buffer
			nvm_flash_load_word_to_buffer(page_address,w_value);
			page_address+=2;
		}

		// Write flash buffer
		if (b_flag_erase) {
			nvm_flash_atomic_write_app_page(page_address-FLASH_PAGE_SIZE);
		}else{
			nvm_flash_split_write_app_page(page_address-FLASH_PAGE_SIZE);
		}
	}
}

//! @}

/**
 * \weakgroup nvm_fuse_lock_group
 * @{
 */

/**
 * \brief Read a fuse byte.
 *
 * This function reads and returns the value of a given fuse byte.
 *
 * \param fuse Fuse byte to read.
 *
 * \return  Byte value of fuse.
 */
uint8_t nvm_fuses_read(enum fuse_byte_t fuse)
{
	// Wait until NVM is ready
	nvm_wait_until_ready();

	// Set address
	NVM.ADDR0 = fuse;

	// Issue READ_FUSES command
	nvm_issue_command(NVM_CMD_READ_FUSES_gc);

	return NVM.DATA0;
}

//! @}
