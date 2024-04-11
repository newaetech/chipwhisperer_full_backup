/**
 * \file
 *
 * \brief USB Driver header file for XMEGA products including USB interface.
 *
 * Copyright (c) 2011-2012 Atmel Corporation. All rights reserved.
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

#ifndef _USB_DEVICE_H_
#define _USB_DEVICE_H_

#include <nvm.h>

/**
 * \ingroup udd_group
 * \defgroup udd_xmega_usb_group Xmega USB Device Driver
 * USBC low-level driver for USB Device mode
 *
 *
 * @{
 */

//! @name USB Device main management
//! @{

/**
 * \brief Initializes the USB DP/DM buffers
 *
 * This functions initializes the USB buffer using the calibration value
 * stored in production raw.
 * If the calibration value is not found (0xFF) value, a default typical
 * value is applied.
 * Alternatively user can force calibration values using USB_PAD_USER_CAL0
 * and USB_PAD_USER_CAL1
 *
 */
static inline void usb_pad_init(void)
{
	uint8_t cal;

#ifdef USB_PAD_USER_CAL0
	USB_CAL0 = USB_PAD_USER_CAL0;
#else
	cal = nvm_read_production_signature_row
			(nvm_get_production_signature_row_offset(USBCAL0));
	if (cal != 0xFF) {
		USB_CAL0 = cal;
	} else {
		USB_CAL0 = 0x1F;
	}
#endif

#ifdef USB_PAD_USER_CAL1
	USB_CAL1 = USB_PAD_USER_CAL1;
#else
	cal = nvm_read_production_signature_row
			(nvm_get_production_signature_row_offset(USBCAL1));
	if (cal != 0xFF) {
		USB_CAL1 = cal;
	} else {
		USB_CAL1 = 0x1F;
	}
#endif
}

#define  udd_enable_interface()                    (USB_CTRLA |= USB_ENABLE_bm)
#define  udd_disable_interface()                   (USB_CTRLA &= ~USB_ENABLE_bm)
#define  udd_attach_device()                       (USB_CTRLB |= USB_ATTACH_bm)
#define  udd_detach_device()                       (USB_CTRLB &= ~USB_ATTACH_bm)
#define  udd_gnak_disable()                        (USB_CTRLB &= ~USB_GNACK_bm)
#define  udd_gnak_enable()                         (USB_CTRLB |=  USB_GNACK_bm)
#define  udd_gnak_is_enable()                      (USB_CTRLB & USB_GNACK_bm)
#define  udd_set_nb_max_ep(n)                      (USB_CTRLA |= n)
#define  udd_enable_store_frame_number()           (USB_CTRLA |= USB_STFRNUM_bm)
#define  udd_disable_store_frame_number()          (USB_CTRLA &= ~USB_STFRNUM_bm)
#define  udd_set_full_speed()                      (USB_CTRLA |= USB_SPEED_bm)
#define  udd_set_low_speed()                       (USB_CTRLA &= ~USB_SPEED_bm)
#define  udd_set_ep_table_addr(n)                  (USB.EPPTR = (uint16_t)n)
#define  udd_get_ep_table_addr()                   (USB.EPPTR)
#define  udd_get_fifo_rp()                         (USB_FIFORP)
#define  udd_reset_fifo()                          (USB_FIFORP=0xFF)
#define  udd_enable_interrupt(level)               (USB_INTCTRLA |= level&(USB_INTLVL1_bm|USB_INTLVL0_bm))

#define  udd_set_device_address(n)                 (USB_ADDR=n)
#define  udd_get_device_address()                  (USB_ADDR)
#define  udd_enable_fifo()                         (USB_CTRLA |= USB_FIFOEN_bm)
#define  udd_disable_fifo()                        (USB_CTRLA &= ~USB_FIFOEN_bm)

