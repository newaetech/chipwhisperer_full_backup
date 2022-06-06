DIRS=bmp examples java libusbJava-src

.PHONY: default all clean distclean avr avrclean avrdistclean

default: 
	@echo "This makefile is intended to clean up the project or to build the utilties and examples"
	@echo "Usage: make all | docs | clean | distclean"

all:
	make -C java all
	make -C examples all

docs:
	make -C docs all

clean: 
	set -e; for i in $(DIRS); do make -C $$i clean; done

distclean:
	set -e; for i in $(DIRS); do make -C $$i distclean; done

avr: 
	make -C examples avr

avrclean: 
	make -C examples avrclean

avrdistclean: 
	make -C examples avrdistclean
