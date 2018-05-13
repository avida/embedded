CC = g++
NAME = raspi_service
INCLUDES = 
LFLAGS = -lbcm2835
CFLAGS += -std=c++11  -g -o0
CFLAGS += $(INCLUDES)

HEADERS = $(wildcard *h $(foreach var,$(DIRS), $(var)/*h))

SOURCES = main.c gpio_rpi.c utils.c spi.c uart.c

include ../Makefile.common

OBJECTS = $(patsubst %.c, $(OBJDIR)/%.o, $(SOURCES))

all::
	mkdir -p $(OBJ_SUBDIRS)

all :: $(OBJDIR)/$(NAME)

$(OBJDIR)/%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $< -c -o $@

$(OBJDIR)/$(NAME): $(OBJECTS) $(OBJECTS_FROM_COMMON)
	$(CC) $(OBJECTS) $(OBJECTS_FROM_COMMON) -o $(OBJDIR)/$(NAME) $(LFLAGS)

$(OBJDIR)/common/%.o: $(COMMON_DIR)/%.c
	$(CC) $(CFLAGS) $(COMMON_DIR)/$< -c -o $@

clean:
	 rm -rf $(OBJDIR)/*
