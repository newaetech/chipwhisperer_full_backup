/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * @file osif_baremetal.c
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 1.3, Taking address of near auto variable.
 * The code is not dynamically linked. An absolute stack address is obtained
 * when taking the address of the near auto variable. A source of error in
 * writing dynamic code is that the stack segment may be different from the data
 * segment.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Directive 4.9, Function-like macro defined.
 * The macros are used to validate input parameters to driver functions.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, External could be made static.
 * Function is defined for usage by application code.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 11.4, Conversion between a pointer and integer type.
 * The cast is required to initialize a pointer with an unsigned long define, representing an address.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 11.6, Cast from unsigned int to pointer.
 * This is required for initializing pointers to the module's memory map, which is located at a
 * fixed address.
 */

#include "osif.h"
#include <stddef.h>

#include "../device/MPC5676R.h"
#include "../device/MPC5676R_features.h"
#include "devassert.h"

#if defined (USING_OS_FREERTOS)
#error "Wrong OSIF selected. Please define symbol USING_OS_BAREMETAL (or no OS define) in project settings or change the OSIF variant"
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/*! @cond DRIVER_INTERNAL_USE_ONLY */

/*! @brief Converts milliseconds to ticks - in this case, one tick = one millisecond */
#define MSEC_TO_TICK(msec) (msec)

#if (FEATURE_OSIF_USE_SYSTICK != 0) || (FEATURE_OSIF_USE_PIT != 0)
/* Only include headers for configurations that need them. */
#include "interrupt_manager.h"
//#include "clock_manager.h"

static volatile uint32_t s_osif_tick_cnt = 0u;

static inline void osif_Tick(void)
{
    s_osif_tick_cnt++;
}

static inline uint32_t osif_GetCurrentTickCount(void)
{
    return s_osif_tick_cnt;
}

#endif /* (FEATURE_OSIF_USE_SYSTICK != 0) || (FEATURE_OSIF_USE_PIT != 0) */

#if FEATURE_OSIF_USE_SYSTICK

#ifndef OSIF_CLK
    #define OSIF_CLK CORE_CLK
#endif

void SysTick_Handler(void);

void SysTick_Handler(void)
{
    osif_Tick();
}

static inline void osif_UpdateTickConfig(void)
{
    uint32_t core_freq = 0u;
    /* Get the correct name of the core clock */
    status_t clk_status = CLOCK_SYS_GetFreq(OSIF_CLK, &core_freq);
    //DEV_ASSERT(clk_status == STATUS_SUCCESS);
    //DEV_ASSERT(core_freq > 0u);
    (void)clk_status;

    /* For Cortex-M0 devices the systick counter is initialized with an undefined
     value, so make sure to initialize it to 0 before starting */
    S32_SysTick->CSR = S32_SysTick_CSR_ENABLE(0u);
    S32_SysTick->RVR = S32_SysTick_RVR_RELOAD(core_freq / 1000u);
    S32_SysTick->CVR = S32_SysTick_CVR_CURRENT(0U);
    S32_SysTick->CSR = S32_SysTick_CSR_ENABLE(1u) | S32_SysTick_CSR_TICKINT(1u) | S32_SysTick_CSR_CLKSOURCE(1u);
}

static inline void osif_DisableIrqGlobal(void)
{
    INT_SYS_DisableIRQGlobal();
}

static inline void osif_EnableIrqGlobal(void)
{
    INT_SYS_EnableIRQGlobal();
}

#elif FEATURE_OSIF_USE_PIT

#ifndef OSIF_CLK
    #define OSIF_CLK PIT0_CLK
#endif

/* application is allowed to change the pit channel used by osif
 * but care must be taken in multicore environments to not use 
 * the same channel for multiple cores
 */
#ifndef OSIF_PIT_CHAN_ID
#define OSIF_PIT_CHAN_ID (OSIF_PIT_CHAN_ID_MAX - (uint32_t)GET_CORE_ID())
#endif /* OSIF_PIT_CHAN_ID */

void OSIF_PIT_IRQHandler(void);

void OSIF_PIT_IRQHandler(void)
{
    OSIF_PIT->TIMER[OSIF_PIT_CHAN_ID].TFLG = PIT_TFLG_TIF(1u);

    osif_Tick();
}

