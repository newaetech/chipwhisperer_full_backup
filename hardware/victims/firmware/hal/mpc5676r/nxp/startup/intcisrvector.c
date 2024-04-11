/**************************************************************************/
/* FILE NAME: INTCISRVECTOR.c               COPYRIGHT (c) Freescale 2012  */
/*                                                All Rights Reserved     */
/* DESCRIPTION:                                                           */
/* This file contains a table of ISRs for INTC in software (SW) vector    */
/* mode. The table contains addresses for 1024 ISR vectors.               */
/* Table address stards at base of section ".isrvectbl"                   */
/* This file is an example Interrupt Service Routine call file with       */
/* ISR's be replaced by the user application code as needed.              */
/*                                                                        */
/*========================================================================*/
/* REV      AUTHOR        DATE        DESCRIPTION OF CHANGE               */
/* ---   -----------    ----------    ---------------------               */
/* 0.1   S. Mihalik      07/Jun/04    Initial version                     */
/* 0.2	  R. Moran       23/Feb/07    Modified for specific test ISRs	  */
/* 0.21	  I. Harris      11/Jan/10    Modified for specific test ISRs	  */
/* 0.23   I.Harris       01/Feb/11    Modified for GTM FPGA               */
/* 0.23   I.Harris       01/Feb/12    Modified for MPC5764M               */
/**************************************************************************/
#include "typedefs.h"
/*========================================================================*/
/*		PROTOTYPES						  */
/*========================================================================*/
void dummy 						  (void);

/*========================================================================*/
/*	GLOBAL VARIABLES						  */
/*========================================================================*/
        
//const uint32_t __attribute__ ((section (".intc_vector_table"))) IntcIsrVectorTable[];
/*========================================================================*/
/*			FUNCTIONS													  */
/*========================================================================*/
  
/**************************************************************************/
/* FUNCTION     : dummy													  */
/* PURPSE      : Dummy function which is entered if any non-used vector   */
/*							is called.									  */
/**************************************************************************/
void dummy (void) {
     
    while (1){};                 
	    
}  
