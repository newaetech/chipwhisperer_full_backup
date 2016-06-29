
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
 HAL = avr
 PLTNAME = 802.15.4 Test Board, MegaRF Target
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