/**
 * \file
 *
 * \brief AVR XMEGA Analog to Digital Converter driver
 *
 * Copyright (C) 2010-2015 Atmel Corporation. All rights reserved.
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
#ifndef ADC_H
#define ADC_H

#include <compiler.h>
#include <conf_adc.h>
#include <nvm.h>
#include <parts.h>
#include <sleepmgr.h>
#include <sysclk.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Fix header error in ADC_CH_t structure about missing SCAN register */
#ifndef ADC_CH_OFFSET_gp
# define ADC_CH_OFFSET_gp  4  /* Positive MUX setting offset group position. */
# if XMEGA_A || XMEGA_D
#   ifdef __ICCAVR__
#     define SCAN   reserved_0x06
#   else
#     define SCAN   reserved_0x6
#   endif
# endif
#endif

/* Fix header error */
#define ADC_EVACT_SYNCSWEEP_gc  (0x06 << 0)
#define ADC_REFSEL_INTVCC_gc   (0x01 << 4)
#define ADC_REFSEL_VCCDIV2_gc  (0x04 << 4)
#define ADC_CH_GAIN_DIV2_gc    (0x07 << 2)
#if (!XMEGA_A)
/* ADC.CTRLB  bit masks and bit positions */
#  define ADC_CURRLIMIT_NO_gc   (0x00 << 5)
#  define ADC_CURRLIMIT_LOW_gc  (0x01 << 5)
#  define ADC_CURRLIMIT_MED_gc  (0x02 << 5)
#  define ADC_CURRLIMIT_HIGH_gc (0x03 << 5)
#endif
#if (!XMEGA_A) && (!defined ADC_CURRLIMIT_gm)
/* ADC.CTRLB  bit masks and bit positions */
#  define ADC_CURRLIMIT_gm  0x60  /* Current limit group mask. */
#endif
#if (!XMEGA_E)
/* Negative input multiplexer selection without gain */
typedef enum ADC_CH_MUXNEG_MODE10_enum
{
    ADC_CH_MUXNEG_MODE10_PIN0_gc = (0x00<<0),  /* Input pin 0 */
    ADC_CH_MUXNEG_MODE10_PIN1_gc = (0x01<<0),  /* Input pin 1 */
    ADC_CH_MUXNEG_MODE10_PIN2_gc = (0x02<<0),  /* Input pin 2 */
    ADC_CH_MUXNEG_MODE10_PIN3_gc = (0x03<<0),  /* Input pin 3 */
    ADC_CH_MUXNEG_MODE10_GND_gc = (0x05<<0),  /* PAD ground */
    ADC_CH_MUXNEG_MODE10_INTGND_gc = (0x07<<0),  /* Internal ground */
} ADC_CH_MUXNEGL_t;

/* Negative input multiplexer selection with gain */
typedef enum ADC_CH_MUXNEG_MODE11_enum
{
    ADC_CH_MUXNEG_MODE11_PIN4_gc = (0x00<<0),  /* Input pin 4 */
    ADC_CH_MUXNEG_MODE11_PIN5_gc = (0x01<<0),  /* Input pin 5 */
    ADC_CH_MUXNEG_MODE11_PIN6_gc = (0x02<<0),  /* Input pin 6 */
    ADC_CH_MUXNEG_MODE11_PIN7_gc = (0x03<<0),  /* Input pin 7 */
    ADC_CH_MUXNEG_MODE11_INTGND_gc = (0x04<<0),  /* Internal ground */
    ADC_CH_MUXNEG_MODE11_GND_gc = (0x05<<0),  /* PAD ground */
} ADC_CH_MUXNEGH_t;
#endif

/**
 * \defgroup adc_group Analog to Digital Converter (ADC)
 *
 * See \ref adc_quickstart.
 *
 * This is a driver for the AVR XMEGA ADC. It provides functions for enabling,
 * disabling and configuring the ADC modules and their individual channels.
 *
 * The driver API is split in two parts:
 * - \ref adc_channel_group
 * - \ref adc_module_group
 *
 * Both APIs use structures that contain the configuration. These structures
 * must be set up before the configuration is written to either an ADC module or
 * one of their channels.
 *
 * After the ADC has been configured it must be enabled before any conversions
 * may be performed. To ensure accurate conversions, please wait for at least
 * the specified start-up time between enabling the ADC module, and starting
 * a conversion. For most XMEGA devices the start-up time is specified
 * to be a maximum of 24 ADC clock cycles. Please verify the start-up time for
 * the device in use.
 *
 * \note Not all of the documented functions are available on all devices. This
 * is due to differences in the ADC feature set. Refer to the device manual and
 * datasheet for details on which features are available for a specific device.
 *
 * \note The functions for creating/changing configurations are not protected
 * against interrupts. The functions that read from or write to the ADC's
 * registers are protected unless otherwise noted.
 *
 * \section dependencies Dependencies
 * This driver depends on the following modules:
 * - \ref sysclk_group for peripheral clock control.
 * - \ref sleepmgr_group for setting allowed sleep mode.
 * - \ref nvm_group for getting factory calibration data.
 * - \ref interrupt_group for ISR definition and disabling interrupts during
 * critical code sections.
 * @{
 */

/**
 * \defgroup adc_module_group ADC module
 *
 * Management and configuration functions for the ADC module.
 *
 * The API functions and definitions can be divided in three groups:
 * - interrupt callback: configure and set interrupt callback function.
 * - module management: direct access for enabling and disabling the ADC,
 * starting conversions, getting interrupt flags, etc.
 * - module configuration: create/change configurations and write/read them
 * to/from an ADC.
 *
 * @{
 */

/**
 * \def ADC_NR_OF_CHANNELS
 * \brief Number of channels per ADC
 */
#if XMEGA_A || XMEGA_AU || defined(__DOXYGEN__)
#  define ADC_NR_OF_CHANNELS    4
#elif XMEGA_B || XMEGA_C || XMEGA_D || XMEGA_E
#  define ADC_NR_OF_CHANNELS    1
#endif

/** ADC configuration */
struct adc_config {
#if ADC_NR_OF_CHANNELS > 1
	/* DMA group request is stored in CTRLA */
	uint8_t ctrla;
#endif
	uint8_t ctrlb;
	uint8_t refctrl;
	uint8_t evctrl;
	uint8_t prescaler;
	uint16_t cmp;
#if XMEGA_E
	/* XMEGA E sample time value stored in SAMPCTRL */
	uint8_t sampctrl;
#endif
};

/**
 * \name Calibration data addresses
 * \note The temperature sensor calibration is sampled at 85 degrees Celsius
 * with unsigned, 12-bit conversion.
 */
/** @{ */

/** ADC A, calibration byte 0. */
#define ADCACAL0      offsetof(NVM_PROD_SIGNATURES_t, ADCACAL0)
/** ADC A, calibration byte 1. */
#define ADCACAL1      offsetof(NVM_PROD_SIGNATURES_t, ADCACAL1)
/** ADC B, calibration byte 0. */
#define ADCBCAL0      offsetof(NVM_PROD_SIGNATURES_t, ADCBCAL0)
/** ADC B, calibration byte 1. */
#define ADCBCAL1      offsetof(NVM_PROD_SIGNATURES_t, ADCBCAL1)
/** Temperature sensor calibration byte 0. */
#define TEMPSENSE0    offsetof(NVM_PROD_SIGNATURES_t, TEMPSENSE0)
/** Temperature sensor calibration byte 1. */
#define TEMPSENSE1    offsetof(NVM_PROD_SIGNATURES_t, TEMPSENSE1)
/** Temperature at which TEMPSENSE1/TEMPSENSE0 is measured. */
#define HOTTEMP       offsetof(NVM_PROD_SIGNATURES_t, HOTTEMP)
/** Temperature at which TEMPSENSE3/TEMPSENSE2 is measured. */
#define ROOMTEMP      offsetof(NVM_PROD_SIGNATURES_t, ROOMTEMP)
/** Temperature sensor calibration byte 2. */
#define TEMPSENSE2    offsetof(NVM_PROD_SIGNATURES_t, TEMPSENSE2)
/** Temperature sensor calibration byte 3. */
#define TEMPSENSE3    offsetof(NVM_PROD_SIGNATURES_t, TEMPSENSE3)
/** @} */

/** \brief ADC calibration data */
enum adc_calibration_data {
	/** ADC A pipeline calibration data. */
	ADC_CAL_ADCA,
	/** ADC B pipeline calibration data. */
	ADC_CAL_ADCB,
	/**
	 * \brief Temperature sensor calibration data.
	 * \note 12-bit unsigned, measured at 85 degrees Celsius, equivalent to
	 * 358.15 kelvin.
	 *
	 * For AVR XMEGA E devices, the calibration reading is 12 Bit signed.
	 * AVR XMEGA E production signature row contains data for two 
	 * calibration points. Each calibration point has one byte for
	 * storing the temperature at which the internal temperature sensor
	 * is measured and two bytes for the corresponding ADC reading.
	 */
	ADC_CAL_TEMPSENSE,
	/** Normally one calibration point is taken at 85 Deg C,
	 * but the exact value in Deg C is given in the HOTTEMP signature row.
	 */
	ADC_CAL_HOTTEMP,
	/** ADC reading at ROOMTEMP in stored in TEMPSENSE2. */
	ADC_CAL_TEMPSENSE2,
	/** The room temperature (in Deg C) measured during device manufacturing.
	 */
	ADC_CAL_ROOMTEMP
};

/** \name ADC channel masks */
/** @{ */

#define ADC_CH0      (1U << 0)                 /**< ADC channel 0. */

#if XMEGA_A || XMEGA_AU || defined(__DOXYGEN__)
#  define ADC_CH1    (1U << 1)                 /**< ADC channel 1. */
#  define ADC_CH2    (1U << 2)                 /**< ADC channel 2. */
#  define ADC_CH3    (1U << 3)                 /**< ADC channel 3. */
#endif

/** @} */

/** \name Internal ADC input masks */
/** @{ */

#define ADC_INT_TEMPSENSE    ADC_TEMPREF_bm    /**< Temperature sensor. */
#define ADC_INT_BANDGAP      ADC_BANDGAP_bm    /**< Bandgap reference. */

/** @} */

