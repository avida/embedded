#set PATH=G:\toolchains\arm\arm-none-eabi\bin;%PATH%
CC = arm-none-eabi-g++

CFLAGS = -mcpu=cortex-m3 -mthumb -g0 -Wall -Os -std=c++11 -c -DSTM32F103VEH6 -DSTM32F10X_LD_VL
INCLUDES= -Isystem/cmsis_core -Isystem/STM32F10x_HD_STDLIB


LFLAGS = -mcpu=cortex-m3 -mthumb -g0  -Os -Wl,--gc-sections 
LFLAGS += -lm -lgcc -lc -lrdimon --specs=nosys.specs -Wl,-Tsystem/arm-gcc-link.ld

DIRS = console utils rc_decoder lcd system system/STM32F10x_HD_STDLIB 
SOURCES = key.c led.c main.c utils/timer.c console/console.c rc_decoder/rc_decoder.c lcd/lcd.c lcd/FComic18B_ASCII.c 
SOURCES += $(wildcard system/STM32F10x_HD_STDLIB/*.c) system/STM32F10x_HD_STDLIB/startup_stm32f10x_hd.s 

OBJDIR = obj
LFLAGS += -Wl,-Map=$(OBJDIR)/main.map
OBJECTS = $(patsubst %.c, $(OBJDIR)/%.o, $(filter %.c, $(SOURCES)) )
OBJECTS += $(patsubst %.s, $(OBJDIR)/%.o, $(filter %.s, $(SOURCES)) )

OBJ_SUBDIRS = $(patsubst %, $(OBJDIR)/%, $(DIRS))

all:
	mkdir -p $(OBJ_SUBDIRS)
	mingw32-make $(OBJDIR)/main.bin

$(OBJDIR)/main.bin: $(OBJDIR)/main.elf
	objcopy -j.text -j.data -O binary $< $@

$(OBJDIR)/main.elf: $(OBJECTS)
	$(CC) $(LFLAGS) $(OBJECTS) -o $@

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/%.o: %.s
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

NON_SYS_DIRS = $(filter-out $(OBJDIR)/system%, $(OBJ_SUBDIRS))
clean:
	rm -fd $(patsubst %, %/*.o, $(NON_SYS_DIRS)) 
	rm -fd $(NON_SYS_DIRS)
	rm -fd $(OBJDIR)/*.elf $(OBJDIR)/*.bin $(OBJDIR)/*.map $(OBJDIR)/*.o

clean-all:
	rm -rf $(OBJDIR)/*

