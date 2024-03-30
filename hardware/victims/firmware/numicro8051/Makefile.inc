#
# SDCC Makefile for mcs51
# Modified for Numicro 8051 series
# ------------------------------------------------------
# PATH
PLATFORM := CW308_N76E003
PINCDIR  = $(FIRMWAREPATH)/inc
OBJDIR  = objdir-$(PLATFORM)
LIBDIR  = $(FIRMWAREPATH)/commonlib
HALDIR = $(FIRMWAREPATH)/hal
TOOLSDIR = $(FIRMWAREPATH)/tools
EXTRAINCDIRS += $(HALDIR)
VPATH += $(LIBDIR):$(HALDIR)

# Platform Specific defines
# ------------------------------------------------------
ifeq ($(PLATFORM),)
    $(error PLATFORM not defined)
endif

# These CPUs aren't really stable at other frequencies when using the internal oscillator
INT_OSC_ALLOWED = 16000000 16600000
ifeq ($(PLATFORM), CW308_N76E003)
    ifeq ($(F_CPU),)
        F_CPU = 16000000
    endif
    SUPPORTS_24MHZ = 0
    INCDIR = $(PINCDIR)/N76E003
    # PRG Size = 18K Bytes
    CODE_SIZE = 18432
    # INT-MEM Size = 256 Bytes
    IRAM_SIZE = 256
    # EXT-MEM Size = 768 Bytes
    XRAM_SIZE = 768
else ifeq ($(PLATFORM), CW308_N76S003)
    ifeq ($(F_CPU),)
        F_CPU = 16000000
    endif
    SUPPORTS_24MHZ = 0
    INCDIR = $(PINCDIR)/N76S003
    # PRG Size = 18K Bytes, -128 for SPROM (maybe????)
    # Spec sheet implies it has an SPROM area, but it is not clear if it actually does.
    # The feature part looks like they sloppily copied it from the MS51 series spec sheet:
    #  - "128-bytes security protection memory SPROM (only for 32KB/16KB APROM)"
    # The MS51 has 32KB/16KB APROM options; the N76S003 only has an 18KB option.
    # It is also missing parts that explain how the SPROM works like the MS51 series spec sheet does.
    # Setting it anyway so that we don't inadvertantly overwrite the last byte and lock it if it does.
    CODE_SIZE = 18304
    # INT-MEM Size = 256 Bytes
    IRAM_SIZE = 256
    # EXT-MEM Size = 1K Bytes
    XRAM_SIZE = 1024
else ifeq ($(PLATFORM), CW308_MS5116K)
    ifeq ($(F_CPU),)
        F_CPU = 24000000
    endif
    SUPPORTS_24MHZ = 1
    INCDIR = $(PINCDIR)/MS5132K
    # PRG Size = 16K Bytes, -128 for the SPROM
    CODE_SIZE = 16256
    # INT-MEM Size = 256 Bytes
    IRAM_SIZE = 256
    # EXT-MEM Size = 1K Bytes
    XRAM_SIZE = 1024
else ifeq ($(PLATFORM), CW308_MS5132K)
    ifeq ($(F_CPU),)
        F_CPU = 24000000
    endif
    SUPPORTS_24MHZ = 1
    CDEFS += -DNUM51_CPU24MHZ_SUPPORTED
    INCDIR = $(PINCDIR)/MS5132K
    # PRG Size = 32K Bytes, -128 for the SPROM
    CODE_SIZE = 32512
    # INT-MEM Size = 256 Bytes
    IRAM_SIZE = 256
    # EXT-MEM Size = 2K Bytes
    XRAM_SIZE = 2048
else
    $(error Unsupported PLATFORM: $(PLATFORM))
endif
# default: Use the External clock
ifeq ($(USE_EXTERNAL_CLOCK),)
    USE_EXTERNAL_CLOCK = 1
endif
ifeq ($(SUPPORTS_24MHZ),1)
    CDEFS += -DNUM51_CPU24MHZ_SUPPORTED
    INT_OSC_ALLOWED = 16000000 16600000 24000000
else
    ifeq ($(shell test $(F_CPU) -gt 16600000; echo $$?),0)
        $(warning WARNING: F_CPU is set to $(F_CPU), but $(PLATFORM) only supports up to 16.6MHz)
    endif
endif

ifeq ($(USE_EXTERNAL_CLOCK),0)
    ifeq ($(filter $(F_CPU),$(INT_OSC_ALLOWED)),)
        $(error Invalid F_CPU: $(F_CPU); When using internal clock, allowed values for $(PLATFORM) are: $(INT_OSC_ALLOWED))
    endif
endif

# CDEFS :=
CDEFS += -DF_CPU=$(F_CPU) -DUSE_EXTERNAL_CLOCK=$(USE_EXTERNAL_CLOCK) -D__SDCC__
# ------------------------------------------------------
# Target and Source

