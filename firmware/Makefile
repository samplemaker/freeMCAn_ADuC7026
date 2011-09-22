ARMPATH=~/DevelToolbin/binaries/armThumb-4.4.6/bin/
CC=$(ARMPATH)arm-elf-gcc
OBJCPY=$(ARMPATH)arm-elf-objcopy
OBJDUMP=$(ARMPATH)arm-elf-objdump

LIBADUC = ../aduc/
CCFLAGS=  -mcpu=arm7tdmi -gstabs -Os -g -Wall -Wextra -Wstrict-prototypes -std=gnu99
ASMFLAGS= -mcpu=arm7tdmi -I. -x assembler-with-cpp -gstabs -Wall -Wextra
CINCS = -I../include -I$(LIBADUC)
LDFLAGS = -mcpu=arm7tdmi -std=gnu99 -Os -I. -T$(LIBADUC)project.ld -nostartfiles -Wl,-Map=main.map,--cref -g

OBJ_LIBADUC = $(LIBADUC)flash.o  $(LIBADUC)init.o  $(LIBADUC)int.o  $(LIBADUC)target_init.o
OBJ_COMMON = main.o checksum.o uart-comm.o frame-comm.o packet-comm.o software-version.o switch.o

########################################################################################

OBJ_ADC_INT_MCA = $(OBJ_LIBADUC) $(OBJ_COMMON) perso-adc-int-mca-ext-trig.o timer1-countdown-and-stop.o timer1-get-duration.o timer1-init-simple.o  

OBJ_ADC_INT_MCA_TIMED = $(OBJ_ADC_INT_MCA) $(OBJ_COMMON) perso-adc-int-mca-timed-trig.o timer1-adc-trigger.o

OBJ_ADC_INT_TIMED_SAMPLING = $(OBJ_ADC_INT_MCA) $(OBJ_COMMON) perso-adc-int-log-timed-trig.o timer1-adc-trigger.o data-table-all-other-memory.o

########################################################################################

all: main

main:	$(OBJ_ADC_INT_MCA)
	$(CC) $(LDFLAGS) -o main.elf $^
	$(OBJCPY) --output-target ihex main.elf main.hex
	$(OBJCPY) --output-target binary main.elf main.bin
	$(OBJDUMP) -h -S main.elf > main.lss

firmware-adc-int-mca:	$(OBJ_ADC_INT_MCA)
	$(CC) $(LDFLAGS) -o firmware-adc-int-mca.elf $^
	$(OBJCPY) --output-target ihex firmware-adc-int-mca.elf firmware-adc-int-mca.hex
	$(OBJCPY) --output-target binary firmware-adc-int-mca.elf firmware-adc-int-mca.bin
	$(OBJDUMP) -h -S firmware-adc-int-mca.elf > firmware-adc-int-mca.lss

firmware-adc-int-mca-timed:	$(OBJ_ADC_INT_MCA_TIMED)
	$(CC) $(LDFLAGS) -o firmware-adc-int-mca-timed.elf $^
	$(OBJCPY) --output-target ihex firmware-adc-int-mca-timed.elf firmware-adc-int-mca-timed.hex
	$(OBJCPY) --output-target binary firmware-adc-int-mca-timed.elf firmware-adc-int-mca-timed.bin
	$(OBJDUMP) -h -S firmware-adc-int-mca-timed.elf > firmware-adc-int-mca-timed.lss

firmware-adc-int-timed-sampling:	$(OBJ_ADC_INT_TIMED_SAMPLING)
	$(CC) $(LDFLAGS) -o firmware-adc-int-timed-sampling.elf $^
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

timer1-adc-trigger.o : timer1-adc-trigger.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -std=gnu99 -marm -mthumb-interwork -c -o $@

perso-adc-int-mca-timed-trig.o : perso-adc-int-mca-timed-trig.c $(HEADERS)
	$(CC) $(CCFLAGS) $(CINCS) $< -std=gnu99 -marm -mthumb-interwork -c -o $@

########################################################################################


clean:
	$(RM) *.o *.bin *.map *.hex *.elf *.lss
	$(RM) $(LIBADUC)*.o 
	$(RM) main
	$(RM) -r ./dox/

dox:
	doxygen Doxyfile

