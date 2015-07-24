#ifndef __TIMER_H__
#define __TIMER_H__

#include <stm32f10x.h>                              // stm32f10x definitions

void Delay(u32 ticks);
void initTimer(void);

void initTIM6Timer();


#endif
