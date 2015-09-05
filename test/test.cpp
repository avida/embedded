#define BOOST_TEST_MODULE main test
#include <iostream>
#include "gpio_mock.h"
#include <boost/test/unit_test.hpp>
#include <rc_decoder/rc_transmitter.h>
#include "rc_decoder/rc_decoder.h"
#include <uart.h>

gpio::TestPinOutput testPin;
char str[80];
uart::UART serial;

void OnDecode(void *data)
{
   irRemote::PulseData* pdata = (irRemote::PulseData*)data;
   std::cout << "DECODED!!!!: "<< std::endl;
   for (auto i = 0; i < 4; ++i)
   {
      std::cout  << "c[" << i << "]: " << std::hex <<(short)pdata->chars[i] << std::dec<< std::endl;
   }
}
BOOST_AUTO_TEST_CASE(first_test)
{
   irRemote::RCTransmitter transmitter(testPin);
   irRemote::PulseData data;
   data.data  = 0x00ccad00;
   std::cout << "data is : " << data.data << std::endl;
   for (auto i = 0; i < 4; ++i)
   {
      std::cout  << "c[" << i << "]: " << std::hex <<(short)data.chars[i] << std::dec<< std::endl;
   }

   transmitter.StartTransmit(data);
   auto step_counter = 0; 
   irRemote::DataDecoder ir_decoder; 
   ir_decoder.SetDecodeCB(OnDecode); 
   for (auto i = 0; i < 100; ++i)
   {
      while(!transmitter.DoStep())
      {
         if (!(step_counter % 100))
            std::cout << "pulse:" << step_counter ;

         ir_decoder.ProcessSignal(testPin);
         step_counter++;
      }
      transmitter.StartTransmit(data);
   }
   ir_decoder.ProcessSignal(true);

   std::cout << "The end" << std::endl;
      

}
