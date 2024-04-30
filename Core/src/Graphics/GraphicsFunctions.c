#include "GraphicsFunctions.h"
#include "../Framebuffer/FramebufferFunctions.h"
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

uint8_t GetTextCol()
{
    return textColor;
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

void DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t c)
{
    if (x1 == x2)
        for (int i = y1; i < y2; i++)
        {
            plot_point(x1, i, c);
        }
	else if (y1 == y2)
        for (int i = x1; i < x2; i++)
        {
            plot_point(i, y1, c);
        }
	else {
		int e;
		signed int dx,dy,j, temp;
		signed char s1,s2, xchange;
		signed int x,y;

		x = x1;
		y = y1;
	
		//take absolute value
		if (x2 < x1) {
			dx = x1 - x2;
			s1 = -1;
		}
		else if (x2 == x1) {
			dx = 0;
			s1 = 0;
		}
		else {
			dx = x2 - x1;
			s1 = 1;
		}

		if (y2 < y1) {
			dy = y1 - y2;
			s2 = -1;
		}
		else if (y2 == y1) {
			dy = 0;
			s2 = 0;
		}
		else {
			dy = y2 - y1;
			s2 = 1;
		}

		xchange = 0;   

		if (dy>dx) {
			temp = dx;
			dx = dy;
			dy = temp;
			xchange = 1;
		} 

		e = ((int)dy<<1) - dx;  
	 
		for (j=0; j<=dx; j++) {
			plot_point(x, y, c);
		 
			if (e>=0) {
				if (xchange==1) x = x + s1;
				else y = y + s2;
				e = e - ((int)dx<<1);
			}
			if (xchange==1)
				y = y + s2;
			else
				x = x + s1;
			e = e + ((int)dy<<1);
		}
	}
}

void DrawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t c)
{
    int32_t x1 = x;
    int32_t y1 = y;
    int32_t x2 = x + w;
    int32_t y2 = y + h;

    if (x < 320 && x2 >= 0)
    {
        if (y >= 0 && y < 240)
        {
            for(int i = x; i < x2; i++)
            {
                plot_point(i,y,c);
            }
        }
        if (y2 >= 0 && y2 < 240)
        {
            for(int i = x; i < x2; i++)
            {
                plot_point(i,y2,c);
            }
        }
    }

    if (y < 240 && y2 >= 0)
    {
        if (x >= 0 && x < 320)
        {
            for(int i = y+1; i < y2-1; i++)
            {
                plot_point(x,i,c);
            }
        }
        if (x2 >= 0 && x2 < 320)
        {
            for(int i = y+1; i < y2-1; i++)
            {
                plot_point(x2,i,c);
            }
        }
    }
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

void DrawTri(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint8_t c)
{
    DrawLine(x1,y1,x2,y2,c);
    DrawLine(x1,y1,x3,y3,c);
    DrawLine(x3,y3,x2,y2,c);
}

void DrawTrifill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint8_t c)
{

}

void DrawEllipse(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t c)
{

}

void DrawEllipsefill(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t c)
{

}

static void DrawLetter(unsigned int fontIndex, uint32_t x, uint32_t y)
{
    uint32_t sHeigth = 6;

    uint32_t currentByte = font4x6[fontIndex];
    uint32_t currentByteIndex = fontIndex;

    for(uint32_t h = 0; h < sHeigth; h++)
    {
        for(uint32_t w = 0; w < 1; w++)
        {
            for(uint32_t b = 0; b < 8; b++)
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

void DrawText(char* textString, uint32_t x, uint32_t y)
{
    size_t stringLength = strlen(textString);
    
    if (stringLength == 0)
    {
        return;
    }

    uint32_t currentX = x;
    uint32_t currentY = y;

    for(uint32_t i = 0; i < stringLength; i++)
    {
        if (textString[i] == '\n')
        {
            currentY += 6;
            currentX = x;
        }
        else
        {
            uint32_t currentFontIndex = textString[i] - 32;

            DrawLetter(3+(currentFontIndex*6), currentX, currentY);

            currentX += textwidth;
        }
    }
}

void DrawChar(char textString, uint32_t x, uint32_t y)
{
    uint32_t currentX = x;
    uint32_t currentY = y;


    uint32_t currentFontIndex = textString - 32;

    DrawLetter(3+(currentFontIndex*6), currentX, currentY);

    currentX += textwidth;
}