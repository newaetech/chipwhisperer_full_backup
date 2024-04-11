compile:
	iverilog -ccmdfile \
	-g2005-sv \
	-I../../../../openadc/hdl/hdl \
        -I../hdl/cwlite_ise \
	-Wportbind \
	-Wselect-range \

lint:
	verilator --lint-only -Wall \
	--error-limit 0 \
	-Wno-PINCONNECTEMPTY \
	-Wno-ASSIGNDLY \
	-f cmdfile \
	-I../../../../openadc/hdl/hdl \
        -I../hdl/cwlite_ise \
	-D__ICARUS__ \
	--top-module cwlite_interface \
	2>&1 | tee lint.out \
