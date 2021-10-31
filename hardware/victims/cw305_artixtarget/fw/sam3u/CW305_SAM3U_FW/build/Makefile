################################################################################
# Automatically-generated file. Do not edit!
################################################################################

#
# This makefile has been HORRIBLY HACKED to generate a build by default now on Linux.
# It 
#

.DEFAULT_GOAL := all

#Uncomment for Linux
SHELL := bash     
CC := "arm-none-eabi-gcc"

#Uncomment for Windows
#SHELL := cmd.exe  
#CC := "C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-gcc.exe"

RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS :=  \
../src/ \
../src/naeusb/ \
../src/ASF/ \
../src/ASF/common/ \
../src/ASF/common/boards/ \
../src/ASF/common/services/ \
../src/ASF/common/services/clock/ \
../src/ASF/common/services/clock/sam3u/ \
../src/ASF/common/services/delay/ \
../src/ASF/common/services/delay/sam/ \
../src/ASF/common/services/gpio/ \
../src/ASF/common/services/gpio/sam_gpio/ \
../src/ASF/common/services/ioport/ \
../src/ASF/common/services/ioport/sam/ \
../src/ASF/common/services/serial/ \
../src/ASF/common/services/serial/sam_uart/ \
../src/ASF/common/services/sleepmgr/ \
../src/ASF/common/services/sleepmgr/sam/ \
../src/ASF/common/services/twi/ \
../src/ASF/common/services/twi/sam_twi/ \
../src/ASF/common/services/usb/ \
../src/ASF/common/services/usb/class/ \
../src/ASF/common/services/usb/class/vendor/ \
../src/ASF/common/services/usb/class/vendor/device/ \
../src/ASF/common/services/usb/class/vendor/device/example/ \
../src/ASF/common/services/usb/class/vendor/device/example/sam3u4e_sam3u_ek/ \
../src/ASF/common/services/usb/udc/ \
../src/ASF/common/utils/ \
../src/ASF/common/utils/interrupt/ \
../src/ASF/common/utils/stdio/ \
../src/ASF/sam/ \
../src/ASF/sam/boards/ \
../src/ASF/sam/boards/cw305 \
../src/ASF/sam/drivers/ \
../src/ASF/sam/drivers/chipid/ \
../src/ASF/sam/drivers/ebi/ \
../src/ASF/sam/drivers/ebi/smc/ \
../src/ASF/sam/drivers/efc/ \
../src/ASF/sam/drivers/spi/ \
../src/ASF/sam/drivers/tc/ \
../src/ASF/sam/drivers/twi/ \
../src/ASF/sam/drivers/uart \
../src/ASF/sam/drivers/pio/ \
../src/ASF/sam/drivers/pmc/ \
../src/ASF/sam/drivers/udphs/ \
../src/ASF/sam/drivers/usart/ \
../src/ASF/sam/services/ \
../src/ASF/sam/services/flash_efc/ \
../src/ASF/sam/utils/ \
../src/ASF/sam/utils/cmsis/ \
../src/ASF/sam/utils/cmsis/sam3u/ \
../src/ASF/sam/utils/cmsis/sam3u/include/ \
../src/ASF/sam/utils/cmsis/sam3u/include/component/ \
../src/ASF/sam/utils/cmsis/sam3u/include/instance/ \
../src/ASF/sam/utils/cmsis/sam3u/include/pio/ \
../src/ASF/sam/utils/cmsis/sam3u/source/ \
../src/ASF/sam/utils/cmsis/sam3u/source/templates/ \
../src/ASF/sam/utils/cmsis/sam3u/source/templates/gcc/ \
../src/ASF/sam/utils/header_files/ \
../src/ASF/sam/utils/linker_scripts/ \
../src/ASF/sam/utils/linker_scripts/sam3u/ \
../src/ASF/sam/utils/linker_scripts/sam3u/sam3u4/ \
../src/ASF/sam/utils/linker_scripts/sam3u/sam3u4/gcc/ \
../src/ASF/sam/utils/make/ \
../src/ASF/sam/utils/preprocessor/ \
../src/ASF/sam/utils/syscalls/ \
../src/ASF/sam/utils/syscalls/gcc/ \
../src/ASF/thirdparty/ \
../src/ASF/thirdparty/CMSIS/ \
../src/ASF/thirdparty/CMSIS/Include/ \
../src/ASF/thirdparty/CMSIS/Lib/ \
../src/ASF/thirdparty/CMSIS/Lib/GCC/ \
../src/config/ \
../src/pdi