/**
 * \brief ADC conversion trigger settings
 *
 * \note The choice in conversion triggers varies between device families.
 * Refer to the device manual for detailed information.
 */
enum adc_trigger {
	/** Manually triggered conversions */
	ADC_TRIG_MANUAL,

	/** Freerun mode conversion */
	ADC_TRIG_FREERUN,

	/**
	 * \brief Event-triggered conversions on individual channels
	 * Pairs each event channel with an ADC channel.
	 * \note The maximum base event channel that can be used is determined
	 * by the number of channels to trigger conversions on.
	 */
	ADC_TRIG_EVENT_SINGLE,

#if ADC_NR_OF_CHANNELS > 1
	/**
	 * \brief Freerunning conversion sweeps
	 * \note These will start as soon as the ADC is enabled.
	 */
	ADC_TRIG_FREERUN_SWEEP,

	/**
	 * \brief Event-triggered conversion sweeps
	 * \note Only the base event channel is used in this mode.
	 */
	ADC_TRIG_EVENT_SWEEP,
#endif

	/**
	 * \brief Event-triggered, synchronized conversion sweeps
	 * \note Only the base event channel is used in this mode.
	 */
	ADC_TRIG_EVENT_SYNCSWEEP,
};

/** \brief ADC signedness settings */
enum adc_sign {
	ADC_SIGN_OFF,                    /**< Unsigned conversions. */
	ADC_SIGN_ON = ADC_CONMODE_bm,    /**< Signed conversions. */
};

/** \brief ADC resolution settings */
enum adc_resolution {
	/** 8-bit resolution, right-adjusted. */
	ADC_RES_8       = ADC_RESOLUTION_8BIT_gc,
	/** 12-bit resolution, right-adjusted. */
	ADC_RES_12      = ADC_RESOLUTION_12BIT_gc,
	/** 12-bit resolution, left-adjusted. */
	ADC_RES_12_LEFT = ADC_RESOLUTION_LEFT12BIT_gc,
#if XMEGA_E

	/** More than 12-bit resolution.
	 * Must be used when adcch_enable_averaging() or
	 * adcch_enable_oversampling() is used.
	 */
	ADC_RES_MT12 = ADC_RESOLUTION_MT12BIT_gc,
#endif
};

/**
 * \brief ADC reference settings
 *
 * \note The choice in voltage reference varies between device families.
 * Refer to the device manual for detailed information.
 */
enum adc_reference {
	/** Internal 1 V from bandgap reference. */
	ADC_REF_BANDGAP  = ADC_REFSEL_INT1V_gc,
	/** VCC divided by 1.6. */
	ADC_REF_VCC      = ADC_REFSEL_INTVCC_gc,
	/** External reference on AREFA pin. */
	ADC_REF_AREFA    = ADC_REFSEL_AREFA_gc,
#if XMEGA_E
	/** External reference on AREFD pin. */
	ADC_REF_AREFD    = ADC_REFSEL_AREFD_gc,
#else
	/** External reference on AREFB pin. */
	ADC_REF_AREFB    = ADC_REFSEL_AREFB_gc,
#endif
	/** VCC divided by 2. */
	ADC_REF_VCCDIV2 = ADC_REFSEL_VCCDIV2_gc,
};

/** \name Internal functions for driver */
/** @{ */

/**
 * \internal
 * \brief Get ADC channel pointer from channel mask
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask Mask of ADC channel(s):
 * \arg \c ADC_CHn , where \c n specifies the channel. (Only a single channel
 * can be given in mask)
 *
 * \return Pointer to ADC channel
 */
__always_inline static  ADC_CH_t *adc_get_channel(
		ADC_t *adc, uint8_t ch_mask)
{
	uint8_t index = 0;

	Assert(ch_mask & ((1 << ADC_NR_OF_CHANNELS) - 1));

	/* Use a conditional inline ctz for optimization. */
#if ADC_NR_OF_CHANNELS > 4
	if (!(ch_mask & 0x0f)) {
		index += 4;
		ch_mask >>= 4;
	}
#endif
#if ADC_NR_OF_CHANNELS > 2
	if (!(ch_mask & 0x03)) {
		index += 2;
		ch_mask >>= 2;
	}
#endif
#if ADC_NR_OF_CHANNELS > 1
	if (!(ch_mask & 0x01)) {
		index++;
	}
#endif

	return (ADC_CH_t *)(&adc->CH0 + index);
}

/** @} */

#if defined(CONFIG_ADC_CALLBACK_ENABLE) || defined(__DOXYGEN__)
/** \name ADC interrupt callback function */
/** @{ */

/**
 * \def CONFIG_ADC_CALLBACK_ENABLE
 * \brief Configuration symbol to enable callback on ADC interrupts
 *
 * Define this symbol in \ref conf_adc.h to enable callbacks on ADC interrupts.
 * A function of type \ref adc_callback_t must be defined by the user, and the
 * driver be configured to use it with \ref adc_set_callback.
 */
#if !defined(CONFIG_ADC_CALLBACK_ENABLE) || defined(__DOXYGEN__)
#  define CONFIG_ADC_CALLBACK_ENABLE
#endif

/**
 * \def CONFIG_ADC_CALLBACK_TYPE
 * \brief Configuration symbol for datatype of result parameter for callback
 *
 * Define the datatype of the ADC conversion result parameter for callback
 * functions. This should be defined according to the signedness and resolution
 * of the conversions:
 * - \c int16_t for signed, 12-bit
 * - \c uint16_t for unsigned, 12-bit (the default type)
 * - \c int8_t for signed, 8-bit
 * - \c uint8_t for unsigned, 8-bit
 *
 * Define this in \ref conf_adc.h if the default datatype is not desired.
 */
#if !defined(CONFIG_ADC_CALLBACK_TYPE) || defined(__DOXYGEN__)
#  define CONFIG_ADC_CALLBACK_TYPE    uint16_t
#endif

/** Datatype of ADC conversion result parameter for callback */
typedef CONFIG_ADC_CALLBACK_TYPE adc_result_t;

/**
 * \brief ADC interrupt callback function pointer
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask Mask of ADC channel(s):
 * \arg \c ADC_CHn , where \c n specifies the channel. (Only a single channel
 * can be given in mask)
 * \param res ADC conversion result.
 */
typedef void (*adc_callback_t)(ADC_t *adc, uint8_t ch_mask, adc_result_t res);

void adc_set_callback(ADC_t *adc, adc_callback_t callback);

/** @} */
#endif

/** \name ADC module management */
/** @{ */

void adc_enable(ADC_t *adc);
void adc_disable(ADC_t *adc);
bool adc_is_enabled(ADC_t *adc);

/**
 * \brief Start one-shot conversion on ADC channel(s)
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask Mask of ADC channel(s):
 * \arg \c ADC_CHn , where \c n specifies the channel. (These can be OR'ed
 * together.)
 *
 * \note The ADC must be enabled for this function to have any effect.
 */
static inline void adc_start_conversion(ADC_t *adc, uint8_t ch_mask)
{
	irqflags_t flags = cpu_irq_save();
#if !XMEGA_E
	adc->CTRLA |= ch_mask << ADC_CH0START_bp;
#else
	adc->CTRLA |= ch_mask << ADC_START_bp;
#endif
	cpu_irq_restore(flags);
}

/**
 * \brief Get result from ADC channel
 *
 * Gets the latest conversion result from the ADC channel.
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask Mask of ADC channel(s):
 * \arg \c ADC_CHn , where \c n specifies the channel. (Only a single channel
 * can be given in mask)
 *
 * \return Latest conversion result of ADC channel. Signedness does not matter.
 *
 * \note This macro does not protect the 16-bit read from interrupts. If an
 * interrupt may do a 16-bit read or write to the ADC while this macro is
 * executing, interrupts \a must be temporarily disabled to avoid corruption of
 * the read.
 */
#define adc_get_result(adc, ch_mask)    (adc_get_channel(adc, ch_mask)->RES)

/**
 * \brief Get signed result from ADC channel
 *
 * Returns the latest conversion result from the ADC channel as a signed type,
 * with interrupt protection of the 16-bit read.
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask Mask of ADC channel(s):
 * \arg \c ADC_CHn , where \c n specifies the channel. (Only a single channel
 * can be given in mask)
 *
 * \return Latest conversion result of ADC channel, as signed 16-bit integer.
 */
static inline int16_t adc_get_signed_result(ADC_t *adc, uint8_t ch_mask)
{
	int16_t val;
	irqflags_t flags;
	ADC_CH_t *adc_ch;

	adc_ch = adc_get_channel(adc, ch_mask);

	flags = cpu_irq_save();
	val = adc_ch->RES;
	cpu_irq_restore(flags);

	return val;
}

/**
 * \brief Get unsigned result from ADC channel
 *
 * Returns the latest conversion result from the ADC channel as an unsigned
 * type, with interrupt protection of the 16-bit read.
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask Mask of ADC channel(s):
 * \arg \c ADC_CHn , where \c n specifies the channel. (Only a single channel
 * can be given in mask)
 *
 * \return Latest conversion result of ADC channel, as unsigned 16-bit integer.
 */
static inline uint16_t adc_get_unsigned_result(ADC_t *adc, uint8_t ch_mask)
{
	uint16_t val;
	irqflags_t flags;
	ADC_CH_t *adc_ch;

	adc_ch = adc_get_channel(adc, ch_mask);

	flags = cpu_irq_save();
	val = adc_ch->RES;
	cpu_irq_restore(flags);

	return val;
}

/**
 * \brief Get interrupt flag of ADC channel(s)
 *
 * Returns the interrupt flag of the masked channels. The meaning of the
 * interrupt flag depends on what mode the individual channels are in.
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask Mask of ADC channel(s):
 * \arg \c ADC_CHn , where \c n specifies the channel. (These can be OR'ed
 * together.)
 *
 * \return Mask with interrupt flags.
 */
static inline uint8_t adc_get_interrupt_flag(ADC_t *adc, uint8_t ch_mask)
{
	return (adc->INTFLAGS >> ADC_CH0IF_bp) & ch_mask;
}

