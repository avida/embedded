#include "uart.h"
#include <Arduino.h>
#include <DHT.h>

#include <RCSwitch.h>
#include "utils.h"
#include "atmega_pin.h"

RCSwitch* pSwitch;
ISR(INT1_vect)
{
   utils::InterruptsLock lck;
//    EIFR = 1<<INTF1;
//    serial << "C\n";
   pSwitch->handleInterrupt();
 }

void setup_pin_interrupt() {
   EIMSK = 1<<INT1;
   EICRA = 1<<ISC10;
   EIFR = 1<<INTF1;
   sei();
}


void test_main(void) {
    init();
    setup_pin_interrupt();
    serial << "Hi\n";
    RCSwitch mySwitch = RCSwitch();
    // mySwitch.enableTransmit(3);
    mySwitch.enableReceive();
    pSwitch = &mySwitch;
    while(1) {
        if (mySwitch.available()){
            serial << "Received: ";

            unsigned long val;
            {
                utils::InterruptsLock lck;
                val = mySwitch.getReceivedValue();
            }
            serial << val << "\n";
            if (val == 16766276) {
                // if code is "E on"
                // send "A off"
                // mySwitch.resetAvailable();
                // InterruptLock lck;
                // mySwitch.enableTransmit(3);
                // mySwitch.send(16762193 , 24);
                // mySwitch.enableReceive();
                // setup_pin_interrupt();
            }
            mySwitch.resetAvailable();
        }
    // mySwitch.send(16762193 , 24);
     }
}
