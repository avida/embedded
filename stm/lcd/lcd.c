#include "../lcd/lcd.h"
#include "../utils/timer.h"
#include "ili9030reg.h"
#include <stm32f10x_fsmc.h>

u16 LCD_WIDTH = 320;
u16 LCD_HEIGHT = 240;
u32 bgColor;
u32 fgColor;

#define LCDRegister 	(*((volatile u16*) 0x60000000))
#define LCDMemory		(*((volatile u16*) 0x60020000))

void LCD_WR_CMD(unsigned int index,unsigned int val)
{	
	LCDRegister = index;	
	LCDMemory = val;
}

#define nCS     GPIO_Pin_0
#define RS      GPIO_Pin_1
#define nWR     GPIO_Pin_2
#define nRD     GPIO_Pin_3
#define nReset  GPIO_Pin_4

#define BacklightPin 					GPIO_Pin_13
#define BacklightPort					GPIOD

#define ResetPin 					GPIO_Pin_1
#define ResetPort					GPIOE

void FSMC_LCD_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    FSMC_NORSRAMInitTypeDef  		FSMC_InitStructure;
    FSMC_NORSRAMTimingInitTypeDef  	FSMC_Timing;


    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_InitStructure.GPIO_Pin = 	GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                    GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10|
                                    GPIO_Pin_11| GPIO_Pin_14| GPIO_Pin_15; //Interface
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                  GPIO_Pin_11| GPIO_Pin_12| GPIO_Pin_13| GPIO_Pin_14 |
                                  GPIO_Pin_15;	//Interface
    GPIO_Init(GPIOE, &GPIO_InitStructure);

	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	//Backlight
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ResetPin; 	//Reset
    GPIO_Init(ResetPort, &GPIO_InitStructure);

    GPIO_SetBits(ResetPort,ResetPin);

    FSMC_Timing.FSMC_AddressSetupTime = 1;
    FSMC_Timing.FSMC_AddressHoldTime = 0;
    FSMC_Timing.FSMC_DataSetupTime = 5;
    FSMC_Timing.FSMC_BusTurnAroundDuration = 0;
    FSMC_Timing.FSMC_CLKDivision = 0;
    FSMC_Timing.FSMC_DataLatency = 0;
    FSMC_Timing.FSMC_AccessMode = FSMC_AccessMode_B;

    FSMC_InitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_InitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_InitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
    FSMC_InitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_InitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_InitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_InitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_InitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_InitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_InitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_InitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_InitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_InitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_InitStructure.FSMC_ReadWriteTimingStruct = &FSMC_Timing;
    FSMC_InitStructure.FSMC_WriteTimingStruct = &FSMC_Timing;

    FSMC_NORSRAMInit(&FSMC_InitStructure);
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

void LCDHardwareReset()
{
    GPIO_ResetBits(ResetPort,ResetPin);
    Delay(50);
    GPIO_SetBits(ResetPort,ResetPin);
    Delay(10);
}

void LCDInit(void) {
	//lcd_rst();	 
	FSMC_LCD_Init();
    LCDHardwareReset();
	 
	//############# void Power_Set(void) ################//
	LCD_WR_CMD(0x0000,0x0001);
	Delay(50);
	
	LCD_WR_CMD(0x0015,0x0030);
	LCD_WR_CMD(0x0011,0x0040);
	LCD_WR_CMD(0x0010,0x1628);
	LCD_WR_CMD(0x0012,0x0000);
	LCD_WR_CMD(0x0013,0x104d);
	Delay(50);
	LCD_WR_CMD(0x0012,0x0010);
	Delay(50);
	LCD_WR_CMD(0x0010,0x2620);
	LCD_WR_CMD(0x0013,0x344d); //304d
	Delay(50);
	
	LCD_WR_CMD(0x0001,0x0100);
	LCD_WR_CMD(0x0002,0x0300);
	LCD_WR_CMD(0x0003,0x1030);
	LCD_WR_CMD(0x0008,0x0604);
	LCD_WR_CMD(0x0009,0x0000);
	LCD_WR_CMD(0x000A,0x0008);
	
	LCD_WR_CMD(0x0041,0x0002);
	LCD_WR_CMD(0x0060,0x2700);
	LCD_WR_CMD(0x0061,0x0001);
	LCD_WR_CMD(0x0090,0x0182);
	LCD_WR_CMD(0x0093,0x0001);
	LCD_WR_CMD(0x00a3,0x0010);
	Delay(50);
	
	//################# void Gamma_Set(void) ####################//
	 LCD_WR_CMD(0x30,0x0000);		
	 LCD_WR_CMD(0x31,0x0502);		
	 LCD_WR_CMD(0x32,0x0307);		
	 LCD_WR_CMD(0x33,0x0305);		
	 LCD_WR_CMD(0x34,0x0004);		
	 LCD_WR_CMD(0x35,0x0402);		
	 LCD_WR_CMD(0x36,0x0707);		
	 LCD_WR_CMD(0x37,0x0503);		
	 LCD_WR_CMD(0x38,0x1505);		
	 LCD_WR_CMD(0x39,0x1505);
	 Delay(50);
	
	//################## void Display_ON(void) ####################//
	 LCD_WR_CMD(0x0007,0x0001);
	 Delay(50);
	 LCD_WR_CMD(0x0007,0x0021);
	 LCD_WR_CMD(0x0007,0x0023);
	 Delay(50);
	 LCD_WR_CMD(0x0007,0x0033);
	 Delay(50);
	 LCD_WR_CMD(0x0007,0x0133);
	 bgColor = GUI_Color565(0x0e2f1c);
	 fgColor = GUI_Color565(0x1e964c);

}  

