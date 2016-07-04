#include <rc_decoder/rc_decoder.h>

irRemote::DataDecoder ir_decoder;

gpio::Pin pb5(gpio::B, 5);
gpio::Pin pd4(gpio::D, 4);

int cnt = 0;

ISR (TIMER0_COMPA_vect)  
{
   utils::InterruptsLock lck;
  ir_decoder.ProcessSignal(!pd4);
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
   pb5 = true;
   serial << str;
   pb5 = false;
}

void test_main()
{
   pd4.SetToInput();
   setupTimer();
   ir_decoder.SetDecodeCB(onDecode);   
   while(true);
}