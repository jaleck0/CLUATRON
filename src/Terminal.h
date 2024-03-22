#ifndef TERMINAL_H
#define TERMINAL_H
#include "GraphicsFunctions.h"

extern void TerminalSetPosition(uint32_t x, uint32_t y);

extern void TerminalSetBackCol(uint8_t col);

extern void TerminalClear();

extern void TerminalPutCharacter(char textChar);

extern void TerminalPutString(char* textString);

#endif