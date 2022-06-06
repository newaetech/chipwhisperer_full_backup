TOP=iCE40UP5K_SS_example
FREQ=20

all: synth pnr pack prog
bitfile: synth pnr pack

synth:
	yosys -p 'synth_ice40 -top $(TOP) -json $(TOP).json' \
	../hdl/iCE40UP5K_SS.v \
	../../spartan6lx9/hdl_src/SimpleSerial.v \
	../../spartan6lx9/hdl_src/UART.v \
	../../../cw305_artixtarget/fpga/cryptosrc/aes_googlevault/aes_ks.v \
	../../../cw305_artixtarget/fpga/cryptosrc/aes_googlevault/aes_sbox.v \
	../../../cw305_artixtarget/fpga/cryptosrc/aes_googlevault/aes_core.v  \
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
	../hdl/iCE40UP5K_SS.v \
	../../spartan6lx9/hdl_src/SimpleSerial.v \
	../../spartan6lx9/hdl_src/UART.v \
	../../../cw305_artixtarget/fpga/cryptosrc/aes_googlevault/aes_ks.v \
	../../../cw305_artixtarget/fpga/cryptosrc/aes_googlevault/aes_sbox.v \
	../../../cw305_artixtarget/fpga/cryptosrc/aes_googlevault/aes_core.v  \
	--top-module $(TOP) \
	2>&1 | tee lint.out \

