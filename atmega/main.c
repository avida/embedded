//dccduino ->   gpio
// D0-7 = PD0-7
// D8-13 = PB0-5
// A0-5 -> PC0-5
// A6-7 = ADC6-7
//#define F_CPU 16000000UL
// led pin = PB5
// #include "firmware/lamp_firmware.h"
#include "firmware/test_firmware.h"

int main(void) {
   fw_main();
   return 0;
}
