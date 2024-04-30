#ifndef GRAPHICSF_H
#define GRAPHICSF_H

#include "pico.h"
#include "pico/stdlib.h"
#include "pico/scanvideo.h"
#include "pico/scanvideo/composable_scanline.h"
#include "pico/multicore.h"
#include "hardware/clocks.h"

extern void cls(uint8_t col);

extern void SetFont(unsigned char* newFont);

extern uint8_t GetTextCol();

extern void SetTextColor(uint8_t col);

extern void DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t c);

extern void DrawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t c);

extern void DrawRectfill(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t c);

extern void DrawTri(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint8_t c);

extern void DrawTrifill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint8_t c);

extern void DrawEllipse(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t c);

extern void DrawEllipsefill(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t c);

extern void DrawText(char* textString, uint32_t x, uint32_t y);

extern void DrawChar(char textString, uint32_t x, uint32_t y);

#endif