static inline void osif_UpdateTickConfig(void)
{
#if defined(DEV_ERROR_DETECT) || defined(CUSTOM_DEVASSERT)
    static uint8_t first_init = 1;
#endif
    uint32_t pit_chan_id = OSIF_PIT_CHAN_ID;
    uint32_t tick_freq = 0;
    PIT_Type * base = OSIF_PIT;
    
    /* get the clock frequency for the timer and compute ticks for 1 ms */
    status_t clk_status = CLOCK_SYS_GetFreq(OSIF_CLK, &tick_freq);
    
    //CLOCK_SYS_GetFreq(OSIF_CLK, &tick_freq);
    //DEV_ASSERT(clk_status == STATUS_SUCCESS);
    //DEV_ASSERT(tick_freq > 0u);
    (void)clk_status;
    uint32_t tick_1ms = tick_freq / 1000u;

#if defined(DEV_ERROR_DETECT) || defined(CUSTOM_DEVASSERT)
    if (first_init)
    {
        /* check if the timer is already running. If it is, we cannot use it.
         * This only needs to be checked for the first initialization of osif.
         */
        //DEV_ASSERT((base->TIMER[pit_chan_id].TCTRL & PIT_TCTRL_TEN_MASK) == 0u);
        first_init = 0;
    }
#endif
    /* setup timer and enable interrupt */
#if FEATURE_OSIF_PIT_FRZ_IN_DEBUG
	base->MCR |= PIT_MCR_FRZ(1u); /* stop the timer in debug */
#endif /* FEATURE_OSIF_PIT_FRZ_IN_DEBUG */
    base->MCR &= ~PIT_MCR_MDIS(1u); /* make sure module is started */
    base->TIMER[pit_chan_id].LDVAL = tick_1ms;
    base->TIMER[pit_chan_id].TCTRL |= PIT_TCTRL_TEN(1u) | PIT_TCTRL_TIE(1u);
    static const IRQn_Type pitIrqId[PIT_INSTANCE_COUNT][PIT_IRQS_CH_COUNT] = PIT_IRQS;

    INT_SYS_InstallHandler(pitIrqId[0U][pit_chan_id], OSIF_PIT_IRQHandler, NULL);
    INT_SYS_EnableIRQ(pitIrqId[0U][pit_chan_id]);
}

static inline void osif_DisableIrqGlobal(void)
{
    INT_SYS_DisableIRQGlobal();
}

static inline void osif_EnableIrqGlobal(void)
{
    INT_SYS_EnableIRQGlobal();
}

#else /* FEATURE_OSIF_USE_SYSTICK == 0, FEATURE_OSIF_USE_PIT == 0 */

static inline uint32_t osif_GetCurrentTickCount(void)
{
    return 0u;
}

static inline void osif_UpdateTickConfig(void)
{
    /* do not update tick */
}

#define osif_DisableIrqGlobal() (void)0;

#define osif_EnableIrqGlobal() (void)0;

#endif /* FEATURE_OSIF_USE_SYSTICK */

/*! @endcond */

#define MS_TIMEOUT (100)

/*******************************************************************************
 * Code
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_TimeDelay
 * Description   : This function blocks execution for a number of milliseconds.
 *
 * Implements : OSIF_TimeDelay_baremetal_Activity
 *END**************************************************************************/
