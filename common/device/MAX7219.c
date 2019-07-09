#include "MAX7219.h"
#ifdef FONT_SUPPORT
#include "font8x8_basic.h"
#endif
#include "utils.h"
#include <uart.h>

#define DISP_TEST_REG  0x0f
#define SHUTDOWN_REG   0x0c
#define DECODE_REG     0x09
#define SCAN_LIMIT_REG 0x0b
#define INTENSITY_REG  0x0A

char bytes[2];

namespace device
{
MAX7219::MAX7219(protocol::SPI& spi, gpio::IPinOutput* cs): m_spi(spi), m_cs(cs)
{
   m_spi.SetControlPin(cs);
   SetReg(DISP_TEST_REG, 0x1);
   utils::Delay_ms(1);
   SetReg(DISP_TEST_REG, 0x0);
   Power(true);
   SetReg(SCAN_LIMIT_REG, 0xff);
   Intensity(2);
   Power(true);
   m_spi.SetControlPin();
}

void MAX7219::ClearDisplay(bool black)
{
   m_spi.SetControlPin(m_cs);
   char fill = black ? 0 : 0xff;
   for(auto i = 0; i < 8; i++)
   {
      SetReg(0x08 - i, fill);
   }
   m_spi.SetControlPin();
}

void MAX7219::DrawCustomChar(const char * char_data)
{
   m_spi.SetControlPin(m_cs);
   for (auto i = 0; i < 8; i++)
   {  
      SetReg(0x08 - i, char_data[i]);
   }
   m_spi.SetControlPin();
}

void MAX7219::DrawChar(char c)
{
#ifdef FONT_SUPPORT
   DrawCustomChar(font8x8_basic[c]);
#endif
}

void MAX7219::TestRender()
{
#ifdef FONT_SUPPORT   
   for (auto j=0x21; j < 0x7e; j++)
   {
      for (auto i = 0; i < 8; i++)
      {
         SetReg(0x08 - i, font8x8_basic[j][i]);
      }
      utils::Delay_ms(1000);
   }
#endif   
}

void MAX7219::Power(bool on)
{
   SetReg(SHUTDOWN_REG, on ? 1 : 0);
}

void MAX7219::Intensity(uint8_t intensity)
{
   SetReg(INTENSITY_REG, intensity);
}

void MAX7219::SetReg(uint8_t reg, uint8_t data)
{
   bytes[0] = reg;
   bytes[1] = data;
   m_spi.TransferBytes(bytes, 2);
}  
}
