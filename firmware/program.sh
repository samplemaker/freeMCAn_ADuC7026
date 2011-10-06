#!/bin/bash
#
#  program.sh [firmware]
#
#  Program firmware with OPENOCD
#
#
#  Copyright (C) 2011 samplemaker
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


if [ $# -ne 1 ]
then
  echo 'program binary file with OPENOCD. wrong number arguments:'
  echo './program.sh [binary file]'
  exit 1
fi

echo 'start openocd in terminal and wait ... '
xterm -e "openocd -f ./openocd-usb.cfg -f ./openocd-target.cfg" &
sleep 3

echo 'start telnet and programming ... '
var=$(expect -c "
spawn telnet localhost 4444
expect \"> \"
send \"soft_reset_halt\r\"
expect \"> \"
send \"flash_load $1\r\"
expect \"> \"
send \"exit\"
")

echo 'killall openocd ... '
killall openocd
