/**
 * \file
 *
 * \brief USB Device Driver for UDPHS. Compliant with common UDD driver.
 *
 * Copyright (c) 2012 - 2013 Atmel Corporation. All rights reserved.
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

#ifndef UDPHS_DEVICE_H_INCLUDED
#define UDPHS_DEVICE_H_INCLUDED

#include "compiler.h"
#include "preprocessor.h"

/* Get USB VBus pin configuration in board configuration */
#include "conf_board.h"
#include "board.h"
#include "ioport.h"
#include "pio.h"
#include "pio_handler.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

__always_inline static void io_pin_init(uint32_t pin, uint32_t flags,
	IRQn_Type port_irqn, uint8_t irq_level,
	void (*handler)(uint32_t,uint32_t), uint32_t wkup)
{
	// IOPORT must be initialized before by ioport_init(), \see ioport_group
	pio_handler_set_pin(pin, flags, handler);
	ioport_set_pin_sense_mode(pin, ioport_get_pin_level(pin) ?
		IOPORT_SENSE_LEVEL_LOW : IOPORT_SENSE_LEVEL_HIGH);
	NVIC_SetPriority(port_irqn, irq_level);
	NVIC_EnableIRQ(port_irqn);
	pio_enable_pin_interrupt(pin);
	if (wkup) {
		pmc_set_fast_startup_input(wkup);
	}
}

//! \ingroup udd_group
//! \defgroup udd_udphs_group USB Device High-Speed Port (UDPHS)
//! UDPHS low-level driver for USB device mode
//!
//! @{

//! @name UDPHS Device IP properties
//! These macros give access to IP properties
//! @{
  //! Get maximal number of endpoints
#define  udd_get_endpoint_max_nbr()            (6)
#define  UDD_MAX_PEP_NB                        (udd_get_endpoint_max_nbr()+1)
  //! Get maximal number of banks of endpoints
#define  udd_get_endpoint_bank_max_nbr(ep)      (((ep)==0)?1:(((ep)<=2)?2:3))
  //! Get maximal size of endpoint (3U, 1024/64)
#define  udd_get_endpoint_size_max(ep)          (((ep)==0)?64:(((ep)<=2)?512:(((ep)<=4)?64:1024)))
  //! Get High Band Width support of endpoints
#define  Is_udd_endpoint_high_bw_supported(ep)  (((ep)==0||(ep)==3||(ep)==4)?false:true)
//! @}

//! @name UDPHS Device speeds management
//! @{
  //! Enable high speed mode
#define  udd_high_speed_enable()             ( Wr_bitfield(UDPHS->UDPHS_TST, UDPHS_TST_SPEED_CFG_Msk, 0))
  //! Disable high speed mode
#define  udd_high_speed_disable()            ( Wr_bitfield(UDPHS->UDPHS_TST, UDPHS_TST_SPEED_CFG_Msk, 3))
  //! Test if controller is in full speed mode
#define  Is_udd_full_speed_mode()            (!Tst_bits(UDPHS->UDPHS_INTSTA, UDPHS_INTSTA_SPEED))
//! @}

//! @name UDPHS Device HS test mode management
//! @{
#ifdef USB_DEVICE_HS_SUPPORT
  //! Enable high speed test mode
#  define   udd_enable_hs_test_mode()        (Wr_bitfield(UDPHS->UDPHS_TST, UDPHS_TST_SPEED_CFG_Msk, 2))
#  define   udd_enable_hs_test_mode_j()      (Set_bits(UDPHS->UDPHS_TST, UDPHS_TST_TST_J))
#  define   udd_enable_hs_test_mode_k()      (Set_bits(UDPHS->UDPHS_TST, UDPHS_TST_TST_K))
#  define   udd_enable_hs_test_mode_packet() (Set_bits(UDPHS->UDPHS_TST, UDPHS_TST_TST_PKT))
#endif
//! @}

//! @name UDPHS Device vbus pin management
//! UDPHS peripheral does not support vbus management and it's monitored by a
//! PIO pin.
//! This feature is optional, and it is enabled if USB_VBUS_PIN is defined in
//! board.h and CONF_BOARD_USB_VBUS_DETECT defined in conf_board.h.
//!
//! @note ioport_init() must be invoked before using vbus pin functions since
//!       they use IOPORT API, \see ioport_group.
//!
//! @{
#define UDD_VBUS_DETECT (defined(CONF_BOARD_USB_PORT) && \
 		defined(CONF_BOARD_USB_VBUS_DETECT))
#define UDD_VBUS_IO     (defined(USB_VBUS_PIN) && UDD_VBUS_DETECT)
#ifndef USB_VBUS_WKUP
#  define USB_VBUS_WKUP 0
#endif

#define udd_vbus_init(handler) io_pin_init(USB_VBUS_PIN, USB_VBUS_FLAGS, \
	USB_VBUS_PIN_IRQn, UDD_USB_INT_LEVEL, handler, USB_VBUS_WKUP)
#define Is_udd_vbus_high()           ioport_get_pin_level(USB_VBUS_PIN)
#define Is_udd_vbus_low()            (!Is_udd_vbus_high())
#define udd_enable_vbus_interrupt()  pio_enable_pin_interrupt(USB_VBUS_PIN)
#define udd_disable_vbus_interrupt() pio_disable_pin_interrupt(USB_VBUS_PIN)
#define udd_ack_vbus_interrupt(high) ioport_set_pin_sense_mode(USB_VBUS_PIN,\
	high ? IOPORT_SENSE_LEVEL_LOW : IOPORT_SENSE_LEVEL_HIGH)
//! @}

//! @name UDP peripheral enable/disable
//!
//! @warning The UDP peripheral clock in the Power Management Controller (PMC)
//!          must be enabled before any read/write operations to the UDPHS registers.
//!
//! @{
#define  udd_enable_periph_ck()              pmc_enable_periph_clk(ID_UDPHS)
#define  udd_disable_periph_ck()             pmc_disable_periph_clk(ID_UDPHS)
#define  Is_udd_periph_ck_enabled()          pmc_is_periph_clk_enabled(ID_UDPHS)
//! @}

