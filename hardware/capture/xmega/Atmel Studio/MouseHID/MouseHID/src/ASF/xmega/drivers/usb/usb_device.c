/**
 * \file
 *
 * \brief USB Device driver
 * Compliance with common driver UDD
 *
 * Copyright (c) 2011 - 2013 Atmel Corporation. All rights reserved.
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

#include "conf_usb.h"

// Read Modify Write opcode is implemented after IAR AVR 5.51
#ifdef __ICCAVR__
#  if (__VER__ <= 551 || (__VER__ <= 611 && XMEGA_A1U) )
#    undef   USB_WORKAROUND_DO_NOT_USE_RMW
#    define  USB_WORKAROUND_DO_NOT_USE_RMW
#  endif
#endif

#include "sysclk.h"
#include "udd.h"
#include "usb_device.h"
#include <string.h>

#ifndef UDD_NO_SLEEP_MGR
#include "sleepmgr.h"
#endif

#ifndef UDD_USB_INT_LEVEL
// By default USB interrupt have low priority
#  define UDD_USB_INT_LEVEL USB_INTLVL_LO_gc
#endif



#ifdef USB_DEVICE_HS_SUPPORT
#error   This product does not support high speed mode, please remove define USB_DEVICE_HS_SUPPORT in conf_usb.h
#endif

////////////////////////////////////////////////////
//    USBB Device low-level driver (UDD)
////////////////////////////////////////////////////
/**
 * \ingroup udd_group
 * \defgroup udd_xmega_usb_group Xmega USB Device Driver
 *
 * \section USBB_CONF USBB Custom configuration
 * The following USBB driver configuration must be included in the conf_usb.h
 * file of the application.
 *
 * UDD_USB_INT_LEVEL<br>
 * Option to change the interrupt priority (USB_INTLVL_x_gc)
 * by default USB_INTLVL_LO_gc (recommended).
 *
 * \section Callbacks management
 * The USB driver is fully managed by interrupt and does not request periodic
 * task. Thereby, the USB events use callbacks to transfer the information.
 * The callbacks are declared in static during compilation or in variable during
 * code execution.
 *
 * Static declarations defined in conf_usb.h:
 * - UDC_VBUS_EVENT(bool b_present)<br>
 *   To signal Vbus level change
 * - UDC_SUSPEND_EVENT()<br>
 *   Called when USB bus enter in suspend mode
 * - UDC_RESUME_EVENT()<br>
 *   Called when USB bus is wakeup
 * - UDC_SOF_EVENT()<br>
 *   Called for each received SOF, Note: Each 1ms in HS/FS mode only.
 *
 * Dynamic callbacks, called "endpoint job" , are registered
 * in udd_ep_job_t structure via the following functions:
 * - udd_ep_run()<br>
 *   To call it when a transfer is finish
 * - udd_ep_wait_stall_clear()<br>
 *   To call it when a endpoint halt is disabled
 *
 * \section Power mode management
 * The Sleep modes authorized :
 * - in USB IDLE state, the USB needs of USB clock and authorizes up to IDLE mode
 * - in USB SUSPEND state, the USB no needs USB clock but requests a minimum
 *   clock restart timing. Thus, it authorizes up to POWER_DOWN or STANDBY mode.
 *
 * The USB_SLEEP_MODE_USB_IDLE equals SLEEPMGR_IDLE.
 *
 * The USB_SLEEP_MODE_USB_SUSPEND depends on USB clock startup timing:
 * | Clock Startup | Sleep mode authorized |
 * | >10ms         | SLEEPMGR_STDBY       |
 * | <=10ms        | SLEEPMGR_PDOWN       |
 *
 * @{
 */


// Check USB Device configuration
#ifndef USB_DEVICE_EP_CTRL_SIZE
#  error USB_DEVICE_EP_CTRL_SIZE not defined
#endif
#ifndef USB_DEVICE_MAX_EP
#  error USB_DEVICE_MAX_EP not defined
#endif


/**
 * \name Power management routine.
 */
//@{


#ifndef UDD_NO_SLEEP_MGR

//! Definition of sleep levels
#if ((defined USB_DEVICE_HS_SUPPORT) && (USBCLK_STARTUP_TIMEOUT>3000)) \
 || ((!defined USB_DEVICE_HS_SUPPORT) && (USBCLK_STARTUP_TIMEOUT>10000))
#  define USBC_SLEEP_MODE_USB_SUSPEND  SLEEPMGR_IDLE
#else
#  define USBC_SLEEP_MODE_USB_SUSPEND  SLEEPMGR_PDOWN
#endif
#define USBC_SLEEP_MODE_USB_IDLE  SLEEPMGR_IDLE

//! State of USB line
static bool udd_b_idle;


/*! \brief Authorize or not the CPU powerdown mode
 *
 * \param b_enable   true to authorize powerdown mode
 */
static void udd_sleep_mode(bool b_idle)
{
	if (!b_idle && udd_b_idle) {
		sleepmgr_unlock_mode(USBC_SLEEP_MODE_USB_IDLE);
	}
	if (b_idle && !udd_b_idle) {
		sleepmgr_lock_mode(USBC_SLEEP_MODE_USB_IDLE);
	}
	udd_b_idle = b_idle;
}
#else

static void udd_sleep_mode(bool b_idle) {
}
#endif  // UDD_NO_SLEEP_MGR

//@}

/**
 * \brief USB SRAM data about fifo, endpoint descriptor table and frame number
 *
 * The content of the USB SRAM can be:
 * - modified by USB hardware by interface to signal endpoint status.
 *   Thereby, it is read by software.
 * - modified by USB software to control endpoint.
 *   Thereby, it is read by hardware.
 * This data section is volatile and the specific opcode read/modify/write must be used.
 *
 * @{
 */
struct udd_sram_data {
#if XMEGA_A1U
#  if (0!=((USB_DEVICE_MAX_EP+1)%4))
	uint8_t padding_align[16 - ((USB_DEVICE_MAX_EP + 1) *
					sizeof(uint32_t)) % 16];
#  endif
#endif
	uint32_t fifo[USB_DEVICE_MAX_EP + 1];
	USB_EP_t ep_ctrl[2 * (USB_DEVICE_MAX_EP + 1)];
	uint16_t frame_number;
};
#if XMEGA_A1U
COMPILER_ALIGNED(16)
#else
COMPILER_ALIGNED(4) //! Caution seems GCC does not handle 2 alignment properly
#endif
static volatile struct udd_sram_data udd_sram;
#define UDD_EP_t  USB_EP_t volatile

