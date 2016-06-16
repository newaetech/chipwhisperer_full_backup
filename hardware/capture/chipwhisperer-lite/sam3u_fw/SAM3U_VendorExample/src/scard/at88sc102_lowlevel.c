/* CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at src/license_cddl-1.0.txt
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at src/license_cddl-1.0.txt
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*! \file   smartcard.c
*   \brief  Smart Card low level functions
*   Copyright [2014] [Mathieu Stephan]
*/
#include <asf.h>
#include "at88sc102_highlevel.h"
#include "at88sc102_lowlevel.h"

#define scard_sck_low() ioport_set_pin_level(SCARD_SCK_IDX, false)
#define scard_sck_high() ioport_set_pin_level(SCARD_SCK_IDX, true)

#define scard_aux1_low() ioport_set_pin_level(SCARD_AUX1_IDX, false)
#define scard_aux1_high() ioport_set_pin_level(SCARD_AUX1_IDX, true)

#define scard_aux2_low() ioport_set_pin_level(SCARD_AUX2_IDX, false)
#define scard_aux2_high() ioport_set_pin_level(SCARD_AUX2_IDX, true)

#define scard_rst_low() ioport_set_pin_level(SCARD_RST_IDX, false)
#define scard_rst_high() ioport_set_pin_level(SCARD_RST_IDX, true)

#define scard_io_low() ioport_set_pin_level(SCARD_IOTX_IDX, false)
#define scard_io_high() ioport_set_pin_level(SCARD_IOTX_IDX, true)

#define scard_io_read() ioport_get_pin_level(SCARD_IORX_IDX)

#define scard_pgm_low scard_aux2_low
#define scard_pgm_high scard_aux2_high
#define scard_fus_low scard_aux1_low
#define scard_fus_high scard_aux1_high

/** Counter for successive card detects **/
volatile uint8_t card_detect_counter = 0;
/** Current detection state, see detect_return_t */
volatile uint8_t button_return;

void smartcardHPulseDelay(void);
void smartcardPowerDelay(void);
static inline void smartcardTchpDelay(void);
void clockPulseSMC(void);
void invertedClockPulseSMC(void);


/*! \fn     smartcardHPulseDelay(void)
*   \brief  2us half pulse delay, specified by datasheet (min 3.3us/2)
*/
void smartcardHPulseDelay(void)
{
    delay_us(2);
}

/*! \fn     smartcardPowerDelay(void)
*   \brief  Delay to let the card come online/offline
*/
void smartcardPowerDelay(void)
{
    delay_ms(130);
}

/*! \fn     smartcardTchpDelay(void)
*   \brief  Tchp delay (3.0ms min)
*/
static inline void smartcardTchpDelay(void)
{
    delay_ms(4);
}

/*! \fn     clockPulseSMC(void)
*   \brief  Send a 4us H->L clock pulse (datasheet: min 3.3us)
*/
void clockPulseSMC(void)
{
	scard_sck_high();
	smartcardHPulseDelay();
	scard_sck_low();
	smartcardHPulseDelay();
}

/*! \fn     invertedClockPulseSMC(void)
*   \brief  Send a 4us L->H clock pulse (datasheet: min 3.3us)
*/
void invertedClockPulseSMC(void)
{
	scard_sck_low();
	smartcardHPulseDelay();
	scard_sck_high();
	smartcardHPulseDelay();
}

/*! \fn     clearPgmRstSignals(void)
*   \brief  Clear PGM / RST signal for normal operation mode
*/
void clearPgmRstSignals(void)
{
	scard_pgm_low();
	scard_rst_low();
    smartcardHPulseDelay();
	smartcardHPulseDelay();
}

/*! \fn     setPgmRstSignals(void)
*   \brief  Set PGM / RST signal for standby mode
*/
void setPgmRstSignals(void)
{
	scard_rst_high();
	scard_pgm_low();
    smartcardHPulseDelay();
}

