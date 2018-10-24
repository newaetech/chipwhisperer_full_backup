/**
 * \file Ifx_TypesReg.h
 * \brief
 * \copyright Copyright (c) 2012 Infineon Technologies AG. All rights reserved.
 *
 * Version: IFXREGTYPES_V1.0.R0
 *
 * MAY BE CHANGED BY USER [yes/no]: No
 *
 *                                 IMPORTANT NOTICE
 *
 *
 * Infineon Technologies AG (Infineon) is supplying this file for use
 * exclusively with Infineon's microcontroller products. This file can be freely
 * distributed within development tools that are supporting such microcontroller
 * products.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 */

#ifndef IFX_TYPESREG_H
#define IFX_TYPESREG_H 1
/******************************************************************************/

#if defined(__TASKING__)
#define Ifx_Strict_16Bit unsigned __sfrbit16
#define Ifx_Strict_32Bit unsigned __sfrbit32
#endif
#if defined(__GNUC__)
#define Ifx_Strict_16Bit volatile unsigned short
#define Ifx_Strict_32Bit volatile unsigned int
#endif
#if defined(__DCC__)
#define Ifx_Strict_16Bit unsigned short
#define Ifx_Strict_32Bit unsigned int
#endif

/******************************************************************************/
#endif /* IFX_TYPESREG_H */
