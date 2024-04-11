/**
 * \file
 *
 * \brief Programmable Multilevel Interrupt Controller driver
 *
 * Copyright (c) 2010-2012 Atmel Corporation. All rights reserved.
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
#ifndef PMIC_H
#define PMIC_H

#include <compiler.h>
#include <ccp.h>

/**
 * \defgroup pmic_group Programmable Multilevel Interrupt Controller
 *
 * See \ref xmega_pmic_quickstart.
 *
 * This is a low-level driver implementation for the AVR XMEGA Programmable
 * Multilevel Interrupt Controller.
 *
 * \note If these functions are used in interrupt service routines (ISRs), any
 * non-ISR code or ISR code for lower level interrupts must ensure that the
 * operations are atomic, i.e., by disabling interrupts during the function
 * calls.
 * @{
 */

/**
 * \brief Interrupt level bitmasks
 *
 * \note These may be OR'ed, e.g., if multiple levels are to be enabled or
 * disabled.
 */
enum pmic_level {
	PMIC_LVL_LOW    = PMIC_LOLVLEN_bm,    //!< Low-level interrupts
	PMIC_LVL_MEDIUM = PMIC_MEDLVLEN_bm,   //!< Medium-level interrupts
	PMIC_LVL_HIGH   = PMIC_HILVLEN_bm,    //!< High-level interrupts
	/**
	 * \brief Non-maskable interrupts
	 * \note These cannot be enabled nor disabled.
	 */
	PMIC_LVL_NMI    = PMIC_NMIEX_bp,
};

//! Interrupt vector locations
enum pmic_vector {
	PMIC_VEC_APPLICATION,    //!< Application section
	PMIC_VEC_BOOT,           //!< Boot section
	PMIC_NR_OF_VECTORS,      //!< Number of interrupt vector locations
};

//! Interrupt scheduling schemes
enum pmic_schedule {
	PMIC_SCH_FIXED_PRIORITY, //!< Default, fixed priority scheduling
	PMIC_SCH_ROUND_ROBIN,    //!< Round-robin scheduling
	PMIC_NR_OF_SCHEDULES,    //!< Number of interrupt scheduling schemes
};

/**
 * \brief Initialize the PMIC
 *
 * Enables all interrupt levels, with vectors located in the application section
 * and fixed priority scheduling.
 */
static inline void pmic_init(void)
{
	PMIC.CTRL = PMIC_LVL_LOW | PMIC_LVL_MEDIUM |
			PMIC_LVL_HIGH;
}

/**
 * \brief Enable interrupts with specified \a level(s).
 *
 * \param level Interrupt level(s) to enable.
 */
static inline void pmic_enable_level(enum pmic_level level)
{
	Assert((level & PMIC_LVL_NMI));

	PMIC.CTRL |= level;
}

/**
 * \brief Disable interrupts with specified \a level(s).
 *
 * \param level Interrupt level(s) to disable.
 */
static inline void pmic_disable_level(enum pmic_level level)
{
	Assert((level & PMIC_LVL_NMI));

	PMIC.CTRL &= ~level;
}

/**
 * \brief Check if specified interrupt \a level(s) is enabled.
 *
 * \param level Interrupt level(s) to check.
 *
 * \return True if interrupt level(s) is enabled.
 */
static inline bool pmic_level_is_enabled(enum pmic_level level)
{
	Assert((level & PMIC_LVL_NMI));

	return PMIC.CTRL & level;
}

/**
 * \brief Get currently enabled level(s)
 *
 * \return Bitmask with currently enabled levels.
 */
static inline enum pmic_level pmic_get_enabled_levels(void)
{
	return (enum pmic_level)(PMIC.CTRL & (PMIC_LVL_LOW | PMIC_LVL_MEDIUM
				| PMIC_LVL_HIGH));
}

/**
 * \brief Check if an interrupt level(s) is currently executing.
 *
 * \param level Interrupt level(s) to check.
 *
 * \return True if interrupt level(s) is currently executing.
 */
static inline bool pmic_level_is_executing(enum pmic_level level)
{
	return PMIC.STATUS & level;
}

/**
 * \brief Set interrupt scheduling for low-level interrupts.
 *
 * \param schedule Interrupt scheduling method to set.
 *
 * \note The low-priority vector, INTPRI, must be set to 0 when round-robin
 * scheduling is disabled to return to default interrupt priority order.
 */
static inline void pmic_set_scheduling(enum pmic_schedule schedule)
{
	Assert(schedule < PMIC_NR_OF_SCHEDULES);

	switch (schedule) {
	case PMIC_SCH_FIXED_PRIORITY:
		PMIC.CTRL &= ~PMIC_RREN_bm;
		PMIC.INTPRI = 0;
		break;

	case PMIC_SCH_ROUND_ROBIN:
		PMIC.CTRL |= PMIC_RREN_bm;
		break;

	default:
		break;
	};
}

/**
 * \brief Set location of interrupt vectors.
 *
 * \param vector Location to use for interrupt vectors.
 */
