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

#include "conf_usb.h"
#include "sysclk.h"
#include "udd.h"
#include "udphs_device.h"
#include <string.h>

#ifndef UDD_NO_SLEEP_MGR
#  include "sleep.h"
#  include "sleepmgr.h"
#endif

#if !(SAM3U)
#  error The current UDPHS Device Driver supports only SAM3U.
#endif

#ifdef USB_DEVICE_LOW_SPEED
#  error The Low speed mode is not supported on this part, please remove USB_DEVICE_LOW_SPEED in conf_usb.h
#endif

#ifndef UDD_USB_INT_FUN
#  define UDD_USB_INT_FUN UDPHS_Handler
#endif

#ifndef UDD_USB_INT_LEVEL
#  define UDD_USB_INT_LEVEL 5 // By default USB interrupt have low priority
#endif

#ifndef UDC_VBUS_EVENT
#  define UDC_VBUS_EVENT(present)
#endif

#define UDD_EP_USED(ep)      (USB_DEVICE_MAX_EP >= ep)

// for debug text
//#define dbg_print printf
#define dbg_print(...)

/**
 * \ingroup udd_group
 * \defgroup udd_udphs_group USB Device High-Speed Port (UDPHS)
 *
 * \section UDPHS_CONF UDPHS Custom configuration
 * The following UDPHS driver configuration must be included in the conf_usb.h
 * file of the application.
 *
 * UDD_USB_INT_LEVEL<br>
 * Option to change the interrupt priority (0 to 15) by default 5 (recommended).
 *
 * UDD_USB_INT_FUN<br>
 * Option to fit interrupt function to what defined in exception table.
 *
 * UDD_ISOCHRONOUS_NB_BANK(ep)<br>
 * Feature to reduce or increase isochronous endpoints buffering (1 to 3).
 * Default value 2.
 *
 * UDD_BULK_NB_BANK(ep)<br>
 * Feature to reduce or increase bulk endpoints buffering (1 to 2).
 * Default value 2.
 *
 * UDD_INTERRUPT_NB_BANK(ep)<br>
 * Feature to reduce or increase interrupt endpoints buffering (1 to 2).
 * Default value 1.
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
 * - in USB IDLE state, the UDPHS needs of USB clock and authorizes up to sleep mode WFI.
 * - in USB SUSPEND state, the UDPHS no needs USB clock and authorizes up to sleep mode WAIT.
 * @{
 */

// Check USB Device configuration
#ifndef USB_DEVICE_EP_CTRL_SIZE
#  error USB_DEVICE_EP_CTRL_SIZE not defined
#endif
#ifndef USB_DEVICE_MAX_EP
#  error USB_DEVICE_MAX_EP not defined
#endif
#if USB_DEVICE_MAX_EP > (UDD_MAX_PEP_NB-1) // USB_DEVICE_MAX_EP does not include control endpoint
#  error USB_DEVICE_MAX_EP is too high and not supported by this part
#endif

#define UDD_EP_ISO_NBANK_ERROR(ep)            \
	( (UDD_ISOCHRONOUS_NB_BANK(ep) < 1)   \
		|| (UDD_ISOCHRONOUS_NB_BANK(ep) > 3) )
#define UDD_EP_BULK_NBANK_ERROR(ep)           \
	( (UDD_BULK_NB_BANK(ep) < 1) || (UDD_BULK_NB_BANK(ep) > 2) )
#define UDD_EP_INT_NBANK_ERROR(ep)            \
	( (UDD_INTERRUPT_NB_BANK(ep) < 1) || (UDD_INTERRUPT_NB_BANK(ep) > 2) )

#define UDD_EP_ISO_NB_BANK_ERROR(ep)          \
	(UDD_EP_USED(ep) && UDD_EP_ISO_NBANK_ERROR(ep))
#define UDD_EP_BULK_NB_BANK_ERROR(ep)         \
	(UDD_EP_USED(ep) && UDD_EP_ISO_NBANK_ERROR(ep))
#define UDD_EP_INT_NB_BANK_ERROR(ep)          \
	(UDD_EP_USED(ep) && UDD_EP_ISO_NBANK_ERROR(ep))

#define UDD_EP_NB_BANK_ERROR(ep, type)        \
	(ATPASTE3(UDD_EP_, type, _NB_BANK_ERROR(ep)))

#define UDD_ISO_NB_BANK_ERROR \
	(          UDD_EP_NB_BANK_ERROR( 1, ISO) \
		|| UDD_EP_NB_BANK_ERROR( 2, ISO) \
		|| UDD_EP_NB_BANK_ERROR( 3, ISO) \
		|| UDD_EP_NB_BANK_ERROR( 4, ISO) \
		|| UDD_EP_NB_BANK_ERROR( 5, ISO) \
		|| UDD_EP_NB_BANK_ERROR( 6, ISO) \
		|| UDD_EP_NB_BANK_ERROR( 7, ISO) \
		|| UDD_EP_NB_BANK_ERROR( 8, ISO) \
		|| UDD_EP_NB_BANK_ERROR( 9, ISO) \
		|| UDD_EP_NB_BANK_ERROR(10, ISO) \
		|| UDD_EP_NB_BANK_ERROR(11, ISO) \
		|| UDD_EP_NB_BANK_ERROR(12, ISO) \
		|| UDD_EP_NB_BANK_ERROR(13, ISO) \
		|| UDD_EP_NB_BANK_ERROR(14, ISO) \
		|| UDD_EP_NB_BANK_ERROR(15, ISO) )
#define UDD_BULK_NB_BANK_ERROR \
	(          UDD_EP_NB_BANK_ERROR( 1, BULK) \
		|| UDD_EP_NB_BANK_ERROR( 2, BULK) \
		|| UDD_EP_NB_BANK_ERROR( 3, BULK) \
		|| UDD_EP_NB_BANK_ERROR( 4, BULK) \
		|| UDD_EP_NB_BANK_ERROR( 5, BULK) \
		|| UDD_EP_NB_BANK_ERROR( 6, BULK) \
		|| UDD_EP_NB_BANK_ERROR( 7, BULK) \
		|| UDD_EP_NB_BANK_ERROR( 8, BULK) \
		|| UDD_EP_NB_BANK_ERROR( 9, BULK) \
		|| UDD_EP_NB_BANK_ERROR(10, BULK) \
		|| UDD_EP_NB_BANK_ERROR(11, BULK) \
		|| UDD_EP_NB_BANK_ERROR(12, BULK) \
		|| UDD_EP_NB_BANK_ERROR(13, BULK) \
		|| UDD_EP_NB_BANK_ERROR(14, BULK) \
		|| UDD_EP_NB_BANK_ERROR(15, BULK) )
#define UDD_INTERRUPT_NB_BANK_ERROR \
	(          UDD_EP_NB_BANK_ERROR( 1, INT) \
		|| UDD_EP_NB_BANK_ERROR( 2, INT) \
		|| UDD_EP_NB_BANK_ERROR( 3, INT) \
		|| UDD_EP_NB_BANK_ERROR( 4, INT) \
		|| UDD_EP_NB_BANK_ERROR( 5, INT) \
		|| UDD_EP_NB_BANK_ERROR( 6, INT) \
		|| UDD_EP_NB_BANK_ERROR( 7, INT) \
		|| UDD_EP_NB_BANK_ERROR( 8, INT) \
		|| UDD_EP_NB_BANK_ERROR( 9, INT) \
		|| UDD_EP_NB_BANK_ERROR(10, INT) \
		|| UDD_EP_NB_BANK_ERROR(11, INT) \
		|| UDD_EP_NB_BANK_ERROR(12, INT) \
		|| UDD_EP_NB_BANK_ERROR(13, INT) \
		|| UDD_EP_NB_BANK_ERROR(14, INT) \
		|| UDD_EP_NB_BANK_ERROR(15, INT) )

