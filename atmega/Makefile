CC = avr-gcc
AVR-OC = avr-objcopy
F_CPU = 16000000
CFLAGS = -D__AVR_ATmega328P__  -DF_CPU=$(F_CPU)UL -mmcu=atmega328
#CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums 
#CFLAGS += -Wall -Wstrict-prototypes
#CFLAGS += -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--relax
#CFLAGS += -std=gnu99
SOURCES = main.c
OBJECTS = main.o

main: main.hex
	echo END
main.hex: main.elf
	$(AVR-OC) -j .text -j .data  -O ihex $< $@
#-R .eeprom

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

main.elf: $(OBJECTS)
	$(CC) -mmcu=avr4  $(OBJECTS) -o $@

flash:  main.hex
	avrdude -F -c avrisp -p atmega328p -P COM3 -b 57600 -U flash:w:$<

 clean: 
	rm *.hex *elf *o
	rm rc_decoder/*o
	