/**
 * \brief Clear interrupt flag of ADC channel(s)
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask Mask of ADC channel(s):
 * \arg \c ADC_CHn , where \c n specifies the channel. (These can be OR'ed
 * together.)
 *
 * \note The ADC must be enabled for this function to have any effect.
 */
static inline void adc_clear_interrupt_flag(ADC_t *adc, uint8_t ch_mask)
{
	adc->INTFLAGS = ch_mask << ADC_CH0IF_bp;
}

/**
 * \brief Wait for interrupt flag of ADC channel(s)
 *
 * Waits for the interrupt flag of the specified channel(s) to be set, then
 * clears it before returning. If several channels are masked, the function will
 * wait for \a all interrupt flags to be set.
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask Mask of ADC channel(s):
 * \arg \c ADC_CHn , where \c n specifies the channel. (These can be OR'ed
 * together.)
 */
static inline void adc_wait_for_interrupt_flag(ADC_t *adc, uint8_t ch_mask)
{
	do { } while (adc_get_interrupt_flag(adc, ch_mask) != ch_mask);
	adc_clear_interrupt_flag(adc, ch_mask);
}

/**
 * \brief Flush the ADC
 *
 * Forces the ADC to abort any ongoing conversions and restart its clock on the
 * next peripheral clock cycle. Pending conversions are started after the
 * clock reset.
 *
 * \param adc Pointer to ADC module.
 *
 * \note The ADC must be enabled for this function to have any effect.
 */
static inline void adc_flush(ADC_t *adc)
{
	irqflags_t flags = cpu_irq_save();
	adc->CTRLA |= ADC_FLUSH_bm;
	cpu_irq_restore(flags);
}

/**
 * \brief Set compare value directly to ADC
 *
 * Sets the compare value directly to the ADC, for quick access while the ADC is
 * enabled.
 *
 * \param adc Pointer to ADC module.
 * \param val Compare value to set, either signed or unsigned.
 *
 * \note The ADC must be enabled for this function to have any effect.
 */
#define adc_set_compare_value(adc, val)	\
	do { \
		irqflags_t ATPASTE2(adc_flags, __LINE__) =  cpu_irq_save(); \
		(adc)->CMP = val; \
		cpu_irq_restore(ATPASTE2(adc_flags, __LINE__));	\
	} \
	while (0)

/**
 * \brief Get compare value directly from ADC
 *
 * Gets the compare value directly from the ADC, for quick access while the ADC
 * is enabled.
 *
 * \param adc Pointer to ADC module.
 *
 * \return Current compare value of the ADC. Signedness does not matter.
 *
 * \note This macro does not protect the 16-bit read from interrupts. If an
 * interrupt may do a 16-bit read or write to the ADC while this macro is
 * executing, interrupts \a must be temporarily disabled to avoid corruption of
 * the read.
 */
#define adc_get_compare_value(adc)    ((adc)->CMP)

/**
 * \brief Get signed compare value directly from ADC
 *
 * Gets the signed compare value directly from the ADC, with interrupt
 * protection of the 16-bit read, for quick access while the ADC is enabled.
 *
 * \param adc Pointer to ADC module.
 */
static inline int16_t adc_get_signed_compare_value(ADC_t *adc)
{
	int16_t val;
	irqflags_t flags;

	flags = cpu_irq_save();
	val = adc->CMP;
	cpu_irq_restore(flags);

	return val;
}

/**
 * \brief Get unsigned compare value directly from ADC
 *
 * Gets the unsigned compare value directly from the ADC, with interrupt
 * protection of the 16-bit read, for quick access while the ADC is enabled.
 *
 * \param adc Pointer to ADC module.
 */
static inline uint16_t adc_get_unsigned_compare_value(ADC_t *adc)
{
	uint16_t val;
	irqflags_t flags;

	flags = cpu_irq_save();
	val = adc->CMP;
	cpu_irq_restore(flags);

	return val;
}

#if XMEGA_E

/**
 * \brief Set sample time value directly to ADC
 *
 * Sets the sample time value directly to the ADC, for quick access while the
 * ADC is enabled.
 *
 * \param adc Pointer to ADC module.
 * \param val Sample time value to set.
 *
 * \note The ADC must be enabled for this function to have any effect.
 */
static inline void adc_set_sample_value(ADC_t *adc, uint8_t val)
{
	irqflags_t flags;

	flags = cpu_irq_save();
	adc->SAMPCTRL = (uint8_t)val;
	cpu_irq_restore(flags);
}

/**
 * \brief Get sample time value directly from ADC
 *
 * Gets the sample time value directly from the ADC, for quick access while the
 * ADC is enabled.
 *
 * \param adc Pointer to ADC module.
 *
 * \return Current sample time value of the ADC.
 *
 * \note This macro does not protect the 8-bit read from interrupts. If an
 * interrupt may do a 8-bit read or write to the ADC while this macro is
 * executing, interrupts \a must be temporarily disabled to avoid corruption of
 * the read.
 */
static inline uint8_t adc_get_sample_value(ADC_t *adc)
{
	return adc->SAMPCTRL;
}

#endif

/**
 * \brief Get calibration data
 *
 * \param cal Identifier for calibration data to get.
 */
static inline uint16_t adc_get_calibration_data(enum adc_calibration_data cal)
{
	uint16_t data;

	switch (cal) {
#ifdef ADCA
	case ADC_CAL_ADCA:
		data = nvm_read_production_signature_row(ADCACAL1);
		data <<= 8;
		data |= nvm_read_production_signature_row(ADCACAL0);
		break;
#endif

#ifdef ADCB
	case ADC_CAL_ADCB:
		data = nvm_read_production_signature_row(ADCBCAL1);
		data <<= 8;
		data |= nvm_read_production_signature_row(ADCBCAL0);
		break;
#endif

#if defined(ADCA) || defined(ADCB)
	case ADC_CAL_TEMPSENSE:
		data = nvm_read_production_signature_row(TEMPSENSE1);
		data <<= 8;
		data |= nvm_read_production_signature_row(TEMPSENSE0);
		break;
#endif

#if XMEGA_E
	case ADC_CAL_HOTTEMP:
		data = nvm_read_production_signature_row(HOTTEMP);
		break;
	
	case ADC_CAL_ROOMTEMP:
		data = nvm_read_production_signature_row(ROOMTEMP);
		break;
	
	case ADC_CAL_TEMPSENSE2:
		data = nvm_read_production_signature_row(TEMPSENSE3);
		data <<= 8;
		data |= nvm_read_production_signature_row(TEMPSENSE2);
		break;
#endif

	default:
		Assert(0);
		data = 0;
	}

	return data;
}

/** @} */

/** \name ADC module configuration */
/** @{ */

void adc_write_configuration(ADC_t *adc, const struct adc_config *conf);
void adc_read_configuration(ADC_t *adc, struct adc_config *conf);

/**
 * \brief Set ADC prescaler to get desired clock rate
 *
 * Sets the ADC prescaling so that its clock rate becomes _at most_
 * \a clk_adc_hz. This is done by computing the ratio of the peripheral clock
 * rate to the desired ADC clock rate, and rounding it upward to the nearest
 * prescaling factor.
 *
 * \param conf Pointer to ADC module configuration.
 * \param clk_adc Desired ADC clock rate.
 *
 * \note The sample rate is not determined solely by the ADC clock rate for all
 * devices. Setting the current limit mode on some devices will also affect the
 * maximum ADC sampling rate. Refer to the device manual for detailed
 * information on conversion timing and/or the current limitation mode.
 */
static inline void adc_set_clock_rate(struct adc_config *conf, uint32_t clk_adc)
{
	uint32_t clk_per;
	uint16_t ratio;
	uint8_t psc;

	Assert(clk_adc);
#if XMEGA_A || XMEGA_AU
	Assert(clk_adc <= 2000000UL);
#elif XMEGA_D
	Assert(clk_adc <= 1400000UL);
#elif XMEGA_B || XMEGA_C || XMEGA_E
	Assert(clk_adc <= 1800000UL);
#endif

	clk_per = sysclk_get_per_hz();
	ratio = clk_per / clk_adc;

	/* Round ratio up to the nearest prescaling factor. */
	if (ratio <= 4) {
		psc = ADC_PRESCALER_DIV4_gc;
	} else if (ratio <= 8) {
		psc = ADC_PRESCALER_DIV8_gc;
	} else if (ratio <= 16) {
		psc = ADC_PRESCALER_DIV16_gc;
	} else if (ratio <= 32) {
		psc = ADC_PRESCALER_DIV32_gc;
	} else if (ratio <= 64) {
		psc = ADC_PRESCALER_DIV64_gc;
	} else if (ratio <= 128) {
		psc = ADC_PRESCALER_DIV128_gc;
	} else if (ratio <= 256) {
		psc = ADC_PRESCALER_DIV256_gc;
	} else {
		psc = ADC_PRESCALER_DIV512_gc;
	}

	conf->prescaler = psc;
}

/**
 * \brief Set ADC conversion parameters
 *
 * Sets the signedness, resolution and voltage reference for conversions in the
 * ADC module configuration.
 *
 * \param conf Pointer to ADC module configuration.
 * \param sign Conversion signedness.
 * \param res Resolution of conversions.
 * \param ref Voltage reference to use.
 */
static inline void adc_set_conversion_parameters(struct adc_config *conf,
		enum adc_sign sign, enum adc_resolution res,
		enum adc_reference ref)
{
	/* Preserve all but conversion and resolution config. */
	conf->ctrlb &= ~(ADC_CONMODE_bm | ADC_RESOLUTION_gm);
	conf->ctrlb |= (uint8_t)res | (uint8_t)sign;

	conf->refctrl &= ~ADC_REFSEL_gm;
	conf->refctrl |= ref;
}

/**
 * \brief Set ADC conversion trigger
 *
 * Configures the conversion triggering of the ADC.
 *
 * For automatic triggering modes, the number of channels to start conversions
 * on must be specified with \a nr_of_ch. The channel selection for these
 * modes is incrementally inclusive, always starting with channel 0.
 *
 * For event triggered modes, the base event channel must also be specified with
 * \a base_ev_ch. The event channels are assigned to the ADC channels in an
 * incremental fashion \a without \a wrap-around (in single-trigger event mode).
 * This means that the maximum base event channel that can be used is determined
 * by the number of ADC channels to start conversions on, i.e., \a nr_of_ch.
 *
 * \param conf Pointer to ADC module configuration.
 * \param trig Conversion trigger to set.
 * \param nr_of_ch Number of ADC channels to trigger conversions on:
 * \arg \c 1 - \c ADC_NR_OF_CHANNELS (must be non-zero).
 * \param base_ev_ch Base event channel, if used.
 */
