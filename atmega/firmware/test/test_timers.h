#include "utils.h"

// int cntr = 0;
// ISR (TIMER0_COMPA_vect)  
// {
//   // serial << "tick\n";
//   // TCNT0=0;
//    cntr++;
// }

void setupTimer()
{
   // set timer to CTC mode
   TCCR0A |= _BV(WGM01);
   // set prescaler to 8x
   TCCR0B |= _BV(CS02);
   
   // 40 ticks = 20 us on 2Mhz timer
   OCR0A = 100;
   // enable interrupts
   TIMSK0 |= _BV(OCIE0A);
   sei();
}

#define PERF_TEST_BEGIN \
   TCCR0B |= _BV(CS00); \
   TCNT0 = 0; 

#define PERF_TEST_END \
   serial << "performance test: " << TCNT0 << "\n"; 



void perf_test()
{
   PERF_TEST_BEGIN
   // *led.m_port = _BV(5);
   // led = true;
   gpio::atmega::Pin led (gpio::B, 5);
   // utils::Delay_us(2);
   PERF_TEST_END   
}

enum Figures
{
   BlackPawn = 'p',
   BlackRook = 'r',
   BlackKnight = 'n',
   BlackBishop = 'b',
   BlackQueen = 'q',
   BlackKing = 'k',
   WhitePawn = 'P',
   WhiteRook = 'R',
   WhiteKnight = 'N',
   WhiteBishop = 'B',
   WhiteQueen = 'Q',
   WhiteKing = 'K'
};

enum FiguresCodes
{
   NoneCode = 0,
   WhitePawnCode, // 1
   WhiteRookCode, // 2
   WhiteKnightCode, // 3
   WhiteBishopCode, // 4
   WhiteQueenCode, // 5
   WhiteKingCode, // 6
   BlackPawnCode, // 7
   BlackRookCode, // 8
   BlackKnightCode, //9
   BlackBishopCode, //10
   BlackQueenCode, //11
   BlackKingCode //12
};

union BoardEntry
{
   struct
   {
      uint8_t code : 4;
      uint8_t addr : 3;
      uint8_t zero : 1;
   };
   char val;
};

void test_main()
{
   // setupTimer();
   // TCCR0A |= _BV(WGM01);
   // utils::CountMillis();
   BoardEntry b;
   b.code = 8;
   b.addr = 0x19 & 0x07;
   b.zero = 0;
   serial << (uint8_t)b.val <<'\n';
   // utils::Delay_ms(20); 
   // serial << utils::GetTimeValue() << "\n";
   // // perf_test()
   return;
   while(true);
}