C_SRCS +=  \
../src/ASF/sam/boards/cw305/init.c \
../src/ASF/sam/boards/cw305/led.c \
../src/ASF/sam/drivers/twi/twi.c \
../src/ASF/sam/drivers/usart/usart.c \
../src/cdce906.c \
../src/naeusb/circbuffer.c \
../src/naeusb/fpga_program.c \
../src/fpgaspi_program.c \
../src/fpgautil_io.c \
../src/naeusb/XMEGANVM.c \
../src/naeusb/XPROGNewAE.c \
../src/naeusb/XPROGTarget.c \
../src/naeusb/XPROGTimeout.c \
../src/tasks.c \
../src/tps56520.c \
../src/naeusb/usart_driver.c \
../src/naeusb/usb_xmem.c \
../src/ASF/sam/drivers/chipid/chipid.c \
../src/ASF/sam/drivers/efc/efc.c \
../src/ASF/sam/drivers/tc/tc.c \
../src/ASF/sam/services/flash_efc/flash_efc.c \
../src/ASF/common/services/delay/sam/cycle_counter.c \
../src/ASF/common/services/usb/class/vendor/device/example/sam3u4e_sam3u_ek/ui_cw305.c \
../src/ASF/common/services/clock/sam3u/sysclk.c \
../src/ASF/common/services/sleepmgr/sam/sleepmgr.c \
../src/ASF/common/services/usb/class/vendor/device/udi_vendor.c \
../src/ASF/common/services/usb/class/vendor/device/udi_vendor_desc.c \
../src/ASF/common/services/usb/udc/udc.c \
../src/ASF/common/utils/interrupt/interrupt_sam_nvic.c \
../src/ASF/common/utils/stdio/read.c \
../src/ASF/common/utils/stdio/write.c \
../src/ASF/sam/drivers/ebi/smc/smc.c \
../src/ASF/sam/drivers/pio/pio.c \
../src/ASF/sam/drivers/pio/pio_handler.c \
../src/ASF/sam/drivers/pmc/pmc.c \
../src/ASF/sam/drivers/pmc/sleep.c \
../src/ASF/sam/drivers/spi/spi.c \
../src/ASF/sam/drivers/udphs/udphs_device.c \
../src/ASF/sam/drivers/uart/uart.c \
../src/ASF/sam/utils/cmsis/sam3u/source/templates/exceptions.c \
../src/ASF/sam/utils/cmsis/sam3u/source/templates/gcc/startup_sam3u.c \
../src/ASF/sam/utils/cmsis/sam3u/source/templates/system_sam3u.c \
../src/ASF/sam/utils/syscalls/gcc/syscalls.c \
../src/main_cw305.c

PREPROCESSING_SRCS += 


ASM_SRCS += 

OBJS +=  \
../src/ASF/sam/boards/cw305/init.o \
../src/ASF/sam/boards/cw305/led.o \
../src/ASF/sam/drivers/twi/twi.o \
../src/ASF/sam/drivers/usart/usart.o \
../src/cdce906.o \
../src/naeusb/circbuffer.o \
../src/naeusb/fpga_program.o \
../src/fpgaspi_program.o \
../src/fpgautil_io.o \
../src/naeusb/XMEGANVM.o \
../src/naeusb/XPROGNewAE.o \
../src/naeusb/XPROGTarget.o \
../src/naeusb/XPROGTimeout.o \
../src/tasks.o \
../src/tps56520.o \
../src/naeusb/usart_driver.o \
../src/naeusb/usb_xmem.o \
../src/ASF/sam/drivers/chipid/chipid.o \
../src/ASF/sam/drivers/efc/efc.o \
../src/ASF/sam/drivers/tc/tc.o \
../src/ASF/sam/services/flash_efc/flash_efc.o \
../src/ASF/common/services/delay/sam/cycle_counter.o \
../src/ASF/common/services/usb/class/vendor/device/example/sam3u4e_sam3u_ek/ui_cw305.o \
../src/ASF/common/services/clock/sam3u/sysclk.o \
../src/ASF/common/services/sleepmgr/sam/sleepmgr.o \
../src/ASF/common/services/usb/class/vendor/device/udi_vendor.o \
../src/ASF/common/services/usb/class/vendor/device/udi_vendor_desc.o \
../src/ASF/common/services/usb/udc/udc.o \
../src/ASF/common/utils/interrupt/interrupt_sam_nvic.o \
../src/ASF/common/utils/stdio/read.o \
../src/ASF/common/utils/stdio/write.o \
../src/ASF/sam/drivers/ebi/smc/smc.o \
../src/ASF/sam/drivers/pio/pio.o \
../src/ASF/sam/drivers/pio/pio_handler.o \
../src/ASF/sam/drivers/pmc/pmc.o \
../src/ASF/sam/drivers/pmc/sleep.o \
../src/ASF/sam/drivers/spi/spi.o \
../src/ASF/sam/drivers/udphs/udphs_device.o \
../src/ASF/sam/drivers/uart/uart.o \
../src/ASF/sam/utils/cmsis/sam3u/source/templates/exceptions.o \
../src/ASF/sam/utils/cmsis/sam3u/source/templates/gcc/startup_sam3u.o \
../src/ASF/sam/utils/cmsis/sam3u/source/templates/system_sam3u.o \
../src/ASF/sam/utils/syscalls/gcc/syscalls.o \
../src/main_cw305.o