static inline void adc_set_conversion_trigger(struct adc_config *conf,
		enum adc_trigger trig, uint8_t nr_of_ch, uint8_t base_ev_ch)
{
	Assert(nr_of_ch);
	Assert(nr_of_ch <= ADC_NR_OF_CHANNELS);
#if XMEGA_A || XMEGA_AU || XMEGA_E
	Assert(base_ev_ch <= 7);
#elif XMEGA_B || XMEGA_C || XMEGA_D
	Assert(base_ev_ch <= 3);
#endif

	switch (trig) {
	case ADC_TRIG_MANUAL:
		conf->ctrlb &= ~ADC_FREERUN_bm;
		conf->evctrl = ADC_EVACT_NONE_gc;
		break;

	case ADC_TRIG_EVENT_SINGLE:
		conf->ctrlb &= ~ADC_FREERUN_bm;
		conf->evctrl = (base_ev_ch << ADC_EVSEL_gp) |
				(nr_of_ch << ADC_EVACT_gp);
		break;

	case ADC_TRIG_FREERUN:
		conf->ctrlb |= ADC_FREERUN_bm;
		break;

#if ADC_NR_OF_CHANNELS > 1
	case ADC_TRIG_FREERUN_SWEEP:
		conf->ctrlb |= ADC_FREERUN_bm;
		conf->evctrl = (nr_of_ch - 1) << ADC_SWEEP_gp;
		break;

	case ADC_TRIG_EVENT_SWEEP:
		conf->ctrlb &= ~ADC_FREERUN_bm;
		conf->evctrl = (nr_of_ch - 1) << ADC_SWEEP_gp |
				(base_ev_ch << ADC_EVSEL_gp) |
				ADC_EVACT_SWEEP_gc;
		break;
#endif

	case ADC_TRIG_EVENT_SYNCSWEEP:
		conf->ctrlb &= ~ADC_FREERUN_bm;
		conf->evctrl =
#if ADC_NR_OF_CHANNELS > 1
				((nr_of_ch - 1) << ADC_SWEEP_gp) |
#endif
				(base_ev_ch << ADC_EVSEL_gp) |
				ADC_EVACT_SYNCSWEEP_gc;
		break;

	default:
		Assert(0);
	}
}

#if ADC_NR_OF_CHANNELS > 1

/**
 * \brief Set DMA request group
 *
 * Configures the DMA group request for the specified number of ADC channels.
 * The channel group selection is incrementally inclusive, always starting with
 * channel 0.
 *
 * \param conf Pointer to ADC module configuration.
 * \param nr_of_ch Number of channels for group request:
 * \arg 0 to disable.
 * \arg 2, 3 or 4 to enable.
 *
 * \note The number of channels in the DMA request group cannot be 1.
 * \note Not all device families feature this setting.
 */
static inline void adc_set_dma_request_group(struct adc_config *conf,
		uint8_t nr_of_ch)
{
	Assert(nr_of_ch <= ADC_NR_OF_CHANNELS);
	Assert(nr_of_ch != 1);

	if (nr_of_ch) {
		conf->ctrla = (nr_of_ch - 1) << ADC_DMASEL_gp;
	} else {
		conf->ctrla = ADC_DMASEL_OFF_gc;
	}
}

#endif

/**
 * \brief Enable internal ADC input
 *
 * \param conf Pointer to ADC module configuration.
 * \param int_inp Internal input to enable:
 * \arg \c ADC_INT_TEMPSENSE for temperature sensor.
 * \arg \c ADC_INT_BANDGAP for bandgap reference.
 */
static inline void adc_enable_internal_input(struct adc_config *conf,
		uint8_t int_inp)
{
	conf->refctrl |= int_inp;
}

/**
 * \brief Disable internal ADC input
 *
 * \param conf Pointer to ADC module configuration.
 * \param int_inp Internal input to disable:
 * \arg \c ADC_INT_TEMPSENSE for temperature sensor.
 * \arg \c ADC_INT_BANDGAP for bandgap reference.
 */
static inline void adc_disable_internal_input(struct adc_config *conf,
		uint8_t int_inp)
{
	conf->refctrl &= ~int_inp;
}

#if XMEGA_AU || defined(__DOXYGEN__)
/** \brief ADC gain stage impedance settings */
enum adc_gainstage_impmode {
	/** High impedance sources */
	ADC_GAIN_HIGHIMPEDANCE,
	/** Low impedance sources */
	ADC_GAIN_LOWIMPEDANCE,
};

/**
 * \brief Set ADC gain stage impedance mode
 *
 * \param conf Pointer to ADC module configuration.
 * \param impmode Gain stage impedance mode.
 *
 * \note Not all device families feature this setting.
 */
static inline void adc_set_gain_impedance_mode(struct adc_config *conf,
		enum adc_gainstage_impmode impmode)
{
	switch (impmode) {
	case ADC_GAIN_HIGHIMPEDANCE:
		conf->ctrlb &= ~ADC_IMPMODE_bm;
		break;

	case ADC_GAIN_LOWIMPEDANCE:
		conf->ctrlb |= ADC_IMPMODE_bm;
		break;

	default:
		Assert(0);
	}
}

#endif

#if !XMEGA_A
/** \brief ADC current limit settings */
enum adc_current_limit {
	/** No current limit */
	ADC_CURRENT_LIMIT_NO,
	/** Low current limit, max sampling rate 1.5 MSPS */
	ADC_CURRENT_LIMIT_LOW,
	/** Medium current limit, max sampling rate 1 MSPS */
	ADC_CURRENT_LIMIT_MED,
	/** High current limit, max sampling rate 0.5 MSPS */
	ADC_CURRENT_LIMIT_HIGH
};

/**
 * \brief Set ADC current limit
 *
 * Set the current limit mode for the ADC module. This setting affects the max
 * sampling rate of the ADC.
 *
 * \note See the device datasheet and manual for detailed information about
 * current consumption and sample rate limit.
 *
 * \param conf Pointer to ADC module configuration.
 * \param currlimit Current limit setting.
 *
 * \note Not all device families feature this setting.
 */
static inline void adc_set_current_limit(struct adc_config *conf,
		enum adc_current_limit currlimit)
{
	conf->ctrlb &= ~ADC_CURRLIMIT_gm;

	switch (currlimit) {
	case ADC_CURRENT_LIMIT_NO:
		conf->ctrlb |= ADC_CURRLIMIT_NO_gc;
		break;

	case ADC_CURRENT_LIMIT_LOW:
		conf->ctrlb |= ADC_CURRLIMIT_LOW_gc;
		break;

	case ADC_CURRENT_LIMIT_MED:
		conf->ctrlb |= ADC_CURRLIMIT_MED_gc;
		break;

	case ADC_CURRENT_LIMIT_HIGH:
		conf->ctrlb |= ADC_CURRLIMIT_HIGH_gc;
		break;

	default:
		Assert(0);
	}
}

#endif

/**
 * \brief Set ADC compare value in configuration
 *
 * \param conf Pointer to ADC module configuration.
 * \param val Compare value to set.
 */
#define adc_set_config_compare_value(conf, val)	\
	do { \
		conf->cmp = (uint16_t)val; \
	} \
	while (0)

/**
 * \brief Get ADC compare value from configuration
 *
 * \param conf Pointer to ADC module configuration.
 */
#define adc_get_config_compare_value(conf)    (conf->cmp)

#if XMEGA_E

/**
 * \brief Set ADC sample time value in configuration
 *
 * \param conf Pointer to ADC module configuration.
 * \param val Sample time value to set.
 */
#define adc_set_config_sample_value(conf, val) \
	do { \
		conf->sampctrl = (uint8_t)val; \
	} \
	while (0)

/**
 * \brief Get ADC sample time value from configuration
 *
 * \param conf Pointer to ADC module configuration.
 */
#define adc_get_config_sample_value(conf)    (conf->sampctrl)
#endif

/** @} */

/** @} */

/**
 * \defgroup adc_channel_group ADC channel
 *
 * Management and configuration functions for the individual ADC channels.
 *
 * The API functions and definitions can be divided in two groups:
 * - channel management: direct access for getting conversion result.
 * - channel configuration: create/change configurations and write/read them
 * to/from ADC channels.
 *
 * @{
 */

/**
 * \brief Default ADC channel interrupt level
 *
 * \note To override the channel interrupt level, define this symbol as the
 * desired level in \ref conf_adc.h.
 */
#if !defined(CONFIG_ADC_INTLVL) || defined(__DOXYGEN__)
#  define CONFIG_ADC_INTLVL    ADC_CH_INTLVL_LO_gc
#endif

/** ADC channel configuration */
struct adc_channel_config {
	uint8_t ctrl;
	uint8_t muxctrl;
	uint8_t intctrl;
	uint8_t scan;
#if XMEGA_E
	uint8_t corrctrl;
	uint8_t offsetcorr0;
	uint8_t offsetcorr1;
	uint8_t gaincorr0;
	uint8_t gaincorr1;
	uint8_t avgctrl;
#endif
};

/**
 * \brief ADC channel positive input
 *
 * Identifies the external and internal signals that can be used as positive
 * input to the ADC channels.
 */
enum adcch_positive_input {
	ADCCH_POS_PIN0,
	ADCCH_POS_PIN1,
	ADCCH_POS_PIN2,
	ADCCH_POS_PIN3,
	ADCCH_POS_PIN4,
	ADCCH_POS_PIN5,
	ADCCH_POS_PIN6,
	ADCCH_POS_PIN7,
	ADCCH_POS_PIN8,
	ADCCH_POS_PIN9,
	ADCCH_POS_PIN10,
	ADCCH_POS_PIN11,
	ADCCH_POS_PIN12,
	ADCCH_POS_PIN13,
	ADCCH_POS_PIN14,
	ADCCH_POS_PIN15,

