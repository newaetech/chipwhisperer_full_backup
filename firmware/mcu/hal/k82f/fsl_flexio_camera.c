/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_flexio_camera.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.flexio_camera"
#endif


/*******************************************************************************
 * Prototypes
 ******************************************************************************/


/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Codes
 ******************************************************************************/

static uint32_t FLEXIO_CAMERA_GetInstance(FLEXIO_CAMERA_Type *base)
{
    return FLEXIO_GetInstance(base->flexioBase);
}

void FLEXIO_CAMERA_GetDefaultConfig(flexio_camera_config_t *config)
{
    assert(config);

    config->enablecamera = false;
    config->enableInDoze = false;
    config->enableInDebug = false;
    config->enableFastAccess = false;
}

void FLEXIO_CAMERA_Init(FLEXIO_CAMERA_Type *base, const flexio_camera_config_t *config)
{
    assert(base && config);

    volatile uint32_t i = 0;
    volatile uint32_t controlVal = 0;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate flexio clock. */
    CLOCK_EnableClock(s_flexioClocks[FLEXIO_CAMERA_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    flexio_shifter_config_t shifterConfig;
    flexio_timer_config_t timerConfig;

    /* Clear the shifterConfig & timerConfig struct. */
    memset(&shifterConfig, 0, sizeof(shifterConfig));
    memset(&timerConfig, 0, sizeof(timerConfig));

    /* Configure flexio camera */
    controlVal = base->flexioBase->CTRL;
    controlVal &=
        ~(FLEXIO_CTRL_DOZEN_MASK | FLEXIO_CTRL_DBGE_MASK | FLEXIO_CTRL_FASTACC_MASK | FLEXIO_CTRL_FLEXEN_MASK);
    controlVal |= (FLEXIO_CTRL_DBGE(config->enableInDebug) | FLEXIO_CTRL_FASTACC(config->enableFastAccess) |
                   FLEXIO_CTRL_FLEXEN(config->enablecamera));
    if (!config->enableInDoze)
    {
        controlVal |= FLEXIO_CTRL_DOZEN_MASK;
    }
    base->flexioBase->CTRL = controlVal;

    /* FLEXIO_CAMERA shifter config */
    shifterConfig.timerSelect = base->timerIdx;
    shifterConfig.timerPolarity = kFLEXIO_ShifterTimerPolarityOnPositive;
    shifterConfig.pinConfig = kFLEXIO_PinConfigOutputDisabled;
    shifterConfig.pinSelect = base->datPinStartIdx;
    shifterConfig.pinPolarity = kFLEXIO_PinActiveHigh;
    shifterConfig.shifterMode = kFLEXIO_ShifterModeReceive;
    shifterConfig.parallelWidth = FLEXIO_CAMERA_PARALLEL_DATA_WIDTH - 1U;
    shifterConfig.inputSource = kFLEXIO_ShifterInputFromNextShifterOutput;
    shifterConfig.shifterStop = kFLEXIO_ShifterStopBitDisable;
    shifterConfig.shifterStart = kFLEXIO_ShifterStartBitDisabledLoadDataOnEnable;
    /* Configure the shifters as FIFO buffer. */
    for (i = base->shifterStartIdx; i < (base->shifterStartIdx + base->shifterCount - 1U); i++)
    {
        FLEXIO_SetShifterConfig(base->flexioBase, i, &shifterConfig);
    }
    shifterConfig.inputSource = kFLEXIO_ShifterInputFromPin;
    FLEXIO_SetShifterConfig(base->flexioBase, i, &shifterConfig);

    /* FLEXIO_CAMERA timer config, the PCLK's clk is source of timer to drive the shifter, the HREF is the selecting
     * signal for available data. */
    timerConfig.triggerSelect = FLEXIO_TIMER_TRIGGER_SEL_PININPUT(base->hrefPinIdx);
    timerConfig.triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveHigh;
    timerConfig.triggerSource = kFLEXIO_TimerTriggerSourceInternal;
    timerConfig.pinConfig = kFLEXIO_PinConfigOutputDisabled;
    timerConfig.pinSelect = base->pclkPinIdx;
    timerConfig.pinPolarity = kFLEXIO_PinActiveHigh;
    timerConfig.timerMode = kFLEXIO_TimerModeSingle16Bit;
    timerConfig.timerOutput = kFLEXIO_TimerOutputZeroNotAffectedByReset;
    timerConfig.timerDecrement = kFLEXIO_TimerDecSrcOnPinInputShiftPinInput;
    timerConfig.timerReset = kFLEXIO_TimerResetOnTimerTriggerRisingEdge;
    timerConfig.timerDisable = kFLEXIO_TimerDisableOnTriggerFallingEdge;
    timerConfig.timerEnable = kFLEXIO_TimerEnableOnTriggerRisingEdge;
    timerConfig.timerStop = kFLEXIO_TimerStopBitDisabled;
    timerConfig.timerStart = kFLEXIO_TimerStartBitDisabled;
    timerConfig.timerCompare = 8U * base->shifterCount - 1U;

    FLEXIO_SetTimerConfig(base->flexioBase, base->timerIdx, &timerConfig);
    /* Clear flags. */
    FLEXIO_ClearShifterErrorFlags(base->flexioBase, ((1U << (base->shifterCount)) - 1U) << (base->shifterStartIdx));
    FLEXIO_ClearTimerStatusFlags(base->flexioBase, 1U << (base->timerIdx));
}

void FLEXIO_CAMERA_Deinit(FLEXIO_CAMERA_Type *base)
{
    base->flexioBase->SHIFTCFG[base->shifterStartIdx] = 0;
    base->flexioBase->SHIFTCTL[base->shifterStartIdx] = 0;
    base->flexioBase->TIMCFG[base->timerIdx] = 0;
    base->flexioBase->TIMCMP[base->timerIdx] = 0;
    base->flexioBase->TIMCTL[base->timerIdx] = 0;
    /* Clear the shifter flag. */
    base->flexioBase->SHIFTSTAT = (1U << base->shifterStartIdx);
    /* Clear the timer flag. */
    base->flexioBase->TIMSTAT = (1U << base->timerIdx);
}

uint32_t FLEXIO_CAMERA_GetStatusFlags(FLEXIO_CAMERA_Type *base)
{
    uint32_t status = 0;
    status = ((FLEXIO_GetShifterStatusFlags(base->flexioBase) >> (base->shifterStartIdx)) &
              ((1U << (base->shifterCount)) - 1U));
    return status;
}

void FLEXIO_CAMERA_ClearStatusFlags(FLEXIO_CAMERA_Type *base, uint32_t mask)
{
    if (mask & kFLEXIO_CAMERA_RxDataRegFullFlag)
    {
        FLEXIO_ClearShifterStatusFlags(base->flexioBase, ((1U << (base->shifterCount)) - 1U)
                                                             << (base->shifterStartIdx));
    }
    if (mask & kFLEXIO_CAMERA_RxErrorFlag)
    { /* Clear error flags if they are asserted to make sure the buffer would be available. */
        FLEXIO_ClearShifterErrorFlags(base->flexioBase, ((1U << (base->shifterCount)) - 1U) << (base->shifterStartIdx));
    }
}

void FLEXIO_CAMERA_EnableInterrupt(FLEXIO_CAMERA_Type *base)
{
    FLEXIO_EnableShifterStatusInterrupts(base->flexioBase, 1U << (base->shifterStartIdx));
}

void FLEXIO_CAMERA_DisableInterrupt(FLEXIO_CAMERA_Type *base)
{
    FLEXIO_DisableShifterStatusInterrupts(base->flexioBase, 1U << (base->shifterStartIdx));
}
