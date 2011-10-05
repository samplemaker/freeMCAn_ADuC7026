#!/bin/bash

if [ $# -ne 1 ]
then
  echo 'program binary file with OPENOCD. wrong number arguments:'
  echo './program.sh [binary file]'
  exit 1
fi

xterm -e "openocd -f ./openocd-usb.cfg -f ./openocd-target.cfg" &
sleep 3

VAR=$(expect -c "
spawn telnet localhost 4444
expect \"> \"
send \"soft_reset_halt\r\"
expect \"> \"
send \"flash_load $1\r\"
expect \"> \"
send \"exit\"
")

killall openocd
