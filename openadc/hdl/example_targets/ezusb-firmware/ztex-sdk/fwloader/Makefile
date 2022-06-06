#########################
# configuration section #
#########################

ZTEXPREFIX=..

#PREFIX  base directory for installing 
#default: /usr/local for OS=UNIX and c:\usr else
#e.g. PREFIX=/usr

#JAVADIR  where to install jar's
#default: $(PREFIX)/share/java

#BINDIR  where to install bin's
#default: $(PREFIX)/bin

#MANDIR  where to install mans's
#default: $(PREFIX)/share/man/man1

################################
# DO NOT CHANAGE THE FOLLOWING #
################################

.PHONY: all jar clean distclean install

JARTARGET=FWLoader.jar
CLASSTARGETS=FWLoader.class

JAVAC=javac
INSTALL=install
INSTALLDIR=$(INSTALL) -d 
INSTALLEXE=$(INSTALL) -m 0755
INSTALLFILE=$(INSTALL) -m 0644

CLASSPATH:=.:$(ZTEXPREFIX)/libusbJava:$(ZTEXPREFIX)/java:/usr/local/share/java:/usr/share/java:$(CLASSPATH)
CLASSEXTRADEPS:=$(wildcard ztex/*.java)

ifndef PREFIX
  PREFIX=/usr/local
endif

ifndef JAVADIR
  JAVADIR=$(PREFIX)/share/java
endif

ifndef BINDIR
  BINDIR=$(DESTDIR)$(PREFIX)/bin
endif

ifndef MANDIR
  MANDIR=$(DESTDIR)$(PREFIX)/share/man/man1
endif

all : jar

jar : $(JARTARGET)

%.class: %.java $(CLASSEXTRADEPS)
	$(JAVAC) -cp "$(CLASSPATH)" $< $(CLASSEXTRADEPS)

$(JARTARGET) : $(CLASSTARGETS) $(EXTRAJARFILES)
	jar cf $(JARTARGET) *.class $(EXTRAJARFILES) -C $(ZTEXPREFIX)/libusbJava . $(shell cd $(ZTEXPREFIX)/java; ls ztex/*.class | while read a; do echo "-C $(ZTEXPREFIX)/java $$a"; done)

install: $(CLASSTARGETS)
#	$(INSTALLDIR) $(JAVADIR)
#	jar cf $(JAVADIR)/ztex.jar *.class ztex/*.class
	$(INSTALLDIR) $(JAVADIR)/ztex
	$(INSTALLFILE) *.class $(JAVADIR)
	$(INSTALLFILE) ztex/*.class $(JAVADIR)/ztex
	$(INSTALLDIR) $(BINDIR)
	$(INSTALLEXE) FWLoader $(BINDIR)
	$(INSTALLDIR) $(MANDIR)
	$(INSTALLFILE) FWLoader.1 $(MANDIR)

clean: 
	rm -f *~ *.bak *.old
	rm -f ztex/*~ ztex/*.bak ztex/*.old
	rm -f *.class 
	rm -f *.rel *.rst *.lnk *.lst *.map *.asm *.sym *.mem *.tmp.c 

distclean: clean
	rm -f $(JARTARGET)
	rm -f ztex/*.class
	rm -f *.ihx