#define  udd_send_remote_wake_up()                 (USB_CTRLB &= ~USB_RWAKEUP_bm, USB_CTRLB |= USB_RWAKEUP_bm)
#define  udd_set_global_nack()                     (USB_CTRLB |= USB_GNACK_bm)
#define  udd_is_crc_event()                        (USB_INTFLAGSASET & USB_CRCIF_bm ? true : false)
#define  udd_ack_crc_event()                       (USB_INTFLAGSACLR = USB_CRCIF_bm)
#define  udd_set_crc_event()                       (USB_INTFLAGSASET = USB_CRCIF_bm)
#define  udd_enable_crc_interrupt()                (USB_INTCTRLA |= USB_CRCIE_bm)
#define  udd_disable_crc_interrupt()               (USB_INTCTRLA &= ~USB_CRCIE_bm)

#define  udd_is_start_of_frame_event()             (USB_INTFLAGSASET & USB_SOFIF_bm ? true : false)
#define  udd_ack_start_of_frame_event()            (USB_INTFLAGSACLR = USB_SOFIF_bm)
#define  udd_set_start_of_frame_event()            (USB_INTFLAGSASET = USB_SOFIF_bm)
#define  udd_enable_start_of_frame_interrupt()     (USB_INTCTRLA |= USB_SOFIE_bm)
#define  udd_disable_start_of_frame_interrupt()    (USB_INTCTRLA &= ~USB_SOFIE_bm)
#define  udd_is_enable_start_of_frame_interrupt()  (0!=(USB_INTCTRLA|USB_SOFIE_bm))

#define  udd_is_reset_event()                      (USB_INTFLAGSASET & USB_RSTIF_bm ? true : false)
#define  udd_ack_reset_event()                     (USB_INTFLAGSACLR = USB_RSTIF_bm)
#define  udd_set_reset_event()                     (USB_INTFLAGSASET = USB_RSTIF_bm)

#define  udd_is_suspend_event()                    (USB_INTFLAGSASET & USB_SUSPENDIF_bm ? true : false)
#define  udd_ack_suspend_event()                   (USB_INTFLAGSACLR = USB_SUSPENDIF_bm)
#define  udd_set_suspend_event()                   (USB_INTFLAGSASET = USB_SUSPENDIF_bm)

#define  udd_is_resume_event()                     (USB_INTFLAGSASET & USB_RESUMEIF_bm ? true : false)
#define  udd_ack_resume_event()                    (USB_INTFLAGSACLR = USB_RESUMEIF_bm)
#define  udd_set_resume_event()                    (USB_INTFLAGSASET = USB_RESUMEIF_bm)

#define  udd_enable_busevt_interrupt()             (USB_INTCTRLA |= USB_BUSEVIE_bm)
#define  udd_disable_busevt_interrupt()            (USB_INTCTRLA &= ~USB_BUSEVIE_bm)

#define  udd_is_setup_event()                      (USB_INTFLAGSBCLR & USB_SETUPIF_bm ? true : false)
#define  udd_ack_setup_event()                     (USB_INTFLAGSBCLR = USB_SETUPIF_bm)
#define  udd_set_setup_event()                     (USB_INTFLAGSBSET = USB_SETUPIF_bm)
#define  udd_enable_setup_interrupt()              (USB_INTCTRLB |= USB_SETUPIE_bm)
#define  udd_disable_setup_interrupt()             (USB_INTCTRLB &= ~USB_SETUPIE_bm)

#define  udd_is_tc_event()                         (USB_INTFLAGSBCLR & USB_TRNIF_bm ? true : false)
#define  udd_ack_tc_event()                        (USB_INTFLAGSBCLR = USB_TRNIF_bm)
#define  udd_set_tc_event()                        (USB_INTFLAGSBSET = USB_TRNIF_bm)
#define  udd_enable_tc_interrupt()                 (USB_INTCTRLB |= USB_TRNIE_bm)
#define  udd_disable_tc_interrupt()                (USB_INTCTRLB &= ~USB_TRNIE_bm)

#define  udd_is_overflow_event()                   (USB_INTFLAGSASET & USB_OVFIF_bm ? true : false)
#define  udd_ack_overflow_event()                  (USB_INTFLAGSACLR = USB_OVFIF_bm)
#define  udd_set_overflow_event()                  (USB_INTFLAGSASET = USB_OVFIF_bm)
#define  udd_enable_overflow_interrupt()           (USB_INTCTRLA |= USB_BUSERRIE_bm)
#define  udd_disable_overflow_interrupt()          (USB_INTCTRLA &= ~USB_BUSERRIE_bm)
#define  udd_is_enable_overflow_interrupt()        (USB_INTCTRLA&USB_BUSERRIE_bm ? true : false)

