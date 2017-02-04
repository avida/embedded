#include <spi.h>
#include <util/delay.h>
#include "font8x8_basic.h"

char bytes[2];

#define DISP_TEST_REG  0x0f
#define SHUTDOWN_REG   0x0c
#define DECODE_REG     0x09
#define SCAN_LIMIT_REG 0x0b
#define INTENSITY_REG  0xA;

#define SetReg(reg, data) \
   bytes[0] = reg; \
   bytes[1] = data; \
   spi.TranseferBytes(bytes, 2)

void render(protocol::SPI& spi)
{
   for (auto j=0x21; j < 0x7e; j++)
   {
      for (auto i = 0; i < 8; i++)
      {
         SetReg(0x08 - i, font8x8_basic[j][i]);
      }
      _delay_ms(1000);
   }
}

void renderChar(protocol::SPI& spi, char c)
{
   for (auto i = 0; i < 8; i++)
   {
      SetReg(0x08 - i, font8x8_basic[c][i]);
   }
}

void test_main()
{
   gpio::Pin cc(gpio::B, 1);
   protocol::SPI spi(&cc);

   serial << "power off" << "\n";
   SetReg(SHUTDOWN_REG, 0);
   _delay_ms(10);
   serial << "power on" << "\n";
   SetReg(SHUTDOWN_REG, 1);
   _delay_ms(10);
   // set scan limit
   SetReg(SCAN_LIMIT_REG, 0xff);
   SetReg(INTENSITY_REG, 0x01);
   //
   _delay_ms(100);
   while(true)
   {
      renderChar(spi, '>');
      _delay_ms(400);
      renderChar(spi, ' ');
      _delay_ms(400);
   }
   while(true)
   {
      render(spi);
   }
   serial << "light" << "\n";
   auto count = 0x03;
   auto line =  0x0f;
   while(true)
   {
      SetReg(count, line);
      if (++count > 0x08)
      {
         count = 0x01;
         line = line == 0x0f ? 0xf0 : 0x0f;
      }
      _delay_ms(100);
   }
   _delay_ms(100);
   serial << "power off" << "\n";
   SetReg(SHUTDOWN_REG, 0);
}