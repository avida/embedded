#include <device/mfrc522.h>
#include "atmega_pin.h"
#include <spi.h>
#include <utils.h>
#include <uart.h>

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

void fw_main()
{
   second_mux_en = true;
   serial << "hi its ches board\n";
   protocol::SPI spi(&cs);
   MFRC522 rfid(spi, cs, reset);
   rfid.PCD_Init();
   rfid.PCD_SetAntennaGain(MFRC522::RxGain_max);
   uint8_t buffer[18];
   uint8_t index = 0;
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
         // buffer[3]= 0;
         // serial << (char *)(buffer + 1) << "\n";
         serial << "card found " << index << "!!\n";
      }
      index = index >= 31 ? 0 : index + 1;
      address_port(index);
      // utils::Delay_ms(1);
   }
}