#define  udd_is_underflow_event()                  (USB_INTFLAGSASET & USB_UNFIF_bm ? true : false)
#define  udd_ack_underflow_event()                 (USB_INTFLAGSACLR = USB_UNFIF_bm)
#define  udd_set_underflow_event()                 (USB_INTFLAGSASET = USB_UNFIF_bm)
#define  udd_enable_underflow_interrupt()          (USB_INTCTRLA |= USB_BUSERRIE_bm)
#define  udd_disable_underflow_interrupt()         (USB_INTCTRLA &= ~USB_BUSERRIE_bm)
#define  udd_is_enable_underflow_interrupt()       (USB_INTCTRLA&USB_BUSERRIE_bm ? true : false)

#define  udd_is_stall_event()                      (USB_INTFLAGSASET & USB_STALLIF_bm ? true : false)
#define  udd_ack_stall_event()                     (USB_INTFLAGSACLR = USB_STALLIF_bm)
#define  udd_set_stall_event()                     (USB_INTFLAGSASET = USB_STALLIF_bm)
#define  udd_enable_stall_interrupt()              (USB_INTCTRLA |= USB_STALLIE_bm)
#define  udd_disable_stall_interrupt()             (USB_INTCTRLA &= ~USB_STALLIE_bm)
#define  udd_is_enable_stall_interrupt()           (USB_INTCTRLA&USB_STALLIE_bm ? true : false)
//! @}

//! @name USB Device read/modify/write management
//! @{
#ifndef USB_WORKAROUND_DO_NOT_USE_RMW
/*
 * Read modify write new instructions for Xmega
 * inline asm implementation with R16 register.
 * This should be removed later on when the new instructions
 * will be available within the compiler.
 *
 */
// Load and Clear
#ifdef __GNUC__
#define LACR16(addr,msk) \
   __asm__ __volatile__ ( \
         "ldi r16, %1" "\n\t" \
         ".dc.w 0x9306" "\n\t"\
         ::"z" (addr), "M" (msk):"r16")
#else
#define LACR16(addr,msk) __lac((unsigned char)msk,(unsigned char*)addr)
#endif

// Load and Set
#ifdef __GNUC__
#define LASR16(addr,msk) \
   __asm__ __volatile__ ( \
         "ldi r16, %1" "\n\t" \
         ".dc.w 0x9305" "\n\t"\
         ::"z" (addr), "M" (msk):"r16")
#else
#define LASR16(addr,msk) __las((unsigned char)msk,(unsigned char*)addr)
#endif

// Exchange
#ifdef __GNUC__
#define XCHR16(addr,msk) \
   __asm__ __volatile__ ( \
         "ldi r16, %1" "\n\t" \
         ".dc.w 0x9304" "\n\t"\
         ::"z" (addr), "M" (msk):"r16")
#else
#define XCHR16(addr,msk) __xch(msk,addr)
#endif

// Load and toggle
#ifdef __GNUC__
#define LATR16(addr,msk) \
   __asm__ __volatile__ ( \
         "ldi r16, %1" "\n\t" \
         ".dc.w 0x9307" "\n\t"\
         ::"z" (addr), "M" (msk):"r16")
#else
#define LATR16(addr,msk) __lat(msk,addr)
#endif

#else

// Load and Clear
#define LACR16(addr,msk) (*addr &= ~msk)
// Load and Set
#define LASR16(addr,msk)(*addr |= msk)

#endif
//! @}


//! @name USB Device endpoints table management
//! @{

#define  udd_endpoint_set_control(ep_ctrl,val)             (ep_ctrl->CTRL=val)
#define  udd_endpoint_get_control(ep_ctrl)                (ep_ctrl->CTRL)

#define  udd_endpoint_disable(ep_ctrl)                    udd_endpoint_set_control(ep_ctrl,0)
#define  udd_endpoint_is_enable(ep_ctrl)                  (USB_EP_TYPE_DISABLE_gc!=udd_endpoint_get_type(ep_ctrl))


