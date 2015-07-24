CC = arm-none-eabi-gcc
CFLAGS = -mcpu=cortex-m3 -mthumb -g0 -Wall -Os -std=gnu99 -c -DSTM32F103VEH6 -DSTM32F10X_LD_VL
INCLUDES= -Isystem/cmsis_core -Isystem/STM32F10x_HD_STDLIB
LFLAGS = -mcpu=cortex-m3 -mthumb -g0 -Wl,-Map=main.map -Os -Wl,--gc-sections -lm -lgcc -lc -lrdimon  -Wl,-Tsystem/arm-gcc-link.ld

SOURCES = key.c led.c main.c utils/timer.c console/console.c rc_decoder/rc_decoder.c lcd/lcd.c lcd/FComic18B_ASCII.c $(wildcard system/STM32F10x_HD_STDLIB/*.c) system/STM32F10x_HD_STDLIB/startup_stm32f10x_hd.s 
COMP_SRC = $(wildcard system/STM32F10x_HD_STDLIB/*.c)
COMP_OBJS = $(patsubst system/STM32F10x_HD_STDLIB/%.c, system/STM32F10x_HD_STDLIB/%.o, $(COMP_SRC))
COMP_OBJS += system/STM32F10x_HD_STDLIB/startup_stm32f10x_hd.o
COMP_OBJS += system/CLib/syscalls.o
OBJS = key.o led.o main.o utils/timer.o console/console.o rc_decoder/rc_decoder.o lcd/lcd.o lcd/FComic18B_ASCII.o $(COMP_OBJS)

main: main.elf
	objcopy -j.text -j.data -O binary main.elf main.bin

main.elf: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o main.elf	

%.o: %.c
	#set PATH=G:\toolchains\arm\arm-none-eabi\bin;%PATH%
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm $(OBJS) *map *bin *elf