/*! \fn     performLowLevelWriteNErase(uint8_t is_write)
*   \brief  Perform a write or erase operation on the smart card
*   \param  is_write    Boolean to indicate if it is a write
*/
void performLowLevelWriteNErase(uint8_t is_write)
{
    /* Set programming control signal */
	scard_pgm_high();
    smartcardHPulseDelay();

    /* Set data according to write / erase */
    if (is_write != FALSE)
    {
		scard_io_high();
    }
    else
    {
		scard_io_low();
    }
    smartcardHPulseDelay();

    /* Set clock */
	scard_sck_high();
    smartcardHPulseDelay();

    /* Release program signal and data, wait for tchp */
    scard_pgm_low();
    smartcardTchpDelay();

    /* Release clock */
    scard_sck_low();
    smartcardHPulseDelay();

    /* Release data */
    scard_io_low();
    smartcardHPulseDelay();
}

/*! \fn     setSPIModeSMC(void)
*   \brief  Activate SPI controller for the SMC
*/
void setSPIModeSMC(void)
{
	usart_spi_opt_t opts;
	
	opts.baudrate = 125000;
	opts.char_length = US_MR_CHRL_8_BIT;
	opts.channel_mode = US_MR_CHMODE_NORMAL;
	opts.spi_mode = SPI_MODE_0;
	
	sysclk_enable_peripheral_clock(SCARD_USART_ID);
	usart_init_spi_master(SCARD_USART, &opts ,sysclk_get_cpu_hz());
	gpio_configure_pin(SCARD_IOTX_IDX, PIO_PERIPH_A);
	gpio_configure_pin(SCARD_IORX_IDX, PIO_PERIPH_A);
	gpio_configure_pin(SCARD_SCK_IDX, SCK_PERIPH);
	
	usart_enable_rx(SCARD_USART);
	usart_enable_tx(SCARD_USART);
	
}

/*! \fn     setBBModeAndPgmRstSMC(void)
*   \brief  Switch to big banging, and clear pgm/rst signal for normal operation
*/
void setBBModeAndPgmRstSMC(void)
{
    /* Deactivate SPI port */
    usart_disable_rx(SCARD_USART);
	usart_disable_tx(SCARD_USART);

    /* Clock & data low */
	gpio_configure_pin(SCARD_IOTX_IDX, (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT));
	gpio_configure_pin(SCARD_IORX_IDX, (PIO_TYPE_PIO_INPUT | PIO_DEFAULT));
	gpio_configure_pin(SCARD_SCK_IDX, (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT));
    scard_sck_low();
    scard_io_low();
	
    smartcardHPulseDelay();

    /* Clear PGM and RST signals */
    clearPgmRstSignals();
}

/*! \fn     blowFuse(uint8_t fuse_name)
*   \brief  Blow the manufacturer or issuer fuse
*   \param  fuse_name    Which fuse to blow
*/
void blowFuse(uint8_t fuse_name)
{
    uint16_t i;

    /* Set the index to write */
    if (fuse_name == MAN_FUSE)
    {
        i = 1460;
    }
    else if (fuse_name == ISSUER_FUSE)
    {
        i = 1560;
    }
    else if (fuse_name == EC2EN_FUSE)
    {
        i = 1529;
    }
    else
    {
        i = 0;
    }

    /* Switch to bit banging */
    setBBModeAndPgmRstSMC();

    /* Get to the good index */
    while(i--)clockPulseSMC();

    /* Set RST signal */
	scard_rst_high();

    /* Perform a write */
    performLowLevelWriteNErase(TRUE);

    /* Set PGM / RST signals to standby mode */
    setPgmRstSignals();

    /* Switch to SPI mode */
    setSPIModeSMC();
}


