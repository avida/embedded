#include <avr/io.h>
#include <uart.h>
#include "utils.h"
#include <string.h>
#include "atmega_pin.h"
#include "dht11.h"

void InitADC()
{
   // Select Vref=AVcc
   ADMUX |= (1<<REFS0);
   //set prescaller to 128 and enable ADC 
   ADCSRA |= _BV(ADPS1) |_BV(ADPS0)|(1<<ADEN);    
   ADCSRA |= 1<<ADIE;
}

uint16_t ReadADC(uint8_t ADCchannel)
{
   //select ADC channel with  safety mask
   ADC = 0;
   ADMUX = (ADMUX & 0xF0) | (ADCchannel & 0x0F);
   //single conversion mode
   ADCSRA |= (1<<ADSC);
   // wait until ADC conversion is complete
   while( ADCSRA & (1<<ADSC) );
   return ADC;
}

void fw_main()
{
   // serial << "green house begin\n";
   InitADC();
   char input_cmd[80];
   gpio::atmega::Pin pump_pin(gpio::D, 3);
   sensors::DHT11 dht(gpio::D, 4);
   pump_pin = false;
   while(true)
   {
      serial >> input_cmd;
      if (strcmp(input_cmd, "moisture") == 0)
      {
         for (int i =0; i < 4; ++i)
         {
            serial << "channel "<< i <<": "<< ReadADC(i) << "\n";
         }
      }
      else if (strcmp(input_cmd, "pumpon") == 0)
      {
         serial << "pump on\n";
         pump_pin = true;
      }
      else if (strcmp(input_cmd, "pumpoff") == 0)
      {
         serial << "pump off\n";
         pump_pin = false;
      }
      else if (strcmp(input_cmd, "sensor") == 0)
      {
         if (dht.ReadData())
         {
            serial << "humidity: " << dht.GetHumidity()
                   << "\n temp: "  << dht.GetTemperature()  << "\n";

         }
         else
         {
            serial << "error reading sensor data\n";
         }
      }
      else
      {
         // serial << "unkown command\n";
      }
   }
}
