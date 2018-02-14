#include <uart.h>
#include <utils.h>
#include <RCSwitch.h>
#include <device/NRF24L01.h>
#include "atmega_pin.h"

RCSwitch* pSwitch;
device::NRF24L01 *nrf_ptr = NULL;
volatile bool sending = false;
const char* str = "test";
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
//    if (ready){
//        serial << "Data sent\n";
//    } else
//    {
//        serial << "Send failed\n";
//    }
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

void fw_main()
{
    init();
    gpio::atmega::Pin cc(gpio::B, 2);
    gpio::atmega::Pin ce(gpio::B, 1);
    protocol::SPI spi(&cc);
    device::NRF24L01 nrf(spi, ce);
    nrf_ptr  = &nrf;
    serial << "Hi\n";
    RCSwitch mySwitch = RCSwitch();
    mySwitch.enableReceive();
    pSwitch = &mySwitch;
    
    setup_pin_interrupt();
    auto nrf_data_buff = nrf_ptr->GetBufferPtr();
    Packet packet = {42, 0}; 
    while(1) {
        if (mySwitch.available()){
            auto RCcode = mySwitch.getReceivedValue();
            packet.value = RCcode;
            // serial <<  RCcode << "\n";
            *((unsigned long *) nrf_data_buff + 1) = RCcode;
            sending = true;
            nrf.SendDataAsync(data_sent, (char *)&packet);
            pSwitch->resetAvailable();
        }
     }
}