/*! \fn     eraseApplicationZone1NZone2SMC(uint8_t zone1_nzone2)
*   \brief  Set E1 or E2 flag by presenting the correct erase key (always FFFF...) and erase the AZ1 or AZ2
*   \param  zone1_nzone2    Zone 1 or Not Zone 2
*/
void eraseApplicationZone1NZone2SMC(uint8_t zone1_nzone2)
{
    #ifdef SMARTCARD_FUSE_V1        
        uint8_t temp_bool;
    #endif
    uint16_t i;

    /* Which index to go to */
    if (zone1_nzone2 == FALSE)
    {
        i = 1248;
    }
    else
    {
        i = 688;
    }

        /* Switch to bit banging */
        setBBModeAndPgmRstSMC();

        /* Get to the good EZx */
        while(i--) invertedClockPulseSMC();

        /* How many bits to compare */
        if (zone1_nzone2 == FALSE)
        {
            i = 32;
        }
        else
        {
            i = 48;
        }

        /* Clock is at high level now, as input must be switched during this time */
        /* Enter the erase key */
        smartcardHPulseDelay();
        while(i--)
        {
            // The code is always FFFF...
            smartcardHPulseDelay();

            /* Inverted clock pulse */
            invertedClockPulseSMC();
        }

        /* Bring clock and data low */
        scard_sck_low();
        smartcardHPulseDelay();smartcardHPulseDelay();
        scard_io_low();
        smartcardHPulseDelay();smartcardHPulseDelay();
        
        /* In smart card fuse V1 (early versions sent to beta testers), EC2EN is not blown so we're limited to 128 erase operations... */
        #ifdef SMARTCARD_FUSE_V1
            if (zone1_nzone2 == FALSE)
            {               
                i = 0;
                temp_bool = TRUE;
                /* Write one of the four SCAC bits to 0 and check if successful */
                while((temp_bool == TRUE) && (i < 128))
                {
                    /* If one of the four bits is at 1, write a 0 */
                    if (scard_io_read())
                    {
                        /* Set write command */
                        performLowLevelWriteNErase(TRUE);

                        /* Wait for the smart card to output a 0 */
						uint16_t timeout = 0xFF;
                        while(scard_io_read())
						{
							timeout--;
							if(timeout == 0)
							{
								/* Set PGM / RST signals to standby mode */
								setPgmRstSignals();

								/* Switch to SPI mode */
								setSPIModeSMC();
							
								return;
							}
						}

                        /* Exit loop */
                        temp_bool = FALSE;
                    }
                    else
                    {
                        /* Clock pulse */
                        clockPulseSMC();
                        i++;
                    }
                }
            }
        #endif

        /* Erase AZ1/AZ2 */
        performLowLevelWriteNErase(FALSE);

        /* Set PGM / RST signals to standby mode */
        setPgmRstSignals();

        /* Switch to SPI mode */
        setSPIModeSMC();
}

