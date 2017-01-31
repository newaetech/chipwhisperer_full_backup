/*
 * Copyright (c) 2016, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the Intel Corporation nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __FW_MANAGER_H__
#define __FW_MANAGER_H__

/* NOTE: these includes must be removed once the dfu example is updated */
#include "dfu/dfu.h"
#include "dfu/qda/qda.h"
#include "dfu/qda/xmodem.h"
#include "qfm/qfm.h"

/**
 * Bootloader Firmware Management.
 *
 * The module implementing the FM mode of the bootloader.
 *
 * @defgroup groupFM Bootloader Firmware Management.
 * @{
 */

/* NOTE: to be moved in a SoC-specific header */
/** Check if the FM sticky bit is set */
#define FM_STICKY_BITCHK() (QM_SCSS_GP->gps0 & BIT(0))
/** Set the FM sticky bit */
#define FM_STICKY_BITSET() (QM_SCSS_GP->gps0 |= BIT(0))
/** Clear the FM sticky bit */
#define FM_STICKY_BITCLR() (QM_SCSS_GP->gps0 &= ~BIT(0))

/**
 * Start Firmware Management mode.
 *
 * Setup up FM resources (communication link, timers, flash, etc.) and run the
 * FM main loop. This function never returns; indeed, FM mode is exited through
 * a reset.
 */
void fm_main(void);

/**
 * @}
 */

#endif /* __FW_MANAGER_H__ */