//! @name UDPHS main management
//! These macros allows to enable/disable pad and UDPHS hardware
//! @{
  //! Enable USB macro
#define  udd_enable_periph()                 (Set_bits(UDPHS->UDPHS_CTRL, UDPHS_CTRL_EN_UDPHS))
  //! Disable USB macro
#define  udd_disable_periph()                (Clr_bits(UDPHS->UDPHS_CTRL, UDPHS_CTRL_EN_UDPHS))
  //! Reset and disable USB
#define  udd_reset_periph()                  udd_disable_periph()
#define  Is_udd_periph_enabled()             (Tst_bits(UDPHS->UDPHS_CTRL, UDPHS_CTRL_EN_UDPHS))
//! @}

//! @name UDPHS device attach control
//! These macros manage the UDPHS Device attach.
//! @{
  //! detaches from USB bus
#define  udd_detach_device()                                       \
	do {                                                       \
		Set_bits(UDPHS->UDPHS_CTRL, UDPHS_CTRL_DETACH);    \
		Clr_bits(UDPHS->UDPHS_CTRL, UDPHS_CTRL_PULLD_DIS); \
	} while (0)
  //! attaches to USB bus
#define  udd_attach_device()                                       \
	do {                                                       \
		Set_bits(UDPHS->UDPHS_CTRL, UDPHS_CTRL_PULLD_DIS); \
		Clr_bits(UDPHS->UDPHS_CTRL, UDPHS_CTRL_DETACH);    \
	} while (0)
  //! test if the device is detached
#define  Is_udd_detached()                   (Tst_bits(UDPHS->UDPHS_CTRL, UDPHS_CTRL_DETACH))
//! @}


//! @name UDPHS device bus events control
//! These macros manage the UDPHS Device bus events.
//! @{

//! Initiates a remote wake-up event
//! @{
#define  udd_initiate_remote_wake_up()       (Set_bits(UDPHS->UDPHS_CTRL, UDPHS_CTRL_REWAKEUP))
#define  Is_udd_pending_remote_wake_up()     (Tst_bits(UDPHS->UDPHS_CTRL, UDPHS_CTRL_REWAKEUP))
//! @}

//! Manage upstream resume event
//! The USB driver sends a resume signal called "Upstream Resume"
//! @{
#define  udd_enable_remote_wake_up_interrupt()     (Set_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_UPSTR_RES))
#define  udd_disable_remote_wake_up_interrupt()    (Clr_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_UPSTR_RES))
#define  Is_udd_remote_wake_up_interrupt_enabled() (Tst_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_UPSTR_RES))
#define  udd_ack_remote_wake_up_start()            (UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_UPSTR_RES)
#define  Is_udd_remote_wake_up_start()             (Tst_bits(UDPHS->UDPHS_INTSTA, UDPHS_INTSTA_UPSTR_RES))
//! @}

//! Manage end of resume event
//! The USB controller detects a valid "End of Resume" signal initiated by the host
//! @{
#define  udd_enable_resume_interrupt()             (Set_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_ENDOFRSM))
#define  udd_disable_resume_interrupt()            (Clr_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_ENDOFRSM))
#define  Is_udd_resume_interrupt_enabled()         (Tst_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_ENDOFRSM))
#define  udd_ack_resume()                          (UDPHS->UDPHS_CLRINT = UDPHS_INTSTA_ENDOFRSM)
#define  Is_udd_resume()                           (Tst_bits(UDPHS->UDPHS_INTSTA, UDPHS_INTSTA_ENDOFRSM))
//! @}

//! Manage wake-up event (=usb line activity)
//! The USB controller is reactivated by a filtered non-idle signal from the lines
//! @{
#define  udd_enable_wake_up_interrupt()            (Set_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_WAKE_UP))
#define  udd_disable_wake_up_interrupt()           (Clr_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_WAKE_UP))
#define  Is_udd_wake_up_interrupt_enabled()        (Tst_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_WAKE_UP))
#define  udd_ack_wake_up()                         (UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_WAKE_UP)
#define  Is_udd_wake_up()                          (Tst_bits(UDPHS->UDPHS_INTSTA, UDPHS_INTSTA_WAKE_UP))
//! @}

//! Manage reset event
//! Set when a USB "End of Reset" has been detected
//! @{
#define  udd_enable_reset_interrupt()              (Set_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_ENDRESET))
#define  udd_disable_reset_interrupt()             (Clr_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_ENDRESET))
#define  Is_udd_reset_interrupt_enabled()          (Tst_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_ENDRESET))
#define  udd_ack_reset()                           (UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_ENDRESET)
#define  Is_udd_reset()                            (Tst_bits(UDPHS->UDPHS_INTSTA, UDPHS_INTSTA_ENDRESET))
//! @}

//! Manage start of frame event
//! @{
#define  udd_enable_sof_interrupt()                (Set_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_INT_SOF))
#define  udd_disable_sof_interrupt()               (Clr_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_INT_SOF))
#define  Is_udd_sof_interrupt_enabled()            (Tst_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_INT_SOF))
#define  udd_ack_sof()                             (UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_INT_SOF)
#define  Is_udd_sof()                              (Tst_bits(UDPHS->UDPHS_INTSTA, UDPHS_INTSTA_INT_SOF))
#define  udd_frame_number()                        (Rd_bitfield(UDPHS->UDPHS_FNUM, UDPHS_FNUM_FRAME_NUMBER_Msk))
#define  Is_udd_frame_number_crc_error()           (Tst_bits(UDPHS->UDPHS_FNUM, UDPHS_FNUM_FNUM_ERR))
//! @}

