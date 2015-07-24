#include "timer.h"
#include <stm32f10x.h>                              // stm32f10x definitions
#include <misc.h>
#include <stm32f10x_rcc.h>

#define SysTick_Counter_Disable        ((u32)0xFFFFFFFE)
#define SysTick_Counter_Enable         ((u32)0x00000001)
#define SysTick_Counter_Clear          ((u32)0x00000000)

#define CTRL_TICKINT_Set      ((u32)0x00000002)
#define CTRL_TICKINT_Reset    ((u32)0xFFFFFFFD)

volatile u32 tick;
void SysTick_Handler()
{
	if (tick != 0)
		tick -=1;
}

void SysTick_CounterCmd(u32);

void Delay(u32 ticks)
{
	SysTick_CounterCmd(SysTick_Counter_Enable); 
	tick = ticks;
	while(tick);
	SysTick_CounterCmd(SysTick_Counter_Disable);   
    /* Clear SysTick Counter */   
    SysTick_CounterCmd(SysTick_Counter_Clear);
}

void SysTick_CounterCmd(u32 SysTick_Counter)
{
  /* Check the parameters */
  assert_param(IS_SYSTICK_COUNTER(SysTick_Counter));

  if (SysTick_Counter == SysTick_Counter_Enable)
  {
    SysTick->CTRL |= SysTick_Counter_Enable;
  }
  else if (SysTick_Counter == SysTick_Counter_Disable)
  {
    SysTick->CTRL &= SysTick_Counter_Disable;
  }
  else /* SysTick_Counter == SysTick_Counter_Clear */
  {
    SysTick->VAL = SysTick_Counter_Clear;
  }
}


void SysTick_ITConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    SysTick->CTRL |= CTRL_TICKINT_Set;
  }
  else
  {
    SysTick->CTRL &= CTRL_TICKINT_Reset;
  }
}

void initTimer(void)
{
	SysTick_CounterCmd(SysTick_Counter_Disable);   
     
    SysTick_ITConfig(DISABLE);   
     
	SysTick->LOAD = 9000;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	SysTick_ITConfig(ENABLE);
}

void initTIM6Timer()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	//TIM6->PSC = 24000;
	TIM6->ARR = 480; //generate interrupt every 20 us
	TIM6->DIER |= TIM_DIER_UIE;
	TIM6->CR1 |= TIM_CR1_CEN;
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
}