u16 GUI_Color565(u32 RGB)
{
  u8  r, g, b;

  b = ( RGB >> (0+3) ) & 0x1f;		
  g = ( RGB >> (8+2) ) & 0x3f;		
  r = ( RGB >> (16+3)) & 0x1f;		
   
  return( (r<<11) + (g<<5) + (b<<0) );		
}
void LCDSetBounds(u16 left, u16 top, u16 right, u16 bottom)
 {
    LCD_WR_CMD(CTR_VERT_START, left);
    LCD_WR_CMD(CTR_VERT_END, right);
    LCD_WR_CMD(CTR_HORZ_START, top);
    LCD_WR_CMD(CTR_HORZ_END, bottom);

    LCD_WR_CMD(CTR_HORZ_ADDRESS, top);
    LCD_WR_CMD(CTR_VERT_ADDRESS, left);
 }

void LCDSetCursor(u16 left, u16 top)
{
	LCD_WR_CMD(CTR_HORZ_ADDRESS, top);
    LCD_WR_CMD(CTR_VERT_ADDRESS, left);
}

void LCDClear(void)
{
	int i;
	LCDRegister = CTR_WRITE_DATA;
	for(i=0;i<LCD_WIDTH*LCD_HEIGHT;i++)
	{
		LCDMemory = bgColor; 	
	}
}

void LCDFillDisplay()
{
	int i;
	LCDRegister = CTR_WRITE_DATA;
	for(i=0;i<30*30;i++)
	{
		LCDMemory = 0x00ff00; 	
	}
}

void DrawStringInvert(u16 x, u16 y, char *str, GUI_FONT font)
{
 	u32 tmp = fgColor;
 	fgColor = bgColor;
	DrawString(x, y, str, font);	
	fgColor = tmp;
}

void DrawChar(u16 x, u16 y, u16 chr, GUI_FONT font)
{
	int chr_x,chr_y;
	char mask;
	char BytesPerLine;
	chr = chr - ((GUI_FONT_PROP *)font.p.pFontData)->First;
	BytesPerLine = ((GUI_FONT_PROP *)font.p.pFontData)->paCharInfo[chr].BytesPerLine;
	//LCDSetBounds(x, y, font.YSize, ((GUI_FONT_PROP *)font.p.pFontData)->paCharInfo[chr].XSize);
    
	for (chr_y = 0; chr_y < font.YSize; chr_y++)
	{
	for (chr_x = 0; chr_x < ((GUI_FONT_PROP *)font.p.pFontData)->paCharInfo[chr].XSize; chr_x++)
	{
		LCDSetCursor( x + ((GUI_FONT_PROP *)font.p.pFontData)->paCharInfo[chr].XSize -  chr_x , y+chr_y);
		if (chr_x >= 8)
		{
			mask = ((GUI_FONT_PROP *)font.p.pFontData)->paCharInfo[chr].pData[ BytesPerLine * chr_y + 1];
		}
		else
		{
			mask = ((GUI_FONT_PROP *)font.p.pFontData)->paCharInfo[chr].pData[ BytesPerLine * chr_y];
		}
		mask = mask << (chr_x >= 8 ? chr_x - 8 : chr_x);
 	 	mask = mask >> 7;
		LCDRegister = CTR_WRITE_DATA;
		if (mask)
		{
			LCDMemory = fgColor;
		}
	}
	}
}
u16 getCharLen(char chr,GUI_FONT font)
{
	if (!chr)
		return 0;
	chr = chr - ((GUI_FONT_PROP *)font.p.pFontData)->First;
	return ((GUI_FONT_PROP *)font.p.pFontData)->paCharInfo[chr].XSize;
}

void DrawString(u16 x, u16 y, char *str, GUI_FONT font)
{
	while(*str)
	{
		DrawChar(x, y, *str, font);
		x-=getCharLen(*(str+1), font);
		str++;
	}
}
