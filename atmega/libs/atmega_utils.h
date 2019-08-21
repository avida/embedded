#pragma once 

#include <avr/io.h>

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
