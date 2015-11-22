#include <i2c.h>

void test_main()
{
   protocol::I2C i2c;
   char data = 0xff;
   serial << "i2c test\n";
   auto result = i2c.Transmit(0x27, &data, 1);
   serial <<"transmit result:" << result << "\n";
   while(1)
      ;
}