OBJS_AS_ARGS +=  \
../src/ASF/sam/boards/cw305/init.o \
../src/ASF/sam/boards/cw305/led.o \
../src/ASF/sam/drivers/twi/twi.o \
../src/ASF/sam/drivers/usart/usart.o \
../src/cdce906.o \
../src/naeusb/circbuffer.o \
../src/naeusb/fpga_program.o \
../src/fpgaspi_program.o \
../src/fpgautil_io.o \
../src/naeusb/XMEGANVM.o \
../src/naeusb/XPROGNewAE.o \
../src/naeusb/XPROGTarget.o \
../src/naeusb/XPROGTimeout.o \
../src/tasks.o \
../src/tps56520.o \
../src/naeusb/usart_driver.o \
../src/naeusb/usb_xmem.o \
../src/ASF/sam/drivers/chipid/chipid.o \
../src/ASF/sam/drivers/efc/efc.o \
../src/ASF/sam/drivers/tc/tc.o \
../src/ASF/sam/services/flash_efc/flash_efc.o \
../src/ASF/common/services/delay/sam/cycle_counter.o \
../src/ASF/common/services/usb/class/vendor/device/example/sam3u4e_sam3u_ek/ui_cw305.o \
../src/ASF/common/services/clock/sam3u/sysclk.o \
../src/ASF/common/services/sleepmgr/sam/sleepmgr.o \
../src/ASF/common/services/usb/class/vendor/device/udi_vendor.o \
../src/ASF/common/services/usb/class/vendor/device/udi_vendor_desc.o \
../src/ASF/common/services/usb/udc/udc.o \
../src/ASF/common/utils/interrupt/interrupt_sam_nvic.o \
../src/ASF/common/utils/stdio/read.o \
../src/ASF/common/utils/stdio/write.o \
../src/ASF/sam/drivers/ebi/smc/smc.o \
../src/ASF/sam/drivers/pio/pio.o \
../src/ASF/sam/drivers/pio/pio_handler.o \
../src/ASF/sam/drivers/pmc/pmc.o \
../src/ASF/sam/drivers/pmc/sleep.o \
../src/ASF/sam/drivers/spi/spi.o \
../src/ASF/sam/drivers/udphs/udphs_device.o \
../src/ASF/sam/drivers/uart/uart.o \
../src/ASF/sam/utils/cmsis/sam3u/source/templates/exceptions.o \
../src/ASF/sam/utils/cmsis/sam3u/source/templates/gcc/startup_sam3u.o \
../src/ASF/sam/utils/cmsis/sam3u/source/templates/system_sam3u.o \
../src/ASF/sam/utils/syscalls/gcc/syscalls.o \
../src/main_cw305.o

