
void InitADC()
{
 // Select Vref=AVcc
 ADMUX |= (1<<REFS0);
 // Select Vref=Internal 1.1v voltage
  // ADMUX |= (1<<REFS0) | (1<<REFS1);
 //set prescaller to 128 and enable ADC 
 ADCSRA |= _BV(ADPS1) |_BV(ADPS0)|(1<<ADEN);    
 ADCSRA |= 1<<ADIE;
}

uint16_t ReadADC(uint8_t ADCchannel)
{
 //select ADC channel with  safety mask
 ADMUX = (ADMUX & 0xF0) | (ADCchannel & 0x0F);
 //single conversion mode
 ADCSRA |= (1<<ADSC);
 // wait until ADC conversion is complete
 while( ADCSRA & (1<<ADSC) );
 return ADC;
}

char buf[10];
void printDouble(double v, int decimalDigits)
{
  auto i = 1;
  int intPart, fractPart;
  for (;decimalDigits!=0; i*=10, decimalDigits--);
  intPart = (int)v;
  fractPart = (int)((v-(double)(int)v)*i);
  if(fractPart < 0) fractPart *= -1;
  sprintf(buf,"%d.%d", intPart, fractPart);
}

int cntr = 0;
int max = 0;
int min = 0;
ISR (TIMER0_COMPA_vect)  
{
  // serial << "tick\n";
  // TCNT0=0;
   cntr++;
}

ISR (ADC_vect)
{
  // serial << "conv completed";
  cntr++;
  if (ADC > max)
    max = ADC;
  if (ADC < min) 
    min = ADC;
  ADCSRA |= (1<<ADSC);
}

void StartADC()
{
   //select ADC channel with  safety mask
  ADMUX = (ADMUX & 0xF0) | (0 & 0x0F);
  //single conversion mode
  ADCSRA |= (1<<ADSC);

}
void test_main()
{ 
  sei();
   InitADC();
   StartADC();
   while(true)
   {
      // auto voltage = ReadADC(0);

      // serial << "voltage: " << res << "\n";
         // printDouble(voltage, 4);
      // if (abs(512 - voltage)  > 10)

         // serial << voltage << "\n";
      serial << "wait" << "\n";
      int save = 0;
      cntr = 0;
      _delay_ms(20);
      save = cntr;
      serial << "max=  " << max <<" min= "<<min << "\n";
      _delay_ms(500);
      max = 0;
      min = 1024;

   }
}