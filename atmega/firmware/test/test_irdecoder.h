#include <rc_decoder/rc_decoder.h>

#include "atmega_pin.h"
#include "utils.h"
irRemote::DataDecoder ir_decoder;

gpio::atmega::Pin ir_pin(gpio::D, 3);

int cnt = 0;

char str[80];

ISR (TIMER0_COMPA_vect)  
{
   utils::InterruptsLock lck;
  ir_decoder.ProcessSignal(!ir_pin);
}

void setupTimer()
{
   // set timer to CTC mode
   TCCR0A |= (1 << WGM01);
   // set prescaler to 8x
   TCCR0B |= 1 << CS01;
   
   // 40 ticks = 20 us on 2Mhz timer
   OCR0A = 40;
   // enable interrupts
   TIMSK0 |= (1 << OCIE0A);
   sei();
}

void onDecode (void* d)
{
   char str[60];
   char* data = (char *)d;
   sprintf(str, "decoded: %02x%02x%02x\n", *(data+2), *(data+1), *data );
   serial << str;
}

void test_main()
{
   ir_pin.SetToInput();
   setupTimer();
   ir_decoder.SetDecodeCB(onDecode);   
   while(true);
  //  {
  //    ir_decoder.ProcessSignal(!ir_pin);
  //    _delay_us(14);
  //  }
}