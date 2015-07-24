#include "console.h"
#include "../lcd/lcd.h"
#include <string.h>
#include <stdlib.h>

#define LINES 10
#define CHARS 80

char *console_ptr[LINES];
char lines[LINES][CHARS+1];

unsigned char InitConsole()
{
	int i;
	for (i = 0; i < LINES; i++ )
	{
		console_ptr[i] = &lines[i][0];
		if (!console_ptr[i])
		{
			return 0;
		}
	}
	return 1;
}
void PrintStr(char *str)
{
	int i;
	char * upperstr;
	EraseConsole();
	upperstr = console_ptr[0];

	for(i = 0; i < LINES-1; ++i)
	{
		console_ptr[i] = console_ptr[i+1];
	}
	console_ptr[LINES-1] = upperstr;
	strcpy(console_ptr[LINES-1], str);
	DrawConsole();

}
void EraseConsole()
{
	int i;
	for (i = 0; i < LINES; ++i)
	{
		DrawStringInvert(300, i* GUI_FontComic18B_ASCII.YSize, console_ptr[i], GUI_FontComic18B_ASCII);
	}
}
void DrawConsole()
{
	int i;
	for (i = 0; i < LINES; ++i)
	{
		DrawString(300, i* GUI_FontComic18B_ASCII.YSize, console_ptr[i], GUI_FontComic18B_ASCII);
	}
}