// @}

/**
 * \name initialization of endpoint
 */
//@{
/**
 * \brief Configures and enables an endpoint
 *
 * \param ep               Endpoint number including direction (USB_EP_DIR_IN/USB_EP_DIR_OUT).
 * \param bmAttributes     Attribute of endpoint declared in descriptor.
 * \param MaxEndpointSize  Endpoint size maximum
 */
static void udd_ep_init(udd_ep_id_t ep, uint8_t bmAttributes,
		uint16_t MaxEndpointSize);

/**
 * \brief Returns a pointer on endpoint control SRAM corresponding at endpoint number
 *
 * \param ep               Endpoint number including direction (USB_EP_DIR_IN/USB_EP_DIR_OUT).
 *
 * \return endpoint descriptor index
 */
static UDD_EP_t *udd_ep_get_ctrl(udd_ep_id_t ep);
//@}


/**
 * \name Control endpoint low level management routine.
 *
 * This function performs control endpoint management.
 * It handle the SETUP/DATA/HANDSHAKE phases of a control transaction.
 */
//@{

//! Global variable to give and record information about setup request management
udd_ctrl_request_t udd_g_ctrlreq;

//! Bit definitions about endpoint control state machine for udd_ep_control_state
typedef enum {
	UDD_EPCTRL_SETUP = 0,     //!< Wait a SETUP packet
	UDD_EPCTRL_DATA_OUT = 1,  //!< Wait a OUT data packet
	UDD_EPCTRL_DATA_IN = 2,   //!< Wait a IN data packet
	UDD_EPCTRL_HANDSHAKE_WAIT_IN_ZLP = 3,  //!< Wait a IN ZLP packet
	UDD_EPCTRL_HANDSHAKE_WAIT_OUT_ZLP = 4, //!< Wait a OUT ZLP packet
	UDD_EPCTRL_STALL_REQ = 5, //!< STALL enabled on IN & OUT packet
} udd_ctrl_ep_state_t;

//! State of the endpoint control management
static udd_ctrl_ep_state_t udd_ep_control_state;
//! Total number of data received/sent during data packet phase with previous payload buffers
static uint16_t udd_ctrl_prev_payload_nb_trans;
//! Number of data received/sent to/from udd_g_ctrlreq.payload buffer
static uint16_t udd_ctrl_payload_nb_trans;

/**
 * \brief Buffer to store the data received on control endpoint (SETUP/OUT endpoint 0)
 *
 * Used to avoid a RAM buffer overflow in case of the payload buffer
 * is smaller than control endpoint size
 */
static uint8_t udd_ctrl_buffer[USB_DEVICE_EP_CTRL_SIZE];

/**
 * \brief Reset control endpoint management
 *
 * Called after a USB line reset or at the end of SETUP request (after ZLP)
 */
static void udd_ctrl_init(void);

//! \brief Managed reception of SETUP packet on control endpoint
static void udd_ctrl_setup_received(void);

//! \brief Managed reception of IN packet on control endpoint
static void udd_ctrl_in_sent(void);

//! \brief Managed reception of OUT packet on control endpoint
static void udd_ctrl_out_received(void);

//! \brief Managed underflow event of IN packet on control endpoint
//! It is used to detect a DATA phase stopped by the host via a ZLP request.
//! This is mandatory for chapter 8 compliance
static void udd_ctrl_underflow(void);

//! \brief Managed overflow event of OUT packet on control endpoint
//! It is used to detect a DATA phase stopped by the host via a ZLP request.
//! This is mandatory for chapter 8 compliance
static void udd_ctrl_overflow(void);

//! \brief Managed stall event of IN/OUT packet on control endpoint
static void udd_ctrl_stall_data(void);

//! \brief Send a ZLP IN on control endpoint
static void udd_ctrl_send_zlp_in(void);

//! \brief Send a ZLP OUT on control endpoint
static void udd_ctrl_send_zlp_out(void);

//! \brief Call callback associated to setup request
static void udd_ctrl_endofrequest(void);

/**
 * \brief Sub interrupt routine to manage error on control endpoint
 *
 * \return \c 1 if an error about control endpoint is occurred, otherwise \c 0.
 */
static bool udd_ctrl_interrupt_error(void);

/**
 * \brief Sub interrupt routine to manage a SETUP transfer complete on control endpoint
 *
 * \return \c 1 if an SETUP transfer complete about control endpoint is occurred,
 * otherwise \c 0.
 */
static bool udd_ctrl_interrupt_tc_setup(void);

//@}


/**
 * \name Management of bulk/interrupt/isochronous endpoints
 *
 * The UDD manages the data transfer on endpoints:
 * - Start data transfer on endpoint with USB Device DMA
 * - Send a ZLP packet if requested
 * - Call callback registered to signal end of transfer
 * The transfer abort and stall feature are supported.
 */
//@{
#if (0!=USB_DEVICE_MAX_EP)

//! Structure definition about job registered on an endpoint
typedef struct {
	//! A job is registered on this endpoint
	uint8_t busy:1;
	//! A short packet is requested for this job on endpoint IN
	uint8_t b_shortpacket:1;
	//! The cache buffer is currently used on endpoint OUT
	uint8_t b_use_out_cache_buffer:1;
	//! Buffer located in internal RAM to send or fill during job
	uint8_t *buf;
	//! Size of buffer to send or fill
	iram_size_t buf_size;
	//! Total number of data transfered on endpoint
	iram_size_t nb_trans;
	union {
		//! Callback to call at the end of transfer
		udd_callback_trans_t call_trans;
		//! Callback to call when the endpoint halt is cleared
		udd_callback_halt_cleared_t call_nohalt;
	};
} udd_ep_job_t;

//! Array to register a job on bulk/interrupt/isochronous endpoint
static udd_ep_job_t udd_ep_job[USB_DEVICE_MAX_EP * 2];

/**
 * \brief Buffer to store the data received on bulk/interrupt endpoints
 *
 * Used to avoid a RAM buffer overflow in case of the user buffer
 * is smaller than endpoint size
 *
 * \warning The isochronous endpoint is not protected by this system
 * and the user must always use a buffer corresponding at endpoint size
 */