#define  udd_endpoint_enable_stall(ep_ctrl)               (ep_ctrl->CTRL |= USB_EP_STALL_bm)
#define  udd_endpoint_disable_stall(ep_ctrl)              (ep_ctrl->CTRL &= ~USB_EP_STALL_bm)
#define  udd_endpoint_is_stall(ep_ctrl)                   (ep_ctrl->CTRL &USB_EP_STALL_bm ? true : false)
#define  udd_endpoint_set_multipacket(ep_ctrl)            (ep_ctrl->CTRL |= USB_EP_MULTIPKT_bm)
#define  udd_endpoint_TC_int_disable(ep_ctrl)             (ep_ctrl->CTRL |= USB_EP_INTDSBL_bm)
#define  udd_endpoint_set_pingpong(ep_ctrl)               (ep_ctrl->CTRL |= USB_EP_PINGPONG_bm)
#define  udd_endpoint_get_size_field(ep_ctrl)             (ep_ctrl->CTRL & USB_EP_BUFSIZE_gm)
#define  udd_endpoint_get_type(ep_ctrl)                   (ep_ctrl->CTRL & USB_EP_TYPE_gm)

#define  udd_endpoint_get_status(ep_ctrl)                 (ep_ctrl->STATUS)
#define  udd_endpoint_clear_status(ep_ctrl)               (ep_ctrl->STATUS=USB_EP_BUSNACK0_bm|USB_EP_BUSNACK1_bm)

#define  udd_endpoint_setup_received(ep_ctrl)             (ep_ctrl->STATUS&USB_EP_SETUP_bm ? true : false)
#define  udd_endpoint_ack_setup_received(ep_ctrl)         LACR16(&ep_ctrl->STATUS, USB_EP_SETUP_bm)

#define  udd_endpoint_transfer_complete(ep_ctrl)          (ep_ctrl->STATUS&USB_EP_TRNCOMPL0_bm ? true : false)
#define  udd_endpoint_ack_transfer_complete(ep_ctrl)      LACR16(&(ep_ctrl->STATUS), USB_EP_TRNCOMPL0_bm)
#define  udd_endpoint_transfer_complete_bank0(ep_ctrl)    (ep_ctrl->STATUS&USB_EP_TRNCOMPL0_bm ? true : false)
#define  udd_endpoint_ack_transfer_complete_bankO(ep_ctrl) LACR16(&ep_ctrl->STATUS, USB_EP_TRNCOMPL0_bm)
#define  udd_endpoint_transfer_complete_bank1(ep_ctrl)    (ep_ctrl->STATUS&USB_EP_SETUP_bm ? true : false)
#define  udd_endpoint_ack_transfer_complete_bank1(ep_ctrl) LACR16(&ep_ctrl->STATUS, USB_EP_SETUP_bm)

#define  udd_endpoint_get_bank(ep_ctrl)                   (ep_ctrl->STATUS & USB_EP_BANK_bm ? true : false)
#define  udd_endpoint_set_bank(ep_ctrl)                   LASR16(&ep_ctrl->STATUS, USB_EP_BANK_bm)
#define  udd_endpoint_clear_bank(ep_ctrl)                 LACR16(&ep_ctrl->STATUS, USB_EP_BANK_bm)

#define  udd_endpoint_set_dtgl(ep_ctrl)                   LASR16(&ep_ctrl->STATUS,USB_EP_TOGGLE_bm)
#define  udd_endpoint_clear_dtgl(ep_ctrl)                 LACR16(&ep_ctrl->STATUS, USB_EP_TOGGLE_bm )
#define  udd_endpoint_get_dtgl(ep_ctrl)                   ((ep_ctrl->STATUS)&USB_EP_TOGGLE_bm ? true : false)
#define  udd_endpoint_toggle_dtgl(ep_ctrl)                LATR16(&ep_ctrl->STATUS, USB_EP_TOGGLE_bm)

