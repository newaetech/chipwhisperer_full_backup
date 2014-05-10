/*
  SPI Driver for ChipWhisperer using AVR.
*/

/*
  Copyright 2014  Colin O'Flynn (coflynn [at] newae [dot] com)
  
  Based on GenericHID Example which is:  
  Copyright 2014  Dean Camera (dean [at] fourwalledcubicle [dot] com)  

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#include "ChipWhispererSPI.h"

typedef void (*BootPtr_t)(void) __attribute__ ((noreturn));
BootPtr_t BootPtr = (BootPtr_t)0x1800;

//Use PD5 (PDIC Pin) as CS
#define SETUP_CS()  (DDRD |= 1<<5)
#define CS_HIGH()   (PORTD |= 1<<5)
#define CS_LOW()    (PORTD &= ~(1<<5))

/** Buffer to hold the previously generated HID report, for comparison purposes inside the HID class driver. */
static uint8_t PrevHIDReportBuffer[GENERIC_REPORT_SIZE];

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t Generic_HID_Interface =
	{
		.Config =
			{
				.InterfaceNumber              = INTERFACE_ID_GenericHID,
				.ReportINEndpoint             =
					{
						.Address              = GENERIC_IN_EPADDR,
						.Size                 = GENERIC_EPSIZE,
						.Banks                = 1,
					},
				.PrevReportINBuffer           = PrevHIDReportBuffer,
				.PrevReportINBufferSize       = sizeof(PrevHIDReportBuffer),
			},
	};


/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
	SetupHardware();
    SETUP_CS();
    CS_HIGH();

	// Initialize the SPI driver before first use
    // NB: Should check the SPI_SAMPLE_RISING etc for your own use
    SPI_Init(SPI_SPEED_FCPU_DIV_128 | SPI_ORDER_MSB_FIRST | SPI_SCK_LEAD_RISING |
             SPI_SAMPLE_LEADING | SPI_MODE_MASTER);
             
    DDRB |= (1 << 0);
 
	GlobalInterruptEnable();

	for (;;)
	{
		HID_Device_USBTask(&Generic_HID_Interface);
		USB_USBTask();
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	/* Hardware Initialization */
	USB_Init();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	//LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	//LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Generic_HID_Interface);

	USB_Device_EnableSOFEvents();
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	HID_Device_ProcessControlRequest(&Generic_HID_Interface);
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
	HID_Device_MillisecondElapsed(&Generic_HID_Interface);
}

uint8_t spi_data[64];
uint8_t spi_wait;

/** HID class driver callback function for the creation of HID reports to the host.
 *
 *  \param[in]     HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in,out] ReportID    Report ID requested by the host if non-zero, otherwise callback should set to the generated report ID
 *  \param[in]     ReportType  Type of the report to create, either HID_REPORT_ITEM_In or HID_REPORT_ITEM_Feature
 *  \param[out]    ReportData  Pointer to a buffer where the created report should be stored
 *  \param[out]    ReportSize  Number of bytes written in the report (or zero if no report is to be sent)
 *
 *  \return Boolean true to force the sending of the report, false to let the library determine if it needs to be sent
 */
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                         uint8_t* const ReportID,
                                         const uint8_t ReportType,
                                         void* ReportData,
                                         uint16_t* const ReportSize)
{
	*ReportID = 0;
	uint8_t* Data = (uint8_t*)ReportData;

	if(spi_wait > 0){		
	
		if(spi_wait > (GENERIC_REPORT_SIZE-1)){
			spi_wait = GENERIC_REPORT_SIZE-1;
		}
	
        Data[0] = 0x80 | spi_wait;
    
		for(uint8_t i = 0; i < (GENERIC_REPORT_SIZE-1); i++){
			Data[i+1] = spi_data[i];
		}
		spi_wait = 0;	
		*ReportSize = GENERIC_REPORT_SIZE;
		return true;
	}
		
	*ReportSize = 0;
	return false;
}

/** HID class driver callback function for the processing of HID reports from the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in] ReportID    Report ID of the received report from the host
 *  \param[in] ReportType  The type of report that the host has sent, either HID_REPORT_ITEM_Out or HID_REPORT_ITEM_Feature
 *  \param[in] ReportData  Pointer to a buffer where the received report has been stored
 *  \param[in] ReportSize  Size in bytes of the received HID report
 */
 
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
	uint8_t* Data = (uint8_t*)ReportData;	
	
    /* Sending 0xFE as first byte means jump to bootloader */
    if (*Data == 0xfe) {
        USB_Detach();
		_delay_ms(400);
		cli();
		USB_Attach();
		BootPtr();
        return;    
    }
    
    /* Sending 0x00 as first byte means send SPI command */
    if (*Data == 0x01) {
        /* Get number of valid bytes */
        uint8_t spibytes = *(++Data);
        uint8_t bindex = 0;
        
        //Max size is 64-2 = 62 bytes
        if (spibytes > (GENERIC_EPSIZE - 2)){
            return;
        }
        
        CS_LOW();        
        while(spibytes){
            ++Data;
            spi_data[bindex] = SPI_TransferByte(*Data);
            bindex++;
            spibytes--;
        }
        CS_HIGH();
        
        spi_wait = bindex;    
    }
}
