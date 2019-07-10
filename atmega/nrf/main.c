//dccduino ->   gpio
// D0-7 = PD0-7
// D8-13 = PB0-5
// A0-5 -> PC0-5
// A6-7 = ADC6-7
//#define F_CPU 16000000UL
// led pin = PB5

#include <uart.h>
#include <utils.h>
#include <RCSwitch.h>
#include <device/NRF24L01.h>
#include "atmega_pin.h"

RCSwitch* pSwitch;
device::NRF24L01 *nrf_ptr = NULL;
volatile bool sending = false;
const char* str = "test";
enum {
    A = 6980744,
    B = 6980740,
    C = 6980738,
    D = 6980737
} SmallRCCodes;

enum {
    Aon = 16762196,
    Aoff = 16762193,
    Bon = 16765268,
    Boff = 16765265,
    Con = 16766036,
    Coff = 16766033,
    Don =  16766228,
    Doff = 16766225,
    Eon = 16766276,
    Eoff = 16766273
} BigRemoteCodes;

ISR(INT1_vect)
{
   utils::InterruptsLock lck;
   if (!sending)
   {
       pSwitch->handleInterrupt();
   }
 }

void setup_pin_interrupt() {
   EIMSK = 1<<INT1 | 1<<INT0;
   EICRA = 1<<ISC10 | 1;
   EIFR = 1<<INTF1 | 1<<INTF0;
   sei();
}

void data_sent(bool ready)
{
   if (!ready){
       serial << "Data sent failed\n";
   }
   sending = false;
}

ISR(INT0_vect)
{
   utils::InterruptsLock lck;
   if (nrf_ptr)
   {
      nrf_ptr->Async_ext_event();
   }
 }

typedef struct {
    uint8_t type;
    unsigned long value;
} Packet;

void sendCode(RCSwitch* transmitter, unsigned long code)
{
    utils::InterruptsLock lck;
    transmitter->send(code, 24);
}


int main(void) {
    init();
    gpio::atmega::Pin cc(gpio::B, 2);
    gpio::atmega::Pin ce(gpio::B, 1);
    protocol::SPI spi(&cc);
    device::NRF24L01 nrf(spi, ce);
    nrf_ptr  = &nrf;
    serial << "Hi\n";
    serial << sizeof(unsigned long) <<"\n";
    RCSwitch mySwitch = RCSwitch();
    RCSwitch transmitter = RCSwitch();
    mySwitch.enableReceive();
    pSwitch = &mySwitch;
    setup_pin_interrupt();
    transmitter.enableTransmit(4);
    auto nrf_data_buff = nrf_ptr->GetBufferPtr();
    Packet packet = {42, 0}; 
    serial << "start\n";
    while(1) {
        if ( mySwitch.available()){
            auto RCcode = mySwitch.getReceivedValue();
            packet.value = RCcode;
            serial <<  RCcode << "\n";
            sending = true;
            nrf.SendDataAsync(data_sent, (char *)&packet);
            pSwitch->resetAvailable();
            switch (RCcode) {
                case C:
                {
                    sendCode(&transmitter,Aon);
                    break;
                }
                case D:
                {
                    sendCode(&transmitter,Aoff);
                    break;
                }
            }
        }
     }
}
