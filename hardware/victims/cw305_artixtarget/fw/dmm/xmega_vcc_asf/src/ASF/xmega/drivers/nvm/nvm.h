/**
 * \file
 *
 * \brief Non Volatile Memory controller driver
 *
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef NVM_H
#define NVM_H

#include <compiler.h>
#include <ccp.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup nvm_group NVM driver
 *
 * See \ref xmega_nvm_quickstart
 *
 * \brief Low-level driver implementation for the AVR XMEGA Non Volatile
 *        Memory Controller (NVM).
 *
 * The XMEGA NVM controller interfaces the internal non-volatile memories
 * in the XMEGA devices. Program memory, EEPROM and signature row is can be
 * interfaced by the module. See the documentation of each sub-module for
 * more information.
 *
 * \note If using GCC and the flash sub-module, remember to configure
 *       the boot section in the make file. More information in the sub-module
 *       documentation.
 *
 * \section xmega_nvm_quickstart_section Quick Start Guide
 * See \ref xmega_nvm_quickstart
 */

/**
 * \defgroup nvm_generic_group NVM driver generic module handling
 * \ingroup nvm_group
 * \brief Support functions for the NVM driver.
 *
 * These functions are helper functions for the functions of the
 * \ref nvm_group "NVM driver".
 *
 * @{
 */

/**
 * \brief Wait for any NVM access to finish.
 *
 * This function is blocking and waits for any NVM access to finish.
 * Use this function before any NVM accesses, if you are not certain that
 * any previous operations are finished yet.
 */
static inline void nvm_wait_until_ready( void )
{
	do {
		// Block execution while waiting for the NVM to be ready
	} while ((NVM.STATUS & NVM_NVMBUSY_bm) == NVM_NVMBUSY_bm);
}

/**
 * \brief Non-Volatile Memory Execute Command
 *
 * This function sets the CCP register before setting the CMDEX bit in the
 * NVM.CTRLA register.
 *
 * \note The correct NVM command must be set in the NVM.CMD register before
 *       calling this function.
 */
static inline void nvm_exec(void)
{
	ccp_write_io((uint8_t *)&NVM.CTRLA, NVM_CMDEX_bm);
}

/**
 * \brief Non-Volatile Memory Execute Specific Command
 *
 * This function sets a command in the NVM.CMD register, then performs an
 * execute command by writing the CMDEX bit to the NVM.CTRLA register.
 *
 * \note The function saves and restores the NVM.CMD register, but if this
 *       function is called from an interrupt, interrupts must be disabled
 *       before this function is called.
 *
 * \param nvm_command NVM Command to execute.
 */
static inline void nvm_issue_command(NVM_CMD_t nvm_command)
{
	uint8_t old_cmd;

	old_cmd = NVM.CMD;
	NVM.CMD = nvm_command;
	ccp_write_io((uint8_t *)&NVM.CTRLA, NVM_CMDEX_bm);
	NVM.CMD = old_cmd;
}

/**
 * \brief Read one byte using the LDI instruction
 * \internal
 *
 * This function sets the specified NVM_CMD, reads one byte using at the
 * specified byte address with the LPM instruction. NVM_CMD is restored after
 * use.
 *
 * \note Interrupts should be disabled before running this function
 *       if program memory/NVM controller is accessed from ISRs.
 *
 * \param nvm_cmd NVM command to load before running LPM
 * \param address Byte offset into the signature row
 */
uint8_t nvm_read_byte(uint8_t nvm_cmd, uint16_t address);


/**
 * \brief Perform SPM write
 * \internal
 *
 * This function sets the specified NVM_CMD, sets CCP and then runs the SPM
 * instruction to write to flash.
 *
 * \note Interrupts should be disabled before running this function
 *       if program memory/NVM controller is accessed from ISRs.
 *
 * \param addr Address to perform the SPM on.
 * \param nvm_cmd NVM command to use in the NVM_CMD register
 */
void nvm_common_spm(uint32_t addr, uint8_t nvm_cmd);

//! @}

/**
 * \defgroup nvm_signature_group NVM driver signature handling
 * \ingroup nvm_group
 * \brief Handling of signature rows
 *
 * Functions for handling signature rows. The following is supported:
 * - Reading values from production and user signature row
 * - Reading device id
 * - Reading device revision
 * - Reading device serial
 *
 * \note Some of these functions are modifying the NVM.CMD register.
 *       If the application are using program space access in interrupts
 *       (__flash pointers in IAR EW or pgm_read_byte in GCC) interrupts
 *       needs to be disabled when running EEPROM access functions. If not,
 *       the program space reads will be corrupted. See documentation for
 *       each individual function.
 * \note Do not use the functions of this module in an interrupt service
 *       routine (ISR), since the functions can take several milliseconds to
 *       complete and hence block the interrupt for several milliseconds.
 *       In addition the functions of this module are modifying the page buffer
 *       which will corrupt any ongoing EEPROM handing used outside an ISR.
 * @{
 */

