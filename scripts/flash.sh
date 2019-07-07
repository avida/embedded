#!/bin/bash
RESET_PIN=18
SERIAL_DEVICE=ttyUSB0
test -e /sys/class/gpio/gpio$RESET_PIN || {
   echo "Setting up gpio"
   test `id -u` == 0 || {
      echo "You must be root to setup gpio"
      exit 1
   }
   echo $RESET_PIN > /sys/class/gpio/export
   echo out > /sys/class/gpio/gpio$RESET_PIN/direction
}

echo 0 > /sys/class/gpio/gpio$RESET_PIN/value
bash -c "sleep .5; echo 1 > /sys/class/gpio/gpio$RESET_PIN/value" &
avrdude -c avrisp -p atmega168 -b 19200  -P /dev/$SERIAL_DEVICE -U flash:w:./fw.elf
stty -F /dev/$SERIAL_DEVICE 115200
cat /dev/$SERIAL_DEVICE
