include ../common.mk

#ARMPATH=~/DevelToolbin/binaries/armThumb-4.4.6-interwork/bin/
ARMPATH=~/DevelToolbin/binaries/armThumb-4.6.2/bin/
CC=$(ARMPATH)arm-elf-gcc
OBJCPY=$(ARMPATH)arm-elf-objcopy
OBJDUMP=$(ARMPATH)arm-elf-objdump

LIBADUC = ../aduc/

# include directory
CINCS = -I../include -I$(LIBADUC)

CCFLAGS=  -mcpu=arm7tdmi -g -Wall -Wextra -Wstrict-prototypes -std=gnu99
# uncomment if you want to have optimized code otherwise resonably debug the code
CCFLAGS += -Os

# options generating the linker command file
CCLFAGS_LCMD = -E -nostdinc -P -C $(CINCS)

# options generating startup code
ASMFLAGS= -mcpu=arm7tdmi -I. -x assembler-with-cpp -g -Wall -Wextra

# linker flags used in all personalities
LDFLAGS_COMMON = -mcpu=arm7tdmi -Os -I. -nostartfiles

OBJ_LIBADUC = $(LIBADUC)flash.o  $(LIBADUC)init.o  $(LIBADUC)interrupt.o  $(LIBADUC)target_init.o
OBJ_COMMON = main.o checksum.o uart-comm.o frame-comm.o packet-comm.o software-version.o switch.o

# Notes:
#   * Run "make BUILD_FREEMCAN_PRINTF=yes" to build the printf code
#     into the firmware.
#   * Adds lots of code (approx. 16kb flash) und uses lots of ram

ifeq ($(BUILD_FREEMCAN_PRINTF),yes)
OBJ_COMMON += uart-printf.o syscalls.o
CCFLAGS += -DHAVE_UPRINTF_IMPLEMENTATION
endif

########################################################################################

OBJ_ADC_INT_MCA = $(OBJ_LIBADUC) $(OBJ_COMMON) perso-adc-int-mca-ext-trig.o timer1-countdown-and-stop.o timer1-get-duration.o timer1-init-simple.o
LDFLAGS_ADC_INT_MCA =$(LDFLAGS_COMMON) -T$(LIBADUC)project.lds -Wl,-Map=firmware-adc-int-mca.map,--cref -gstabs

OBJ_ADC_INT_MCA_TIMED = $(OBJ_LIBADUC) $(OBJ_COMMON) perso-adc-int-mca-timed-trig.o timer1-adc-trigger.o
LDFLAGS_ADC_INT_MCA_TIMED =$(LDFLAGS_COMMON) -T$(LIBADUC)project.lds -Wl,-Map=firmware-adc-int-mca-timed.map,--cref -gstabs

OBJ_ADC_INT_TIMED_SAMPLING = $(OBJ_LIBADUC) $(OBJ_COMMON) perso-adc-int-log-timed-trig.o timer1-adc-trigger.o data-table-all-other-memory.o
LDFLAGS_ADC_INT_TIMED_SAMPLING =$(LDFLAGS_COMMON) -T$(LIBADUC)project.lds data_table_empty_ram.lds -Wl,--defsym=MALLOC_HEAP_SIZE=500 -Wl,-Map=firmware-adc-int-timed-sampling.map,--cref -gstabs

########################################################################################

### call for linkerfiles and firmware compile & link ###

all: firmware-adc-int-mca firmware-adc-int-mca-timed firmware-adc-int-timed-sampling

firmware-adc-int-mca: $(LIBADUC)project.lds make-adc-int-mca

firmware-adc-int-mca-timed: $(LIBADUC)project.lds make-adc-int-mca-timed

firmware-adc-int-timed-sampling: $(LIBADUC)project.lds data_table_empty_ram.lds make-adc-int-timed-sampling


########################################################################################

### firmware compile and link ###

make-adc-int-mca:	$(OBJ_ADC_INT_MCA)
	$(CC) $(LDFLAGS_ADC_INT_MCA) -o firmware-adc-int-mca.elf $^
	$(OBJCPY) --output-target ihex firmware-adc-int-mca.elf firmware-adc-int-mca.hex
	$(OBJCPY) --output-target binary firmware-adc-int-mca.elf firmware-adc-int-mca.bin
	$(OBJDUMP) -h -S firmware-adc-int-mca.elf > firmware-adc-int-mca.lss