//! Manage Micro start of frame event (High Speed Only)
//! @{
#define  udd_enable_msof_interrupt()               (Set_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_MICRO_SOF))
#define  udd_disable_msof_interrupt()              (Clr_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_MICRO_SOF))
#define  Is_udd_msof_interrupt_enabled()           (Tst_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_MICRO_SOF))
#define  udd_ack_msof()                            (UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_MICRO_SOF)
#define  Is_udd_msof()                             (Tst_bits(UDPHS->UDPHS_INTSTA, UDPHS_INTSTA_MICRO_SOF))
#define  udd_micro_frame_number()                  (Rd_bitfield(UDPHS->UDPHS_FNUM, (UDPHS_FNUM_MICRO_FRAME_NUM_Msk)))
//! @}

//! Manage suspend event
//! @{
#define  udd_enable_suspend_interrupt()            (Set_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_DET_SUSPD))
#define  udd_disable_suspend_interrupt()           (Clr_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_DET_SUSPD))
#define  Is_udd_suspend_interrupt_enabled()        (Tst_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_DET_SUSPD))
#define  udd_ack_suspend()                         (UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_DET_SUSPD)
#define  Is_udd_suspend()                          (Tst_bits(UDPHS->UDPHS_INTSTA, UDPHS_INTSTA_DET_SUSPD))
//! @}

//! @}

//! @name UDPHS device address control
//! These macros manage the UDPHS Device address.
//! @{
  //! enables USB device address
#define  udd_enable_address()                      (Set_bits(UDPHS->UDPHS_CTRL, UDPHS_CTRL_FADDR_EN))
  //! disables USB device address
#define  udd_disable_address()                     (Clr_bits(UDPHS->UDPHS_CTRL, UDPHS_CTRL_FADDR_EN))
#define  Is_udd_address_enabled()                  (Tst_bits(UDPHS->UDPHS_CTRL, UDPHS_CTRL_FADDR_EN))
  //! configures the USB device address
#define  udd_configure_address(addr)               (Wr_bitfield(UDPHS->UDPHS_CTRL, UDPHS_CTRL_DEV_ADDR_Msk, addr))
  //! gets the currently configured USB device address
#define  udd_get_configured_address()              (Rd_bitfield(UDPHS->UDPHS_CTRL, UDPHS_CTRL_DEV_ADDR_Msk))
//! @}


//! @name UDPHS Device endpoint drivers
//! These macros manage the common features of the endpoints.
//! @{

//! Generic macro for UDPHS registers that can be arrayed
//! @{
#define UDPHS_ARRAY(reg,index)                     ((&(UDPHS->reg))[(index)])
//! @}

//! @name UDPHS Device endpoint configuration
//! @{
  //! enables the selected endpoint
#define  udd_enable_endpoint(ep)                   (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_EPT_ENABL)
  //! disables the selected endpoint
#define  udd_disable_endpoint(ep)                  (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLDIS = UDPHS_EPTCTLDIS_EPT_DISABL)
  //! tests if the selected endpoint is enabled
#define  Is_udd_endpoint_enabled(ep)               (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTL, UDPHS_EPTCTL_EPT_ENABL))
  //! resets the selected endpoint
#define  udd_reset_endpoint(ep) \
	do { \
		Set_bits(UDPHS->UDPHS_EPTRST, UDPHS_EPTRST_EPT_0 << (ep)); \
		Clr_bits(UDPHS->UDPHS_EPTRST, UDPHS_EPTRST_EPT_0 << (ep)); \
	} while (0)

  //! configures the selected endpoint type
#define  udd_configure_endpoint_type(ep, type)     (Wr_bitfield(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCFG, UDPHS_EPTCFG_EPT_TYPE_Msk, type))
  //! gets the configured selected endpoint type
#define  udd_get_endpoint_type(ep)                 (Rd_bitfield(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCFG, UDPHS_EPTCFG_EPT_TYPE_Msk))
  //! enables the bank autoswitch for the selected endpoint
#define  udd_enable_endpoint_bank_autoswitch(ep)   (Set_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLENB, UDPHS_EPTCTLENB_AUTO_VALID))
  //! disables the bank autoswitch for the selected endpoint
#define  udd_disable_endpoint_bank_autoswitch(ep)   (Clr_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLDIS, UDPHS_EPTCTLDIS_AUTO_VALID))
#define  Is_udd_endpoint_bank_autoswitch_enabled(ep) (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTL, UDPHS_EPTCTL_AUTO_VALID))
  //! configures the selected endpoint direction
#define  udd_configure_endpoint_direction(ep, dir) (Wr_bitfield(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCFG, UDPHS_EPTCFG_EPT_DIR, dir))
  //! gets the configured selected endpoint direction
#define  udd_get_endpoint_direction(ep)            (Rd_bitfield(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCFG, UDPHS_EPTCFG_EPT_DIR))
#define  Is_udd_endpoint_in(ep)                    (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCFG, UDPHS_EPTCFG_EPT_DIR))
  //! Bounds given integer size to allowed range and rounds it up to the nearest
  //! available greater size, then applies register format of UDPHS controller
  //! for endpoint size bit-field.
#define  udd_format_endpoint_size(size)            (32 - clz(((uint32_t)min(max(size, 8), 1024) << 1) - 1) - 1 - 3)
  //! configures the selected endpoint size
#define  udd_configure_endpoint_size(ep, size)     (Wr_bitfield(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCFG, UDPHS_EPTCFG_EPT_SIZE_Msk, udd_format_endpoint_size(size)))
  //! gets the configured selected endpoint size
#define  udd_get_endpoint_size(ep)                 (8 << Rd_bitfield(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCFG, UDPHS_EPTCFG_EPT_SIZE_Msk))
  //! configures the selected endpoint number of banks (0~3)
#define  udd_configure_endpoint_bank(ep, bank)     (Wr_bitfield(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCFG, UDPHS_EPTCFG_BK_NUMBER_Msk, bank))
  //! gets the configured selected endpoint number of banks (0~3)
