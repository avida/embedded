#include <stm32f10x.h>
#include "key.h"

void setuKeyPorts()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

u16 translateKey(u16 key)
{
	switch (key)
	{
		case 0:
			return GPIO_Pin_2;
		case 1:
			return GPIO_Pin_3;
		case 2:
			return GPIO_Pin_4;
		case 3:
			return GPIO_Pin_5;
		case 4:
			return GPIO_Pin_6;
	};
	return 0;
}

u16 isKeyPressed(u16 key)
{
	u16 pin = translateKey(key);
	return !(GPIOE->IDR & pin);
}

short keyPressed()
{
	for (int i = 0; i < 4; ++i)
	{
		if (isKeyPressed(i))
		{
			return i;
		}
	}
	return -1;
}
