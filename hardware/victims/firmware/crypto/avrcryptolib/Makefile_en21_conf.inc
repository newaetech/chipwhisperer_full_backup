
MCU_TARGET     = atmega128
F_CPU          = 14745600
OPTIMIZE       = -Os # -Os
EXTRALINK      = xram.o
DEFS           = -D$(call uc, $(MCU_TARGET)) -DF_CPU=$(F_CPU)
BOARD_NAME     = ethernut2.1
FLASHCMD       = /bin/bash openocd_flash.sh #
RESETCMD       = 

override CFLAGS_A	= -MMD -MF$(DEP_DIR)$(patsubst %.o,%.d,$(notdir $(1))) $(DEBUG) $(WARNING) -std=$(CSTD) $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override CFLAGS 	= -MMD -MF$(DEP_DIR)$(patsubst %.o,%.d,$(notdir $@))   $(DEBUG) $(WARNING) -std=$(CSTD) $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)

override LDFLAGS       = -gdwarf-2 -Wl,--section-start,.data=0x801100,--defsym=__heap_end=0x807fff -Wl,-Map,#
override ASFLAGS	   = -mmcu=$(MCU_TARGET) -Wa,--gdwarf-2

