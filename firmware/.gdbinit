
# arm-none-eabi-gdb ./foo.elf
# ddd --debugger arm-none-eabi-gdb ./foo.elf

target remote localhost:3333

# avoid problem with DDD's VSL interpreter
set print elements 3

# we have no possibility to perform a system reset because the HW
# doesnt support a physical reset pin. the only thing we can do is
# to perform a soft_reset and seek the PC to the entry point at
# 0x0 (flash mirrored in ram)
monitor soft_reset_halt

# break main
break do_initcalls
cont

info register
info break


