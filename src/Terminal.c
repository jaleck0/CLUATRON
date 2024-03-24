#include "Terminal.h"
#include "GraphicsFunctions.h"
#include <string.h>
#include <stdlib.h>

uint32_t textX = 0;
uint32_t textY = 0;
uint32_t backCol = 0;

void TerminalSetPosition(uint32_t x, uint32_t y)
{
    textX = x;
    textY = y;
}

void TerminalSetBackCol(uint8_t col)
{
    backCol = col;
}

void TerminalClear()
{
    cls(backCol);
    textX = 0;
    textY = 0;
}

void DrawWritePoint()
{
    DrawRectfill(textX, textY, 1, 5, GetTextCol());
}

void RemoveWritePoint()
{
    DrawRectfill(textX, textY, 1, 5, backCol);
}

void TerminalPutCharacter(char textChar)
{
    DrawRectfill(textX, textY, 4, 6, backCol);
    if (textChar == '\n')
    {
        textX = 0;
        textY += 6;
    }
    else if(textChar >= ' ')
    {
        DrawChar(textChar, textX, textY);
        textX += 4;
    }
    
    if (textX > (320-4))
    {
        textY += 6;
        textX = 0;
    }

    if (textY > (240-6))
    {
        TerminalClear();
    }
}

void TerminalPutString(char* textString)
{
    uint32_t length = strlen(textString);

    for(uint32_t i = 0; i < length; i++)
    {
        TerminalPutCharacter(textString[i]);
    }
}

extern void TerminalPutNumber(int64_t num)
{
    char snum[64];
    itoa(num, snum, 10);
    TerminalPutString(snum);
}