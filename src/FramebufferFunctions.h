#ifndef FRAMEB_F_H
#define FRAMEB_F_H

#include "pico.h"
#include "pico/stdlib.h"
#include "pico/scanvideo.h"
#include "pico/scanvideo/composable_scanline.h"
#include "pico/multicore.h"
#include "hardware/clocks.h"

#define WIDTH          320
#define HEIGHT         240
#define COLOR_COUNT    16

extern uint16_t colours[COLOR_COUNT];
extern uint32_t dblpal[COLOR_COUNT * COLOR_COUNT];
extern uint8_t fbuf[WIDTH * HEIGHT / 2];

extern void setup_video(void);

extern void set_colours();

extern void ChangeColor(uint8_t index, uint16_t newCol);

extern void plot_point(int x, int y, int clr);

extern void __time_critical_func(render_loop)(void);

#endif