#define  udd_get_endpoint_bank(ep)                 (Rd_bitfield(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCFG, UDPHS_EPTCFG_BK_NUMBER_Msk))
  //! configures the selected endpoint number of transaction per microframe (1 ~ 3)
#define  udd_configure_endpoint_nb_tran(ep, nb)    (Wr_bitfield(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCFG, UDPHS_EPTCFG_NB_TRANS_Msk, nb))
  //! gets the number of transaction per microframe
#define  udd_get_endpoint_nb_tran(ep)              (Rd_bitfield(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCFG, UDPHS_EPTCFG_NB_TRANS_Msk))
  //! tests if the endpoint is mapped correctly
#define  Is_udd_endpoint_mapped(ep)                (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCFG, UDPHS_EPTCFG_EPT_MAPD))

  //! configures selected endpoint in one step
#define  udd_configure_endpoint(ep, type, dir, size, bank, nb_trans) (\
	Wr_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCFG, UDPHS_EPTCFG_EPT_TYPE_Msk |\
		UDPHS_EPTCFG_EPT_DIR  |\
		UDPHS_EPTCFG_EPT_SIZE_Msk  |\
		UDPHS_EPTCFG_BK_NUMBER_Msk |\
		UDPHS_EPTCFG_NB_TRANS_Msk,  \
		(((uint32_t)(type) << UDPHS_EPTCFG_EPT_TYPE_Pos)     & UDPHS_EPTCFG_EPT_TYPE_Msk) |\
		(((uint32_t)(dir ) << UDPHS_EPTCFG_EPT_DIR_Pos )     & UDPHS_EPTCFG_EPT_DIR) |\
		( (uint32_t)udd_format_endpoint_size(size) << UDPHS_EPTCFG_EPT_SIZE_Pos) |\
		(((uint32_t)(bank) << UDPHS_EPTCFG_BK_NUMBER_Pos)    & UDPHS_EPTCFG_BK_NUMBER_Msk) |\
		(((uint32_t)(nb_trans) << UDPHS_EPTCFG_NB_TRANS_Pos) & UDPHS_EPTCFG_NB_TRANS_Msk)) )

#define UDPHS_EPTCFG_EPT_DIR_Pos 3
  //! tests if current endpoint is configured
#define  Is_udd_endpoint_configured(ep)            (Is_udd_endpoint_mapped(ep)&&Is_udd_endpoint_enabled(ep))
  //! returns the control direction
#define  udd_control_direction()                   (udd_get_endpoint_direction(ep))

  //! resets the data toggle sequence
#define  udd_reset_data_toggle(ep)                 (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_TOGGLESQ)
  //! returns data toggle
#define  udd_data_toggle(ep)                       (Rd_bitfield(UDPHS->UDPHS_EPT[ep].UDPHS_EPTSTA, UDPHS_EPTSTA_TOGGLESQ_STA_Msk))
//! @}


//! @name UDPHS Device control endpoint
//! These macros control the endpoints.
//! @{

//! @name UDPHS Device handle endpoint statuses
//! These macros handle the endpoints statuses.
//!
//! @note Some of bits in status register is cleared right after reading,
//!       so the status value should be read only once in ISR.
//!
//! @{
      //! Structure for DMA control register
typedef struct {
	uint32_t reserved:5;         // 0~4
	uint32_t FRCESTALL:1;        // 5
	uint32_t TOGGLESQ_STA:2;     // 6,7
	uint32_t ERR_OVFLW:1;        // 8
	union {                      // 9
		uint32_t RX_BK_RDY:1;
		uint32_t KILL_BANK:1;
	};
	uint32_t TX_COMPLT:1;        // 10
	union {                      // 11
		uint32_t TX_PK_RDY:1;
		uint32_t ERR_TRANS:1;
	};
	union {                      // 12
		uint32_t RX_SETUP:1;
		uint32_t ERR_FL_ISO:1;
	};
	union {                      // 13
		uint32_t STALL_SNT:1;
		uint32_t ERR_CRISO:1;
		uint32_t ERR_NBTRA:1;
	};
	union {                      // 14
		uint32_t NAK_IN:1;
		uint32_t ERR_FLUSH:1;
	};
	uint32_t NAK_OUT:1;          // 15
	union {                      // 16,17
		uint32_t CURRENT_BANK:2;
		uint32_t CONTROL_DIR:2;
	};
	uint32_t BUSY_BANK_STA:2;    // 18,19
	uint32_t BYTE_COUNT:11;      // 20~30
	uint32_t SHRT_PCKT:1;        // 31

} udphs_endpoint_status_t;
#define  udd_get_endpoint_status(ep)                    (UDPHS->UDPHS_EPT[ep].UDPHS_EPTSTA)

#define  udd_get_endpoint_status_byte_count(status)     ((status & UDPHS_EPTSTA_BYTE_COUNT_Msk)    >> UDPHS_EPTSTA_BYTE_COUNT_Pos)
#define  udd_get_endpoint_status_nb_busy_bank(status)   ((status & UDPHS_EPTSTA_BUSY_BANK_STA_Msk) >> UDPHS_EPTSTA_BUSY_BANK_STA_Pos)
#define  udd_get_endpoint_status_current_bank(status)   ((status & UDPHS_EPTSTA_CURRENT_BANK_Msk)  >> UDPHS_EPTSTA_CURRENT_BANK_Pos)
#define  udd_get_endpoint_status_control_dir(status)    ((status & UDPHS_EPTSTA_CONTROL_DIR_Msk)   >> UDPHS_EPTSTA_CONTROL_DIR_Pos)
#define  udd_get_endpoint_status_data_toggle(status)    ((status & UDPHS_EPTSTA_TOGGLESQ_STA_Msk)  >> UDPHS_EPTSTA_TOGGLESQ_STA_Pos)

