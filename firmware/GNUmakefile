# Hey Emacs, this is a -*- makefile -*-

# Notes:
#   * Run "make BUILD_FREEMCAN_PRINTF=yes" to build the printf code
#     into the firmware.
#     CAUTION: This adds about 1500 byte to the firmware.
#   * You can define these variables in settings.mk if you like.

# AVR-GCC Makefile template, derived from the WinAVR template (which
# is public domain), believed to be neutral to any flavor of "make"
# (GNU make, BSD make, SysV make).
#
# Relying on GNU make features now as I can't stand the compatible
# make deficiencies.


CFLAGS =
CLEANFILES =

include ../common.mk

# Override settings by defining them in settings.mk Things not defined
# in settings.mk will be later defined with ?= later in this Makefile.
# AVRDUDE_PROGRAMMER, AVRDUDE_PORT, MCU are good candidates for
# settings.mk.
-include settings.mk


CPU ?= arm7tdmi
TARGET = firmware
SRC = main.c uart-comm.c frame-comm.c packet-comm.c firmware-version.c
LIBADUC = ../aduc
ASRC = $(LIBADUC)/target_init.S
OPT = s
BSRC =

ifeq ($(BUILD_FREEMCAN_PRINTF),yes)
SRC += uart-printf.c syscalls.c
CFLAGS += -DHAVE_UPRINTF_IMPLEMENTATION
endif

# Name of this Makefile (used for "make depend").
MAKEFILE = GNUmakefile

# Debugging format.
#DEBUG =
DEBUG = stabs

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = -std=gnu99

# Place -D or -U options here
CDEFS =

# Place -I options here
CINCS = -I../include -I$(LIBADUC)


CDEBUG = -g$(DEBUG)
CWARN = -Wall -Wextra -Wstrict-prototypes
# Some more flags that might be useful for tuning:
#    -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
#CFLAGS += -fshort-enums
#CEXTRA = -Wa,-adhlns=$(<:.c=.lst)
CFLAGS += $(CDEBUG) $(CDEFS) $(CINCS) -O$(OPT) $(CWARN) $(CSTANDARD) $(CEXTRA)


ASFLAGS =
ASFLAGS += -Wa,-adhlns=$(<:.S=.lst),-gstabs
#ASFLAGS += -Wa,-adhlns=$(<:.S=.lst)
ASFLAGS += -Wall -Wextra $(CINCS)


#Additional libraries.

# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires MATH_LIB = - below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt

PRINTF_LIB =

# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt

SCANF_LIB =

MATH_LIB = -lm

LINKERFILE = -T$(LIBADUC)/project.ld

HAVE_CRTs = -nostartfiles

# External memory options

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# only used for heap (malloc()).
#EXTMEMOPTS = -Wl,--defsym=__heap_start=0x801100,--defsym=__heap_end=0x80ffff

EXTMEMOPTS =

LDMAP = -Wl,-Map=$(TARGET).map,--cref
LDFLAGS = $(EXTMEMOPTS) $(LDMAP) $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB) $(LINKERFILE) $(HAVE_CRTs)

ARMPATH=~/DevelToolbin/binaries/armThumb-4.4.6/bin/

ERL = erl
CC = $(ARMPATH)/arm-elf-gcc
OBJCOPY = $(ARMPATH)/arm-elf-objcopy
OBJDUMP = $(ARMPATH)/arm-elf-objdump
SIZE = $(ARMPATH)/arm-elf-size
NM = $(ARMPATH)/arm-elf-nm
REMOVE = rm -f
MV = mv -f
OD = od

# Define all object files.
OBJ = $(SRC:.c=.o) $(ASRC:.S=.o) $(BSRC:.bin=.o)

# Define all listing files.
LST = $(ASRC:.S=.lst) $(SRC:.c=.lst)

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mcpu=$(CPU) -I. $(CFLAGS)
ALL_ASFLAGS = -mcpu=$(CPU) -I. -x assembler-with-cpp $(ASFLAGS)

