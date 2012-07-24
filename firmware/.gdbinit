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
  printf "Status register:     0x%x \n", $cpsr

  if ($cpsr & 0x80)
    printf "Interrupts:          DISABLED \n"
  else
    printf "Interrupts:          ENABLED \n"
  end

  if ($cpsr & 0x20)
    printf "Processor state:     THUMB \n"
  else
    printf "Processor state:     ARM \n"
  end

  printf "Mode:                "
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
  set $MMR_PLLCON = $MMR_BASE + 0x0414
  set $MMR_POWCON = $MMR_BASE + 0x0408
  set $MMR_ADCCON = $MMR_BASE + 0x0500
  set $MMR_T0CON = $MMR_BASE + 0x0308
  set $MMR_T1CON = $MMR_BASE + 0x0328
  set $MMR_T2CON = $MMR_BASE + 0x0348
  set $MMR_T3CON = $MMR_BASE + 0x0368
  set $MMR_GP0CON = $MMR_BASE + 0xF400
  set $MMR_GP1CON = $MMR_BASE + 0xF404
  set $MMR_GP2CON = $MMR_BASE + 0xF408
  set $MMR_GP3CON = $MMR_BASE + 0xF40C
  set $MMR_GP4CON = $MMR_BASE + 0xF410

  printf "\n"
  printf "MMR's\n"
  printf "-----\n"

  set $PLLCON = (*(uint8_t *)($MMR_PLLCON))
  printf "PLLCON               0x%02x",$PLLCON
  #check the OSEL bit in the PLLCON MMR
  if ($PLLCON != 0)
    if ($PLLCON & 0x20)
      printf "        [Internal resonator] \n"
    else
      printf "        [External crystal] \n"
    end
  else
      printf " ?\n"
  end
  set $POWCON = (*(uint8_t *)($MMR_POWCON))
  printf "POWCON               0x%02x        [f_hclk=%0.2f MHz]\n",$POWCON, 41.78/(1 << ($POWCON & 0x7))
  set $ADCCON = (*(uint16_t *)($MMR_ADCCON))
  printf "ADCCON               0x%04x      [divider=%01d;",$ADCCON,(1 << (($ADCCON & 0x1c00) >> 10))
  if ($ADCCON & 0x80)
    printf " online] \n"
  else
    printf " offline] \n"
  end
  printf "T0CON                0x%04x \n",*(uint16_t *)($MMR_T0CON)
  printf "T1CON                0x%08x \n",*(uint32_t *)($MMR_T1CON)
  printf "T2CON                0x%04x \n",*(uint16_t *)($MMR_T2CON)
  printf "T3CON                0x%04x \n",*(uint16_t *)($MMR_T3CON)
  printf "GP0CON               0x%08x \n",*(uint32_t *)($MMR_GP0CON)
  printf "GP1CON               0x%08x \n",*(uint32_t *)($MMR_GP1CON)
  printf "GP2CON               0x%08x \n",*(uint32_t *)($MMR_GP2CON)
  printf "GP3CON               0x%08x \n",*(uint32_t *)($MMR_GP3CON)
  printf "GP4CON               0x%08x \n",*(uint32_t *)($MMR_GP4CON)


end
document mmr_status
  shows some informations about the target
end