/**
 * \brief Structure containing the device ID
 *
 * This structure can be used to store the device ID of a device.
 */
struct nvm_device_id {
	union {
		struct {
			uint8_t devid0;
			uint8_t devid1;
			uint8_t devid2;
		};
		uint8_t byte[3];
	};
};

/**
 * \brief Structure containing the device serial
 *
 * This structure can be used to store the serial number of a device.
 */
struct nvm_device_serial {
	union {
		struct {
			uint8_t lotnum0;
			uint8_t lotnum1;
			uint8_t lotnum2;
			uint8_t lotnum3;
			uint8_t lotnum4;
			uint8_t lotnum5;
			uint8_t wafnum;
			uint8_t coordx0;
			uint8_t coordx1;
			uint8_t coordy0;
			uint8_t coordy1;
		};
		uint8_t byte[11];
	};
};

/**
 * \brief Get offset of calibration bytes in the production signature row
 *
 * \param regname Name of register within the production signature row
 * \retval Offset of register into the production signature row
 */
#if defined(__GNUC__)
# define nvm_get_production_signature_row_offset(regname) \
		offsetof(NVM_PROD_SIGNATURES_t, regname)
#elif defined(__ICCAVR__)
# define nvm_get_production_signature_row_offset(regname) (regname##_offset)
#else
#  error Unknown compiler
#endif


/**
 * \brief Read one byte from the production signature row
 *
 * This function reads one byte from the production signature row of the device
 * at the given address.
 *
 * \note This function is modifying the NVM.CMD register.
 *       If the application are using program space access in interrupts
 *       (__flash pointers in IAR EW or pgm_read_byte in GCC) interrupts
 *       needs to be disabled when running EEPROM access functions. If not
 *       the program space reads will be corrupted.
 *
 * \param address Byte offset into the signature row
 */
static inline uint8_t nvm_read_production_signature_row(uint8_t address)
{
	return nvm_read_byte(NVM_CMD_READ_CALIB_ROW_gc, address);
}

/**
 * \brief Read one byte from the user signature row
 *
 * This function reads one byte from the user signature row of the device
 * at the given address.
 *
 * \note This function is modifying the NVM.CMD register.
 *       If the application are using program space access in interrupts
 *       (__flash pointers in IAR EW or pgm_read_byte in GCC) interrupts
 *       needs to be disabled when running EEPROM access functions. If not
 *       the program space reads will be corrupted.
 *
 * \param address Byte offset into the signature row
 */
static inline uint8_t nvm_read_user_signature_row(uint16_t address)
{
	return nvm_read_byte(NVM_CMD_READ_USER_SIG_ROW_gc, address);
}

/**
 * \brief Read the device id
 *
 * This function returns the device ID stored in the device.
 *
 * \retval storage Pointer to the structure where to store the device id
 */
static inline void nvm_read_device_id(struct nvm_device_id *storage)
{
	storage->devid0 = MCU.DEVID0;
	storage->devid1 = MCU.DEVID1;
	storage->devid2 = MCU.DEVID2;
}

/**
 * \brief Read the device revision
 *
 * This function returns the device revision stored in the device.
 *
 * \retval unsigned 8 bit value with the current device revision.
 */
static inline uint8_t nvm_read_device_rev(void)
{
	return MCU.REVID;
}

void nvm_read_device_serial(struct nvm_device_serial *storage);

//! @}


/**
 * \defgroup nvm_eeprom_group NVM driver EEPROM handling
 * \ingroup nvm_group
 * \brief Functions for handling internal EEPROM memory.
 *
 * The internal EEPROM can be used to store data that will persist after
 * power is removed. This can typically be used to store calibration data,
 * application state, encryption keys or other data that need to be preserved
 * when power is removed.
 *
 * The functions in this module uses IO register access to manipulate the
 * EEPROM.
 *
 * \note The functions in this module are modifying the NVM.CMD register.
 *       If the application are using program space access in interrupts
 *       (__flash pointers in IAR EW or pgm_read_byte in GCC) interrupts
 *       needs to be disabled when running EEPROM access functions. If not
 *       the program space reads will be corrupted.
 * @{
 */

#ifndef EEPROM_PAGE_SIZE
#  if XMEGA_A || XMEGA_AU || XMEGA_B || XMEGA_C || XMEGA_D || XMEGA_E
#    define EEPROM_PAGE_SIZE 32
#  else
#    error Unknown EEPROM page size
#  endif
#endif