	/** \name Internal inputs. */
	/** @{ */
	ADCCH_POS_TEMPSENSE,     /**< Temperature sensor. */
	ADCCH_POS_BANDGAP,       /**< Bandgap reference. */
	ADCCH_POS_SCALED_VCC,    /**< VCC scaled down by 10. */
#if XMEGA_A || XMEGA_AU || XMEGA_E || defined(__DOXYGEN__)
	ADCCH_POS_DAC,           /**< DAC output. */
#endif
	/** @} */
};

/**
 * \brief ADC channel negative input
 *
 * Identifies the signals that can be used as negative input to the ADC channels
 * in differential mode. Some of the input signals are only available with
 * certain gain settings, e.g., 1x gain.
 *
 * \note The ADC must be set in signed mode to use differential measurements.
 * For single-ended measurements, ADDCH_NEG_NONE should be specified as negative
 * input.
 *
 * \note Pad and internal GND are not available on all devices. See the device
 * manual for an overview of available input signals.
 */
enum adcch_negative_input {
	/** \name Input pins for differential measurements with 1x gain. */
	/** @{ */
	/** ADC0 pin */
	ADCCH_NEG_PIN0,
	/** ADC1 pin */
	ADCCH_NEG_PIN1,
	/** ADC2 pin */
	ADCCH_NEG_PIN2,
	/** ADC3 pin */
	ADCCH_NEG_PIN3,
	/** @} */

	/** \name Input pins for differential measurements with any gain. */
	/** @{ */
	/** ADC4 pin */
	ADCCH_NEG_PIN4,
	/** ADC5 pin */
	ADCCH_NEG_PIN5,
	/** ADC6 pin */
	ADCCH_NEG_PIN6,
	/** ADC7 pin */
	ADCCH_NEG_PIN7,
	/** @} */

	/** \name GND signals for differential measurements. */
	/** @{ */
	/** PAD ground */
	ADCCH_NEG_PAD_GND,
	/** Internal ground */
	ADCCH_NEG_INTERNAL_GND,
	/** @} */

	/** Single ended mode */
	ADCCH_NEG_NONE,
};

/** \brief ADC channel interrupt modes */
enum adcch_mode {
	/** Set interrupt flag when conversions complete. */
	ADCCH_MODE_COMPLETE = ADC_CH_INTMODE_COMPLETE_gc,
	/** Set interrupt flag when conversion result is below compare value. */
	ADCCH_MODE_BELOW    = ADC_CH_INTMODE_BELOW_gc,
	/** Set interrupt flag when conversion result is above compare value. */
	ADCCH_MODE_ABOVE    = ADC_CH_INTMODE_ABOVE_gc,
};

/** \name ADC channel configuration */
/** @{ */

void adcch_write_configuration(ADC_t *adc, uint8_t ch_mask,
		const struct adc_channel_config *ch_conf);
void adcch_read_configuration(ADC_t *adc, uint8_t ch_mask,
		struct adc_channel_config *ch_conf);

/** Force enabling of gainstage with unity gain. */
#define ADCCH_FORCE_1X_GAINSTAGE 0xff

/**
 * \internal
 * \brief Get ADC channel setting for specified gain
 *
 * Returns the setting that corresponds to specified gain.
 *
 * \param gain Valid gain factor for the measurement.
 *
 * \return Gain setting of type ADC_CH_GAIN_t.
 */
static inline uint8_t adcch_get_gain_setting(uint8_t gain)
{
	switch (gain) {
	case 0:
		return ADC_CH_GAIN_DIV2_gc;

	case 1:
		return ADC_CH_GAIN_1X_gc;

	case 2:
		return ADC_CH_GAIN_2X_gc;

	case 4:
		return ADC_CH_GAIN_4X_gc;

	case 8:
		return ADC_CH_GAIN_8X_gc;

	case 16:
		return ADC_CH_GAIN_16X_gc;

	case 32:
		return ADC_CH_GAIN_32X_gc;

	case 64:
		return ADC_CH_GAIN_64X_gc;

	case ADCCH_FORCE_1X_GAINSTAGE:
		return ADC_CH_GAIN_1X_gc;

	default:
		Assert(0);
		return 0;
	}
}

/**
 * \brief Set ADC channel input mode, multiplexing and gain
 *
 * Sets up an ADC channel's input mode and multiplexing according to specified
 * input signals, as well as the gain.
 *
 * \param ch_conf Pointer to ADC channel configuration.
 * \param pos Positive input signal.
 * \param neg Negative input signal:
 * \arg \c ADCCH_NEG_NONE for single-ended measurements.
 * \arg \c ADCCH_NEG_PINn , where \c n specifies a pin, for differential
 * measurements.
 * \arg \c ADDCH_x_GND , where \c x specified pad or internal GND, for
 * differential measurements.
 * \param gain Gain factor for measurements:
 * \arg 1 for single-ended or differential with pin 0, 1, 2 or 3, pad or
 * internal GND as negative
 * input.
 * \arg 0 (0.5x), 1, 2, 4, 8, 16, 32 or 64 for differential with pin 4, 5, 6 or
 * 7, pad or internal GND as negative input.
 * \arg ADCCH_FORCE_1X_GAINSTAGE to force the gain stage to be enabled with
 * unity gain for differential measurement.
 *
 * \note The GND signals are not available on all devices. Refer to the device
 * manual for information on available input signals.
 *
 * \note With unity (1x) gain, some input selections may be possible both with
 * and without the gain stage enabled. The driver will default to the
 * configuration without gainstage to keep the current consumption as low as
 * possible unless the user specifies \ref ADCCH_FORCE_1X_GAINSTAGE as \a gain.
 */
static inline void adcch_set_input(struct adc_channel_config *ch_conf,
		enum adcch_positive_input pos, enum adcch_negative_input neg,
		uint8_t gain)
{
	if (pos >= ADCCH_POS_TEMPSENSE) {
		/* Configure for internal input. */
		Assert(gain == 1);
		Assert(neg == ADCCH_NEG_NONE);

		ch_conf->ctrl = ADC_CH_INPUTMODE_INTERNAL_gc;
		ch_conf->muxctrl = (pos - ADCCH_POS_TEMPSENSE) <<
				ADC_CH_MUXPOS_gp;
	} else if (neg == ADCCH_NEG_NONE) {
		/* Configure for single-ended measurement. */
		Assert(gain == 1);

		ch_conf->ctrl = ADC_CH_INPUTMODE_SINGLEENDED_gc;
		ch_conf->muxctrl = pos << ADC_CH_MUXPOS_gp;
	} else if (neg <= ADCCH_NEG_PIN3) {
		/* Configure for differential measurement.
		 * Pins 0-3 can only be used for negative input if the gain
		 * stage is not used, i.e., unity gain (except XMEGA E).
		 */
#if XMEGA_E
		ch_conf->ctrl = adcch_get_gain_setting(gain) |
				ADC_CH_INPUTMODE_DIFFWGAINL_gc;
#else
		Assert(gain == 1);
		ch_conf->ctrl = ADC_CH_INPUTMODE_DIFF_gc;
#endif
		ch_conf->muxctrl = (pos << ADC_CH_MUXPOS_gp) |
				(neg << ADC_CH_MUXNEG_gp);
	} else if (neg <= ADCCH_NEG_PIN7) {
		/* Configure for differential measurement.
		 * Pins 4-7 can be used for all gain settings,
		 * including unity gain, which is available even if
		 * the gain stage is active.
		 */
#if XMEGA_E
		ch_conf->ctrl = adcch_get_gain_setting(gain) |
				ADC_CH_INPUTMODE_DIFFWGAINH_gc;
#else
		ch_conf->ctrl = adcch_get_gain_setting(gain) |
				ADC_CH_INPUTMODE_DIFFWGAIN_gc;
#endif
		ch_conf->muxctrl = (pos << ADC_CH_MUXPOS_gp) |
				((neg - ADCCH_NEG_PIN4) <<
				ADC_CH_MUXNEG_gp);
	} else {
		Assert((neg == ADCCH_NEG_PAD_GND) ||
				(neg == ADCCH_NEG_INTERNAL_GND));
#if XMEGA_E

		/* Configure for differential measurement through PAD GND or
		 * internal GND.
		 * DIFFWGAINH (INPUTMODE) is not used because it support
		 * only PAD GND.
		 */
		ch_conf->ctrl = ADC_CH_INPUTMODE_DIFFWGAINL_gc |
				adcch_get_gain_setting(gain);
		ch_conf->muxctrl = (pos << ADC_CH_MUXPOS_gp) |
				((neg == ADCCH_NEG_INTERNAL_GND) ?
				ADC_CH_MUXNEGL_INTGND_gc
				: ADC_CH_MUXNEGL_GND_gc);
#else

		/* Configure for differential measurement through GND or
		 * internal GND.
		 * The bitmasks for the on-chip GND signals change when
		 * gain is enabled. To avoid unnecessary current consumption,
		 * do not enable gainstage for unity gain unless user explicitly
		 * specifies it with the ADCCH_FORCE_1X_GAINSTAGE macro.
		 */
		if (gain == 1) {
			ch_conf->ctrl = ADC_CH_INPUTMODE_DIFF_gc;
			ch_conf->muxctrl = (pos << ADC_CH_MUXPOS_gp) |
					((neg == ADCCH_NEG_PAD_GND) ?
					ADC_CH_MUXNEG_MODE10_GND_gc
					: ADC_CH_MUXNEG_MODE10_INTGND_gc);
		} else {
			ch_conf->ctrl = ADC_CH_INPUTMODE_DIFFWGAIN_gc |
					adcch_get_gain_setting(gain);
			ch_conf->muxctrl = (pos << ADC_CH_MUXPOS_gp) |
					((neg == ADCCH_NEG_INTERNAL_GND) ?
					ADC_CH_MUXNEG_MODE11_INTGND_gc
					: ADC_CH_MUXNEG_MODE11_GND_gc);
		}

#endif
	}
}