#define  Is_udd_endpoint_status_stall_requested(status) (Tst_bits(status, UDPHS_EPTSTA_FRCESTALL))
#define  Is_udd_endpoint_status_overflow(status)        (Tst_bits(status, UDPHS_EPTSTA_ERR_OVFLW))
#define  Is_udd_endpoint_status_out_received(status)    (Tst_bits(status, UDPHS_EPTSTA_RX_BK_RDY))
#define  Is_udd_endpoint_status_kill_bank(status)       (Tst_bits(status, UDPHS_EPTSTA_KILL_BANK))
#define  Is_udd_endpoint_status_in_sent(status)         (Tst_bits(status, UDPHS_EPTSTA_TX_COMPLT))
#define  Is_udd_endpoint_status_tx_pkt_ready(status)    (Tst_bits(status, UDPHS_EPTSTA_TX_PK_RDY))
#define  Is_udd_endpoint_status_err_trans(status)       (Tst_bits(status, UDPHS_EPTSTA_ERR_TRANS))
#define  Is_udd_endpoint_status_setup_received(status)  (Tst_bits(status, UDPHS_EPTSTA_RX_SETUP))
#define  Is_udd_endpoint_status_err_flow(status)        (Tst_bits(status, UDPHS_EPTSTA_ERR_FL_ISO)
#define  Is_udd_endpoint_status_stall(status)           (Tst_bits(status, UDPHS_EPTSTA_STALL_SNT))
#define  Is_udd_endpoint_status_crc_err(status)         (Tst_bits(status, UDPHS_EPTSTA_ERR_CRISO))
#define  Is_udd_endpoint_status_nb_tran(status)         (Tst_bits(status, UDPHS_EPTSTA_ERR_NBTRA))
#define  Is_udd_endpoint_status_nak_in(status)          (Tst_bits(status, UDPHS_EPTSTA_NAK_IN))
#define  Is_udd_endpoint_status_flush_error(status)     (Tst_bits(status, UDPHS_EPTSTA_ERR_FLUSH))
#define  Is_udd_endpoint_status_nak_out(status)         (Tst_bits(status, UDPHS_EPTSTA_NAK_OUT))
#define  Is_udd_endpoint_status_short_packet(status)    (Tst_bits(status, UDPHS_EPTSTA_SHRT_PCKT))
//! @}

//! @name UDPHS Device control endpoint interrupts
//! These macros control the endpoints interrupts.
//! @{
  //! enables the selected endpoint interrupt
#define  udd_enable_endpoint_interrupt(ep)         (Set_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_EPT_0 << (ep)))
  //! disables the selected endpoint interrupt
#define  udd_disable_endpoint_interrupt(ep)        (Clr_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_EPT_0 << (ep)))
  //! tests if the selected endpoint interrupt is enabled
#define  Is_udd_endpoint_interrupt_enabled(ep)     (Tst_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_EPT_0 << (ep)))
  //! tests if an interrupt is triggered by the selected endpoint
#define  Is_udd_endpoint_interrupt(ep)             (Tst_bits(UDPHS->UDPHS_INTSTA, UDPHS_INTSTA_EPT_0 << (ep)))
  //! returns the lowest endpoint number generating an endpoint interrupt or MAX_PEP_NB if none
#define  udd_get_interrupt_endpoint_number()       (ctz(((UDPHS->UDPHS_INTSTA >> UDPHS_INTSTA_EPT_Pos) &\
                                                   (UDPHS->UDPHS_IEN >> UDPHS_IEN_EPT_Pos)) |\
                                                   (1 << MAX_PEP_NB)))

#define  UDPHS_INTSTA_EPT_Pos                      (ctz(UDPHS_INTSTA_EPT_0))
#define  UDPHS_IEN_EPT_Pos                         (ctz(UDPHS_IEN_EPT_0))
#define  MAX_PEP_NB                                (udd_get_endpoint_max_nbr()+1)
//! @}

//! @name UDPHS Device control endpoint errors
//! These macros control the endpoint errors.
//! @{
  //! enables the STALL handshake
#define  udd_enable_stall_handshake(ep)            (UDPHS->UDPHS_EPT[ep].UDPHS_EPTSETSTA = UDPHS_EPTSETSTA_FRCESTALL)
  //! disables the STALL handshake
#define  udd_disable_stall_handshake(ep)           (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_FRCESTALL)
  //! tests if STALL handshake request is running
#define  Is_udd_endpoint_stall_requested(ep)       (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTSTA, UDPHS_EPTSTA_FRCESTALL))
  //! tests if STALL sent
#define  Is_udd_stall(ep)                          (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTSTA, UDPHS_EPTSTA_STALL_SNT))
  //! acks STALL sent
#define  udd_ack_stall(ep)                         (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_STALL_SNT)
  //! enables STALL sent interrupt
#define  udd_enable_stall_interrupt(ep)            (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_STALL_SNT)
  //! disables STALL sent interrupt
#define  udd_disable_stall_interrupt(ep)           (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLDIS = UDPHS_EPTCTLDIS_STALL_SNT)
  //! tests if STALL sent interrupt is enabled
#define  Is_udd_stall_interrupt_enabled(ep)        (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTL, UDPHS_EPTCTL_STALL_SNT))

  //! tests if NAK OUT received
#define  Is_udd_nak_out(ep)                        (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTSTA, UDPHS_EPTSTA_NAK_OUT))
  //! acks NAK OUT received
#define  udd_ack_nak_out(ep)                       (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_NAK_OUT)
  //! enables NAK OUT interrupt
#define  udd_enable_nak_out_interrupt(ep)          (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_NAK_OUT)
  //! disables NAK OUT interrupt
#define  udd_disable_nak_out_interrupt(ep)         (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLDIS = UDPHS_EPTCTLDIS_NAK_OUT)
  //! tests if NAK OUT interrupt is enabled
#define  Is_udd_nak_out_interrupt_enabled(ep)      (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTL, UDPHS_EPTCTL_NAK_OUT))

  //! tests if NAK IN received
