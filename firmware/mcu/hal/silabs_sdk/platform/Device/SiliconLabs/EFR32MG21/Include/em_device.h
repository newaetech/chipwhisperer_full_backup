/**************************************************************************//**
 * @file
 * @brief CMSIS Cortex-M Peripheral Access Layer for Silicon Laboratories
 *        microcontroller devices
 *
 * This is a convenience header file for defining the part number on the
 * build command line, instead of specifying the part specific header file.
 *
 * @verbatim
 * Example: Add "-DEFM32G890F128" to your build options, to define part
 *          Add "#include "em_device.h" to your source files

 *
 * @endverbatim
 ******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories, Inc. www.silabs.com</b>
 ******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *****************************************************************************/

#ifndef EM_DEVICE_H
#define EM_DEVICE_H
#if defined(EFR32MG21A010F1024IM32)
#include "efr32mg21a010f1024im32.h"

#elif defined(EFR32MG21A010F512IM32)
#include "efr32mg21a010f512im32.h"

#elif defined(EFR32MG21A010F768IM32)
#include "efr32mg21a010f768im32.h"

#elif defined(EFR32MG21A020F1024IM32)
#include "efr32mg21a020f1024im32.h"

#elif defined(EFR32MG21A020F512IM32)
#include "efr32mg21a020f512im32.h"

#elif defined(EFR32MG21A020F768IM32)
#include "efr32mg21a020f768im32.h"

#elif defined(EFR32MG21B010F1024IM32)
#include "efr32mg21b010f1024im32.h"

#elif defined(EFR32MG21B010F512IM32)
#include "efr32mg21b010f512im32.h"

#elif defined(EFR32MG21B010F768IM32)
#include "efr32mg21b010f768im32.h"

#elif defined(EFR32MG21B020F1024IM32)
#include "efr32mg21b020f1024im32.h"

#elif defined(EFR32MG21B020F512IM32)
#include "efr32mg21b020f512im32.h"

#elif defined(EFR32MG21B020F768IM32)
#include "efr32mg21b020f768im32.h"

#elif defined(RM21Z000F1024IM32)
#include "rm21z000f1024im32.h"

#else
#error "em_device.h: PART NUMBER undefined"
#endif
#endif /* EM_DEVICE_H */