#define  udd_endpoint_set_NACK0(ep_ctrl)                  LASR16(&ep_ctrl->STATUS,USB_EP_BUSNACK0_bm)
#define  udd_endpoint_set_NACK1(ep_ctrl)                  LASR16(&ep_ctrl->STATUS,USB_EP_BUSNACK1_bm)
#define  udd_endpoint_clear_NACK0(ep_ctrl)                LACR16(&ep_ctrl->STATUS, USB_EP_BUSNACK0_bm)
#define  udd_endpoint_clear_NACK1(ep_ctrl)                LACR16(&ep_ctrl->STATUS, USB_EP_BUSNACK1_bm)
#define  udd_endpoint_get_NACK1(ep_ctrl)                  ((ep_ctrl->STATUS&USB_EP_BUSNACK1_bm) ? true : false)
#define  udd_endpoint_get_NACK0(ep_ctrl)                  ((ep_ctrl->STATUS&USB_EP_BUSNACK0_bm) ? true : false)
#define  udd_endpoint_overflow(ep_ctrl)                   (ep_ctrl->STATUS&USB_EP_OVF_bm ? true : false)
#define  udd_endpoint_underflow(ep_ctrl)                  (ep_ctrl->STATUS&USB_EP_UNF_bm ? true : false)

#define  UDD_ENDPOINT_MAX_TRANS                           (0x3FF)

#define  udd_endpoint_out_nb_receiv(ep_ctrl)              (ep_ctrl->CNT)
#define  udd_endpoint_out_reset_nb_received(ep_ctrl)      (ep_ctrl->CNT = 0)
#define  udd_endpoint_in_set_bytecnt(ep_ctrl,n)           (ep_ctrl->CNT = n)
#define  udd_endpoint_set_azlp(ep_ctrl)                   (ep_ctrl->CNT |= 0x8000)
#define  udd_endpoint_clear_azlp(ep_ctrl)                 (ep_ctrl->CNT &= ~0x8000)

#define  udd_endpoint_set_buf(ep_ctrl,buf)                 (ep_ctrl->DATAPTR = (uint16_t) buf)

#define  udd_endpoint_in_nb_sent(ep_ctrl)                 (ep_ctrl->AUXDATA)
#define  udd_endpoint_in_reset_nb_sent(ep_ctrl)           (ep_ctrl->AUXDATA = 0)
#define  udd_endpoint_out_set_nbbyte(ep_ctrl,nb)           (ep_ctrl->AUXDATA = nb)
#define  udd_endpoint_out_get_nbbyte_requested(ep_ctrl)    (ep_ctrl->AUXDATA)
#define  udd_endpoint_set_aux(ep_ctrl,buf)                 (ep_ctrl->AUXDATA = (uint16_t) buf)
//! @}


//! @name USB Device endpoint control field management
//! @{

//! @name USB Device endpoint control setup field management
//! @{
#define  udd_control_setup()                       (udd_sram.ep_ctrl[0].STATUS&USB_EP_SETUP_bm ? true : false)
#define  udd_control_ack_setup()                   LACR16(&udd_sram.ep_ctrl[0].STATUS,USB_EP_SETUP_bm)
//! @}

//! @name USB Device endpoint control OUT field management
//! @{
#define  udd_control_out_is_enable_stall()         (udd_sram.ep_ctrl[0].CTRL&USB_EP_STALL_bm ? true : false)
#define  udd_control_out_enable_stall()            LASR16(&udd_sram.ep_ctrl[0].CTRL,USB_EP_STALL_bm)
#define  udd_control_out_disable_stall()           LACR16(&udd_sram.ep_ctrl[0].CTRL,USB_EP_STALL_bm)
#define  udd_control_out_is_stalled()              (udd_sram.ep_ctrl[0].STATUS&USB_EP_STALLF_bm ? true : false)
#define  udd_control_out_ack_stall()               LACR16(&udd_sram.ep_ctrl[0].STATUS,USB_EP_STALLF_bm)
#define  udd_control_out_set_NACK0()               LASR16(&udd_sram.ep_ctrl[0].STATUS,USB_EP_BUSNACK0_bm)
#define  udd_control_out_clear_NACK0()             LACR16(&udd_sram.ep_ctrl[0].STATUS,USB_EP_BUSNACK0_bm)

#define  udd_control_out_overflow()                (udd_sram.ep_ctrl[0].STATUS&USB_EP_OVF_bm ? true : false)
#define  udd_control_ack_out_overflow()            LACR16(&udd_sram.ep_ctrl[0].STATUS,USB_EP_OVF_bm)

