#
# ddd --debugger arm-none-eabi-gdb ./foo.elf
#
target remote localhost:3333
# we have no possibility to perform a system reset since the HW
# doesn support it. the only thing we can do is a soft_reset
# and seek to the PC to the entry point
monitor soft_reset_halt
info register
break main
info break
# avoid problem with DDD's VSL interpreter
set print elements 3


