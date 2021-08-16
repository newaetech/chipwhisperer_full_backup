#!/bin/sh
# 
# Use this script to update Makefile.linux that is based on the Makefile.
# The original makefile is very explicit in the rules and a clean makefile would be a great idea but apparently
# that initial makefile has already genrated hence this workaround.
#
cat Makefile | \
 sed 's,$(QUOTE)C:\\Program Files (x86)\\Atmel\\Studio\\7.0\\toolchain\\arm\\arm-gnu-toolchain\\bin\\arm-none-eabi-gcc.exe$(QUOTE),arm-none-eabi-gcc,g' | \
 sed 's,C:\\Program Files (x86)\\Atmel\\Studio\\7.0\\toolchain\\arm\\arm-gnu-toolchain\\bin\\arm-none-eabi-,arm-none-eabi-,g' | \
 sed 's,cmd.exe,sh,g' | \
 sed 's,.exe,,g' | \
 sed 's,^src\/,..\/src\/,g'  \
 > Makefile.linux
