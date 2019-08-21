#include <uart.h>
#include "atmega_utils.h"
#include <utils.h>
#include "atmega_pin.h"
#include <util/delay.h>

gpio::atmega::Pin a(gpio::D, 4);
gpio::atmega::Pin b(gpio::D, 5);
gpio::atmega::Pin c(gpio::D, 6);
gpio::atmega::Pin d(gpio::D, 7);
gpio::atmega::Pin led(gpio::B, 5);

gpio::atmega::Pin* pins[4] = {&a,&b,&c,&d};


const auto fullstep_mask = 0x0c;
const auto max_counter = 3;
const auto start_counter = 0;

inline void ApplytoPins(const char& mask)
{
   for (auto i = 0; i <= 3; ++i)
   {
      *(pins[i]) = (mask >> i) & 1;
   }
}

void test_main()
{
   int counter = start_counter;
   auto mask = fullstep_mask;
   while(true)
   {
      mask = fullstep_mask >> counter;
      if (counter == max_counter)
      {
         mask |= (1<<3);
         counter = 0;
      }
      else
      {
         counter++;
         if (counter < 0)
            counter = max_counter;
      }
      ApplytoPins(mask);
      utils::Delay_ms(3);
   }
}

gpio::atmega::Pin servo(gpio::B, 0);
void sendPulse(bool up){
   for(auto i = 0; i < 25; i++) {
      servo = 1;
      if (up){
         utils::Delay_ms(2);
         utils::Delay_us(50);
         servo = 0;
         utils::Delay_ms(18);
      } else {
         utils::Delay_ms(0);
         servo = 0;
         utils::Delay_ms(20);
      }
   }

}
extern uart::UART serial;
int main(void) {

   servo = 0;
   serial << "Hi this is readadc app\n";
   //test_main();
   while(true) {
      sendPulse(true);
      utils::Delay_ms(1000);
      sendPulse(false);
      utils::Delay_ms(1000);
   }
   InitADC();
   while(1) {
      for (int i =0; i < 1; ++i)
      {
         serial << "channel "<< i <<": "<< ReadADC(i) << "\n";
      }
      utils::Delay_ms(1000);
   }
}