DEPEND_MK = .depend.mk
CLEANFILES += $(DEPEND_MK)

# Default target.
all: depend build

build: elf hex bin checks

ALL: depend elf hex bin lss sym checks

elf: $(TARGET).elf
hex: $(TARGET).hex
bin: $(TARGET).bin
lss: $(TARGET).lss
sym: $(TARGET).sym


.SUFFIXES: .elf .hex .bin .lss .sym

.elf.hex:
	$(OBJCOPY) --output-target ihex $< $@

.elf.bin:
	-$(OBJCOPY) --output-target binary $< $@

# Create extended listing file from ELF output file.
.elf.lss:
	$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
.elf.sym:
	$(NM) -n $< > $@


# Link: create ELF output file from object files.
$(TARGET).elf: depend $(OBJ)
	$(CC) $(ALL_CFLAGS) $(OBJ) --output $@ $(LDFLAGS)

# Check that the SRAM usage fits into SRAM
# size: __bss_end must be <= 0x11FFF
checks : check-bss-end
check-bss-end: $(TARGET).elf
	@MAX_BSS_END="$$(printf "%d\n" 0x11FFF)"; \
	BSS_END="$$(nm $< | awk '/__bss_end/ { print (strtonum("0x"$$1) % 0x10000); }')"; \
	printf "CHECK: MAX_BSS_END=0x%x >= BSS_END=0x%x\n" "$$MAX_BSS_END" "$$BSS_END"; \
	test "$$MAX_BSS_END" -ge "$$BSS_END"

# Check that SRAM + stack fit into the ATmega644's 4K of SRAM.
MIN_STACK_SIZE = 800
checks : check-stack-size
check-stack-size : $(TARGET).elf
	@MAX_BSS_END="$$(printf "%d\n" 0x1100)"; \
	BSS_END="$$(nm $< | awk '/__bss_end/ { print (strtonum("0x"$$1) % 0x10000); }')"; \
	STACK_START="$$(nm $< | awk '/__stack/ { print (strtonum("0x"$$1)); }')"; \
	SUM="$$(expr $$BSS_END + $(MIN_STACK_SIZE))"; \
	printf "CHECK: STACK_START=0x%x >= 0x%x = (BSS_END=0x%x + MIN_STACK_SIZE=0x%x)\n" \
		"$$STACK_START" "$$SUM" "$$BSS_END" "$(MIN_STACK_SIZE)"; \
	test "$$STACK_START" -ge "$$SUM"

# Caution: The "data" number includes FUSES and other things besides the .data section.
checks : check-avr-size
check-avr-size: $(TARGET).elf
	avr-size $<

# Compile: create object files from C source files.
.c.o:
	$(CC) -c $(ALL_CFLAGS) $< -o $@


# Compile: create assembler files from C source files.
.c.s:
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
.S.o:
	$(CC) -c $(ALL_ASFLAGS) $< -o $@


# Target: clean project.
clean:
	$(REMOVE) $(TARGET).hex $(TARGET).bin $(TARGET).cof $(TARGET).elf \
	$(TARGET).map $(TARGET).sym $(TARGET).lss \
	$(CLEANFILES) \
	$(OBJ) $(LST) $(SRC:.c=.s) $(SRC:.c=.d)

.PHONY: depend
depend: $(DEPEND_MK)

$(DEPEND_MK): $(SRC) $(ASRC)
	echo '# DO NOT MODIFY THIS FILE -- it is autogenerated' > $(DEPEND_MK)
	echo '# You may delete this file, though (for the same reason).' >> $(DEPEND_MK)
	$(COMPILE.c) $(ALL_CFLAGS) -M $(SRC) $(ASRC) >> $(DEPEND_MK)

.PHONY:	all build elf hex bin lss sym program coff extcoff clean depend fuses compare-fuses program-fuses read-fuses ALL

-include $(DEPEND_MK)

# End of GNUmakefile
