import os

SRC = Glob("../common/*/*.c", strings=True) + Glob("../common/*.c")
AVR_TOOLCHAIN_DIR = r'g:\toolchains\avr\bin'

CPPDEFINES= []
def cb(option,opt, value, parser):
   global CPPDEFINES
   CPPDEFINES.append(value)

AddOption("--DEFINE", type="string", action = 'callback', callback = cb)

COMMON_PATH = "../common"

fw_builder = Builder(action="""
   avr-objcopy -j .text -j .data  -O ihex $SOURCE $TARGET 
""")
atmega_env = Environment(ENV = os.environ, 
                  tools = ['mingw'],
                  CC = 'avr-g++',
                  CXX = 'avr-g++',
                  BUILDERS = {'FW' : fw_builder})

atmega_env.AppendENVPath('PATH', AVR_TOOLCHAIN_DIR)

Export('COMMON_PATH', 'atmega_env', 'CPPDEFINES')
ACTION = ARGUMENTS.get('action', 'm168')

if (ACTION == 'flash'):
    FW_PATH = r".\atmega\3rdparty\{}"
    FWs = {"m168": "ATmegaBOOT_168_diecimila.hex",
           "m328": "ATmegaBOOT_168_atmega328.hex"}
    TARGET = ARGUMENTS.get('target', 'm168')
    FW = FW_PATH.format(FWs[TARGET])
    TARGET+='p'
    cmd = "avrdude -p {}  -c usbasp  -U flash:w:{}".format(TARGET, FW)
    print("Flashing 328 with cmd {}".format(cmd))
    flash = atmega_env.Command("flash",FW,  [cmd])
    AlwaysBuild(flash)
else:
    SConscript('atmega/SConscript')