#ifndef CONFIG_NVM_IGNORE_XMEGA_A3_D3_REVB_ERRATA
#  if XMEGA_A3 || XMEGA_D3
#    error This NVM driver does not support rev B of XMEGA A3/D3 devices. \
     Set CONFIG_NVM_IGNORE_XMEGA_A3_D3_REVB_ERRATA to disable this message
#  endif
#endif

/**
 * Data type for holding eeprom memory addresses.
 */
typedef uint16_t eeprom_addr_t;


/*! \brief Enable EEPROM mapping into data space.
 *
 *  This macro enables mapping of EEPROM into data space.
 *  EEPROM starts at EEPROM_START in data memory. Read access
 *  can be done similar to ordinary SRAM access.
 *
 *  \note This disables IO-mapped access to EEPROM, although page erase and
 *        write operations still needs to be done through IO register.
 */
static inline void eeprom_enable_mapping(void)
{
#if !XMEGA_E
	NVM_CTRLB = NVM_CTRLB | NVM_EEMAPEN_bm;
#endif
}


/*! \brief Disable EEPROM mapping into data space.
 *
 *  This macro disables mapping of EEPROM into data space.
 *  IO mapped access is now enabled.
 */
static inline void eeprom_disable_mapping(void)
{
#if !XMEGA_E
	NVM_CTRLB = NVM_CTRLB & ~NVM_EEMAPEN_bm;
#endif
}


uint8_t nvm_eeprom_read_byte(eeprom_addr_t addr);
void nvm_eeprom_write_byte(eeprom_addr_t address, uint8_t value);
void nvm_eeprom_read_buffer(eeprom_addr_t address, void *buf, uint16_t len);
void nvm_eeprom_erase_and_write_buffer(eeprom_addr_t address, const void *buf, uint16_t len);

void nvm_eeprom_flush_buffer(void);
void nvm_eeprom_load_byte_to_buffer(uint8_t byte_addr, uint8_t value);
void nvm_eeprom_load_page_to_buffer(const uint8_t *values);
void nvm_eeprom_atomic_write_page(uint8_t page_addr);
void nvm_eeprom_split_write_page(uint8_t page_addr);
void nvm_eeprom_fill_buffer_with_value(uint8_t value);
void nvm_eeprom_erase_bytes_in_page(uint8_t page_addr);
void nvm_eeprom_erase_page(uint8_t page_addr);
void nvm_eeprom_erase_bytes_in_all_pages(void);
void nvm_eeprom_erase_all(void);

//! @}

/**
 * \defgroup nvm_flash_group NVM driver flash handling
 * \ingroup nvm_group
 * \brief Functions for handling internal flash memory.
 *
 * The internal flash memory on the XMEGA devices consists of the application
 * section, the application table section and the bootloader section.
 * All these sections can store program code for the MCU, but if there is
 * available space, they can be used for storing other persistent data.
 *
 * Writing the flash memory can only be done one page at a time. It consists
 * of loading the data to the internal page buffer and then running one of
 * the write commands. If the page has not been erased before writing, the
 * data will not be written correctly.
 *
 * In order to be able to write to flash memory the programming commands need
 * to be run from the boot section.
 * - When using IAR this is handled automatically by the linker script.
 * - When using GCC this needs to be specified manually in the make files. For
 *   example the ATxmega128A1 has the boot section at the word address 0x10000
 *   the corresponding byte address of 0x20000 needs to be added to the
 *   config.mk makefile:
 *   LDFLAGS += -Wl,--section-start=.BOOT=0x20000
 *   See the device datasheet for the correct address for other devices.
 *
 * \note If using GCC and the flash sub-module, remember to configure
 *       the boot section in the make file.
 *
 * \note The functions in this module are modifying the NVM.CMD register.
 *       If the application are using program space access in interrupts
 *       (__flash pointers in IAR EW or pgm_read_byte in GCC) interrupts
 *       needs to be disabled when running EEPROM access functions. If not
 *       the program space reads will be corrupted.
 * @{
 */

/**
 * \brief Size of a flash page in bytes
 *
 * The page size in bytes taken from the toolchain header files.
 *
 * \note Page size is currently missing from the IAR header files, so it needs
 *       to be defined in the driver until it is fixed.
 */
#ifdef __DOXYGEN__
#  define FLASH_SIZE
#  define FLASH_PAGE_SIZE
#else

// 8K devices
#  if AVR8_PART_IS_DEFINED(ATxmega8E5)
#    define FLASH_SIZE      (8*1024L)
#    define FLASH_PAGE_SIZE (128)

// 16K devices
#  elif AVR8_PART_IS_DEFINED(ATxmega16A4)            | \
		AVR8_PART_IS_DEFINED(ATxmega16A4U) | \
		AVR8_PART_IS_DEFINED(ATxmega16D4)  | \
		AVR8_PART_IS_DEFINED(ATxmega16C4)
