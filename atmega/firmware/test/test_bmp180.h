#include <i2c.h>
#include <device/BMP180.h>

void test_main()
{
   protocol::I2C i2c(BMP180_ADDR);
   device::BMP180 bmp180(i2c);
   if(!bmp180.begin())
      serial <<"error intializing bmp180\n";
   while(1)
   {
      auto wait = bmp180.startTemperature();
      serial << 34.34 << "\n";
      _delay_ms(5);
      double temp = 0;
      if (!bmp180.getTemperature(temp))
         serial << "Failed to get temperature\n";
      else
         serial << "Temperature: " << temp << "\n";
      bmp180.startPressure(0);
      _delay_ms(5);
      double pressure;
      if (!bmp180.getPressure(pressure, temp))
         serial << "Failed to read pressure\n";
      else
         serial << "Pressure" << pressure << "\n";
      _delay_ms(1000);
   }
}