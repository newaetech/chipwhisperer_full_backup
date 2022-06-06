/*!
   ZTEX Firmware Kit for EZ-USB FX2 Microcontrollers
   Copyright (C) 2009-2011 ZTEX GmbH.
   http://www.ztex.de

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 3 as
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see http://www.gnu.org/licenses/.
!*/

/*
    Common functions for FPGA configuration from flash memory
*/    


__code BYTE fpga_flash_boot_id[] = {'Z','T','E', 'X', 'B', 'S', '\1', '\1'};

/* *********************************************************************
   ***** fpga_configure_from_flash *************************************
   ********************************************************************* */
/* 
    Configure the FPGA using a bitstream from flash.
    If force == 0 a already configured FPGA is not re-configured.
    Return values:
	0 : Configuration successful
	1 : FPGA already configured
	2 : Flash error
	3 : No bitstream found
	4 : Configuration error

    The format of the boot sector (secor 0 of the flash memeory) is
	0..7	ID, must be "ZTEXBS",1,1
	8..9	Number of used sectors, or 0 is disabled
	10..11  Number of bytes in the last sector, i.e. th total size of bitstream is ((bs[8] | (bs[9]<<8) - 1) * flash_sector_size + ((bs[10] | (bs[11]<<8)
	the rest of the sector is reserved for future use and preserved by the host software
*/

BYTE fpga_configure_from_flash( BYTE force) {
    WORD i,j,k;
    if ( force == 0 ) {
	OEA &= ~bmBIT1;
	if ( IOA1 )  {			// FPGA already configured
	    IOA1 = 1;		
	    OEA |= bmBIT1;
	    return 1;
	}
    }
    {
	PRE_FPGA_RESET
    }
    reset_fpga_flash();			// reset FPGA

    // read the boot sector
    if ( flash_read_init( 0 ) )		// prepare reading sector 0
	return 2;
    for ( i=0; i<8 && flash_read_byte()==fpga_flash_boot_id[i]; i++ );
    if ( i != 8 ) {
	flash_read_finish(flash_sector_size - i);	// dummy-read the rest of the sector + finish read opration
	return 3;
    }
    i = flash_read_byte();
    i |= flash_read_byte() << 8;
    j = flash_read_byte();
    j |= flash_read_byte() << 8;
    flash_read_finish(flash_sector_size - 12);		// dummy-read the rest of the sector + finish read opration
    if ( i==0 )
	return 3;

    // read the bitstream
    if ( flash_read_init( 1 ) )			// prepare reading sector 1
        return 2;
    for ( k=1; k<i; k++ ) {
	fpga_send_bitstream_from_flash( flash_sector_size );
	if ( flash_read_next() )		// prepare reading next sector
    	    return 2;
    }
    fpga_send_bitstream_from_flash( j );
    flash_read_finish(flash_sector_size - j);	// finish read opration
    
    finish_fpga_configuration();		// finish the FPGA configuration

    OEA &= ~bmBIT1;
    if ( IOA1 )  {				// FPGA configured
	IOA1 = 1;		
	OEA |= bmBIT1;
	return 0;
    }
    
    IOA1 = 1;		
    OEA |= bmBIT1;
    return 4;					// FPGA not configured correctly
}

/* *********************************************************************
   ***** fpga_first_free_sector ****************************************
   ********************************************************************* */
// First free sector. Returns 0 if no boot sector exeists.   
// Use the macro FLASH_FIRST_FREE_SECTOR instead of this function.
#define[FLASH_FIRST_FREE_SECTOR][fpga_first_free_sector()];
WORD fpga_first_free_sector() {
    BYTE i,j;
    flash_read_init( 0 ); 				// prepare reading sector 0
    for ( i=0; i<8 && flash_read_byte()==fpga_flash_boot_id[i]; i++ );
    if ( i != 8 ) {
	flash_read_finish(flash_sector_size - i);	// dummy-read the rest of the sector + finish read opration
	return 0;
    }
    i=flash_read_byte();
    j=flash_read_byte();
    flash_read_finish(flash_sector_size - 10);		// dummy-read the rest of the sector + finish read opration
    
    return (i | (j<<8))+1;
}


/* *********************************************************************
   ***** fpga_configure_from_flash_init ********************************
   ********************************************************************* */
// this function is colled by init_USB;
void fpga_configure_from_flash_init() {
    if ( ! flash_enabled ) {
        fpga_flash_result = 2;
        return;
    }

    fpga_flash_result = fpga_configure_from_flash(0);
    if ( fpga_flash_result == 1 ) {
    	post_fpga_config();
    }
    else if ( fpga_flash_result == 4 ) {
	fpga_flash_result = fpga_configure_from_flash(0);	// up to two tries
    }
}    


#define[INIT_CMDS;][INIT_CMDS;
fpga_flash_result= 255;
]