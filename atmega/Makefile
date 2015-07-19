CC = avr-gcc
AVR-OC = avr-objcopy
F_CPU = 16000000
CFLAGS = -D__AVR_ATmega328P__  -DF_CPU=$(F_CPU)UL -mmcu=atmega328
CFLAGS += -funsigned-char -funsigned-bitfields -fshort-enums -O3
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--relax
CFLAGS += -std=gnu99
SOURCES = main.c rc_decoder/rc_decoder.c uart.c
OBJECTS = main.o rc_decoder/rc_decoder.o uart.o

#override CFLAGS = -g -DF_CPU=$(F_CPU) -Wall -O2 -mmcu=atmega328 -std=gnu99$
#override LDFLAGS= -Wl,-u,vfprintf,-Map,main.map

flash:  main.hex
	avrdude -F -c avrisp -p atmega328p -P COM3 -b 57600 -U flash:w:$<

main.hex: main.elf
	$(AVR-OC) -j .text -j .data  -O ihex $< $@
#-R .eeprom

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

main.elf: $(OBJECTS)
	$(CC) -mmcu=atmega328  $(OBJECTS) -o $@

clean: 
	rm *.hex *elf *o
	rm rc_decoder/*o
	