#ifdef USB_DEVICE_LOW_SPEED
static uint8_t udd_ep_out_cache_buffer[USB_DEVICE_MAX_EP][8];
#else
static uint8_t udd_ep_out_cache_buffer[USB_DEVICE_MAX_EP][64];
#endif


/**
 * \brief Checks endpoint number
 *
 * \param ep         endpoint number
 */
bool udd_ep_is_valid(udd_ep_id_t ep);

/**
 * \brief Manages transfer complete on bulk/interrupt/isochronous endpoints
 *
 * \param ep   endpoint number to manage
 */
static void udd_ep_trans_complet(udd_ep_id_t ep);

/**
 * \brief Returns the size of endpoint
 *
 * \return the size of current selected endpoint
 */
static uint16_t udd_ep_get_size(UDD_EP_t * ep_ctrl);

/**
 * \brief Returns a pointer on endpoint job corresponding at endpoint number
 *
 * \param ep  Endpoint number including direction (USB_EP_DIR_IN/USB_EP_DIR_OUT).
 */
static udd_ep_job_t *udd_ep_get_job(udd_ep_id_t ep);

#endif // (0!=USB_DEVICE_MAX_EP)
//@}


void udd_enable(void)
{
	uint8_t i;
	irqflags_t flags;

	// Sanity check Silicon revision
#if AVR8_PART_IS_DEFINED(ATxmega128A1U)
	// The part ATxmega128A1U Rev. J is not supported, please use new silicon revision.
	Assert(!(MCU_REVID < 0x0A));
#endif

#ifdef CONFIG_OSC_AUTOCAL_RC32MHZ_REF_OSC
# if CONFIG_OSC_AUTOCAL_RC32MHZ_REF_OSC == OSC_ID_USBSOF
	// RC oscillator calibration via USB Start Of Frame is not available
	// in low speed mode.
	// Thus, the calibration is disabled
	// when USB interface start in low speed mode
	DFLLRC32M.CTRL = 0;
# endif
#endif

#ifdef USB_DEVICE_LOW_SPEED
	// The USB hardware need of 6MHz in low speed mode
	sysclk_enable_usb(6);
	udd_set_low_speed();
#else
	// The USB hardware need of 48MHz in full speed mode
	sysclk_enable_usb(48);
	udd_set_full_speed();
#endif

// The XMEGA_A1U does not support the RC calibration through Keepalive (Low speed).
#if (!defined USB_DEVICE_LOW_SPEED) || (!XMEGA_A1U)
# ifdef CONFIG_OSC_AUTOCAL_RC32MHZ_REF_OSC
#   if CONFIG_OSC_AUTOCAL_RC32MHZ_REF_OSC == OSC_ID_USBSOF
	// The SOF calibration can be enabled
	DFLLRC32M.CTRL = DFLL_ENABLE_bm;
#   endif
# endif
#endif

	flags = cpu_irq_save();

	// Reset endpoints table
	for (i = 0; i < ((USB_DEVICE_MAX_EP + 1) * 2); i++) {
		udd_sram.ep_ctrl[i].CTRL = 0;
	}
#if (0!=USB_DEVICE_MAX_EP)
	// Reset internal variables
	for (i = 0; i < (USB_DEVICE_MAX_EP * 2); i++) {
		udd_ep_job[i].busy = false;
	}
#endif

	//** Enable USB hardware
	usb_pad_init();
	udd_set_nb_max_ep(USB_DEVICE_MAX_EP);
	udd_enable_interface();
	udd_enable_store_frame_number();
#if XMEGA_A1U
	Assert(((uint16_t)(&udd_sram) & 0x0F) == 0); /* check align on 16bit */
#else
	Assert(((uint16_t)(&udd_sram) & 0x01) == 0); /* check align on WORD */
#endif
	udd_set_ep_table_addr(udd_sram.ep_ctrl);
	// Enable TC fifo management
	udd_enable_fifo();
	udd_reset_fifo();
	// Enable Interrupt USB Device
	udd_enable_interrupt(UDD_USB_INT_LEVEL);

#ifndef UDD_NO_SLEEP_MGR
	// Initialize the sleep mode authorized for the USB suspend mode
	udd_b_idle = false;
	sleepmgr_lock_mode(USBC_SLEEP_MODE_USB_SUSPEND);
#endif

#ifndef USB_DEVICE_ATTACH_AUTO_DISABLE
	udd_attach();
#endif
	cpu_irq_restore(flags);
}


void udd_disable(void)
{
	irqflags_t flags;
	flags = cpu_irq_save();
	udd_detach_device();
	// Disable interface
	USB_CTRLA = 0;
	USB_CTRLB = 0;
	sysclk_disable_usb();
	udd_sleep_mode(false);
#ifndef UDD_NO_SLEEP_MGR
	sleepmgr_unlock_mode(USBC_SLEEP_MODE_USB_SUSPEND);
#endif
	cpu_irq_restore(flags);
}

bool udd_include_vbus_monitoring(void)
{
	return false; // No Vbus monitoring
}

void udd_attach(void)
{
	irqflags_t flags;
	flags = cpu_irq_save();

	// At startup the USB bus state is unknown,
	// therefore the state is considered IDLE to not miss any USB event
	udd_sleep_mode(true);

	udd_ack_suspend_event();
	udd_ack_resume_event();
	udd_attach_device();
	// Enable main USB interrupts
	udd_enable_tc_interrupt();
	udd_enable_busevt_interrupt();
	udd_enable_setup_interrupt();
	udd_enable_start_of_frame_interrupt();

	cpu_irq_restore(flags);
}

void udd_detach(void)
{
	// Detach device from the bus
	udd_detach_device();
}

bool udd_is_high_speed(void)
{
	return false;
}

void udd_set_address(uint8_t address)
{
	udd_set_device_address(address);
}

uint8_t udd_getaddress(void)
{
	return udd_get_device_address();
}

uint16_t udd_get_frame_number(void)
{
	return udd_sram.frame_number;
}

uint16_t udd_get_micro_frame_number(void)
{
	return 0;
}

void udd_send_remotewakeup(void)
{
#ifndef UDD_NO_SLEEP_MGR
	if (!udd_b_idle)
#endif
	{
		udd_sleep_mode(true); // Enter in IDLE mode
		udd_send_remote_wake_up();
	}
}

void udd_set_setup_payload( uint8_t *payload, uint16_t payload_size )
{
	udd_g_ctrlreq.payload = payload;
	udd_g_ctrlreq.payload_size = payload_size;
}