make-adc-int-mca-timed:	$(OBJ_ADC_INT_MCA_TIMED)
	$(CC) $(LDFLAGS_ADC_INT_MCA_TIMED) -o firmware-adc-int-mca-timed.elf $^
	$(OBJCPY) --output-target ihex firmware-adc-int-mca-timed.elf firmware-adc-int-mca-timed.hex
	$(OBJCPY) --output-target binary firmware-adc-int-mca-timed.elf firmware-adc-int-mca-timed.bin
	$(OBJDUMP) -h -S firmware-adc-int-mca-timed.elf > firmware-adc-int-mca-timed.lss

make-adc-int-timed-sampling:	$(OBJ_ADC_INT_TIMED_SAMPLING)
	$(CC) $(LDFLAGS_ADC_INT_TIMED_SAMPLING) -o firmware-adc-int-timed-sampling.elf $^
	$(OBJCPY) --output-target ihex firmware-adc-int-timed-sampling.elf firmware-adc-int-timed-sampling.hex
	$(OBJCPY) --output-target binary firmware-adc-int-timed-sampling.elf firmware-adc-int-timed-sampling.bin
	$(OBJDUMP) -h -S firmware-adc-int-timed-sampling.elf > firmware-adc-int-timed-sampling.lss

########################################################################################

### Rule for LIB ADUC ###

#%.o : %.c $(HEADERS)
#	$(CC) $(CCFLAGS) $< -marm -c -o $@

# rule generating the aduc base linker command file
$(LIBADUC)project.lds : $(LIBADUC)project.lds.S
	$(CC) $(CCLFAGS_LCMD) $< -c -o $@

# \todo: something changed during update from gcc 4.4.6 to gcc 4.6.2
$(LIBADUC)target_init.o : $(LIBADUC)target_init.S
	$(CC) $(ASMFLAGS) $< $(CINCS) -c -o $@
#	$(CC) $(ASMFLAGS) $< -Wa,-adhlns=$(LIBADUC)target_init.lst, $(CINCS) -c -o $@

$(LIBADUC)flash.o : $(LIBADUC)flash.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -mthumb -mthumb-interwork -c -o $@

$(LIBADUC)interrupt.o : $(LIBADUC)interrupt.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -marm -mthumb-interwork -c -o $@

$(LIBADUC)init.o : $(LIBADUC)init.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -marm -mthumb-interwork -c -o $@

########################################################################################

### Rule for COMMON OBJECTS ###

switch.o : switch.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -marm -mthumb-interwork -c -o $@

software-version.o : software-version.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -marm -mthumb-interwork -c -o $@

checksum.o : checksum.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -marm -mthumb-interwork -c -o $@

main.o : main.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -marm -mthumb-interwork -c -o $@

uart-comm.o : uart-comm.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -marm -mthumb-interwork -c -o $@

frame-comm.o : frame-comm.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -marm -mthumb-interwork -c -o $@

packet-comm.o : packet-comm.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -marm -mthumb-interwork -c -o $@


### Used by more than one personality ###

timer1-adc-trigger.o : timer1-adc-trigger.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -marm -mthumb-interwork -c -o $@

### Specials like printf ###

syscalls.o : syscalls.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -marm -mthumb-interwork -c -o $@

uart-printf.o : uart-printf.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -marm -mthumb-interwork -c -o $@


########################################################################################

### Rule for OBJ_ADC_INT_MCA ###

timer1-init-simple.o : timer1-init-simple.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -marm -mthumb-interwork -c -o $@

perso-adc-int-mca-ext-trig.o : perso-adc-int-mca-ext-trig.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -marm -mthumb-interwork -c -o $@

timer1-countdown-and-stop.o : timer1-countdown-and-stop.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -marm -mthumb-interwork -c -o $@

timer1-get-duration.o : timer1-get-duration.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -marm -mthumb-interwork -c -o $@

########################################################################################

### Rule for OBJ_ADC_INT_MCA_TIMED ###

perso-adc-int-mca-timed-trig.o : perso-adc-int-mca-timed-trig.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -marm -mthumb-interwork -c -o $@

########################################################################################

### Rule for OBJ_ADC_INT_TIMED_SAMPLING ###

# rule linker command files augmenting the base linker command file
data_table_empty_ram.lds :  data_table_empty_ram.lds.S
	$(CC) $(CCLFAGS_LCMD) $< -c -o $@

perso-adc-int-log-timed-trig.o : perso-adc-int-log-timed-trig.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -marm -mthumb-interwork -c -o $@

data-table-all-other-memory.o : data-table-all-other-memory.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -marm -mthumb-interwork -c -o $@

########################################################################################

clean:
	$(RM) *.o *.bin *.map *.hex *.elf *.lss *.lds
	$(RM) $(LIBADUC)*.o $(LIBADUC)*.lst $(LIBADUC)*.lds
	$(RM) main
	$(RM) -r ./dox/

dox:
	doxygen Doxyfile

