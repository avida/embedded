#include <i2c.h>

// #define SLAVE

const char SLAVE_ADDRESS = 0x8;

#ifdef SLAVE

void test_main()
{
   protocol::I2C i2c(SLAVE_ADDRESS);
   while(true)
   {
      serial << "Listen\n";
      i2c.Listen();
   }
}

#else

void test_main()
{
   char str[] = "Hell";
   serial << "i2c test\n";
   str[3] |= 0xff;
   // char BMP180_address = 0x77;
   // auto result = i2c.ReadRegister(BMP180_address,0xd0, &data, 1);
   while(true)
   {
      protocol::I2C i2c;
      auto result = i2c.Transmit(SLAVE_ADDRESS, str, 4);
      serial <<"transmit result:" << result << "\n";
         _delay_ms(1000);
      if (result)
      {
         // serial << data << "\n";
      }
   }
}

#endif