static inline void pmic_set_vector_location(enum pmic_vector vector)
{
	uint8_t ctrl = PMIC.CTRL;

	Assert(vector < PMIC_NR_OF_VECTORS);

	switch (vector) {
	case PMIC_VEC_APPLICATION:
		ctrl &= ~PMIC_IVSEL_bm;
		break;

	case PMIC_VEC_BOOT:
		ctrl |= PMIC_IVSEL_bm;
		break;

	default:
		break;
	}

	ccp_write_io((uint8_t*)&PMIC.CTRL, ctrl);
}

//! @}

/**
 * \page xmega_pmic_quickstart Quick start guide for PMIC driver
 *
 * This is the quick start guide for the \ref pmic_group "PMIC driver" and
 * the closely related \ref interrupt_group "global interrupt driver", with
 * step-by-step instructions on how to configure and use the drivers in a
 * selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 *
 * \section pmic_basic_use_case Basic use case
 * In this basic use case, the PMIC is configured for:
 * - all interrupt levels enabled
 * - round-robin scheduling
 * 
 * This will allow for interrupts from other modules being used.
 *
 * \section pmic_basic_use_case_setup Setup steps
 *
 * \subsection pmic_basic_use_case_setup_prereq Prerequisites
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * -# Interrupts for the module requiring the PMIC module have to be 
 *    enabled.
 * -# An Interrupt Service Routine (ISR) for a given interrupt vector has to be 
 *    defined, where the interrupt vectors available are defined by toolchain and 
 *    listed in the subsection 'Interrupt Vector Summary' in the data sheet.
 * \code
 *     ISR(interrupt_vector){
 *         //Interrupt Service Routine
 *     }
 * \endcode
 *
 * \subsection pmic_basic_use_case_setup_code Example code
 * Add to the initialization code:
 * \code
 *     pmic_init();
 *     pmic_set_scheduling(PMIC_SCH_ROUND_ROBIN);
 *     cpu_irq_enable();
 * \endcode
 *
 * \subsection pmic_basic_use_case_setup_flow Workflow
 * -# call the PMIC driver's own init function to enable all interrupt levels:
 *   - \code pmic_init(); \endcode
 * -# enable round-robin instead of fixed priority interrupt scheduling:
 *   - \code pmic_set_scheduling(PMIC_SCH_ROUND_ROBIN); \endcode
 * -# enable interrupts globally:
 *   - \code cpu_irq_enable(); \endcode
 *   - \attention Interrupts will not trigger without this step.
 *
 * \section pmic_use_cases Advanced use cases
 * For more advanced use of the PMIC driver, see the following use cases:
 * - \subpage pmic_use_case_1 : atomic operations
 */

/**
 * \page pmic_use_case_1 Use case #1
 *
 * In this use case, the PMIC is configured for:
 * - all interrupt levels enabled
 * 
 * This will allow for interrupts from other modules being used.
 *
 * This use case shows how to make an operation which consists of multiple
 * instructions uninterruptible, i.e., into an atomic operation. This is often
 * necessary if there is a risk that data can be accessed by interrupt handlers
 * while other code is accessing it, and at least one of them modifies it.
 *
 * \section pmic_use_case_1_setup Setup steps
 *
 * \subsection pmic_basic_use_case_setup_prereq Prerequisites
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * -# Interrupts for the module requiring the PMIC module have to be 
 *    enabled.
 * -# An Interrupt Service Routine (ISR) for a given interrupt vector has to be 
 *    defined, where the interrupt vectors available are defined by toolchain and 
 *    listed in the subsection 'Interrupt Vector Summary' in the data sheet.
 * \code
 *     ISR(interrupt_vector){
 *         //Interrupt Service Routine
 *     }
 * \endcode
 *
 * \subsection pmic_use_case_1_setup_code Example code
 * Add to application initialization:
 * \code
 *     pmic_init();
 *     cpu_irq_enable();
 * \endcode
 *
 * \subsection pmic_use_case_1_setup_flow Workflow
 * -# call the PMIC driver's own init function to enable all interrupt levels:
 *   - \code pmic_init(); \endcode
 * -# set global interrupt enable flag:
 *   - \code cpu_irq_enable(); \endcode
 *
 * \section pmic_use_case_1_usage Usage steps
 *
 * \subsection pmic_use_case_1_usage_code Example code
 * \code
 * Add to application:
 * void atomic_operation(void)
 * {
 *     irqflags_t flags;
 *
 *     flags = cpu_irq_save();
 *
 *     // Uninterruptible block of code
 *
 *     cpu_irq_restore(flags);
 * }
 * \endcode
 *
 * \subsection pmic_use_case_1_usage_flow Workflow
 * -# allocate temporary storage for interrupt enable:
 *   - \code irqflags_t flags; \endcode
 * -# clear global interrupt enable flag while saving its previous state:
 *   - \code flags = cpu_irq_save(); \endcode
 * -# restore the previous state of global interrupt flag after operation:
 *   - \code cpu_irq_restore(flags); \endcode
 */

#endif /* PMIC_H */
