######################################################################
# freemcan gdb init file customized for ADuC7026
#
# Copyright (C) 2012 samplemaker
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public License
#  as published by the Free Software Foundation; either version 2.1
#  of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free
#  Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
#  Boston, MA 02110-1301 USA
#
#
#
# you may start either ddd or gdb via:
#
# arm-none-eabi-gdb ./foo.elf
# ddd --debugger arm-none-eabi-gdb ./foo.elf
#
#
######################################################################


# customized functions

define cpu_status

  printf "\n"
  printf "CPU\n"
  printf "---\n"
  printf "Processor state register     : 0x%x \n", $cpsr

  if ($cpsr & 0x80)
    printf "Interrupts                   : DISABLED \n"
  else
    printf "Interrupts                   : ENABLED \n"
  end

  if ($cpsr & 0x20)
    printf "Processor state is           : THUMB \n"
  else
    printf "Processor state is           : ARM \n"
  end

  printf "The mode set is              : "
  if (($cpsr & 0x1F) == 0x10)
    printf "User \n"
  end
  if (($cpsr & 0x1F) == 0x11)
    printf "Fast Interrupt \n"
  end
  if (($cpsr & 0x1F) == 0x12)
    printf "Interrupt \n"
  end
  if (($cpsr & 0x1F) == 0x13)
    printf "Supervisor \n"
  end
  if (($cpsr & 0x1F) == 0x17)
    printf "Abort \n"
  end
  if (($cpsr & 0x1F) == 0x1B)
    printf "Undefined \n"
  end
  if (($cpsr & 0x1F) == 0x1F)
    printf "System \n"
  end


end
document cpu_status
  shows some informations about the target
end


define mmr_status

  set $MMR_BASE = 0xFFFF0000
  set $MMR_IRQSTA = $MMR_BASE + 0
  set $MMR_IRQSIG = $MMR_BASE + 4
  set $MMR_IRQEN = $MMR_BASE + 8
  set $MMR_PLLCON = $MMR_BASE + 0x0414
  set $MMR_POWCON = $MMR_BASE + 0x0408
  set $MMR_ADCCON = $MMR_BASE + 0x0500

  printf "\n"
  printf "MMR's\n"
  printf "-----\n"


  set $PLLCON = (*(uint8_t *)($MMR_PLLCON))
  printf "PLLCON                       : 0x%02x \n",$PLLCON
  #check the OSEL bit in the PLLCON MMR
  if ($PLLCON & 0x20)
    printf "Clock source                 : Internal resonator \n"
  else
    printf "Clock source                 : External crystal \n"
  end
  set $POWCON = (*(uint8_t *)($MMR_POWCON))
  printf "POWCON                       : 0x%02x \n",$POWCON
  printf "Core clock                   : %0.2f MHz \n", 41.78/(1 << ($POWCON & 0x7))
  printf "IRQEN  (masked)              : 0x%08x \n",*(uint32_t *)($MMR_IRQEN)
  printf "IRQSTA (enabled and pending) : 0x%08x \n",*(uint32_t *)($MMR_IRQSTA)
  printf "IRQSIG (pending)             : 0x%08x \n",*(uint32_t *)($MMR_IRQSIG)
  set $ADCCON = (*(uint16_t *)($MMR_ADCCON))
  printf "ADCCON                       : 0x%04x \n",$ADCCON
  printf "ADC clock divider            : %01d \n",(1 << (($ADCCON & 0x1c00) >> 10))
  printf "\n"

end
document mmr_status
  shows some informations about the target
end


######################################################################


# connect to OPENOCD

target remote localhost:3333

# avoid problem with DDD's VSL interpreter

set print elements 3

# we have no possibility to perform a system reset because the HW
# doesnt support a physical reset pin. the only thing we can do is
# to perform a soft_reset and seek the PC to the entry point at
# 0x0 (flash mirrored in ram)

monitor soft_reset_halt

break main
# break do_initcalls
cont
# info break

info register

cpu_status
mmr_status