/**
 * \brief Set ADC channel 0 pin scan
 *
 * Sets the parameters for pin scan, which enables measurements on multiple,
 * successive input pins without any reconfiguration between conversions.
 *
 * Pin scan works by adding a offset to the positive MUX setting to get the
 * current input pin. The offset is incremented for each conversion, and is
 * reset to 0 once a conversion with the maximum offset is done.
 *
 * \param ch_conf Pointer to the ADC channel configuration structure
 * \param start_offset Initial offset to start pin scan at
 * \arg \c 0 - \c max_offset
 * \param max_offset Maximum offset for the pin scan
 * \arg \c 0 to disable
 * \arg \c 1 - \c 15 to enable
 *
 * \note Only the AVR XMEGA AU family features this setting.
 * \note Pin scan is only available on ADC channel 0.
 */
static inline void adcch_set_pin_scan(struct adc_channel_config *ch_conf,
		uint8_t start_offset, uint8_t max_offset)
{
	Assert(start_offset < 16);
	Assert(max_offset < 16);
	Assert(start_offset <= max_offset);

	ch_conf->scan = max_offset | (start_offset << ADC_CH_OFFSET_gp);
}

/**
 * \brief Set ADC channel interrupt mode
 *
 * \param ch_conf Pointer to ADC channel configuration.
 * \param mode Interrupt mode to set.
 */
static inline void adcch_set_interrupt_mode(struct adc_channel_config *ch_conf,
		enum adcch_mode mode)
{
	ch_conf->intctrl &= ~ADC_CH_INTMODE_gm;
	ch_conf->intctrl |= mode;
}

/**
 * \brief Enable interrupts on ADC channel
 *
 * \param ch_conf Pointer to ADC channel configuration.
 */
static inline void adcch_enable_interrupt(struct adc_channel_config *ch_conf)
{
	ch_conf->intctrl &= ~ADC_CH_INTLVL_gm;
	ch_conf->intctrl |= CONFIG_ADC_INTLVL;
}

/**
 * \brief Disable interrupts on ADC channel
 *
 * \param ch_conf Pointer to ADC channel configuration.
 */
static inline void adcch_disable_interrupt(struct adc_channel_config *ch_conf)
{
	ch_conf->intctrl &= ~ADC_CH_INTLVL_gm;
	ch_conf->intctrl |= ADC_CH_INTLVL_OFF_gc;
}

#if XMEGA_E

/**
 * \brief Enable gain & offset corrections on ADC channel
 *
 * \param ch_conf         Pointer to ADC channel configuration.
 * \param offset_corr     Offset correction value to set.
 * \param expected_value  Expected value for a specific input voltage
 * \param captured_value  Captured value for a specific input voltage
 *
 * \Note
 * Gived "expected_value = captured_value = 1" to ignore the gain correction
 * Gain correction is equal to "expected_value / captured_value"
 */
static inline void adcch_enable_correction(struct adc_channel_config *ch_conf,
		uint16_t offset_corr, uint16_t expected_value,
		uint16_t captured_value)
{
	uint32_t gain_corr;

	gain_corr = (2048L * expected_value) / captured_value;
	ch_conf->offsetcorr0 = LSB(offset_corr);
	ch_conf->offsetcorr1 = MSB(offset_corr);
	ch_conf->gaincorr0 = LSB(gain_corr);
	ch_conf->gaincorr1 = MSB(gain_corr);
	ch_conf->corrctrl = ADC_CH_CORREN_bm;
}

/**
 * \brief Disable gain & offset correction on ADC channel
 *
 * \param ch_conf Pointer to ADC channel configuration.
 */
static inline void adcch_disable_correction(struct adc_channel_config *ch_conf)
{
	ch_conf->corrctrl = ADC_CH_CORREN_bp;
}

/** \brief ADC channel sample number settings */
enum adcch_sampnum {
	/** 2 samples to accumulate. */
	ADC_SAMPNUM_2X = ADC_SAMPNUM_2X_gc,
	/** 4 samples to accumulate. */
	ADC_SAMPNUM_4X = ADC_SAMPNUM_4X_gc,
	/** 8 samples to accumulate. */
	ADC_SAMPNUM_8X = ADC_SAMPNUM_8X_gc,
	/** 16 samples to accumulate. */
	ADC_SAMPNUM_16X = ADC_SAMPNUM_16X_gc,
	/** 32 samples to accumulate. */
	ADC_SAMPNUM_32X = ADC_SAMPNUM_32X_gc,
	/** 64 samples to accumulate. */
	ADC_SAMPNUM_64X = ADC_SAMPNUM_64X_gc,
	/** 128 samples to accumulate. */
	ADC_SAMPNUM_128X = ADC_SAMPNUM_128X_gc,
	/** 256 samples to accumulate. */
	ADC_SAMPNUM_256X = ADC_SAMPNUM_256X_gc,
	/** 512 samples to accumulate. */
	ADC_SAMPNUM_512X = ADC_SAMPNUM_512X_gc,
	/** 1024 samples to accumulate. */
	ADC_SAMPNUM_1024X = ADC_SAMPNUM_1024X_gc,
};

/**
 * \brief Enables ADC channel averaging
 *
 * Sets the parameters number of samples used during averaging.
 *
 * \param ch_conf  Pointer to the ADC channel configuration structure
 * \param sample   Number of samples to accumulate
 *
 * \note Only the AVR XMEGA E family features this setting.
 * \note Check that "ADC_RES_MT12" param is used
 * in adc_set_conversion_parameters() call.
 */
static inline void adcch_enable_averaging(
		struct adc_channel_config *ch_conf,
		enum adcch_sampnum sample)
{
	uint8_t rshift;

	Assert( sample >= ADC_SAMPNUM_2X );

	if (sample >= ADC_SAMPNUM_16X) {
		rshift = 4;
	} else if (sample == ADC_SAMPNUM_8X) {
		rshift = 3;
	} else if (sample == ADC_SAMPNUM_4X) {
		rshift = 2;
	} else {
		rshift = 1;
	}

	ch_conf->avgctrl = sample | (rshift << ADC_CH_RIGHTSHIFT_gp);
}

/**
 * \brief Disables ADC channel averaging
 *
 * \param ch_conf    Pointer to the ADC channel configuration structure
 *
 * \note Only the AVR XMEGA E family features this setting.
 * \note Check that "ADC_RES_MT12" param is not used
 * in adc_set_conversion_parameters() call.
 */
static inline void adcch_disable_averaging(struct adc_channel_config *ch_conf)
{
	ch_conf->avgctrl = 0;
}

/**
 * \brief Enables ADC channel over-sampling
 *
 * Sets the parameters number of samples and result resolution
 * used during over-sampling.
 *
 * \param ch_conf    Pointer to the ADC channel configuration structure
 * \param sample     Number of samples to accumulate
 * \param resolution result resolution (12 bits to 16 bits)
 *                   15 bits maximum if sample = 8
 *                   14 bits maximum if sample = 4
 *                   13 bits maximum if sample = 2
 *
 * \note Only the AVR XMEGA E family features this setting.
 * \note Check that "ADC_RES_MT12" param is used
 * in adc_set_conversion_parameters() call.
 */
static inline void adcch_enable_oversampling(
		struct adc_channel_config *ch_conf,
		enum adcch_sampnum sample, uint8_t resolution)
{
	uint8_t rshift;

	Assert((resolution >= 12) && (resolution <= 16));

	if (sample >= ADC_SAMPNUM_16X) {
		rshift = 4;
	} else if (sample == ADC_SAMPNUM_8X) {
		rshift = 3;
	} else if (sample == ADC_SAMPNUM_4X) {
		rshift = 2;
	} else {
		rshift = 1;
	}

	Assert(rshift >= resolution - 12);
	rshift -= resolution - 12;
	ch_conf->avgctrl = sample | (rshift << ADC_CH_RIGHTSHIFT_gp);
}

/**
 * \brief Disables ADC channel over-sampling
 *
 * \param ch_conf    Pointer to the ADC channel configuration structure
 *
 * \note Only the AVR XMEGA E family features this setting.
 * \note Check that "ADC_RES_MT12" param is not used
 * in adc_set_conversion_parameters() call.
 */
static inline void adcch_disable_oversampling(struct adc_channel_config *ch_conf)
{
	ch_conf->avgctrl = 0;
}

#endif

/** @} */

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

