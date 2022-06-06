# This Makef^ile defines the standard rules for building the project.
#
# The following variables are used:
#
# ZTEXPREFIX
#   Defines the location of the EZ-USB SDK
#   Must be defined!
#   Example: ZTEXPREFIX=../../..
#
# JARTARGET
#   The name of the jar archive
#   Example: JARTARGET=UCEcho.jar
#
# CLASSTARGETS
#   Java Classes that have to be build 
#   Example: CLASSTARGETS=UCEcho.class
#
# CLASSEXTRADEPS
#   Extra dependencies for Java Classes
#   Example: CLASSEXTRADEPS:=$(wildcard $(ZTEXPREFIX)/java/ztex/*.java)
#
# IHXTARGETS
#   ihx files (firmware ROM files) that have to be build 
#   Example: IHXTARGETS=ucecho.ihx
#
# IHXEXTRADEPS
#   Extra Dependencies for ihx files
#   Example: IHXEXTRADEPS:=$(wildcard $(ZTEXPREFIX)/include/*.h)
#
# EXTRAJARFILES
#   Extra files that should be included into the jar archive
#   Example: EXTRAJARFILES=ucecho.ihx fpga/ucecho.bin
# EXTRAJARFLAGS
#   Extra flags for the jar command
#   Example: EXTRAJARFLAGS=-C com

# EXTRACLEANFILES
#   Extra files that should be cleaned by target "clean"

# EXTRADISTCLEANFILES
#   Extra files that should be cleaned by target "distclean"


.PHONY: all ihx jar clean distclean default avr avrclean avrdistclean
.SUFFIXES: .ihx .class .jar .java .c

JAVAC=javac
SDCC=$(ZTEXPREFIX)/bin/bmpsdcc.sh
CLASSPATH:=.:$(ZTEXPREFIX)/libusbJava:$(ZTEXPREFIX)/java
INCLUDES=-I $(ZTEXPREFIX)/include/

all : ihx jar 
ihx : $(IHXTARGETS)
jar : $(JARTARGET)

%.ihx: %.c $(IHXEXTRADEPS)
	$(SDCC) $< "$(INCLUDES)"

%.class: %.java $(CLASSEXTRADEPS)
	$(JAVAC) -cp "$(CLASSPATH)" $<

$(JARTARGET) : $(CLASSTARGETS) $(EXTRAJARFILES)
	jar cf $(JARTARGET) *.class $(EXTRAJARFILES) $(EXTRAJARFLAGS) -C $(ZTEXPREFIX)/libusbJava . $(shell cd $(ZTEXPREFIX)/java; ls ztex/*.class | while read a; do echo "-C $(ZTEXPREFIX)/java $$a"; done)

clean: 
	rm -f *~ *.bak *.old
	rm -f *.class 
	rm -f *.rel *.rst *.lnk *.lst *.map *.asm *.sym *.mem *.tmp.c 
	rm -f $(EXTRACLEANFILES)

distclean: clean
	rm -f $(JARTARGET)
	rm -f *.ihx
	rm -f $(EXTRADISTCLEANFILES)
	
avr:
	if [ -d avr ]; then make -C avr all; fi

avrclean:
	if [ -d avr ]; then make -C avr clean; fi

avrdistclean:
	if [ -d avr ]; then make -C avr distclean; fi