#ifndef UDD_ISOCHRONOUS_NB_BANK
# define UDD_ISOCHRONOUS_NB_BANK(ep) 2
#else
# if UDD_ISO_NB_BANK_ERROR
#  error UDD_ISOCHRONOUS_NB_BANK(ep) must be define within 1 to 3.
# endif
#endif

#ifndef UDD_BULK_NB_BANK
# define UDD_BULK_NB_BANK(ep) ((ep < 3) ? 2: 1)
#else
# if UDD_BULK_NB_BANK_ERROR
#  error UDD_BULK_NB_BANK must be define with 1 or 2.
# endif
#endif

#ifndef UDD_INTERRUPT_NB_BANK
# define UDD_INTERRUPT_NB_BANK(ep) 1
#else
# if UDD_INTERRUPT_NB_BANK_ERROR
#  error UDD_INTERRUPT_NB_BANK must be define with 1 or 2.
# endif
#endif


/**
 * \name Power management routine.
 */
//@{

#ifndef UDD_NO_SLEEP_MGR

//! Definition of sleep levels
#define UDPHS_SLEEP_MODE_USB_SUSPEND  SLEEPMGR_WAIT_FAST
#define UDPHS_SLEEP_MODE_USB_IDLE     SLEEPMGR_SLEEP_WFI

//! State of USB line
static bool udd_b_idle;


/*! \brief Authorize or not the CPU powerdown mode
 *
 * \param b_enable   true to authorize idle mode
 */
static void udd_sleep_mode(bool b_idle)
{
	if (!b_idle && udd_b_idle) {
		dbg_print("_S ");
		sleepmgr_unlock_mode(UDPHS_SLEEP_MODE_USB_IDLE);
	}
	if (b_idle && !udd_b_idle) {
		sleepmgr_lock_mode(UDPHS_SLEEP_MODE_USB_IDLE);
		dbg_print("_W ");
	}
	udd_b_idle = b_idle;
}
#else

static void udd_sleep_mode(bool b_idle)
{
	UNUSED(b_idle);
}

#endif // UDD_NO_SLEEP_MGR

//@}

/**
 * \name VBus monitor routine
 */
//@{

#if UDD_VBUS_IO

# if !defined(UDD_NO_SLEEP_MGR) && !defined(USB_VBUS_WKUP)
/* Lock to SLEEPMGR_SLEEP_WFI if VBus not connected */
static bool b_vbus_sleep_lock = false;
/**
 * Lock sleep mode for VBus PIO pin change detection
 */
static void udd_vbus_monitor_sleep_mode(bool b_lock)
{
	if (b_lock && !b_vbus_sleep_lock) {
		b_vbus_sleep_lock = true;
		sleepmgr_lock_mode(SLEEPMGR_SLEEP_WFI);
	}
	if (!b_lock && b_vbus_sleep_lock) {
		b_vbus_sleep_lock = false;
		sleepmgr_unlock_mode(SLEEPMGR_SLEEP_WFI);
	}
}
# else
#  define udd_vbus_monitor_sleep_mode(lock)
# endif

/**
 * USB VBus pin change handler
 */
static void udd_vbus_handler(uint32_t id, uint32_t mask)
{
	if (USB_VBUS_PIO_ID != id || USB_VBUS_PIO_MASK != mask) {
		return;
	}
	/* PIO interrupt status has been cleared, just detect level */
	bool b_vbus_high = Is_udd_vbus_high();
	if (b_vbus_high) {
		udd_ack_vbus_interrupt(true);
		udd_vbus_monitor_sleep_mode(false);
		udd_attach();
	} else {
		udd_ack_vbus_interrupt(false);
		udd_vbus_monitor_sleep_mode(true);
		udd_detach();
	}
	UDC_VBUS_EVENT(b_vbus_high);
}

#endif

//@}

/**
 * \name Control endpoint low level management routine.
 *
 * This function performs control endpoint management.
 * It handle the SETUP/DATA/HANDSHAKE phases of a control transaction.
 */
//@{

//! Global variable to give and record information about setup request management
COMPILER_WORD_ALIGNED udd_ctrl_request_t udd_g_ctrlreq;

//! Bit definitions about endpoint control state machine for udd_ep_control_state
typedef enum {
	UDD_EPCTRL_SETUP                  = 0, //!< Wait a SETUP packet
	UDD_EPCTRL_DATA_OUT               = 1, //!< Wait a OUT data packet
	UDD_EPCTRL_DATA_IN                = 2, //!< Wait a IN data packet
	UDD_EPCTRL_HANDSHAKE_WAIT_IN_ZLP  = 3, //!< Wait a IN ZLP packet
	UDD_EPCTRL_HANDSHAKE_WAIT_OUT_ZLP = 4, //!< Wait a OUT ZLP packet
	UDD_EPCTRL_STALL_REQ              = 5, //!< STALL enabled on IN & OUT packet
} udd_ctrl_ep_state_t;

//! State of the endpoint control management
static udd_ctrl_ep_state_t udd_ep_control_state;

//! Total number of data received/sent during data packet phase with previous payload buffers
static uint16_t udd_ctrl_prev_payload_buf_cnt;

//! Number of data received/sent to/from udd_g_ctrlreq.payload buffer
static uint16_t udd_ctrl_payload_buf_cnt;

/**
 * \brief Reset control endpoint
 *
 * Called after a USB line reset or when UDD is enabled
 */
static void udd_reset_ep_ctrl(void);

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
static void udd_ctrl_underflow(void);

//! \brief Managed overflow event of OUT packet on control endpoint
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
 * \brief Main interrupt routine for control endpoint
 *
 * This switches control endpoint events to correct sub function.
 *
 * \return \c 1 if an event about control endpoint is occurred, otherwise \c 0.
 */
static bool udd_ctrl_interrupt(void);

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
	union {
		//! Callback to call at the end of transfer
		udd_callback_trans_t call_trans;

		//! Callback to call when the endpoint halt is cleared
		udd_callback_halt_cleared_t call_nohalt;
	};
	//! Buffer located in internal RAM to send or fill during job
	uint8_t *buf;
	//! Size of buffer to send or fill
	iram_size_t buf_size;
	//!< Size of data transfered
	iram_size_t buf_cnt;
	//!< Size of data prepared for DMA last time
	iram_size_t buf_load;
	//! A job is registered on this endpoint
	uint8_t busy:1;
	//! A short packet is requested for this job on endpoint IN
	uint8_t b_shortpacket:1;
	//! A stall has been requested but not executed
	uint8_t stall_requested:1;
} udd_ep_job_t;


//! Array to register a job on bulk/interrupt/isochronous endpoint
static udd_ep_job_t udd_ep_job[USB_DEVICE_MAX_EP];

//! \brief Reset all job table
static void udd_ep_job_table_reset(void);

//! \brief Abort all endpoint jobs on going
static void udd_ep_job_table_kill(void);

/**
 * \brief Abort endpoint job on going
 *
 * \param ep         endpoint number of job to abort
 */
static void udd_ep_abort_job(udd_ep_id_t ep);

/**
 * \brief Call the callback associated to the job which is finished
 *
 * \param ptr_job  job to complete
 * \param b_abort  if true then the job has been aborted
 */
static void udd_ep_finish_job(udd_ep_job_t * ptr_job, bool b_abort, uint8_t ep_num);

/**
 * \brief Start the next transfer if necessary or complete the job associated.
 *
 * \param ep         endpoint number without direction flag
 */
static void udd_ep_trans_done(udd_ep_id_t ep);

/**
 * \brief Main interrupt routine for bulk/interrupt/isochronous endpoints
 *
 * This switches endpoint events to correct sub function.
 *
 * \return \c 1 if an event about bulk/interrupt/isochronous endpoints has occurred, otherwise \c 0.
 */
static bool udd_ep_interrupt(void);

#endif // (0!=USB_DEVICE_MAX_EP)
//@}