/**
 * \page adc_quickstart Quick start guide for XMEGA ADC
 *
 * This is the quick start guide for the \ref adc_group, with step-by-step
 * instructions on how to configure and use the driver in a selection of use
 * cases.
 *
 * The use cases are described with "setup" and "usage" sections, which each
 * have "example code" and "workflow" subsections. This documentation first
 * presents code fragments and function definitions along with instructions on
 * where they can be placed, e.g., into the application C-file or the main()
 * function, then follows up with explanations for all the lines of code.
 *
 * \section adc_use_cases Use cases
 *
 * In addition to the basic use case below, refer to the following use cases for
 * demonstrations of the ADC's features:
 * - \subpage adc_use_case_1
 * - \subpage adc_use_case_2
 *
 * We recommend reading all the use cases for the sake of all the notes on
 * considerations, limitations and other helpful details.
 *
 * \section adc_basic_use_case Basic use case
 *
 * In this basic use case, ADCA is configured for:
 * - sampling on a single channel (0)
 *    - I/O pin as single-ended input (PA0)
 * - unsigned conversions
 * - 12-bit resolution
 * - internal 1V reference
 * - manual conversion triggering
 * - polled operation (no interrupts)
 *
 * Completed conversions are detected by waiting for the relevant interrupt flag
 * to get set. The ADC result is then stored in a local variable.
 *
 * \section adc_basic_use_case_setup Setup steps
 *
 * \subsection adc_basic_use_case_setup_code Example code
 *
 * Add to application C-file:
 * \code
	 #define MY_ADC    ADCA
	 #define MY_ADC_CH ADC_CH0

	 static void adc_init(void)
	 {
	     struct adc_config adc_conf;
	     struct adc_channel_config adcch_conf;

	     adc_read_configuration(&MY_ADC, &adc_conf);
	     adcch_read_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);

	     adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12,
	             ADC_REF_BANDGAP);
	     adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	     adc_set_clock_rate(&adc_conf, 200000UL);

	     adcch_set_input(&adcch_conf, ADCCH_POS_PIN0, ADCCH_NEG_NONE, 1);

	     adc_write_configuration(&MY_ADC, &adc_conf);
	     adcch_write_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);
	 }
\endcode
 *
 * Add to \c main():
 * \code
	sysclk_init();
	adc_init();
\endcode
 *
 * \subsection adc_basic_use_case_setup_flow Workflow
 *
 * -# Add macros for the ADC and its channel to use, so they are easy to change:
 *     - \code
	#define MY_ADC    ADCA
	#define MY_ADC_CH ADC_CH0
\endcode
 * -# Create a function \c adc_init() to intialize the ADC:
 *     - \code
	static void adc_init(void)
	{
	    // ...
	}
\endcode
 * -# Allocate configuration structs for the ADC and its channel:
 *     - \code
	struct adc_config adc_conf;
	struct adc_channel_config adcch_conf;
\endcode
 * -# Initialize the structs:
 *     - \code
	adc_read_configuration(&MY_ADC, &adc_conf);
	adcch_read_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);
\endcode
 *     \attention This step must not be skipped because uninitialized structs
 * may contain invalid configurations, thus giving unpredictable behavior.
 * -# Set conversion parameters to unsigned, 12-bit and internal 1V reference:
 *     - \code
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12,
	        ADC_REF_BANDGAP);
\endcode
 *     \note Only single-ended input is possible with unsigned conversions.
 * -# Set conversion trigger to manual triggering:
 *     - \code
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
\endcode
 *     \note The number of channels to trigger (1) and base event channel (0)
 * don't affect operation in this trigger mode, but sane values should still be
 * supplied.
 * -# Set ADC clock rate to 200 KHz or less:
 *     - \code
	adc_set_clock_rate(&adc_conf, 200000UL);
\endcode
 *     \note The driver attempts to set the ADC clock rate to the fastest
 * possible without exceeding the specified limit. Refer to the applicable
 * device datasheet and manual for details on maximum ADC clock rate.
 * -# Set pin 0 on the associated port as the single-ended input:
 *     - \code
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN0, ADCCH_NEG_NONE, 1);
\endcode
 *     \note For single-ended input, the negative input must be none and the
 * gain must be unity (1x).
 * -# Write the configurations to ADC and channel:
 *     - \code
	adc_write_configuration(&MY_ADC, &adc_conf);
	adcch_write_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);
\endcode
 * -# Initialize the clock system:
 *      - \code sysclk_init(); \endcode
 *      \note The ADC driver requires the system clock driver to be
 * initialized in order to compute the correct ADC clock rate in step 6.
 * -# Call our ADC init function:
 *      - \code adc_init(); \endcode
 *
 * \section adc_basic_use_case_usage Usage steps
 *
 * \subsection adc_basic_use_case_usage_code Example code
 *
 * Add to, e.g., main-loop in application C-file:
 * \code
	 uint16_t result;

	 adc_enable(&MY_ADC);

	 adc_start_conversion(&MY_ADC, MY_ADC_CH);
	 adc_wait_for_interrupt_flag(&MY_ADC, MY_ADC_CH);

	 result = adc_get_result(&MY_ADC, MY_ADC_CH);
\endcode
 *
 * \subsection adc_basic_use_case_usage_flow Workflow
 *
 * -# Allocate a variable to contain the ADC result:
 *     - \code uint16_t result; \endcode
 * -# Enable the configured ADC:
 *     - \code adc_enable(&MY_ADC); \endcode
 * -# Trigger a single conversion on the ADC channel:
 *     - \code adc_start_conversion(&MY_ADC, MY_ADC_CH); \endcode
 * -# Wait for the channel's interrupt flag to get set, indicating a completed
 * conversion:
 *     - \code adc_wait_for_interrupt_flag(&MY_ADC, MY_ADC_CH); \endcode
 *     \note The interrupt flags are set even if the interrupts are disabled.
 * Further, this function will clear the interrupt flag after it has been set,
 * so we do not need to clear it manually.
 * -# Read out the result of the ADC channel:
 *     - \code result = adc_get_result(&MY_ADC, MY_ADC_CH); \endcode
 * -# To do more conversions, go back to step 3.
 */

/**
 * \page adc_use_case_1 Free-running conversions with interrupt
 *
 * In this use case, ADCA is configured for:
 * \li sampling on two channels (0 and 1) with respective inputs:
 *     - I/O pin as single-ended input (PA0)
 *     - two I/O pins as differential input w/ 2x gain (PA1 and PA5)
 * \li signed conversions
 * \li 12-bit resolution
 * \li internal 1V reference
 * \li free-running conversions
 * \li interrupt-based conversion handling
 *
 * The ADC results are handled in an interrupt callback function which simply
 * stores the result in one of two channel-specific, global variables.
 *
 * \note This use case assumes that the device has multiple ADC channels. Refer
 * to the applicable device datasheet for information about the number of ADC
 * channels.
 *
 * \section adc_use_case_1_setup Setup steps
 *
 * \subsection adc_use_case_1_setup_code Example code
 *
 * Ensure that \ref conf_adc.h contains:
 * \code
	#define CONFIG_ADC_CALLBACK_ENABLE
	#define CONFIG_ADC_CALLBACK_TYPE int16_t
\endcode
 *
 * Add to application C-file:
 * \code
	 #define MY_ADC   ADCA

	 int16_t ch0_result;
	 int16_t ch1_result;

	 static void adc_handler(ADC_t *adc, uint8_t ch_mask, adc_result_t result)
	 {
	     switch (ch_mask) {
	     case ADC_CH0:
	         ch0_result = result;
	         break;

	     case ADC_CH1:
	         ch1_result = result;
	         break;

	     default:
	         break;
	     }
	 }

	 static void adc_init(void)
	 {
	     struct adc_config adc_conf;
	     struct adc_channel_config adcch_conf;

	     adc_read_configuration(&MY_ADC, &adc_conf);
	     adcch_read_configuration(&MY_ADC, ADC_CH0, &adcch_conf);

	     adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12,
	             ADC_REF_BANDGAP);
	     adc_set_conversion_trigger(&adc_conf, ADC_TRIG_FREERUN_SWEEP, 2, 0);
	     adc_set_clock_rate(&adc_conf, 5000UL);
	     adc_set_callback(&MY_ADC, &adc_handler);
	     adc_write_configuration(&MY_ADC, &adc_conf);

	     adcch_enable_interrupt(&adcch_conf);
	     adcch_set_input(&adcch_conf, ADCCH_POS_PIN0, ADCCH_NEG_NONE, 1);
	     adcch_write_configuration(&MY_ADC, ADC_CH0, &adcch_conf);

	     adcch_set_input(&adcch_conf, ADCCH_POS_PIN1, ADCCH_NEG_PIN5, 2);
	     adcch_write_configuration(&MY_ADC, ADC_CH1, &adcch_conf);
	 }
\endcode
 *
 * Add to \c main():
 * \code
	sysclk_init();
	adc_init();
	pmic_init();
\endcode
 *
 * \subsection adc_use_case_1_setup_flow Workflow
 *
 * -# Define a macro for the ADC to use, in case we want to change it later:
 *     - \code #define MY_ADC   ADCA \endcode
 * -# Define global variables to contain the ADC result of each channel:
 *     - \code
	int16_t ch0_result;
	int16_t ch1_result;
\endcode
 * -# Create an ADC interrupt callback function that stores the results in the
 * channels' respective global variables:
 *     - \code
	 static void adc_handler(ADC_t *adc, uint8_t ch_mask, adc_result_t result)
	 {
	     switch (ch_mask) {
	     case ADC_CH0:
	         ch0_result = result;
	         break;

	     case ADC_CH1:
	         ch1_result = result;
	         break;

	     default:
	         break;
	     }
	 }
\endcode
 *       \note Refer to \ref adc_callback_t for documentation on the interrupt
 * callback function type.
 * -# Create a function \c adc_init() to intialize the ADC:
 *     - \code
	static void adc_init(void)
	{
	    // ...
	}
\endcode
 * -# Allocate configuration structs for ADC and channel, then initialize them:
 *     - \code
	 struct adc_config adc_conf;
	 struct adc_channel_config adcch_conf;

	 adc_read_configuration(&MY_ADC, &adc_conf);
	 adcch_read_configuration(&MY_ADC, ADC_CH0, &adcch_conf);
\endcode
 * -# Set signed, 12-bit conversions with internal 1V voltage reference:
 *     - \code
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12,
	        ADC_REF_BANDGAP);
\endcode
 *     \note With signed, 12-bit conversion, 1 bit is used to indicate
 * sign/polarity, so the resolution is halved in terms of Volt per LSB.
 * -# Set free-running conversions on the first two ADC channels:
 *     - \code
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_FREERUN_SWEEP, 2, 0);
\endcode
 *     \note The base event channel (0) does not affect operation in this
 * mode.
 * -# Set ADC clock rate to maximum 5 KHz:
 *     - \code adc_set_clock_rate(&adc_conf, 5000UL); \endcode
 *     \note In free-running mode, it is wise to reduce the ADC clock so that
 * the device has time to handle the results, e.g., channel 0 does not complete
 * a new conversion before channel 1's result has been handled.
 * -# Set the interrupt callback function to use for the ADC:
 *     - \code adc_set_callback(&MY_ADC, &adc_handler); \endcode
 * -# Write the configuration to the ADC:
 *     - \code adc_write_configuration(&MY_ADC, &adc_conf); \endcode
 * -# Enable interrupts for the ADC channels:
 *     - \code adcch_enable_interrupt(&adcch_conf); \endcode
 * -# Set up single-ended input from pin 0 on port A, then write the config to
 * the first channel (0):
 *     - \code
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN0, ADCCH_NEG_NONE, 1);
	adcch_write_configuration(&MY_ADC, ADC_CH0, &adcch_conf);
\endcode
 * -# Set up differential input from pins 1 and 5 on port A, with 2x gain, then
 * write the config to the second channel (1):
 *     - \code
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN1, ADCCH_NEG_PIN5, 2);
	adcch_write_configuration(&MY_ADC, ADC_CH1, &adcch_conf);
\endcode
 *     \note Not all input and gain combinations are valid. Refer to
 * \ref adcch_set_input() for documentation on the restrictions.
 * -# Initialize the clock system, the ADC, and the PMIC since we will be using
 * interrupts:
 *     - \code
	sysclk_init();
	adc_init();
	pmic_init();
\endcode
 *     \note The call to \ref pmic_init() does not enable interrupts globally,
 * which must be done explicitly with \ref cpu_irq_enable().
 *
 * \section adc_use_case_1_usage Usage steps
 *
 * \subsection adc_use_case_1_usage_code Example code
 *
 * Add to \c main.c():
 * \code
	 cpu_irq_enable();
	 adc_enable(&MY_ADC);

	 do {
	 } while (true);
\endcode
 *
 * \subsection adc_use_case_1_usage_flow Workflow
 * -# Enable interrupts globally to allow the ADC interrupts to be handled:
 *     - \code cpu_irq_enable(); \endcode
 * -# Enable the ADC to start conversions:
 *     - \code adc_enable(&MY_ADC); \endcode
 *     \note When configured for free-running conversions, the ADC will start
 * doing conversions as soon as it is enabled, so we do not need to do it
 * manually.
 * -# Enter a busy-loop while interrupts handle the ADC results:
 *     - \code
	do {
	} while (true);
\endcode
 */

