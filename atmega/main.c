#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <uart.h>
#include "atmega_pin.h"
#include "rc_decoder/rc_decoder.h"
#include <rc_decoder/rc_transmitter.h>
#include "pwm_rc_pin.h"
#include "dht11.h"
#include <PCD8544.h>
#include <util/delay.h>
//dccduino ->   gpio
// D0-7 = PD0-7
// D8-13 = PB0-5
// A0-5 -> PC0-7
// A6-7 = ADC6-7
//#define F_CPU 16000000UL

extern char zerg [];

gpio::Pin ledPin(gpio::B, 5);
gpio::PWMRCPin pwmPin(38);

gpio::Pin pd4(gpio::D, 4);
//irRemote::RCTransmitter transmitter(pd4);

uart::UART serial;

char str[60];
void onDecode (void* d)
{
   char* data = (char *)d;
   sprintf(str, "decoded: %02x%02x%02x\n", *(data+2), *(data+1), *data );
   serial << str;
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

void testDHT()
{
   sensors::DHT11 dht(gpio::C, 6);
   while (1)
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
      _delay_ms(5000);
   }
}

void testLCD()
{
   // sensors::DHT11 dht(gpio::B, 3);
   // gpio::Pin rst(gpio::D, 2);
   // gpio::Pin light(gpio::D, 3);
   // gpio::Pin ce(gpio::D, 4);
   // gpio::Pin dc(gpio::D, 5);
   // gpio::Pin din(gpio::D, 6);
   // gpio::Pin clk(gpio::D, 7);
   // lcd::PCD8544 lcd( rst, ce, dc, din, clk, light);
   // auto yoffset = 0;
   // while (1)
   // {
   //    lcd.GotoXY(20, yoffset++);
   //    lcd.DrawBitmap(zerg, 84*6);
   //    lcd.GotoXY(0,0);
   //    if (true || dht.ReadData())
   //    {
   //       auto hum = dht.GetHumidity();
   //       auto temp = dht.GetTemperature();
   //       sprintf(str, "Temp.: %d *C", temp);
   //       lcd.DrawLine(str);
   //       lcd.GotoXY(0,1);
   //       sprintf(str, "Humid.: %d %%", hum);
   //       lcd.DrawLine(str);
   //    }
   //    yoffset = yoffset > 5 ? 0 : yoffset;

   //    _delay_ms(2000);
   // }
}

#include <spi.h>

//#define MASTER
void testSPI()
{
   char bt = 12;
   char bt2 = 24;
#ifdef MASTER
   serial << "master\n";
   protocol::SPI spi(true);
   gpio::Pin cc(gpio::B, 2);
   cc = false;
   auto ret = spi.SendReceiveByte(&bt);
   cc = true;
   serial << "sent " << ret << "\n";
#else
   serial << "slave \n";
   protocol::SPI spi(false);
   while (1)
   {
      auto ret = spi.SendReceiveByte(&bt2);
      bt2++;
      serial << "received\n";
      serial << (int) ret << "\n";
   }
#endif
   while(1);
}

int main(void) {
   ir_decoder.SetDecodeCB(onDecode);
   serial << "Privet\n";
   // testDHT();
   //transmitter.StartTransmit(data);
   //setupTimer();
   //ledPin = true;
   //testLCD();
   testSPI();
   while (1)
   {
      ledPin = !pd4;
   }
   return 0;    
}