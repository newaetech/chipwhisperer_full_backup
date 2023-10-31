LOGFILE=make.log
WAVEFORMAT=fst
SEED=1
VERBOSE=0
TIMEOUT=12000000
DUMP=0

all: compile run

compile:
	iverilog -ccmdfile \
	-g2005-sv \
	-Wportbind \
	-Wselect-range \
	-I../hdl/ \
	-I. \
	-I../../cryptosrc/cryptech/ecdsa256/rtl/curve/ \
	-I../../cryptosrc/cryptech/ecdsa256/rtl/lowlevel/ \
	-I../../cryptosrc/cryptech/ecdsa256/rtl/modular/modular_invertor/ \
	-P tb.pVERBOSE=$(VERBOSE) \
	-P tb.pDUMP=$(DUMP) \
	-P tb.pSEED=$(SEED) \
	-P tb.pTIMEOUT=$(TIMEOUT) \

run:
	vvp -l$(LOGFILE) a.out -$(WAVEFORMAT)

clean:
	rm -f a.out
