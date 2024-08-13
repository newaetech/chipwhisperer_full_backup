/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
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

#if !defined(CGM_HW_ACCESS_H)
#define CGM_HW_ACCESS_H

#include "MPC5676R.h"
#include "MPC5676R_features.h"
#include <stdbool.h>
#include <stddef.h>

/*!
 * @file clock_siu_hw_access.h
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 */

/*!
 * @ingroup clock_siu_hw_access
 * @defgroup clock_siu_hw_access
 * @{
 */




#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*!
 * @brief Gets current system clock
 */
static inline uint32_t SIU_GetCurrentSystemClock(const SIU_Type * base)
{
     /* Get System Clock from CGM */
    return (base->SYSDIV & SIU_SYSDIV_SYSCLKSEL_MASK) >> SIU_SYSDIV_SYSCLKSEL_SHIFT;
}

/*!
 * @brief Gets SIU System Clock Divider
 */
static inline uint32_t SIU_GetSysClkDivider(const SIU_Type * base)
{
    return ((base->SYSDIV & SIU_SYSDIV_SYSCLKDIV_MASK) >> SIU_SYSDIV_SYSCLKDIV_SHIFT);
}

/*!
 * @brief Gets SIU Peripheral Clock Selector
 */
static inline uint32_t SIU_GetPerClkSel(const SIU_Type * base)
{
	return ((base->SYSDIV & SIU_SYSDIV_PERCLKSEL_MASK) >> SIU_SYSDIV_PERCLKSEL_SHIFT);
}

/*!
 * @brief Gets SIU Peripheral Divider
 */
static inline uint32_t SIU_GetPerDiv(const SIU_Type * base)
{
	return ((base->SYSDIV & SIU_SYSDIV_PERDIV_MASK) >> SIU_SYSDIV_PERDIV_SHIFT);
}

/*!
 * @brief Gets SIU FM Peripheral Divider
 */
static inline uint32_t SIU_GetFMPerDiv(const SIU_Type * base)
{
	return ((base->SYSDIV & SIU_SYSDIV_FMPERDIV_MASK) >> SIU_SYSDIV_FMPERDIV_SHIFT);
}


/*!
 * @brief Gets SIU MCAN Select
 */
static inline uint32_t SIU_GetMCANSel(const SIU_Type * base)
{
	return ((base->SYSDIV & SIU_SYSDIV_MCANSEL_MASK) >> SIU_SYSDIV_MCANSEL_SHIFT);
}


/*!
 * @brief Gets PLL0 clock source
 */
static inline uint32_t SIU_GetPLL0ClockSource(const SIU_Type * base)
{
	return ((base->SYSDIV & SIU_SYSDIV_PLL0SEL_MASK) >> SIU_SYSDIV_PLL0SEL_SHIFT);
}

/*!
 * @brief Gets PLL1 clock source
 */
static inline uint32_t SIU_GetPLL1ClockSource(const SIU_Type * base)
{
	return ((base->SYSDIV & SIU_SYSDIV_PLL1SEL_MASK) >> SIU_SYSDIV_PLL1SEL_SHIFT);
}

/*!
 * This function clears the halt registers
 */
static inline void SIU_ClearHaltRegisters(SIU_Type * base)
{
    base->HLT1 &= SIU_HLT1_CORE0_MASK | SIU_HLT1_CORE1_MASK | SIU_HLT1_CSE_MASK | SIU_HLT1_ETPUC_MASK | SIU_HLT1_ETPUA_MASK | SIU_HLT1_NPC_MASK | SIU_HLT1_EBI_MASK | SIU_HLT1_EQADCB_MASK
    		| SIU_HLT1_EQADCA_MASK | SIU_HLT1_EMIOS0_MASK | SIU_HLT1_DECFIL_MASK | SIU_HLT1_EMIOS1_MASK | SIU_HLT1_PIT_MASK | SIU_HLT1_FLEXCAND_MASK
			| SIU_HLT1_FLEXCANC_MASK | SIU_HLT1_FLEXCANB_MASK | SIU_HLT1_FLEXCANA_MASK | SIU_HLT1_DSPID_MASK | SIU_HLT1_DSPIC_MASK
			|SIU_HLT1_DSPIB_MASK | SIU_HLT1_DSPIA_MASK | SIU_HLT1_DSPIE_MASK | SIU_HLT1_ESCIF_MASK | SIU_HLT1_ESCIE_MASK | SIU_HLT1_ESCID_MASK
			| SIU_HLT1_ESCIC_MASK | SIU_HLT1_ESCIB_MASK | SIU_HLT1_ESCIA_MASK;
}

/*!
 * @brief Sets system clock source
 */
static inline void SIU_SetSourceClock(SIU_Type * base, uint32_t clockSource)
{
	uint32_t regValue = base->SYSDIV;
	regValue &= (uint32_t)(~(SIU_SYSDIV_SYSCLKSEL_MASK));
	regValue |= SIU_SYSDIV_SYSCLKSEL(clockSource);
	base->SYSDIV = regValue;
}