#if (0!=USB_DEVICE_MAX_EP)
bool udd_ep_alloc(udd_ep_id_t ep, uint8_t bmAttributes,
		uint16_t MaxEndpointSize)
{
	UDD_EP_t *ep_ctrl;
	Assert(udd_ep_is_valid(ep));

	ep_ctrl = udd_ep_get_ctrl(ep);
	if (udd_endpoint_is_enable(ep_ctrl)) {
		return false; // Already allocated
	}
	udd_ep_init(ep, bmAttributes, MaxEndpointSize);

	// Do not use multipacket mode with isochronous 1023 bytes endpoint
	if (udd_endpoint_get_type(ep_ctrl)==USB_EP_TYPE_ISOCHRONOUS_gc
			&& (udd_endpoint_get_size_field(ep_ctrl)
			==USB_EP_BUFSIZE_1023_gc)) {
		return true;
	}

	udd_endpoint_set_multipacket(ep_ctrl);
	return true;
}

void udd_ep_free(udd_ep_id_t ep)
{
	UDD_EP_t *ep_ctrl;
	Assert(udd_ep_is_valid(ep));

	udd_ep_abort(ep);
	ep_ctrl = udd_ep_get_ctrl(ep);
	udd_endpoint_disable(ep_ctrl);
}

bool udd_ep_is_halted(udd_ep_id_t ep)
{
	UDD_EP_t *ep_ctrl;
	Assert(udd_ep_is_valid(ep));

	ep_ctrl = udd_ep_get_ctrl(ep);
	return (udd_endpoint_is_stall(ep_ctrl));
}

bool udd_ep_set_halt(udd_ep_id_t ep)
{
	UDD_EP_t *ep_ctrl;
	Assert(udd_ep_is_valid(ep));

	ep_ctrl = udd_ep_get_ctrl(ep);
	udd_endpoint_enable_stall(ep_ctrl);
	udd_endpoint_clear_dtgl(ep_ctrl);

	udd_ep_abort(ep);
	return true;
}

bool udd_ep_clear_halt(udd_ep_id_t ep)
{
	udd_ep_job_t *ptr_job;
	UDD_EP_t *ep_ctrl;
	Assert(udd_ep_is_valid(ep));

	ep_ctrl = udd_ep_get_ctrl(ep);
	if (!udd_endpoint_is_stall(ep_ctrl)) {
		return true; // No stall on going
	}
	udd_endpoint_disable_stall(ep_ctrl);

	// If a job is register on clear halt action
	// then execute callback
	ptr_job = udd_ep_get_job(ep);
	if (ptr_job->busy == true) {
		ptr_job->busy = false;
		ptr_job->call_nohalt();
	}
	return true;
}

bool udd_ep_run(udd_ep_id_t ep, bool b_shortpacket, uint8_t * buf,
		iram_size_t buf_size, udd_callback_trans_t callback)
{
	udd_ep_job_t *ptr_job;
	irqflags_t flags;
	UDD_EP_t *ep_ctrl;

	Assert(udd_ep_is_valid(ep));

	// Get control & job about this endpoint
	ptr_job = udd_ep_get_job(ep);
	ep_ctrl = udd_ep_get_ctrl(ep);

	if (!udd_endpoint_is_enable(ep_ctrl)) {
		return false; // Endpoint not allocated
	}
	if (udd_endpoint_get_type(ep_ctrl)!=USB_EP_TYPE_ISOCHRONOUS_gc
		&& udd_endpoint_is_stall(ep_ctrl)) {
		return false; // Endpoint is halted
	}
	flags = cpu_irq_save();
	if (ptr_job->busy == true) {
		cpu_irq_restore(flags);
		return false; // Job already on going
	}
	ptr_job->busy = true;
	cpu_irq_restore(flags);


	// Update Job information
	ptr_job->buf = buf;
	ptr_job->buf_size = buf_size;
	ptr_job->nb_trans = 0;
	ptr_job->call_trans = callback;
	// Need to enable shortpacket to send a ZLP (buf_size==0)
	ptr_job->b_shortpacket = b_shortpacket || (buf_size==0);
	ptr_job->b_use_out_cache_buffer = false;

	// Initialize value to simulate a empty transfer
	if (USB_EP_DIR_IN == (ep & USB_EP_DIR_IN)) {
		udd_endpoint_in_reset_nb_sent(ep_ctrl);
	}
	else
	{
		if ((USB_EP_TYPE_ISOCHRONOUS_gc == udd_endpoint_get_type(ep_ctrl))
		&& (0 != (buf_size % udd_ep_get_size(ep_ctrl)))) {
			// The user must use a buffer size modulo endpoint size
			ptr_job->busy = false;
			return false;
		}
		udd_endpoint_out_reset_nb_received(ep_ctrl);
		udd_endpoint_out_set_nbbyte(ep_ctrl, 0);
	}
	// Request next transfer
	udd_ep_trans_complet(ep);
	return true;
}

void udd_ep_abort(udd_ep_id_t ep)
{
	UDD_EP_t *ep_ctrl;
	udd_ep_job_t *ptr_job;
	Assert(udd_ep_is_valid(ep));

	ep_ctrl = udd_ep_get_ctrl(ep);
	ptr_job = udd_ep_get_job(ep);

	// Stop transfer
	udd_endpoint_set_NACK0(ep_ctrl);
	if (ptr_job->busy == false) {
		return; // No job on going
	}
	ptr_job->busy = false;
	if (NULL != ptr_job->call_trans) {
		ptr_job->call_trans(UDD_EP_TRANSFER_ABORT,
				(ep & USB_EP_DIR_IN) ?
				udd_endpoint_in_nb_sent(ep_ctrl)
				: udd_endpoint_out_nb_receiv(ep_ctrl),
				ep);
	}
}

bool udd_ep_wait_stall_clear(udd_ep_id_t ep,
		udd_callback_halt_cleared_t callback)
{
	udd_ep_job_t *ptr_job;
	UDD_EP_t *ep_ctrl;
	Assert(udd_ep_is_valid(ep));

	ep_ctrl = udd_ep_get_ctrl(ep);
	ptr_job = udd_ep_get_job(ep);

	if (udd_endpoint_is_stall(ep_ctrl)) {
		// Wait clear halt endpoint
		if (ptr_job->busy == true) {
			return false; // Job already on going
		}
		ptr_job->busy = true;
		ptr_job->call_nohalt = callback;
	} else {
		// endpoint not halted then call directly callback
		callback();
	}
	return true;
}
#endif // (0!=USB_DEVICE_MAX_EP)