/*! \fn     securityValidationSMC(uint16_t code)
*   \brief  Check security code
*   \param  code    The code
*   \return success_status (see card_detect_return_t)
*/
RET_TYPE securityValidationSMC(uint16_t code, uint8_t reset_after_send)
{
    RET_TYPE return_val = RETURN_PIN_NOK_0;
    uint8_t temp_bool;
    uint16_t i;

    /* Switch to bit banging */
    setBBModeAndPgmRstSMC();

    /* Get to the SC */
    for(i = 0; i < 80; i++)
        invertedClockPulseSMC();

	scard_aux1_high();	

    /* Clock is at high level now, as input must be switched during this time */
    /* Enter the SC */
    smartcardHPulseDelay();
    for(i = 0; i < 16; i++)
    {
        if (((code >> (15-i)) & 0x0001) != 0x0000)
        {
            scard_io_low();
        }
        else
        {
            scard_io_high();
        }
        smartcardHPulseDelay();

        /* Inverted clock pulse */
        invertedClockPulseSMC();
    }

    /* Bring clock and data low */
    scard_sck_low();
    smartcardHPulseDelay();
	smartcardHPulseDelay();
    scard_io_low();
    smartcardHPulseDelay();	
	smartcardHPulseDelay();
	
	if (reset_after_send == 1){
		delay_us(10);
		scard_aux1_low();
		/* Set PGM / RST signals to standby mode */
		setPgmRstSignals();

		/* Switch to SPI mode */
		setSPIModeSMC();
			
		return RETURN_PIN_NOK_0;
	}

	
    i = 0;
    temp_bool = TRUE;
    /* Write one of the four SCAC bits to 0 and check if successful */
    while((temp_bool == TRUE) && (i < 4))
    {
        /* If one of the four bits is at 1, write a 0 */
        if (scard_io_read())
        {
			/* Reset after start of program command */			
			if (reset_after_send == 2){			
			    /* Set programming control signal */
			    scard_pgm_high();
			    smartcardHPulseDelay();

			    /* Write command */
			    scard_io_high();
			    smartcardHPulseDelay();

			    /* Set clock */
			    scard_sck_high();
			    smartcardHPulseDelay();
				
				scard_aux1_low();
				/* Set PGM / RST signals to standby mode */
				setPgmRstSignals();

				/* Switch to SPI mode */
				setSPIModeSMC();
				
				return RETURN_PIN_NOK_0;
			}
			
			
            /* Set write command */
            performLowLevelWriteNErase(TRUE);

            /* Wait for the smart card to output a 0 */
			uint16_t timeout = 0x20;
            while(scard_io_read())
			{
				timeout--;
				if(timeout == 0)
				{
					scard_aux1_low();
					
					/* Set PGM / RST signals to standby mode */
					setPgmRstSignals();

					/* Switch to SPI mode */
					setSPIModeSMC();
					
					return RETURN_PIN_TIMEOUT;
				}
			}
			
			/* Reset after end of program command */
			if (reset_after_send == 3){			
				scard_aux1_low();
				/* Set PGM / RST signals to standby mode */
				setPgmRstSignals();

				/* Switch to SPI mode */
				setSPIModeSMC();
				
				return RETURN_PIN_NOK_0;
			}

            /* Now, erase SCAC */
            performLowLevelWriteNErase(FALSE);

            /* Were we successful? */
            if (scard_io_read())
            {
                // Success !
                return_val = RETURN_PIN_OK;
            }
            else
            {
                // Wrong pin, return number of tries left
                if (i == 0)
                {
                    return_val = RETURN_PIN_NOK_3;
                }
                else if (i == 1)
                {
                    return_val = RETURN_PIN_NOK_2;
                }
                else if (i == 2)
                {
                    return_val = RETURN_PIN_NOK_1;
                }
                else if (i == 3)
                {
                    return_val = RETURN_PIN_NOK_0;
                }
            }

            /* Clock pulse */
            clockPulseSMC();

            /* Exit loop */
            temp_bool = FALSE;
        }
        else
        {
            /* Clock pulse */
            clockPulseSMC();
            i++;
        }
    }

    /* If we couldn't find a spot to write, no tries left */
    if (i == 4)
    {
        return_val = RETURN_PIN_NOK_0;
    }


	scard_aux1_low();

    /* Set PGM / RST signals to standby mode */
    setPgmRstSignals();

    /* Switch to SPI mode */
    setSPIModeSMC();

    return return_val;
}

