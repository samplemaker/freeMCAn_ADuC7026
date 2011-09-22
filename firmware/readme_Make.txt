rm *.o
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -c -mcpu=arm7tdmi -I.  -gstabs  -I../include -I../aduc -Wall -Wextra -Wstrict-prototypes -std=gnu99  uart-comm.c -o uart-comm.o
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -c -mcpu=arm7tdmi -I.  -gstabs  -I../include -I../aduc -Wall -Wextra -Wstrict-prototypes -std=gnu99  switch.c -o switch.o
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -c -mcpu=arm7tdmi -I.  -gstabs  -I../include -I../aduc  -Wall -Wextra -Wstrict-prototypes -std=gnu99  perso-adc-int-mca-ext-trig.c -o perso-adc-int-mca-ext-trig.o
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -c -mcpu=arm7tdmi -I.  -gstabs  -I../include -I../aduc  -Wall -Wextra -Wstrict-prototypes -std=gnu99  packet-comm.c -o packet-comm.o
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -c -mcpu=arm7tdmi -I.  -gstabs  -I../include -I../aduc  -Wall -Wextra -Wstrict-prototypes -std=gnu99  timer1-init-simple.c -o timer1-init-simple.o
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -c -mcpu=arm7tdmi -I.  -gstabs  -I../include -I../aduc  -Wall -Wextra -Wstrict-prototypes -std=gnu99  checksum.c -o checksum.o
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -c -mcpu=arm7tdmi -I.  -gstabs  -I../include -I../aduc  -Wall -Wextra -Wstrict-prototypes -std=gnu99  frame-comm.c -o frame-comm.o
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -c -mcpu=arm7tdmi -I.  -gstabs  -I../include -I../aduc  -Wall -Wextra -Wstrict-prototypes -std=gnu99  software-version.c -o software-version.o
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -c -mcpu=arm7tdmi -I.  -gstabs  -I../include -I../aduc  -Wall -Wextra -Wstrict-prototypes -std=gnu99  timer1-countdown-and-stop.c -o timer1-countdown-and-stop.o
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -c -mcpu=arm7tdmi -I.  -gstabs  -I../include -I../aduc  -Wall -Wextra -Wstrict-prototypes -std=gnu99  timer1-get-duration.c -o timer1-get-duration.o
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -c -mcpu=arm7tdmi -I.  -gstabs  -I../include -I../aduc  -Wall -Wextra -Wstrict-prototypes -std=gnu99  main.c -o main.o
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -c -mcpu=arm7tdmi -I. -Wa,-adhlns=../aduc/target_init.lst,-gstabs -Wall -Wextra -I../include -I../aduc ../aduc/target_init.S -o ../aduc/target_init.o
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -c -mcpu=arm7tdmi -I.  -gstabs  -I../include -I../aduc -Os -Wall -Wextra -Wstrict-prototypes -std=gnu99  ../aduc/int.c -o ../aduc/int.o
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -c -mcpu=arm7tdmi -I.  -gstabs  -I../include -I../aduc -Os -Wall -Wextra -Wstrict-prototypes -std=gnu99  ../aduc/init.c -o ../aduc/init.o
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -c -mcpu=arm7tdmi -I.  -gstabs  -I../include -I../aduc -Os -Wall -Wextra -Wstrict-prototypes -std=gnu99  ../aduc/flash.c -o ../aduc/flash.o

~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -mcpu=arm7tdmi -I.  -gstabs  -I../include -I../aduc -Os -Wall -Wextra -Wstrict-prototypes -std=gnu99 checksum.o  frame-comm.o  main.o  packet-comm.o  perso-adc-int-mca-ext-trig.o  switch.o  timer1-countdown-and-stop.o  timer1-get-duration.o  timer1-init-simple.o  uart-comm.o ../aduc/target_init.o ../aduc/int.o ../aduc/flash.o ../aduc/init.o check-data-stack-in-SRAM.x --output firmware.elf  -Wl,-Map=firmware.map,--cref -Wl,--defsym=RAM_END=0x0011FFF -Wl,--defsym=MIN_STACK_SIZE=256 -Wl,--defsym=MALLOC_HEAP_SIZE=64   -lm -T../aduc/project.ld -nostartfiles
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-objcopy --output-target ihex firmware.elf firmware.hex
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-objcopy --output-target binary firmware.elf firmware.bin


############################################################################################################################################

~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc  -gstabs  -I../include -I../aduc -Os -Wall -Wextra -Wstrict-prototypes -std=gnu99    -c -mcpu=arm7tdmi -I.  -gstabs  -I../include -I../aduc -Os -Wall -Wextra -Wstrict-prototypes -std=gnu99  -M main.c uart-comm.c frame-comm.c packet-comm.c firmware-version.c ../aduc/target_init.S >> .depend.mk
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -c -mcpu=arm7tdmi -I.  -gstabs  -I../include -I../aduc -Os -Wall -Wextra -Wstrict-prototypes -std=gnu99  main.c -o main.o
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -c -mcpu=arm7tdmi -I.  -gstabs  -I../include -I../aduc -Os -Wall -Wextra -Wstrict-prototypes -std=gnu99  frame-comm.c -o frame-comm.o
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -c -mcpu=arm7tdmi -I.  -gstabs  -I../include -I../aduc -Os -Wall -Wextra -Wstrict-prototypes -std=gnu99  packet-comm.c -o packet-comm.o
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -c -mcpu=arm7tdmi -I.  -gstabs  -I../include -I../aduc -Os -Wall -Wextra -Wstrict-prototypes -std=gnu99  firmware-version.c -o firmware-version.o
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -c -mcpu=arm7tdmi -I. -x assembler-with-cpp  -Wa,-adhlns=../aduc/target_init.lst,-gstabs -Wall -Wextra -I../include -I../aduc ../aduc/target_init.S -o ../aduc/target_init.o
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-gcc -mcpu=arm7tdmi -I.  -gstabs  -I../include -I../aduc -Os -Wall -Wextra -Wstrict-prototypes -std=gnu99  main.o uart-comm.o frame-comm.o packet-comm.o firmware-version.o ../aduc/target_init.o  --output firmware.elf  -Wl,-Map=firmware.map,--cref   -lm -T../aduc/project.ld -nostartfiles
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-objcopy --output-target ihex firmware.elf firmware.hex
~/DevelToolbin/binaries/armThumb-4.4.6/bin//arm-elf-objcopy --output-target binary firmware.elf firmware.bin

