#include <device/mfrc522.h>
#include "atmega_pin.h"
#include <spi.h>
#include <utils.h>
#include <uart.h>
#include <string.h>
#include "utils.h"

gpio::atmega::Pin cs(gpio::B, 2);
gpio::atmega::Pin reset(gpio::B, 1);


gpio::atmega::Pin first_mux_en(gpio::D, 2);
gpio::atmega::Pin first_mux[4] = {
   gpio::atmega::Pin(gpio::D, 3),
   gpio::atmega::Pin(gpio::D, 4),
   gpio::atmega::Pin(gpio::D, 5),
   gpio::atmega::Pin(gpio::D, 6)
};

gpio::atmega::Pin second_mux_en(gpio::D, 7);
gpio::atmega::Pin second_mux[4] = {
   gpio::atmega::Pin(gpio::C, 0),
   gpio::atmega::Pin(gpio::C, 1),
   gpio::atmega::Pin(gpio::C, 2),
   gpio::atmega::Pin(gpio::C, 3)
};


void address_port(uint8_t port)
{
   gpio::atmega::Pin *current_mux = first_mux;
   if ((port >> 4) & 1)
   {
      current_mux = second_mux;
      first_mux_en = true;
      second_mux_en = false;
   }
   else
   {
      first_mux_en = false;
      second_mux_en = true;  
   }
   for (auto i = 0; i < 4; ++i)
   {
      if ((port >> i) & 1)
      {
         current_mux[i] = true;
      }
      else
         current_mux[i] = false;
   }
}

enum Figures
{
   BlackPawn = 'p',
   BlackRook = 'r',
   BlackKnight = 'n',
   BlackBishop = 'b',
   BlackQueen = 'q',
   BlackKing = 'k',
   WhitePawn = 'P',
   WhiteRook = 'R',
   WhiteKnight = 'N',
   WhiteBishop = 'B',
   WhiteQueen = 'Q',
   WhiteKing = 'K'
};

char board[32];

void fw_main()
{
   second_mux_en = true;
   memset(board, '0', 32);
   serial << "hi its ches board\n";
   protocol::SPI spi(&cs);
   MFRC522 rfid(spi, cs, reset);
   rfid.PCD_Init();
   rfid.PCD_SetAntennaGain(MFRC522::RxGain_max);
   uint8_t buffer[18];
   uint8_t index = 0;
   utils::CountMillis();
   auto old_time = utils::GetTimeValue();
   while (true)
   {
      if ( ! rfid.PICC_IsNewCardPresent() || ! rfid.PICC_ReadCardSerial() )
      {
         // serial << "no card ( \n";
         board[index] = '0';
      }
      else
      {
         uint8_t size = sizeof(buffer);
         rfid.MIFARE_Read(6, buffer, &size);
         // serial << "card found " << index << "!!\n";
         // buffer[2]= 0;
         board[index] = buffer[1];
         // serial << index <<" : " << (char *)(buffer + 1) << "\n";
      }
      index = index >= 31 ? 0 : index + 1;
      if (index == 0)
      {
         auto  time_diff = utils::GetTimeValue() - old_time;
         for (auto i = 0; i < 4; ++i)
         {
            for (auto j = 0; j < 8; ++j)
            {
               serial << board[i * 8 + j] <<" ";
            }
            serial << "\n";
         }
         serial << time_diff << " \n";
         old_time = utils::GetTimeValue();
      }
      address_port(index);
      // utils::Delay_ms(2);
   }
}