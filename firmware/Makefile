
include ../common.mk

ARMPATH=~/DevelToolbin/binaries/armThumb-4.4.6/bin/
CC=$(ARMPATH)arm-elf-gcc
OBJCPY=$(ARMPATH)arm-elf-objcopy
OBJDUMP=$(ARMPATH)arm-elf-objdump

LIBADUC = ../aduc/
CCFLAGS=  -mcpu=arm7tdmi -gstabs -g -Wall -Wextra -Wstrict-prototypes -std=gnu99
########################################################################################
# uncomment if you want to have optimized code
# comment if you want to resonably debug the code
CCFLAGS += -Os
########################################################################################
ASMFLAGS= -mcpu=arm7tdmi -I. -x assembler-with-cpp -gstabs -Wall -Wextra
CINCS = -I../include -I$(LIBADUC)
LDFLAGS_COMMON = -mcpu=arm7tdmi -std=gnu99 -Os -I. -nostartfiles

OBJ_LIBADUC = $(LIBADUC)flash.o  $(LIBADUC)init.o  $(LIBADUC)int.o  $(LIBADUC)target_init.o
OBJ_COMMON = main.o checksum.o uart-comm.o frame-comm.o packet-comm.o software-version.o switch.o

########################################################################################

OBJ_ADC_INT_MCA = $(OBJ_LIBADUC) $(OBJ_COMMON) perso-adc-int-mca-ext-trig.o timer1-countdown-and-stop.o timer1-get-duration.o timer1-init-simple.o
LDFLAGS_ADC_INT_MCA =$(LDFLAGS_COMMON) -T$(LIBADUC)project.ld -Wl,-Map=firmware-adc-int-mca.map,--cref -g

OBJ_ADC_INT_MCA_TIMED = $(OBJ_LIBADUC) $(OBJ_COMMON) perso-adc-int-mca-timed-trig.o timer1-adc-trigger.o
LDFLAGS_ADC_INT_MCA_TIMED =$(LDFLAGS_COMMON) -T$(LIBADUC)project.ld -Wl,-Map=firmware-adc-int-mca-timed.map,--cref -g

OBJ_ADC_INT_TIMED_SAMPLING = $(OBJ_LIBADUC) $(OBJ_COMMON) perso-adc-int-log-timed-trig.o timer1-adc-trigger.o data-table-all-other-memory.o
LDFLAGS_ADC_INT_TIMED_SAMPLING =$(LDFLAGS_COMMON) -T$(LIBADUC)project.ld data-table-all-other-memory.x -Wl,--defsym=MIN_STACK_SIZE=780 -Wl,--defsym=MALLOC_HEAP_SIZE=500 -Wl,-Map=firmware-adc-int-timed-sampling.map,--cref -g

########################################################################################

all: firmware-adc-int-mca firmware-adc-int-mca-timed firmware-adc-int-timed-sampling

firmware-adc-int-mca:	$(OBJ_ADC_INT_MCA)
	$(CC) $(LDFLAGS_ADC_INT_MCA) -o firmware-adc-int-mca.elf $^
	$(OBJCPY) --output-target ihex firmware-adc-int-mca.elf firmware-adc-int-mca.hex
	$(OBJCPY) --output-target binary firmware-adc-int-mca.elf firmware-adc-int-mca.bin
	$(OBJDUMP) -h -S firmware-adc-int-mca.elf > firmware-adc-int-mca.lss

firmware-adc-int-mca-timed:	$(OBJ_ADC_INT_MCA_TIMED)
	$(CC) $(LDFLAGS_ADC_INT_MCA_TIMED) -o firmware-adc-int-mca-timed.elf $^
	$(OBJCPY) --output-target ihex firmware-adc-int-mca-timed.elf firmware-adc-int-mca-timed.hex
	$(OBJCPY) --output-target binary firmware-adc-int-mca-timed.elf firmware-adc-int-mca-timed.bin
	$(OBJDUMP) -h -S firmware-adc-int-mca-timed.elf > firmware-adc-int-mca-timed.lss