#define  Is_udd_nak_in(ep)                         (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTSTA, UDPHS_EPTSTA_NAK_IN))
  //! acks NAK IN received
#define  udd_ack_nak_in(ep)                        (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_NAK_IN)
  //! enables NAK IN interrupt
#define  udd_enable_nak_in_interrupt(ep)           (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_NAK_IN)
  //! disables NAK IN interrupt
#define  udd_disable_nak_in_interrupt(ep)          (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLDIS = UDPHS_EPTCTLDIS_NAK_IN)
  //! tests if NAK IN interrupt is enabled
#define  Is_udd_nak_in_interrupt_enabled(ep)       (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTL, UDPHS_EPTCTL_NAK_IN))

  //! disables NYET for HS bulk OUT
#define  udd_disable_nyet(ep)                      (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_NYET_DIS)
  //! enables NYET for HS bulk OUT
#define  udd_enable_nyet(ep)                       (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLDIS = UDPHS_EPTCTLDIS_NYET_DIS)
  //! tests if NYET is disabled
#define  Is_udd_nyet_disabled(ep)                  (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTL, UDPHS_EPTCTL_NYET_DIS))

  //! acks endpoint isochronous error flow interrupt
#define  udd_ack_errflow_interrupt(ep)             (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_ERR_FL_ISO)
  //! tests if an overflow occurs
#define  Is_udd_errflow(ep)                        (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTSTA, UDPHS_EPTSTA_ERR_FL_ISO))
  //! enables overflow interrupt
#define  udd_enable_errflow_interrupt(ep)          (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_ERR_FL_ISO)
  //! disables overflow interrupt
#define  udd_disable_errflow_interrupt(ep)         (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLDIS = UDPHS_EPTCTLDIS_ERR_FL_ISO)
  //! tests if overflow interrupt is enabled
#define  Is_udd_errflow_interrupt_enabled(ep)      (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTL, UDPHS_EPTCTL_ERR_FL_ISO))

  //! acks endpoint transaction error interrupt
#define  udd_ack_errtran_interrupt(ep)             (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCLRSTA = UDPHS_EPTSTA_ERR_TRANS)
  //! tests if an transaction error occurs
#define  Is_udd_errtran(ep)                        (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTSTA, UDPHS_EPTSTA_ERR_TRANS))
  //! enables transaction error interrupt
#define  udd_enable_errtran_interrupt(ep)          (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_ERR_TRANS)
  //! disables transaction error interrupt
#define  udd_disable_errtran_interrupt(ep)         (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLDIS = UDPHS_EPTCTLDIS_ERR_TRANS)
  //! tests if transaction error interrupt is enabled
#define  Is_udd_errtran_interrupt_enabled(ep)      (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTL, UDPHS_EPTCTL_ERR_TRANS))

  //! tests if CRC ERROR ISO detected
#define  Is_udd_crc_error(ep)                      (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTSTA, UDPHS_EPTSTA_ERR_CRISO))
  //! acks CRC ERROR ISO detected
#define  udd_ack_crc_error(ep)                     (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCLRSTA = UDPHS_EPTSTA_ERR_CRISO)
  //! enables CRC ERROR ISO detected interrupt
#define  udd_enable_crc_error_interrupt(ep)        (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_ERR_CRISO)
  //! disables CRC ERROR ISO detected interrupt
#define  udd_disable_crc_error_interrupt(ep)       (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLDIS = UDPHS_EPTCTLDIS_ERR_CRISO)
  //! tests if CRC ERROR ISO detected interrupt is enabled
#define  Is_udd_crc_error_interrupt_enabled(ep)    (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTL, UDPHS_EPTCTL_ERR_CRISO))

  //! tests if Number of Transaction Error detected
#define  Is_udd_nb_tran(ep)                        (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTSTA, UDPHS_EPTSTA_ERR_NBTRA))
  //! acks Number of Transaction Error detected
#define  udd_ack_nb_tran(ep)                       (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_ERR_NBTRA)
  //! enables Number of Transaction Error detected interrupt
#define  udd_enable_nb_tran_interrupt(ep)          (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_ERR_NBTRA)
  //! disables Number of Transaction Error detected interrupt
#define  udd_disable_nb_tran_interrupt(ep)         (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLDIS = UDPHS_EPTCTLDIS_ERR_NBTRA)
  //! tests if Number of Transaction Error detected interrupt is enabled
#define  Is_udd_nb_tran_interrupt_enabled(ep)      (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTL, UDPHS_EPTCTL_ERR_NBTRA))

  //! acks endpoint bank flush error interrupt
#define  udd_ack_flush_error_interrupt(ep)         (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_ERR_FLUSH)
  //! tests if a flush error occurs
#define  Is_udd_flush_error(ep)                    (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTSTA, UDPHS_EPTSTA_ERR_FLUSH))
  //! enables flush error interrupt
#define  udd_enable_flush_error_interrupt(ep)      (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_ERR_FLUSH)
  //! disables flush error interrupt
#define  udd_disable_flush_error_interrupt(ep)     (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLDIS = UDPHS_EPTCTLDIS_ERR_FLUSH)
  //! tests if flush error interrupt is enabled
#define  Is_udd_flush_error_interrupt_enabled(ep)  (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTL, UDPHS_EPTCTL_ERR_FLUSH))
//! @}

//! @name UDPHS Device control endpoint transfer
//! These macros control the endpoint transfer.
//! @{

  //! returns the byte count
#define  udd_byte_count(ep)                        (Rd_bitfield(UDPHS->UDPHS_EPT[ep].UDPHS_EPTSTA, UDPHS_EPTSTA_BYTE_COUNT_Msk))

  //! raises TX_PK_RDY
#define  udd_raise_tx_pkt_ready(ep)                (UDPHS->UDPHS_EPT[ep].UDPHS_EPTSETSTA = UDPHS_EPTSETSTA_TX_PK_RDY)
  //! tests if TX_PK_RDY bit set
