#include <device/mfrc522.h>
#include "atmega_pin.h"
#include <spi.h>
#include <utils.h>

gpio::atmega::Pin cs(gpio::B, 2);
gpio::atmega::Pin reset(gpio::B, 1);

void test_main()
{
   protocol::SPI spi(&cs);
   MFRC522 rfid(spi, cs, reset);
   rfid.PCD_Init();
   uint8_t buffer[18];
   while (true)
   {
      if ( ! rfid.PICC_IsNewCardPresent() || ! rfid.PICC_ReadCardSerial() )
      {
         // serial << "no card ( \n";
      }
      else
      {
         uint8_t size = sizeof(buffer);
         rfid.MIFARE_Read(6, buffer, &size);
         buffer[3]= 0;
         serial << (char *)(buffer + 1) << "\n";
         serial << "card found !!\n";
      }

     utils::Delay_ms(500);
   }
}