define int_status

  set $MMR_BASE = 0xFFFF0000
  set $MMR_IRQSTA = $MMR_BASE + 0
  set $MMR_IRQSIG = $MMR_BASE + 4
  set $MMR_IRQEN = $MMR_BASE + 8

  printf "\n"
  printf "Interrupts\n"
  printf "----------\n"
  set $IRQEN = *(uint32_t *)($MMR_IRQEN)
  set $IRQSTA = *(uint32_t *)($MMR_IRQSTA)
  set $IRQSIG = *(uint32_t *)($MMR_IRQSIG)
  printf "IRQEN                0x%08x  (masked)\n",$IRQEN
  printf "IRQSTA               0x%08x  (enabled and pending)\n",$IRQSTA
  printf "IRQSIG               0x%08x  (pending)\n",$IRQSIG

  printf "INT_SWI              ENA %01x STA %01x SIG %01x\n", (($IRQEN >>1) & 1), (($IRQSTA >>1) & 1), (($IRQSIG >>1) & 1)
  printf "INT_TIMER0           ENA %01x STA %01x SIG %01x\n", (($IRQEN >>2) & 1), (($IRQSTA >>2) & 1), (($IRQSIG >>2) & 1)
  printf "INT_TIMER1           ENA %01x STA %01x SIG %01x\n", (($IRQEN >>3) & 1), (($IRQSTA >>3) & 1), (($IRQSIG >>3) & 1)
  printf "INT_WAKEUP_TIMER2    ENA %01x STA %01x SIG %01x\n", (($IRQEN >>4) & 1), (($IRQSTA >>4) & 1), (($IRQSIG >>4) & 1)
  printf "INT_WATCHDOG_TIMER3  ENA %01x STA %01x SIG %01x\n", (($IRQEN >>5) & 1), (($IRQSTA >>5) & 1), (($IRQSIG >>5) & 1)
  printf "INT_FLASH_CONTROL    ENA %01x STA %01x SIG %01x\n", (($IRQEN >>6) & 1), (($IRQSTA >>6) & 1), (($IRQSIG >>6) & 1)
  printf "INT_ADC_CHANNEL      ENA %01x STA %01x SIG %01x\n", (($IRQEN >>7) & 1), (($IRQSTA >>7) & 1), (($IRQSIG >>7) & 1)
  printf "INT_PLL_LOCK         ENA %01x STA %01x SIG %01x\n", (($IRQEN >>8) & 1), (($IRQSTA >>8) & 1), (($IRQSIG >>8) & 1)
  printf "INT_I2C0_SLAVE       ENA %01x STA %01x SIG %01x\n", (($IRQEN >>9) & 1), (($IRQSTA >>9) & 1), (($IRQSIG >>9) & 1)
  printf "INT_I2C0_MASTER      ENA %01x STA %01x SIG %01x\n", (($IRQEN >>10) & 1), (($IRQSTA >>10) & 1), (($IRQSIG >>10) & 1)
  printf "INT_I2C1_MASTER      ENA %01x STA %01x SIG %01x\n", (($IRQEN >>11) & 1), (($IRQSTA >>11) & 1), (($IRQSIG >>11) & 1)
  printf "INT_SPI_SLAVE        ENA %01x STA %01x SIG %01x\n", (($IRQEN >>12) & 1), (($IRQSTA >>12) & 1), (($IRQSIG >>12) & 1)
  printf "INT_SPI_MASTER       ENA %01x STA %01x SIG %01x\n", (($IRQEN >>13) & 1), (($IRQSTA >>13) & 1), (($IRQSIG >>13) & 1)
  printf "INT_UART             ENA %01x STA %01x SIG %01x\n", (($IRQEN >>14) & 1), (($IRQSTA >>14) & 1), (($IRQSIG >>14) & 1)
  printf "INT_EXTERNAL_IRQ0    ENA %01x STA %01x SIG %01x\n", (($IRQEN >>15) & 1), (($IRQSTA >>15) & 1), (($IRQSIG >>15) & 1)
  printf "INT_COMPARATOR       ENA %01x STA %01x SIG %01x\n", (($IRQEN >>16) & 1), (($IRQSTA >>16) & 1), (($IRQSIG >>16) & 1)
  printf "INT_PSM              ENA %01x STA %01x SIG %01x\n", (($IRQEN >>17) & 1), (($IRQSTA >>17) & 1), (($IRQSIG >>17) & 1)
  printf "INT_EXTERNAL_IRQ1    ENA %01x STA %01x SIG %01x\n", (($IRQEN >>18) & 1), (($IRQSTA >>18) & 1), (($IRQSIG >>18) & 1)
  printf "INT_PLA_IRQ0         ENA %01x STA %01x SIG %01x\n", (($IRQEN >>19) & 1), (($IRQSTA >>19) & 1), (($IRQSIG >>19) & 1)
  printf "INT_PLA_IRQ1         ENA %01x STA %01x SIG %01x\n", (($IRQEN >>20) & 1), (($IRQSTA >>20) & 1), (($IRQSIG >>20) & 1)
  printf "INT_EXTERNAL_IRQ2    ENA %01x STA %01x SIG %01x\n", (($IRQEN >>21) & 1), (($IRQSTA >>21) & 1), (($IRQSIG >>21) & 1)
  printf "INT_EXTERNAL_IRQ3    ENA %01x STA %01x SIG %01x\n", (($IRQEN >>22) & 1), (($IRQSTA >>22) & 1), (($IRQSIG >>22) & 1)
  printf "INT_PWM_TRIP         ENA %01x STA %01x SIG %01x\n", (($IRQEN >>23) & 1), (($IRQSTA >>23) & 1), (($IRQSIG >>23) & 1)
  printf "\n"

end
document int_status
  shows some informations about the target
end


define status
  cpu_status
  mmr_status
  int_status
end
document status
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

status