//--------------------------------------------------------
//--- INTERNAL ROUTINES TO MANAGED GLOBAL EVENTS
/**
 * \internal
 * \brief Function called by UDPHS interrupt to manage USB Device interrupts
 *
 * USB Device interrupt events are split in three parts:
 * - USB line events (SOF, reset, suspend, resume, wakeup)
 * - control endpoint events (setup reception, end of data transfer, underflow, overflow, stall)
 * - bulk/interrupt/isochronous endpoints events (end of data transfer)
 *
 * Note:
 * Here, the global interrupt mask is not clear when an USB interrupt is enabled
 * because this one can not be occurred during the USB ISR (=during INTX is masked).
 * See Technical reference $3.8.3 Masking interrupt requests in peripheral modules.
 */
ISR(UDD_USB_INT_FUN)
{
	udd_enable_periph_ck();

	/* For fast wakeup clocks restore
	 * In WAIT mode, clocks are switched to FASTRC.
	 * After wakeup clocks should be restored, before that ISR should not
	 * be served.
	 */
	if (!pmc_is_wakeup_clocks_restored() && !Is_udd_suspend()) {
		cpu_irq_disable();
		return;
	}

	if (Is_udd_sof()) {
		udd_ack_sof();
		if (Is_udd_full_speed_mode()) {
			udc_sof_notify();
		}
#ifdef UDC_SOF_EVENT
		UDC_SOF_EVENT();
#endif
		goto udd_interrupt_sof_end;
	}

	if (Is_udd_msof()) {
		udd_ack_msof();
		udc_sof_notify();
		goto udd_interrupt_sof_end;
	}
	dbg_print("%c ", udd_is_high_speed() ? 'H' : 'F');

	if (udd_ctrl_interrupt()) {
		goto udd_interrupt_end; // Interrupt acked by control endpoint managed
	}

#if (0 != USB_DEVICE_MAX_EP)
	if (udd_ep_interrupt()) {
		goto udd_interrupt_end; // Interrupt acked by bulk/interrupt/isochronous endpoint managed
	}
#endif

	// USB bus reset detection
	if (Is_udd_reset()) {
		dbg_print("EoR ");
		udd_ack_reset();
		// Abort all jobs on-going
#if (USB_DEVICE_MAX_EP != 0)
		udd_ep_job_table_kill();
#endif
		// Reset USB Device Stack Core
		udc_reset();
		// Reset endpoint control
		udd_reset_ep_ctrl();
		// Reset endpoint control management
		udd_ctrl_init();
		// Enable SOF interrupts cleared by USB reset event
		udd_enable_sof_interrupt();
		udd_enable_msof_interrupt();
		udd_enable_suspend_interrupt();
		goto udd_interrupt_end;
	}

	if (Is_udd_suspend_interrupt_enabled() && Is_udd_suspend()) {
		dbg_print("Susp ");
		// Remove old wakeup status
		udd_ack_wake_up();
		// The suspend interrupt is automatic acked when a wakeup occur
		udd_disable_suspend_interrupt();
		udd_enable_wake_up_interrupt();

		// The wakeup interrupt is generated even if the device controller clock is disabled.
		udd_ack_suspend();

		udd_disable_periph_ck();

#ifdef UDC_SUSPEND_EVENT
		UDC_SUSPEND_EVENT();
#endif
		udd_sleep_mode(false); // Enter in SUSPEND mode
		goto udd_interrupt_end;
	}

	// On wakeup, _IEN is reseted only with EoR enabled
	if (Is_udd_wake_up()) {
		udd_sleep_mode(true); // Enter in IDLE mode
		// Remove old suspend status
		udd_ack_suspend();
		// Ack wakeup interrupt and enable suspend interrupt
		udd_ack_wake_up();
		// Ack wakeup interrupt and enable suspend interrupt
		udd_disable_wake_up_interrupt();
		udd_enable_suspend_interrupt();
		udd_enable_sof_interrupt();
		udd_enable_msof_interrupt();

#ifdef UDC_RESUME_EVENT
		UDC_RESUME_EVENT();
#endif
		dbg_print("Wkup ");
		goto udd_interrupt_end;
	}
	//dbg_print("i%08x ", UDPHS->UDPHS_INTSTA);

udd_interrupt_end:
	//dbg_print("i%08x ", UDPHS->UDPHS_INTSTA);
	//dbg_print("m%08x", UDPHS->UDPHS_IEN);
	dbg_print("\n\r");
udd_interrupt_sof_end:
	return;
}


bool udd_include_vbus_monitoring(void)
{
#if UDD_VBUS_IO
	return true;
#else
	return false;
#endif
}


void udd_enable(void)
{
	irqflags_t flags;
	sysclk_enable_usb();

	udd_enable_periph_ck();

	flags = cpu_irq_save();

	//** Enable USB hardware
	udd_enable_periph();

	// Cortex-M3, uses NVIC, no need to register IRQ handler
	// Here, only the Device mode is possible,
	// UDPHS interrupt is UDD interrupt
	NVIC_SetPriority((IRQn_Type) ID_UDPHS, UDD_USB_INT_LEVEL - 1);
	NVIC_EnableIRQ((IRQn_Type) ID_UDPHS);

	// Reset internal variables
#if (0!=USB_DEVICE_MAX_EP)
	udd_ep_job_table_reset();
#endif

	// Set the USB speed requested by configuration file
#ifdef USB_DEVICE_HS_SUPPORT
	udd_high_speed_enable();
#else
	udd_high_speed_disable();
#endif

	// Always authorize asynchronous USB interrupts to exit of sleep mode
	pmc_set_fast_startup_input(PMC_FSMR_USBAL);

#ifndef UDD_NO_SLEEP_MGR
	// Initialize the sleep mode authorized for the USB suspend mode
	udd_b_idle = false;
	sleepmgr_lock_mode(UDPHS_SLEEP_MODE_USB_SUSPEND);
#endif

#if UDD_VBUS_IO
	/* Initialize VBus monitor */
	udd_vbus_init(udd_vbus_handler);
	udd_vbus_monitor_sleep_mode(true);
	/* Force Vbus interrupt when Vbus is always high
	 * This is possible due to a short timing between a Host mode stop/start.
	 */
	if (Is_udd_vbus_high()) {
		udd_vbus_handler(USB_VBUS_PIO_ID, USB_VBUS_PIO_MASK);
	}
#else
#  ifndef USB_DEVICE_ATTACH_AUTO_DISABLE
	udd_attach();
#  endif
#endif

	cpu_irq_restore(flags);
}


void udd_disable(void)
{
	irqflags_t flags;
	flags = cpu_irq_save();
	// Disable USB
	udd_reset_periph();
	sysclk_disable_usb();
	pmc_disable_periph_clk(ID_UDPHS);
	udd_sleep_mode(false);
#ifndef UDD_NO_SLEEP_MGR
	sleepmgr_unlock_mode(UDPHS_SLEEP_MODE_USB_SUSPEND);
#endif

# if UDD_VBUS_IO
	udd_vbus_monitor_sleep_mode(false);
# endif

	cpu_irq_restore(flags);
}


void udd_attach(void)
{
	irqflags_t flags;
	flags = cpu_irq_save();

	// At startup the USB bus state is unknown,
	// therefore the state is considered IDLE to not miss any USB event
	udd_sleep_mode(true);
	udd_enable_periph_ck();

	// Authorize attach if Vbus is present
	udd_attach_device();

	// Enable USB line events
	udd_enable_reset_interrupt();
	udd_enable_suspend_interrupt();
	udd_enable_wake_up_interrupt();
	udd_enable_sof_interrupt();
#ifdef USB_DEVICE_HS_SUPPORT
	udd_enable_msof_interrupt();
#endif
	// Reset following interrupts flag
	udd_ack_sof();
	udd_ack_msof();
	udd_ack_reset();
	udd_ack_suspend();
	udd_ack_wake_up();

	udd_disable_periph_ck();
	cpu_irq_restore(flags);
}


