##
## This is the OLD file included by the build process. It will be removed at some point in time.
##
$(warning ########################  WARNING  ################################## )
$(warning ## You are using old build system with PLATFORM_INCLUDE.mk - you   ## )
$(warning ## should update your app to use the new build system, see         ## )
$(warning ## http://wiki.newae.com/Target_Firmware_Build_System              ## )
$(warning ##################################################################### )
PLTNAME = Unknown Platform

ifeq ($(PLATFORM),CW301_AVR)
 MCU = atmega328
 HAL = avr
 PLTNAME = Multi-Target Board, AVR Target
else ifeq ($(PLATFORM),CW301_XMEGA)
 MCU = atxmega16a4
 HAL = xmega
 PLTNAME = Multi-Target Board, XMEGA Target
else ifeq ($(PLATFORM),CW303)
#d4 not officially supported, by has same reg map
 MCU = atxmega128d3
 HAL = xmega
 PLTNAME = CW-Lite XMEGA
else ifeq ($(PLATFORM),CW304)
 MCU = atmega328
 HAL = avr
 PLTNAME = CW-Lite NOTDUINO
else ifeq ($(PLATFORM),CW308_MEGARF)
 MCU = atmega128rfa1
# MCU = atmega2564rfr2
 HAL = avr
 PLTNAME = CW308T: ATMegaRF Target
else ifeq ($(PLATFORM),CW308_SAM4L)
 HAL = sam4l
 PLTNAME = CW308T: SAM4L Target
else
  $(error Invalid or empty PLATFORM: $(PLATFORM))
endif

ifeq ($(HAL),avr)
 VPATH = $(HALPATH)/avr
 HALSRC = uart.c
 EXTRAINCDIRS += $(HALPATH)/avr
else ifeq ($(HAL),xmega)
 VPATH=$(HALPATH)/xmega
 HALSRC = XMEGA_AES_driver.c uart.c usart_driver.c xmega_hal.c
 EXTRAINCDIRS += $(HALPATH)/xmega
else
 $(error: Unknown HAL: $(HAL))
endif


CDEFS += -DHAL_TYPE=HAL_$(HAL) -DPLATFORM=$(PLATFORM)