#    define FLASH_SIZE      (16*1024L)
#    define FLASH_PAGE_SIZE (256)

#  elif AVR8_PART_IS_DEFINED(ATxmega16E5)
#    define FLASH_SIZE      (16*1024L)
#    define FLASH_PAGE_SIZE (128)

// 32K devices
#  elif AVR8_PART_IS_DEFINED(ATxmega32A4)          | \
		AVR8_PART_IS_DEFINED(ATxmega32A4U) | \
		AVR8_PART_IS_DEFINED(ATxmega32D4)  | \
		AVR8_PART_IS_DEFINED(ATxmega32C4)
#    define FLASH_SIZE      (32*1024L)
#    define FLASH_PAGE_SIZE (256)

#  elif AVR8_PART_IS_DEFINED(ATxmega32E5)
#    define FLASH_SIZE      (32*1024L)
#    define FLASH_PAGE_SIZE (128)

// 64K devices
#  elif AVR8_PART_IS_DEFINED(ATxmega64A1)          | \
		AVR8_PART_IS_DEFINED(ATxmega64A1U) | \
		AVR8_PART_IS_DEFINED(ATxmega64A3)  | \
		AVR8_PART_IS_DEFINED(ATxmega64A3U) | \
		AVR8_PART_IS_DEFINED(ATxmega64A4U) | \
		AVR8_PART_IS_DEFINED(ATxmega64B1)  | \
		AVR8_PART_IS_DEFINED(ATxmega64B3)  | \
		AVR8_PART_IS_DEFINED(ATxmega64C3)  | \
		AVR8_PART_IS_DEFINED(ATxmega64D3)  | \
		AVR8_PART_IS_DEFINED(ATxmega64D4)
#    define FLASH_SIZE      (64*1024L)
#    define FLASH_PAGE_SIZE (256)

// 128K devices
#  elif AVR8_PART_IS_DEFINED(ATxmega128A1)          | \
		AVR8_PART_IS_DEFINED(ATxmega128A1U) | \
		AVR8_PART_IS_DEFINED(ATxmega128A3)  | \
		AVR8_PART_IS_DEFINED(ATxmega128A3U) | \
		AVR8_PART_IS_DEFINED(ATxmega128C3) | \
		AVR8_PART_IS_DEFINED(ATxmega128D3)  | \
		AVR8_PART_IS_DEFINED(ATxmega128D4)
#    define FLASH_SIZE      (128*1024L)
#    define FLASH_PAGE_SIZE (512)

#  elif AVR8_PART_IS_DEFINED(ATxmega128A4U)         | \
		AVR8_PART_IS_DEFINED(ATxmega128B1)  | \
		AVR8_PART_IS_DEFINED(ATxmega128B3)
#    define FLASH_SIZE      (128*1024L)
#    define FLASH_PAGE_SIZE (256)

// 192K devices
#  elif AVR8_PART_IS_DEFINED(ATxmega192A3U) | \
		AVR8_PART_IS_DEFINED(ATxmega192C3)  | \
		AVR8_PART_IS_DEFINED(ATxmega192D3)
#    define FLASH_SIZE      (192*1024L)
#    define FLASH_PAGE_SIZE (512)

// 256K devices
#  elif AVR8_PART_IS_DEFINED(ATxmega256A3)           | \
		AVR8_PART_IS_DEFINED(ATxmega256A3U)  | \
		AVR8_PART_IS_DEFINED(ATxmega256A3B)  | \
		AVR8_PART_IS_DEFINED(ATxmega256A3BU) | \
		AVR8_PART_IS_DEFINED(ATxmega256C3)   | \
		AVR8_PART_IS_DEFINED(ATxmega256D3)
#    define FLASH_SIZE      (256*1024L)
#    define FLASH_PAGE_SIZE (512)

// 384K devices
#  elif AVR8_PART_IS_DEFINED(ATxmega384C3)
#    define FLASH_SIZE      (384*1024L)
#    define FLASH_PAGE_SIZE (512)
#  elif AVR8_PART_IS_DEFINED(ATxmega384D3)
#    define FLASH_SIZE      (384*1024L)
#    define FLASH_PAGE_SIZE (512)
#  else
#    error Flash page size needs to be defined.
#  endif
#endif

/**
 * Data type for holding flash memory addresses.
 *
 */
#if (FLASH_SIZE >= 0x10000UL) // Note: Xmega with 64KB of flash have 4KB boot flash
typedef uint32_t flash_addr_t;
#else
typedef uint16_t flash_addr_t;
#endif

/**
 * Flash pointer type to use for accessing flash memory with IAR
 */
#if (FLASH_SIZE >= 0x10000UL) // Note: Xmega with 64KB of flash have 4KB boot flash
#  define IAR_FLASH_PTR __farflash
#else
#  define IAR_FLASH_PTR __flash
#endif

