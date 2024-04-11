LOGFILE=sim.log
SEED=0
VERBOSE=0
TIMEOUT=12000000
WAVEFORMAT=fst
DUMP=0


all: compile run

compile:
	iverilog -ccmdfile \
	-g2005-sv \
	-Wportbind \
	-Wselect-range \
	-I../rtl/curve/ \
	-I../rtl/lowlevel/ \
	-I../rtl/modular/modular_invertor/ \
 	-P tb_curve_multiplier_256.pVERBOSE=$(VERBOSE) \
 	-P tb_curve_multiplier_256.pDUMP=$(DUMP) \
 	-P tb_curve_multiplier_256.pSEED=$(SEED) \
 	-P tb_curve_multiplier_256.pTIMEOUT=$(TIMEOUT) \

run:
	vvp -l$(LOGFILE) a.out -$(WAVEFORMAT)

clean:
	rm -f a.out
