
PLTNAME = Unknown Platform

ifeq ($(PLATFORM),CW301_AVR)
 MCU = atmega328
 HAL = avr
 PLTNAME = Multi-Target Board, AVR Target
else ifeq ($(PLATFORM),CW301_XMEGA)
#a4 not officially supported, but has same reg map
 MCU = atxmega128a3
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
else
  $(error Invalid or empty PLATFORM: $(PLATFORM))
endif

ifeq ($(HAL),avr)
 VPATH = $(HALPATH)/avr
 HALSRC = uart.c
 EXTRAINCDIRS += $(HALPATH)/avr
else ifeq ($(HAL),xmega)
 VPATH=$(HALPATH)/xmega
 HALSRC = AES_driver.c uart.c usart_driver.c xmega_hal.c
 EXTRAINCDIRS += $(HALPATH)/xmega
else
 $(error: Unknown HAL: $(HAL))
endif


CDEFS += -DHAL_TYPE=HAL_$(HAL)