//--------------------------------------------------------
//--- INTERNAL ROUTINES TO MANAGED GLOBAL EVENTS

/**
 * \internal
 * \brief Function called by USB bus event interrupt
 *
 * USB bus event interrupt includes :
 * - USB line events SOF, reset, suspend, resume, wakeup
 * - endpoint control errors underflow, overflow, stall
 */
ISR(USB_BUSEVENT_vect)
{
	if (udd_is_start_of_frame_event()) {
		udd_ack_start_of_frame_event();
		udc_sof_notify();
#ifdef UDC_SOF_EVENT
		UDC_SOF_EVENT();
#endif
		goto udd_interrupt_bus_event_end;
	}

	if (udd_ctrl_interrupt_error()) {
		goto udd_interrupt_bus_event_end;
	}
	if (udd_is_reset_event()) {
		udd_ack_reset_event();
#if (0!=USB_DEVICE_MAX_EP)
		// Abort all endpoint jobs on going
		uint8_t i;
		for (i = 1; i < USB_DEVICE_MAX_EP; i++) {
			udd_ep_abort(i);
			udd_ep_abort(i | USB_EP_DIR_IN);
		}
#endif
		udc_reset();

		// Reset USB address to 0
		udd_set_device_address(0);
		// Alloc and configure control endpoint
		udd_ep_init(0, USB_EP_TYPE_CONTROL, USB_DEVICE_EP_CTRL_SIZE);
		udd_ep_init(0 | USB_EP_DIR_IN, USB_EP_TYPE_CONTROL,
				USB_DEVICE_EP_CTRL_SIZE);
		udd_control_out_set_buf(&udd_ctrl_buffer);
		// Reset endpoint control management
		udd_ctrl_init();
		goto udd_interrupt_bus_event_end;
	}

	if (udd_is_suspend_event()) {
		udd_ack_suspend_event();
		udd_sleep_mode(false); // Enter in SUSPEND mode
#ifdef UDC_SUSPEND_EVENT
		UDC_SUSPEND_EVENT();
#endif
		goto udd_interrupt_bus_event_end;
	}

	if (udd_is_resume_event()) {
		udd_ack_resume_event();
		udd_sleep_mode(true); // Enter in power reduction mode
#ifdef UDC_RESUME_EVENT
		UDC_RESUME_EVENT();
#endif
		goto udd_interrupt_bus_event_end;
	}

udd_interrupt_bus_event_end:
	return;
}

/**
 * \internal
 * \brief Function called by USB transfer complete interrupt
 *
 * USB transfer complete interrupt includes events about endpoint transfer on all endpoints.
 */
ISR(USB_TRNCOMPL_vect)
{
#if (0!=USB_DEVICE_MAX_EP)
	uint8_t ep_index;
	uint8_t i_fifo;
	uint16_t ad;
	uint16_t *p_ad;
	int8_t rp;
	UDD_EP_t *ep_ctrl;
	udd_ep_id_t ep;
#endif

	if (!udd_is_tc_event()) {
		// If no other transfer complete
		// then check reception of SETUP packet on control endpoint
		if (udd_ctrl_interrupt_tc_setup()) {
			// Interrupt acked by control endpoint managed
			goto udd_interrupt_tc_end;
		}
		Assert(false);
	}
	// Check IN/OUT transfer complete on all endpoints
	udd_ack_tc_event();

#if (0!=USB_DEVICE_MAX_EP)
	//** Decode TC FIFO
	// Compute ep addr
	rp = udd_get_fifo_rp();
	i_fifo = 2 * (1 + ~rp);
	ad = ((uint16_t) udd_sram.ep_ctrl) - i_fifo;
	p_ad = (uint16_t *) ad;
	// Compute ep
	ep_index = (((uint16_t) * p_ad - ((uint16_t) udd_sram.ep_ctrl)) >> 3);
	ep = (ep_index / 2) + ((ep_index & 1) ? USB_EP_DIR_IN : 0);
	Assert(USB_DEVICE_MAX_EP >= (ep & USB_EP_ADDR_MASK));

	// Ack IT TC of endpoint
	ep_ctrl = udd_ep_get_ctrl(ep);
	if (!udd_endpoint_transfer_complete(ep_ctrl)) {
		return; // Error, TC is generated by Multipacket transfer
	}
	udd_endpoint_ack_transfer_complete(ep_ctrl);

	// Check status on control endpoint
	if (ep == 0) {
		udd_ctrl_out_received();
		goto udd_interrupt_tc_end; // Interrupt acked by control endpoint managed
	}
	if (ep == (0 | USB_EP_DIR_IN)) {
		udd_ctrl_in_sent();
		goto udd_interrupt_tc_end; // Interrupt acked by control endpoint managed
	}
	Assert(udd_ep_is_valid(ep));
	// Manage end of transfer on endpoint bulk/interrupt/isochronous
	udd_ep_trans_complet(ep);

#else

	udd_get_fifo_rp();
	if (udd_endpoint_transfer_complete(udd_ep_get_ctrl(0))) {
		udd_endpoint_ack_transfer_complete(udd_ep_get_ctrl(0));
		udd_ctrl_out_received();
	}else{
		udd_endpoint_ack_transfer_complete(udd_ep_get_ctrl(0 | USB_EP_DIR_IN));
		udd_ctrl_in_sent();
	}
#endif

udd_interrupt_tc_end:
	return;
}

//--------------------------------------------------------
//--- INTERNAL ROUTINES TO INITIALIZE ENDPOINT

