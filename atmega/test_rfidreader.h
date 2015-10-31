#include <rfid_reader.h>

void test_main()
{
   serial << "test_ rfid\n";
   gpio::Pin pin_reset(gpio::D,2);
   gpio::Pin cc(gpio::B, 2);
   protocol::SPI spi(&cc);
   device::RFIDReader rfid(spi, pin_reset);
   rfid.Init();
   while(1)
   {
      //serial << "value: " << rfid.Version() << "\n"; 
      rfid.SelfTest();
      _delay_ms(500);
   }
}