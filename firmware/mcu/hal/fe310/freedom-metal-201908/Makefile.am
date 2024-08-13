# Copyright 2018-2019 SiFive, Inc
# SPDX-License-Identifier: Apache-2.0

# Every test depends on the generated linker script.
$(check_PROGRAMS): riscv__mmachine__@MACHINE_NAME@.lds

# Every test depends on the freshly-compiled library.
$(check_PROGRAMS): libriscv__mmachine__@MACHINE_NAME@.a

# Generates a linker script that's more reasonable that whatever GCC's default
# is.
ldsdir = $(libdir)
lds_DATA = riscv__mmachine__@MACHINE_NAME@.lds

if PRECONFIGURED
riscv__mmachine__@MACHINE_NAME@.lds: @MACHINE_HEADER@
	cp $< $@
else # !PRECONFIGURED
riscv__mmachine__@MACHINE_NAME@.lds: @LDSCRIPT_GENERATOR@ @MACHINE_NAME@.dtb
	$< --dtb $(filter %.dtb,$^) --linker $@
endif # PRECONFIGURED

# Generates a SPEC file that sets a reasonable set of default options for this
# build.
specdir = $(libdir)
spec_DATA =

spec_DATA += riscv__mmachine__@MACHINE_NAME@.specs
if PRECONFIGURED
riscv__mmachine__@MACHINE_NAME@.specs:
	touch $@
else # !PRECONFIGURED
riscv__mmachine__@MACHINE_NAME@.specs: @SPECS_GENERATOR@ @MACHINE_NAME@.dtb
	$< --dtb $(filter %.dtb,$^) --specs $@ --prefix @prefix@ --machine @MACHINE_NAME@
endif # PRECONFIGURED

spec_DATA += riscv__menv__metal.specs
riscv__menv__metal.specs: riscv__menv__metal.specs.in
	cat $^ > $@

# In order to generate code that's actually compatible with a machine we must
# pass the march and mabi arguments to GCC that coorespond to the hardware.
# This is handled by generating a makefile fragment, including it, and then
# adding those argument to CFLAGS.
-include @MACHINE_NAME@.mk
if PRECONFIGURED
@MACHINE_NAME@.mk: @MACHINE_MAKEATTRIBUTES@
	cp $< $@
else # !PRECONFIGURED
@MACHINE_NAME@.mk: @MAKEATTRIBUTES_GENERATOR@ @MACHINE_NAME@.dtb
	$< --dtb $(filter %.dtb,$^) --output $@
endif # PRECONFIGURED

# Install some METAL-specific headers, one of which is automatically generated.
# The files that aren't automatically generated are the same for all machines.
nobase_include_HEADERS = \
	metal/drivers/fixed-clock.h \
	metal/drivers/fixed-factor-clock.h \
	metal/drivers/riscv_clint0.h \
	metal/drivers/riscv_cpu.h \
	metal/drivers/riscv_plic0.h \
	metal/drivers/sifive_ccache0.h \
	metal/drivers/sifive_clic0.h \
	metal/drivers/sifive_fe310-g000_hfrosc.h \
	metal/drivers/sifive_fe310-g000_hfxosc.h \
	metal/drivers/sifive_fe310-g000_lfrosc.h \
	metal/drivers/sifive_fe310-g000_pll.h \
	metal/drivers/sifive_fe310-g000_prci.h \
	metal/drivers/sifive_fu540-c000_l2.h \
	metal/drivers/sifive_global-external-interrupts0.h \
	metal/drivers/sifive_gpio-buttons.h \
	metal/drivers/sifive_gpio-leds.h \
	metal/drivers/sifive_gpio-switches.h \
	metal/drivers/sifive_gpio0.h \
	metal/drivers/sifive_local-external-interrupts0.h \
	metal/drivers/sifive_rtc0.h \
	metal/drivers/sifive_spi0.h \
	metal/drivers/sifive_test0.h \
	metal/drivers/sifive_trace.h \
	metal/drivers/sifive_uart0.h \
	metal/drivers/sifive_wdog0.h \
	metal/machine/inline.h \
	metal/machine/platform.h \
	metal/button.h \
	metal/cache.h \
	metal/clock.h \
	metal/compiler.h \
	metal/cpu.h \
	metal/gpio.h \
	metal/interrupt.h \
	metal/io.h \
	metal/itim.h \
	metal/led.h \
	metal/lock.h \
	metal/machine.h \
	metal/memory.h \
	metal/pmp.h \
	metal/privilege.h \
	metal/rtc.h \
	metal/shutdown.h \
	metal/spi.h \
	metal/switch.h \
	metal/timer.h \
	metal/time.h \
	metal/tty.h \
	metal/uart.h \
	metal/watchdog.h