static void udd_ep_init(udd_ep_id_t ep, uint8_t bmAttributes,
		uint16_t MaxEndpointSize)
{
	USB_EP_TYPE_t type = USB_EP_TYPE_CONTROL_gc;
	USB_EP_BUFSIZE_t size;
	UDD_EP_t *ep_ctrl;

#if (0!=USB_DEVICE_MAX_EP)
	// Translate USB attribute to hardware defines
	switch (bmAttributes & USB_EP_TYPE_MASK) {
	case USB_EP_TYPE_CONTROL:
		type = USB_EP_TYPE_CONTROL_gc;
		break;
	case USB_EP_TYPE_ISOCHRONOUS:
		type = USB_EP_TYPE_ISOCHRONOUS_gc;
		break;
	case USB_EP_TYPE_BULK:
	case USB_EP_TYPE_INTERRUPT: //interrupt behaves as bulk
		type = USB_EP_TYPE_BULK_gc;
		break;
	default:
		Assert(false); // Wrong value
		break;
	}
#else
	type = USB_EP_TYPE_CONTROL_gc;
#endif

	// Translate USB endpoint size to hardware defines
	switch (MaxEndpointSize) {
	default:
		Assert(false); // Wrong value
	case 8:
		size = USB_EP_BUFSIZE_8_gc;
		break;
	case 16:
		size = USB_EP_BUFSIZE_16_gc;
		break;
	case 32:
		size = USB_EP_BUFSIZE_32_gc;
		break;
	case 64:
		size = USB_EP_BUFSIZE_64_gc;
		break;
#if (0!=USB_DEVICE_MAX_EP)
	case 128:
		size = USB_EP_BUFSIZE_128_gc;
		break;
	case 256:
		size = USB_EP_BUFSIZE_256_gc;
		break;
	case 512:
		size = USB_EP_BUFSIZE_512_gc;
		break;
	case 1023:
		size =USB_EP_BUFSIZE_1023_gc;
		break;
#endif
	}

	// Enable endpoint
	ep_ctrl = udd_ep_get_ctrl(ep);
	udd_endpoint_disable(ep_ctrl);
	udd_endpoint_clear_status(ep_ctrl);
	udd_endpoint_set_control(ep_ctrl, (uint8_t) type | (uint8_t) size);
}

static UDD_EP_t *udd_ep_get_ctrl(udd_ep_id_t ep)
{
	return &udd_sram.ep_ctrl[(2 * (ep & USB_EP_ADDR_MASK) +
			((ep & USB_EP_DIR_IN) ? 1 : 0))];
}


//--------------------------------------------------------
//--- INTERNAL ROUTINES TO MANAGED THE CONTROL ENDPOINT

static void udd_ctrl_init(void)
{
	udd_disable_overflow_interrupt();
	udd_disable_underflow_interrupt();

	// Clear status flag from control endpoints
	// Mandatory for ATxmega128A1 Rev. K
	udd_control_in_set_NACK0();
	udd_control_in_set_bytecnt(0);
	udd_control_in_ack_tc();
	udd_control_ack_in_underflow();
	udd_control_out_ack_tc();
	udd_control_ack_out_overflow();

	udd_g_ctrlreq.callback = NULL;
	udd_g_ctrlreq.over_under_run = NULL;
	udd_g_ctrlreq.payload_size = 0;
	udd_ep_control_state = UDD_EPCTRL_SETUP;
}

static void udd_ctrl_setup_received(void)
{
	if (UDD_EPCTRL_SETUP != udd_ep_control_state) {
		if ((UDD_EPCTRL_HANDSHAKE_WAIT_IN_ZLP == udd_ep_control_state)
				|| (UDD_EPCTRL_HANDSHAKE_WAIT_OUT_ZLP == udd_ep_control_state)) {
			// Accept that ZLP event can be hidden by setup packet event
			// in case of setup packet sending quickly after a ZLP
			udd_ctrl_endofrequest();
		}
		// Reinitializes control endpoint management
		udd_ctrl_init();
	}
	// Fill setup request structure
	if (8 != udd_control_out_get_bytecnt())
		return; // Error data number don't correspond to SETUP packet
	memcpy((uint8_t *) & udd_g_ctrlreq.req, udd_ctrl_buffer, 8);

	// To detect a protocol error on setup, enable nak interrupt on IN/OUT of control endpoint
	udd_enable_overflow_interrupt();
	udd_enable_underflow_interrupt();

	// Decode setup request
	if (udc_process_setup() == false) {
		// Setup request unknown then stall it
		udd_ctrl_stall_data();
		return;
	}

	if (Udd_setup_is_in()) {
		udd_ctrl_prev_payload_nb_trans = 0;
		udd_ctrl_payload_nb_trans = 0;
		udd_ep_control_state = UDD_EPCTRL_DATA_IN;
		udd_ctrl_in_sent(); // Send first data transfer
	} else {
		if (0 == udd_g_ctrlreq.req.wLength) {
			// No data phase requested
			// Send IN ZLP to ACK setup request
			udd_ctrl_send_zlp_in();
			return;
		}
		// OUT data phase requested
		udd_ctrl_prev_payload_nb_trans = 0;
		udd_ctrl_payload_nb_trans = 0;
		udd_ep_control_state = UDD_EPCTRL_DATA_OUT;
		// Clear packet to receive first packet
		udd_control_out_clear_NACK0();
	}
}

static void udd_ctrl_in_sent(void)
{
	static bool b_shortpacket = false;
	uint16_t nb_remain;

	if (UDD_EPCTRL_HANDSHAKE_WAIT_IN_ZLP == udd_ep_control_state) {
		// ZLP on IN is sent, then valid end of setup request
		udd_ctrl_endofrequest();
		// Reinitializes control endpoint management
		udd_ctrl_init();
		return;
	}
	Assert(udd_ep_control_state == UDD_EPCTRL_DATA_IN);

	nb_remain = udd_g_ctrlreq.payload_size - udd_ctrl_payload_nb_trans;
	if (0 == nb_remain) {
		// Update number of total data sending by previous playload buffer
		udd_ctrl_prev_payload_nb_trans += udd_ctrl_payload_nb_trans;
		if ((udd_g_ctrlreq.req.wLength == udd_ctrl_prev_payload_nb_trans)
				|| b_shortpacket) {
			// All data requested are transfered or a short packet has been sent
			// then it is the end of data phase.
			// Generate an OUT ZLP for handshake phase.
			udd_ctrl_send_zlp_out();
			return;
		}
		// Need of new buffer because the data phase is not complete
		if ((!udd_g_ctrlreq.over_under_run)
				|| (!udd_g_ctrlreq.over_under_run())) {
			// Underrun then send zlp on IN
			// nb_remain == 0 allows to send a IN ZLP
		} else {
			// A new payload buffer is given
			udd_ctrl_payload_nb_trans = 0;
			nb_remain = udd_g_ctrlreq.payload_size;
		}
	}
	// Continue transfer an send next data
	if (nb_remain >= USB_DEVICE_EP_CTRL_SIZE) {
		nb_remain = USB_DEVICE_EP_CTRL_SIZE;
		b_shortpacket = false;
	} else {
		b_shortpacket = true;
	}
	udd_control_in_set_bytecnt(nb_remain);

	// Link payload buffer directly on USB hardware
	udd_control_in_set_buf(udd_g_ctrlreq.payload + udd_ctrl_payload_nb_trans);
	udd_ctrl_payload_nb_trans += nb_remain;

	// Valid and sent the data available in control endpoint buffer
	udd_control_in_clear_NACK0();
}

