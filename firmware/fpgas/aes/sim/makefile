LOGFILE=make.log
WAVEFORMAT=fst
SEED=1
TIMEOUT=50000
DUMP=0

all: compile run

compile:
	iverilog -ccmdfile \
	-g2005-sv \
	-Wportbind \
	-Wselect-range \
	-I../../../common/ \
	-I../hdl/ \
	-I. \
	-DGOOGLE_VAULT_AES \
	-P tb.pDUMP=$(DUMP) \
	-P tb.pSEED=$(SEED) \
	-P tb.pTIMEOUT=$(TIMEOUT) \

run:
	vvp -l$(LOGFILE) a.out -$(WAVEFORMAT)

clean:
	rm -f a.out
