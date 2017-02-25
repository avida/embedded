CC = avr-g++
AVR-OC = avr-objcopy
F_CPU = 16000000
COM_PORT = COM5
COMMON_DIR = ../common
TOOLCHAIN_DIR = G:\toolchains\avr\avr\bin

SOURCES = main.c uart.c  utils.c atmega_pin.c \
			 i2c.c \
			 spi.c \
			 firmware/lamp.c \
			 # dht11.c \
			 # pwm_rc_pin.c \

COMMON_SOURCES =  rfid_reader.c\
					  device/NRF24L01.c \
					  device/BMP180.c \
					  rc_decoder/rc_decoder.c rc_decoder/rc_transmitter.c \
					  rc_decoder/rc_base_state_machine.c rc_decoder/rc_pulse.c \
					  rc_decoder/rc_state_machine.c rc_decoder/rc_pulse_data.c \
					  # PCDdata.c PCD8544.c \
					 
DIRS = firmware \
	    firmware/test
#Compiler flags
328_CFLAGS = -D__AVR_ATmega328P__  -DF_CPU=$(F_CPU)UL -mmcu=atmega328 
168_CFLAGS = -D__AVR_ATmega168__  -DF_CPU=$(F_CPU)UL -mmcu=atmega168 
CFLAGS += -funsigned-char -funsigned-bitfields -fshort-enums -Os $(foreach var,$(DEFINES), -D$(var))
CFLAGS += -Wall -ffunction-sections -Wl,--gc-sections -Wl,--relax -Dprintf=iprintf -lprintf_flt -lm
CFLAGS += -std=c++11 -fno-merge-constants
INCLUDES= -I$(COMMON_DIR) -I.
CFLAGS += $(INCLUDES)
LFLAGS = -Wl,-u,vfprintf
168_LFLAGS = $(LFLAGS) -mmcu=atmega168 
328_LFLAGS = $(LFLAGS) -mmcu=atmega328p

OBJDIR = obj
OBJECTS = $(patsubst %.c, $(OBJDIR)/%.o, $(SOURCES))
HEADERS = $(wildcard *h $(foreach var,$(DIRS), $(var)/*h))
OBJECTS_FROM_COMMON = $(patsubst %.c, $(OBJDIR)/common/%.o, $(COMMON_SOURCES))

OBJ_SUBDIRS = $(patsubst %, $(OBJDIR)/%, $(DIRS)) 
OBJ_SUBDIRS += $(patsubst %, $(OBJDIR)/common/%, $(dir $(COMMON_SOURCES)))
GCCVER= $(shell $(CC) --version | grep g++ | sed 's/^.* //g')
$(eval PATH=$(TOOLCHAIN_DIR);$(PATH))

all:
	mkdir -p $(OBJ_SUBDIRS)
	mingw32-make flash TARGET=m328

m168:
ifneq ($(shell cat .DEFINES), $(DEFINES))
	rm -rf $(OBJDIR)/*
	echo $(DEFINES) > .DEFINES
endif
	mkdir -p $(OBJ_SUBDIRS) 
	mingw32-make flash TARGET=m168

flash: $(OBJDIR)/main.hex
ifeq ($(TARGET),m168)
	$(eval AVRDUDE_FLAGS = -p atmega168 -b 19200)
else
	$(eval AVRDUDE_FLAGS = -p atmega328p -b 57600)
endif
	avrdude -c avrisp $(AVRDUDE_FLAGS) -P $(COM_PORT) -U flash:w:$<

$(OBJDIR)/main.hex: $(OBJDIR)/main.elf
	$(AVR-OC) -j .text -j .data  -O ihex $< $@

$(OBJDIR)/main.elf: $(OBJECTS) $(OBJECTS_FROM_COMMON)
	$(CC) $(CFLAGS) $(LFLAGS)  $(OBJECTS) $(OBJECTS_FROM_COMMON) -o $@

$(OBJDIR)/common/%.o: $(COMMON_DIR)/%.c
	$(CC) $(CFLAGS) $(COMMON_DIR)/$< -c -o $@

$(OBJDIR)/%.o: %.c $(HEADERS)
ifeq ($(TARGET),m168)
	$(eval CFLAGS += $(168_CFLAGS))
	$(eval LFLAGS = $(168_LFLAGS))
	$(eval AVRDUDE_FLAGS = -p atmega168 -b 19200)
else
	$(eval CFLAGS = $(CFLAGS) $(328_CFLAGS))
	$(eval LFLAGS = $(328_LFLAGS))
	$(eval AVRDUDE_FLAGS = -p atmega328p -b 57600)
endif
	$(CC) $(CFLAGS) $< -c -o $@

clean: 
	rm -rf $(OBJDIR)/*