C_DEPS +=  \
../src/ASF/sam/boards/cw305/init.d \
../src/ASF/sam/boards/cw305/led.d \
../src/ASF/sam/drivers/twi/twi.d \
../src/ASF/sam/drivers/usart/usart.d \
../src/cdce906.d \
../src/naeusb/circbuffer.d \
../src/naeusb/fpga_program.d \
../src/fpgaspi_program.d \
../src/fpgautil_io.d \
../src/naeusb/XMEGANVM.d \
../src/naeusb/XPROGNewAE.d \
../src/naeusb/XPROGTarget.d \
../src/naeusb/XPROGTimeout.d \
../src/tasks.d \
../src/tps56520.d \
../src/naeusb/usart_driver.d \
../src/naeusb/usb_xmem.d \
../src/ASF/sam/drivers/chipid/chipid.d \
../src/ASF/sam/drivers/efc/efc.d \
../src/ASF/sam/drivers/tc/tc.d \
../src/ASF/sam/services/flash_efc/flash_efc.d \
../src/ASF/common/services/delay/sam/cycle_counter.d \
../src/ASF/common/services/usb/class/vendor/device/example/sam3u4e_sam3u_ek/ui_cw305.d \
../src/ASF/common/services/clock/sam3u/sysclk.d \
../src/ASF/common/services/sleepmgr/sam/sleepmgr.d \
../src/ASF/common/services/usb/class/vendor/device/udi_vendor.d \
../src/ASF/common/services/usb/class/vendor/device/udi_vendor_desc.d \
../src/ASF/common/services/usb/udc/udc.d \
../src/ASF/common/utils/interrupt/interrupt_sam_nvic.d \
../src/ASF/common/utils/stdio/read.d \
../src/ASF/common/utils/stdio/write.d \
../src/ASF/sam/drivers/ebi/smc/smc.d \
../src/ASF/sam/drivers/pio/pio.d \
../src/ASF/sam/drivers/pio/pio_handler.d \
../src/ASF/sam/drivers/pmc/pmc.d \
../src/ASF/sam/drivers/pmc/sleep.d \
../src/ASF/sam/drivers/spi/spi.d \
../src/ASF/sam/drivers/udphs/udphs_device.d \
../src/ASF/sam/drivers/uart/uart.d \
../src/ASF/sam/utils/cmsis/sam3u/source/templates/exceptions.d \
../src/ASF/sam/utils/cmsis/sam3u/source/templates/gcc/startup_sam3u.d \
../src/ASF/sam/utils/cmsis/sam3u/source/templates/system_sam3u.d \
../src/ASF/sam/utils/syscalls/gcc/syscalls.d \
../src/main_cw305.d

C_DEPS_AS_ARGS +=  \
../src/ASF/sam/boards/cw305/init.d \
../src/ASF/sam/boards/cw305/led.d \
../src/ASF/sam/drivers/twi/twi.d \
../src/ASF/sam/drivers/usart/usart.d \
../src/cdce906.d \
../src/naeusb/circbuffer.d \
../src/naeusb/fpga_program.d \
../src/fpgaspi_program.d \
../src/fpgautil_io.d \
../src/naeusb/XMEGANVM.d \
../src/naeusb/XPROGNewAE.d \
../src/naeusb/XPROGTarget.d \
../src/naeusb/XPROGTimeout.d \
../src/tasks.d \
../src/tps56520.d \
../src/naeusb/usart_driver.d \
../src/naeusb/usb_xmem.d \
../src/ASF/sam/drivers/chipid/chipid.d \
../src/ASF/sam/drivers/efc/efc.d \
../src/ASF/sam/drivers/tc/tc.d \
../src/ASF/sam/services/flash_efc/flash_efc.d \
../src/ASF/common/services/delay/sam/cycle_counter.d \
../src/ASF/common/services/usb/class/vendor/device/example/sam3u4e_sam3u_ek/ui_cw305.d \
../src/ASF/common/services/clock/sam3u/sysclk.d \
../src/ASF/common/services/sleepmgr/sam/sleepmgr.d \
../src/ASF/common/services/usb/class/vendor/device/udi_vendor.d \
../src/ASF/common/services/usb/class/vendor/device/udi_vendor_desc.d \
../src/ASF/common/services/usb/udc/udc.d \
../src/ASF/common/utils/interrupt/interrupt_sam_nvic.d \
../src/ASF/common/utils/stdio/read.d \
../src/ASF/common/utils/stdio/write.d \
../src/ASF/sam/drivers/ebi/smc/smc.d \
../src/ASF/sam/drivers/pio/pio.d \
../src/ASF/sam/drivers/pio/pio_handler.d \
../src/ASF/sam/drivers/pmc/pmc.d \
../src/ASF/sam/drivers/pmc/sleep.d \
../src/ASF/sam/drivers/spi/spi.d \
../src/ASF/sam/drivers/udphs/udphs_device.d \
../src/ASF/sam/drivers/uart/uart.d \
../src/ASF/sam/utils/cmsis/sam3u/source/templates/exceptions.d \
../src/ASF/sam/utils/cmsis/sam3u/source/templates/gcc/startup_sam3u.d \
../src/ASF/sam/utils/cmsis/sam3u/source/templates/system_sam3u.d \
../src/ASF/sam/utils/syscalls/gcc/syscalls.d \
../src/main_cw305.d
OUTPUT_FILE_PATH +=CW305_SAM3UFW.elf