/*!
 * @brief Sets clock source divider
 */
static inline void SIU_SetClockSourceDivider(SIU_Type * base, uint32_t clockDiv)
{
	uint32_t regValue = base->SYSDIV;
	regValue &= (uint32_t)(~(SIU_SYSDIV_SYSCLKDIV_MASK));
	regValue |= SIU_SYSDIV_SYSCLKDIV(clockDiv);
	base->SYSDIV = regValue;
}

/*!
 * @brief Sets FM clock peripheral divider
 */
static inline void SIU_SetPbridgeClock(SIU_Type * base, uint32_t clockDiv)
{
	uint32_t regValue = base->SYSDIV;
	regValue &= (uint32_t)(~(SIU_SYSDIV_FMPERDIV_MASK));
	regValue |= SIU_SYSDIV_FMPERDIV(clockDiv);
	base->SYSDIV = regValue;
}

/*!
 * @brief Sets non-FM clock peripheral divider
 */
static inline void SIU_SetPerClock(SIU_Type * base, uint32_t clockDiv)
{
	uint32_t regValue = base->SYSDIV;
	regValue &= (uint32_t)(~(SIU_SYSDIV_PERDIV_MASK));
	regValue |= SIU_SYSDIV_PERDIV(clockDiv);
	base->SYSDIV = regValue;
}

/*!
 * @brief Sets eTPU clock divider
 */
static inline void SIU_SetEtpuClock(SIU_Type * base, uint32_t clockDiv)
{
	uint32_t regValue = base->SYSDIV;
	regValue &= (uint32_t)(~(SIU_SYSDIV_ETPUDIV_MASK));
	regValue |= SIU_SYSDIV_ETPUDIV(clockDiv);
	base->SYSDIV = regValue;
}

/*!
 * @brief Sets peripheral clock source
 */
static inline void SIU_SetPeripheralClock(SIU_Type * base, uint32_t clockSource)
{
	uint32_t regValue = base->SYSDIV;
	regValue &= (uint32_t)(~(SIU_SYSDIV_PERCLKSEL_MASK));
	regValue |= SIU_SYSDIV_PERCLKSEL(clockSource);
	base->SYSDIV = regValue;
}

/*!
 * @brief Sets engineering clock division factor
 */
static inline void SIU_SetEngineeringClockDivisionFactor(SIU_Type * base, uint32_t clockSource)
{
	uint32_t regValue = base->ECCR;
	regValue &= (uint32_t)(~(SIU_ECCR_ENGDIV_MASK));
	regValue |= SIU_ECCR_ENGDIV(clockSource);
	base->ECCR = regValue;
}

/*!
 * @brief Sets engineering clock source select
 */
static inline void SIU_SetEngineeringClockSource(SIU_Type * base, uint32_t clockSource)
{
	uint32_t regValue = base->ECCR;
	regValue &= (uint32_t)(~(SIU_ECCR_ECCS_MASK));
	regValue |= SIU_ECCR_ECCS(clockSource);
	base->ECCR = regValue;
}

/*!
 * @brief Sets external bus division factor
 */
static inline void SIU_SetExternalBusDivisionFactor(SIU_Type * base, uint32_t clockSource)
{
	uint32_t regValue = base->ECCR;
	regValue &= (uint32_t)(~(SIU_ECCR_EBDF_MASK));
	regValue |= SIU_ECCR_EBDF(clockSource);
	base->ECCR = regValue;
}

/*!
 * @brief Sets MCAN clock source
 */
static inline void SIU_SetMCANClock(SIU_Type * base, uint32_t clockSource)
{
	uint32_t regValue = base->SYSDIV;
	regValue &= (uint32_t)(~(SIU_SYSDIV_MCANSEL_MASK));
	regValue |= SIU_SYSDIV_MCANSEL(clockSource);
	base->SYSDIV = regValue;
}

/*!
 * @brief Sets CSE halt
 */
static inline void SIU_SetCSE(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_CSE_MASK));
	regValue |= SIU_HLT1_CSE(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets ETPUC halt
 */
static inline void SIU_SetETPUC(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_ETPUC_MASK));
	regValue |= SIU_HLT1_ETPUC(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets ETPUA halt
 */
static inline void SIU_SetETPUA(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_ETPUA_MASK));
	regValue |= SIU_HLT1_ETPUA(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets NPC halt
 */
static inline void SIU_SetNPC(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_NPC_MASK));
	regValue |= SIU_HLT1_NPC(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets EBI halt
 */
static inline void SIU_SetEBI(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_EBI_MASK));
	regValue |= SIU_HLT1_EBI(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}


/*!
 * @brief Sets EQADCB halt
 */
