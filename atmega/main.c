//dccduino ->   gpio
// D0-7 = PD0-7
// D8-13 = PB0-5
// A0-5 -> PC0-5
// A6-7 = ADC6-7
//#define F_CPU 16000000UL
// led pin = PB5
#ifdef TEST

#include "firmware/test_firmware.h"

#else
#include "firmware/lamp_firmware.h"
// #include "firmware/bicycle_fw.h"
// #include "firmware/greenhouse_fw.h"
// #include "firmware/chess_firmware.h"

#endif

int main(void) {
   fw_main();
   return 0;
}