#define  Is_udd_tx_pkt_ready(ep)                   (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTSTA, UDPHS_EPTSTA_TX_PK_RDY))
  //! enables TX_PK_RDY interrupt
#define  udd_enable_tx_pkt_ready_interrupt(ep)     (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_TX_PK_RDY)
  //! disables TX_PK_RDY interrupt
#define  udd_disable_tx_pkt_ready_interrupt(ep)    (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLDIS = UDPHS_EPTCTLDIS_TX_PK_RDY)
  //! tests if TX_PK_RDY interrupt is enabled
#define  Is_udd_tx_pkt_ready_interrupt_enabled(ep) (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTL, UDPHS_EPTCTL_TX_PK_RDY))

  //! acks TX_COMPLT
#define  udd_ack_tx_complt(ep)                     (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_TX_COMPLT)
  //! tests if TX_COMPLT bit set
#define  Is_udd_tx_complt(ep)                      (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTSTA, UDPHS_EPTSTA_TX_COMPLT);
  //! enables TX_COMPLT interrupt
#define  udd_enable_tx_complt_interrupt(ep)        (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_TX_COMPLT)
  //! disables TX_COMPLT interrupt
#define  udd_disable_tx_complt_interrupt(ep)       (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLDIS = UDPHS_EPTCTLDIS_TX_COMPLT)
  //! tests if TX_COMPLT interrupt is enabled
#define  Is_udd_tx_complt_interrupt_enabled(ep)    (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTL, UDPHS_EPTCTL_TX_COMPLT))

  //! returns the number of busy banks
#define  udd_nb_busy_bank(ep)                      (Rd_bitfield(UDPHS->UDPHS_EPT[ep].UDPHS_EPTSTA, UDPHS_EPTSTA_BUSY_BANK_STA_Msk))
  //! returns the number of the current bank
#define  udd_current_bank(ep)                      (Rd_bitfield(UDPHS->UDPHS_EPT[ep].UDPHS_EPTSTA, UDPHS_EPTSTA_CURRENT_BANK_Msk))
  //! kills last bank
#define  udd_kill_last_in_bank(ep)                 (UDPHS->UDPHS_EPT[ep].UDPHS_EPTSETSTA = UDPHS_EPTSETSTA_KILL_BANK)
#define  Is_udd_kill_last(ep)                      (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTSTA, UDPHS_EPTSETSTA_KILL_BANK))
  //! enables all banks full (OUT) or free (IN) interrupt
#define  udd_enable_bank_interrupt(ep)             (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_BUSY_BANK)
  //! disables all banks full (OUT) or free (IN) interrupt
#define  udd_disable_bank_interrupt(ep)            (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLDIS = UDPHS_EPTCTLDIS_BUSY_BANK)
  //! tests if all banks full (OUT) or free (IN) interrupt enabled
#define  Is_udd_bank_interrupt_enabled(ep)         (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTL, UDPHS_EPTCTL_BUSY_BANK))

  //! tests if SHORT PACKET received
#define  Is_udd_short_packet(ep)                   (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTSTA, UDPHS_EPTSTA_SHRT_PCKT))
  //! enables SHORT PACKET received interrupt
#define  udd_enable_short_packet_interrupt(ep)     (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_SHRT_PCKT)
  //! disables SHORT PACKET received interrupt
#define  udd_disable_short_packet_interrupt(ep)    (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLDIS = UDPHS_EPTCTLDIS_SHRT_PCKT)
  //! tests if SHORT PACKET received interrupt is enabled
#define  Is_udd_short_packet_interrupt_enabled(ep) (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTL, UDPHS_EPTCTL_SHRT_PCKT))

  //! tests if SETUP received
#define  Is_udd_setup_received(ep)                    (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTSTA, UDPHS_EPTSTA_RX_SETUP))
  //! acks SETUP received
#define  udd_ack_setup_received(ep)                   (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_RX_SETUP)
  //! enables SETUP received interrupt
#define  udd_enable_setup_received_interrupt(ep)      (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_RX_SETUP)
  //! disables SETUP received interrupt
#define  udd_disable_setup_received_interrupt(ep)     (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLDIS = UDPHS_EPTCTLDIS_RX_SETUP)
  //! tests if SETUP received interrupt is enabled
#define  Is_udd_setup_received_interrupt_enabled(ep)  (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTL, UDPHS_EPTCTL_RX_SETUP))

  //! tests if OUT received
#define  Is_udd_out_received(ep)                   (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTSTA, UDPHS_EPTCTL_RX_BK_RDY))
  //! acks OUT received
#define  udd_ack_out_received(ep)                  (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_RX_BK_RDY)
  //! enables OUT received interrupt
#define  udd_enable_out_received_interrupt(ep)     (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_RX_BK_RDY)
  //! disables OUT received interrupt
#define  udd_disable_out_received_interrupt(ep)    (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLDIS = UDPHS_EPTCTLDIS_RX_BK_RDY)
  //! tests if OUT received interrupt is enabled
#define  Is_udd_out_received_interrupt_enabled(ep) (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTL, UDPHS_EPTCTL_RX_BK_RDY))

  //! tests if IN sending
#define  Is_udd_in_send(ep)                        (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTSTA, UDPHS_EPTSTA_TX_COMPLT))
  //! acks IN sending
#define  udd_ack_in_send(ep)                       (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_TX_COMPLT)
  //! enables IN sending interrupt
#define  udd_enable_in_send_interrupt(ep)          (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_TX_COMPLT)
  //! disables IN sending interrupt
#define  udd_disable_in_send_interrupt(ep)         (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLDIS = UDPHS_EPTCTLDIS_TX_COMPLT)
  //! tests if IN sending interrupt is enabled
