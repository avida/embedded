#include "led.h"
#include "key.h"
#include "utils/timer.h"
#include "lcd/lcd.h"
#include "console/console.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "usart.h"
#include "rc_decoder/rc_decoder.h"

extern int _ebss;

char str[80];
void Blink()
{
	int on = 0;
	while(1)
	{
		Delay(1000);
		if (on)
		{
			BulbOn(3);
		}
		else
		{
			BulbOff(3);
		}
		on = !on;
	}
}

void SetupNVIC()
{
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_AFIOEN , ENABLE);
	AFIO->EXTICR[0]|=AFIO_EXTICR1_EXTI2_PE;
	EXTI->IMR|=( EXTI_IMR_MR2);
	EXTI->RTSR|=( EXTI_RTSR_TR2);
	NVIC_EnableIRQ (EXTI2_IRQn);
}

void Setup()
{
	setupBulbPorts();
	setuKeyPorts();
	initTimer();
	SetupNVIC();

	initTIM6Timer();
	//if (InitConsole())
	{
		//Blink();
	}
	//LCDInit();


}

PulseDecoder ir_decoder;
PulseProcessor rc_processor;

extern "C"
{
void EXTI2_IRQHandler(void)
{
	BulbOn(2);
	EXTI->PR |=0x03;
}

void TIM6_IRQHandler(void)
{
	TIM6->SR &= ~TIM_SR_UIF;
	ProcessPulse(&rc_processor, isKeyPressed(1));
}
}

void signalMatched(uint32_t *data)
{
	sprintf(str, "ir data: %x \r\n", *data );
	sendString(str);
}

void printPulses(struct IR_Pulses_struct *pc)
{
	/*
	for (int i = 0; i < max_pulse; i++)
	{
		sprintf(str, "pulse %d on %d off %d \r\n", i, pc->pulses[i][0], pc->pulses[i][1]);
		sendString(str);
	}
	*/
}


int main(void)
{
	//ir_decoder.matched_cb = signalMatched;
	rc_processor.decoder = &ir_decoder;
	InitializeUSART();
	sendString("hello\n");
	Setup();
	//LCDSetBounds(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);
	//LCDClear();
	//PrintStr("Lets go");
	//sprintf(str, "_ebss %x", &_ebss);
	//PrintStr(str);
	while(true)
	{
		if(isKeyPressed(1))
		{
			BulbOn(1);
		}
		else
		{
			BulbOff(1);
		}
	}
}