firmware-adc-int-timed-sampling:	$(OBJ_ADC_INT_TIMED_SAMPLING)
	$(CC) $(LDFLAGS_ADC_INT_TIMED_SAMPLING) -o firmware-adc-int-timed-sampling.elf $^
	$(OBJCPY) --output-target ihex firmware-adc-int-timed-sampling.elf firmware-adc-int-timed-sampling.hex
	$(OBJCPY) --output-target binary firmware-adc-int-timed-sampling.elf firmware-adc-int-timed-sampling.bin
	$(OBJDUMP) -h -S firmware-adc-int-timed-sampling.elf > firmware-adc-int-timed-sampling.lss

########################################################################################

### Rule for LIB ADUC ###

#%.o : %.c $(HEADERS)
#	$(CC) $(CCFLAGS) $< -std=gnu99 -marm -c -o $@

$(LIBADUC)target_init.o : $(LIBADUC)target_init.S
	$(CC) $(ASMFLAGS) $< -Wa,-adhlns=../aduc/target_init.lst, $(CINCS) -c -o $@

$(LIBADUC)flash.o : $(LIBADUC)flash.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -std=gnu99 -mthumb -mthumb-interwork -c -o $@

$(LIBADUC)int.o : $(LIBADUC)int.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -std=gnu99 -marm -mthumb-interwork -c -o $@

$(LIBADUC)init.o : $(LIBADUC)init.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -std=gnu99 -marm -mthumb-interwork -c -o $@

########################################################################################

### Rule for COMMON OBJECTS ###

switch.o : switch.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -std=gnu99 -marm -mthumb-interwork -c -o $@

software-version.o : software-version.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -std=gnu99 -marm -mthumb-interwork -c -o $@

checksum.o : checksum.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -std=gnu99 -marm -mthumb-interwork -c -o $@

main.o : main.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -std=gnu99 -marm -mthumb-interwork -c -o $@

uart-comm.o : uart-comm.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -std=gnu99 -marm -mthumb-interwork -c -o $@

frame-comm.o : frame-comm.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -std=gnu99 -marm -mthumb-interwork -c -o $@

packet-comm.o : packet-comm.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -std=gnu99 -marm -mthumb-interwork -c -o $@


### Used by more than one personality ###

timer1-adc-trigger.o : timer1-adc-trigger.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -std=gnu99 -marm -mthumb-interwork -c -o $@

########################################################################################

### Rule for OBJ_ADC_INT_MCA ###

timer1-init-simple.o : timer1-init-simple.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -std=gnu99 -marm -mthumb-interwork -c -o $@

perso-adc-int-mca-ext-trig.o : perso-adc-int-mca-ext-trig.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -std=gnu99 -marm -mthumb-interwork -c -o $@

timer1-countdown-and-stop.o : timer1-countdown-and-stop.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -std=gnu99 -marm -mthumb-interwork -c -o $@

timer1-get-duration.o : timer1-get-duration.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -std=gnu99 -marm -mthumb-interwork -c -o $@

########################################################################################

### Rule for OBJ_ADC_INT_MCA_TIMED ###

perso-adc-int-mca-timed-trig.o : perso-adc-int-mca-timed-trig.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -std=gnu99 -marm -mthumb-interwork -c -o $@

########################################################################################

### Rule for OBJ_ADC_INT_TIMED_SAMPLING ###

perso-adc-int-log-timed-trig.o : perso-adc-int-log-timed-trig.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -std=gnu99 -marm -mthumb-interwork -c -o $@

data-table-all-other-memory.o : data-table-all-other-memory.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -std=gnu99 -marm -mthumb-interwork -c -o $@

########################################################################################

clean:
	$(RM) *.o *.bin *.map *.hex *.elf *.lss
	$(RM) $(LIBADUC)*.o $(LIBADUC)*.lst
	$(RM) main
	$(RM) -r ./dox/

dox:
	doxygen Doxyfile

