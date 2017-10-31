################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
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
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../../../../../../hal/sam4l/hwaes.c \
../../../../../../hal/sam4l/sam4l_hal.c \
../../../../../../hal/sam4l/startup_sam4l.c \
../../../../../../hal/sam4l/uart.c \
../../../../../../simpleserial/simpleserial.c \
../../../../../simpleserial-aes.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
hwaes.o \
sam4l_hal.o \
startup_sam4l.o \
uart.o \
simpleserial.o \
simpleserial-aes.o

OBJS_AS_ARGS +=  \
hwaes.o \
sam4l_hal.o \
startup_sam4l.o \
uart.o \
simpleserial.o \
simpleserial-aes.o

C_DEPS +=  \
hwaes.d \
sam4l_hal.d \
startup_sam4l.d \
uart.d \
simpleserial.d \
simpleserial-aes.d

C_DEPS_AS_ARGS +=  \
hwaes.d \
sam4l_hal.d \
startup_sam4l.d \
uart.d \
simpleserial.d \
simpleserial-aes.d

OUTPUT_FILE_PATH +=sam4l-simpleserial-aes.elf

OUTPUT_FILE_PATH_AS_ARGS +=sam4l-simpleserial-aes.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+=  \
../../../../../../hal/sam4l/sam4lc2a_flash.ld \
../../../../../../hal/sam4l/sam4lc2a_sram.ld


# AVR32/GNU C Compiler













./%.o: ../../../../../../hal/sam4l/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 6.2.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-gcc.exe$(QUOTE)  -x c -mthumb -D__SAM4LC2A__ -DDEBUG -DHAL_TYPE=HAL_sam4l -DPLATFORM=CW308_SAM4L  -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\SAM4L_DFP\1.0.27\sam4l4\include" -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\arm\cmsis\4.2.0\CMSIS\Include" -I"../../../../.." -I"../../../../../../hal" -I"../../../../../../hal/sam4l" -I"../../../../../../crypto" -I"../../../../../../simpleserial"  -O1 -ffunction-sections -mlong-calls -g3 -Wall -mcpu=cortex-m4 -c -std=gnu99 -D__ATSAM4LC2A__ -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

./%.o: ../../../../../../simpleserial/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 6.2.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-gcc.exe$(QUOTE)  -x c -mthumb -D__SAM4LC2A__ -DDEBUG -DHAL_TYPE=HAL_sam4l -DPLATFORM=CW308_SAM4L  -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\SAM4L_DFP\1.0.27\sam4l4\include" -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\arm\cmsis\4.2.0\CMSIS\Include" -I"../../../../.." -I"../../../../../../hal" -I"../../../../../../hal/sam4l" -I"../../../../../../crypto" -I"../../../../../../simpleserial"  -O1 -ffunction-sections -mlong-calls -g3 -Wall -mcpu=cortex-m4 -c -std=gnu99 -D__ATSAM4LC2A__ -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

./%.o: ../../../../../%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 6.2.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-gcc.exe$(QUOTE)  -x c -mthumb -D__SAM4LC2A__ -DDEBUG -DHAL_TYPE=HAL_sam4l -DPLATFORM=CW308_SAM4L  -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\SAM4L_DFP\1.0.27\sam4l4\include" -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\arm\cmsis\4.2.0\CMSIS\Include" -I"../../../../.." -I"../../../../../../hal" -I"../../../../../../hal/sam4l" -I"../../../../../../crypto" -I"../../../../../../simpleserial"  -O1 -ffunction-sections -mlong-calls -g3 -Wall -mcpu=cortex-m4 -c -std=gnu99 -D__ATSAM4LC2A__ -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
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
	@echo Invoking: ARM/GNU Linker : 6.2.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -mthumb -Wl,-Map="sam4l-simpleserial-aes.map" -Wl,--start-group -lm  -Wl,--end-group -L"../../../../../../hal/sam4l"  -Wl,--gc-sections -mcpu=cortex-m4 -Tsam4lc2a_flash.ld  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-objcopy.exe" -O binary "sam4l-simpleserial-aes.elf" "sam4l-simpleserial-aes.bin"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature  "sam4l-simpleserial-aes.elf" "sam4l-simpleserial-aes.hex"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-objcopy.exe" -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O binary "sam4l-simpleserial-aes.elf" "sam4l-simpleserial-aes.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-objdump.exe" -h -S "sam4l-simpleserial-aes.elf" > "sam4l-simpleserial-aes.lss"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature  "sam4l-simpleserial-aes.elf" "sam4l-simpleserial-aes.srec"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-size.exe" "sam4l-simpleserial-aes.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "sam4l-simpleserial-aes.elf" "sam4l-simpleserial-aes.a" "sam4l-simpleserial-aes.hex" "sam4l-simpleserial-aes.bin" "sam4l-simpleserial-aes.lss" "sam4l-simpleserial-aes.eep" "sam4l-simpleserial-aes.map" "sam4l-simpleserial-aes.srec"
	