/**
 * \brief Load byte from flash memory
 *
 * Load one word of flash using byte addressing. IAR has __flash pointers
 * and GCC have pgm_read_byte_xx functions which load data from flash memory.
 * This function used for compatibility between the compilers.
 *
 * \param addr Byte address to load
 * \return Byte from program memory
 */
static inline uint8_t nvm_flash_read_byte(flash_addr_t addr)
{
#if defined(__GNUC__)
	return pgm_read_byte_far(addr);
#elif defined(__ICCAVR__)
	uint8_t IAR_FLASH_PTR *flashptr = (uint8_t IAR_FLASH_PTR *)addr;
	return *flashptr;
#else
#  error Unknown compiler
#endif
}

/**
 * \brief Load word from flash memory
 *
 * Load one word of flash using byte addressing. IAR has __flash pointers
 * and GCC have pgm_read_byte_xx functions which load data from flash memory.
 * This function used for compatibility between the compilers.
 *
 * \param addr Byte address to load (last bit is ignored)
 * \return Word from program memory
 */
static inline uint16_t nvm_flash_read_word(flash_addr_t addr)
{
#if defined(__GNUC__)
	return pgm_read_word_far(addr);
#elif defined(__ICCAVR__)
	uint16_t IAR_FLASH_PTR *flashptr = (uint16_t IAR_FLASH_PTR *)addr;
	return *flashptr;
#endif
}


/**
 * \brief Flush flash page buffer
 *
 * Clear the NVM controller page buffer for flash. This needs to be called
 * before using \ref nvm_flash_load_word_to_buffer if it has not already been
 * cleared.
 *
 */
static inline void nvm_flash_flush_buffer(void)
{
	nvm_wait_until_ready();
	nvm_common_spm(0, NVM_CMD_ERASE_FLASH_BUFFER_gc);
}


/**
 * \brief Load word into flash page buffer
 *
 * Clear the NVM controller page buffer for flash. This needs to be called
 * before using \ref nvm_flash_load_word_to_buffer if it has not already been
 * cleared.
 *
 * \param word_addr Address to store data. The upper bits beyond the page size
 *                  is ignored. \ref FLASH_PAGE_SIZE
 * \param data Data word to load into the page buffer
 */
void nvm_flash_load_word_to_buffer(uint32_t word_addr, uint16_t data);


/**
 * \brief Erase entire application section
 *
 * Erase all of the application section.
 */
static inline void nvm_flash_erase_app(void)
{
	nvm_wait_until_ready();
	nvm_common_spm(0, NVM_CMD_ERASE_APP_gc);
}

/**
 * \brief Erase a page within the application section
 *
 * Erase one page within the application section
 *
 * \param page_addr Byte address to the page to delete
 */
static inline void nvm_flash_erase_app_page(flash_addr_t page_addr)
{
	nvm_wait_until_ready();
	nvm_common_spm(page_addr, NVM_CMD_ERASE_APP_PAGE_gc);
}

/**
 * \brief Write a page within the application section
 *
 * Write a page within the application section with the data stored in the
 * page buffer. The page needs to be erased before the write to avoid
 * corruption of the data written.
 *
 * \param page_addr Byte address to the page to delete
 */
static inline void nvm_flash_split_write_app_page(flash_addr_t page_addr)
{
	nvm_wait_until_ready();
	nvm_common_spm(page_addr, NVM_CMD_WRITE_APP_PAGE_gc);
}

/**
 * \brief Erase and write a page within the application section
 *
 * Erase and the write a page within the application section with the data
 * stored in the page buffer. Erase and write is done in an atomic operation.
 *
 * \param page_addr Byte address to the page to delete
 */
static inline void nvm_flash_atomic_write_app_page(flash_addr_t page_addr)
{
	nvm_wait_until_ready();
	nvm_common_spm(page_addr, NVM_CMD_ERASE_WRITE_APP_PAGE_gc);
}

void nvm_issue_flash_range_crc(flash_addr_t start_addr, flash_addr_t end_addr);

void nvm_flash_read_buffer(flash_addr_t address, void *buf, uint16_t len);

void nvm_flash_erase_and_write_buffer(flash_addr_t address, const void *buf,
	uint16_t len, bool b_blank_check);

/**
 * \brief Erase a page within the boot section
 *
 * Erase one page within the boot section
 *
 * \param page_addr Byte address to the page to delete
 */
static inline void nvm_flash_erase_boot_page(flash_addr_t page_addr)
{
	nvm_wait_until_ready();
	nvm_common_spm(page_addr, NVM_CMD_ERASE_BOOT_PAGE_gc);
}