#define  udd_control_out_tc()                      (udd_sram.ep_ctrl[0].STATUS&USB_EP_TRNCOMPL0_bm ? true : false)
#define  udd_control_out_ack_tc()                  LACR16(&udd_sram.ep_ctrl[0].STATUS,USB_EP_TRNCOMPL0_bm)
#define  udd_control_out_set_tc()                  LASR16(&udd_sram.ep_ctrl[0].STATUS,USB_EP_TRNCOMPL0_bm)

#define  udd_control_out_dt_get()                  (udd_sram.ep_ctrl[0].STATUS&USB_EP_TOGGLE_bm ? true : false)
#define  udd_control_out_dt_set()                  LASR16(&udd_sram.ep_ctrl[0].STATUS,USB_EP_TOGGLE_bm )
#define  udd_control_out_dt_clear()                LACR16(&udd_sram.ep_ctrl[0].STATUS,USB_EP_TOGGLE_bm )
#define  udd_control_out_dt_toggle()               LATR16(&udd_sram.ep_ctrl[0].STATUS,USB_EP_TOGGLE_bm)

#define  udd_control_out_set_buf(buf)              (udd_sram.ep_ctrl[0].DATAPTR = (uint16_t) buf)

#define  udd_control_out_get_bytecnt()             (udd_sram.ep_ctrl[0].CNT)
//! @}

//! @name USB Device endpoint control IN field management
//! @{
#define  udd_control_in_is_enable_stall()          (udd_sram.ep_ctrl[1].CTRL&USB_EP_STALL_bm ? true : false)
#define  udd_control_in_enable_stall()             LASR16(&udd_sram.ep_ctrl[1].CTRL,USB_EP_STALL_bm)
#define  udd_control_in_disable_stall()            LACR16(&udd_sram.ep_ctrl[1].CTRL,USB_EP_STALL_bm)
#define  udd_control_in_is_stalled()               (udd_sram.ep_ctrl[1].STATUS&USB_EP_STALLF_bm ? true : false)
#define  udd_control_in_ack_stall()                LACR16(&udd_sram.ep_ctrl[1].STATUS,USB_EP_STALLF_bm)
#define  udd_control_in_set_NACK0()                LASR16(&udd_sram.ep_ctrl[1].STATUS,USB_EP_BUSNACK0_bm)
#define  udd_control_in_clear_NACK0()              LACR16(&udd_sram.ep_ctrl[1].STATUS,USB_EP_BUSNACK0_bm)

#define  udd_control_in_underflow()                (udd_sram.ep_ctrl[1].STATUS&USB_EP_UNF_bm ? true : false)
#define  udd_control_ack_in_underflow()            LACR16(&udd_sram.ep_ctrl[1].STATUS,USB_EP_UNF_bm)

#define  udd_control_in_tc()                       (udd_sram.ep_ctrl[1].STATUS&USB_EP_TRNCOMPL0_bm ? true : false)
#define  udd_control_in_ack_tc()                   LACR16(&udd_sram.ep_ctrl[1].STATUS,USB_EP_TRNCOMPL0_bm)
#define  udd_control_in_set_tc()                   LASR16(&udd_sram.ep_ctrl[1].STATUS,USB_EP_TRNCOMPL0_bm)

#define  udd_control_in_dt_get()                   (udd_sram.ep_ctrl[1].STATUS&USB_EP_TOGGLE_bm ? true : false)
#define  udd_control_in_dt_set()                   LASR16(&udd_sram.ep_ctrl[1].STATUS,USB_EP_TOGGLE_bm )
#define  udd_control_in_dt_clear()                 LACR16(&udd_sram.ep_ctrl[1].STATUS,USB_EP_TOGGLE_bm )
#define  udd_control_in_dt_toggle()                LATR16(&udd_sram.ep_ctrl[1].STATUS,USB_EP_TOGGLE_bm)

#define  udd_control_in_set_buf(buf)               (udd_sram.ep_ctrl[1].DATAPTR = (uint16_t) buf)

#define  udd_control_in_set_bytecnt(n)             (udd_sram.ep_ctrl[1].CNT = n)
//! @}
//! @}

//! @}

#endif // _USB_DEVICE_H_