void udd_detach(void)
{
	udd_enable_periph_ck();
	// Detach device from the bus
	udd_detach_device();
	udd_disable_periph_ck();
	udd_sleep_mode(false);
}


bool udd_is_high_speed(void)
{
#ifdef USB_DEVICE_HS_SUPPORT
	return !Is_udd_full_speed_mode();
#else
	return false;
#endif
}


void udd_set_address(uint8_t address)
{
	dbg_print("Addr%d ", address);
	udd_disable_address();
	udd_configure_address(address);
	udd_enable_address();
}


uint8_t udd_getaddress(void)
{
	return udd_get_configured_address();
}


uint16_t udd_get_frame_number(void)
{
	return udd_frame_number();
}

uint16_t udd_get_micro_frame_number(void)
{
	return udd_micro_frame_number();
}

void udd_send_remotewakeup(void)
{
#ifndef UDD_NO_SLEEP_MGR
	if (!udd_b_idle)
#endif
	{
		dbg_print("SendWkup\n\r");
		udd_sleep_mode(true); // Enter in IDLE mode
		udd_enable_periph_ck();
		udd_initiate_remote_wake_up();
	}
}


void udd_set_setup_payload(uint8_t *payload, uint16_t payload_size)
{
	udd_g_ctrlreq.payload = payload;
	udd_g_ctrlreq.payload_size = payload_size;
}


#if (0!=USB_DEVICE_MAX_EP)
bool udd_ep_alloc(udd_ep_id_t ep, uint8_t bmAttributes,
		uint16_t MaxEndpointSize)
{
	bool b_dir_in;
	uint16_t ep_allocated;
	uint8_t nb_bank, bank, nb_tran = 0, i;
	bool b_iso_hbw = false;

	b_dir_in = ep & USB_EP_DIR_IN;
	ep = ep & USB_EP_ADDR_MASK;

	if (ep > USB_DEVICE_MAX_EP) {
		return false;
	}
	if (Is_udd_endpoint_enabled(ep)) {
		return false;
	}

	// Bank choice
	switch (bmAttributes & USB_EP_TYPE_MASK) {
	case USB_EP_TYPE_ISOCHRONOUS:
		b_iso_hbw = Is_udd_endpoint_high_bw_supported(ep);
		nb_bank = UDD_ISOCHRONOUS_NB_BANK(ep);
		break;
	case USB_EP_TYPE_INTERRUPT:
		nb_bank = UDD_INTERRUPT_NB_BANK(ep);
		break;
	case USB_EP_TYPE_BULK:
		nb_bank = UDD_BULK_NB_BANK(ep);
		break;
	default:
		Assert(false);
		return false;
	}
	switch (nb_bank) {
	case 1:
		bank = UDPHS_EPTCFG_BK_NUMBER_1 >> UDPHS_EPTCFG_BK_NUMBER_Pos;
		break;
	case 2:
		bank = UDPHS_EPTCFG_BK_NUMBER_2 >> UDPHS_EPTCFG_BK_NUMBER_Pos;
		break;
	case 3:
		bank = UDPHS_EPTCFG_BK_NUMBER_3 >> UDPHS_EPTCFG_BK_NUMBER_Pos;
		break;
	default:
		Assert(false);
		return false;
	}
	if (b_iso_hbw) {
		switch(MaxEndpointSize & (0x3u << 11)) {
		case (0x1u << 11):
			nb_tran = 2;
			break;
		case (0x2u << 11):
			nb_tran = 3;
			break;
		default:
			nb_tran = 1;
			break;
		}
		MaxEndpointSize &= ~(0x3u << 11);
	} else if (MaxEndpointSize & (0x3u << 11)) {
		// High BW not supported
		return false;
	}

	Assert(MaxEndpointSize <= udd_get_endpoint_size_max(ep));
	Assert(nb_bank <= udd_get_endpoint_bank_max_nbr(ep));

	// Set configuration of new endpoint
	// Note: NB_TRANS is at maximum for isochronous endpoint, else 0 (ignored).
	udd_configure_endpoint(ep, bmAttributes, (b_dir_in ? 1 : 0),
			MaxEndpointSize, bank, (b_iso_hbw)?nb_tran:0);
	if (!Is_udd_endpoint_mapped(ep)) {
		return false;
	}
	udd_enable_endpoint(ep);

	// To avoid conflict during reorganization of the DPRAM
	// the superior endpoint must be unallocated and reallocated
	ep_allocated = 0;

	// Unalloc endpoints superior
	for (i = USB_DEVICE_MAX_EP; i >= ep; i--) {
		if (Is_udd_endpoint_enabled(i)) {
			// Save number of bank value
			bank = udd_get_endpoint_bank(i);
			ep_allocated |=  bank << (i * 2);
			// Disable and unallocate endpoint
			udd_disable_endpoint(i);
			udd_configure_endpoint_bank(i, 0);

		}
	}

	// Realloc/Enable endpoints
	for (i = ep; i <= USB_DEVICE_MAX_EP; i++) {
		udd_ep_job_t *ptr_job = &udd_ep_job[i - 1];
		bool b_restart = ptr_job->busy;
		// Unallocated banks?
		bank = (ep_allocated >> (i * 2)) & 0x03;
		if (bank == 0) {
			continue;
		}
		// Restart running job because
		// memory window slides up and its data is lost
		ptr_job->busy = false;
		// Re-allocate memory
		udd_configure_endpoint_bank(i, bank);
		udd_enable_endpoint(i);
		if (!Is_udd_endpoint_mapped(i)) {
			dbg_print("ErrRealloc%d-JobE ", i);
			if (NULL == ptr_job->call_trans) {
				return false;
			}
			if (Is_udd_endpoint_in(i)) {
				i |= USB_EP_DIR_IN;
			}
			ptr_job->call_trans(UDD_EP_TRANSFER_ABORT,
					ptr_job->buf_cnt, i);
			return false;
		}
		udd_enable_endpoint_bank_autoswitch(i);
		if (b_restart) {
			// Re-run the job remaining part
			ptr_job->buf_cnt -= ptr_job->buf_load;
			b_restart = udd_ep_run(i,
					ptr_job->b_shortpacket,
					&ptr_job->buf[ptr_job->buf_cnt],
					ptr_job->buf_size
						- ptr_job->buf_cnt,
					ptr_job->call_trans);
			if (!b_restart) {
				dbg_print("ErrReRun%d ", i);
				return false;
			}
		}
	}
	dbg_print("ep_alloc(%d:%08x) ", ep, UDPHS->UDPHS_EPT[ep].UDPHS_EPTCFG);
	return true;
}


void udd_ep_free(udd_ep_id_t ep)
{
	uint8_t ep_index = ep & USB_EP_ADDR_MASK;

	if (USB_DEVICE_MAX_EP < ep_index) {
		return;
	}
	udd_disable_endpoint(ep_index);
	udd_configure_endpoint_bank(ep_index, 0);
	udd_ep_abort_job(ep);
	udd_ep_job[ep_index - 1].stall_requested = false;
}


bool udd_ep_is_halted(udd_ep_id_t ep)
{
	uint8_t ep_index = ep & USB_EP_ADDR_MASK;
	udd_ep_job_t *ptr_job = &udd_ep_job[ep_index - 1];
	return (Is_udd_endpoint_stall_requested(ep_index)
			|| ptr_job->stall_requested);
}


