#!/bin/bash
set -x
function usage {
    echo "Usage: ./flash.sh [OPTIONS] [FILE]
   Tool for flashing avr board on raspberrypi
   Options:
      -p   GPIO pin for reset
      -d   Programmer ttyUSB device
      -t   Target mcu type (168 or 328p)
   File:
      .hex file with compiled firmware.
    "
}


RESET_PIN=18
SERIAL_DEVICE=ttyUSB0
TARGET=168
while getopts "hp:d:t:" opt
do
    case $opt in
        p)
            RESET_PIN=$OPTARG
        ;;
        d)
            SERIAL_DEVICE=$OPTARG
        ;;
        t)
            TARGET=$OPTARG
        ;;
        h)
            usage
            exit 0
        ;;
    esac
done
shift $(($OPTIND - 1))
FW=$1

test -f $1 || {
    echo "File $1 doesnt exists"
    exit 1
}

test -c /dev/$SERIAL_DEVICE || {
    echo "Serial device /dev/$SERIAL_DEVICE doesnt exists"
    exit 1
}

# test -e /sys/class/gpio/gpio$RESET_PIN || {
#    echo "Setting up gpio"
#    test `id -u` == 0 || {
#       echo "You must be root to setup gpio"
#       exit 1
#    }
#    echo $RESET_PIN > /sys/class/gpio/export || exit 1
#    echo out > /sys/class/gpio/gpio$RESET_PIN/direction || exit 1
# }


avr-size -C -x ${FW/.hex/}
# echo 0 > /sys/class/gpio/gpio$RESET_PIN/value || exit 1
# bash -c "sleep .5; echo 1 > /sys/class/gpio/gpio$RESET_PIN/value" &
## avrdude -c avrisp -p atmega168 -b 19200 -P /dev/$SERIAL_DEVICE -V -U flash:w:$FW
## avrdude -c avrisp -p atmega328p -b 57600 -P /dev/$SERIAL_DEVICE -V -U flash:w:$FW

if [ "$TARGET" = "168" ]; then
    BAUD=19200
else
    BAUD=57600
fi

avrdude -c avrisp -p atmega$TARGET -b $BAUD -P /dev/$SERIAL_DEVICE -V -U flash:w:$FW
test $? -ne 0  &&  {
    echo "Flash failed"
    exit 1
}
stty -F /dev/$SERIAL_DEVICE 115200
cat /dev/$SERIAL_DEVICE
