#include <uart.h>

extern uart::UART serial;
int main(void) {
   serial << "Hi this is simple app\n";
}