############################################################################################################################################

perso-adc-int-mca-ext-trig.c: In function 'personality_start_measurement_sram':
perso-adc-int-mca-ext-trig.c:263: warning: dereferencing pointer 'timer1_value' does break strict-aliasing rules
perso-adc-int-mca-ext-trig.c:261: note: initialized from here


avr-gcc  -Os -gstabs -std=gnu99 -Wstrict-prototypes -fshort-enums -fno-common -DF_CPU=18432000UL -I../include -Wall -Wextra -mmcu=atmega644 -I. -Wl,-Map=firmware-adc-int-mca.map,--cref -Wl,--defsym=RAM_END=0x00801100 -Wl,--defsym=MIN_STACK_SIZE=256 -Wl,--defsym=MALLOC_HEAP_SIZE=64  .objs/perso-adc-int-mca-ext-trig.o .objs/main.o .objs/checksum.o .objs/uart-comm.o .objs/frame-comm.o .objs/packet-comm.o .objs/wdt-softreset.o .objs/software-version.o .objs/switch.o check-data-stack-in-SRAM.x .objs/timer1-init-simple.o .objs/timer1-countdown-and-stop.o .objs/timer1-get-duration.o -lm --output firmware-adc-int-mca.elf



avr-gcc  -Os -gstabs -std=gnu99 -Wstrict-prototypes -fshort-enums -fno-common -DF_CPU=18432000UL -I../include -Wall -Wextra -mmcu=atmega644 -I.  -c main.c -o .objs/main.o
avr-gcc  -Os -gstabs -std=gnu99 -Wstrict-prototypes -fshort-enums -fno-common -DF_CPU=18432000UL -I../include -Wall -Wextra -mmcu=atmega644 -I.  -c timer1-get-duration.c -o .objs/timer1-get-duration.o
avr-gcc  -Os -gstabs -std=gnu99 -Wstrict-prototypes -fshort-enums -fno-common -DF_CPU=18432000UL -I../include -Wall -Wextra -mmcu=atmega644 -I.  -c timer1-countdown-and-stop.c -o .objs/timer1-countdown-and-stop.o
avr-gcc  -Os -gstabs -std=gnu99 -Wstrict-prototypes -fshort-enums -fno-common -DF_CPU=18432000UL -I../include -Wall -Wextra -mmcu=atmega644 -I.  -c software-version.c -o .objs/software-version.o
avr-gcc  -Os -gstabs -std=gnu99 -Wstrict-prototypes -fshort-enums -fno-common -DF_CPU=18432000UL -I../include -Wall -Wextra -mmcu=atmega644 -I.  -c wdt-softreset.c -o .objs/wdt-softreset.o
avr-gcc  -Os -gstabs -std=gnu99 -Wstrict-prototypes -fshort-enums -fno-common -DF_CPU=18432000UL -I../include -Wall -Wextra -mmcu=atmega644 -I.  -c frame-comm.c -o .objs/frame-comm.o
avr-gcc  -Os -gstabs -std=gnu99 -Wstrict-prototypes -fshort-enums -fno-common -DF_CPU=18432000UL -I../include -Wall -Wextra -mmcu=atmega644 -I.  -c checksum.c -o .objs/checksum.o
avr-gcc  -Os -gstabs -std=gnu99 -Wstrict-prototypes -fshort-enums -fno-common -DF_CPU=18432000UL -I../include -Wall -Wextra -mmcu=atmega644 -I.  -c timer1-init-simple.c -o .objs/timer1-init-simple.o
avr-gcc  -Os -gstabs -std=gnu99 -Wstrict-prototypes -fshort-enums -fno-common -DF_CPU=18432000UL -I../include -Wall -Wextra -mmcu=atmega644 -I.  -c perso-adc-int-mca-ext-trig.c -o .objs/perso-adc-int-mca-ext-trig.o
avr-gcc  -Os -gstabs -std=gnu99 -Wstrict-prototypes -fshort-enums -fno-common -DF_CPU=18432000UL -I../include -Wall -Wextra -mmcu=atmega644 -I.  -c uart-comm.c -o .objs/uart-comm.o
avr-gcc  -Os -gstabs -std=gnu99 -Wstrict-prototypes -fshort-enums -fno-common -DF_CPU=18432000UL -I../include -Wall -Wextra -mmcu=atmega644 -I.  -c switch.c -o .objs/switch.o
avr-gcc  -Os -gstabs -std=gnu99 -Wstrict-prototypes -fshort-enums -fno-common -DF_CPU=18432000UL -I../include -Wall -Wextra -mmcu=atmega644 -I.  -c packet-comm.c -o .objs/packet-comm.o