/**
 * \brief Write a page within the boot section
 *
 * Write a page within the boot section with the data stored in the
 * page buffer. The page needs to be erased before the write to avoid
 * corruption of the data written.
 *
 * \param page_addr Byte address to the page to delete
 */
static inline void nvm_flash_split_write_boot_page(flash_addr_t page_addr)
{
	nvm_wait_until_ready();
	nvm_common_spm(page_addr, NVM_CMD_WRITE_BOOT_PAGE_gc);
}

/**
 * \brief Erase and write a page within the boot section
 *
 * Erase and the write a page within the boot section with the data
 * stored in the page buffer. Erase and write is done in an atomic operation.
 *
 * \param page_addr Byte address to the page to delete
 */
static inline void nvm_flash_atomic_write_boot_page(flash_addr_t page_addr)
{
	nvm_wait_until_ready();
	nvm_common_spm(page_addr, NVM_CMD_ERASE_WRITE_BOOT_PAGE_gc);
}

void nvm_user_sig_read_buffer(flash_addr_t address, void *buf, uint16_t len);
void nvm_user_sig_write_buffer(flash_addr_t address, const void *buf,
	uint16_t len, bool b_blank_check);

/**
 * \brief Erase the user calibration section page
 *
 * Erase the user calibration section page. There is only one page, so no
 * parameters are needed.
 */
static inline void nvm_flash_erase_user_section(void)
{
	nvm_wait_until_ready();
	nvm_common_spm(0, NVM_CMD_ERASE_USER_SIG_ROW_gc);
}

/**
 * \brief Write the user calibration section page
 *
 * Write a the user calibration section page with the data stored in the
 * page buffer. The page needs to be erased before the write to avoid
 * corruption of the data written. There is only one page, so no
 * parameters are needed.
 */
static inline void nvm_flash_write_user_page(void)
{
	nvm_wait_until_ready();
	nvm_common_spm(0, NVM_CMD_WRITE_USER_SIG_ROW_gc);
}

//! @}

/**
 * \defgroup nvm_fuse_lock_group NVM driver fuse and lock bits handling
 * \ingroup nvm_group
 * \brief Functions for reading fuses and writing lock bits.
 *
 * The Fuses are used to set important system functions and can only be written
 * from an external programming interface. The application software can read
 * the fuses. The fuses are used to configure reset sources such as Brown-out
 * Detector and Watchdog, Start-up configuration, JTAG enable and JTAG user ID.
 *
 * The Lock bits are used to set protection level on the different flash
 * sections. They are used to block read and/or write on the different flash
 * sections. Lock bits can be written from en external programmer and from the
 * application software to set a more strict protection level, but not to set a
 * less strict protection level. Chip erase is the only way to erase the lock
 * bits. The lock bits are erased after the rest of the flash memory is erased.
 * An unprogrammed fuse or lock bit will have the value one, while a programmed
 * fuse or lock bit will have the value zero.
 * Both fuses and lock bits are reprogrammable like the Flash Program memory.
 *
 * \note The functions in this module are modifying the NVM.CMD register.
 *       If the application are using program space access in interrupts
 *       (__flash pointers in IAR EW or pgm_read_byte in GCC) interrupts
 *       needs to be disabled when running EEPROM access functions. If not
 *       the program space reads will be corrupted.
 * @{
 */

// The different fuse bytes
enum fuse_byte_t {
	FUSEBYTE0 = 0,
	FUSEBYTE1 = 1,
	FUSEBYTE2 = 2,
	FUSEBYTE3 = 3, // not used on current devices
	FUSEBYTE4 = 4,
	FUSEBYTE5 = 5,
};

uint8_t nvm_fuses_read(enum fuse_byte_t fuse);

/**
 * \brief Program the lock bits.
 *
 * Program the lock bits to the given values. Lock bits can only be programmed
 * to a more secure setting than previously programmed. To clear lock bits, a
 * flash erase has to be issued.
 *
 * \param blbb_lock Boot loader section lock bits to program
 * \param blba_lock Application section lock bits to program
 * \param blbat_lock Application table section lock bits to program
 * \param lb_lock Flash/eeprom lock bits to program
 */
static inline void nvm_lock_bits_write(enum NVM_BLBB_enum blbb_lock,
	enum NVM_BLBA_enum blba_lock, enum NVM_BLBAT_enum blbat_lock,
	enum NVM_LB_enum lb_lock)
{
	nvm_wait_until_ready();
	NVM.DATA0 = (uint8_t)blbb_lock | (uint8_t)blba_lock | (uint8_t)blbat_lock |
		(uint8_t)lb_lock;
	nvm_issue_command(NVM_CMD_WRITE_LOCK_BITS_gc);
}

