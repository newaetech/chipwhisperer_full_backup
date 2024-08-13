
/*
 * Name: iROM.ld 
 * 
 * Generated Linker Description File
 * Copyright (C) 2010 HighTec EDV-Systeme GmbH. 
 * (!Do not edit outsite of the protection areas!)
 *
 * Description: 
 * internal flash configuration
 */

/*
 * Define Entrypoint of Executable
 */
ENTRY(_start)

/*
 * Global
 */
/*Program Flash Memory (PFLASH0)*/ 
__PMU_PFLASH0_BEGIN = 0xA0000000;
__PMU_PFLASH0_SIZE = 2M;
/*Data Flash Memory (DFLASH0)*/ 
__PMU_DFLASH0_BEGIN = 0xAF000000;
__PMU_DFLASH0_SIZE = 128K;
/*Boot ROM (BROM)*/ 
__BROM_BEGIN = 0x8FFF8000;
__BROM_SIZE = 32K;
/*Scratch-Pad RAM (PSPR)*/ 
__PMI_PSPR_BEGIN = 0xC0000000;
__PMI_PSPR_SIZE = 8K;
/*Local Data RAM (DSPR)*/ 
__DMI_DSPR_BEGIN = 0xD0000000;
__DMI_DSPR_SIZE = 184K;
/*Local Data RAM (DSPR)*/ 
__LMU_SRAM_BEGIN = 0x90000000;
__LMU_SRAM_SIZE = 32K;


__USTACK_SIZE = DEFINED (__USTACK_SIZE) ? __USTACK_SIZE : 4K;  /* Section for ustack*/ 
__ISTACK_SIZE = DEFINED (__ISTACK_SIZE) ? __ISTACK_SIZE : 1K;  /* Section for istack*/ 
__HEAP_SIZE = DEFINED (__HEAP_SIZE) ? __HEAP_SIZE : 4K;  /* Section for heap*/ 
__CSA_SIZE = DEFINED (__CSA_SIZE) ? __CSA_SIZE : 16K;  /* Section for CSA*/ 

/**
 *	User defined global region
 */
/*PROTECTED REGION ID(Protection:iROM-Global) ENABLED START*/
/*Protection-Area for your own LDF-Code*/
/*PROTECTED REGION END*/

/*
 * internal flash configuration
 */
MEMORY
{
	PMU_PFLASH0 (rx!p):	org = 0xA0000000, len = 2M  /*Program Flash Memory (PFLASH0)*/ 
	PMU_DFLASH0 (r!xp):	org = 0xAF000000, len = 128K  /*Data Flash Memory (DFLASH0)*/ 
	BROM (rx!p):	org = 0x8FFF8000, len = 32K  /*Boot ROM (BROM)*/ 
	PMI_PSPR (wx!p):	org = 0xC0000000, len = 8K  /*Scratch-Pad RAM (PSPR)*/ 
	DMI_DSPR (w!xp):	org = 0xD0000000, len = 184K  /*Local Data RAM (DSPR)*/ 
	LMU_SRAM (w!xp):	org = 0x90000000, len = 32K  /*Local Data RAM (DSPR)*/ 
	
}			

