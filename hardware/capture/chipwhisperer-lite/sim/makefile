all: compile

compile:
	iverilog -ccmdfile \
	-g2005-sv \
 	-I../../../../openadc/hdl/hdl/ \
	-I../../../capture/chipwhisperer-lite/hdl/cwlite_ise/ \
	-Wportbind \
	-Wselect-range \

clean:
	rm -f a.out
