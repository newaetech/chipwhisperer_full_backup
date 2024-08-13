#
# Copyright (c) 2016, Intel Corporation
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the Intel Corporation nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

PDF_NAME= "Quark Microcontroller Software Interface 1.3.pdf"

V ?= 0

DOXYFILE_CMD = ( cat Doxyfile ; echo "GENERATE_HTML=YES" ) | doxygen -
DOXYFILE_0 = @echo "Create HTML Doxygen";  $(DOXYFILE_CMD) > /dev/null 2>&1
DOXYFILE_1 = $(DOXYFILE_CMD)
DOXYFILE = $(DOXYFILE_$(V))

DOXYLATEX_CMD = ( cat Doxyfile ; echo "GENERATE_LATEX=YES" ) | doxygen -
DOXYLATEX_0 = @echo "Create LaTeX Doxygen"; $(DOXYLATEX_CMD) > /dev/null 2>&1
DOXYLATEX_1 = $(DOXYLATEX_CMD)
DOXYLATEX = $(DOXYLATEX_$(V))

MAKEPDF_0 = @echo "Creating PDF..."; make -C latex pdf > /dev/null 2>&1
MAKEPDF_1 = make -C latex pdf
MAKEPDF = $(MAKEPDF_$(V))

CAT_0 =
CAT_1 = cat doxygen.log
CAT = $(CAT_$(V))

.PHONY: clean all html pdf

all: html pdf

html:
	$(DOXYFILE)
	$(CAT)
	mkdir -p ../html
	cp html/* ../html/
pdf:
	$(DOXYLATEX)
	$(CAT)
	$(MAKEPDF)
	mkdir -p pdf
	cp latex/refman.pdf pdf/$(PDF_NAME)

clean:
	$(RM) -r latex
	$(RM) -r html
	$(RM) -r pdf
	$(RM) doxygen.log