bool udd_ep_set_halt(udd_ep_id_t ep)
{
	udd_ep_job_t *ptr_job;
	uint8_t ep_index = ep & USB_EP_ADDR_MASK;

	if (USB_DEVICE_MAX_EP < ep_index) {
		return false;
	}

	ptr_job = &udd_ep_job[ep_index - 1];

	if (Is_udd_endpoint_stall_requested(ep_index) // Endpoint stalled
			|| ptr_job->stall_requested) { // Endpoint stall is requested
		return true; // Already STALL
	}

	if (ptr_job->busy == true) {
		return false; // Job on going, stall impossible
	}

	if ((ep & USB_EP_DIR_IN) && (0 != udd_nb_busy_bank(ep_index))) {
			// Delay the stall after the end of IN transfer on USB line
			ptr_job->stall_requested = true;
			udd_enable_bank_interrupt(ep_index);
			udd_enable_endpoint_interrupt(ep_index);
			dbg_print("<reqHalt%x> ", ep);
			return true;
	}

	// Stall endpoint immediately
	udd_disable_endpoint_bank_autoswitch(ep_index);
	udd_ack_stall(ep_index);
	udd_enable_stall_handshake(ep_index);
	dbg_print("<Halt%x> ", ep);
	return true;
}


bool udd_ep_clear_halt(udd_ep_id_t ep)
{
	bool b_stall_cleared = false;
	udd_ep_job_t *ptr_job;

	ep &= USB_EP_ADDR_MASK;
	if (USB_DEVICE_MAX_EP < ep)
		return false;
	ptr_job = &udd_ep_job[ep - 1];

	if (ptr_job->stall_requested) {
		// Endpoint stall has been requested but not done
		// Remove stall request
		dbg_print("<unWHalt%x> ", ep);
		ptr_job->stall_requested = false;
		udd_disable_bank_interrupt(ep);
		udd_disable_endpoint_interrupt(ep);
		b_stall_cleared = true;
	}
	if (Is_udd_endpoint_stall_requested(ep)) {
		dbg_print("<unHalt%x> ", ep);
		if (Is_udd_stall(ep)) {
			udd_ack_stall(ep);
			// A packet has been stalled
			// then reset datatoggle
			udd_reset_data_toggle(ep);
		}
		// Disable stall
		udd_disable_stall_handshake(ep);
		udd_enable_endpoint_bank_autoswitch(ep);
		b_stall_cleared = true;
	}
	if (b_stall_cleared) {
		// If a job is register on clear halt action
		// then execute callback
		if (ptr_job->busy == true) {
			ptr_job->busy = false;
			ptr_job->call_nohalt();
		}
	}
	return true;
}


bool udd_ep_run(udd_ep_id_t ep, bool b_shortpacket,
		uint8_t * buf, iram_size_t buf_size,
		udd_callback_trans_t callback)
{
	udd_ep_job_t *ptr_job;
	irqflags_t flags;

	ep &= USB_EP_ADDR_MASK;
	if (USB_DEVICE_MAX_EP < ep) {
		return false;
	}

	// Get job about endpoint
	ptr_job = &udd_ep_job[ep - 1];

	if ((!Is_udd_endpoint_enabled(ep))
			|| Is_udd_endpoint_stall_requested(ep)
			|| ptr_job->stall_requested) {
		return false; // Endpoint is halted
	}

	flags = cpu_irq_save();
	if (ptr_job->busy == true) {
		cpu_irq_restore(flags);
		return false; // Job already on going
	}
	ptr_job->busy = true;
	cpu_irq_restore(flags);

	// No job running. Let's setup a new one.
	ptr_job->buf = buf;
	ptr_job->buf_size = buf_size;
	ptr_job->buf_cnt = 0;
	ptr_job->buf_load = 0;
	ptr_job->call_trans = callback;
	ptr_job->b_shortpacket = b_shortpacket || (buf_size == 0);

	// Request first DMA transfer
	//dbg_print("runDMA%d\n\r", ep);
	udd_ep_trans_done(ep);
	return true;
}


void udd_ep_abort(udd_ep_id_t ep)
{
	uint8_t ep_index = ep & USB_EP_ADDR_MASK;
	// Stop DMA transfer
	udd_endpoint_dma_set_control(ep_index, 0);
	// Kill banks for IN
	if (ep & USB_EP_DIR_IN) {
		for(;udd_nb_busy_bank(ep_index);) {
			udd_kill_last_in_bank(ep_index);
			while(Is_udd_kill_last(ep_index));
		}
	}
	udd_ep_abort_job(ep);
}


bool udd_ep_wait_stall_clear(udd_ep_id_t ep,
		udd_callback_halt_cleared_t callback)
{
	udd_ep_job_t *ptr_job;

	ep &= USB_EP_ADDR_MASK;
	if (USB_DEVICE_MAX_EP < ep) {
		return false;
	}

	ptr_job = &udd_ep_job[ep - 1];

	if (!Is_udd_endpoint_enabled(ep)) {
		return false; // Endpoint not enabled
	}

	// Wait clear halt endpoint
	if (ptr_job->busy == true) {
		return false; // Job already on going
	}

	if (Is_udd_endpoint_stall_requested(ep)
			|| ptr_job->stall_requested) {
		// Endpoint halted then registers the callback
		ptr_job->busy = true;
		ptr_job->call_nohalt = callback;
	} else {
		// endpoint not halted then call directly callback
		callback();
	}
	return true;
}
#endif // (0!=USB_DEVICE_MAX_EP)


#ifdef USB_DEVICE_HS_SUPPORT

void udd_test_mode_j(void)
{
	udd_enable_hs_test_mode();
	udd_enable_hs_test_mode_j();
}


void udd_test_mode_k(void)
{
	udd_enable_hs_test_mode();
	udd_enable_hs_test_mode_k();
}


void udd_test_mode_se0_nak(void)
{
	udd_enable_hs_test_mode();
}


void udd_test_mode_packet(void)
{
	uint8_t i;
	uint8_t *ptr_dest;
	const uint8_t *ptr_src;

	const uint8_t test_packet[] = {
		// 00000000 * 9
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		// 01010101 * 8
		0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
		// 01110111 * 8
		0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
		// 0, {111111S * 15}, 111111
		0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				0xFF, 0xFF,
		// S, 111111S, {0111111S * 7}
		0x7F, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB, 0xFD,
		// 00111111, {S0111111 * 9}, S0
		0xFC, 0x7E, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB, 0xFD, 0x7E
	};

	// Reconfigure control endpoint to bulk IN endpoint
	udd_disable_endpoint(0);
	udd_configure_endpoint(0, // endpoint number: 0
			USB_EP_TYPE_BULK, // Type: bulk
			1, // dir: IN
			64, //Size
			1, // Bank Number
			0); // NB Trans (not used for bulk ept)
	udd_enable_endpoint(0);

	udd_enable_hs_test_mode();
	udd_enable_hs_test_mode_packet();

	// Send packet on endpoint 0
	ptr_dest = (uint8_t *) & udd_get_endpoint_fifo_access(0, 8);
	ptr_src = test_packet;

	for (i = 0; i < sizeof(test_packet); i++) {
		*ptr_dest++ = *ptr_src++;
	}
	// Validate and send the data available in the control endpoint buffer
	udd_ack_in_send(0);
	udd_raise_tx_pkt_ready(0);
}
#endif // USB_DEVICE_HS_SUPPORT



//--------------------------------------------------------
//--- INTERNAL ROUTINES TO MANAGED THE CONTROL ENDPOINT

static void udd_reset_ep_ctrl(void)
{
	irqflags_t flags;

	// Reset USB address to 0
	udd_configure_address(0);
	udd_enable_address();

	// Alloc and configure control endpoint
	udd_configure_endpoint(0,
			USB_EP_TYPE_CONTROL,
			0,
			USB_DEVICE_EP_CTRL_SIZE,
			UDPHS_EPTCFG_BK_NUMBER_1 >> UDPHS_EPTCFG_BK_NUMBER_Pos,
			0);

	dbg_print("rst(0:%08x) ", UDPHS->UDPHS_EPT[0].UDPHS_EPTCFG);
	udd_enable_endpoint(0);
	flags = cpu_irq_save();
	udd_enable_setup_received_interrupt(0);
	udd_enable_out_received_interrupt(0);
	udd_enable_endpoint_interrupt(0);
	cpu_irq_restore(flags);
}

