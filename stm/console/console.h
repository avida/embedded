#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "../lcd/font.h"

unsigned char InitConsole(void);
void PrintStr(char *str);
void EraseConsole(void);
void DrawConsole(void);

#endif
