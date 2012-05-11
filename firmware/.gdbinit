# ddd --debugger arm-elf-gdb ./foo.elf
target remote localhost:3333
monitor soft_reset_halt
info register
break main
info break

