#include "GraphicsFunctions.h"
#include "FramebufferFunctions.h"
#include "font4x6.h"
#include <string.h>

unsigned char* currentFont = NULL;
unsigned int textwidth = 0; 
unsigned int textheigth = 0; 
unsigned int textStart = 32; 
uint8_t textColor = 7;

void cls(uint8_t col)
{
    for(uint32_t i = 0; i<320; i++)
    {
        for(uint32_t j = 0; j<240; j++)
        {
            plot_point (i, j, col);
        }
    }
}

void SetFont(unsigned char* newFont)
{
    currentFont = newFont;
    textwidth = newFont[0]; 
    textheigth = newFont[1]; 
    textStart = newFont[2]; 
}

void SetTextColor(uint8_t col)
{
    textColor = col;
}

void DrawRectfill(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t c)
{
    int32_t x1 = x;
    int32_t x2 = x + w;
    int32_t y1 = y;
    int32_t y2 = y + h;
    
    if (x2 < x1)
    {
        int32_t xt = x1;
        x1 = x2;
        x2 = xt;
    }
    
    if (y2 < y1)
    {
        int32_t yt = y1;
        y1 = y2;
        y2 = yt;
    }
    
    if (x1 < 0)
    {
        x1 = 0;
    }
    
    if (y1 < 0)
    {
        y1 = 0;
    }
    
    if (x2 > 320)
    {
        x2 = 320;
    }
    
    if (y2 > 240)
    {
        y2 = 240;
    }
    
    if (x1 < 320 || y1 < 240 || x2 > 0 || y2 > 0)
    {
        for(int32_t i = y1; i < y2; i++)
        {
            for(int32_t j = x1; j < x2; j++)
            {
                plot_point(j, i, c);
            }
        }
    }
}

static void DrawLetter(unsigned int fontIndex, int x, int y)
{
    unsigned int sHeigth = 6;

    unsigned int currentByte = font4x6[fontIndex];
    unsigned int currentByteIndex = fontIndex;

    for(unsigned int h = 0; h < sHeigth; h++)
    {
        for(unsigned int w = 0; w < 1; w++)
        {
            for(unsigned int b = 0; b < 8; b++)
            {
                if (currentByte<<b & 0x80)
                {
                    plot_point(x+b+(w*8), y+h, textColor);
                }
            }
            currentByteIndex++;
            currentByte = font4x6[currentByteIndex];
        }
    }
}

void DrawText(char* textString, int x, int y)
{
    size_t stringLength = strlen(textString);
    
    if (stringLength == 0)
    {
        return;
    }

    int currentX = x;
    int currentY = y;

    for(int i = 0; i < stringLength; i++)
    {
        if (textString[i] == '\n')
        {
            currentY += 6;
            currentX = x;
        }
        else
        {
            unsigned int currentFontIndex = textString[i] - 32;

            DrawLetter(3+(currentFontIndex*6), currentX, currentY);

            currentX += textwidth;
        }
    }
}