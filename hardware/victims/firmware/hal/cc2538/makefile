#TI_CCPATH=/home/cwuser/Downloads/ti-cgt-arm_16.9.6.LTS
#TI_CC=$(TI_CCPATH)/bin/armcl
#TI_CCINCLUDE+=$(TI_CCPATH)/include/
#TI_CCLIB=$(TI_CCPATH)/lib/libc.a
#SRC+=$(wildcard *.c)
#SSRC+=$(wildcard startup/*.c)

#TI_CCFLAGS+= -g -O4 --code_state=16 -mv7M3 -me -ual --abi=eabi --gen_func_subsections=on --define=CC2538_USE_ALTERNATE_INTERRUPT_MAP
#TI_LIBFLAGS+= --xml_link_info="link.xml" --rom_model --heap_size=0 --stack_size=256 --map_file bar.map

#assembly:
#	$(TI_CC) $(TI_CCFLAGS) $(SSRC) -I $(TI_CCINCLUDE) -n

#TI:
#	$(TI_CC) $(SRC) $(SSRC) $(TI_CCFLAGS) -I $(TI_CCINCLUDE) -z $(TI_LIBFLAGS) --library=$(TI_CCLIB) --output_file=myoutput.out cx2538xf53_ccs.cmd
#TI Stuff not needed anymore
#####################GCC#####################################

CC=arm-none-eabi-gcc
CFLAGS=--specs=nosys.specs -mcpu=cortex-m3 -mthumb
SRC=$(wildcard *.c)
LFLAGS=-Map,foo.map
ASRC=start.S

gcc:
	$(CC) $(SRC) $(ASRC)  $(CFLAGS) -o test.out -T LinkerFile.ld -Wl,$(LFLAGS)