if PRECONFIGURED
metal/machine.h: @MACHINE_HEADER@
	@mkdir -p $(dir $@)
	cp $< $@

metal/machine/inline.h: @MACHINE_INLINE@
	@mkdir -p $(dir $@)
	cp $< $@

metal/machine/platform.h: @PLATFORM_HEADER@
	@mkdir -p $(dir $@)
	cp $< $@

else # !PRECONFIGURED
# Builds the machine-specific METAL header file, which paramaterizes the METAL for
# one specific machine.  This is automatically picked up by GCC so users
# automatically get the relevant definitions.  This is a two-step process:
# first a DTB is built, and then the DTB is converted to a header file using an
# external tool.
@MACHINE_NAME@.dtb: @DTC@ @MACHINE_DTS@
	$< $(filter %.dts,$^) -o $@ -O dtb -I dts

metal/machine.h: @METAL_HEADER_GENERATOR@ @MACHINE_NAME@.dtb
	@mkdir -p $(dir $@)
	$< --dtb $(filter %.dtb,$^) --output $@

metal/machine/platform.h: @BARE_HEADER_GENERATOR@ @MACHINE_NAME@.dtb
	@mkdir -p $(dir $@)
	$< -d $(filter %.dtb,$^) -o $@

endif # PRECONFIGURED

# Quash an automake warning.
lib_LIBRARIES =

# Everything in here is compiled into a single library, which contains all the
# source files in the project.  It's named for one specific machine, which GCC
# uses to select the target machine that this METAL implementation points at.
lib_LIBRARIES += libriscv__mmachine__@MACHINE_NAME@.a

libriscv__mmachine__@MACHINE_NAME@_a_CFLAGS  = @MENV_METAL@ @MMACHINE_MACHINE_NAME@
libriscv__mmachine__@MACHINE_NAME@_a_CCASFLAGS = @MENV_METAL@ @MMACHINE_MACHINE_NAME@

# This will generate these sources before the compilation step
BUILT_SOURCES = \
	metal/machine.h \
	metal/machine/inline.h \
	metal/machine/platform.h \
	riscv__menv__metal.specs \
	riscv__mmachine__@MACHINE_NAME@.specs

