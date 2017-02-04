#include <rfid_reader.h>

void test_main()
{
   serial << "test_ rfid\n";
   gpio::Pin pin_reset(gpio::B,1);
   gpio::Pin cc(gpio::B, 2);
   protocol::SPI spi(&cc);
   device::RFIDReader rfid(spi, pin_reset);
   rfid.Init();
   for (auto i = 0 ; i < 3 ; ++i)
   {
      serial << "Version: " << rfid.Version() << "\n"; 
      rfid.SelfTest();
      _delay_ms(500);
   }
}