OUTPUT_FILE_PATH_AS_ARGS +=CW305_SAM3UFW.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+=  \
../src/ASF/sam/utils/linker_scripts/sam3u/sam3u4/gcc/flash.ld

CFLAGS_ALL =  -x c -mthumb -D__SAM3U2E__ -DDEBUG -D__SAM3U2E__ -DARM_MATH_CM3=true -Dprintf=iprintf -DUDD_ENABLE -DBOARD=CW305 \
-Dscanf=iscanf -DPLATFORMCW305=1  -I"../common/services/usb/class/vendor/device/example" \
-I"../common/services/usb/class/vendor/device/example/sam3u4e_sam3u_ek" -I"../src/ASF/common/services/usb/udc" \
-I"../src/ASF/common/utils" -I"../src/ASF/common/services/sleepmgr" -I"../src/ASF/sam/utils/cmsis/sam3u/source/templates" \
-I"../src/ASF/sam/drivers/pmc" -I"../src/ASF/common/services/gpio" -I"../src/ASF/sam/boards" -I"../src/ASF/sam/drivers/pio" \
-I"../src/ASF/common/boards" -I"../src/ASF/sam/utils/header_files" -I"../src/ASF/common/services/ioport" \
-I"../src/ASF/common/services/usb/class/vendor" -I"../src/ASF/common/services/usb/class/vendor/device" \
-I"../src/ASF/thirdparty/CMSIS/Include" -I"../src/ASF/common/services/usb" -I"../src/ASF/sam/drivers/udphs" \
-I"../src/ASF/sam/utils/cmsis/sam3u/include" -I"../src/ASF/thirdparty/CMSIS/Lib/GCC" -I"../src/config" -I"../src" \
-I"../src/ASF/common/services/clock" -I"../src/ASF/sam/utils" -I"../src/ASF/sam/utils/preprocessor" -I"../src/ASF/sam/boards/sam3u_ek" \
-I"../src/ASF/common/utils/stdio/stdio_serial" -I"../src/ASF/common/services/serial" -I"../src/ASF/sam/drivers/uart" -I"../src/naeusb/" \
-I"../src/ASF/sam/drivers/usart" -I"../src/ASF/sam/drivers/ebi/smc" -I"../src/ASF/sam/drivers/spi" -I"../src/ASF/common/services/delay" \
-I"../src/ASF/sam/drivers/tc" -I"../src/ASF/sam/drivers/chipid" -I"../src/ASF/sam/drivers/efc" -I"../src/ASF/sam/services/flash_efc" \
-I"../src/ASF/sam/services/smart_card" -I"../src/ASF/common/services/spi/sam_spi" -I"../src/ASF/common/services/spi" \
-I"../src/ASF/common/services/wtk" -I"../src/ASF/common/utils/membag" -I"../src/ASF/common/services/serial/sam_uart" \
-I"../src/ASF/common/services/twi" -I"../src/ASF/sam/drivers/twi"  -O1 -fdata-sections -ffunction-sections -mlong-calls -g3 -Wall \
-mcpu=cortex-m3 -c -pipe -fno-strict-aliasing -Wall -Wstrict-prototypes -Wmissing-prototypes -Werror-implicit-function-declaration \
-Wpointer-arith -std=gnu99 -ffunction-sections -fdata-sections -Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int -Wmain \
-Wparentheses -Wsequence-point -Wreturn-type -Wswitch -Wtrigraphs -Wunused -Wuninitialized -Wunknown-pragmas -Wfloat-equal -Wundef \
-Wshadow -Wbad-function-cast -Wwrite-strings -Wsign-compare -Waggregate-return  -Wmissing-declarations -Wformat \
-Wmissing-format-attribute -Wno-deprecated-declarations -Wpacked -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code \
-Wcast-align --param max-inline-insns-single=500 -MD -MP -MF