/**
 * \brief Program the BLBB lock bits.
 *
 * Program the lock bits for the boot loader section (BLBB). Other lock bits
 * (BLBA, BLBAT and LB) are not altered (ie. programmed to NOLOCK).
 *
 * \param blbb_lock Boot loader section lock bits to program
 */
static inline void nvm_blbb_lock_bits_write(enum NVM_BLBB_enum blbb_lock)
{
	nvm_lock_bits_write(blbb_lock, NVM_BLBA_NOLOCK_gc, NVM_BLBAT_NOLOCK_gc,
		NVM_LB_NOLOCK_gc);
}

/**
 * \brief Program the BLBA lock bits.
 *
 * Program the lock bits for the application section (BLBA). Other lock bits
 * (BLBB, BLBAT and LB) are not altered (ie. programmed to NOLOCK).
 *
 * \param blba_lock Application section lock bits to program
 */
static inline void nvm_blba_lock_bits_write(enum NVM_BLBA_enum blba_lock)
{
	nvm_lock_bits_write(NVM_BLBB_NOLOCK_gc, blba_lock, NVM_BLBAT_NOLOCK_gc,
		NVM_LB_NOLOCK_gc);
}

/**
 * \brief Program the BLBAT lock bits.
 *
 * Program the lock bits for the application table section (BLBAT). Other lock
 * bits (BLBB, BLBA and LB) are not altered (ie. programmed to NOLOCK).
 *
 * \param blbat_lock Application table section lock bits to program
 */
static inline void nvm_blbat_lock_bits_write(enum NVM_BLBAT_enum blbat_lock)
{
	nvm_lock_bits_write(NVM_BLBB_NOLOCK_gc, NVM_BLBA_NOLOCK_gc, blbat_lock,
		NVM_LB_NOLOCK_gc);
}

/**
 * \brief Program the LB lock bits.
 *
 * Program the lock bits for the flash and eeprom (LB). Other lock bits
 * (BLBB, BLBA and BLBAT) are not altered (ie. programmed to NOLOCK).
 *
 * \param lb_lock Flash/eeprom lock bits to program
 */
static inline void nvm_lb_lock_bits_write(enum NVM_LB_enum lb_lock)
{
	nvm_lock_bits_write(NVM_BLBB_NOLOCK_gc, NVM_BLBA_NOLOCK_gc,
		NVM_BLBAT_NOLOCK_gc, lb_lock);
}

//! @}

