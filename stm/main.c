#include "led.h"
#include "key.h"
#include "utils/timer.h"
#include "lcd/lcd.h"
#include "console/console.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <uart.h>
#include <rc_decoder/rc_decoder.h>
#include <gpio.h>

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
gpio::PinInput *irpin_ptr;
uart::UART *serial_ptr;

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
	ProcessPulse(&rc_processor, *irpin_ptr);
}
}

void signalMatched(void *data)
{
	sprintf(str, "ir data: %x \r\n", *((uint32_t *) data) );
	*serial_ptr << str;
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
	// led pins c6,7 d13,6
	gpio::PinOutput led1(gpio::C, 7);
	// key pin E2-5
	gpio::PinInput irpin(gpio::E, 3);
	irpin_ptr = &irpin;
	uart::UART serial;
	serial_ptr = &serial;
	serial << "hehelo\n";
	ir_decoder.matched_cb = signalMatched;
	rc_processor.decoder = &ir_decoder;

	Setup();
	//LCDSetBounds(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);
	//LCDClear();
	while(true)
	{
		led1 = irpin;
	}
}
