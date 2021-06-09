/******************************************************************************
*  Filename:       ioc.c
*  Revised:        $Date: 2013-03-22 15:36:20 +0100 (Fri, 22 Mar 2013) $
*  Revision:       $Revision: 9511 $
*
*  Description:    Driver for the I/O controller.
*
*  Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com/
*
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

//*****************************************************************************
//
//! \addtogroup ioc_api
//! @{
//
//*****************************************************************************

#include "hw_ioc.h"
#include "hw_gpio.h"
#include "hw_memmap.h"
#include "debug.h"
#include "ioc.h"
#include "gpio.h"

//*****************************************************************************
//
// This is the mapping between a pin number within port A and the corresponding
// override register.
//
//*****************************************************************************
static const uint32_t g_pui32IOCPortAOverrideReg[] =
{
    IOC_PA0_OVER, IOC_PA1_OVER, IOC_PA2_OVER, IOC_PA3_OVER,
    IOC_PA4_OVER, IOC_PA5_OVER, IOC_PA6_OVER, IOC_PA7_OVER
};

//*****************************************************************************
//
// This is the mapping between a pin number within port B and the corresponding
// override register.
//
//*****************************************************************************
static const uint32_t g_pui32IOCPortBOverrideReg[] =
{
    IOC_PB0_OVER, IOC_PB1_OVER, IOC_PB2_OVER, IOC_PB3_OVER,
    IOC_PB4_OVER, IOC_PB5_OVER, IOC_PB6_OVER, IOC_PB7_OVER
};

//*****************************************************************************
//
// This is the mapping between a pin number within port C and the corresponding
// override register.
//
//*****************************************************************************
static const uint32_t g_pui32IOCPortCOverrideReg[] =
{
    IOC_PC0_OVER, IOC_PC1_OVER, IOC_PC2_OVER, IOC_PC3_OVER,
    IOC_PC4_OVER, IOC_PC5_OVER, IOC_PC6_OVER, IOC_PC7_OVER
};

//*****************************************************************************
//
// This is the mapping between a pin number within port D and the corresponding
// override register.
//
//*****************************************************************************
static const uint32_t g_pui32IOCPortDOverrideReg[] =
{
    IOC_PD0_OVER, IOC_PD1_OVER, IOC_PD2_OVER, IOC_PD3_OVER,
    IOC_PD4_OVER, IOC_PD5_OVER, IOC_PD6_OVER, IOC_PD7_OVER
};

//*****************************************************************************
//
// This is the mapping between a pin number within port A and the corresponding
// signal select register.
//
//*****************************************************************************
static const uint32_t g_pui32IOCPortASignSelectReg[] =
{
    IOC_PA0_SEL, IOC_PA1_SEL, IOC_PA2_SEL, IOC_PA3_SEL,
    IOC_PA4_SEL, IOC_PA5_SEL, IOC_PA6_SEL, IOC_PA7_SEL
};

//*****************************************************************************
//
// This is the mapping between a pin number within port B and the corresponding
// signal select register.
//
//*****************************************************************************
static const uint32_t g_pui32IOCPortBSignSelectReg[] =
{
    IOC_PB0_SEL, IOC_PB1_SEL, IOC_PB2_SEL, IOC_PB3_SEL,
    IOC_PB4_SEL, IOC_PB5_SEL, IOC_PB6_SEL, IOC_PB7_SEL
};

//*****************************************************************************
//
// This is the mapping between a pin number within port C and the corresponding
// signal select register.
//
//*****************************************************************************
static const uint32_t g_pui32IOCPortCSignSelectReg[] =
{
    IOC_PC0_SEL, IOC_PC1_SEL, IOC_PC2_SEL, IOC_PC3_SEL,
    IOC_PC4_SEL, IOC_PC5_SEL, IOC_PC6_SEL, IOC_PC7_SEL
};

//*****************************************************************************
//
// This is the mapping between a pin number within port D and the corresponding
// signal select register.
//
//*****************************************************************************
static const uint32_t g_pui32IOCPortDSignSelectReg[] =
{
    IOC_PD0_SEL, IOC_PD1_SEL, IOC_PD2_SEL, IOC_PD3_SEL,
    IOC_PD4_SEL, IOC_PD5_SEL, IOC_PD6_SEL, IOC_PD7_SEL
};

//*****************************************************************************
//
// Defined values for the port select registers.
// (The registers are in the addr range: IOC_UARTRXD_UART0 - IOC_GPT3OCP2).
//
//*****************************************************************************
#define IOC_PAD_IN_SEL_PA0      0x00000000  // PA0
#define IOC_PAD_IN_SEL_PA1      0x00000001  // PA1
#define IOC_PAD_IN_SEL_PA2      0x00000002  // PA2
#define IOC_PAD_IN_SEL_PA3      0x00000003  // PA3
#define IOC_PAD_IN_SEL_PA4      0x00000004  // PA4
#define IOC_PAD_IN_SEL_PA5      0x00000005  // PA5
#define IOC_PAD_IN_SEL_PA6      0x00000006  // PA6
#define IOC_PAD_IN_SEL_PA7      0x00000007  // PA7
#define IOC_PAD_IN_SEL_PB0      0x00000008  // PB0
#define IOC_PAD_IN_SEL_PB1      0x00000009  // PB1
#define IOC_PAD_IN_SEL_PB2      0x0000000A  // PB2
#define IOC_PAD_IN_SEL_PB3      0x0000000B  // PB3
#define IOC_PAD_IN_SEL_PB4      0x0000000C  // PB4
#define IOC_PAD_IN_SEL_PB5      0x0000000D  // PB5
#define IOC_PAD_IN_SEL_PB6      0x0000000E  // PB6
#define IOC_PAD_IN_SEL_PB7      0x0000000F  // PB7
#define IOC_PAD_IN_SEL_PC0      0x00000010  // PC0
#define IOC_PAD_IN_SEL_PC1      0x00000011  // PC1
#define IOC_PAD_IN_SEL_PC2      0x00000012  // PC2
#define IOC_PAD_IN_SEL_PC3      0x00000013  // PC3
#define IOC_PAD_IN_SEL_PC4      0x00000014  // PC4
#define IOC_PAD_IN_SEL_PC5      0x00000015  // PC5
#define IOC_PAD_IN_SEL_PC6      0x00000016  // PC6
#define IOC_PAD_IN_SEL_PC7      0x00000017  // PC7
#define IOC_PAD_IN_SEL_PD0      0x00000018  // PD0
#define IOC_PAD_IN_SEL_PD1      0x00000019  // PD1
#define IOC_PAD_IN_SEL_PD2      0x0000001A  // PD2
#define IOC_PAD_IN_SEL_PD3      0x0000001B  // PD3
#define IOC_PAD_IN_SEL_PD4      0x0000001C  // PD4
#define IOC_PAD_IN_SEL_PD5      0x0000001D  // PD5
#define IOC_PAD_IN_SEL_PD6      0x0000001E  // PD6
#define IOC_PAD_IN_SEL_PD7      0x0000001F   // PD7


//*****************************************************************************
//
//! Mux desired on-chip peripheral output signal to the desired port pin(s).
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pins is the bit-packed representation of the port pin(s).
//! \param ui32OutputSignal is the desired peripheral output signal to drive the
//! desired port pin(s).
//!
//! This function routes the desired on-chip peripheral signal to the
//! desired pin(s) on the selected GPIO port.  Functions are available within
//! the GPIO device driver that can set the peripheral signal to be under
//! hardware control. The IOCPadConfigSet() function can be used to set the pin
//! drive type on the desired port pin.
//!
//! The \e ui32OutputSignal parameter is an enumerated type that controls which
//! peripheral output signal to route to the desired port pin(s).
//! This parameter can have any of the following values:
//!
//! - \b IOC_MUX_OUT_SEL_UART0_TXD
//! - \b IOC_MUX_OUT_SEL_UART1_RTS
//! - \b IOC_MUX_OUT_SEL_UART1_TXD
//! - \b IOC_MUX_OUT_SEL_SSI0_TXD
//! - \b IOC_MUX_OUT_SEL_SSI0_CLKOUT
//! - \b IOC_MUX_OUT_SEL_SSI0_FSSOUT
//! - \b IOC_MUX_OUT_SEL_SSI0_STXSER_EN
//! - \b IOC_MUX_OUT_SEL_SSI1_TXD
//! - \b IOC_MUX_OUT_SEL_SSI1_CLKOUT
//! - \b IOC_MUX_OUT_SEL_SSI1_FSSOUT
//! - \b IOC_MUX_OUT_SEL_SSI1_STXSER_EN
//! - \b IOC_MUX_OUT_SEL_I2C_CMSSDA
//! - \b IOC_MUX_OUT_SEL_I2C_CMSSCL
//! - \b IOC_MUX_OUT_SEL_GPT0_ICP1
//! - \b IOC_MUX_OUT_SEL_GPT0_ICP2
//! - \b IOC_MUX_OUT_SEL_GPT1_ICP1
//! - \b IOC_MUX_OUT_SEL_GPT1_ICP2
//! - \b IOC_MUX_OUT_SEL_GPT2_ICP1
//! - \b IOC_MUX_OUT_SEL_GPT2_ICP2
//! - \b IOC_MUX_OUT_SEL_GPT3_ICP1
//! - \b IOC_MUX_OUT_SEL_GPT3_ICP2
//!
//! The pin(s) in \e ui8Pins are specified using a bit-packed byte, where each
//! bit that is set identifies the pin to be accessed, and where bit 0 of the
//! byte represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so
//! on.
//! 
//! \return None
//
//*****************************************************************************
void
IOCPinConfigPeriphOutput(uint32_t ui32Port, uint8_t ui8Pins,
                         uint32_t ui32OutputSignal)
{
    uint32_t ui32PortAddr;
    uint32_t ui32PinNo;
    uint32_t ui32PinBit;

    //
    // Check the arguments
    //
    ASSERT((ui32Port == GPIO_A_BASE) || (ui32Port == GPIO_B_BASE) ||
           (ui32Port == GPIO_C_BASE) || (ui32Port == GPIO_D_BASE));
    ASSERT(ui8Pins != 0);

    //
    // Initialize to default value
    //
    ui32PortAddr = IOC_PA0_SEL;

    //
    // Look for specified port pins to be configured, multiple pins are allowed
    //
    for(ui32PinNo = 0; ui32PinNo < 8; ui32PinNo++)
    {
        ui32PinBit = (ui8Pins >> ui32PinNo) & 0x00000001;
        if(ui32PinBit != 0)
        {
            //
            // Find register addresses for configuring specified port pin
            //
            switch(ui32Port)
            {
            case GPIO_A_BASE:
                ui32PortAddr = g_pui32IOCPortASignSelectReg[ui32PinNo];
                break;

            case GPIO_B_BASE:
                ui32PortAddr = g_pui32IOCPortBSignSelectReg[ui32PinNo];
                break;

            case GPIO_C_BASE:
                ui32PortAddr = g_pui32IOCPortCSignSelectReg[ui32PinNo];
                break;

            case GPIO_D_BASE:
                ui32PortAddr = g_pui32IOCPortDSignSelectReg[ui32PinNo];
                break;

            default:
                // Default to port A pin 0
                ui32PortAddr = IOC_PA0_SEL;
                break;
            }

            //
            // Set the mux for the desired port pin to select the desired
            // peripheral output signal
            //
            HWREG(ui32PortAddr) = ui32OutputSignal;
        }
    }
}

//*****************************************************************************
//
//! Mux the desired port pin to the desired on-chip peripheral input signal
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pin is the bit-packed representation of the desired port pin.
//! \param ui32PinSelectReg is the address of the IOC mux-register for the
//! desired peripheral input signal to which the desired port pin shall be
//! routed.
//!
//! This function routes the desired port pin to the desired on-chip
//! peripheral input signal.  Functions are available within the GPIO device
//! driver that set the peripheral signal to be under hardware control and
//! configures the pin drive type on the desired port pin.
//!
//! The parameter \e ui32PinSelectReg is an enumerated data type that can be one
//! of the following values:
//!
//! - \b IOC_UARTRXD_UART0
//! - \b IOC_UARTCTS_UART1
//! - \b IOC_UARTRXD_UART1
//! - \b IOC_CLK_SSI_SSI0
//! - \b IOC_SSIRXD_SSI0
//! - \b IOC_SSIFSSIN_SSI0
//! - \b IOC_CLK_SSIIN_SSI0
//! - \b IOC_CLK_SSI_SSI1
//! - \b IOC_SSIRXD_SSI1
//! - \b IOC_SSIFSSIN_SSI1
//! - \b IOC_CLK_SSIIN_SSI1
//! - \b IOC_I2CMSSDA
//! - \b IOC_I2CMSSCL
//! - \b IOC_GPT0OCP1
//! - \b IOC_GPT0OCP2
//! - \b IOC_GPT1OCP1
//! - \b IOC_GPT1OCP2
//! - \b IOC_GPT2OCP1
//! - \b IOC_GPT2OCP2
//! - \b IOC_GPT3OCP1
//! - \b IOC_GPT3OCP2
//!
//! The pin in ui8Pin is specified using a bit-packed byte, where the bit that
//! is set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \return None
//
//*****************************************************************************
void
IOCPinConfigPeriphInput(uint32_t ui32Port, uint8_t ui8Pin,
                        uint32_t ui32PinSelectReg)
{
    uint32_t ui32PortPin;

    //
    //Set initial values
    //
    ui32PortPin = IOC_PAD_IN_SEL_PA0;

    //
    // Check the arguments
    //
    ASSERT((ui32Port == GPIO_A_BASE) || (ui32Port == GPIO_B_BASE) ||
           (ui32Port == GPIO_C_BASE) || (ui32Port == GPIO_D_BASE));
    ASSERT((ui8Pin == GPIO_PIN_0) || (ui8Pin == GPIO_PIN_1) ||
           (ui8Pin == GPIO_PIN_2) || (ui8Pin == GPIO_PIN_3) ||
           (ui8Pin == GPIO_PIN_4) || (ui8Pin == GPIO_PIN_5) ||
           (ui8Pin == GPIO_PIN_6) || (ui8Pin == GPIO_PIN_7));
    ASSERT((ui32PinSelectReg == IOC_UARTRXD_UART0)  ||
           (ui32PinSelectReg == IOC_UARTCTS_UART1)  ||
           (ui32PinSelectReg == IOC_UARTRXD_UART1)  ||
           (ui32PinSelectReg == IOC_CLK_SSI_SSI0)   ||
           (ui32PinSelectReg == IOC_SSIRXD_SSI0)    ||
           (ui32PinSelectReg == IOC_SSIFSSIN_SSI0)  ||
           (ui32PinSelectReg == IOC_CLK_SSIIN_SSI0) ||
           (ui32PinSelectReg == IOC_CLK_SSI_SSI1)   ||
           (ui32PinSelectReg == IOC_SSIRXD_SSI1)    ||
           (ui32PinSelectReg == IOC_SSIFSSIN_SSI1)  ||
           (ui32PinSelectReg == IOC_CLK_SSIIN_SSI1) ||
           (ui32PinSelectReg == IOC_I2CMSSDA)       ||
           (ui32PinSelectReg == IOC_I2CMSSCL)       ||
           (ui32PinSelectReg == IOC_GPT0OCP1)       ||
           (ui32PinSelectReg == IOC_GPT0OCP2)       ||
           (ui32PinSelectReg == IOC_GPT1OCP1)       ||
           (ui32PinSelectReg == IOC_GPT1OCP2)       ||
           (ui32PinSelectReg == IOC_GPT2OCP1)       ||
           (ui32PinSelectReg == IOC_GPT2OCP2)       ||
           (ui32PinSelectReg == IOC_GPT3OCP1)       ||
           (ui32PinSelectReg == IOC_GPT3OCP2));

    switch(ui32Port)
    {
    case GPIO_A_BASE:
        if(ui8Pin == GPIO_PIN_0)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PA0;
        }
        if(ui8Pin == GPIO_PIN_1)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PA1;
        }
        if(ui8Pin == GPIO_PIN_2)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PA2;
        }
        if(ui8Pin == GPIO_PIN_3)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PA3;
        }
        if(ui8Pin == GPIO_PIN_4)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PA4;
        }
        if(ui8Pin == GPIO_PIN_5)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PA5;
        }
        if(ui8Pin == GPIO_PIN_6)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PA6;
        }
        if(ui8Pin == GPIO_PIN_7)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PA7;
        }
        break;

    case GPIO_B_BASE:
        if(ui8Pin == GPIO_PIN_0)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PB0;
        }
        if(ui8Pin == GPIO_PIN_1)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PB1;
        }
        if(ui8Pin == GPIO_PIN_2)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PB2;
        }
        if(ui8Pin == GPIO_PIN_3)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PB3;
        }
        if(ui8Pin == GPIO_PIN_4)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PB4;
        }
        if(ui8Pin == GPIO_PIN_5)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PB5;
        }
        if(ui8Pin == GPIO_PIN_6)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PB6;
        }
        if(ui8Pin == GPIO_PIN_7)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PB7;
        }
        break;

    case GPIO_C_BASE:
        if(ui8Pin == GPIO_PIN_0)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PC0;
        }
        if(ui8Pin == GPIO_PIN_1)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PC1;
        }
        if(ui8Pin == GPIO_PIN_2)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PC2;
        }
        if(ui8Pin == GPIO_PIN_3)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PC3;
        }
        if(ui8Pin == GPIO_PIN_4)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PC4;
        }
        if(ui8Pin == GPIO_PIN_5)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PC5;
        }
        if(ui8Pin == GPIO_PIN_6)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PC6;
        }
        if(ui8Pin == GPIO_PIN_7)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PC7;
        }
        break;

    case GPIO_D_BASE:
        if(ui8Pin == GPIO_PIN_0)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PD0;
        }
        if(ui8Pin == GPIO_PIN_1)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PD1;
        }
        if(ui8Pin == GPIO_PIN_2)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PD2;
        }
        if(ui8Pin == GPIO_PIN_3)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PD3;
        }
        if(ui8Pin == GPIO_PIN_4)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PD4;
        }
        if(ui8Pin == GPIO_PIN_5)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PD5;
        }
        if(ui8Pin == GPIO_PIN_6)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PD6;
        }
        if(ui8Pin == GPIO_PIN_7)
        {
            ui32PortPin = IOC_PAD_IN_SEL_PD7;
        }
        break;

    default:
        // Default to port A pin 0
        ui32PortPin = IOC_PAD_IN_SEL_PA0;
        break;
    }

    //
    // Set the mux for the desired peripheral inputsignal to select the
    // the desired port pin.
    //
    HWREG(ui32PinSelectReg) = ui32PortPin;
}

//*****************************************************************************
//
//! Set desired drive type on the pad for the desired port pin(s).
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pins is the bit-packed representation of the port pin(s).
//! \param ui32PinDrive is the drive configuration of the desired port
//! pin.
//!
//! This function sets the desired pin drive type for the desired pin(s)
//! on the selected GPIO port.
//!
//! The \e ui32PinDrive parameter controls the configuration of the pin drive on
//! the pad for the desired pin(s). The parameter is the logical OR of any of
//! the following:
//!
//! - \b IOC_OVERRIDE_OE
//! - \b IOC_OVERRIDE_PUE
//! - \b IOC_OVERRIDE_PDE
//! - \b IOC_OVERRIDE_ANA
//! - \b IOC_OVERRIDE_DIS
//!
//! where \b IOC_OVERRIDE_OE is the output enable bit connected directly
//! to the output enable pin for the IO driver cell, after it is ORed
//! with any OE signal from the desired peripheral.  The OE is driven from the
//! SSI, I2C and GPT peripherals.  \b IOC_OVERRIDE_PUE is the enable bit for
//! the pull-up.  \b IOC_OVERRIDE_PDE is the enable bit for the pull-down.
//! \b IOC_OVERRIDE_ANA must be set for the analog signal.
//!
//! The pin(s) in \e ui8Pins are specified using a bit-packed byte, where each
//! bit that is set identifies the pin to be accessed, and where bit 0 of the
//! byte represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so
//! on.
//!
//! \note PC0 through PC3 are bidirectional high-drive pad-cells. They do not
//! support on-die pullup or pulldown resistors or analog connectivity.
//! For these four pins the \e ui32PinDrive parameter must be set to either
//! \b IOC_OVERRIDE_OE or IOC_OVERRIDE_DIS.
//!
//! \return None
//
//*****************************************************************************
void
IOCPadConfigSet(uint32_t ui32Port, uint8_t ui8Pins,
                uint32_t ui32PinDrive)
{
    uint32_t ui32OverrideRegAddr;
    uint32_t ui32PinNo;
    uint32_t ui32PinBit;

    //
    // Check the arguments
    //
    ASSERT((ui32Port == GPIO_A_BASE) || (ui32Port == GPIO_B_BASE) ||
           (ui32Port == GPIO_C_BASE) || (ui32Port == GPIO_D_BASE));
    ASSERT(ui8Pins != 0);
    ASSERT((ui32PinDrive == IOC_OVERRIDE_OE)  ||
           (ui32PinDrive == IOC_OVERRIDE_PUE) ||
           (ui32PinDrive == IOC_OVERRIDE_PDE) ||
           (ui32PinDrive == IOC_OVERRIDE_ANA) ||
           (ui32PinDrive == IOC_OVERRIDE_DIS));
    //
    // PC0-PC3 does not support on-die pullup, pulldown or analog connectivity.
    //
    ASSERT(!((ui32Port == GPIO_C_BASE) && ((ui8Pins & 0xf) > 0) &&
             ((ui32PinDrive == IOC_OVERRIDE_PUE) ||
              (ui32PinDrive == IOC_OVERRIDE_PDE) ||
              (ui32PinDrive == IOC_OVERRIDE_ANA))));

    //
    // Initialize to default value
    //
    ui32OverrideRegAddr = IOC_PA0_SEL;

    //
    // Look for specified port pins to be configured, multiple pins are allowed
    //
    for(ui32PinNo = 0; ui32PinNo < 8; ui32PinNo++)
    {
        ui32PinBit = (ui8Pins >> ui32PinNo) & 0x00000001;
        if(ui32PinBit != 0)
        {
            //
            // Find register addresses for configuring specified port pin
            //
            switch(ui32Port)
            {
            case GPIO_A_BASE:
                ui32OverrideRegAddr = g_pui32IOCPortAOverrideReg[ui32PinNo];
                break;

            case GPIO_B_BASE:
                ui32OverrideRegAddr = g_pui32IOCPortBOverrideReg[ui32PinNo];
                break;

            case GPIO_C_BASE:
                ui32OverrideRegAddr = g_pui32IOCPortCOverrideReg[ui32PinNo];
                break;

            case GPIO_D_BASE:
                ui32OverrideRegAddr = g_pui32IOCPortDOverrideReg[ui32PinNo];
                break;

            default:
                // Default to port A pin 0
                ui32OverrideRegAddr = IOC_PA0_OVER;
                break;
            }

            //
            // Set desired pin drive for the desired port pin
            //
            HWREG(ui32OverrideRegAddr) = ui32PinDrive;
        }
    }
}

//*****************************************************************************
//
//! Get drive type on the pad for the desired port pin.
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pin is the bit-packed representation of the port pin.
//!
//! This function returns the configured pin drive type for the desired
//! pin on the selected GPIO port.
//!
//! The pin in \e ui8Pin is specified using a bit-packed byte, where the bit
//! that is set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \return Returns the logical OR of the enumerated data types described for
//! \e ui32PinDrive parameter in IOCPadConfigSet().
//! 
//! \sa IOCPadConfigSet()
//
//*****************************************************************************
uint32_t
IOCPadConfigGet(uint32_t ui32Port, uint8_t ui8Pin)
{
    uint32_t ui32OverrideRegAddr;

    //
    //Set initial values
    //
    ui32OverrideRegAddr = IOC_PA0_OVER;

    //
    // Check the arguments
    //
    ASSERT((ui32Port == GPIO_A_BASE) || (ui32Port == GPIO_B_BASE) ||
           (ui32Port == GPIO_C_BASE) || (ui32Port == GPIO_D_BASE));
    ASSERT((ui8Pin == GPIO_PIN_0) || (ui8Pin == GPIO_PIN_1) ||
           (ui8Pin == GPIO_PIN_2) || (ui8Pin == GPIO_PIN_3) ||
           (ui8Pin == GPIO_PIN_4) || (ui8Pin == GPIO_PIN_5) ||
           (ui8Pin == GPIO_PIN_6) || (ui8Pin == GPIO_PIN_7));

    //
    // Find the address of the override register for the desired pin
    //
    switch(ui32Port)
    {
    case GPIO_A_BASE:
        if(ui8Pin == GPIO_PIN_0)
        {
            ui32OverrideRegAddr = IOC_PA0_OVER;
        }
        if(ui8Pin == GPIO_PIN_1)
        {
            ui32OverrideRegAddr = IOC_PA1_OVER;
        }
        if(ui8Pin == GPIO_PIN_2)
        {
            ui32OverrideRegAddr = IOC_PA2_OVER;
        }
        if(ui8Pin == GPIO_PIN_3)
        {
            ui32OverrideRegAddr = IOC_PA3_OVER;
        }
        if(ui8Pin == GPIO_PIN_4)
        {
            ui32OverrideRegAddr = IOC_PA4_OVER;
        }
        if(ui8Pin == GPIO_PIN_5)
        {
            ui32OverrideRegAddr = IOC_PA5_OVER;
        }
        if(ui8Pin == GPIO_PIN_6)
        {
            ui32OverrideRegAddr = IOC_PA6_OVER;
        }
        if(ui8Pin == GPIO_PIN_7)
        {
            ui32OverrideRegAddr = IOC_PA7_OVER;
        }
        break;

    case GPIO_B_BASE:
        if(ui8Pin == GPIO_PIN_0)
        {
            ui32OverrideRegAddr = IOC_PB0_OVER;
        }
        if(ui8Pin == GPIO_PIN_1)
        {
            ui32OverrideRegAddr = IOC_PB1_OVER;
        }
        if(ui8Pin == GPIO_PIN_2)
        {
            ui32OverrideRegAddr = IOC_PB2_OVER;
        }
        if(ui8Pin == GPIO_PIN_3)
        {
            ui32OverrideRegAddr = IOC_PB3_OVER;
        }
        if(ui8Pin == GPIO_PIN_4)
        {
            ui32OverrideRegAddr = IOC_PB4_OVER;
        }
        if(ui8Pin == GPIO_PIN_5)
        {
            ui32OverrideRegAddr = IOC_PB5_OVER;
        }
        if(ui8Pin == GPIO_PIN_6)
        {
            ui32OverrideRegAddr = IOC_PB6_OVER;
        }
        if(ui8Pin == GPIO_PIN_7)
        {
            ui32OverrideRegAddr = IOC_PB7_OVER;
        }
        break;

    case GPIO_C_BASE:
        if(ui8Pin == GPIO_PIN_0)
        {
            ui32OverrideRegAddr = IOC_PC0_OVER;
        }
        if(ui8Pin == GPIO_PIN_1)
        {
            ui32OverrideRegAddr = IOC_PC1_OVER;
        }
        if(ui8Pin == GPIO_PIN_2)
        {
            ui32OverrideRegAddr = IOC_PC2_OVER;
        }
        if(ui8Pin == GPIO_PIN_3)
        {
            ui32OverrideRegAddr = IOC_PC3_OVER;
        }
        if(ui8Pin == GPIO_PIN_4)
        {
            ui32OverrideRegAddr = IOC_PC4_OVER;
        }
        if(ui8Pin == GPIO_PIN_5)
        {
            ui32OverrideRegAddr = IOC_PC5_OVER;
        }
        if(ui8Pin == GPIO_PIN_6)
        {
            ui32OverrideRegAddr = IOC_PC6_OVER;
        }
        if(ui8Pin == GPIO_PIN_7)
        {
            ui32OverrideRegAddr = IOC_PC7_OVER;
        }
        break;

    case GPIO_D_BASE:
        if(ui8Pin == GPIO_PIN_0)
        {
            ui32OverrideRegAddr = IOC_PD0_OVER;
        }
        if(ui8Pin == GPIO_PIN_1)
        {
            ui32OverrideRegAddr = IOC_PD1_OVER;
        }
        if(ui8Pin == GPIO_PIN_2)
        {
            ui32OverrideRegAddr = IOC_PD2_OVER;
        }
        if(ui8Pin == GPIO_PIN_3)
        {
            ui32OverrideRegAddr = IOC_PD3_OVER;
        }
        if(ui8Pin == GPIO_PIN_4)
        {
            ui32OverrideRegAddr = IOC_PD4_OVER;
        }
        if(ui8Pin == GPIO_PIN_5)
        {
            ui32OverrideRegAddr = IOC_PD5_OVER;
        }
        if(ui8Pin == GPIO_PIN_6)
        {
            ui32OverrideRegAddr = IOC_PD6_OVER;
        }
        if(ui8Pin == GPIO_PIN_7)
        {
            ui32OverrideRegAddr = IOC_PD7_OVER;
        }
        break;

    default:
        // Default to port A pin 0
        ui32OverrideRegAddr = IOC_PA0_OVER;
        break;
    }

    //
    // Return pin drive type
    //
    return(HWREG(ui32OverrideRegAddr));
}
//*****************************************************************************
//
//! Close the Doxygen group.
//! @}
//
//*****************************************************************************
