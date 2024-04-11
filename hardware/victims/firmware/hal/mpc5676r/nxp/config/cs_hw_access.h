/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
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

#if !defined(CGMCS_HW_ACCESS_H)
#define CGMCS_HW_ACCESS_H

#include "MPC5676R.h"
#include "MPC5676R_features.h"
#include <stdbool.h>
#include <stddef.h>
#if 0 
/*!
 * @file cs_hw_access.h
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 */

/*!
 * @ingroup cs_hw_access
 * @defgroup cs_hw_access
 * @{
 */

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus*/

    /*!
 * @brief Gets the PLLDIG clock source first output divider ratio.
 */
    static inline uint32_t CS_GetPllFirstOutputDividerRatio(const PLLDIG_Type *base, uint32_t instance)
    {
        uint32_t retVal;

        if (instance == 0U)
        {
            retVal = ((base->PLL0DV & PLLDIG_PLL0DV_RFDPHI_MASK) >> PLLDIG_PLL0DV_RFDPHI_SHIFT);
        }
        else
        {
            retVal = ((base->PLL1DV & PLLDIG_PLL1DV_RFDPHI_MASK) >> PLLDIG_PLL1DV_RFDPHI_SHIFT);
        }

        return retVal;
    }

    /*!
 * @brief Gets the PLLDIG clock source second output divider ratio.
 */
    static inline uint32_t CS_GetPllSecondOutputDividerRatio(const PLLDIG_Type *base, uint32_t instance)
    {
        uint32_t retVal;

        if (instance == 0U)
        {
            retVal = ((base->PLL0DV & PLLDIG_PLL0DV_RFDPHI1_MASK) >> PLLDIG_PLL0DV_RFDPHI1_SHIFT);
        }
        else
        {
            retVal = 0U;
        }

        return retVal;
    }

    /*!
 * @brief Gets the PLLDIG clock source multiply factor.
 */
    static inline uint32_t CS_GetPLLMultiplyFactor(const PLLDIG_Type *base, uint32_t instance)
    {
        uint32_t retVal;

        if (instance == 0U)
        {
            retVal = ((base->PLL0DV & PLLDIG_PLL0DV_MFD_MASK) >> PLLDIG_PLL0DV_MFD_SHIFT);
        }
        else
        {
            retVal = ((base->PLL1DV & PLLDIG_PLL1DV_MFD_MASK) >> PLLDIG_PLL1DV_MFD_SHIFT);
        }

        return retVal;
    }

    /*!
 * @brief Gets the PLLDIG clock source predivider ratio.
 */
    static inline uint32_t CS_GetPllPreDividerRatio(const PLLDIG_Type *base, uint32_t instance)
    {
        uint32_t retVal;

        if (instance == 0U)
        {
            retVal = ((base->PLL0DV & PLLDIG_PLL0DV_PREDIV_MASK) >> PLLDIG_PLL0DV_PREDIV_SHIFT);
        }
        else
        {
            retVal = 0U;
        }

        return retVal;
    }

    /*!
 * @brief Sets PLL0 clock configuration
 */
    static inline void CS_SetPLL0(PLLDIG_Type *base, bool clockSource)
    {
        uint32_t regValue = base->PLL0CR;
        regValue &= (uint32_t)(~(PLLDIG_PLL0CR_CLKCFG_MASK));
        regValue |= PLLDIG_PLL0CR_CLKCFG(clockSource ? 3UL : 0UL);
        base->PLL0CR = regValue;
    }

    /*!
 * @brief Sets PLL1 clock configuration
 */
    static inline void CS_SetPLL1(PLLDIG_Type *base, bool clockSource)
    {
        uint32_t regValue = base->PLL1CR;
        regValue &= (uint32_t)(~(PLLDIG_PLL1CR_CLKCFG_MASK));
        regValue |= PLLDIG_PLL1CR_CLKCFG(clockSource ? 3UL : 0UL);
        base->PLL1CR = regValue;
    }

    /*!
 * @brief Gets the PLLDIG clock source fractional divide input.
 */
    static inline uint32_t CS_GetPllFractionalDivider(const PLLDIG_Type *base)
    {
        return ((base->PLL1FD & PLLDIG_PLL1FD_FRCDIV_MASK) >> PLLDIG_PLL1FD_FRCDIV_SHIFT);
    }

    /*!
 * @brief Gets PLL0 status
 */
    static inline bool CS_GetPLL0Status(const PLLDIG_Type *base)
    {
        return (base->PLL0CR & PLLDIG_PLL0CR_CLKCFG_MASK) != 0U;
    }

    /*!
 * @brief Gets PLL1 status
 */
    static inline bool CS_GetPLL1Status(const PLLDIG_Type *base)
    {
        return (base->PLL1CR & PLLDIG_PLL1CR_CLKCFG_MASK) != 0U;
    }

    /*!
 * @brief Gets PLL1 rectangular dither disable
 */
    static inline bool CS_RectangularDitherDisable(const PLLDIG_Type *base)
    {
        return (base->PLL1FD & PLLDIG_PLL1FD_DTHDIS1_MASK) != 0U;
    }

    /*!
 * @brief Gets PLL1 fractional divider status
 */
    static inline bool CS_FractionalDivideEnable(const PLLDIG_Type *base)
    {
        return (base->PLL1FD & PLLDIG_PLL1FD_FDEN_MASK) != 0U;
    }

    /*!
 * @brief Disable XOSC, PLL0, PLL1
 */
    static inline void CS_DisableClockSources(PLLDIG_Type *base)
    {
        uint32_t regValue;

        regValue = base->PLL0CR;
        regValue &= (uint32_t)(~(PLLDIG_PLL0CR_CLKCFG_MASK));
        regValue |= SIU_SYSDIV_SYSCLKSEL(0U);
        base->PLL0CR = regValue;

        regValue = base->PLL1CR;
        regValue &= (uint32_t)(~(PLLDIG_PLL1CR_CLKCFG_MASK));
        regValue |= SIU_SYSDIV_SYSCLKSEL(0U);
        base->PLL1CR = regValue;
    }

    /*!
 * @brief Sets PLL Divider Register
 */
    static inline void CS_PLLDivider(PLLDIG_Type *base, uint32_t instance, uint32_t phi0Divider, uint32_t phi1Divider, uint32_t predivider, uint32_t mulFactorDiv)
    {
        uint32_t value = 0U;

        /* Configure divider */
        switch (instance)
        {
        case 0U:
        {
            value = (uint32_t)(PLLDIG_PLL0DV_RFDPHI1(phi1Divider) |
                               PLLDIG_PLL0DV_RFDPHI(phi0Divider) |
                               PLLDIG_PLL0DV_PREDIV(predivider) |
                               PLLDIG_PLL0DV_MFD(mulFactorDiv));
            base->PLL0DV = value;
        }
        break;
        case 1U:
        {
            value = (uint32_t)(PLLDIG_PLL1DV_RFDPHI(phi0Divider) |
                               PLLDIG_PLL1DV_MFD(mulFactorDiv));
            base->PLL1DV = value;
        }
        break;

        default:
        {
            /* 2 Invalid instance number. */
            //DEV_ASSERT(false);
        }
        break;
        }
    }

    /*!
 * @brief Configures PLL Modulation
 */
    static inline void CS_ConfigurePllModulation(PLLDIG_Type *base, bool enable, uint32_t modulationType, uint32_t modulationPeriod, uint32_t incrementStep)
    {
        uint32_t value;

        value = (uint32_t)(PLLDIG_PLL1FM_MODEN(enable ? 1UL : 0UL) |
                           PLLDIG_PLL1FM_MODSEL(modulationType) |
                           PLLDIG_PLL1FM_INCSTP(incrementStep) |
                           PLLDIG_PLL1FM_MODPRD(modulationPeriod));
        base->PLL1FM = value;
    }

    /*!
 * @brief Sets PLL Fractional Divider
 */
    static inline void CS_SetPLLFractionalDivider(PLLDIG_Type *base, bool enable, uint32_t fracDivider, bool rectangularDither, bool triangularDither)
    {
        uint32_t value = (uint32_t)(PLLDIG_PLL1FD_FDEN(enable ? 1UL : 0UL) |
                                    PLLDIG_PLL1FD_FRCDIV(fracDivider) |
                                    PLLDIG_PLL1FD_DTHDIS1(rectangularDither ? 1UL : 0UL) |
                                    PLLDIG_PLL1FD_DTHDIS0(triangularDither ? 1UL : 0UL));
        base->PLL1FD = value;
    }

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

/*! @}*/

#endif
#endif /* CS_HW_ACCESS_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/
