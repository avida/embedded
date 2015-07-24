#ifndef __LCD_H__
#define __LCD_H__
#include <stm32f10x.h>
#include "font.h"

extern u32 bgColor;
extern u32 fgColor;

extern u16 LCD_WIDTH;
extern u16 LCD_HEIGHT;
u16 GUI_Color565(u32 RGB);
void LCDInit(void);
void LCDClear(void);
void LCDSetBounds(u16 left, u16 top, u16 right, u16 bottom);
void LCDFillDisplay(void);
void DrawChar(u16 x, u16 y, u16 chr, GUI_FONT font);
void DrawString(u16 x, u16 y, char *str, GUI_FONT font);
void DrawStringInvert(u16 x, u16 y, char *str, GUI_FONT font);


#endif