SRC += $(wildcard $(SRCDIR)/*.c $(LIBDIR)/*.c $(HALDIR)/*.c)
ASM_SRC = $(wildcard $(SRCDIR)/*.asm)

ifeq ($(CRYPTO_OPTIONS),)
    CRYPTO_OPTIONS = NONE
endif
ifeq ($(CRYPTO_TARGET),)
    CRYPTO_TARGET = NONE
endif
ifneq ($(CRYPTO_OPTIONS),NONE)
  ifeq ($(CRYPTO_OPTIONS), AES128C)
    ifeq ($(CRYPTO_TARGET),NONE)
        CRYPTO_TARGET = TINYAES128C
    endif
  endif
endif

ifeq ($(BAUD_RATE),)
    BAUD_RATE = 115200
endif
CDEFS += -DBAUD_RATE=$(BAUD_RATE)

# Not enough memory to run crypto targets with SS_VER_1_1 when XRAM_SIZE <= 768, only SS_VER_2_1
SMALL_XRAM_SIZES = 128 256 512 768
ifneq ($(MAKECMDGOALS),clean)
    ifneq ($(CRYPTO_TARGET),NONE)
        ifneq ($(CRYPTO_TARGET),TINYAES128C)
            $(error TINYAES128C is the only supported CRYPTO_TARGET for $(PLATFORM))
        endif
        ifneq ($(SS_VER), SS_VER_2_1)
            ifneq ($(filter $(XRAM_SIZE),$(SMALL_XRAM_SIZES)),)
                $(error CRYPTO_TARGET is not supported with SS_VER=$(SS_VER) for $(PLATFORM), use SS_VER_2_1)
            endif
        endif
        include $(FIRMWAREPATH)/crypto/Makefile.crypto
    endif
endif

C_SRC_FILE = $(notdir $(SRC))
C_OBJ_FILE = $(C_SRC_FILE:%.c=%.c.rel)
C_TO_ASM_FILE = $(C_SRC_FILE:%.c=%.asm)

ASM_SRC_FILE = $(notdir $(ASM_SRC))
ASM_OBJ_FILE = $(ASM_SRC_FILE:%.asm=%.asm.rel)

OBJ = $(addprefix $(OBJDIR)/, $(C_OBJ_FILE)) $(addprefix $(OBJDIR)/, $(ASM_OBJ_FILE))
CTOASM = $(addprefix $(OUTDIR)/, $(C_TO_ASM_FILE))

#$(info $(CTOASM))

# ------------------------------------------------------
# Usually SDCC's small memory model is the best choice.  If
# you run out of internal RAM, you will need to declare
# variables as "xdata", or switch to larger model

# Memory Model (small, medium, large, huge)
ifeq ($(MODEL),)
    MODEL  := medium
endif
$(info MODEL=$(MODEL))
MODEL_VAL =
ifeq ($(MODEL), small)
    MODEL_VAL = 0
else ifeq ($(MODEL), medium)
    MODEL_VAL = 1
else ifeq ($(MODEL), large)
    MODEL_VAL = 2
else ifeq ($(MODEL), huge)
    MODEL_VAL = 3
else
    $(error Invalid MODEL: $(MODEL))
endif
CDEFS += -DMODEL=$(MODEL_VAL)

# USE_FLOATS (this should be combined with model large if set 1)
# -DUSE_FLOATS=1
# $(info OBJ=$(OBJ))
# $(info SRC=$(SRC))
# $(info CDEFS=$(CDEFS))
# $(info EXTRAINCDIRS=$(EXTRAINCDIRS))
# $(info VPATH=$(VPATH))
# $(info TARGET=$(TARGET))

# ------------------------------------------------------
# SDCC

CC = sdcc
AS = sdas8051

MCU_MODEL = mcs51

#LIBS    =
#LIBPATH = -L $(LIBDIR)

#DEBUG = --debug
# NOTE: --stack-auto seems to be broken for N76E003, not recommended
AFLAGS =  -l -s
CFLAGS = --less-pedantic --disable-warning 85 -I$(INCDIR) -I$(LIBDIR) -m$(MCU_MODEL) --model-$(MODEL) --out-fmt-ihx --no-xinit-opt $(DEBUG) $(CDEFS) --peep-file $(TOOLSDIR)/peep.def
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
LFLAGS = $(LIBPATH) $(LIBS) -m$(MCU_MODEL) --model-$(MODEL) --code-size $(CODE_SIZE) --iram-size $(IRAM_SIZE) --xram-size $(XRAM_SIZE) --out-fmt-ihx $(DEBUG) $(CDEFS)

TARGET-PLAT = $(TARGET)-$(PLATFORM)
# ------------------------------------------------------
# Recepies, see GNU MAKE manual

.PHONY: all

all: make-dirs $(TARGET-PLAT).bin $(TARGET-PLAT).hex

make-dirs:
	mkdir -p $(OBJDIR)

%.hex: $(OBJDIR)/%.ihx
	packihx $^ > $@

%.bin: $(OBJDIR)/%.ihx
	makebin -p $^ $@

$(OBJDIR)/%.ihx: $(OBJ)
	$(CC) -o $@ $(LFLAGS) $^

$(OBJDIR)/%.c.rel: %.c
	$(CC) -o $@ $(CFLAGS) -c $^

$(OBJDIR)/%.asm.rel: %.asm
	$(AS) $(AFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -rf $(OBJDIR)/*
	rm -rf $(TARGET-PLAT).hex
	rm -rf $(TARGET-PLAT).bin
	rm -rf $(TARGET-PLAT).asm


asm: $(CTOASM)

%.asm: %.c
	$(CC) -o $@ -S $(CFLAGS) -c $^

