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
	
.PHONY: all $(ATMEGA_DIR) $(STM_DIR)