SECTIONS
{
	/*Code-Sections*/
	
	/*
	 * Startup code for TriCore
	 */
	.startup_code  :
	{
		PROVIDE(__startup_code_start = .);
		
		/*PROTECTED REGION ID(Protection: iROM .startup_code.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.startup_code) /*Startup code for TriCore*/ 
		*(.startup_code*)
		
		/*PROTECTED REGION ID(Protection: iROM .startup_code) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__startup_code_end = .);
		. = ALIGN(8);
		
	} > PMU_PFLASH0 /* PMU_PFLASH0: Program Flash Memory (PFLASH0) */ 
	
	/*
	 * Code section
	 */
	.text  :
	{
		PROVIDE(__text_start = .);
		
		/*PROTECTED REGION ID(Protection: iROM .text.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.text) /*Code section*/ 
		*(.text*)
		*(.gnu.linkonce.t.*)
		
		/*PROTECTED REGION ID(Protection: iROM .text) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__text_end = .);
		. = ALIGN(8);
		
	} > PMU_PFLASH0 /* PMU_PFLASH0: Program Flash Memory (PFLASH0) */ 
	
	/*
	 * Code executed before calling main
	 */
	.init  :
	{
		PROVIDE(__init_start = .);
		
		/*PROTECTED REGION ID(Protection: iROM .init.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		KEEP(*(.init)) /*Code executed before calling main*/ 
		KEEP(*(.init*))
		
		/*PROTECTED REGION ID(Protection: iROM .init) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__init_end = .);
		. = ALIGN(8);
		
	} > PMU_PFLASH0 /* PMU_PFLASH0: Program Flash Memory (PFLASH0) */ 
	
	/*
	 * Code executed before exiting program
	 */
	.fini  :
	{
		PROVIDE(__fini_start = .);
		
		/*PROTECTED REGION ID(Protection: iROM .fini.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		KEEP(*(.fini)) /*Code executed before exiting program*/ 
		KEEP(*(.fini*))
		
		/*PROTECTED REGION ID(Protection: iROM .fini) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__fini_end = .);
		. = ALIGN(8);
		
	} > PMU_PFLASH0 /* PMU_PFLASH0: Program Flash Memory (PFLASH0) */ 
	
	/*
	 * Section for trap table
	 */
	.traptab  :
	{
		PROVIDE(__traptab_start = .);
		
		/*PROTECTED REGION ID(Protection: iROM .traptab.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.traptab) /*Section for trap table*/ 
		*(.traptab*)
		
		/*PROTECTED REGION ID(Protection: iROM .traptab) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__traptab_end = .);
		. = ALIGN(8);
		
	} > PMU_PFLASH0 /* PMU_PFLASH0: Program Flash Memory (PFLASH0) */ 
	
	/*
	 * Section for interrupt table
	 */
	.inttab  :
	{
		PROVIDE(__inttab_start = .);
		
		/*PROTECTED REGION ID(Protection: iROM .inttab.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.inttab) /*Section for interrupt table*/ 
		*(.inttab*)
		
		/*PROTECTED REGION ID(Protection: iROM .inttab) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__inttab_end = .);
		. = ALIGN(8);
		
	} > PMU_PFLASH0 /* PMU_PFLASH0: Program Flash Memory (PFLASH0) */ 
	
	/*
	 * Exception handling frame for C++ exceptions
	 */
	.eh_frame  :
	{
		PROVIDE(__eh_frame_start = .);
		
		/*PROTECTED REGION ID(Protection: iROM .eh_frame.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.eh_frame) /*Exception handling frame for C++ exceptions*/ 
		*(.eh_frame*)
		
		/*PROTECTED REGION ID(Protection: iROM .eh_frame) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__eh_frame_end = .);
		. = ALIGN(8);
		
	} > PMU_PFLASH0 /* PMU_PFLASH0: Program Flash Memory (PFLASH0) */ 
	



	/*Absolute Data-Sections*/
	
	/*
	 * Initialised data addressed as absolute
	 */
	.zdata  :
	{
		PROVIDE(__zdata_start = .);
		
		/*PROTECTED REGION ID(Protection: iROM .zdata.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.zdata) /*Initialised data addressed as absolute*/ 
		*(.zdata*)
		*(.zdata.rodata) /*absolute addressable readonly data*/ 
		*(.zdata.rodata*)
		*(.zrodata) /*absolute addressable readonly data*/ 
		*(.zrodata*)
		*(.gnu.linkonce.z.*)
		*(.gnu.linkonce.zr.*)
		
		/*PROTECTED REGION ID(Protection: iROM .zdata) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/

		PROVIDE(__zdata_end = .);
		. = ALIGN(8);
		
	} > DMI_DSPR AT > PMU_PFLASH0 /* DMI_DSPR: Local Data RAM (DSPR) */ /* PMU_PFLASH0: Program Flash Memory (PFLASH0) */ 
	
	/*
	 * Not initialised data addressed as absolute
	 */
	.zbss (NOLOAD) :
	{
		PROVIDE(__zbss_start = .);
		
		/*PROTECTED REGION ID(Protection: iROM .zbss.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.zbss) /*Not Initialised data addressed as absolute*/ 
		*(.zbss*)
		*(.gnu.linkonce.zb.*)
		
		/*PROTECTED REGION ID(Protection: iROM .zbss) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/

		PROVIDE(__zbss_end = .);
		. = ALIGN(8);
		
	} > DMI_DSPR /* DMI_DSPR: Local Data RAM (DSPR) */ 
	
	/*
	 * Not initialised bit data
	 */
	.bbss (NOLOAD) :
	{
		PROVIDE(__bbss_start = .);
		
		/*PROTECTED REGION ID(Protection: iROM .bbss.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.bbss) /*Not initialised bit data*/ 
		*(.bbss*)
		
		/*PROTECTED REGION ID(Protection: iROM .bbss) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/

		PROVIDE(__bbss_end = .);
		. = ALIGN(8);
		
	} > DMI_DSPR /* DMI_DSPR: Local Data RAM (DSPR) */ 
	
	/*
	 * Bit variables
	 */
	.bdata  :
	{
		PROVIDE(__bdata_start = .);
		
		/*PROTECTED REGION ID(Protection: iROM .bdata.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.bdata) /*Bit variables*/ 
		*(.bdata*)
		
		/*PROTECTED REGION ID(Protection: iROM .bdata) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/

		PROVIDE(__bdata_end = .);
		. = ALIGN(8);
		
	} > DMI_DSPR AT > PMU_PFLASH0 /* DMI_DSPR: Local Data RAM (DSPR) */ /* PMU_PFLASH0: Program Flash Memory (PFLASH0) */ 
	


	/*Small Data-Sections*/
	
	/*
	 * Storage of write-protected data addressed as small
	 */
	.sdata2  :
	{
		PROVIDE(__sdata2_start = .);
		
		/*PROTECTED REGION ID(Protection: iROM .sdata2.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.sdata.rodata) /*Storage of write-protected data addressed as small*/ 
		*(.sdata.rodata*)
		*(.gnu.linkonce.sr.*)
		
		/*PROTECTED REGION ID(Protection: iROM .sdata2) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__sdata2_end = .);
		. = ALIGN(8);
		
	} > PMU_PFLASH0 /* PMU_PFLASH0: Program Flash Memory (PFLASH0) */ 
	
	/*
	 * Section stores initialised data which is addressable by small data area pointer (%a0)
	 */
	.sdata  :
	{
		PROVIDE(__sdata_start = .);
		
		/*PROTECTED REGION ID(Protection: iROM .sdata.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.sdata) /*Section stores initialised data which is addressable by small data area pointer (%a0)*/ 
		*(.sdata*)
		*(.gnu.linkonce.s.*)
		
		/*PROTECTED REGION ID(Protection: iROM .sdata) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__sdata_end = .);
		. = ALIGN(8);
		
	} > DMI_DSPR AT > PMU_PFLASH0 /* DMI_DSPR: Local Data RAM (DSPR) */ /* PMU_PFLASH0: Program Flash Memory (PFLASH0) */ 
	
	/*
	 * Not initialised data in section â€™.sbssâ€™, addressable by small data area pointer (%a0)
	 */
	.sbss (NOLOAD) :
	{
		PROVIDE(__sbss_start = .);
		
		/*PROTECTED REGION ID(Protection: iROM .sbss.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.sbss) /*Not initialised data in section â€™.sbssâ€™, addressable by small data area pointer (%a0)*/ 
		*(.sbss*)
		*(.gnu.linkonce.sb.*)
		
		/*PROTECTED REGION ID(Protection: iROM .sbss) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__sbss_end = .);
		. = ALIGN(8);
		
	} > DMI_DSPR /* DMI_DSPR: Local Data RAM (DSPR) */ 
	


	/*Normal Data-Sections*/
	
	/*
	 * Storage of write-protected data
	 */
	.rodata  :
	{
		PROVIDE(__rodata_start = .);
		
		/*PROTECTED REGION ID(Protection: iROM .rodata.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.rodata) /*Storage of write-protected data*/ 
		*(.rodata*)
		*(.gnu.linkonce.r.*)
		*(.jcr.*)
		
		/*PROTECTED REGION ID(Protection: iROM .rodata) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__rodata_end = .);
		. = ALIGN(8);
		
	} > PMU_PFLASH0 /* PMU_PFLASH0: Program Flash Memory (PFLASH0) */ 
	
	/*
	 * Initialised data
	 */
	.data  :
	{
		PROVIDE(__data_start = .);
		
		/*PROTECTED REGION ID(Protection: iROM .data.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.data) /*Initialised data*/ 
		*(.data*)
		*(.gnu.linkonce.d.*)
		
		/*PROTECTED REGION ID(Protection: iROM .data) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__data_end = .);
		. = ALIGN(8);
		
	} > DMI_DSPR AT > PMU_PFLASH0 /* DMI_DSPR: Local Data RAM (DSPR) */ /* PMU_PFLASH0: Program Flash Memory (PFLASH0) */ 
	
	/*
	 * Not Initialised data
	 */
	.bss (NOLOAD) :
	{
		PROVIDE(__bss_start = .);
		
		/*PROTECTED REGION ID(Protection: iROM .bss.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.bss) /*Not Initialised data*/ 
		*(.bss*)
		*(.gnu.linkonce.b.*)
		
		/*PROTECTED REGION ID(Protection: iROM .bss) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__bss_end = .);
		. = ALIGN(8);
		
	} > DMI_DSPR /* DMI_DSPR: Local Data RAM (DSPR) */ 
	
	

	/*PCP-Sections*/
	
	/*
	 * PCP code section
	 */
	.pcptext  :
	{
		PROVIDE(__pcptext_start = .);
		
		/*PROTECTED REGION ID(Protection: iROM .pcptext.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		
		/*PROTECTED REGION ID(Protection: iROM .pcptext) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__pcptext_end = .);
		. = ALIGN(8);
		
	} > PMU_PFLASH0 /* PMU_PFLASH0: Program Flash Memory (PFLASH0) */ 
	 
	/*
	 * PCP data section
	 */
	.pcpdata  :
	{
		PROVIDE(__pcpdata_start = .);
		
		/*PROTECTED REGION ID(Protection: iROM .pcpdata.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		
		/*PROTECTED REGION ID(Protection: iROM .pcpdata) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__pcpdata_end = .);
		. = ALIGN(8);
		
	} > PMU_PFLASH0 /* PMU_PFLASH0: Program Flash Memory (PFLASH0) */ 
	 

	 
	 
	/*
	 * Section for constructors
	 */
	.ctors          :
	{
		__CTOR_LIST__ = . ;

		/*PROTECTED REGION ID(Protection: iROMconstructor.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2);
		KEEP (*crtbegin.o(.ctors))
		KEEP (*(EXCLUDE_FILE (*crtend.o ) .ctors))
		KEEP (*(SORT(.ctors.*)))
		KEEP (*(.ctors))
		LONG(0) ;
		/*PROTECTED REGION ID(Protection: iROMconstructor) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		__CTOR_END__ = . ;
		 . = ALIGN(8);
	}  > PMU_PFLASH0


	/*
	 * Section for destructors
	 */
	.dtors          :
	{
		__DTOR_LIST__ = . ;
		/*PROTECTED REGION ID(Protection: iROM destructor.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2);
		KEEP (*crtbegin.o(.dtors))
		KEEP (*(EXCLUDE_FILE (*crtend.o ) .dtors))
		KEEP (*(SORT(.dtors.*)))
		KEEP (*(.dtors))
		LONG(0) ;
		/*PROTECTED REGION ID(Protection: iROM destructor) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		__DTOR_END__ = . ;
		. = ALIGN(8);
	} > PMU_PFLASH0
	
	
	/*
	 * Section for clear table
	 */
	.clear_sec :
	{
		. = ALIGN(8);
		PROVIDE(__clear_table = .) ;
			LONG(0 + ADDR(.bss));	LONG(SIZEOF(.bss));
			LONG(0 + ADDR(.sbss));	LONG(SIZEOF(.sbss));
			LONG(0 + ADDR(.zbss));	LONG(SIZEOF(.zbss));
			LONG(0 + ADDR(.bbss));	LONG(SIZEOF(.bbss));
			/*PROTECTED REGION ID(Protection: iROM clear section) ENABLED START*/
				/*Protection-Area for your own LDF-Code*/
			/*PROTECTED REGION END*/
			LONG(-1);                 LONG(-1);
	} > PMU_PFLASH0
	 
	/*
	* Section for copy table
	*/	
	.copy_sec :
	{
		. = ALIGN(8);
		PROVIDE(__copy_table = .) ;
			LONG(LOADADDR(.data));	LONG(0 + ADDR(.data));	LONG(SIZEOF(.data));
			LONG(LOADADDR(.sdata));	LONG(0 + ADDR(.sdata));	LONG(SIZEOF(.sdata));
			LONG(LOADADDR(.zdata));	LONG(0 + ADDR(.zdata));	LONG(SIZEOF(.zdata));
			LONG(LOADADDR(.bdata));	LONG(0 + ADDR(.bdata));	LONG(SIZEOF(.bdata));
			/*PROTECTED REGION ID(Protection: iROM copy section) ENABLED START*/
				/*Protection-Area for your own LDF-Code*/
			/*PROTECTED REGION END*/
			LONG(-1);                 LONG(-1);                  LONG(-1);
	} > PMU_PFLASH0 
	 
	
	/*
	 * Section for ustack
	 */	
	.ustack :
	{
		. = ALIGN(8);
		 __USTACK_BEGIN = . ;
		. += __USTACK_SIZE ;
		. = ALIGN(8);
		 __USTACK = . ;
		 __USTACK_END = . ;
	} > DMI_DSPR
	
	/*
	 * Section for istack
	 */	
	.istack :
	{
		. = ALIGN(8);
		 __ISTACK_BEGIN = . ;
		. += __ISTACK_SIZE ;
		. = ALIGN(8);
		 __ISTACK = . ;
		 __ISTACK_END = . ;
	} > DMI_DSPR
	
	/*
	 * Section for heap
	 */	
	.heap :
	{
		. = ALIGN(8);
		 __HEAP_BEGIN = . ;
		 __HEAP = . ;
		. += __HEAP_SIZE ;
		. = ALIGN(8);
		 __HEAP_END = . ;
	} > DMI_DSPR
	
	/*
	 * Section for CSA
	 */	
	.csa :
	{
		. = ALIGN(64);
		 __CSA_BEGIN = . ;
		 __CSA = . ;
		. += __CSA_SIZE ;
		. = ALIGN(64);
		 __CSA_END = . ;
	} > DMI_DSPR
	
	
	
	
	/*PROTECTED REGION ID(Protection:iROM-User-Sections) ENABLED START*/
		/*Protection-Area for your own LDF-Code*/
	/*PROTECTED REGION END*/
	
}