/*! \fn     writeSMC(uint16_t start_index_bit, uint16_t nb_bits, uint8_t* data_to_write)
*   \brief  Write bits to the smart card
*   \param  start_index_bit         Where to start writing bits
*   \param  nb_bits                 Number of bits to write
*   \param  data_to_write           Pointer to the buffer
*/
void writeSMC(uint16_t start_index_bit, uint16_t nb_bits, uint8_t* data_to_write)
{
    uint16_t current_written_bit = 0;
    uint8_t masked_bit_to_write = 0;
    uint16_t i;

    /* Switch to bit banging */
    setBBModeAndPgmRstSMC();

    /* Try to not erase AZ1 if EZ1 is 0xFFFFFFF... and we're writing the first bit of the AZ2 */
    if (start_index_bit >= SMARTCARD_AZ2_BIT_START)
    {
        /* Clock pulses until AZ2 start - 1 */
        for(i = 0; i < SMARTCARD_AZ2_BIT_START - 1; i++)
        {
            clockPulseSMC();            
        }                
        scard_io_high();
        clockPulseSMC();
        scard_io_low();
        /* Clock for the rest */
        for(i = 0; i < (start_index_bit - SMARTCARD_AZ2_BIT_START); i++)
        {
            clockPulseSMC();            
        }                
    }
    else
    {
        /* Get to the good index, clock pulses */
        for(i = 0; i < start_index_bit; i++)
        {
            clockPulseSMC();
        }                
    }

    /* Start writing */
    for(current_written_bit = 0; current_written_bit < nb_bits; current_written_bit++)
    {
        /* If we are at the start of a 16bits word or writing our first bit, erase the word */
        if ((((start_index_bit+current_written_bit) & 0x000F) == 0) || (current_written_bit == 0))
        {
            performLowLevelWriteNErase(FALSE);
        }

        /* Get good bit to write */
        masked_bit_to_write = (data_to_write[(current_written_bit>>3)] >> (7 - (current_written_bit & 0x0007))) & 0x01;

        /* Write only if the data is a 0 */
        if (masked_bit_to_write == 0x00)
        {
            performLowLevelWriteNErase(TRUE);
        }

        /* Go to next address */
        clockPulseSMC();
    }

    /* Set PGM / RST signals to standby mode */
    setPgmRstSignals();

    /* Switch to SPI mode */
    setSPIModeSMC();
}

/*! \fn     readSMC(uint8_t nb_bytes_total_read, uint8_t start_record_index, uint8_t* data_to_receive)
*   \brief  Read bytes from the smart card
*   \param  nb_bytes_total_read     The number of bytes to be read
*   \param  start_record_index      The index at which we start recording the answer
*   \param  data_to_receive        Pointer to the buffer
*   \return The buffer
*/
uint8_t* readSMC(uint8_t nb_bytes_total_read, uint8_t start_record_index, uint8_t* data_to_receive)
{
    uint8_t* return_val = data_to_receive;
    uint8_t i;

    /* Set PGM / RST signals for operation */
    clearPgmRstSignals();
	
	/* Read */
	uint32_t temp;
    for(i = 0; i < nb_bytes_total_read; i++)
    {
        /* Start transmission */
		usart_putchar(SCARD_USART, 0x00);
        /* Store data in buffer or discard it*/
		while (!(SCARD_USART->US_CSR & US_CSR_TXRDY));
				
		usart_getchar(SCARD_USART, &temp);
		
        if (i >= start_record_index)
        {			
			*(data_to_receive++) = (uint8_t)temp;
        }
    }

    /* Set PGM / RST signals to standby mode */
    setPgmRstSignals();

    return return_val;
}

/*! \fn     firstDetectFunctionSMC(void)
*   \brief  functions performed once the smart card is detected
*   \return The detection result (see card_detect_return_t)
*/
RET_TYPE firstDetectFunctionSMC(void)
{
    uint8_t data_buffer[2];
    uint16_t *data_buf16 = (uint16_t*)data_buffer;
    uint16_t temp_uint;

	/* Fuse pin low */
	gpio_configure_pin(SCARD_AUX1_IDX, (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT));

    /* Default state: PGM to 0 and RST to 1 */
	gpio_configure_pin(SCARD_PGM_IDX, (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT));
	gpio_configure_pin(SCARD_RST_IDX, (PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT));
    scard_pgm_low();
	scard_rst_high();

    /* Activate SPI port */
    setSPIModeSMC();
    
    /* Let the card come online */
    smartcardPowerDelay();

    /* Check smart card FZ */
    readFabricationZone(data_buffer);
    if ((swap16(*data_buf16)) != SMARTCARD_FABRICATION_ZONE)
    {
        return RETURN_CARD_NDET;
    }

    /* Perform test write on MTZ */
    readMemoryTestZone((uint8_t*)&temp_uint);
    temp_uint = temp_uint + 5;
    writeMemoryTestZone((uint8_t*)&temp_uint);
    if (*(uint16_t*)readMemoryTestZone(data_buffer) != temp_uint)
    {
        return RETURN_CARD_TEST_PB;
    }

    /* Read security code attempts counter */
    switch(getNumberOfSecurityCodeTriesLeft())
    {
        case 4: return RETURN_CARD_4_TRIES_LEFT;
        case 3: return RETURN_CARD_3_TRIES_LEFT;
        case 2: return RETURN_CARD_2_TRIES_LEFT;
        case 1: return RETURN_CARD_1_TRIES_LEFT;
        case 0: return RETURN_CARD_0_TRIES_LEFT;
        default: return RETURN_CARD_0_TRIES_LEFT;
    }
}