static inline void SIU_SetEQADCB(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_EQADCB_MASK));
	regValue |= SIU_HLT1_EQADCB(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets EQADCA halt
 */
static inline void SIU_SetEQADCA(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_EQADCA_MASK));
	regValue |= SIU_HLT1_EQADCA(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets EMIOS0 halt
 */
static inline void SIU_SetEMIOS0(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_EMIOS0_MASK));
	regValue |= SIU_HLT1_EMIOS0(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets DECFIL halt
 */
static inline void SIU_SetDECFIL(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_DECFIL_MASK));
	regValue |= SIU_HLT1_DECFIL(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets EMIOS1 halt
 */
static inline void SIU_SetEMIOS1(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_EMIOS1_MASK));
	regValue |= SIU_HLT1_EMIOS1(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets PIT halt
 */
static inline void SIU_SetPIT(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_PIT_MASK));
	regValue |= SIU_HLT1_PIT(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets FLEXCAND halt
 */
static inline void SIU_SetFLEXCAND(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_FLEXCAND_MASK));
	regValue |= SIU_HLT1_FLEXCAND(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets FLEXCANC halt
 */
static inline void SIU_SetFLEXCANC(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_FLEXCANC_MASK));
	regValue |= SIU_HLT1_FLEXCANC(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets FLEXCANB halt
 */
static inline void SIU_SetFLEXCANB(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_FLEXCANB_MASK));
	regValue |= SIU_HLT1_FLEXCANB(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets FLEXCANA halt
 */
static inline void SIU_SetFLEXCANA(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_FLEXCANA_MASK));
	regValue |= SIU_HLT1_FLEXCANA(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets DSPID halt
 */
static inline void SIU_SetDSPID(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_DSPID_MASK));
	regValue |= SIU_HLT1_DSPID(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets DSPIC halt
 */
static inline void SIU_SetDSPIC(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_DSPIC_MASK));
	regValue |= SIU_HLT1_DSPIC(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets DSPIB halt
 */
static inline void SIU_SetDSPIB(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_DSPIB_MASK));
	regValue |= SIU_HLT1_DSPIB(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets DSPIA halt
 */
static inline void SIU_SetDSPIA(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_DSPIA_MASK));
	regValue |= SIU_HLT1_DSPIA(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets DSPIE halt
 */
static inline void SIU_SetDSPIE(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_DSPIE_MASK));
	regValue |= SIU_HLT1_DSPIE(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets ESCIF halt
 */
static inline void SIU_SetESCIF(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_ESCIF_MASK));
	regValue |= SIU_HLT1_ESCIF(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets ESCIE halt
 */
static inline void SIU_SetESCIE(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_ESCIE_MASK));
	regValue |= SIU_HLT1_ESCIE(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets ESCID halt
 */
static inline void SIU_SetESCID(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_ESCID_MASK));
	regValue |= SIU_HLT1_ESCID(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets ESCIC halt
 */
static inline void SIU_SetESCIC(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_ESCIC_MASK));
	regValue |= SIU_HLT1_ESCIC(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets ESCIB halt
 */
static inline void SIU_SetESCIB(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_ESCIB_MASK));
	regValue |= SIU_HLT1_ESCIB(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets ESCIA halt
 */
static inline void SIU_SetESCIA(SIU_Type * base, bool clockSource)
{
	uint32_t regValue = base->HLT1;
	regValue &= (uint32_t)(~(SIU_HLT1_ESCIA_MASK));
	regValue |= SIU_HLT1_ESCIA(clockSource ? 0UL:1UL);
	base->HLT1 = regValue;
}

/*!
 * @brief Sets PLL0 selector
 */
static inline void SIU_SetPLL0(SIU_Type * base, uint32_t clockSource)
{
	uint32_t regValue = base->SYSDIV;
	regValue &= (uint32_t)(~(SIU_SYSDIV_PLL0SEL_MASK));
	regValue |= SIU_SYSDIV_PLL0SEL(clockSource);
	base->SYSDIV = regValue;
}

/*!
 * @brief Sets PLL1 selector
 */
static inline void SIU_SetPLL1(SIU_Type * base, uint32_t clockSource)
{
	uint32_t regValue = base->SYSDIV;
	regValue &= (uint32_t)(~(SIU_SYSDIV_PLL1SEL_MASK));
	regValue |= SIU_SYSDIV_PLL1SEL(clockSource);
	base->SYSDIV = regValue;
}

/*!
* @brief Returns if module is enabled or not
*/
static inline bool SIU_GetModuleStatus(const SIU_Type* base, uint32_t haltRegister, uint32_t haltBitfield, uint32_t mask)
{
  bool retValue = false;
  uint32_t shift;

  shift = 31U - haltBitfield;

  if (haltRegister == 1U)
  {
    if (((base->HLT1 & mask) >> shift) == 0U)
    {
	  retValue = true;
    }
  }

  return retValue;
}

#if defined(__cplusplus)
}
#endif /* __cplusplus*/


/*! @}*/

#endif /* CGM_HW_ACCESS_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/