static void udd_ctrl_init(void)
{
	irqflags_t flags;

	dbg_print("ctlInit ");

	flags = cpu_irq_save();
	// In case of abort of IN Data Phase:
	// No need to abort IN transfer (rise TXINI),
	// because it is automatically done by hardware when a Setup packet is received.
	// But the interrupt must be disabled to don't generate interrupt TXINI
	// after SETUP reception.
	udd_disable_in_send_interrupt(0);
	cpu_irq_restore(flags);

	// In case of OUT ZLP event is no processed before Setup event occurs
	udd_ack_out_received(0);

	udd_g_ctrlreq.callback = NULL;
	udd_g_ctrlreq.over_under_run = NULL;
	udd_g_ctrlreq.payload_size = 0;
	udd_ep_control_state = UDD_EPCTRL_SETUP;
}


static void udd_ctrl_setup_received(void)
{
	irqflags_t flags;
	uint8_t i;

	if (UDD_EPCTRL_SETUP != udd_ep_control_state) {
		dbg_print("sErr ");
		// May be a hidden DATA or ZLP phase
		// or protocol abort
		udd_ctrl_endofrequest();

		// Reinitializes control endpoint management
		udd_ctrl_init();
	}
	// Fill setup request structure
	if (8 != udd_byte_count(0)) {
		dbg_print("cntErr ");
		udd_ctrl_stall_data();
		udd_ack_setup_received(0);
		return; // Error data number doesn't correspond to SETUP packet
	}

	uint8_t *ptr = (uint8_t *) & udd_get_endpoint_fifo_access(0, 8);
	for (i = 0; i < 8; i++) {
		((uint8_t *) & udd_g_ctrlreq.req)[i] = *ptr++;
	}
	// Manage LSB/MSB to fit with CPU usage
	udd_g_ctrlreq.req.wValue = le16_to_cpu(udd_g_ctrlreq.req.wValue);
	udd_g_ctrlreq.req.wIndex = le16_to_cpu(udd_g_ctrlreq.req.wIndex);
	udd_g_ctrlreq.req.wLength = le16_to_cpu(udd_g_ctrlreq.req.wLength);
	dbg_print("(%x %x %x %x %x) ",
			udd_g_ctrlreq.req.bRequest, udd_g_ctrlreq.req.bmRequestType,
			udd_g_ctrlreq.req.wValue, udd_g_ctrlreq.req.wIndex,
			udd_g_ctrlreq.req.wLength);

	// Decode setup request
	if (udc_process_setup() == false) {
		// Setup request unknown then stall it
		dbg_print("ProcErr ");
		udd_ctrl_stall_data();
		udd_ack_setup_received(0);
		return;
	}
	udd_ack_setup_received(0);

	if (Udd_setup_is_in()) {
		dbg_print("_I ");
		// IN data phase requested
		udd_ctrl_prev_payload_buf_cnt = 0;
		udd_ctrl_payload_buf_cnt = 0;
		udd_ep_control_state = UDD_EPCTRL_DATA_IN;
		udd_ctrl_in_sent(); // Send first data transfer
	} else {
		dbg_print("_O ");
		if (0 == udd_g_ctrlreq.req.wLength) {
			// No data phase requested
			dbg_print("Zl ");
			// Send IN ZLP to ACK setup request
			udd_ctrl_send_zlp_in();
			return;
		}
		// OUT data phase requested
		udd_ctrl_prev_payload_buf_cnt = 0;
		udd_ctrl_payload_buf_cnt = 0;
		udd_ep_control_state = UDD_EPCTRL_DATA_OUT;
		// To detect a protocol error, enable nak interrupt on data IN phase
		udd_ack_nak_in(0);
		flags = cpu_irq_save();
		udd_enable_nak_in_interrupt(0);
		cpu_irq_restore(flags);
	}
}


static void udd_ctrl_in_sent(void)
{
	static bool b_shortpacket = false;
	uint16_t nb_remain;
	uint8_t i;
	uint8_t *ptr_dest, *ptr_src;
	irqflags_t flags;

	flags = cpu_irq_save();
	udd_disable_in_send_interrupt(0);
	cpu_irq_restore(flags);

	if (UDD_EPCTRL_HANDSHAKE_WAIT_IN_ZLP == udd_ep_control_state) {
		dbg_print("ZlpE\n\r");
		// ZLP on IN is sent, then valid end of setup request
		udd_ctrl_endofrequest();
		// Reinitializes control endpoint management
		udd_ctrl_init();
		return;
	}
	Assert(udd_ep_control_state == UDD_EPCTRL_DATA_IN);

	nb_remain = udd_g_ctrlreq.payload_size - udd_ctrl_payload_buf_cnt;
	if (0 == nb_remain) {
		// All content of current buffer payload are sent
		// Update number of total data sending by previous payload buffer
		udd_ctrl_prev_payload_buf_cnt += udd_ctrl_payload_buf_cnt;
		if ((udd_g_ctrlreq.req.wLength == udd_ctrl_prev_payload_buf_cnt)
				|| b_shortpacket) {
			// All data requested are transfered or a short packet has been sent
			// then it is the end of data phase.
			// Generate an OUT ZLP for handshake phase.
			dbg_print("_zO ");
			udd_ctrl_send_zlp_out();
			return;
		}
		// Need of new buffer because the data phase is not complete
		if ((!udd_g_ctrlreq.over_under_run)
				|| (!udd_g_ctrlreq.over_under_run())) {
			// Underrun then send zlp on IN
			// Here nb_remain=0 and allows to send a IN ZLP
		} else {
			// A new payload buffer is given
			udd_ctrl_payload_buf_cnt = 0;
			nb_remain = udd_g_ctrlreq.payload_size;
		}
	}
	// Continue transfer and send next data
	if (nb_remain >= USB_DEVICE_EP_CTRL_SIZE) {
		nb_remain = USB_DEVICE_EP_CTRL_SIZE;
		b_shortpacket = false;
	} else {
		b_shortpacket = true;
	}
	// Fill buffer of endpoint control
	ptr_dest = (uint8_t *) & udd_get_endpoint_fifo_access(0, 8);
	ptr_src = udd_g_ctrlreq.payload + udd_ctrl_payload_buf_cnt;
	//** Critical section
	// Only in case of DATA IN phase abort without USB Reset signal after.
	// The IN data don't must be written in endpoint 0 DPRAM during
	// a next setup reception in same endpoint 0 DPRAM.
	// Thereby, an OUT ZLP reception must check before IN data write
	// and if no OUT ZLP is received the data must be written quickly (800us)
	// before an eventually ZLP OUT and SETUP reception
	flags = cpu_irq_save();
	if (Is_udd_out_received(0)) {
		dbg_print("Abort ");
		// IN DATA phase aborted by OUT ZLP
		cpu_irq_restore(flags);
		udd_ep_control_state = UDD_EPCTRL_HANDSHAKE_WAIT_OUT_ZLP;
		return; // Exit of IN DATA phase
	}
	// dbg_print("t%d ", nb_remain);
	// Write quickly the IN data
	for (i = 0; i < nb_remain; i++) {
		*ptr_dest++ = *ptr_src++;
	}
	udd_ctrl_payload_buf_cnt += nb_remain;

	// Validate and send the data available in the control endpoint buffer
	udd_ack_in_send(0);
	udd_raise_tx_pkt_ready(0);
	udd_enable_in_send_interrupt(0);
	// In case of abort of DATA IN phase, no need to enable nak OUT interrupt
	// because OUT endpoint is already free and ZLP OUT accepted.
	cpu_irq_restore(flags);
}


