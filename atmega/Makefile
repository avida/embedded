CC = avr-g++
AVR-OC = avr-objcopy
F_CPU = 16000000
COM_PORT = COM5
COMMON_DIR = ../common

SOURCES = main.c uart.c  utils.c atmega_pin.c \
			 spi.c \
			 # dht11.c \
			 # pwm_rc_pin.c \
			 # i2c.c \

COMMON_SOURCES = 	rfid_reader.c\
					 #  device/NRF24L01.c \
					 #  rc_decoder/rc_decoder.c rc_decoder/rc_transmitter.c \
					 #  rc_decoder/rc_base_state_machine.c rc_decoder/rc_pulse.c \
					 #  rc_decoder/rc_state_machine.c rc_decoder/rc_pulse_data.c \
					 # PCDdata.c PCD8544.c \
					 
DIRS = 
#Compiler flags
328_CFLAGS = -D__AVR_ATmega328P__  -DF_CPU=$(F_CPU)UL -mmcu=atmega328 
168_CFLAGS = -D__AVR_ATmega168__  -DF_CPU=$(F_CPU)UL -mmcu=atmega168 
CFLAGS += -funsigned-char -funsigned-bitfields -fshort-enums -Os
CFLAGS += -Wall -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--relax
CFLAGS += -std=c++11
INCLUDES= -I$(COMMON_DIR)
CFLAGS += $(INCLUDES)
168_LFLAGS = -mmcu=atmega168 
328_LFLAGS = -mmcu=atmega328p

OBJDIR = obj
OBJECTS = $(patsubst %.c, $(OBJDIR)/%.o, $(SOURCES))
OBJECTS_FROM_COMMON = $(patsubst %.c, $(OBJDIR)/common/%.o, $(COMMON_SOURCES))

OBJ_SUBDIRS = $(patsubst %, $(OBJDIR)/%, $(DIRS)) 
OBJ_SUBDIRS += $(patsubst %, $(OBJDIR)/common/%, $(dir $(COMMON_SOURCES)))

all:
	mkdir -p $(OBJ_SUBDIRS)
	mingw32-make flash-m328 TARGET=m328

m168:
	mkdir -p $(OBJ_SUBDIRS) 
	mingw32-make flash-m168 TARGET=m168

flash-m328: $(OBJDIR)/main.hex
	avrdude -c avrisp -p atmega328p -P $(COM_PORT) -b 57600 -U flash:w:$<

flash-m168: $(OBJDIR)/main.hex
	avrdude -c avrisp -p atmega168 -P $(COM_PORT) -b 19200 -U flash:w:$<  

$(OBJDIR)/main.hex: $(OBJDIR)/main.elf
	$(AVR-OC) -j .text -j .data  -O ihex $< $@

$(OBJDIR)/main.elf: $(OBJECTS) $(OBJECTS_FROM_COMMON)
	$(CC) $(CFLAGS) $(LFLAGS)  $(OBJECTS) $(OBJECTS_FROM_COMMON) -o $@


$(OBJDIR)/common/%.o: $(COMMON_DIR)/%.c
	$(CC) $(CFLAGS) $(COMMON_DIR)/$< -c -o $@

$(OBJDIR)/%.o: %.c
ifeq ($(TARGET),m168)
	$(eval CFLAGS = $(CFLAGS) $(168_CFLAGS))
	$(eval LFLAGS = $(168_LFLAGS))
else
	$(eval CFLAGS = $(CFLAGS) $(328_CFLAGS))
	$(eval LFLAGS = $(328_LFLAGS))
endif
	$(CC) $(CFLAGS) $< -c -o $@

clean: 
	rm -rf $(OBJDIR)/*
