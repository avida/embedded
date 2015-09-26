#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <uart.h>
#include "gpio.h"
#include "rc_decoder/rc_decoder.h"
#include <rc_decoder/rc_transmitter.h>
#include "pwm_rc_pin.h"

//dccduino ->   gpio
// D0-7 = PD0-7
// D8-13 = PB0-5
// A0-5 -> PC0-5
// A6-7 = ADC6-7
//#define F_CPU 16000000UL

gpio::PinOutput ledPin(gpio::B, 5);
gpio::PWMRCPin pwmPin(38);

gpio::PinInput pd4(gpio::D, 4);

uart::UART serial;

char str[80];

class Lamp
{
public:
    Lamp():sock1(gpio::D, 5),
           sock2(gpio::D, 6),
           m_lamp_state(Half)
  {
    AdjustLampState();
  }

  void SwitchNext()
  {
    m_lamp_state +=1;
    if (m_lamp_state > Full)
    {
      m_lamp_state = Off;
    }
    AdjustLampState();
  }

  void SwitchOff()
  {
    m_lamp_state = Off;
    AdjustLampState();
  }
private:
  enum LampState
  {
    Off = 0,
    Half,
    Full
  };

  void AdjustLampState()
  {
    switch(m_lamp_state)
    {
      case Off:
        sock1 = sock2 = true;
        break;
      case Half:
        sock1 = true;
        sock2 = false;
        break;
      case Full:
        sock1 = sock2 = false;
        break;
    }
  }
  gpio::PinOutput sock1;
  gpio::PinOutput sock2;  
  int m_lamp_state;
} lamp;

uint32_t sony_red_button_code = 0x000ba5;
uint32_t sony_green_button_code = 0x000ba6;

void onDecode (void* d)
{
   char* data = (char *)d;
   if ( *((uint32_t*)data) == sony_red_button_code)
   {
      lamp.SwitchNext();
      _delay_ms(500);
   }
   else if (*((uint32_t*)data) == sony_green_button_code)
   {
      lamp.SwitchOff();
   }
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

irRemote::DataDecoder ir_decoder;
int cnt = 0;
ISR (TIMER0_COMPA_vect)  
{
  ir_decoder.ProcessSignal(!pd4);
  //transmitter.DoStep();
}
//gpio::PinOutput pin3(gpio::D, 3 );

int main(void) {
  ir_decoder.SetDecodeCB(onDecode);
  irRemote::PulseData data;
  setupTimer();
   while (1);
   return 0;
}