static void udd_ctrl_out_received(void)
{
	irqflags_t flags;
	uint8_t i;
	uint16_t nb_data;

	if (UDD_EPCTRL_DATA_OUT != udd_ep_control_state) {
		if ((UDD_EPCTRL_DATA_IN == udd_ep_control_state)
				|| (UDD_EPCTRL_HANDSHAKE_WAIT_OUT_ZLP ==
				udd_ep_control_state)) {
			dbg_print("EoStp\n\r");
			// End of SETUP request:
			// - Data IN Phase aborted,
			// - or last Data IN Phase hidden by ZLP OUT sending quickly,
			// - or ZLP OUT received normally.
			udd_ctrl_endofrequest();
		} else {
			dbg_print("ErrStp\n\r");
			// Protocol error during SETUP request
			udd_ctrl_stall_data();
		}
		// Reinitializes control endpoint management
		udd_ctrl_init();
		return;
	}
	// Read data received during OUT phase
	nb_data = udd_byte_count(0);
	if (udd_g_ctrlreq.payload_size < (udd_ctrl_payload_buf_cnt + nb_data)) {
		// Payload buffer too small
		nb_data = udd_g_ctrlreq.payload_size - udd_ctrl_payload_buf_cnt;
	}
	uint8_t *ptr_src = (uint8_t *) & udd_get_endpoint_fifo_access(0, 8);
	uint8_t *ptr_dest = udd_g_ctrlreq.payload + udd_ctrl_payload_buf_cnt;
	for (i = 0; i < nb_data; i++) {
		*ptr_dest++ = *ptr_src++;
	}
	udd_ctrl_payload_buf_cnt += nb_data;

	if ((USB_DEVICE_EP_CTRL_SIZE != nb_data)
			|| (udd_g_ctrlreq.req.wLength <=
			(udd_ctrl_prev_payload_buf_cnt +
			udd_ctrl_payload_buf_cnt))) {
		// End of reception because it is a short packet
		// Before send ZLP, call intermediate callback
		// in case of data receive generate a stall
		udd_g_ctrlreq.payload_size = udd_ctrl_payload_buf_cnt;
		if (NULL != udd_g_ctrlreq.over_under_run) {
			if (!udd_g_ctrlreq.over_under_run()) {
				// Stall ZLP
				udd_ctrl_stall_data();
				// Ack reception of OUT to replace NAK by a STALL
				udd_ack_out_received(0);
				return;
			}
		}
		// Send IN ZLP to ACK setup request
		udd_ack_out_received(0);
		udd_ctrl_send_zlp_in();
		return;
	}

	if (udd_g_ctrlreq.payload_size == udd_ctrl_payload_buf_cnt) {
		// Overrun then request a new payload buffer
		if (!udd_g_ctrlreq.over_under_run) {
			// No callback available to request a new payload buffer
			udd_ctrl_stall_data();
			// Ack reception of OUT to replace NAK by a STALL
			udd_ack_out_received(0);
			return;
		}
		if (!udd_g_ctrlreq.over_under_run()) {
			// No new payload buffer delivered
			udd_ctrl_stall_data();
			// Ack reception of OUT to replace NAK by a STALL
			udd_ack_out_received(0);
			return;
		}
		// New payload buffer available
		// Update number of total data received
		udd_ctrl_prev_payload_buf_cnt += udd_ctrl_payload_buf_cnt;
		// Reinit reception on payload buffer
		udd_ctrl_payload_buf_cnt = 0;
	}
	// Free buffer of control endpoint to authorize next reception
	udd_ack_out_received(0);
	// To detect a protocol error, enable nak interrupt on data IN phase
	udd_ack_nak_in(0);
	flags = cpu_irq_save();
	udd_enable_nak_in_interrupt(0);
	cpu_irq_restore(flags);
}


static void udd_ctrl_underflow(void)
{
	if (Is_udd_out_received(0))
		return; // underflow ignored if OUT data is received

	if (UDD_EPCTRL_DATA_OUT == udd_ep_control_state) {
		// Host want to stop OUT transaction
		// then stop to wait OUT data phase and wait IN ZLP handshake
		udd_ctrl_send_zlp_in();
	} else if (UDD_EPCTRL_HANDSHAKE_WAIT_OUT_ZLP == udd_ep_control_state) {
		// A OUT handshake is waiting by device,
		// but host want extra IN data then stall extra IN data
		udd_enable_stall_handshake(0);
	}
}


static void udd_ctrl_overflow(void)
{
	if (Is_udd_in_send(0))
		return; // overflow ignored if IN data is received

	// The case of UDD_EPCTRL_DATA_IN is not managed
	// because the OUT endpoint is already free and OUT ZLP accepted

	if (UDD_EPCTRL_HANDSHAKE_WAIT_IN_ZLP == udd_ep_control_state) {
		// A IN handshake is waiting by device,
		// but host want extra OUT data then stall extra OUT data
		udd_enable_stall_handshake(0);
	}
}


static void udd_ctrl_stall_data(void)
{
	// Stall all packets on IN & OUT control endpoint
	udd_ep_control_state = UDD_EPCTRL_STALL_REQ;
	udd_enable_stall_handshake(0);
}


static void udd_ctrl_send_zlp_in(void)
{
	irqflags_t flags;

	udd_ep_control_state = UDD_EPCTRL_HANDSHAKE_WAIT_IN_ZLP;

	// Validate and send empty IN packet on control endpoint
	flags = cpu_irq_save();
	// Send ZLP on IN endpoint
	udd_ack_in_send(0);
	udd_raise_tx_pkt_ready(0);
	udd_enable_in_send_interrupt(0);
	// To detect a protocol error, enable nak interrupt on data OUT phase
	udd_ack_nak_out(0);
	udd_enable_nak_out_interrupt(0);
	cpu_irq_restore(flags);
}


static void udd_ctrl_send_zlp_out(void)
{
	irqflags_t flags;

	udd_ep_control_state = UDD_EPCTRL_HANDSHAKE_WAIT_OUT_ZLP;
	// No action is necessary to accept OUT ZLP
	// because the buffer of control endpoint is already free

	// To detect a protocol error, enable nak interrupt on data IN phase
	flags = cpu_irq_save();
	udd_ack_nak_in(0);
	udd_enable_nak_in_interrupt(0);
	cpu_irq_restore(flags);
}


static void udd_ctrl_endofrequest(void)
{
	// If a callback is registered then call it
	if (udd_g_ctrlreq.callback) {
		udd_g_ctrlreq.callback();
	}
}


static bool udd_ctrl_interrupt(void)
{
	uint32_t status = udd_get_endpoint_status(0);

	if (!Is_udd_endpoint_interrupt(0)) {
		return false; // No interrupt events on control endpoint
	}
	dbg_print("0: ");

	// By default disable overflow and underflow interrupt
	udd_disable_nak_in_interrupt(0);
	udd_disable_nak_out_interrupt(0);

	// Search event on control endpoint
	if (Is_udd_endpoint_status_setup_received(status)) {
		// SETUP packet received
		dbg_print("Stp ");
		udd_ctrl_setup_received();
		return true;
	}
	if (Is_udd_endpoint_status_in_sent(status)
			&& Is_udd_in_send_interrupt_enabled(0)) {
		// IN packet sent
		dbg_print("In ");
		udd_ctrl_in_sent();
		return true;
	}
	if (Is_udd_endpoint_status_out_received(status)) {
		// OUT packet received
		dbg_print("Out ");
		udd_ctrl_out_received();
		return true;
	}
	if (Is_udd_endpoint_status_stall(status)) {
		dbg_print("Stall\n\r");
		// STALLed
		udd_ack_stall(0);
	}
	if (Is_udd_endpoint_status_nak_out(status)) {
		// Overflow on OUT packet
		dbg_print("NakO ");
		udd_ack_nak_out(0);
		udd_ctrl_overflow();
		return true;
	}
	if (Is_udd_endpoint_status_nak_in(status)) {
		// Underflow on IN packet
		dbg_print("NakI ");
		udd_ack_nak_in(0);
		udd_ctrl_underflow();
		return true;
	}
	dbg_print("UnH ");
	return false;
}


