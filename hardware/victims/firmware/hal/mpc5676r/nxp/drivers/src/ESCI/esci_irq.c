/*
 * Copyright 2018-2019 NXP
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
 * @file esci_irq.h
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, External could be made static.
 * Function is defined for usage by application code.
 */

#include "esci_irq.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* There is one IRQ handler for each instance which is triggered
 * for every enabled interrupt event for that instance */
void ESCI0_CIR_IRQHandler(void);
void ESCI1_CIR_IRQHandler(void);
void ESCI2_CIR_IRQHandler(void);
void ESCI3_CIR_IRQHandler(void);
void ESCI4_CIR_IRQHandler(void);
void ESCI5_CIR_IRQHandler(void);

/*******************************************************************************
 * Code
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI0_CIR_IRQHandler
 * Description   : Handler for ESCI0_CIR_IRQn interrupt
 *
 *END**************************************************************************/
void ESCI0_CIR_IRQHandler(void)
{
    ESCI_IRQ_Handler(0U);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI1_CIR_IRQHandler
 * Description   : Handler for ESCI1_CIR_IRQn interrupt
 *
 *END**************************************************************************/
void ESCI1_CIR_IRQHandler(void)
{
    ESCI_IRQ_Handler(1U);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI2_CIR_IRQHandler
 * Description   : Handler for ESCI2_CIR_IRQn interrupt
 *
 *END**************************************************************************/
void ESCI2_CIR_IRQHandler(void)
{
    ESCI_IRQ_Handler(2U);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI3_CIR_IRQHandler
 * Description   : Handler for ESCI3_CIR_IRQn interrupt
 *
 *END**************************************************************************/
void ESCI3_CIR_IRQHandler(void)
{
    ESCI_IRQ_Handler(3U);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI4_CIR_IRQHandler
 * Description   : Handler for ESCI4_CIR_IRQn interrupt
 *
 *END**************************************************************************/
void ESCI4_CIR_IRQHandler(void)
{
    ESCI_IRQ_Handler(4U);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI5_CIR_IRQHandler
 * Description   : Handler for ESCI5_CIR_IRQn interrupt
 *
 *END**************************************************************************/
void ESCI5_CIR_IRQHandler(void)
{
    ESCI_IRQ_Handler(5U);
}

/******************************************************************************/
/* EOF */
/******************************************************************************/