static void udd_ctrl_out_received(void)
{
	uint16_t nb_data;

	if (UDD_EPCTRL_HANDSHAKE_WAIT_OUT_ZLP == udd_ep_control_state) {
		// Valid end of setup request
		udd_ctrl_endofrequest();
		// Reinitializes control endpoint management
		udd_ctrl_init();
		return;
	}
	Assert(udd_ep_control_state == UDD_EPCTRL_DATA_OUT);

	// Read data received during OUT phase
	nb_data = udd_control_out_get_bytecnt();

	if (udd_g_ctrlreq.payload_size < (udd_ctrl_payload_nb_trans + nb_data)) {
		// Payload buffer too small, ignore data remaining
		nb_data = udd_g_ctrlreq.payload_size - udd_ctrl_payload_nb_trans;
	}

	memcpy((uint8_t *) (udd_g_ctrlreq.payload + udd_ctrl_payload_nb_trans),
			udd_ctrl_buffer, nb_data);
	udd_ctrl_payload_nb_trans += nb_data;

	if ((USB_DEVICE_EP_CTRL_SIZE != nb_data) || (udd_g_ctrlreq.req.wLength
			<= (udd_ctrl_prev_payload_nb_trans
			+ udd_ctrl_payload_nb_trans))) {
		// End of reception because it is a short packet
		// or all data are transfered

		// Before send ZLP, call intermediate callback
		// in case of data receive generate a stall
		udd_g_ctrlreq.payload_size = udd_ctrl_payload_nb_trans;
		if (NULL != udd_g_ctrlreq.over_under_run) {
			if (!udd_g_ctrlreq.over_under_run()) {
				// Stall ZLP
				udd_ctrl_stall_data();
				return;
			}
		}
		// Send IN ZLP to ACK setup request
		udd_ctrl_send_zlp_in();
		return;
	}

	if (udd_g_ctrlreq.payload_size == udd_ctrl_payload_nb_trans) {
		// Overrun then request a new payload buffer
		if (!udd_g_ctrlreq.over_under_run) {
			// No callback available to request a new payload buffer
			udd_ctrl_stall_data();
			return;
		}
		if (!udd_g_ctrlreq.over_under_run()) {
			// No new payload buffer delivered
			udd_ctrl_stall_data();
			return;
		}
		// New payload buffer available
		// Update number of total data received
		udd_ctrl_prev_payload_nb_trans += udd_ctrl_payload_nb_trans;
		// Reinit reception on payload buffer
		udd_ctrl_payload_nb_trans = 0;
	}
	// Free buffer of OUT control endpoint to authorize next reception
	udd_control_out_clear_NACK0();
}

static void udd_ctrl_underflow(void)
{
	if (udd_is_tc_event() || udd_ctrl_interrupt_tc_setup()) {
		return; // underflow ignored if a transfer complete has been no processed
	}
	if (UDD_EPCTRL_DATA_OUT == udd_ep_control_state) {
		// Host want to stop OUT transaction
		// then stop to wait OUT data phase and wait IN ZLP handshake
		udd_ctrl_send_zlp_in();
	} else if (UDD_EPCTRL_HANDSHAKE_WAIT_OUT_ZLP == udd_ep_control_state) {
		// A OUT handshake is waiting by device,
		// but host want extra IN data then stall extra IN data and following status stage
		udd_control_in_enable_stall();
		udd_control_out_enable_stall();
	}
}

static void udd_ctrl_overflow(void)
{
	if (udd_is_tc_event() || udd_ctrl_interrupt_tc_setup()) {
		return; // overflow ignored if a transfer complete has been no processed
	}
	if (UDD_EPCTRL_DATA_IN == udd_ep_control_state) {
		// Host want to stop IN transaction
		// then stop to wait IN data phase and wait OUT ZLP handshake
		udd_ctrl_send_zlp_out();
	} else if (UDD_EPCTRL_HANDSHAKE_WAIT_IN_ZLP == udd_ep_control_state) {
		// A IN handshake is waiting by device,
		// but host want extra OUT data then stall extra OUT data and following status stage
		udd_control_in_enable_stall();
		udd_control_out_enable_stall();
	}
}

static void udd_ctrl_stall_data(void)
{
	// Stall all packets on IN & OUT control endpoint
	udd_ep_control_state = UDD_EPCTRL_STALL_REQ;
	udd_control_in_enable_stall();
	udd_control_out_enable_stall();
}

static void udd_ctrl_send_zlp_in(void)
{
	udd_ep_control_state = UDD_EPCTRL_HANDSHAKE_WAIT_IN_ZLP;
	// Valid and sent empty IN packet on control endpoint
	udd_control_in_set_bytecnt(0);
	udd_control_in_clear_NACK0();
}

static void udd_ctrl_send_zlp_out(void)
{
	udd_ep_control_state = UDD_EPCTRL_HANDSHAKE_WAIT_OUT_ZLP;
	// Valid reception of OUT packet on control endpoint
	udd_control_out_clear_NACK0();
}

static void udd_ctrl_endofrequest(void)
{
	// If a callback is registered then call it
	if (udd_g_ctrlreq.callback) {
		udd_g_ctrlreq.callback();
	}
}

static bool udd_ctrl_interrupt_error(void)
{
	// Underflow only managed for control endpoint
	if (udd_is_underflow_event()) {
		udd_ack_underflow_event();
		if (udd_control_in_underflow()) {
			udd_ctrl_underflow();
		}
		return true;
	}
	// Overflow only managed for control endpoint
	if (udd_is_overflow_event()) {
		udd_ack_overflow_event();
		if (udd_control_out_overflow()) {
			udd_ctrl_overflow();
		}
		return true;
	}
	return false;
}

static bool udd_ctrl_interrupt_tc_setup(void)
{
	if (!udd_is_setup_event()) {
		return false;
	}
	udd_ack_setup_event();

	// Clear eventually previous stall events
	udd_control_out_ack_stall();
	udd_control_in_ack_stall();
	udd_ack_stall_event();

	Assert(udd_control_setup()); // A setup must be received on control endpoint

	// Ack SETUP packet and decode request
	udd_control_ack_setup();
	udd_ctrl_setup_received();
	return true;
}


