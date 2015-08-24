ATMEGA_DIR = atmega
STM_DIR = stm
#set PATH=G:\toolchains\arm\arm-none-eabi\bin;%PATH%
all:	
	mingw32-make stm
	mingw32-make atmega
atmega: 
	$(MAKE) -C $(ATMEGA_DIR)

stm:
	$(MAKE) -C $(STM_DIR)

CC = x86_64-w64-mingw32-g++.exe
COMMON_DIR = common
COMMON_SOURCES = rc_decoder/rc_decoder.c rc_decoder/rc_transmitter.c
INCLUDES= -I$(COMMON_DIR)
CFLAGS = -g -std=c++11
CFLAGS += $(INCLUDES)
TEST_DIR = test
OBJ_DIR = $(TEST_DIR)/obj

COMMON_OBJS = $(patsubst %.c, $(OBJ_DIR)/$(COMMON_DIR)/%.o, $(COMMON_SOURCES))
DIRS = $(dir $(COMMON_OBJS))
SOURCES = test.cpp gpio_mock.cpp
OBJS = $(patsubst %cpp, $(OBJ_DIR)/%o, $(SOURCES))
unittest: 
	mkdir -p $(DIRS)
	echo $(OBJS)
	mingw32-make unittest-exe

unittest-exe: test.exe

test.exe: $(COMMON_OBJS) $(OBJS)
	$(CC) $(CFLAGS) $(COMMON_OBJS) $(OBJS) -o $@

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@	

$(OBJ_DIR)/$(COMMON_DIR)/%.o: common/%.c
	$(CC) $(CFLAGS) -c $< -o $@	
	
.PHONY: all $(ATMEGA_DIR) $(STM_DIR)