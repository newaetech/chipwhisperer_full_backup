/*
 * Copyright 2017 NXP
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
 * @file uart_pal_mapping.h
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 2.5, global macro not referenced
 * This macro is used by user.
 */

#ifndef UART_PAL_MAPPING_H
#define UART_PAL_MAPPING_H
#include "MPC5676R.h"
#include "MPC5676R_features.h"

#if (defined(S32K14x_SERIES) || defined(S32K11x_SERIES))
#define FLEXIO_UART_INSTANCE_COUNT    2U
#endif

/*! @brief Default baudrate value for UART */
#define UART_DEFAULT_BAUDRATE_VALUE   9600U

/*!
 * @brief Enumeration with the types of peripherals supported by UART PAL
 *
 * This enumeration contains the types of peripherals supported by UART PAL.
 * Implements : uart_inst_type_t_Class
 */
typedef enum
{
#if (defined(LPUART_INSTANCE_COUNT))
    UART_INST_TYPE_LPUART       = 0u,
#endif /* defined(LPUART_INSTANCE_COUNT) */

#if (defined(FLEXIO_UART_INSTANCE_COUNT))
    UART_INST_TYPE_FLEXIO_UART  = 1u,
#endif /* defined(FLEXIO_UART_INSTANCE_COUNT) */

#if (defined(LINFlexD_INSTANCE_COUNT))
    UART_INST_TYPE_LINFLEXD     = 2u,
#endif /* defined(LINFlexD_INSTANCE_COUNT) */

#if (defined(eSCI_INSTANCE_COUNT))
    UART_INST_TYPE_ESCI         = 3u
#endif /* defined(ESCI_INSTANCE_COUNT) */
} uart_inst_type_t;

/*!
 * @brief Structure storing PAL instance information
 *
 * This structure is used for storing PAL instance information.
 * Implements : uart_instance_t_Class
 */
typedef struct
{
    uart_inst_type_t instType;    /*!< Peripheral over which the PAL is used */
    uint32_t instIdx;             /*!< Instance index of the peripheral over which the PAL is used */
} uart_instance_t;

#endif /* UART_PAL_MAPPING_H */
