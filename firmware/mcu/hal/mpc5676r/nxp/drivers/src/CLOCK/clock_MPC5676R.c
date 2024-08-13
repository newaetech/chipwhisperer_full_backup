/*
 * Copyright (c) 2013 - 2016, Freescale Semiconductor, Inc.
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

/*! @file clock_MPC5748G.c */

/*!
 * @ingroup clock_manager
 * @defgroup clock_MPC57xx
 * @{
 */
 
 /**
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, External could be made static.
 * Function is defined for usage by application code.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 11.4, conversion between a pointer and integer
 * type.
 * The cast is needed to get the addresses of MC_ME and CGM hardware modules.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 11.6, cast from pointer to unsigned long
 * The cast is needed to get the addresses of MC_ME and CGM hardware modules.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 1.3, Taking address of near auto variable.
 * The code is not dynamically linked. An absolute stack address is obtained
 * when taking the address of the near auto variable. A source of error in
 * writing dynamic code is that the stack segment may be different from the data
 * segment.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 17.2, functions shall not call themselves, either
 * directly or indirectly. The maximum depth of recursion is 2.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.9, An object should be defined at block scope
 * it its identifier only appears in a single function. interfaceClocks must be visible
 * like clockNameMappings
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 16.1, All switch statements shall be well-formed.
 * Source code must address all devices from C55 family.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 16.6, Every switch statement shall have at least two switch-clauses.
 * Source code must address all devices from C55 family.
 */
 
#include "MPC5676R.h"
#include "MPC5676R_features.h"
#include "clock_siu_hw_access.h"
#include "../../inc/clock_manager.h"
#include "interrupt_manager.h"
#include <stddef.h>   
/* This header is included for bool type */
/*
 * README:
 * This file provides these APIs:
 * APIs to get the frequency of output clocks in Reference Manual ->
 * Chapter Clocking -> Figure Clock Generation diagram.
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
static clock_manager_state_t g_clockState;

/*! @brief Interface clocks
 *         Constant array storing the mappings between clock names and interface clocks.
 *         If a clock name is not a module clock name, then the corresponding value is
 *         CLOCK_NAME_COUNT.
 */
static const clock_names_t interfaceClocks[] = INTERFACE_CLOCKS;

static const uint32_t haltRegisterClocks[] = MASKSET_CLOCKS;
static const uint32_t haltClocks[] = SIU_CLOCKS;
static const uint32_t haltBitfieldClocks[] = BITFIELD_CLOCKS;
static const uint32_t haltMaskClocks[] = MASK_CLOCKS;

uint32_t g_xtal0ClkFreq = 10000000;    /* EXTAL0 clock    */

uint32_t g_sipiRefClkFreq;                       /* SIPI_REF clock  */


/*******************************************************************************
 * INTERNAL FUNCTIONS
 ******************************************************************************/
static status_t CLOCK_SYS_GetPll0PhiXFreq(uint32_t *frequency, uint32_t outputChannel);
static status_t CLOCK_SYS_GetPll1PhiXFreq(uint32_t *frequency, uint32_t outputChannel);
static status_t CLOCK_SYS_GetClockSourceFreq(clock_names_t clockName, uint32_t *frequency);
static status_t CLOCK_SYS_GetSystemClockFreq(clock_names_t clockName, uint32_t *frequency);
static status_t CLOCK_SYS_GetModuleClockFreq(clock_names_t clockName,uint32_t *frequency);
static status_t CLOCK_SYS_GetMCANClockFreq(clock_names_t clockName, uint32_t *frequency);
static status_t CLOCK_SYS_GetPerClkSelClockFreq(clock_names_t clockName, uint32_t *frequency);
static status_t CLOCK_SYS_GetLfastClockFreq(clock_names_t clockName, uint32_t *frequency);
static status_t CLOCK_SYS_ConfigureClockSources(clock_manager_user_config_t const* config);
static status_t CLOCK_SYS_ResetConfiguration(void);
static void CLOCK_SYS_SetClockSource(siu_clk_sel_t scs);
static void CLOCK_SYS_SetPeripheralClockSelect(siu_clk_sel_t perClkSel);
static void CLOCK_SYS_SetMCAN(siu_clk_sel_t mcanSel);
static void CLOCK_SYS_SetClockSourceDivider(siu_clk_div_t coreClk);
static void CLOCK_SYS_SetPbridgeClock(siu_clk_div_t pbridgeClk);
static void CLOCK_SYS_SetPerClock(siu_clk_div_t perClkSel);
static void CLOCK_SYS_SetEtpuClk(siu_clk_div_t etpuClk);
static void CLOCK_SYS_SetSIUSysdiv(clock_manager_user_config_t const* config);
static status_t CLOCK_SYS_SetLFASTClock(siu_clk_sel_t lfastSel, siu_clk_div_t lfastClkDiv);
static void CLOCK_SYS_SetSigmaDeltaClockDivider(siu_clk_div_t adcsdClk);
static void CLOCK_SYS_SetHALT(clock_manager_user_config_t const* config);
static void CLOCK_SYS_SetPSI5(clock_manager_user_config_t const* config);
static void CLOCK_SYS_SetExternalClockControl(clock_manager_user_config_t const* config);
static status_t CLOCK_SYS_ConfigureSiu(clock_manager_user_config_t const* config);
static void CLOCK_SYS_SetPll(cs_plldig_config_t const* config, siu_clk_sel_t const siuConfig, uint32_t instance);
static uint32_t CLOCK_SYS_GetFMPerDiv(void);
static uint32_t CLOCK_SYS_GetPerDiv(void);
static uint32_t CLOCK_SYS_GetEtpuDividerValue(void);
static uint32_t CLOCK_SYS_GetSysClkDivider(void);
static status_t CLOCK_SYS_GetPerClkSelFreq(uint32_t *frequency);


/*FUNCTION**********************************************************************
 *
 * Function Name : CLOCK_DRV_Init
 * Description   : This function sets the system to target configuration, it
 * only sets the clock modules registers for clock mode change, but not send
 * notifications to drivers.
 *
 * Implements CLOCK_DRV_Init_Activity
 * END**************************************************************************/
