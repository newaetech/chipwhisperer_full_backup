ENTRY(Reset_Handler)

_Min_Heap_Size = 0;
_Min_Stack_Size = 0xFF;
_estack = 0x20004000;

MEMORY
{
FLASH (rx) : ORIGIN = 0x00200000, LENGTH = 0x0007FFD4
FLASH_CCA (rx) : ORIGIN = 0x0027FFD4, LENGTH = 12
SRAM_RETENTION (rwx) : ORIGIN = 0x20000000, LENGTH = 0x4000
SRAM_NON_RETENTION (rwx) : ORIGIN = 0x20000000 + 0x4000, LENGTH = 0x4000
}

SECTIONS
{
	.isr_vector : 
	{
	 . = ALIGN(4);
	 KEEP (*(.isr_vector))
	 . = ALIGN(4);
	} > FLASH

	.text :
	{
	 . = ALIGN(4);
	 *(.text)
	 *(.text*)
	 *(.glue_7)
	 *(.glue_7t)
	 *(.eh_frame)

	 KEEP (*(.init))
	 KEEP (*(.fini))
	 . = ALIGN(4);
	 _etext = .;
	} > FLASH

	.const :
	{
	 . = ALIGN(4);
	 *(.const)
	 *(.const*)
	 . = ALIGN(4);
	} > FLASH

	.rodata :
	{
	 . = ALIGN(4);
	 *(.rodata)
	 *(.rodata*)
	 . = ALIGN(4);
	} > FLASH

	.ARM.extab	: {
		. = ALIGN(4);
		*(.ARM.extab* .gnu.linkonce.armextab.*)
		. = ALIGN(4);
	} > FLASH

	.ARM : {
	 . = ALIGN(4);
	 __exidx_start = .;
	 *(.ARM.exidx*)
	 __exidx_end = .;
	 . = ALIGN(4);
	} >FLASH

	.preinit_array : 
	{
	 . = ALIGN(4);
	PROVIDE_HIDDEN (__preinit_array_start = .);
	KEEP (*(.preinit_array*))
	PROVIDE_HIDDEN (__preinit_array_end = .);
	 . = ALIGN(4);
	} > FLASH

	.init_array :
	{
	 . = ALIGN(4);
	PROVIDE_HIDDEN (__init_array_start = .);
	KEEP (*(SORT(.init_array.*)))
	KEEP (*(.init_array*))
	PROVIDE_HIDDEN (__init_array_end = .);
	 . = ALIGN(4);
	} > FLASH

	.flashcca : 
	{
	 . = ALIGN(4);
	 *(.flashcca)
	 *(.flashcca*)
	 . = ALIGN(4);
	} > FLASH_CCA

	.vtable	: 
	{
	 . = ALIGN(4);
	 *(.vtable)
	 *(.vtable*)
	 . = ALIGN(4);
	} > SRAM_RETENTION

	_sidata = LOADADDR(.data);

	.data : 
	{
	 . = ALIGN(4);
	 _sdata = .;
	 *(.data)
	 *(.data*)
	 . = ALIGN(4);
	 _edata = .;
	} > SRAM_RETENTION

	. = ALIGN(4);
	.bss :
	{
	 _sbss = .;
	 __bss_start__ = _sbss;
	 *(.bss)
	 *(.bss*)
	 *(COMMON)
	 
	 . = ALIGN(4);
	 _ebss = .;
	 __bss_end__ = _ebss;
	} > SRAM_RETENTION
	.stack : 
	{
	 . = ALIGN(8);
	 PROVIDE ( end = . );
	 PROVIDE ( _end = .);
	 . = . + _Min_Heap_Size;
	 . = . + _Min_Stack_Size;
	 . = ALIGN(8);
	} > SRAM_RETENTION
}
