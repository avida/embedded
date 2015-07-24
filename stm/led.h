#ifndef __LED_H__
#define __LED_H__
#include <stm32f10x.h>

void setupBulbPorts(void);

void BulbOn(u16 bulb);
void BulbOff(u16 bulb);
void BulbSwitch(u16 bulb);

#endif
