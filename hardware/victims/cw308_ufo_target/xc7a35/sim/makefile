LOGFILE=make.log
EXEFILE=a.out
WAVEFORMAT=fst
SEED=1
TIMEOUT_CYCLES=50000
DUMP=0
COCOTB_LOG_LEVEL=INFO
VIVADO_VERSION=2020.2
PERIOD=100
BAUD=115200
REPS=10

all: compile_ss2 run_ss2

ifeq ($(VIVADO_PATH),)
  ifeq ($(OS), Windows_NT)
    VIVADO_PATH=/mnt/c/Xilinx/Vivado/
  else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
      VIVADO_PATH=/tools/Xilinx/Vivado/
    endif
    ifeq ($(UNAME_S),Darwin)
      VIVADO_PATH=$(HOME)/tools/Xilinx/Vivado/
    endif
  endif
endif

compile_ss2:
	iverilog -g2005-sv \
	-gsupported-assertions \
	-y/$(VIVADO_PATH)/$(VIVADO_VERSION)/ids_lite/ISE/verilog/src/unisims/ \
	-s ss2_cocowrapper \
	-s glbl \
	-Wportbind \
	-Wselect-range \
	-DCOCOTB_SIM=1 \
	-DSS2_EXAMPLE=1 \
	-P ss2_cocowrapper.pDUMP=$(DUMP) \
	-o $(EXEFILE) \
	../hdl/ss2_wrapper.v \
	../hdl/ss2.v \
	../hdl/crc.v \
	../hdl/cw305_ss2_example_top.v \
	../hdl/cw305_usb_reg_fe.v \
	../hdl/cw305_reg_test_ss2.v \
	../fpga-common/hdl/fifo_sync.v \
	../fpga-common/hdl/uart_core.v \
	$(VIVADO_PATH)/$(VIVADO_VERSION)/ids_lite/ISE/verilog/src/glbl.v \
	ss2_cocowrapper.v \


lint_aes_ss:
	verilator --lint-only -Wall \
	--error-limit 0 \
	-Wno-PINCONNECTEMPTY \
	-Wno-ASSIGNDLY \
	-Wno-LATCH \
	-I../hdl/ \
	-D__ICARUS__ \
	-DAES_INSTANCES=2 \
	../hdl/cw312_aes_ss.v \
	../../spartan6lx9/hdl_src/UART.v \
	../../spartan6lx9/hdl_src/SimpleSerial.v \
	../../../cw305_artixtarget/fpga/cryptosrc/aes_googlevault/aes_sbox.v \
	../../../cw305_artixtarget/fpga/cryptosrc/aes_googlevault/aes_ks.v \
	../../../cw305_artixtarget/fpga/cryptosrc/aes_googlevault/aes_core.v \
	--top-module cw312_aes_ss \
	2>&1 | tee lint.out \

lint_ss2:
	verilator --lint-only -Wall \
	--error-limit 0 \
	-Wno-PINCONNECTEMPTY \
	-Wno-ASSIGNDLY \
	-Wno-LATCH \
	-I../hdl/ \
	-D__ICARUS__ \
	-DSS2_EXAMPLE=1 \
	../hdl/ss2_wrapper.v \
	../hdl/ss2.v \
	../hdl/crc.v \
	../hdl/cw305_ss2_example_top.v \
	../hdl/cw305_usb_reg_fe.v \
	../hdl/cw305_reg_test_ss2.v \
	../fpga-common/hdl/fifo_sync.v \
	../fpga-common/hdl/uart_core.v \
	--top-module ss2_wrapper \
	2>&1 | tee lint.out \

# note that -M and -m arguments must immediately follow vvp, otherwise Python doesn't start!
run_ss2:
	COCOTB_RESOLVE_X=RANDOM \
	TOPLEVEL=ss2_cocowrapper \
	TOPLEVEL_LANG=verilog \
        PERIOD=$(PERIOD) \
        BAUD=$(BAUD) \
        REPS=$(REPS) \
	MODULE=test_ss2 \
	COCOTB_LOG_LEVEL=$(COCOTB_LOG_LEVEL) \
	LIBPYTHON_LOC=$(shell cocotb-config --libpython) \
	vvp -M $(shell cocotb-config --lib-dir) \
	-m $(shell cocotb-config --lib-name vpi icarus) \
	-l$(LOGFILE) $(EXEFILE) -$(WAVEFORMAT) \

clean:
	rm -f a.out results/*.vvp
