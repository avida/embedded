#include <device/mfrc522.h>
#include "atmega_pin.h"
#include <spi.h>
#include <utils.h>
#include <uart.h>
#include <string.h>
#include <i2c.h>

gpio::atmega::Pin cs(gpio::B, 2); //pin 10
gpio::atmega::Pin reset(gpio::B, 1); // pin9


gpio::atmega::Pin mux_en(gpio::D, 2);
gpio::atmega::Pin mux[4] = {
   gpio::atmega::Pin(gpio::D, 3),
   gpio::atmega::Pin(gpio::D, 4),
   gpio::atmega::Pin(gpio::D, 5),
   gpio::atmega::Pin(gpio::D, 6)
};

void swtch_port(uint8_t port)
{
   for (auto i = 0; i < 4; ++i)
   {
      if ((port >> i) & 1)
      {
         mux[i] = true;
      }
      else
      {
         mux[i] = false;
      }
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

enum FiguresCodes
{
   NoneCode = 0,
   WhitePawnCode, // 1
   WhiteRookCode, // 2
   WhiteKnightCode, // 3
   WhiteBishopCode, // 4
   WhiteQueenCode, // 5
   WhiteKingCode, // 6
   BlackPawnCode, // 7
   BlackRookCode, // 8
   BlackKnightCode, //9
   BlackBishopCode, //10
   BlackQueenCode, //11
   BlackKingCode //12
};

// board size is set 32 for i2c
// but in fact we are using only 16
union BoardEntry
{
   struct
   {
      uint8_t code : 4;
      uint8_t addr : 3;
      uint8_t zero : 1;
   };
   char val;
};

#define BOARD_SIZE 32
BoardEntry board[BOARD_SIZE];

void processCmd(uint8_t cmd, char **_buffer, uint8_t *len)
{
   *len = BOARD_SIZE;
   *_buffer = (char *)board;
}

#ifndef I2C_ADDR 
#error "Please define  i2c address"
#endif

#define LINE_SIZE 18
uint8_t getCode(char figure)
{
   if (figure == 0)
      return NoneCode;

   bool isWhite = true;
   // convert to capital letter
   if (figure >  90 )
   {
      figure -= 32;
      isWhite = false;
   }
   uint8_t code = NoneCode;
   switch (figure)
   {
      case WhitePawn:
         code = WhitePawnCode;
         break;
      case WhiteRook:
         code = WhiteRookCode;
         break;
      case WhiteKnight:
         code = WhiteKnightCode;
         break;
      case WhiteBishop:
         code = WhiteBishopCode;
         break;
      case WhiteQueen:
         code = WhiteQueenCode;
         break;
      case WhiteKing:
         code = WhiteKingCode;
         break;
      default:
         return NoneCode;
   }


   if (code != NoneCode && !isWhite)
   {
      code += BlackPawnCode - WhitePawnCode;
   }
   return code;
}

void WriteToBoard(char figure, uint8_t index)
{
   uint8_t code = getCode(figure);
   FOR_I(2)
   {
      board[index + 16*i].zero = 0;
      board[index + 16*i].addr = I2C_ADDR & 0x07;
      board[index + 16*i].code = code;
   }
}

bool readMFRC(MFRC522& reader, uint8_t *buffer)
{
   if ( reader.PICC_IsNewCardPresent() && reader.PICC_ReadCardSerial())
      {
         uint8_t size = LINE_SIZE;
         reader.MIFARE_Read(6, buffer, &size);
         return true;
      }
      else
      {
         return false;
      }
}

int main()
{
   protocol::I2C i2c(I2C_ADDR);
   // set enable pin to low to enable miltiplexers
   mux_en = false;

   memset(board, 0, BOARD_SIZE);
   i2c.ListenForCommand(processCmd); 
   // serial << "hi its ches board\n";
   protocol::SPI spi(&cs);
   spi.SetControlPin();

   MFRC522 rfid(spi, cs, reset);
   rfid.PCD_Init();
   rfid.PCD_SetAntennaGain(MFRC522::RxGain_max);

   uint8_t buffer[LINE_SIZE];
   uint8_t index = 0;
   utils::CountMillis();
   auto old_time = utils::GetTimeValue();
   while (true)
   {
      if (readMFRC(rfid, buffer))
      {
         WriteToBoard(buffer[1], index);
      }
      else
      {
         WriteToBoard(0, index);  
      }

      index = index >= 15 ? 0 : index + 1;
      swtch_port(index);
      // if (index == 0)
      // {
      //    auto  time_diff = utils::GetTimeValue() - old_time;
      //    for (auto i = 0; i < 2; ++i)
      //    {
      //       for (auto j = 0; j < 8; ++j)
      //       {
      //          serial << board[i * 8 + j] <<" ";
      //       }
      //       serial << "\n";
      //    }
      //    serial << time_diff << " \n";
      //    old_time = utils::GetTimeValue();
      // }
   }
   return 0;
}
