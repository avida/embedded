#include <uart.h>
#include "utils.h"

extern uart::UART serial;

int main(void) {
   serial << "Hi this is simple app \n";
   while (true) {
      serial << "boop\n";
      utils::Delay_ms(3000);
   } 
}
