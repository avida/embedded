#set PATH=G:\toolchains\arm\arm-none-eabi\bin;%PATH%
CC = arm-none-eabi-g++
COMMON_DIR = ../common

CFLAGS = -mcpu=cortex-m3 -mthumb -g0 -Wall -Os -std=c++11 -c -DSTM32F103VEH6 -DSTM32F10X_LD_VL
INCLUDES= -Isystem/cmsis_core -Isystem/STM32F10x_HD_STDLIB -I$(COMMON_DIR)


LFLAGS = -mcpu=cortex-m3 -mthumb -g0  -Os -Wl,--gc-sections 
LFLAGS += -lm -lgcc -lc -lrdimon --specs=nosys.specs -Wl,-Tsystem/arm-gcc-link.ld

DIRS = console utils lcd system system/STM32F10x_HD_STDLIB 
SOURCES =  main.c utils/timer.c console/console.c lcd/lcd.c lcd/FComic18B_ASCII.c gpio.c uart.c
SOURCES += $(wildcard system/STM32F10x_HD_STDLIB/*.c) system/STM32F10x_HD_STDLIB/startup_stm32f10x_hd.s
COMMON_SOURCES = rc_decoder/rc_decoder.c

OBJDIR = obj
LFLAGS += -Wl,-Map=$(OBJDIR)/main.map
OBJECTS = $(patsubst %.c, $(OBJDIR)/%.o, $(filter %.c, $(SOURCES)) )
OBJECTS += $(patsubst %.s, $(OBJDIR)/%.o, $(filter %.s, $(SOURCES)) )
OBJECTS_FROM_COMMON = $(patsubst %.c, $(OBJDIR)/common/%.o, $(COMMON_SOURCES))

OBJ_SUBDIRS = $(patsubst %, $(OBJDIR)/%, $(DIRS))
OBJ_SUBDIRS += $(patsubst %, $(OBJDIR)/common/%, $(dir $(COMMON_SOURCES)))

all:
	mkdir -p $(OBJ_SUBDIRS)
	mingw32-make $(OBJDIR)/main.bin

$(OBJDIR)/main.bin: $(OBJDIR)/main.elf
	objcopy -j.text -j.data -O binary $< $@

$(OBJDIR)/main.elf: $(OBJECTS) $(OBJECTS_FROM_COMMON)
	$(CC) $(LFLAGS) $(OBJECTS) $(OBJECTS_FROM_COMMON) -o $@

$(OBJDIR)/common/%.o: $(COMMON_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $(COMMON_DIR)/$< -o $@

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/%.o: %.s
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

NON_SYS_DIRS = $(filter-out $(OBJDIR)/system%, $(OBJ_SUBDIRS))
NON_SYS_DIRS += $(OBJDIR)/common
clean:
	rm -fd $(patsubst %, %/*.o, $(NON_SYS_DIRS)) 
	rm -fd $(NON_SYS_DIRS)
	rm -fd $(OBJDIR)/*.elf $(OBJDIR)/*.bin $(OBJDIR)/*.map $(OBJDIR)/*.o

clean-all:
	rm -rf $(OBJDIR)/*

