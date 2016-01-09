#include <spi.h>

void test_main()
{
   char bt = 12;
   char bt2 = 24;
#ifdef SPI_MASTER
   serial << "master\n";
   gpio::Pin cc(gpio::B, 2);
   protocol::SPI spi(&cc);
   auto ret = spi.SendReceiveByte(&bt);
   serial << "sent " << ret << "\n";
#else
   serial << "slave \n";
   protocol::SPI spi;
   while (1)
   {
      auto ret = spi.SendReceiveByte(&bt2);
      bt2++;
      serial << "received\n";
      serial << (int) ret << "\n";
   }
#endif
   while(1);
}