status_t CLOCK_DRV_Init(clock_manager_user_config_t const * config)
{
	status_t result = STATUS_SUCCESS;
	//DEV_ASSERT(config != NULL);

	/* Clears peripheral clock gating and all clock sources in all power modes.
	 * Only an IRC type clock source is enabled and set as system clock source. */
	result = CLOCK_SYS_ResetConfiguration();

	if (STATUS_SUCCESS == result)
	{
		/* Configure clock sources. */
		result = CLOCK_SYS_ConfigureClockSources(config);

		if (STATUS_SUCCESS == result)
		{
			/* Configure module clocks and system clocks */
			result = CLOCK_SYS_ConfigureSiu(config);

		}
	}

	return result;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : CLOCK_DRV_GetFreq
 * Description   : This function returns the frequency of a given clock
 *
 * Implements CLOCK_DRV_GetFreq_Activity
 * END**************************************************************************/
status_t CLOCK_DRV_GetFreq(clock_names_t clockName,
                           uint32_t *frequency)
{
	status_t status = STATUS_SUCCESS;
	*frequency = 0U;


	if((clockName == END_OF_CLK_SOURCES) || (clockName == END_OF_SYSTEM_CLKS) || (clockName == END_OF_PERIPHERAL_CLKS))
	{
		status = STATUS_UNSUPPORTED;
	}
	else if(clockName < END_OF_CLK_SOURCES)
	{
		status = CLOCK_SYS_GetClockSourceFreq(clockName, frequency);
	}
	else if (clockName < END_OF_SYSTEM_CLKS)
	{
		status = CLOCK_SYS_GetSystemClockFreq(clockName, frequency);
	}
	else if (clockName < END_OF_PERIPHERAL_CLKS)
	{
		status = CLOCK_SYS_GetModuleClockFreq(clockName, frequency);
	}
	else
	{
		status = STATUS_UNSUPPORTED;
	}

	return status;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : CLOCK_SYS_Init
 * Description   : Install pre-defined clock configurations.
 * This function installs the pre-defined clock configuration table to the
 * clock manager.
 *
 * Implements CLOCK_SYS_Init_Activity
 *END**************************************************************************/
status_t CLOCK_SYS_Init(clock_manager_user_config_t const **clockConfigsPtr,
                              uint8_t configsNumber,
                              clock_manager_callback_user_config_t **callbacksPtr,
                              uint8_t callbacksNumber)
{
    //DEV_ASSERT(clockConfigsPtr != NULL);
    //DEV_ASSERT(callbacksPtr != NULL);

    g_clockState.configTable     = clockConfigsPtr;
    g_clockState.clockConfigNum  = configsNumber;
    g_clockState.callbackConfig  = callbacksPtr;
    g_clockState.callbackNum     = callbacksNumber;

    /*
     * errorCallbackIndex is the index of the callback which returns error
     * during clock mode switch. If all callbacks return success, then the
     * errorCallbackIndex is callbacksNumber.
     */
    g_clockState.errorCallbackIndex = callbacksNumber;

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : CLOCK_SYS_UpdateConfiguration
 * Description   : Send notification and change system clock configuration.
 * This function sends the notification to all callback functions, if all
 * callbacks return OK or forceful policy is used, this function will change
 * system clock configuration. The function should be called only on run mode.
 *
 * Implements CLOCK_SYS_UpdateConfiguration_Activity
 *END**************************************************************************/
status_t CLOCK_SYS_UpdateConfiguration(uint8_t targetConfigIndex,
                                                   clock_manager_policy_t policy)
{
    uint8_t callbackIdx;
    bool successfulSetConfig;           /* SetConfiguraiton status */
    status_t ret = STATUS_SUCCESS;
    const clock_manager_callback_user_config_t * callbackConfig;
    clock_notify_struct_t notifyStruct;

    //DEV_ASSERT(targetConfigIndex < g_clockState.clockConfigNum);       /* Clock configuration index is out of range. */

    notifyStruct.targetClockConfigIndex = targetConfigIndex;
    notifyStruct.policy                 = policy;

    /* Disable interrupts */
    INT_SYS_DisableIRQGlobal();
    /* Set errorcallbackindex as callbackNum, which means no callback error now.*/
    g_clockState.errorCallbackIndex = g_clockState.callbackNum;

    /* First step: Send "BEFORE" notification. */
    notifyStruct.notifyType = CLOCK_MANAGER_NOTIFY_BEFORE;

    /* Send notification to all callback. */
    for (callbackIdx=0; callbackIdx<g_clockState.callbackNum; callbackIdx++)
    {
        callbackConfig = g_clockState.callbackConfig[callbackIdx];
        if ((callbackConfig) &&
            (callbackConfig->callbackType != CLOCK_MANAGER_CALLBACK_AFTER))
        {
            if (STATUS_SUCCESS !=
                    (*callbackConfig->callback)(&notifyStruct,
                        callbackConfig->callbackData))
            {
                g_clockState.errorCallbackIndex = callbackIdx;

                if (CLOCK_MANAGER_POLICY_AGREEMENT == policy)
                {
                    /* Save the error callback index. */
                    ret = STATUS_MCU_NOTIFY_BEFORE_ERROR;
                    break;
                }
            }
        }
    }

    /* If all callback success or forceful policy is used. */
    if ((STATUS_SUCCESS == ret) ||
        (policy == CLOCK_MANAGER_POLICY_FORCIBLE))
    {
        /* clock mode switch. */
        ret = CLOCK_SYS_SetConfiguration(g_clockState.configTable[targetConfigIndex]);
        successfulSetConfig = (STATUS_SUCCESS == ret);
        g_clockState.curConfigIndex = targetConfigIndex;
    }
    else
    {
        /* Unsuccessful setConfiguration */
        successfulSetConfig = false;
    }

    if(successfulSetConfig){
        notifyStruct.notifyType = CLOCK_MANAGER_NOTIFY_AFTER;

        for (callbackIdx=0; callbackIdx<g_clockState.callbackNum; callbackIdx++)
        {
            callbackConfig = g_clockState.callbackConfig[callbackIdx];
            if ((callbackConfig) &&
                (callbackConfig->callbackType != CLOCK_MANAGER_CALLBACK_BEFORE))
            {
                if (STATUS_SUCCESS !=
                        (*callbackConfig->callback)(&notifyStruct,
                            callbackConfig->callbackData))
                {
                    g_clockState.errorCallbackIndex = callbackIdx;

                    if (CLOCK_MANAGER_POLICY_AGREEMENT == policy)
                    {
                        /* Save the error callback index. */
                        ret = STATUS_MCU_NOTIFY_AFTER_ERROR;
                        break;
                    }
                }
            }
        }
    }
    else /* Error occurs, need to send "RECOVER" notification. */
    {
        notifyStruct.notifyType = CLOCK_MANAGER_NOTIFY_RECOVER;
        for(;;)
        {
            callbackConfig = g_clockState.callbackConfig[callbackIdx];
            if (callbackConfig != NULL)
            {
                (void)(*callbackConfig->callback)(&notifyStruct,
                        callbackConfig->callbackData);
            }
            if(callbackIdx == 0U)
            {
                break;
            }
            callbackIdx--;
        }
    }

    /* Enable interrupts */
    INT_SYS_EnableIRQGlobal();

    return ret;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : CLOCK_SYS_SetConfiguration
 * Description   : This function sets the system to target configuration, it
 * only sets the clock modules registers for clock mode change, but not send
 * notifications to drivers.
 *
 * Implements CLOCK_SYS_SetConfiguration_Activity
 *END**************************************************************************/
status_t CLOCK_SYS_SetConfiguration(clock_manager_user_config_t const* config)
{
	return CLOCK_DRV_Init(config);
}

static status_t CLOCK_SYS_GetPll0PhiXFreq(uint32_t *frequency, uint32_t outputChannel)
{
    uint32_t multiplier = 0U, predivider;
    status_t status = STATUS_SUCCESS;

    if (1)
    {
    	predivider = 1;

    	if (predivider != 0U)
    	{
			/* Check the pll input reference */
			if (((uint32_t)PLL_REFERENCE_IRCOSC) == SIU_GetPLL0ClockSource(SIU))
			{
				/* Gets input frequency - IRCCOSC */
				(*frequency) = 16000000U;
			}
			else if (((uint32_t)PLL_REFERENCE_XOSC) == SIU_GetPLL0ClockSource(SIU))
			{
				/* Gets input frequency - XOSC */
				(*frequency) = g_xtal0ClkFreq;
			}
			else
			{
				/* Invalid input reference */
				//DEV_ASSERT(false);
			}

			(*frequency) /= predivider;  /* Pre-divider. */

    	}
    	else
    	{
    		(*frequency) = 0U;
    	}
    }
    else
    {
        /* This clock source is not enabled in current mode. */
        status = STATUS_MCU_GATED_OFF;
    }

    return status;

}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_GetPll1PhiXFreq
 * Description   : Gets PLL1PHIx frequency
 *END**************************************************************************/
static status_t CLOCK_SYS_GetPll1PhiXFreq(uint32_t *frequency, uint32_t outputChannel)
{
    //uint32_t multiplyFactor = 0U, fractionalDivider = 0U, outputDivider = 0U;
    status_t status = STATUS_SUCCESS;
    //uint32_t freq1 = 0U, freq2 = 0U, freq3 = 0U;

    if (1)
    {
        /* Check the pll input reference */
        if (((uint32_t)PLL_REFERENCE_XOSC) == SIU_GetPLL1ClockSource(SIU))
        {
            /* Gets input frequency - XOSC */
            (*frequency) = g_xtal0ClkFreq;
        }
        else if (((uint32_t)PLL_REFERENCE_PLL0_PHI1) == SIU_GetPLL1ClockSource(SIU))
        {
        	/* PLL0 reference */
        	if (((uint32_t)PLL_REFERENCE_IRCOSC) == SIU_GetPLL0ClockSource(SIU))
			{
				/* Gets input frequency - IRCCOSC */
				(*frequency) = 16000000U;
			}
			else if (((uint32_t)PLL_REFERENCE_XOSC) == SIU_GetPLL0ClockSource(SIU))
			{
				/* Gets input frequency - XOSC */
				(*frequency) = g_xtal0ClkFreq;
			}
			else
			{
				/* Invalid input reference */
				//DEV_ASSERT(false);
			}
        }
        else
        {
            /* Invalid input reference */
            //DEV_ASSERT(false);
        }
    }
    else
    {
        /* This clock source is not enabled in current mode. */
        status = STATUS_MCU_GATED_OFF;
    }

    return status;

}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_GetClockSourceFreq
 * Description   : Gets clock source frequency
 *END**************************************************************************/
static status_t CLOCK_SYS_GetClockSourceFreq(clock_names_t clockName,
                           uint32_t *frequency)
{
    status_t status = STATUS_SUCCESS;

    *frequency = 0U;

    switch(clockName)
    {
        case IRCOSC_CLK:
            (*frequency) = 16000000U;
            break;

        case XOSC_CLK:
            (*frequency) = g_xtal0ClkFreq;
            break;


        case PLL0_PHI0_CLK:
            status = CLOCK_SYS_GetPll0PhiXFreq(frequency, 0U);
            break;

        case PLL0_PHI1_CLK:
            status = CLOCK_SYS_GetPll0PhiXFreq(frequency, 1U);
            break;

        case PLL1_PHI0_CLK:
            status = CLOCK_SYS_GetPll1PhiXFreq(frequency, 0U);
            break;

        case SIPI0_CLK:
        	 (*frequency) = g_sipiRefClkFreq;
        	 break;

        default:
            /* This clock source is not supported. */
            status = STATUS_UNSUPPORTED;
            break;
    }
    return status;
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_GetFMPerDiv
 * Description   : Gets FM perdiv frequency
 *END**************************************************************************/
static uint32_t CLOCK_SYS_GetFMPerDiv(void)
{
	uint32_t divider = SIU_GetFMPerDiv(SIU);
	uint32_t ret = 0U;

	switch(divider)
	{
	case (uint32_t)SIU_PBRIDGE_DIV_BY_2:
		ret = 2U;
		break;
	case (uint32_t)SIU_PBRIDGE_DIV_BY_4:
		ret = 4U;
		break;
	case (uint32_t)SIU_PBRIDGE_DIV_BY_8:
		ret = 8U;
		break;
	default:
		//DEV_ASSERT(false);
		break;
	}

	return ret;
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_GetPerDiv
 * Description   : Gets perdiv frequency
 *END**************************************************************************/
static uint32_t CLOCK_SYS_GetPerDiv(void)
{
	uint32_t divider = SIU_GetPerDiv(SIU);
	uint32_t ret = 0U;

	switch(divider)
	{
	case (uint32_t)SIU_PERDIV_DIV_BY_2:
		ret = 2U;
		break;
	case (uint32_t)SIU_PERDIV_DIV_BY_4:
		ret = 4U;
		break;
	case (uint32_t)SIU_PERDIV_DIV_BY_8:
		ret = 8U;
		break;
	default:
		//DEV_ASSERT(false);
		break;
	}

	return ret;
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_GetSysClkDivider
 * Description   : Gets system clock divider
 *END**************************************************************************/
static uint32_t CLOCK_SYS_GetSysClkDivider(void)
{
	uint32_t divider = SIU_GetSysClkDivider(SIU);
	uint32_t ret = 0U;

	switch(divider)
	{
	case (uint32_t)SIU_SYSCLK_DIV_1:
		ret = 1U;
		break;
	case (uint32_t)SIU_SYSCLK_DIV_2:
		ret = 2U;
		break;
	case (uint32_t)SIU_SYSCLK_DIV_4:
		ret = 4U;
		break;
	case (uint32_t)SIU_SYSCLK_DIV_8:
		ret = 8U;
		break;
	case (uint32_t)SIU_SYSCLK_DIV_16:
		ret = 16U;
		break;
	default:
		//DEV_ASSERT(false);
		break;
	}

	return ret;
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_GetPerClkSelFreq
 * Description   : Gets peripheral clock selector frequency
 *END**************************************************************************/
static status_t CLOCK_SYS_GetPerClkSelFreq(uint32_t *frequency)
{
	status_t stat = STATUS_SUCCESS;
	uint32_t scsClkFreq = 0U;

	/* Get selector */
	uint32_t selector = SIU_GetPerClkSel(SIU);

    switch(selector)
    {
    case 0:
    	/* Get CORE_CLK frequency */
	    stat = CLOCK_SYS_GetSystemClockFreq(CORE_CLK, &scsClkFreq);
	    *frequency = scsClkFreq;
	    break;
    case 1:
    	/* Get PLL0_PHI0 frequency */
        stat = CLOCK_SYS_GetClockSourceFreq(PLL0_PHI0_CLK, &scsClkFreq);
	    *frequency = scsClkFreq;
	    break;
    default:
	    //DEV_ASSERT(false);
	    stat = STATUS_ERROR;
	    break;
    }

    return stat;
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_GetSystemClockFreq
 * Description   : Gets system clock frequency
 *END**************************************************************************/
static status_t CLOCK_SYS_GetSystemClockFreq(clock_names_t clockName,
                           uint32_t *frequency)
{
    status_t status = STATUS_SUCCESS;
    uint32_t currentSysClk, scsClkFreq = 0U, divider = 0U;
    *frequency = 0U;

    /* Get System Clock from SIU */
    currentSysClk = SIU_GetCurrentSystemClock(SIU);

    switch(currentSysClk)
    {
        case ((uint32_t)SIU_SYSCLKSEL_IRCOSC):
        	/* Get IRCOSC frequency */
            status = CLOCK_SYS_GetClockSourceFreq(IRCOSC_CLK,&scsClkFreq);
            break;
        case ((uint32_t)SIU_SYSCLKSEL_XOSC):
			/* Get XOSC frequency */
            status = CLOCK_SYS_GetClockSourceFreq(XOSC_CLK,&scsClkFreq);
            break;
        case ((uint32_t)SIU_SYSCLKSEL_PLL1):
			/* Get PLL1_PHI0 frequency */
            status = CLOCK_SYS_GetClockSourceFreq(PLL1_PHI0_CLK,&scsClkFreq);
            break;
        case ((uint32_t)SIU_SYSCLKSEL_PLL0):
			/* Get PLL0_PHI0 frequency */
            status = CLOCK_SYS_GetClockSourceFreq(PLL0_PHI0_CLK,&scsClkFreq);
            break;
        default:
            /* Invalid clock source for system clock */
            //DEV_ASSERT(false);
            status = STATUS_ERROR;
            break;
    }

    if (status == STATUS_SUCCESS)
    {
    	if (clockName == SCS_CLK)
    	{
    		*frequency = scsClkFreq;
    	}
        /* System clock and core clock are the same. */
    	else if ((clockName == SYS_CLK) || (clockName == CORE_CLK))
        {
    		divider = CLOCK_SYS_GetSysClkDivider();
    		scsClkFreq /= divider;
            *frequency = scsClkFreq;
        }
        else if (clockName == PLAT_CLK)
        {
        	divider = CLOCK_SYS_GetSysClkDivider();
        	scsClkFreq /= divider;
            *frequency = scsClkFreq / 2U;
        }
        else if (clockName == PBRIDGEx_CLK)
        {
        	divider = CLOCK_SYS_GetSysClkDivider();
        	scsClkFreq /= divider;
        	*frequency = scsClkFreq;
        	divider = CLOCK_SYS_GetFMPerDiv();
        	*frequency = *frequency / divider;
        }
        else if (clockName == PER_CLK_SEL_CLK)
        {
        	status = CLOCK_SYS_GetPerClkSelFreq(frequency);
        }
        else if(clockName == PER_CLK)
        {
           status = CLOCK_SYS_GetPerClkSelFreq(frequency);
		   divider = CLOCK_SYS_GetPerDiv();
		   *frequency = *frequency / divider;

	    }
        else
        {
            //DEV_ASSERT(false);
            status = STATUS_ERROR;
        }
    }
    return status;
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_GetModuleClockFreq
 * Description   : Gets module clock frequency
 *END**************************************************************************/
static status_t CLOCK_SYS_GetModuleClockFreq(clock_names_t clockName,uint32_t *frequency)
{
    status_t status = STATUS_SUCCESS;
    uint32_t interfaceClock = 0U;

    /* If clock name is not valid, also the interface clock is not valid */
    if (haltClocks[clockName] == 0U)
    {
        status = STATUS_UNSUPPORTED;
    }
    else
    {
        /* Check interface clock */
    	status = CLOCK_SYS_GetFreq(interfaceClocks[clockName],&interfaceClock);

    	if (haltRegisterClocks[clockName] != 0U)
    	{
    		if ((!SIU_GetModuleStatus(SIU, haltRegisterClocks[clockName], haltBitfieldClocks[clockName], haltMaskClocks[clockName]) || (interfaceClock == 0U)))
        	{
            	status = STATUS_MCU_GATED_OFF;
        	}
    	}
    }

    if (status == STATUS_SUCCESS)
    {
        /* It will never reach this point because CLOCK_NAME_COUNT is an invalid clock name */
        if (clockName == CLOCK_NAME_COUNT)
        {
            status = STATUS_UNSUPPORTED;
        }
        /* Module clocks  */
        else if (clockName < END_OF_PLAT_CLKS)
        {
            status = CLOCK_SYS_GetSystemClockFreq(PLAT_CLK,frequency);
        }
        else if (clockName < END_OF_PBRIDGEx_CLKS)
        {
            status = CLOCK_SYS_GetSystemClockFreq(PBRIDGEx_CLK,frequency);
        }
        else if (clockName < END_OF_PER_CLK_SEL_CLKS)
        {
            status = CLOCK_SYS_GetPerClkSelClockFreq(clockName,frequency);
        }
        else if (clockName < END_OF_PER_CLKS)
        {
            status = CLOCK_SYS_GetSystemClockFreq(PER_CLK,frequency);
        }
        else if (clockName < END_OF_LFAST_CLKS)
        {
            status = CLOCK_SYS_GetLfastClockFreq(clockName, frequency);
        }
        else if (clockName < END_OF_CAN_CLKS)
        {
            status = CLOCK_SYS_GetMCANClockFreq(clockName,frequency);
        }
        else if (clockName < END_OF_XOSC_CLK_CLKS)
		{
			status = CLOCK_SYS_GetClockSourceFreq(XOSC_CLK,frequency);
		}
        else if (clockName < END_OF_PERIPHERAL_CLKS)
        {
            /* These modules don't have a protocol clock, output frequency is zero */
            status = STATUS_SUCCESS;
        }
        else
        {
            status = STATUS_UNSUPPORTED;
        }
    }

    return status;
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_GetMCANClockFreq
 * Description   : Gets MCAN clock frequency
 *END**************************************************************************/
static status_t CLOCK_SYS_GetMCANClockFreq(clock_names_t clockName,
                           uint32_t *frequency)
{
    status_t status = STATUS_SUCCESS;
    uint32_t selector = 0U, divider = 0U;
    (void) clockName;
    (void)divider;

    /* Get selector */
    selector = SIU_GetMCANSel(SIU);

    switch(selector)
    {
        case SIU_MCAN_CLK_SEL_XOSC_CLK:
        	/* Get XOSC frequency */
            status = CLOCK_SYS_GetClockSourceFreq(XOSC_CLK,frequency);
            break;
        case SIU_MCAN_CLK_SEL_PER_CLK:
        	/* Get PER_CLK frequency */
            status = CLOCK_SYS_GetSystemClockFreq(PER_CLK,frequency);
            break;
        default:
            /* There is no support in CGM for FLECAN clock selection */
            *frequency = 0U;
            break;
    }

    return status;
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_GetEtpuDividerValue
 * Description   : Gets ETPU divider
 *END**************************************************************************/
static uint32_t CLOCK_SYS_GetEtpuDividerValue(void)
{
	uint32_t divider = 1;
	uint32_t ret = 0U;

	switch(divider)
	{
	case (uint32_t)SIU_ETPU_DIV_BY_1:
		ret = 1U;
		break;
	case (uint32_t)SIU_ETPU_DIV_BY_2:
		ret = 2U;
		break;
	default:
		//DEV_ASSERT(false);
		break;
	}

	return ret;
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_GetPerClkSelClockFreq
 * Description   : Gets peripheral clock selector clock frequency
 *END**************************************************************************/
static status_t CLOCK_SYS_GetPerClkSelClockFreq(clock_names_t clockName,
                           uint32_t *frequency)
{
    status_t status = STATUS_SUCCESS;
    uint32_t divider = 0U;

    /* Get PER_CLK_SEL_CLK frequency */
    status = CLOCK_SYS_GetSystemClockFreq(PER_CLK_SEL_CLK,frequency);

    switch(clockName)
    {
        case ADCSD1_CLK:
        case ADCSD2_CLK:
        case ADCSD3_CLK:
        case ADCSD4_CLK:
        	divider = 1U;
        break;
        case ETPU0_CLK:
        case ETPU1_CLK:
        	divider = CLOCK_SYS_GetEtpuDividerValue();
        break;
        case PSI5RX0_CLK:
        case PSI5RX1_CLK:
        	divider =  1U;
        break;
        case PSI51us0_CLK:
        case PSI51us1_CLK:
        	divider =  1U;
        break;
        default:
        	status = STATUS_ERROR;
            //DEV_ASSERT(false);
            break;
    }
        *frequency /= divider;

    return status;
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_GetLfastClockFreq
 * Description   : Gets LFAST clock frequency
 *END**************************************************************************/
static status_t CLOCK_SYS_GetLfastClockFreq(clock_names_t clockName, uint32_t *frequency)
{
#if 0 
    status_t status = STATUS_SUCCESS;
    uint32_t selector = 0U, divider = 0U;
    (void) clockName;
    (void)divider;

    /* Get selector */
    selector = SIU_GetLfastClkSel(SIU);

    switch(selector)
    {
        case SIU_LFASTx_SEL_XOSC:
        	/* Get XOSC frequency */
            status = CLOCK_SYS_GetClockSourceFreq(XOSC_CLK, frequency);
        break;

        case SIU_LFASTx_SEL_SIPI_REF_CLK:
        	/* Get SIPI0 frequency */
            status = CLOCK_SYS_GetClockSourceFreq(SIPI0_CLK, frequency);
        break;
        case SIU_LFASTx_SEL_PER_CLK:
        	/* Get PER_CLK frequency */
        	status = CLOCK_SYS_GetSystemClockFreq(PER_CLK, frequency);
        break;
        default:
            //DEV_ASSERT(false);
            break;
    }

    divider = 1U;
    *frequency /= divider;

    return status;
#endif 

	while(1);
	return STATUS_SUCCESS;

}


/*FUNCTION**********************************************************************
 *
 * Function Name : CLOCK_SYS_GetFreq
 * Description   : Wrapper over CLOCK_DRV_GetFreq function. It's part of the old API.
 *
 * Implements CLOCK_SYS_GetFreq_Activity
 *END**************************************************************************/
status_t CLOCK_SYS_GetFreq(clock_names_t clockName,
                           uint32_t *frequency)
{
	return CLOCK_DRV_GetFreq(clockName,frequency);
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_ConfigureClockSources
 * Description   : Configures clock sources
 *END**************************************************************************/
static status_t CLOCK_SYS_ConfigureClockSources(clock_manager_user_config_t const* config)
{
    g_xtal0ClkFreq = config->clockSourcesConfig.xosc0Config.freq;
    g_sipiRefClkFreq = config->clockSourcesConfig.sipiRefClkFreq0;

    /* Configure PLL0 */
    CLOCK_SYS_SetPll(&config->clockSourcesConfig.pll0Config, config->siuConfig.pll0Reference, 0U);

    /* Configure PLL1 */
    CLOCK_SYS_SetPll(&config->clockSourcesConfig.pll1Config, config->siuConfig.pll1Reference, 1U);

    return STATUS_SUCCESS;
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_ResetMcMeConfiguration
 * Description   : Clears peripheral clock gating and all clock sources. Only an IRC
 * type clock source is enabled and set as system clock source.
 *END**************************************************************************/
static status_t CLOCK_SYS_ResetConfiguration(void)
{
    status_t retValue = STATUS_SUCCESS;
    uint8_t i = 0U;

    /* Clear halt registers */
    SIU_ClearHaltRegisters(SIU);

    /* Set IRCOSC as system clock */
    SIU_SetSourceClock(SIU, (uint32_t)SIU_SYSCLKSEL_IRCOSC);


    return retValue;
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_SetClockSource
 * Description   : Set clock source
 *END**************************************************************************/
static void CLOCK_SYS_SetClockSource(siu_clk_sel_t scs)
{
	switch(scs)
	{
	case SIU_SYSTEM_CLOCK_SRC_IRCOSC:
		/* Set IRCOSC */
		SIU_SetSourceClock(SIU, (uint32_t)SIU_SYSCLKSEL_IRCOSC);
		break;

	case SIU_SYSTEM_CLOCK_SRC_XOSC:
		/* Set XOSC */
		SIU_SetSourceClock(SIU, (uint32_t)SIU_SYSCLKSEL_XOSC);
		break;

	case SIU_SYSTEM_CLOCK_SRC_PLL0_PHI0:
		/* Set PLL0 */
		SIU_SetSourceClock(SIU, (uint32_t)SIU_SYSCLKSEL_PLL0);
		break;

	case SIU_SYSTEM_CLOCK_SRC_PLL1_PHI0:
		/* Set PLL1 */
		SIU_SetSourceClock(SIU, (uint32_t)SIU_SYSCLKSEL_PLL1);
		break;

	default:
		//DEV_ASSERT(false);
		break;
	}
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_SetPeripheralClockSelect
 * Description   : Set peripheral clock selector
 *END**************************************************************************/
static void CLOCK_SYS_SetPeripheralClockSelect(siu_clk_sel_t perClkSel)
{
	switch(perClkSel)
	{
	case SIU_PER_CLK_SEL_CORE_CLK:
		SIU_SetPeripheralClock(SIU, (uint32_t)SIU_PERCLKSEL_SYSCLKDIV);
		break;
	case SIU_PER_CLK_SEL_PLL0_PHI0:
		SIU_SetPeripheralClock(SIU, (uint32_t)SIU_PERCLKSEL_PLL0);
		break;
	default:
		//DEV_ASSERT(false);
		break;
	}
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_SetMCAN
 * Description   : Set MCAN clock
 *END**************************************************************************/
static void CLOCK_SYS_SetMCAN(siu_clk_sel_t mcanSel)
{
	switch(mcanSel)
	{
	case SIU_MCAN_CLK_SEL_XOSC_CLK:
		SIU_SetMCANClock(SIU, SIU_MCAN_CLK_SEL_XOSC_CLK);
		break;

	case SIU_MCAN_CLK_SEL_PER_CLK:
		SIU_SetMCANClock(SIU, SIU_MCAN_CLK_SEL_PER_CLK);
		break;

	default:
		//DEV_ASSERT(false);
		break;
	}
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_SetClockSourceDivider
 * Description   : Set clock source divider
 *END**************************************************************************/
static void CLOCK_SYS_SetClockSourceDivider(siu_clk_div_t coreClk)
{
	switch(coreClk)
	{
	case SIU_CLOCK_DIV_BY_1:
		SIU_SetClockSourceDivider(SIU, (uint32_t)SIU_SYSCLK_DIV_1);
		break;

	case SIU_CLOCK_DIV_BY_2:
		SIU_SetClockSourceDivider(SIU, (uint32_t)SIU_SYSCLK_DIV_2);
		break;

	case SIU_CLOCK_DIV_BY_4:
		SIU_SetClockSourceDivider(SIU, (uint32_t)SIU_SYSCLK_DIV_4);
		break;

	case SIU_CLOCK_DIV_BY_8:
		SIU_SetClockSourceDivider(SIU, (uint32_t)SIU_SYSCLK_DIV_8);
		break;

	case SIU_CLOCK_DIV_BY_16:
		SIU_SetClockSourceDivider(SIU, (uint32_t)SIU_SYSCLK_DIV_16);
		break;

	default:
		//DEV_ASSERT(false);
		break;
	}
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_SetPbridgeClock
 * Description   : Set PBRIDGE_CLK
 *END**************************************************************************/
static void CLOCK_SYS_SetPbridgeClock(siu_clk_div_t pbridgeClk)
{
	switch(pbridgeClk)
	{
	case SIU_CLOCK_DIV_BY_2:
		SIU_SetPbridgeClock(SIU, (uint32_t)SIU_PBRIDGE_DIV_BY_2);
		break;

	case SIU_CLOCK_DIV_BY_4:
		SIU_SetPbridgeClock(SIU, (uint32_t)SIU_PBRIDGE_DIV_BY_4);
		break;

	case SIU_CLOCK_DIV_BY_8:
		SIU_SetPbridgeClock(SIU, (uint32_t)SIU_PBRIDGE_DIV_BY_8);
		break;

	default:
		//DEV_ASSERT(false);
		break;
	}
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_SetPerClock
 * Description   : Set PER_CLK
 *END**************************************************************************/
static void CLOCK_SYS_SetPerClock(siu_clk_div_t perClkSel)
{
	switch (perClkSel)
	{
	case SIU_CLOCK_DIV_BY_2:
		SIU_SetPerClock(SIU, (uint32_t)SIU_PERDIV_DIV_BY_2);
		break;

	case SIU_CLOCK_DIV_BY_4:
		SIU_SetPerClock(SIU, (uint32_t)SIU_PERDIV_DIV_BY_4);
		break;

	case SIU_CLOCK_DIV_BY_8:
		SIU_SetPerClock(SIU, (uint32_t)SIU_PERDIV_DIV_BY_8);
		break;

	default:
		//DEV_ASSERT(false);
		break;
	}
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_SetEtpuClk
 * Description   : Set ETPU clock
 *END**************************************************************************/
static void CLOCK_SYS_SetEtpuClk(siu_clk_div_t etpuClk)
{
	switch (etpuClk)
	{
	case SIU_CLOCK_DIV_BY_2:
		SIU_SetEtpuClock(SIU, (uint32_t)SIU_ETPU_DIV_BY_2);
		break;

	case SIU_CLOCK_DIV_BY_1:
		SIU_SetEtpuClock(SIU, (uint32_t)SIU_ETPU_DIV_BY_1);
		break;

	default:
		//DEV_ASSERT(false);
		break;
	}
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_SetSIUSysdiv
 * Description   : Set SIU_SYSDIV
 *END**************************************************************************/
static void CLOCK_SYS_SetSIUSysdiv(clock_manager_user_config_t const* config)
{
	/* Configure SYSCLKSEL register */
	CLOCK_SYS_SetClockSource(config->siuConfig.scs);


	/* Configure SYSCLKDIV register */
	CLOCK_SYS_SetClockSourceDivider(config->siuConfig.coreClk);


	/* Configure FMPERDIV register */
	CLOCK_SYS_SetPbridgeClock(config->siuConfig.pbridgeClk);


	/* Configure PERCLKSEL register */
	CLOCK_SYS_SetPeripheralClockSelect(config->siuConfig.perClkSel);


	/* Configure PERDIV register */
	CLOCK_SYS_SetPerClock(config->siuConfig.perClk);

	/* Configure ETPUDIV register */
	CLOCK_SYS_SetEtpuClk(config->siuConfig.etpuClk);


	/* Configure MCANSEL register */
	CLOCK_SYS_SetMCAN(config->siuConfig.mcanSel);
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_SetLFASTClock
 * Description   : Set LFAST clock
 *END**************************************************************************/
static status_t CLOCK_SYS_SetLFASTClock(siu_clk_sel_t lfastSel, siu_clk_div_t lfastClkDiv)
{
	status_t ret = STATUS_SUCCESS;

	switch(lfastSel)
	{
	case SIU_LFASTx_SEL_XOSC:
		SIU_SetLFASTClock(SIU, (uint32_t)SIU_LFASTx_XOSC);
		break;

	case SIU_LFASTx_SEL_SIPI_REF_CLK:
		SIU_SetLFASTClock(SIU, (uint32_t)SIU_LFASTx_SIPI_REFCLK);
		break;

	case SIU_LFASTx_SEL_PER_CLK:
		SIU_SetLFASTClock(SIU, (uint32_t)SIU_LFASTx_PLL);
		break;

	default:
		ret = STATUS_ERROR;
		//DEV_ASSERT(false);
		break;
	}

	if (ret == STATUS_SUCCESS)
	{
		SIU_SetLFASTDiv(SIU, (uint32_t)lfastClkDiv);
	}

	return ret;
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_SetSigmaDeltaClockDivider
 * Description   : Set Sigma - Delta clock divider
 *END**************************************************************************/
static void CLOCK_SYS_SetSigmaDeltaClockDivider(siu_clk_div_t adcsdClk)
{
	SIU_SetSigmaDeltaClockDivider(SIU, (uint32_t)adcsdClk);
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_SetHALT
 * Description   : Set HALT registers
 *END**************************************************************************/
static void CLOCK_SYS_SetHALT(clock_manager_user_config_t const* config)
{
	SIU_SetCSE(SIU, config->siuConfig.SIU_CSE);
	SIU_SetETPUC(SIU, config->siuConfig.SIU_ETPUC);
	SIU_SetETPUA(SIU, config->siuConfig.SIU_ETPUA);
	SIU_SetNPC(SIU, config->siuConfig.SIU_NPC);
	SIU_SetEBI(SIU, config->siuConfig.SIU_EBI);
	SIU_SetEQADCB(SIU, config->siuConfig.SIU_EQADCB);
	SIU_SetEQADCA(SIU, config->siuConfig.SIU_EQADCA);
	SIU_SetEMIOS0(SIU, config->siuConfig.SIU_EMIOS0);
	SIU_SetDECFIL(SIU, config->siuConfig.SIU_DECFIL);
	SIU_SetEMIOS1(SIU, config->siuConfig.SIU_EMIOS1);
	SIU_SetPIT(SIU, config->siuConfig.SIU_PIT);
	SIU_SetFLEXCAND(SIU, config->siuConfig.SIU_FLEXCAND);
	SIU_SetFLEXCANC(SIU, config->siuConfig.SIU_FLEXCANC);
	SIU_SetFLEXCANB(SIU, config->siuConfig.SIU_FLEXCANB);
	SIU_SetFLEXCANA(SIU, config->siuConfig.SIU_FLEXCANA);
	SIU_SetDSPID(SIU, config->siuConfig.SIU_DSPID);
	SIU_SetDSPIC(SIU, config->siuConfig.SIU_DSPIC);
	SIU_SetDSPIB(SIU, config->siuConfig.SIU_DSPIB);
	SIU_SetDSPIA(SIU, config->siuConfig.SIU_DSPIA);
	SIU_SetDSPIE(SIU, config->siuConfig.SIU_DSPIE);
	SIU_SetESCIF(SIU, config->siuConfig.SIU_ESCIF);
	SIU_SetESCIE(SIU, config->siuConfig.SIU_ESCIE);
	SIU_SetESCID(SIU, config->siuConfig.SIU_ESCID);
	SIU_SetESCIC(SIU, config->siuConfig.SIU_ESCIC);
	SIU_SetESCIB(SIU, config->siuConfig.SIU_ESCIB);
	SIU_SetESCIA(SIU, config->siuConfig.SIU_ESCIA);
	//SIU_SetFEC(SIU, config->siuConfig.SIU_ENET);
	//SIU_SetSDD(SIU, config->siuConfig.SIU_SDD);
	//SIU_SetSDC(SIU, config->siuConfig.SIU_SDC);
	//SIU_SetSDB(SIU, config->siuConfig.SIU_SDB);
	//SIU_SetSDA(SIU, config->siuConfig.SIU_SDA);
	//SIU_SetSIPI(SIU, config->siuConfig.SIU_SIPI);
	//SIU_SetCRC(SIU, config->siuConfig.SIU_CRC);
	//SIU_SetSTCU(SIU, config->siuConfig.SIU_STCU);
	//SIU_SetSRX1(SIU, config->siuConfig.SIU_SRX1);
	//SIU_SetSRX0(SIU, config->siuConfig.SIU_SRX0);
	//SIU_SetPSI5B(SIU, config->siuConfig.SIU_PSI5B);
	//SIU_SetPSI5A(SIU, config->siuConfig.SIU_PSI5A);
	//SIU_SetMCANB(SIU, config->siuConfig.SIU_MCANB);
	//SIU_SetMCANA(SIU, config->siuConfig.SIU_MCANA);
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_SetPSI5
 * Description   : Set PSI5 clock
 *END**************************************************************************/
static void CLOCK_SYS_SetPSI5(clock_manager_user_config_t const* config)
{
	//SIU_SetPSDIV(SIU, (uint32_t)config->siuConfig.psi5Rx);
	//SIU_SetPSDIV1M(SIU, (uint32_t)config->siuConfig.psi5Rx1M);
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_SetExternalClockControl
 * Description   : Set external clock control
 *END**************************************************************************/
static void CLOCK_SYS_SetExternalClockControl(clock_manager_user_config_t const* config)
{
	SIU_SetEngineeringClockDivisionFactor(SIU, (uint32_t)config->siuConfig.engClkout);
	SIU_SetEngineeringClockSource(SIU, (uint32_t)config->siuConfig.engClkoutSel);
	SIU_SetExternalBusDivisionFactor(SIU, (uint32_t)config->siuConfig.clkout);
}


/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_ConfigureCgm
 * Description   : Configures clock generation module
 *END**************************************************************************/
static status_t CLOCK_SYS_ConfigureSiu(clock_manager_user_config_t const* config)
{

	status_t ret;

    /* Set SIU_SYSDIV register (SYSCLKSEL,SYSCLKDIV,FMPERDIV,PERCLKSEL,PERDIV,ETPUDIV, MCANSEL) */
	CLOCK_SYS_SetSIUSysdiv(config);

	/* Set SIU_SDCLKCFG register */
	CLOCK_SYS_SetSigmaDeltaClockDivider(config->siuConfig.adcsdClk);

	/* Set SIU_PSCLKCFG register */
	CLOCK_SYS_SetPSI5(config);

	/* Set SIU_LFCLKCFG register */
	ret = CLOCK_SYS_SetLFASTClock(config->siuConfig.lfastSel, config->siuConfig.lfastClk);

	if (ret == STATUS_SUCCESS)
	{
		/* Set SIU_ECCR register */
		CLOCK_SYS_SetExternalClockControl(config);

		/* Set SIU_HALT1 and SIU_HALT2 registers */
		CLOCK_SYS_SetHALT(config);
	}

    return ret;
}



/*FUNCTION**********************************************************************
 * Function Name : CLOCK_SYS_SetPll
 * Description   : Configures ARM PLL clock source
 *END**************************************************************************/
static void CLOCK_SYS_SetPll(cs_plldig_config_t const* config, siu_clk_sel_t const siuConfig, uint32_t instance)
{
      uint32_t phi0Divider = 0U, phi1Divider = 0U;

      /* Set PLL0 and PLL1 references */
      switch(instance)
      {
          case 0U:
           	SIU_SetPLL0(SIU, siuConfig);
          break;

          case 1U:
          	SIU_SetPLL1(SIU, siuConfig);
          break;

          default:
              /* Invalid PLL instance */
              //DEV_ASSERT(false);
              break;
      }

      phi0Divider = ((uint32_t)config->phi0Divider) + 1U;
      phi1Divider = ((uint32_t)config->phi1Divider) + 1U;

}



/*! @}*/

/*******************************************************************************
 * EOF
 ******************************************************************************/

