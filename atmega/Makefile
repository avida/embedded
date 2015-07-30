CC = avr-g++
AVR-OC = avr-objcopy
F_CPU = 16000000

SOURCES = main.c rc_decoder/rc_decoder.c uart.c gpio.c
DIRS = rc_decoder
#Compiler flags
CFLAGS = -D__AVR_ATmega328P__  -DF_CPU=$(F_CPU)UL -mmcu=atmega328
CFLAGS += -funsigned-char -funsigned-bitfields -fshort-enums -Os
CFLAGS += -Wall -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--relax
CFLAGS += -std=c++11

OBJDIR = obj
OBJECTS = $(patsubst %.c, $(OBJDIR)/%.o, $(SOURCES))
OBJ_SUBDIRS = $(patsubst %, $(OBJDIR)/%, $(DIRS))

all:
	mkdir -p $(OBJ_SUBDIRS)
	mingw32-make flash

flash: $(OBJDIR)/main.hex
	avrdude -F -c avrisp -p atmega328p -P COM3 -b 57600 -U flash:w:$<

$(OBJDIR)/main.hex: $(OBJDIR)/main.elf
	$(AVR-OC) -j .text -j .data  -O ihex $< $@
#-R .eeprom

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

$(OBJDIR)/main.elf: $(OBJECTS)
	$(CC) -mmcu=atmega328  $(OBJECTS) -o $@

clean: 
	rm -rf $(OBJDIR)/*
