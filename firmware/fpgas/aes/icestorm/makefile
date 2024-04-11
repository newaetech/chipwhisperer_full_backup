TOP=ss_ice40_aes_top
#TOP=ss2_ice40_aes_top
FREQ=20

all: time synth pnr pack

time:
	python ./generate_timestamp.py

ifeq ($(TOP),ss_ice40_aes_top)
    FILES=../hdl/$(TOP).v \
	  ../hdl/SimpleSerial.v \
	  ../hdl/UART.v \
	  ../../cryptosrc/aes_googlevault/aes_ks.v \
	  ../../cryptosrc/aes_googlevault/aes_sbox.v \
	  ../../cryptosrc/aes_googlevault/aes_core.v  \
	  ../../cryptosrc/aes_newae/aes_sbox_lut.v

else ifeq ($(TOP),ss2_ice40_aes_top)
    FILES=../hdl/$(TOP).v \
	   ../../cryptosrc/aes_googlevault/aes_ks.v \
	   ../../cryptosrc/aes_googlevault/aes_sbox.v \
	   ../../cryptosrc/aes_googlevault/aes_core.v  \
	   ../../cryptosrc/aes_newae/aes_sbox_lut.v \
	   ../hdl/cw305_reg_aes.v \
	   ../hdl/cw305_top.v \
	   ../hdl/cw305_usb_reg_fe.v \
	   ../../fpga-common/hdl/fifo_sync.v \
	   ../../fpga-common/hdl/uart_core.v \
	   ../../fpga-common/hdl/crc_ss2.v \
	   ../../fpga-common/hdl/ss2.v \
	   ../../fpga-common/hdl/cdc_pulse.v

else 
    ${error Unknown or blank TOP: $(TOP)}
    
endif

synth:
	yosys -p 'synth_ice40 -top $(TOP) -json $(TOP).json' \
	-D ICE40 \
	-D SBOX_GF \
	-D SS2_WRAPPER \
	-D GOOGLE_VAULT_AES \
	$(FILES) \
	-l yosys.log

pnr:
	nextpnr-ice40 --up5k --package uwg30 \
	--asc $(TOP).asc \
	--pcf up5k.pcf \
	--json $(TOP).json \
	--freq $(FREQ) \
	-l pnr.log

pack:
	icepack $(TOP).asc $(TOP).bin

prog:
	iceprog -S $(TOP).bin 

lint:
	verilator --lint-only -Wall \
	-Wno-PINCONNECTEMPTY \
	-D__ICARUS__ \
	-DICE40 \
	-DSBOX_GF \
	-DSS2_WRAPPER \
	-DGOOGLE_VAULT_AES \
	-I../hdl/ \
	$(FILES) \
	--top-module $(TOP) \
	2>&1 | tee lint.out \


