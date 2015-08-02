CC = avr-g++
AVR-OC = avr-objcopy
F_CPU = 16000000

COMMON_DIR = ../common
SOURCES = main.c uart.c gpio.c
COMMON_SOURCES = rc_decoder/rc_decoder.c
DIRS = 
#Compiler flags
CFLAGS = -D__AVR_ATmega328P__  -DF_CPU=$(F_CPU)UL -mmcu=atmega328
CFLAGS += -funsigned-char -funsigned-bitfields -fshort-enums -Os
CFLAGS += -Wall -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--relax
CFLAGS += -std=c++11
INCLUDES= -I$(COMMON_DIR)
CFLAGS += $(INCLUDES)

OBJDIR = obj
OBJECTS = $(patsubst %.c, $(OBJDIR)/%.o, $(SOURCES))
OBJECTS_FROM_COMMON = $(patsubst %.c, $(OBJDIR)/common/%.o, $(COMMON_SOURCES))

OBJ_SUBDIRS = $(patsubst %, $(OBJDIR)/%, $(DIRS)) 
OBJ_SUBDIRS += $(patsubst %, $(OBJDIR)/common/%, $(dir $(COMMON_SOURCES)))

all:
	mkdir -p $(OBJ_SUBDIRS) 
	mingw32-make flash

flash: $(OBJDIR)/main.hex
	avrdude -F -c avrisp -p atmega328p -P COM3 -b 57600 -U flash:w:$<

$(OBJDIR)/main.hex: $(OBJDIR)/main.elf
	$(AVR-OC) -j .text -j .data  -O ihex $< $@

$(OBJDIR)/common/%.o: $(COMMON_DIR)/%.c
	$(CC) $(CFLAGS) $(COMMON_DIR)/$< -c -o $@

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@


$(OBJDIR)/main.elf: $(OBJECTS) $(OBJECTS_FROM_COMMON)
	$(CC) -mmcu=atmega328  $(OBJECTS) $(OBJECTS_FROM_COMMON) -o $@

clean: 
	rm -rf $(OBJDIR)/*
