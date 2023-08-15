TOP=iCE40UP5K_SS2
FREQ=20

all: time synth pnr pack prog
bitfile: time synth pnr pack

time:
	python ./generate_timestamp.py

synth:
	yosys -p 'synth_ice40 -top $(TOP) -json $(TOP).json' \
	-D ICE40 \
	-D SBOX_GF \
	-D SS2_WRAPPER \
	-D GOOGLE_VAULT_AES \
	../hdl/iCE40UP5K_SS2.v \
	../../../cw305_artixtarget/fpga/cryptosrc/aes_googlevault/aes_ks.v \
	../../../cw305_artixtarget/fpga/cryptosrc/aes_googlevault/aes_sbox.v \
	../../../cw305_artixtarget/fpga/cryptosrc/aes_googlevault/aes_core.v  \
	../../../cw305_artixtarget/fpga/cryptosrc/aes_newae/aes_sbox_lut.v \
	../../../cw305_artixtarget/fpga/common/cdc_pulse.v \
	../../../cw305_artixtarget/fpga/common/cw305_reg_aes.v \
	../../../cw305_artixtarget/fpga/common/cw305_top.v \
	../../../cw305_artixtarget/fpga/common/cw305_usb_reg_fe.v \
	../../xc7a35/fpga-common/hdl/fifo_sync.v \
	../../xc7a35/fpga-common/hdl/uart_core.v \
	../../xc7a35/fpga-common/hdl/crc_ss2.v \
	../../xc7a35/fpga-common/hdl/ss2.v \
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
	-I../../../cw305_artixtarget/fpga/common/ \
	../hdl/iCE40UP5K_SS2.v \
	../../../cw305_artixtarget/fpga/cryptosrc/aes_googlevault/aes_ks.v \
	../../../cw305_artixtarget/fpga/cryptosrc/aes_googlevault/aes_sbox.v \
	../../../cw305_artixtarget/fpga/cryptosrc/aes_googlevault/aes_core.v  \
	../../../cw305_artixtarget/fpga/cryptosrc/aes_newae/aes_sbox_lut.v \
	../../../cw305_artixtarget/fpga/common/cdc_pulse.v \
	../../xc7a35/hdl/crc.v \
	../../xc7a35/hdl/ss2.v \
	../../../cw305_artixtarget/fpga/common/cw305_reg_aes.v \
	../../../cw305_artixtarget/fpga/common/cw305_top.v \
	../../../cw305_artixtarget/fpga/common/cw305_usb_reg_fe.v \
	../../xc7a35/fpga-common/hdl/fifo_sync.v \
	../../xc7a35/fpga-common/hdl/uart_core.v \
	--top-module $(TOP) \
	2>&1 | tee lint.out \