#define  Is_udd_in_send_interrupt_enabled(ep)      (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTL, UDPHS_EPTCTL_TX_COMPLT))


  //! Get 64-, 32-, 16- or 8-bit access to FIFO data register of selected endpoint.
  //! @param ep     Endpoint of which to access FIFO data register
  //! @param scale  Data scale in bits: 64, 32, 16 or 8
  //! @return       Volatile 64-, 32-, 16- or 8-bit data pointer to FIFO data register
  //! @warning It is up to the user of this macro to make sure that all accesses
  //! are aligned with their natural boundaries except 64-bit accesses which
  //! require only 32-bit alignment.
  //! @warning It is up to the user of this macro to make sure that used HSB
  //! addresses are identical to the DPRAM internal pointer modulo 32 bits.
#define  udd_get_endpoint_fifo_access(ep, scale) \
		(((volatile TPASTE2(U, scale) (*)[0x10000 / ((scale) / 8)])UDPHS_RAM_ADDR)[(ep)])

//! @name UDPHS endpoint DMA drivers
//! These macros manage the common features of the endpoint DMA channels.
//! @{

  //! Maximum transfer size on USB DMA
#define UDD_ENDPOINT_MAX_TRANS 0x10000
  //! enables the disabling of HDMA requests by endpoint interrupts
#define  udd_enable_endpoint_int_dis_hdma_req(ep)     (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_INTDIS_DMA)
  //! disables the disabling of HDMA requests by endpoint interrupts
#define  udd_disable_endpoint_int_dis_hdma_req(ep)    (UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLDIS = UDPHS_EPTCTLDIS_INTDIS_DMA)
  //! tests if the disabling of HDMA requests by endpoint interrupts is enabled
#define  Is_udd_endpoint_int_dis_hdma_req_enabled(ep) (Tst_bits(UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTL, UDPHS_EPTCTL_INTDIS_DMA))

  //! tests if an interrupt is triggered by the selected endpoint DMA channel
#define  Is_udd_endpoint_dma_interrupt(ep)            (Tst_bits(UDPHS->UDPHS_INTSTA, UDPHS_INTSTA_DMA_1 << ((ep) - 1)))
  //! enables the selected endpoint DMA channel interrupt
#define  udd_enable_endpoint_dma_interrupt(ep)        (Set_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_DMA_1 << ((ep) - 1)))
  //! disables the selected endpoint DMA channel interrupt
#define  udd_disable_endpoint_dma_interrupt(ep)       (Clr_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_DMA_1 << ((ep) - 1)))
  //! tests if the selected endpoint DMA channel interrupt is enabled
#define  Is_udd_endpoint_dma_interrupt_enabled(ep)    (Tst_bits(UDPHS->UDPHS_IEN, UDPHS_IEN_DMA_1 << ((ep) - 1)))

  //! Access points to the UDPHS device DMA memory map with arrayed registers
  //! @{
      //! Structure for DMA next descriptor register
typedef struct {
	uint32_t *NXT_DSC_ADD;
} udphs_dma_nextdesc_t;
      //! Structure for DMA control register
typedef struct {
	uint32_t CHANN_ENB:1,
		LDNXT_DSC:1,
		END_TR_EN:1,
		END_B_EN:1,
		END_TR_IT:1,
		END_BUFFIT:1,
		DESC_LD_IT:1, BUST_LCK:1, reserved:8, BUFF_LENGTH:16;
} udphs_dma_control_t;
      //! Structure for DMA status register
typedef struct {
	uint32_t CHANN_ENB:1,
		CHANN_ACT:1,
		reserved0:2,
		END_TR_ST:1,
		END_BF_ST:1, DESC_LDST:1, reserved1:9, BUFF_COUNT:16;
} udphs_dma_status_t;
      //! Structure for DMA descriptor
typedef struct {
	union {
		uint32_t nextdesc;
		udphs_dma_nextdesc_t NEXTDESC;
	};
	uint32_t addr;
	union {
		uint32_t control;
		udphs_dma_control_t CONTROL;
	};
} sam_udphs_dmadesc_t, UDPHS_dmadesc_t;
      //! Structure for DMA registers in a channel
typedef struct {
	union {
		uint32_t nextdesc;
		udphs_dma_nextdesc_t NEXTDESC;
	};
	uint32_t addr;
	union {
		uint32_t control;
		udphs_dma_control_t CONTROL;
	};
	union {
		unsigned long status;
		udphs_dma_status_t STATUS;
	};
} sam_udphs_dmach_t, udphs_dmach_t;

      //! DMA channel control command
#define  UDD_ENDPOINT_DMA_STOP_NOW                 (0)
#define  UDD_ENDPOINT_DMA_RUN_AND_STOP             (UDPHS_DMACONTROL_CHANN_ENB)
#define  UDD_ENDPOINT_DMA_LOAD_NEXT_DESC           (UDPHS_DMACONTROL_LDNXT_DSC)
#define  UDD_ENDPOINT_DMA_RUN_AND_LINK             (UDPHS_DMACONTROL_CHANN_ENB|UDPHS_DMACONTROL_LDNXT_DSC)
      //! Structure for DMA registers
#define  UDPHS_UDDMA_ARRAY(ep)                (((volatile udphs_dmach_t *)UDPHS->UDPHS_DMA)[(ep)])

      //! Set control desc to selected endpoint DMA channel
#define  udd_endpoint_dma_set_control(ep,desc)     (UDPHS_UDDMA_ARRAY(ep).control=desc)
      //! Get control desc to selected endpoint DMA channel
#define  udd_endpoint_dma_get_control(ep)          (UDPHS_UDDMA_ARRAY(ep).control)
      //! Set RAM address to selected endpoint DMA channel
#define  udd_endpoint_dma_set_addr(ep,add)         (UDPHS_UDDMA_ARRAY(ep).addr=add)
      //! Get status to selected endpoint DMA channel
#define  udd_endpoint_dma_get_status(ep)           (UDPHS_UDDMA_ARRAY(ep).status)
   //! @}
//! @}

//! @}
//! @}
//! @}
//! @}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond


#endif /* UDPHS_DEVICE_H_INCLUDED */
