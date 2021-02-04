# Copyright (C) 1999-2011 Stefan Ziegenbalg
#
# main targets:
#   all             build bmp
#   install         install applications
#   install2        install applications to ../bin
#   uninstall       uninstall applications
#   resetconf       reset configuration
#   clean           clean everything but exe's
#   distclean       clean everything

#########################
# configuration section #
#########################
#OS  operating system (UNIX for unix like OS)
#default: detected automatically
#e.g. OS=UNIX

#PCEXTRAFLAGS  additional flags
#e.g. PCEXTRAFLAGS=-Xs
PCEXTRAFLAGS=-gl

#PREFIX  base directory for installing 
#default: /usr/local for OS=UNIX and c:\usr else
#e.g. PREFIX=/usr

#BINDIR  where to install bin's
#default: $(PREFIX)/bin
#e.g. BINDIR=/usr/bin

#MANDIR  where to install mans's
#default: $(PREFIX)/share/man/man1

################################
# DO NOT CHANAGE THE FOLLOWING #
################################
ifndef OS
ifeq ($(WINDIR),)
OS=UNIX
else
OS=WINDOS
endif
endif

SOURCES=src/bmp.pas src/bmpsys.pas src/textbuf.pas 

UNITS=-Fusrc

PCFLAGS:=-Scgm -O3rGp3 -FE. -Xs $(PCEXTRAFLAGS)
INSTALLDIR=$(INSTALL) -d 
INSTALLEXE=$(INSTALL) -m 0755
INSTALLFILE=$(INSTALL) -m 0644
RM=rm -f
ECHO=echo

ifeq ($(OS),UNIX)

CONFIGURESCRIPT=./configure
PC:=$(shell $(CONFIGURESCRIPT) pc "$(PCFLAGS)")
ifndef PC
    $(error Fatal: No correct compiler found or wrong flags)
endif    
INSTALL=$(shell $(CONFIGURESCRIPT) install2)

ifndef PREFIX
  PREFIX=/usr/local
endif

TARGETS=bmp
DEFS=-DUNIX

else

PC=ppc386
INSTALL=install

ifndef PREFIX
  PREFIX=c:\usr
endif

TARGETS=bmp.exe
DEFS=-DWINDOWS

endif

ifndef BINDIR
  BINDIR=$(DESTDIR)$(PREFIX)/bin
endif

ifndef MANDIR
  MANDIR=$(DESTDIR)$(PREFIX)/share/man/man1
endif

.PHONY: all install uninstall resetconf clean distclean

all: bmp.made

bmp.made: $(TARGETS)
	$(ECHO) > bmp.made

bmp bmp.exe: $(SOURCES)
	$(PC) $(PCFLAGS) $(UNITS) src/bmp.pas

install: $(TARGETS)
	$(INSTALLDIR) $(BINDIR)
	$(INSTALLEXE) $(TARGETS) $(BINDIR)
	$(INSTALLDIR) $(MANDIR)
	$(INSTALLFILE) bmp.1 $(MANDIR)

install2: $(TARGETS)
	$(INSTALLEXE) $(TARGETS) ../bin

uninstall:
	- $(RM) $(addprefix $(BINDIR)/,$(TARGETS))

resetconf: 
	- $(RM) Makefile.conf
	- $(RM) conf_*
  
clean:
	- $(RM) *.bak *.o *.ppu *.s */*.bak *.log *.a *~ */*~ *.out
	- $(RM) ppas.sh link.res
	
	
distclean: clean resetconf
	- $(RM) bmp bmp.exe bmp.made bmp.pas
