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
	//InitConsole();
	//LCDInit();
}

irRemote::DataDecoder ir_decoder;
gpio::PinInput *irpin_ptr;
uart::UART *serial_ptr;

// Interraption handlers section
extern "C"
{

void EXTI2_IRQHandler(void)
{
	EXTI->PR |=0x03;
}

void TIM6_IRQHandler(void)
{
	TIM6->SR &= ~TIM_SR_UIF;
	ir_decoder.ProcessSignal(*irpin_ptr);
}

}

void signalMatched(void *data)
{
	sprintf(str, "ir data: %x \r\n", *((uint32_t *) data) );
	*serial_ptr << str;
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
	ir_decoder.SetDecodeCB(signalMatched);

	Setup();
	//LCDSetBounds(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);
	//LCDClear();
	while(true)
	{
		led1 = irpin;
	}
}
