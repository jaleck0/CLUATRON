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

extern void SetTextColor(uint8_t col);

extern void DrawRectfill(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t c);

extern void DrawText(char* textString, int x, int y);

#endif