libriscv__mmachine__@MACHINE_NAME@_a_SOURCES = \
	src/drivers/fixed-clock.c \
	src/drivers/fixed-factor-clock.c \
	src/drivers/inline.c \
	src/drivers/riscv_clint0.c \
	src/drivers/riscv_cpu.c \
	src/drivers/riscv_plic0.c \
	src/drivers/sifive_ccache0.c \
	src/drivers/sifive_clic0.c \
	src/drivers/sifive_fe310-g000_hfrosc.c \
	src/drivers/sifive_fe310-g000_hfxosc.c \
	src/drivers/sifive_fe310-g000_lfrosc.c \
	src/drivers/sifive_fe310-g000_pll.c \
	src/drivers/sifive_fe310-g000_prci.c \
	src/drivers/sifive_fu540-c000_l2.c \
	src/drivers/sifive_global-external-interrupts0.c \
	src/drivers/sifive_gpio-buttons.c \
	src/drivers/sifive_gpio-leds.c \
	src/drivers/sifive_gpio-switches.c \
	src/drivers/sifive_gpio0.c \
	src/drivers/sifive_local-external-interrupts0.c \
	src/drivers/sifive_rtc0.c \
	src/drivers/sifive_spi0.c \
	src/drivers/sifive_test0.c \
	src/drivers/sifive_trace.c \
	src/drivers/sifive_uart0.c \
	src/drivers/sifive_wdog0.c \
	src/button.c \
	src/cache.c \
	src/clock.c \
	src/cpu.c \
	src/entry.S \
	src/gpio.c \
	src/interrupt.c \
	src/led.c \
	src/lock.c \
	src/memory.c \
	src/pmp.c \
	src/privilege.c \
	src/rtc.c \
	src/shutdown.c \
	src/spi.c \
	src/switch.c \
	src/synchronize_harts.c \
	src/timer.c \
	src/time.c \
	src/trap.S \
	src/tty.c \
	src/uart.c \
	src/vector.S \
	src/watchdog.c

# Freedom METAL has its own libgloss implementation that is only built in
# --with-builtin-libgloss is passed to configure.
if WITH_BUILTIN_LIBGLOSS

lib_LIBRARIES += libriscv__menv__metal.a

libriscv__menv__metal_a_SOURCES = \
	gloss/crt0.S \
	gloss/nanosleep.c \
	gloss/sys_access.c \
	gloss/sys_chdir.c \
	gloss/sys_chmod.c \
	gloss/sys_chown.c \
	gloss/sys_close.c \
	gloss/sys_execve.c \
	gloss/sys_exit.c \
	gloss/sys_faccessat.c \
	gloss/sys_fork.c \
	gloss/sys_fstat.c \
	gloss/sys_fstatat.c \
	gloss/sys_ftime.c \
	gloss/sys_getcwd.c \
	gloss/sys_getpid.c \
	gloss/sys_gettimeofday.c \
	gloss/sys_isatty.c \
	gloss/sys_kill.c \
	gloss/sys_link.c \
	gloss/sys_lseek.c \
	gloss/sys_lstat.c \
	gloss/sys_open.c \
	gloss/sys_openat.c \
	gloss/sys_read.c \
	gloss/sys_sbrk.c \
	gloss/sys_stat.c \
	gloss/sys_sysconf.c \
	gloss/sys_times.c \
	gloss/sys_unlink.c \
	gloss/sys_utime.c \
	gloss/sys_wait.c \
	gloss/sys_write.c

endif

# Quash an automake warning.
check_PROGRAMS =

# The simplest possible pair of tests: one that passes and one that fails
check_PROGRAMS       += return_pass
return_pass_SOURCES   = test/return_pass.c
return_pass_CFLAGS    = @MENV_METAL@ @MMACHINE_MACHINE_NAME@
return_pass_LDFLAGS   = -L. -Wl,--gc-sections -Wl,-Map=return_pass.map

check_PROGRAMS       += return_fail
return_fail_SOURCES   = test/return_fail.c
return_fail_CFLAGS    = @MENV_METAL@ @MMACHINE_MACHINE_NAME@
return_fail_LDFLAGS   = -L. -Wl,--gc-sections -Wl,-Map=return_fail.map

# A simple "Hello, World!" program that directly uses the METAL interface to
# print to the serial terminal.
check_PROGRAMS       += hello
hello_SOURCES         = test/hello.c
hello_CFLAGS          = @MENV_METAL@ @MMACHINE_MACHINE_NAME@
hello_LDFLAGS         = -L. -Wl,--gc-sections -Wl,-Map=hello.map

# Extra clean targets
clean-local:
	-rm -rf @MACHINE_NAME@.mk
	-rm -rf metal/machine.h metal/machine/platform.h
	-rm -rf @MACHINE_NAME@.dtb metal-@MACHINE_NAME@.lds
	-rm -rf *.map *.specs