/**
 * \page xmega_nvm_quickstart Quick Start Guide for the XMEGA NVM Driver
 *
 * This is the quick start guide for the \ref nvm_group "NVM Driver", with
 * step-by-step instructions on how to configure and use the driver for
 * specific use cases.
 *
 * The section described below can be compiled into e.g. the main application
 * loop or any other function that will need to interface non-volatile memory.
 *
 * \section xmega_nvm_quickstart_basic Basic usage of the NVM driver
 * This section will present three use cases of the NVM driver. The first will
 * write a page to EEPROM and verify that it has been written, the second will
 * access the BOD-level fuse to verify that the level is correctly set, and the
 * third will read a chunk from the user signature row.
 *
 * \section xmega_nvm_quickstart_eeprom_case Use case 1: EEPROM
 *
 * The NVM driver has functions for interfacing many types of non-volatile
 * memory, including flash, EEPROM, fuses and lock bits. The example code
 * below will write a page to the internal EEPROM, and read it back to verify,
 * using memory mapped I/O.
 *
 * \section xmega_nvm_quickstart_eeprom_case_setup_steps Setup steps
 * There are no setup steps required for this use case.
 *
 * \subsection nvm_quickstart_eeprom_case_example_code Example code
 *
 * \code
	 #define EXAMPLE_PAGE 2
	 #define EXAMPLE_ADDR EXAMPLE_PAGE * EEPROM_PAGE_SIZE

	 uint8_t write_page[EEPROM_PAGE_SIZE];
	 uint8_t read_page[EEPROM_PAGE_SIZE];

	 fill_page_with_known_data(write_page);
	 fill_page_with_zeroes(read_page);

	 nvm_eeprom_load_page_to_buffer(write_page);
	 nvm_eeprom_atomic_write_page(EXAMPLE_PAGE);

	 nvm_eeprom_read_buffer(EXAMPLE_ADDR,
	         read_page, EEPROM_PAGE_SIZE);

	 check_if_pages_are_equal(write_page, read_page);
\endcode
 *
 * \subsection nvm_quickstart_eeprom_case_workflow Workflow
 *
 * -# We define where we would like to store our data, and we arbitrarily
 *    choose page 2 of EEPROM:
 *     - \code
	#define EXAMPLE_PAGE 2
	#define EXAMPLE_ADDR EXAMPLE_PAGE * EEPROM_PAGE_SIZE
\endcode
 * -# Define two tables, one which contains the data which we will write,
 *    and one which we will read the data into:
 *     - \code
	uint8_t write_page[EEPROM_PAGE_SIZE];
	uint8_t read_page[EEPROM_PAGE_SIZE];
\endcode
 * -# Fill the tables with our data, and zero out the read table:
 *     - \code
	fill_page_with_known_data(write_page);
	fill_page_with_zeroes(read_page);
\endcode
 *     - \note These functions are undeclared, you should replace them with
 *             your own appropriate functions.
 * -# We load our page into a temporary EEPROM page buffer:
 *     - \code
	nvm_eeprom_load_page_to_buffer(write_page);
\endcode
 *     - \attention The function used above will not work if memory mapping
 *                  is enabled.
 * -# Do an atomic write of the page from buffer into the specified page:
 *     - \code
	nvm_eeprom_atomic_write_page(EXAMPLE_PAGE);
\endcode
 *     - \note The function \ref nvm_eeprom_atomic_write_page() erases the
 *             page before writing the new one. For non-atomic (split)
 *             writing without deleting, see \ref nvm_eeprom_split_write_page()
 * -# Read the page back into our read_page[] table:
 *     - \code
	nvm_eeprom_read_buffer(EXAMPLE_ADDR,
	        read_page, EEPROM_PAGE_SIZE);
\endcode
 * -# Verify that the page is equal to the one that was written earlier:
 *     - \code
	check_if_pages_are_equal(write_page, read_page);
\endcode
 *     - \note This function is not declared, you should replace it with your
 *             own appropriate function.
 *
 * \section xmega_nvm_quickstart_fuse_case Use case 2: Fuses
 *
 * The NVM driver has functions for reading fuses.
 * See \ref nvm_fuse_lock_group.
 *
 * We would like to check whether the Brown-out Detection level is set to
 * 2.1V. This is set by programming the fuses when the chip is connected
 * to a suitable programmer. The fuse is a part of FUSEBYTE5. If the BODLVL
 * is correct, we turn on LED0.
 *
 * \section xmega_nvm_quickstart_fuse_case_setup_steps Setup steps
 * There are no setup steps required for this use case.
 *
 * \subsection nvm_quickstart_fuse_case_example_code Example code
 * \code
	 uint8_t fuse_value;
	 fuse_value = nvm_fuses_read(FUSEBYTE5);

	 if ((fuse_value & NVM_FUSES_BODLVL_gm) == BODLVL_2V1_gc) {
	     gpio_set_pin_low(LED0_GPIO);
	 }
\endcode
 *
 * \subsection nvm_quickstart_fuse_case_workflow Workflow
 *
 * -# Create a variable to store the fuse contents:
 *     - \code
	uint8_t fuse_value;
\endcode
 * -# The fuse value we are interested in, BODLVL, is stored in FUSEBYTE5.
 *    We call the function \ref nvm_fuses_read() to read the fuse into our
 *    variable:
 *     - \code
	fuse_value = nvm_fuses_read(FUSEBYTE5);
\endcode
 * -# This ends the reading portion, but we would like to see whether the
 *    BOD-level is correct, and if it is, light up an LED:
 *     - \code
	if ((fuse_value & NVM_FUSES_BODLVL_gm) == BODLVL_2V1_gc) {
	    gpio_set_pin_low(LED0_GPIO);
	}
\endcode
 *
 * \section xmega_nvm_quickstart_signature_case Use case 3: Signature row
 *
 * The NVM driver has functions for reading the signature row of the device.
 * Here we will simply read 16 bytes from the user signature row, assuming
 * we need what is stored there.
 *
 * \section xmega_nvm_quickstart_signature_row_setup_steps Setup steps
 * There are no setup steps required for this use case.
 *
 * \subsection xmega_nvm_quickstart_signature_row_example_code Example code
 *
 * \code
	 #define START_ADDR 0x10
	 #define DATA_LENGTH 16

	 uint8_t values[LENGTH];
	 uint8_t i;

	 for (i = 0; i < DATA_LENGTH; i++) {
	     values[i] = nvm_read_user_signature_row(START_ADDR + i);
	 }
\endcode
 *
 * \subsection nvm_quickstart_signature_case_workflow Workflow
 *
 * -# Define starting address and length of data segment, and create
 *    variables needed to store and process the data:
 *     - \code
	       #define START_ADDR 0x10
	       #define DATA_LENGTH 16

	       uint8_t values[LENGTH];
	       uint8_t i;
\endcode
 * -# Iterate through the user signature row, and store our desired data:
 *     - \code
	for (i = 0; i < DATA_LENGTH; i++) {
	    values[i] = nvm_read_user_signature_row(START_ADDR + i);
	}
\endcode
 *
 */

#ifdef __cplusplus
}
#endif

#endif /* NVM_H */