# AVR32/GNU C Compiler
src/ASF/common/services/serial/%.o: ../src/ASF/common/services/serial/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/sam/boards/cw305/%.o: ../src/ASF/sam/boards/cw305/%.c
	@echo $(CFLAGS_ALL)
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/sam/drivers/twi/%.o: ../src/ASF/sam/drivers/twi/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/sam/drivers/usart/%.o: ../src/ASF/sam/drivers/usart/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/%.o: ../src/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	
src/naeusb/%.o: ../src/naeusb/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<

src/pdi/%.o: ../src/pdi/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/sam/drivers/chipid/%.o: ../src/ASF/sam/drivers/chipid/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/sam/drivers/efc/%.o: ../src/ASF/sam/drivers/efc/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/sam/drivers/tc/%.o: ../src/ASF/sam/drivers/tc/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/sam/services/flash_efc/%.o: ../src/ASF/sam/services/flash_efc/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/common/services/delay/sam/%.o: ../src/ASF/common/services/delay/sam/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/common/services/usb/class/vendor/device/example/sam3u4e_sam3u_ek/%.o: ../src/ASF/common/services/usb/class/vendor/device/example/sam3u4e_sam3u_ek/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/common/services/clock/sam3u/%.o: ../src/ASF/common/services/clock/sam3u/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/common/services/sleepmgr/sam/%.o: ../src/ASF/common/services/sleepmgr/sam/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/common/services/usb/class/vendor/device/%.o: ../src/ASF/common/services/usb/class/vendor/device/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/common/services/usb/udc/%.o: ../src/ASF/common/services/usb/udc/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/common/utils/interrupt/%.o: ../src/ASF/common/utils/interrupt/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/common/utils/stdio/%.o: ../src/ASF/common/utils/stdio/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/sam/drivers/ebi/smc/%.o: ../src/ASF/sam/drivers/ebi/smc/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/sam/drivers/pio/%.o: ../src/ASF/sam/drivers/pio/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/sam/drivers/pmc/%.o: ../src/ASF/sam/drivers/pmc/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/sam/drivers/spi/%.o: ../src/ASF/sam/drivers/spi/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/sam/drivers/udphs/%.o: ../src/ASF/sam/drivers/udphs/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/sam/drivers/uart/%.o: ../src/ASF/sam/drivers/uart/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/sam/utils/cmsis/sam3u/source/templates/%.o: ../src/ASF/sam/utils/cmsis/sam3u/source/templates/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/sam/utils/cmsis/sam3u/source/templates/gcc/%.o: ../src/ASF/sam/utils/cmsis/sam3u/source/templates/gcc/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/ASF/sam/utils/syscalls/gcc/%.o: ../src/ASF/sam/utils/syscalls/gcc/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.9.3
	$(CC) $(CFLAGS_ALL) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: ARM/GNU Linker : 4.9.3
	$(CC) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -mthumb -Wl,-Map="CW305_SAM3UFW.map" -Wl,--start-group -larm_cortexM3l_math -lm  -Wl,--end-group -L"../cmsis/linkerScripts" -L"../src/ASF/thirdparty/CMSIS/Lib/GCC"  -Wl,--gc-sections -mcpu=cortex-m3 -Wl,--entry=Reset_Handler -Wl,--cref -mthumb -T../src/ASF/sam/utils/linker_scripts/sam3u/sam3u2/gcc/flash.ld  
	@echo Finished building target: $@
	"arm-none-eabi-objcopy" -O binary "CW305_SAM3UFW.elf" "CW305_SAM3UFW.bin"
	"arm-none-eabi-objcopy" -O ihex -R .eeprom -R .fuse -R .lock -R .signature  "CW305_SAM3UFW.elf" "CW305_SAM3UFW.hex"
	"arm-none-eabi-objcopy" -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O binary "CW305_SAM3UFW.elf" "CW305_SAM3UFW.eep" || exit 0
	"arm-none-eabi-objdump" -h -S "CW305_SAM3UFW.elf" > "CW305_SAM3UFW.lss"
	"arm-none-eabi-objcopy" -O srec -R .eeprom -R .fuse -R .lock -R .signature  "CW305_SAM3UFW.elf" "CW305_SAM3UFW.srec"
	"arm-none-eabi-size" "CW305_SAM3UFW.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "CW305_SAM3UFW.elf" "CW305_SAM3UFW.a" "CW305_SAM3UFW.hex" "CW305_SAM3UFW.bin" "CW305_SAM3UFW.lss" "CW305_SAM3UFW.eep" "CW305_SAM3UFW.map" "CW305_SAM3UFW.srec"
	
