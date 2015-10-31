#include <PCD8544.h>
extern char zerg [];

void test_main()
{
   sensors::DHT11 dht(gpio::B, 3);
   gpio::Pin rst(gpio::D, 2);
   gpio::Pin light(gpio::D, 3);
   gpio::Pin ce(gpio::D, 4);
   gpio::Pin dc(gpio::D, 5);
   gpio::Pin din(gpio::D, 6);
   gpio::Pin clk(gpio::D, 7);
   lcd::PCD8544 lcd( rst, ce, dc, din, clk, light);
   auto yoffset = 0;
   while (1)
   {
      lcd.GotoXY(20, yoffset++);
      lcd.DrawBitmap(zerg, 84*6);
      lcd.GotoXY(0,0);
      if (true || dht.ReadData())
      {
         auto hum = dht.GetHumidity();
         auto temp = dht.GetTemperature();
         sprintf(str, "Temp.: %d *C", temp);
         lcd.DrawLine(str);
         lcd.GotoXY(0,1);
         sprintf(str, "Humid.: %d %%", hum);
         lcd.DrawLine(str);
      }
      yoffset = yoffset > 5 ? 0 : yoffset;

      _delay_ms(2000);
   }
}