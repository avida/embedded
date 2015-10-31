#include <rc_decoder/rc_decoder.h>

irRemote::DataDecoder ir_decoder;
int cnt = 0;
ISR (TIMER0_COMPA_vect)  
{
  ir_decoder.ProcessSignal(!pd4);
  //transmitter.DoStep();
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
   setupTimer();
    ir_decoder.SetDecodeCB(onDecode);   
}