/******************************************************************************
*  Filename:       gpio.c
*  Revised:        $Date: 2013-04-29 09:36:44 +0200 (Mon, 29 Apr 2013) $
*  Revision:       $Revision: 9922 $
*
*  Description:    Driver for the GPIO controller.
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
//! \addtogroup gpio_api
//! @{
//
//*****************************************************************************

#include "hw_gpio.h"
#include "hw_ints.h"
#include "hw_sys_ctrl.h"
#include "hw_memmap.h"
#include "debug.h"
#include "gpio.h"
#include "interrupt.h"
#include "ioc.h"

//*****************************************************************************
//
//! \internal
//! Checks a GPIO base address
//!
//! \param ui32Port is the base address of the GPIO port.
//!
//! This function determines if a GPIO port base address is valid.
//!
//! \return Returns \b true if the base address is valid and \b false
//! otherwise.
//
//*****************************************************************************
#ifdef ENABLE_ASSERT
static bool
GPIOBaseValid(uint32_t ui32Port)
{
    return((ui32Port == GPIO_A_BASE) || (ui32Port == GPIO_B_BASE) ||
           (ui32Port == GPIO_C_BASE) || (ui32Port == GPIO_D_BASE));
}
#endif

//*****************************************************************************
//
//! \internal
//! Gets the GPIO interrupt number
//!
//! \param ui32Port is the base address of the GPIO port.
//!
//! Given a GPIO base address, returns the corresponding interrupt number.
//!
//! \return Returns a GPIO interrupt number, or 0 if \e ui32Port is invalid.
//
//*****************************************************************************
uint32_t
GPIOGetIntNumber(uint32_t ui32Port)
{
    uint32_t ui32Int;

    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));

    //
    // Determine the GPIO interrupt number for the given module.
    //
    switch(ui32Port)
    {
    case GPIO_A_BASE:
    {
        ui32Int = INT_GPIOA;
        break;
    }

    case GPIO_B_BASE:
    {
        ui32Int = INT_GPIOB;
        break;
    }

    case GPIO_C_BASE:
    {
        ui32Int = INT_GPIOC;
        break;
    }

    case GPIO_D_BASE:
    {
        ui32Int = INT_GPIOD;
        break;
    }

    default:
    {
        return(0);
    }
    }

    //
    // Return GPIO interrupt number.
    //
    return(ui32Int);
}

//*****************************************************************************
//
//! Sets the direction and mode of the specified pin(s)
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pins is the bit-packed representation of the pin(s).
//! \param ui32PinIO is the pin direction and/or mode.
//!
//! This function sets the specified pin(s) on the selected GPIO port
//! as either an input or output under software control or sets the
//! pin to be under hardware control.
//!
//! The parameter \e ui32PinIO is an enumerated data type that can be one of
//! the following values:
//!
//! - \b GPIO_DIR_MODE_IN
//! - \b GPIO_DIR_MODE_OUT
//! - \b GPIO_DIR_MODE_HW
//!
//! where \b GPIO_DIR_MODE_IN specifies that the pin will be programmed as
//! a software controlled input, \b GPIO_DIR_MODE_OUT specifies that the pin
//! will be programmed as a software controlled output, and
//! \b GPIO_DIR_MODE_HW specifies that the pin will be placed under
//! hardware control.
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \return None
//
//*****************************************************************************
void
GPIODirModeSet(uint32_t ui32Port, uint8_t ui8Pins,
               uint32_t ui32PinIO)
{

    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));
    ASSERT((ui32PinIO == GPIO_DIR_MODE_IN) || (ui32PinIO == GPIO_DIR_MODE_OUT) ||
           (ui32PinIO == GPIO_DIR_MODE_HW));

    //
    // Set the pin direction and mode.
    //
    HWREG(ui32Port + GPIO_O_DIR)   = ((ui32PinIO & GPIO_DIR_MODE_OUT) ?
                                      (HWREG(ui32Port + GPIO_O_DIR) | ui8Pins) :
                                      (HWREG(ui32Port + GPIO_O_DIR) & ~(ui8Pins)));
    HWREG(ui32Port + GPIO_O_AFSEL) = ((ui32PinIO & GPIO_DIR_MODE_HW) ?
                                      (HWREG(ui32Port + GPIO_O_AFSEL) | ui8Pins) :
                                      (HWREG(ui32Port + GPIO_O_AFSEL) & ~(ui8Pins)));
}

//*****************************************************************************
//
//! Gets the direction and mode of a pin
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pin is the pin number.
//!
//! This function gets the direction and control mode for a specified pin on
//! the selected GPIO port.  The pin can be configured as either an input or
//! output under software control, or it can be under hardware control.  The
//! type of control and direction are returned as an enumerated data type.
//!
//! \return Returns one of the enumerated data types described for
//! GPIODirModeSet().
//
//*****************************************************************************
uint32_t
GPIODirModeGet(uint32_t ui32Port, uint8_t ui8Pin)
{
    uint32_t ui32Dir;
    uint32_t ui32AFSEL;

    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));
    ASSERT(ui8Pin < 8);

    //
    // Convert from a pin number to a bit position.
    //
    ui8Pin = 1 << ui8Pin;

    //
    // Return the pin direction and mode.
    //
    ui32Dir   = HWREG(ui32Port + GPIO_O_DIR);
    ui32AFSEL = HWREG(ui32Port + GPIO_O_AFSEL);
    return(((ui32Dir & ui8Pin) ? GPIO_DIR_MODE_OUT : GPIO_DIR_MODE_IN) |
           ((ui32AFSEL & ui8Pin) ? GPIO_DIR_MODE_HW : GPIO_DIR_MODE_IN));
}

//*****************************************************************************
//
//! Sets the interrupt type for the specified pin(s)
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pins is the bit-packed representation of the pin(s).
//! \param ui32IntType specifies the type of interrupt trigger mechanism.
//!
//! This function sets up the various interrupt trigger mechanisms for the
//! specified pin(s) on the selected GPIO port.
//!
//! The parameter \e ui32IntType is an enumerated data type that can be one of
//! the following values:
//!
//! - \b GPIO_FALLING_EDGE
//! - \b GPIO_RISING_EDGE
//! - \b GPIO_BOTH_EDGES
//! - \b GPIO_LOW_LEVEL
//! - \b GPIO_HIGH_LEVEL
//!
//! where the different values describe the interrupt detection mechanism
//! (edge or level) and the particular triggering event (falling, rising,
//! or both edges for edge detect, low or high for level detect).
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \note To avoid any spurious interrupts, the user must
//! ensure that the GPIO inputs remain stable for the duration of
//! this function.
//!
//! \return None
//
//*****************************************************************************
void
GPIOIntTypeSet(uint32_t ui32Port, uint8_t ui8Pins,
               uint32_t ui32IntType)
{

    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));
    ASSERT((ui32IntType == GPIO_FALLING_EDGE) ||
           (ui32IntType == GPIO_RISING_EDGE) || (ui32IntType == GPIO_BOTH_EDGES) ||
           (ui32IntType == GPIO_LOW_LEVEL)  || (ui32IntType == GPIO_HIGH_LEVEL));

    //
    // Set the pin interrupt type.
    //
    HWREG(ui32Port + GPIO_O_IBE) = ((ui32IntType & 1) ?
                                    (HWREG(ui32Port + GPIO_O_IBE) | ui8Pins) :
                                    (HWREG(ui32Port + GPIO_O_IBE) & ~(ui8Pins)));
    HWREG(ui32Port + GPIO_O_IS) = ((ui32IntType & 2) ?
                                   (HWREG(ui32Port + GPIO_O_IS) | ui8Pins) :
                                   (HWREG(ui32Port + GPIO_O_IS) & ~(ui8Pins)));
    HWREG(ui32Port + GPIO_O_IEV) = ((ui32IntType & 4) ?
                                    (HWREG(ui32Port + GPIO_O_IEV) | ui8Pins) :
                                    (HWREG(ui32Port + GPIO_O_IEV) & ~(ui8Pins)));
}

//*****************************************************************************
//
//! Gets the interrupt type for a pin
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pin is the pin number.
//!
//! This function gets the interrupt type for a specified pin on the selected
//! GPIO port.  The pin can be configured as a falling edge, rising edge, or
//! both edge detected interrupt, or can be configured as a low level or
//! high level detected interrupt.  The type of interrupt detection mechanism
//! is returned as an enumerated data type.
//!
//! \return Returns one of the enumerated data types described for
//! GPIOIntTypeSet().
//
//*****************************************************************************
uint32_t
GPIOIntTypeGet(uint32_t ui32Port, uint8_t ui8Pin)
{
    uint32_t ui32IBE, ui32IS, ui32IEV;

    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));
    ASSERT(ui8Pin < 8);

    //
    // Convert from a pin number to a bit position.
    //
    ui8Pin = 1 << ui8Pin;

    //
    // Return the pin interrupt type.
    //
    ui32IBE = HWREG(ui32Port + GPIO_O_IBE);
    ui32IS  = HWREG(ui32Port + GPIO_O_IS);
    ui32IEV = HWREG(ui32Port + GPIO_O_IEV);
    return(((ui32IBE & ui8Pin) ? 1 : 0) | ((ui32IS & ui8Pin) ? 2 : 0) |
           ((ui32IEV & ui8Pin) ? 4 : 0));
}

//*****************************************************************************
//
//! Enables interrupts for the specified pin(s)
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pins is the bit-packed representation of the pin(s).
//!
//! Unmasks the interrupt for the specified pin(s).
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \return None
//
//*****************************************************************************
void
GPIOPinIntEnable(uint32_t ui32Port, uint8_t ui8Pins)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));

    //
    // Enable the interrupts.
    //
    HWREG(ui32Port + GPIO_O_IE) |= ui8Pins;
}

//*****************************************************************************
//
//! Disables interrupts for the specified pin(s)
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pins is the bit-packed representation of the pin(s).
//!
//! Masks the interrupt for the specified pin(s)
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \return None
//
//*****************************************************************************
void
GPIOPinIntDisable(uint32_t ui32Port, uint8_t ui8Pins)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));

    //
    // Disable the interrupts.
    //
    HWREG(ui32Port + GPIO_O_IE) &= ~(ui8Pins);
}

//*****************************************************************************
//
//! Gets interrupt status for the specified GPIO port
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param bMasked specifies whether masked or raw interrupt status is
//! returned.
//!
//! If \e bMasked is set as \b true, then the masked interrupt status is
//! returned; otherwise, the raw interrupt status is returned.
//!
//! \return Returns a bit-packed byte, where each bit that is set identifies
//! an active masked or raw interrupt, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//! Bits 31:8 should be ignored.
//
//*****************************************************************************
uint32_t
GPIOPinIntStatus(uint32_t ui32Port, bool bMasked)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));

    //
    // Return the interrupt status.
    //
    if(bMasked)
    {
        return(HWREG(ui32Port + GPIO_O_MIS));
    }
    else
    {
        return(HWREG(ui32Port + GPIO_O_RIS));
    }
}

//*****************************************************************************
//
//! Clears the interrupt for the specified pin(s)
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pins is the bit-packed representation of the pin(s).
//!
//! Clears the interrupt for the specified pin(s).
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \note The write buffer in the Cortex-M3 processor can cause the interrupt 
//! source to take several clock cycles before clearing.
//! Therefore, TI recommends clearing the interrupt source early in the 
//! interrupt handler (as opposed to the very last action) to avoid
//! returning from the interrupt handler before the interrupt source is
//! actually cleared.  Failure to clear the interrupt source early can result in
//! the interrupt handler being immediately reentered (because NVIC still sees
//! the interrupt source asserted).
//!
//! \return None
//
//*****************************************************************************
void
GPIOPinIntClear(uint32_t ui32Port, uint8_t ui8Pins)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));

    //
    // Clear the interrupts.
    //
    HWREG(ui32Port + GPIO_O_IC) = ui8Pins;
}

//*****************************************************************************
//
//! Registers an interrupt handler for a GPIO port
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param pfnHandler is a pointer to the GPIO port interrupt handling
//! function.
//!
//! This function ensures that the interrupt handler specified by
//! \e pfnHandler is called when an interrupt is detected from the selected
//! GPIO port.  This function also enables the corresponding GPIO interrupt
//! in the interrupt controller; individual pin interrupts and interrupt
//! sources must be enabled with GPIOPinIntEnable().
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \return None
//
//*****************************************************************************
void
GPIOPortIntRegister(uint32_t ui32Port, void (*pfnHandler)(void))
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));

    //
    // Get the interrupt number associated with the specified GPIO.
    //
    ui32Port = GPIOGetIntNumber(ui32Port);

    //
    // Register the interrupt handler.
    //
    IntRegister(ui32Port, pfnHandler);

    //
    // Enable the GPIO interrupt.
    //
    IntEnable(ui32Port);
}

//*****************************************************************************
//
//! Removes an interrupt handler for a GPIO port
//!
//! \param ui32Port is the base address of the GPIO port.
//!
//! This function unregisters the interrupt handler for the specified
//! GPIO port.  This function also disables the corresponding
//! GPIO port interrupt in the interrupt controller; individual GPIO interrupts
//! and interrupt sources must be disabled with GPIOPinIntDisable().
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \return None
//
//*****************************************************************************
void
GPIOPortIntUnregister(uint32_t ui32Port)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));

    //
    // Get the interrupt number associated with the specified GPIO.
    //
    ui32Port = GPIOGetIntNumber(ui32Port);

    //
    // Disable the GPIO interrupt.
    //
    IntDisable(ui32Port);

    //
    // Unregister the interrupt handler.
    //
    IntUnregister(ui32Port);
}

//*****************************************************************************
//
//! Reads the values present of the specified pin(s)
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pins is the bit-packed representation of the pin(s).
//!
//! The values at the specified pin(s) are read, as specified by \e ui8Pins.
//! Values are returned for both input and output pin(s), and the value
//! for pin(s) that are not specified by \e ui8Pins are set to 0.
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \return Returns a bit-packed byte providing the state of the specified
//! pin, where bit 0 of the byte represents GPIO port pin 0, bit 1 represents
//! GPIO port pin 1, and so on.  Any bit that is not specified by \e ui8Pins
//! is returned as a 0.  Bits 31:8 should be ignored.
//
//*****************************************************************************
uint32_t
GPIOPinRead(uint32_t ui32Port, uint8_t ui8Pins)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));

    //
    // Return the pin value(s).
    //
    return(HWREG(ui32Port + (GPIO_O_DATA + (ui8Pins << 2))));
}

//*****************************************************************************
//
//! Writes a value to the specified pin(s)
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pins is the bit-packed representation of the pin(s).
//! \param ui8Val is the value to write to the pin(s).
//!
//! Writes the corresponding bit values to the output pin(s) specified by
//! \e ui8Pins.  Writing to a pin configured as an input pin has no effect.
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \return None
//
//*****************************************************************************
void
GPIOPinWrite(uint32_t ui32Port, uint8_t ui8Pins, uint8_t ui8Val)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));

    //
    // Write the pins.
    //
    HWREG(ui32Port + (GPIO_O_DATA + (ui8Pins << 2))) = ui8Val;
}

//*****************************************************************************
//
//! Configures pin(s) for use as GPIO inputs
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pins is the bit-packed representation of the pin(s).
//!
//! The GPIO pins must be properly configured in order to function correctly as
//! GPIO inputs.  This function provides the proper configuration for those
//! pin(s).
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \return None
//
//*****************************************************************************
void
GPIOPinTypeGPIOInput(uint32_t ui32Port, uint8_t ui8Pins)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));

    //
    // Make the pin(s) be inputs.
    //
    GPIODirModeSet(ui32Port, ui8Pins, GPIO_DIR_MODE_IN);

    //
    // Set the pad(s) to no override of the drive type.
    //
    IOCPadConfigSet(ui32Port, ui8Pins, IOC_OVERRIDE_DIS);
}

//*****************************************************************************
//
//! Configures pin(s) for use as GPIO outputs
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pins is the bit-packed representation of the pin(s).
//!
//! The GPIO pins must be properly configured to function correctly as
//! GPIO outputs.  This function provides the proper configuration for those
//! pin(s).
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \return None
//
//*****************************************************************************
void
GPIOPinTypeGPIOOutput(uint32_t ui32Port, uint8_t ui8Pins)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));

    //
    // Make the pin(s) be outputs.
    //
    GPIODirModeSet(ui32Port, ui8Pins, GPIO_DIR_MODE_OUT);

    //
    // Set the pad(s) no override of the drive type.
    //
    IOCPadConfigSet(ui32Port, ui8Pins, IOC_OVERRIDE_DIS);
}

//*****************************************************************************
//
//! Configures pin(s) for use by the I2C peripheral
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pins is the bit-packed representation of the pin(s).
//!
//! The I2C pins must be properly configured for the I2C peripheral to function
//! correctly.  This function provides the proper configuration for those
//! pin(s).
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \note This function cannot be used to turn any pin into an I2C pin; it only
//! configures an I2C pin for proper operation.
//!
//! \return None
//
//*****************************************************************************
void
GPIOPinTypeI2C(uint32_t ui32Port, uint8_t ui8Pins)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));

    //
    // Make the pin(s) be peripheral controlled.
    //
    GPIODirModeSet(ui32Port, ui8Pins, GPIO_DIR_MODE_HW);

    //
    // Set the pad(s) to no drive type.
    //
    IOCPadConfigSet(ui32Port, ui8Pins, IOC_OVERRIDE_DIS);
}

//*****************************************************************************
//
//! Configures pin(s) for use by the SSI peripheral
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pins is the bit-packed representation of the pin(s).
//!
//! The SSI pins must be properly configured for the SSI peripheral to function
//! correctly.  This function provides a typical configuration for those
//! pin(s); other configurations might work as well depending upon the board
//! setup (for example, using the on-chip pull-ups).
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \note This function cannot be used to turn any pin into a SSI pin; but only
//! configures an SSI pin for proper operation.
//!
//! \return None
//
//*****************************************************************************
void
GPIOPinTypeSSI(uint32_t ui32Port, uint8_t ui8Pins)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));

    //
    // Make the pin(s) be peripheral controlled.
    //
    GPIODirModeSet(ui32Port, ui8Pins, GPIO_DIR_MODE_HW);

    //
    // Set the pad(s) to no drive type.
    //
    IOCPadConfigSet(ui32Port, ui8Pins, IOC_OVERRIDE_DIS);
}

//*****************************************************************************
//
//! Configures pin(s) for use by the Timer peripheral
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pins is the bit-packed representation of the pin(s).
//!
//! The CCP pins must be properly configured for the timer peripheral to
//! function correctly.  This function provides a typical configuration for
//! those pin(s); other configurations might work as well depending upon the
//! board setup (for example, using the on-chip pull-ups).
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \note This function cannot be used to turn any pin into a timer pin but only
//! configures a timer pin for proper operation.
//!
//! \return None
//
//*****************************************************************************
void
GPIOPinTypeTimer(uint32_t ui32Port, uint8_t ui8Pins)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));

    //
    // Make the pin(s) be peripheral controlled.
    //
    GPIODirModeSet(ui32Port, ui8Pins, GPIO_DIR_MODE_HW);

    //
    // Set the pad(s) to no drive type.
    //
    IOCPadConfigSet(ui32Port, ui8Pins, IOC_OVERRIDE_DIS);
}

//*****************************************************************************
//
//! Configures input pin(s) for use by the UART peripheral
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pins is the bit-packed representation of the pin(s).
//!
//! The UART input pins must be properly configured for the UART peripheral to
//! function correctly.  This function provides a typical configuration for
//! those pin(s); other configurations might work as well depending upon the
//! board setup (for example, using the on-chip pull-ups).
//!
//! \note For PC0 through PC3 the function GPIOPinTypeUARTHiDrive() should
//! be used to configure these high drive pins.
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \note This function cannot be used to turn any pin into a UART pin; but only
//! configures a UART pin for proper operation.
//!
//! \return None
//
//*****************************************************************************
void
GPIOPinTypeUARTInput(uint32_t ui32Port, uint8_t ui8Pins)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));
    ASSERT(!((ui32Port == GPIO_C_BASE) && ((ui8Pins & 0xf) > 0)));

    //
    // Make the pin(s) be peripheral controlled.
    //
    GPIODirModeSet(ui32Port, ui8Pins, GPIO_DIR_MODE_HW);

    //
    // Set the pad(s) to override disable.
    //
    IOCPadConfigSet(ui32Port, ui8Pins, IOC_OVERRIDE_DIS);
}

//*****************************************************************************
//
//! Configures output pin(s) for use by the UART peripheral
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pins is the bit-packed representation of the pin(s).
//!
//! The UART output pins must be properly configured for the UART peripheral to
//! function correctly.  This function provides a typical configuration for
//! those pin(s); other configurations might work as well depending upon the
//! board setup (for example, using the on-chip pull-ups).
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \note This function cannot be used to turn any pin into a UART pin; but only
//! configures a UART pin for proper operation.
//!
//! \return None
//
//*****************************************************************************
void
GPIOPinTypeUARTOutput(uint32_t ui32Port, uint8_t ui8Pins)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));
    ASSERT(!((ui32Port == GPIO_C_BASE) && ((ui8Pins & 0xf) > 0)));

    //
    // Make the pin(s) be peripheral controlled.
    //
    GPIODirModeSet(ui32Port, ui8Pins, GPIO_DIR_MODE_HW);

    //
    // Set the pad(s) to output enable.
    //
    IOCPadConfigSet(ui32Port, ui8Pins, IOC_OVERRIDE_OE);
}

//*****************************************************************************
//
//! Sets the power-up interrupt type for the specified pin(s)
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pins is the bit-packed representation of the pin(s).
//! \param ui32IntType specifies type of power-up interrupt trigger mechanism.
//!
//! This function sets up the various interrupt trigger mechanisms for the
//! specified pin(s) on the selected GPIO port.
//!
//! The parameter \e ui32IntType is an enumerated data type that can be one of
//! the following values:
//!
//! - \b GPIO_POW_FALLING_EDGE
//! - \b GPIO_POW_RISING_EDGE
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \note To avoid any spurious interrupts, the user must
//! ensure that the GPIO inputs remain stable for the duration of
//! this function.
//!
//! \return None
//
//*****************************************************************************
void
GPIOPowIntTypeSet(uint32_t ui32Port, uint8_t ui8Pins,
                  uint32_t ui32IntType)
{
    uint32_t ui32PortOffset;
    uint32_t ui32IntPins;

    //
    // Initialize value
    //
    ui32PortOffset = 0;

    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));
    ASSERT((ui32IntType == GPIO_POW_FALLING_EDGE) ||
           (ui32IntType == GPIO_POW_RISING_EDGE));

    //
    // Find bit mask for wanted pin(s)
    //
    if(ui32Port == GPIO_A_BASE)
    {
        ui32PortOffset = 0;
    }
    if(ui32Port == GPIO_B_BASE)
    {
        ui32PortOffset = 8;
    }
    if(ui32Port == GPIO_C_BASE)
    {
        ui32PortOffset = 16;
    }
    if(ui32Port == GPIO_D_BASE)
    {
        ui32PortOffset = 24;
    }
    ui32IntPins = ui8Pins << ui32PortOffset;

    //
    // Set the pin interrupt type.
    //
    if(ui32IntType == GPIO_POW_FALLING_EDGE)
    {
        HWREG(ui32Port + GPIO_O_P_EDGE_CTRL) |= ui32IntPins;
    }
    else  // GPIO_POW_RAISING_EDGE
    {
        HWREG(ui32Port + GPIO_O_P_EDGE_CTRL) &= ~(ui32IntPins);
    }
}

//*****************************************************************************
//
//! Gets the power-up interrupt type for a pin
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pin is the pin number.
//!
//! This function gets the interrupt type for a specified pin on the selected
//! GPIO port.  The pin can be configured as a falling edge, rising edge, or
//! both edge detected interrupt, or it can be configured as a low level or
//! high level detected interrupt.  The type of interrupt detection mechanism
//! is returned as an enumerated data type.
//!
//! \return Returns one of the enumerated data types described for
//! GPIOIntTypeSet().
//
//*****************************************************************************
uint32_t
GPIOPowIntTypeGet(uint32_t ui32Port, uint8_t ui8Pin)
{
    uint32_t ui32PortOffset;
    uint32_t ui32IntPin;

    //
    // Initialize value
    //
    ui32PortOffset = 0;

    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));
    ASSERT(ui8Pin < 8);

    //
    // Convert from a port- pin number to a bit position.
    //
    if(ui32Port == GPIO_A_BASE)
    {
        ui32PortOffset = 0;
    }
    if(ui32Port == GPIO_B_BASE)
    {
        ui32PortOffset = 8;
    }
    if(ui32Port == GPIO_C_BASE)
    {
        ui32PortOffset = 16;
    }
    if(ui32Port == GPIO_D_BASE)
    {
        ui32PortOffset = 24;
    }
    ui32IntPin = 1 << (ui8Pin + ui32PortOffset);

    //
    // Return the pin interrupt type.
    //
    if(HWREG(ui32Port + GPIO_O_P_EDGE_CTRL) & ui32IntPin)
    {
        return(GPIO_POW_FALLING_EDGE);
    }
    else
    {
        return(GPIO_POW_RISING_EDGE);
    }
}

//*****************************************************************************
//
//! Enables power-up interrupts for the specified pin(s)
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pins is the bit-packed representation of the pin(s).
//!
//! Unmasks the interrupt for the specified pin(s).
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \return None
//
//*****************************************************************************
void
GPIOPowIntEnable(uint32_t ui32Port, uint8_t ui8Pins)
{
    uint32_t ui32PortOffset;
    uint32_t ui32IntPins;

    //
    // Initialize value
    //
    ui32PortOffset = 0;

    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));

    //
    // Find bit mask for wanted pin(s)
    //
    if(ui32Port == GPIO_A_BASE)
    {
        ui32PortOffset = 0;
    }
    if(ui32Port == GPIO_B_BASE)
    {
        ui32PortOffset = 8;
    }
    if(ui32Port == GPIO_C_BASE)
    {
        ui32PortOffset = 16;
    }
    if(ui32Port == GPIO_D_BASE)
    {
        ui32PortOffset = 24;
    }
    ui32IntPins = ui8Pins << ui32PortOffset;

    //
    // Enable the interrupts.
    //
    HWREG(ui32Port + GPIO_O_PI_IEN) |= ui32IntPins;
}

//*****************************************************************************
//
//! Disables power-up interrupts for the specified pin(s)
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pins is the bit-packed representation of the pin(s).
//!
//! Masks the interrupt for the specified pin(s).
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \return None
//
//*****************************************************************************
void
GPIOPowIntDisable(uint32_t ui32Port, uint8_t ui8Pins)
{
    uint32_t ui32PortOffset;
    uint32_t ui32IntPins;

    //
    // Initialize value
    //
    ui32PortOffset = 0;

    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));

    //
    // Find bit mask for wanted pin(s)
    //
    if(ui32Port == GPIO_A_BASE)
    {
        ui32PortOffset = 0;
    }
    if(ui32Port == GPIO_B_BASE)
    {
        ui32PortOffset = 8;
    }
    if(ui32Port == GPIO_C_BASE)
    {
        ui32PortOffset = 16;
    }
    if(ui32Port == GPIO_D_BASE)
    {
        ui32PortOffset = 24;
    }
    ui32IntPins = ui8Pins << ui32PortOffset;

    //
    // Disable the interrupts.
    //
    HWREG(ui32Port + GPIO_O_PI_IEN) &= ~(ui32IntPins);
}

//*****************************************************************************
//
//! Gets power-up interrupt status for the specified GPIO port
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param bMasked specifies whether masked or raw interrupt status is
//! returned.
//!
//! If \e bMasked is set as \b true, then the masked interrupt status is
//! returned; otherwise, the raw interrupt status is returned.
//!
//! \return Returns a bit-packed byte, where each bit that is set identifies
//! an active masked or raw interrupt, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//! Bits 31:8 should be ignored.
//
//*****************************************************************************
uint32_t
GPIOPowIntStatus(uint32_t ui32Port, bool bMasked)
{
    uint32_t ui32PortOffset;

    //
    // Initialize value
    //
    ui32PortOffset = 0;

    // Check the arguments.
    ASSERT(GPIOBaseValid(ui32Port));

    //
    // Find bit mask for wanted pin(s)
    //
    if(ui32Port == GPIO_A_BASE)
    {
        ui32PortOffset = 0;
    }
    if(ui32Port == GPIO_B_BASE)
    {
        ui32PortOffset = 8;
    }
    if(ui32Port == GPIO_C_BASE)
    {
        ui32PortOffset = 16;
    }
    if(ui32Port == GPIO_D_BASE)
    {
        ui32PortOffset = 24;
    }

    // Return the interrupt status.
    if(bMasked)
    {
        return((HWREG(ui32Port + GPIO_O_IRQ_DETECT_ACK) >> ui32PortOffset) &
               0xFF);
    }
    else
    {
        return((HWREG(ui32Port + GPIO_O_IRQ_DETECT_UNMASK) >> ui32PortOffset) &
               0xFF);
    }
}

//*****************************************************************************
//
//! Clears the power-up interrupt for the specified pin(s)
//!
//! \param ui32Port is the base address of the GPIO port.
//! \param ui8Pins is the bit-packed representation of the pin(s).
//!
//! Clears the interrupt for the specified pin(s).
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \return None
//
//*****************************************************************************
void
GPIOPowIntClear(uint32_t ui32Port, uint8_t ui8Pins)
{
    uint32_t ui32PortOffset;
    uint32_t ui32IntPins;

    //
    // Initialize value
    //
    ui32PortOffset = 0;

    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ui32Port));

    //
    // Find bit mask for wanted pin(s)
    //
    if(ui32Port == GPIO_A_BASE)
    {
        ui32PortOffset = 0;
    }
    if(ui32Port == GPIO_B_BASE)
    {
        ui32PortOffset = 8;
    }
    if(ui32Port == GPIO_C_BASE)
    {
        ui32PortOffset = 16;
    }
    if(ui32Port == GPIO_D_BASE)
    {
        ui32PortOffset = 24;
    }
    ui32IntPins = ui8Pins << ui32PortOffset;

    //
    // Clear the interrupts.
    //
    HWREG(ui32Port + GPIO_O_IRQ_DETECT_ACK) |= ui32IntPins;
}

//*****************************************************************************
//
//! Enable Wake Up Interrupt
//!
//! \param ui32Config is the source to enable wake up on interrupt.
//!
//! This function enables wake up on interrupt from the selected sources.
//!
//! The \e ui32Config argument must be one or the logical or of several of
//! the following values:
//!
//! \b GPIO_IWE_PORT_A, \b GPIO_IWE_PORT_B, \b GPIO_IWE_PORT_C,
//! \b GPIO_IWE_PORT_D, \b GPIO_IWE_USB,
//! \b GPIO_IWE_SM_TIMER.
//!
//! \return None
//
//*****************************************************************************
void
GPIOIntWakeupEnable(uint32_t ui32Config)
{
    ASSERT((ui32Config &
            (GPIO_IWE_PORT_A |
             GPIO_IWE_PORT_B |
             GPIO_IWE_PORT_C |
             GPIO_IWE_PORT_D |
             GPIO_IWE_USB |
             GPIO_IWE_SM_TIMER)) != 0);

    //
    // Enable Wakeup from selected Interrupt sources
    //
    HWREG(SYS_CTRL_IWE) |= ui32Config;
}

//*****************************************************************************
//
//! Disable Wake Up Interrupt
//!
//! \param ui32Config is the source to disable wake on interrupt from.
//!
//! This function disables Wake up on interrupt from the selected sources.
//!
//! The \e ui32Config argument must be one or the logical or of several of
//! the following values:
//!
//! \b GPIO_IWE_PORT_A, \b GPIO_IWE_PORT_B, \b GPIO_IWE_PORT_C,
//! \b GPIO_IWE_PORT_D, \b GPIO_IWE_USB,
//! \b GPIO_IWE_SM_TIMER,
//!
//! \return None
//
//*****************************************************************************
void
GPIOIntWakeupDisable(uint32_t ui32Config)
{
    ASSERT((ui32Config &
            (GPIO_IWE_PORT_A |
             GPIO_IWE_PORT_B |
             GPIO_IWE_PORT_C |
             GPIO_IWE_PORT_D |
             GPIO_IWE_USB |
             GPIO_IWE_SM_TIMER)) != 0);

    //
    // Disable Wakeup from selected Interrupt sources
    //
    HWREG(SYS_CTRL_IWE) &= ~ui32Config;
}

//*****************************************************************************
//! Close the Doxygen group.
//! @}
//*****************************************************************************