/**
 * \page adc_use_case_2 Event-triggered conversions
 *
 * In this use case, ADCA is configured for:
 * \li sampling on two channels (0 and 1) with respective inputs:
 *     - internal temperature sensor
 *     - internal bandgap reference
 * \li unsigned conversions
 * \li 12-bit resolution
 * \li VCC/1.6 as voltage reference
 * \li event-triggered conversions
 * \li polled conversion handling
 *
 * Completed conversions are detected via non-blocking polling of the interrupt
 * flags. The ADC results are stored into local variables as soon as they are
 * available.
 *
 * A Timer/Counter is used to generate events that trigger the conversions.
 *
 * \note This use case assumes that the device has multiple ADC channels. Refer
 * to the applicable device datasheet for information about the number of ADC
 * channels.
 *
 * \section adc_use_case_2_setup Setup steps
 *
 * \subsection adc_use_case_2_setup_prereq Prerequisites
 *
 * This use case requires that the Timer/Counter driver is added to the project.
 *
 * \subsection adc_use_case_2_setup_code Example code
 *
 * Add to application C-file:
 * \code
	 #define MY_ADC    ADCA
	 #define MY_TIMER  TCC0

	 static void evsys_init(void)
	 {
	     sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EVSYS);
	     EVSYS.CH3MUX = EVSYS_CHMUX_TCC0_OVF_gc;
	 }

	 static void tc_init(void)
	 {
	     tc_enable(&MY_TIMER);
	     tc_set_wgm(&MY_TIMER, TC_WG_NORMAL);
	     tc_write_period(&MY_TIMER, 200);
	     tc_set_resolution(&MY_TIMER, 2000);
	 }

	 static void adc_init(void)
	 {
	     struct adc_config adc_conf;
	     struct adc_channel_config adcch_conf;

	     adc_read_configuration(&MY_ADC, &adc_conf);
	     adcch_read_configuration(&MY_ADC, ADC_CH0, &adcch_conf);

	     adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12,
	             ADC_REF_VCC);
	     adc_set_conversion_trigger(&adc_conf, ADC_TRIG_EVENT_SWEEP, 2, 3);
	     adc_enable_internal_input(&adc_conf, ADC_INT_BANDGAP
	             | ADC_INT_TEMPSENSE);
	     adc_set_clock_rate(&adc_conf, 200000UL);
	     adc_write_configuration(&MY_ADC, &adc_conf);

	     adcch_set_input(&adcch_conf, ADCCH_POS_TEMPSENSE, ADCCH_NEG_NONE, 1);
	     adcch_write_configuration(&MY_ADC, ADC_CH0, &adcch_conf);

	     adcch_set_input(&adcch_conf, ADCCH_POS_BANDGAP, ADCCH_NEG_NONE, 1);
	     adcch_write_configuration(&MY_ADC, ADC_CH1, &adcch_conf);
	 }
\endcode
 *
 * Add to \c main():
 * \code
	sysclk_init();
	evsys_init();
	tc_init();
	adc_init();
\endcode
 *
 * \subsection adc_use_case_2_setup_flow Workflow
 *
 * -# Add macros for the ADC and the conversion trigger timer to use, so they
 *    are easy to change:
 *     - \code
	#define MY_ADC    ADCA
	#define MY_TIMER  TCC0
\endcode
 * -# Create a function \c evsys_init() to intialize the event system clocks and
 *    to link the conversion timer to the correct event channel:
 *     - \code
	static void evsys_init(void)
	{
	    // ...
	}
\endcode
 * -# Use the sysclk service to enable the clock to the event system:
 *     - \code sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EVSYS); \endcode
 * -# Connect the TCC0 overflow event to event channel 3:
 *     - \code EVSYS.CH3MUX = EVSYS_CHMUX_TCC0_OVF_gc; \endcode
 *    \note If the ADC trigger timer is changed from TCC0, the \c EVSYS_CHMUX_*
 *          mask here will also need to be altered.
 * -# Create a function \c tc_init() to intialize the ADC trigger timer:
 *     - \code
	static void tc_init(void)
	{
	    // ...
	}
\endcode
 * -# Enable the clock to the ADC trigger timer:
 *     - \code tc_enable(&MY_TIMER); \endcode
 * -# Configure the ADC trigger timer in normal Waveform Generation mode:
 *     - \code tc_set_wgm(&MY_TIMER, TC_WG_NORMAL); \endcode
 * -# Configure the ADC trigger timer period to overflow at 200 counts:
 *     - \code tc_write_period(&MY_TIMER, 200); \endcode
 * -# Configure the ADC trigger timer resolution (frequency) for 2KHz:
 *     - \code tc_set_resolution(&MY_TIMER, 2000); \endcode
 * -# Create a function \c adc_init() to intialize the ADC ready for
 *    conversions on channels 0 and 1, triggered by the event system:
 *     - \code
	static void adc_init(void)
	{
	    // ...
	}
\endcode
 * -# Allocate configuration structs for ADC and channel, then initialize them:
 *     - \code
	 struct adc_config adc_conf;
	 struct adc_channel_config adcch_conf;

	 adc_read_configuration(&MY_ADC, &adc_conf);
	 adcch_read_configuration(&MY_ADC, ADC_CH0, &adcch_conf);
\endcode
 * -# Set unsigned, 12-bit conversions with internal VCC/1.6 voltage reference:
 *     - \code
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12,
	ADC_REF_VCC);
\endcode
 * -# Set event system triggered conversions on the first two ADC channels,
 *    with conversions triggered by event system channel 3:
 *     - \code
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_EVENT_SWEEP, 2, 3);
\endcode
 * \note The event system channel used here must match the channel linked to the
 *       conversion trigger timer set up earlier in \c tc_init().
 * -# Turn on the internal bandgap and temperature sensor ADC inputs:
 *     - \code
	adc_enable_internal_input(&adc_conf, ADC_INT_BANDGAP | ADC_INT_TEMPSENSE);
\endcode
 * -# Set ADC clock rate to maximum 200 KHz:
 *     - \code adc_set_clock_rate(&adc_conf, 200000UL); \endcode
 * -# Write the configuration to the ADC:
 *     - \code adc_write_configuration(&MY_ADC, &adc_conf); \endcode
 * -# Set up single-ended input from the internal temperature sensor, then write
 *    the config to the first channel (0):
 *     - \code
	adcch_set_input(&adcch_conf, ADCCH_POS_TEMPSENSE, ADCCH_NEG_NONE, 1);
	adcch_write_configuration(&MY_ADC, ADC_CH0, &adcch_conf);
\endcode
 * -# Set up single-ended input from the internal bandgap voltage, then write
 *    the config to the second channel (1):
 *     - \code
	adcch_set_input(&adcch_conf, ADCCH_POS_BANDGAP, ADCCH_NEG_NONE, 1);
	adcch_write_configuration(&MY_ADC, ADC_CH1, &adcch_conf);
\endcode
 * -# Initialize the clock system, event system, ADC trigger timer, and the ADC:
 *     - \code
	sysclk_init();
	evsys_init();
	tc_init();
	adc_init();
\endcode
 *
 * \section adc_use_case_2_usage Usage steps
 *
 * \subsection adc_use_case_2_usage_code Example code
 *
 * Add to \c main():
 * \code
	 adc_enable(&MY_ADC);

	 do {
	     uint16_t tmp_result;
	     uint16_t bg_result;

	     if (adc_get_interrupt_flag(&MY_ADC, ADC_CH0 | ADC_CH1)
	             == (ADC_CH0 | ADC_CH1)) {
	         tmp_result = adc_get_result(&MY_ADC, ADC_CH0);
	         bg_result = adc_get_result(&MY_ADC, ADC_CH1);

	         adc_clear_interrupt_flag(&MY_ADC, ADC_CH0 | ADC_CH1);
	     }
	 } while (true);
\endcode
 *
 * \subsection adc_use_case_2_usage_flow Workflow
 *
 * -# Enable the configured ADC module, so that it will begin conversions when
 *    triggered:
 *     - \code adc_enable(&MY_ADC); \endcode
 * -# Create an infinite loop so that conversions will be processed forever:
 *     - \code
	do {
	   // ...
	} while (true);
\endcode
 * -# Within the loop, create local variables to contain the ADC result of each
 *    channel (internal temperature sensor and internal bandgap voltage):
 *     - \code
	int16_t temp_result;
	int16_t bg_result;
\endcode
 * -# Test if both ADC channel 0 and channel 1 have completed a conversion by
 *    testing the respective channel conversion complete interrupt flags:
 *     - \code
	if (adc_get_interrupt_flag(&MY_ADC, ADC_CH0 | ADC_CH1)
	        == (ADC_CH0 | ADC_CH1)) {
\endcode
 * -# Store the channel result values into the local variables created earlier:
 *     - \code
	tmp_result = adc_get_result(&MY_ADC, ADC_CH0);
	bg_result = adc_get_result(&MY_ADC, ADC_CH1);
\endcode
 * -# Clear both ADC channel conversion complete interrupt flags, so that we can
 *    detect future conversions at a later stage:
 *     - \code adc_clear_interrupt_flag(&MY_ADC, ADC_CH0 | ADC_CH1); \endcode
 */

#endif /* ADC_H */
