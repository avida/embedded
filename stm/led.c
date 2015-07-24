#include "led.h"

void setupBulbPorts(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_13 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void BulbOn(u16 bulb)
{
	switch (bulb)
	{
		case 0:
			GPIOC->BSRR = 1<<6;
			break;
		case 1:
			GPIOC->BSRR = 1<<7;
			break;
		case 2:
			GPIOD->BSRR = 1<<13;
			break;
		case 3:
			GPIOD->BSRR = 1<<6;
			break;
	}
}

void BulbOff(u16 bulb)
{
	switch (bulb)
	{
		case 0:
			GPIOC->BRR = 1<<6;
			break;
		case 1:
			GPIOC->BRR = 1<<7;	
			break;
		case 2:
			GPIOD->BRR = 1<<13;
			break;
		case 3:
			GPIOD->BRR = 1<<6;
			break;
	}
	
}
void BulbSwitch(u16 bulb)
{
}