//--------------------------------------------------------
//--- INTERNAL ROUTINES TO MANAGED THE BULK/INTERRUPT/ISOCHRONOUS ENDPOINTS

#if (0!=USB_DEVICE_MAX_EP)

static void udd_ep_job_table_reset(void)
{
	uint8_t i;
	for (i = 0; i < USB_DEVICE_MAX_EP; i++) {
		udd_ep_job[i].busy = false;
		udd_ep_job[i].stall_requested = false;
	}
}


static void udd_ep_job_table_kill(void)
{
	uint8_t i;

	// For each endpoint, kill job
	for (i = 0; i < USB_DEVICE_MAX_EP; i++) {
		udd_ep_finish_job(&udd_ep_job[i], true, i + 1);
	}
}


static void udd_ep_abort_job(udd_ep_id_t ep)
{
	ep &= USB_EP_ADDR_MASK;

	// Abort job on endpoint
	udd_ep_finish_job(&udd_ep_job[ep - 1], true, ep);
}


static void udd_ep_finish_job(udd_ep_job_t * ptr_job, bool b_abort, uint8_t ep_num)
{
	if (ptr_job->busy == false) {
		return; // No on-going job
	}
	ptr_job->busy = false;
	dbg_print("JobE%d ", b_abort);
	if (NULL == ptr_job->call_trans) {
		return; // No callback linked to job
	}
	if (Is_udd_endpoint_in(ep_num)) {
		ep_num |= USB_EP_DIR_IN;
	}
	ptr_job->call_trans((b_abort) ? UDD_EP_TRANSFER_ABORT :
			UDD_EP_TRANSFER_OK, ptr_job->buf_size, ep_num);
}

static void udd_ep_trans_done(udd_ep_id_t ep)
{
	uint32_t udd_dma_ctrl = 0;
	udd_ep_job_t *ptr_job;
	iram_size_t next_trans;
	irqflags_t flags;

	// Get job corresponding at endpoint
	ptr_job = &udd_ep_job[ep - 1];

	if (!ptr_job->busy) {
		return; // No job is running, then ignore it (system error)
	}

	if (ptr_job->buf_cnt != ptr_job->buf_size) {
		// Need to send or receive other data
		next_trans = ptr_job->buf_size - ptr_job->buf_cnt;

		if (UDD_ENDPOINT_MAX_TRANS < next_trans) {
			// The USB hardware support a maximum
			// transfer size of UDD_ENDPOINT_MAX_TRANS Bytes
			next_trans = UDD_ENDPOINT_MAX_TRANS;

			// Set 0 to transfer the maximum
			udd_dma_ctrl = UDPHS_DMACONTROL_BUFF_LENGTH(0);
		} else {
			udd_dma_ctrl = UDPHS_DMACONTROL_BUFF_LENGTH(next_trans);
		}
		if (Is_udd_endpoint_in(ep)) {
			if (0 != (next_trans % udd_get_endpoint_size(ep))) {
				// Enable short packet option
				// else the DMA transfer is accepted
				// and interrupt DMA valid but nothing is sent.
				udd_dma_ctrl |= UDPHS_DMACONTROL_END_B_EN;
				// No need to request another ZLP
				ptr_job->b_shortpacket = false;
			}
		} else {
			if ((USB_EP_TYPE_ISOCHRONOUS != udd_get_endpoint_type(ep))
					|| (next_trans <= (iram_size_t) udd_get_endpoint_size(ep))) {

				// Enable short packet reception
				udd_dma_ctrl |= UDPHS_DMACONTROL_END_TR_IT
						| UDPHS_DMACONTROL_END_TR_EN;
			}
		}

		// Start USB DMA to fill or read fifo of the selected endpoint
		udd_endpoint_dma_set_addr(ep, (uint32_t) & ptr_job->buf[ptr_job->buf_cnt]);
		udd_dma_ctrl |= UDPHS_DMACONTROL_END_BUFFIT | UDPHS_DMACONTROL_CHANN_ENB;

		// Disable IRQs to have a short sequence
		// between read of EOT_STA and DMA enable
		flags = cpu_irq_save();
		if (!(udd_endpoint_dma_get_status(ep)
				& UDPHS_DMASTATUS_END_TR_ST)) {
			udd_endpoint_dma_set_control(ep, udd_dma_ctrl);
			ptr_job->buf_cnt += next_trans;
			ptr_job->buf_load = next_trans;
			udd_enable_endpoint_dma_interrupt(ep);
			cpu_irq_restore(flags);
			return;
		}
		cpu_irq_restore(flags);

		// Here a ZLP has been received
		// and the DMA transfer must be not started.
		// It is the end of transfer
		ptr_job->buf_size = ptr_job->buf_cnt;
	}
	if (Is_udd_endpoint_in(ep)) {
		if (ptr_job->b_shortpacket) {
			dbg_print("zlp ");
			// Need to send a ZLP (No possible with USB DMA)
			// enable interrupt to wait a free bank to sent ZLP
			udd_enable_tx_pkt_ready_interrupt(ep);
			udd_enable_endpoint_interrupt(ep);
			return;
		}
	}
	// Call callback to signal end of transfer
	udd_ep_finish_job(ptr_job, false, ep);
}


static bool udd_ep_interrupt(void)
{
	udd_ep_id_t ep;
	udd_ep_job_t *ptr_job;

	// For each endpoint different of control endpoint (0)
	for (ep = 1; ep <= USB_DEVICE_MAX_EP; ep++) {
		// Get job corresponding at endpoint
		ptr_job = &udd_ep_job[ep - 1];

		// Check DMA event
		if (Is_udd_endpoint_dma_interrupt_enabled(ep)
				&& Is_udd_endpoint_dma_interrupt(ep)) {
			uint32_t nb_remaining;
			if (udd_endpoint_dma_get_status(ep)
					& UDPHS_DMASTATUS_CHANN_ENB) {
				return true; // Ignore EOT_STA interrupt
			}
			dbg_print("dma%d: ", ep);
			udd_disable_endpoint_dma_interrupt(ep);
			// Save number of data no transfered
			nb_remaining = (udd_endpoint_dma_get_status(ep) &
					UDPHS_DMASTATUS_BUFF_COUNT_Msk)
					>> UDPHS_DMASTATUS_BUFF_COUNT_Pos;
			if (nb_remaining) {
				// Transfer no complete (short packet or ZLP) then:
				// Update number of data transfered
				ptr_job->buf_cnt -= nb_remaining;
				// Set transfer complete to stop the transfer
				ptr_job->buf_size = ptr_job->buf_cnt;
			}
			udd_ep_trans_done(ep);
			return true;
		}
		// Check empty bank interrupt event
		if (Is_udd_endpoint_interrupt_enabled(ep)) {
			if (Is_udd_tx_pkt_ready_interrupt_enabled(ep)
					&& !Is_udd_tx_pkt_ready(ep)) {
				udd_disable_tx_pkt_ready_interrupt(ep);
				// One bank is free then send a ZLP
				udd_raise_tx_pkt_ready(ep);
				udd_ep_finish_job(ptr_job, false, ep);
				return true;
			}
			if (Is_udd_bank_interrupt_enabled(ep)
					&& (0 == udd_nb_busy_bank(ep))) {
				// End of background transfer on IN endpoint
				udd_disable_bank_interrupt(ep);
				udd_disable_endpoint_interrupt(ep);

				Assert(ptr_job->stall_requested);
				// A stall has been requested during background transfer
				ptr_job->stall_requested = false;
				udd_disable_endpoint_bank_autoswitch(ep);
				udd_enable_stall_handshake(ep);
				udd_reset_data_toggle(ep);
				//dbg_print("exHalt%x ", ep);
				return true;
			}
		}
	}
	return false;
}
#endif // (0!=USB_DEVICE_MAX_EP)

//@}