//--------------------------------------------------------
//--- INTERNAL ROUTINES TO MANAGED THE BULK/INTERRUPT/ISOCHRONOUS ENDPOINTS

#if (0!=USB_DEVICE_MAX_EP)

static uint16_t udd_ep_get_size(UDD_EP_t * ep_ctrl)
{
	// Translate hardware defines to USB endpoint size
	switch (udd_endpoint_get_size_field(ep_ctrl)) {
	default:
	case USB_EP_BUFSIZE_8_gc:
		return 8;
	case USB_EP_BUFSIZE_16_gc:
		return 16;
	case USB_EP_BUFSIZE_32_gc:
		return 32;
	case USB_EP_BUFSIZE_64_gc:
		return 64;
	case USB_EP_BUFSIZE_128_gc:
		return 128;
	case USB_EP_BUFSIZE_256_gc:
		return 256;
	case USB_EP_BUFSIZE_512_gc:
		return 512;
	case USB_EP_BUFSIZE_1023_gc:
		return 1023;
	}
}

static udd_ep_job_t *udd_ep_get_job(udd_ep_id_t ep)
{
	return &udd_ep_job[(2 * (ep & USB_EP_ADDR_MASK) +
		((ep & USB_EP_DIR_IN) ? 1 : 0)) - 2];
}

bool udd_ep_is_valid(udd_ep_id_t ep)
{
	ep &= USB_EP_ADDR_MASK;
	if (ep == 0) {
		return false;
	}
	return (USB_DEVICE_MAX_EP >= ep);
}

static void udd_ep_trans_complet(udd_ep_id_t ep)
{
	UDD_EP_t *ep_ctrl;
	udd_ep_job_t *ptr_job;
	uint16_t ep_size, nb_trans;
	iram_size_t next_trans;

	ptr_job = udd_ep_get_job(ep);
	ep_ctrl = udd_ep_get_ctrl(ep);
	ep_size = udd_ep_get_size(ep_ctrl);

	if (USB_EP_DIR_IN == (ep & USB_EP_DIR_IN)) {
		// Transfer complete on IN
		nb_trans = udd_endpoint_in_nb_sent(ep_ctrl);

		// Update number of data transfered
		ptr_job->nb_trans += nb_trans;

		// Need to send other data
		if (ptr_job->nb_trans != ptr_job->buf_size) {
			next_trans = ptr_job->buf_size - ptr_job->nb_trans;
			if (UDD_ENDPOINT_MAX_TRANS < next_trans) {
				// The USB hardware support a maximum
				// transfer size of UDD_ENDPOINT_MAX_TRANS Bytes
				next_trans = UDD_ENDPOINT_MAX_TRANS -
					(UDD_ENDPOINT_MAX_TRANS % ep_size);
			}
			// Need ZLP, if requested and last packet is not a short packet
			ptr_job->b_shortpacket = ptr_job->b_shortpacket
				&& (0==(next_trans % ep_size));
			udd_endpoint_in_reset_nb_sent(ep_ctrl);
			udd_endpoint_in_set_bytecnt(ep_ctrl, next_trans);
			// Link the user buffer directly on USB hardware DMA
			udd_endpoint_set_buf(ep_ctrl, &ptr_job->buf[ptr_job->nb_trans]);
			udd_endpoint_clear_NACK0(ep_ctrl);
			return;
		}

		// Need to send a ZLP after all data transfer
		if (ptr_job->b_shortpacket) {
			ptr_job->b_shortpacket = false;
			udd_endpoint_in_reset_nb_sent(ep_ctrl);
			udd_endpoint_in_set_bytecnt(ep_ctrl, 0);
			udd_endpoint_clear_NACK0(ep_ctrl);
			return;
		}
	}
	else
	{
		// Transfer complete on OUT
		nb_trans = udd_endpoint_out_nb_receiv(ep_ctrl);

		// Can be necessary to copy data receive from cache buffer to user buffer
		if (ptr_job->b_use_out_cache_buffer) {
			memcpy(&ptr_job->buf[ptr_job->nb_trans]
				, udd_ep_out_cache_buffer[ep - 1]
				, ptr_job->buf_size % ep_size);
		}

		// Update number of data transfered
		ptr_job->nb_trans += nb_trans;
		if (ptr_job->nb_trans > ptr_job->buf_size) {
			ptr_job->nb_trans = ptr_job->buf_size;
		}

		// If all previous data requested are received and user buffer not full
		// then need to receive other data
		if ((nb_trans == udd_endpoint_out_get_nbbyte_requested(ep_ctrl))
			&& (ptr_job->nb_trans != ptr_job->buf_size)) {
			next_trans = ptr_job->buf_size - ptr_job->nb_trans;
			if (UDD_ENDPOINT_MAX_TRANS < next_trans) {
				// The USB hardware support a maximum transfer size
				// of UDD_ENDPOINT_MAX_TRANS Bytes
				next_trans = UDD_ENDPOINT_MAX_TRANS
					- (UDD_ENDPOINT_MAX_TRANS % ep_size);
			} else {
				next_trans -= next_trans % ep_size;
			}

			udd_endpoint_out_reset_nb_received(ep_ctrl);
			if (next_trans < ep_size) {
				// Use the cache buffer for Bulk or Interrupt size endpoint
				ptr_job->b_use_out_cache_buffer = true;
				udd_endpoint_set_buf( ep_ctrl,
					udd_ep_out_cache_buffer[ep - 1]);
				udd_endpoint_out_set_nbbyte(ep_ctrl, ep_size);
			} else {
				// Link the user buffer directly on USB hardware DMA
				udd_endpoint_set_buf(ep_ctrl, &ptr_job->buf[ptr_job->nb_trans]);
				udd_endpoint_out_set_nbbyte(ep_ctrl, next_trans);
			}
			// Start transfer
			udd_endpoint_clear_NACK0(ep_ctrl);
			return;
		}
	}

	// Job complete then call callback
	if (ptr_job->busy) {
		ptr_job->busy = false;
		if (NULL != ptr_job->call_trans) {
			ptr_job->call_trans(UDD_EP_TRANSFER_OK,
				ptr_job->nb_trans,
				ep);
		}
	}
	return;
}
#endif // (0!=USB_DEVICE_MAX_EP)
//@}
