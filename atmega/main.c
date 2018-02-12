//dccduino ->   gpio
// D0-7 = PD0-7
// D8-13 = PB0-5
// A0-5 -> PC0-5
// A6-7 = ADC6-7
//#define F_CPU 16000000UL
// led pin = PB5
#ifdef TEST

// #include "firmware/test_firmware.h"

#else
// #include "firmware/lamp_firmware.h"
// #include "firmware/bicycle_fw.h"
// #include "firmware/greenhouse_fw.h"
// #include "firmware/chess_firmware.h"

#endif

#include "uart.h"
#include <Arduino.h>
#include <DHT.h>

#include <RCSwitch.h>
#include "utils.h"
#include "atmega_pin.h"

// RCSwitch* pSwitch;
// ISR(INT1_vect)
// {
//    utils::InterruptsLock lck;
// //    EIFR = 1<<INTF1;
// //    serial << "C\n";
//    pSwitch->handleInterrupt();
//  }

void setup_pin_interrupt() {
   EIMSK = 1<<INT1;
   EICRA = 1<<ISC10;
   EIFR = 1<<INTF1;
   sei();
}


int main(void) {
    init();
    serial << "Hi\n";
    DHT dht =  DHT(3, DHT22);
    while(1) {
           // mySwitch.enableTransmit(3);
            float temp = dht.readTemperature();
            float hum = dht.readHumidity();

            serial << "Temperature:" << temp << " Humidity:" << hum << "\n";
            delay(1000);
    // mySwitch.send(16762196 , 24);
     }
   return 0;
}