/*! \fn     removeFunctionSMC(void)
*   \brief  functions performed once the smart card is removed
*/
void removeFunctionSMC(void)
{
    /* Setup all output pins as tri-state */
    scard_pgm_low();
	gpio_configure_pin(SCARD_AUX1_IDX, (PIO_TYPE_PIO_INPUT | PIO_DEFAULT));
	gpio_configure_pin(SCARD_AUX2_IDX, (PIO_TYPE_PIO_INPUT | PIO_DEFAULT));
	gpio_configure_pin(SCARD_RST_IDX, (PIO_TYPE_PIO_INPUT | PIO_DEFAULT));
	gpio_configure_pin(SCARD_IOTX_IDX, (PIO_TYPE_PIO_INPUT | PIO_DEFAULT));
	gpio_configure_pin(SCARD_IORX_IDX, (PIO_TYPE_PIO_INPUT | PIO_DEFAULT));
	gpio_configure_pin(SCARD_SCK_IDX, (PIO_TYPE_PIO_INPUT | PIO_DEFAULT));
	
    /* Deactivate port */
	usart_disable_rx(SCARD_USART);
	usart_disable_tx(SCARD_USART);
}

/*! \fn     initPortSMC(void)
*   \brief  Initialize smart card port
*/
void initPortSMC(void)
{
    /* Setup card detection input with pull-up */
	gpio_configure_pin(SCARD_PRESENT_IDX, (PIO_TYPE_PIO_INPUT | PIO_DEFAULT | PIO_PULLUP));
	
    /* Setup MISO as input, SS as input with pull-up */
    #if 0
        DDRB &= ~((1 << MISO_SPI_NATIVE) | (1 << SS_SPI_NATIVE));
        PORT_SPI_NATIVE &= ~(1 << MISO_SPI_NATIVE);
        PORT_SPI_NATIVE |= (1 << SS_SPI_NATIVE);
    #endif

    /* Set all output pins as tri-state */
    removeFunctionSMC();
	setPgmRstSignals();
}



// Greg's function
// Walk up to the security code register, enter a bit in the right place, then reset.
void compareBit(uint16_t bit, uint16_t value)
{
    uint16_t i;

    /* Switch to bit banging */
    setBBModeAndPgmRstSMC();

    /* Get to the SC */
    for(i = 0; i < 80 + bit; i++)
		invertedClockPulseSMC();

    scard_aux1_high();

    // Clock is at high level now, as input must be switched during this time */
    // Enter test value
    smartcardHPulseDelay();
	if (value)
	{
	    scard_io_low();
    }
    else
    {
	    scard_io_high();
    }
    smartcardHPulseDelay();
    invertedClockPulseSMC();
	
	// Bring reset high
	scard_rst_high();

    /* Bring clock and data low */
    scard_sck_low();
    smartcardHPulseDelay();
    smartcardHPulseDelay();
    scard_io_low();
    smartcardHPulseDelay();
    smartcardHPulseDelay();
    
    
	// Cleanup
    scard_aux1_low();

    /* Set PGM / RST signals to standby mode */
    setPgmRstSignals();

    /* Switch to SPI mode */
    setSPIModeSMC();

    return;	
}