void OSIF_TimeDelay(const uint32_t delay)
{
    osif_UpdateTickConfig();
    uint32_t start = osif_GetCurrentTickCount();
    uint32_t crt_ticks = osif_GetCurrentTickCount();
    uint32_t delta = crt_ticks - start;
    uint32_t delay_ticks = MSEC_TO_TICK(delay);
    while (delta < delay_ticks)
    {
        for(uint32_t i = 0; i< MS_TIMEOUT; i++);

        crt_ticks +=1;
        delta = crt_ticks - start;
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_GetMilliseconds
 * Description   : This function returns the number of miliseconds elapsed since
 *                  starting the internal timer. To initialize the internal timer
 *                  (Systick) in bare-metal, call either OSIF_TimeDelay or
 *                  OSIF_SemaWait functions. Calling OSIF_TimeDelay(0) will initialize
 *                  the timer without any side-effects (no delay).
 *
 * Implements : OSIF_GetMilliseconds_baremetal_Activity
 *END**************************************************************************/
uint32_t OSIF_GetMilliseconds(void)
{
    /*
     * Please make sure the timer is initialized before calling this function.
     * For example, calling OSIF_TimeDelay(0) ensures that the timer is initialized
     * without any other side-effects. If OSIF_TimeDelay or OSIF_SemaWait functions
     * have been called, the timer is already initialized.
     */
    return osif_GetCurrentTickCount(); /* This assumes that 1 tick = 1 millisecond */
}

/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_MutexLock
 * Description   : This function locks a mutex (mock operation in baremetal case).
 *
 * Implements : OSIF_MutexLock_baremetal_Activity
 *END**************************************************************************/
status_t OSIF_MutexLock(const mutex_t * const pMutex,
                        const uint32_t timeout)
{
    (void)pMutex;
    (void)timeout;

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_MutexUnlock
 * Description   : This function unlocks a mutex (mock operation in baremetal case).
 *
 * Implements : OSIF_MutexUnlock_baremetal_Activity
 *END**************************************************************************/
status_t OSIF_MutexUnlock(const mutex_t * const pMutex)
{
    (void)pMutex;

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_MutexCreate
 * Description   : This function creates a mutex (mock operation in baremetal case).
 *
 * Implements : OSIF_MutexCreate_baremetal_Activity
 *END**************************************************************************/
status_t OSIF_MutexCreate(mutex_t * const pMutex)
{
    (void)pMutex;

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_MutexDestroy
 * Description   : This function destroys a mutex (mock operation in baremetal case).
 *
 * Implements : OSIF_MutexDestroy_baremetal_Activity
 *END**************************************************************************/
status_t OSIF_MutexDestroy(const mutex_t * const pMutex)
{
    (void)pMutex;

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_SemaWait
 * Description   : This function performs the 'wait' (decrement) operation on a semaphore.
 *      When timeout value is 0, it's the equivalent of TryWait - try to decrement but return
 *      immediately if it fails (counter is 0).
 *
 * Implements : OSIF_SemaWait_baremetal_Activity
 *END**************************************************************************/
status_t OSIF_SemaWait(semaphore_t * const pSem,
                       const uint32_t timeout)
{
    ////DEV_ASSERT(pSem != NULL);

    status_t osif_ret_code = STATUS_SUCCESS;

    osif_UpdateTickConfig();
    if ((timeout == 0u))
    {
        /* when the timeout is 0 the wait operation is the equivalent of try_wait,
            meaning that if the semaphore is 0 return immediately with an error code
        */
        if (*pSem == 0u)
        {
            osif_ret_code = STATUS_TIMEOUT;
        }
    }
    else
    {
        /* timeout is not 0 */
        uint32_t timeoutTicks;
        if (timeout == OSIF_WAIT_FOREVER)
        {
            timeoutTicks = OSIF_WAIT_FOREVER;
        }
        else
        {
            /* Convert timeout from milliseconds to ticks. */
            timeoutTicks = MSEC_TO_TICK(timeout);
        }
        uint32_t start = osif_GetCurrentTickCount();
        uint32_t end = (uint32_t)(start + timeoutTicks);
        uint32_t max = end - start;
        while (*pSem == 0u)
        {
            uint32_t crt_ticks = osif_GetCurrentTickCount();
            uint32_t delta = crt_ticks - start;
            if ((timeoutTicks != OSIF_WAIT_FOREVER) && (delta > max))
            {
                /* Timeout occured, stop waiting and return fail code */
                osif_ret_code = STATUS_TIMEOUT;
                break;
            }
        }
    }

    if (osif_ret_code == STATUS_SUCCESS)
    {
        osif_DisableIrqGlobal();
        --(*pSem);
        osif_EnableIrqGlobal();
    }

    return osif_ret_code;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_SemaPost
 * Description   : This function performs the 'post' (increment) operation on a semaphore.
 *
 * Implements : OSIF_SemaPost_baremetal_Activity
 *END**************************************************************************/
status_t OSIF_SemaPost(semaphore_t * const pSem)
{
    //DEV_ASSERT(pSem != NULL);

    status_t osif_ret_code = STATUS_SUCCESS;
    osif_DisableIrqGlobal();
    if (*pSem != 255u)
    {
        ++(*pSem);
    }
    else
    {
        osif_ret_code = STATUS_ERROR;
    }

    osif_EnableIrqGlobal();

    return osif_ret_code;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_SemaCreate
 * Description   : This function creates (initializes) a semaphore.
 *
 * Implements : OSIF_SemaCreate_baremetal_Activity
 *END**************************************************************************/
status_t OSIF_SemaCreate(semaphore_t * const pSem,
                         const uint8_t initValue)
{
    //DEV_ASSERT(pSem != NULL);
    osif_DisableIrqGlobal();
    *pSem = initValue;
    osif_EnableIrqGlobal();

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_SemaDestroy
 * Description   : This function destroys a semaphore object (mock operation in baremetal case).
 *
 * Implements : OSIF_SemaDestroy_baremetal_Activity
 *END**************************************************************************/
status_t OSIF_SemaDestroy(const semaphore_t * const pSem)
{
    //DEV_ASSERT(pSem != NULL);

    (void)pSem;

    return STATUS_SUCCESS;
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
