import os

SRC = Glob("../common/*/*.c", strings=True) + Glob("../common/*.c")
AVR_TOOLCHAIN_DIR = 'c:\\avr\\bin'
#print (SOURCE_LIST)

#env.Program('hello', 'hello.cpp')
COMMON_PATH = "../common"

fw_builder = Builder(action="""
   avr-objcopy -j .text -j .data  -O ihex $SOURCE $TARGET 
""")

atmega_env = Environment(ENV = os.environ, 
                  tools = ['mingw'],
                  CC = 'avr-g++', 
                  BUILDERS = {'FW' : fw_builder})

atmega_env.AppendENVPath('PATH', AVR_TOOLCHAIN_DIR)

Export('COMMON_PATH', 'atmega_env')

SConscript('atmega/SConscript')