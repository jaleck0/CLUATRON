/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico.h"
#include "pico/scanvideo.h"
#include "pico/sync.h"
#include "pico/scanvideo/composable_scanline.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/time.h"

#if PICO_ON_DEVICE

#include "hardware/clocks.h"
#include "hardware/vreg.h"

#endif

CU_REGISTER_DEBUG_PINS(generation)

#define vga_mode vga_mode_320x240_60

//#define USE_FLOAT 1

#if USE_FLOAT
    //typedef float fixed;
    typedef double fixed;
    static inline fixed float_to_fixed(float x) {
        return x;
    }
    static inline fixed fixed_mult(fixed a, fixed b) {
        return a*b;
    }
#else
    #define FRAC_BITS 25u
    typedef int32_t fixed;

    static inline fixed float_to_fixed(float x) {
        return (fixed) (x * (float) (1u << FRAC_BITS));
    }

    #if !PICO_ON_DEVICE || (FRAC_BITS != 25)
        static inline fixed fixed_mult(fixed a, fixed b) {
            int64_t r = ((int64_t) a) * b;
            return (int32_t) (r >> FRAC_BITS);
        }
    #else
        // Since we're trying to go fast, do a better multiply of 32x32 preserving the bits we want
        static inline fixed fixed_mult(fixed a, fixed b) {
            uint32_t tmp1, tmp2, tmp3;
            __asm__ volatile (
            ".syntax unified\n"
            "asrs   %[r_tmp1], %[r_b], #16 \n" // r_tmp1 = BH
            "uxth   %[r_tmp2], %[r_a]      \n" // r_tmp2 = AL
            "muls   %[r_tmp2], %[r_tmp1]   \n" // r_tmp2 = BH * AL
            "asrs   %[r_tmp3], %[r_a], #16 \n" // r_tmp3 = AH
            "muls   %[r_tmp1], %[r_tmp3]   \n" // r_tmp1 = BH * AH
            "uxth   %[r_b], %[r_b]         \n" // r_b = BL
            "uxth   %[r_a], %[r_a]         \n" // r_a = AL
            "muls   %[r_a], %[r_b]         \n" // r_a = AL * BL
            "muls   %[r_b], %[r_tmp3]      \n" // r_b = BL * AH
            "add    %[r_b], %[r_tmp2]      \n" // r_b = BL * AH + BH * AL
            "lsls   %[r_tmp1], #32 - 25    \n" // r_tmp1 = (BH * AH) >> (32 - FRAC_BITS)
            "lsrs   %[r_a], #16            \n" // r_a = (AL & BL) H
            "add    %[r_a], %[r_b]         \n"
            "asrs   %[r_a], #25- 16        \n" // r_a = (BL * AH + BH * AL) H | (AL & BL) H >> (32 - FRAC_BITS)
            "add    %[r_a], %[r_tmp1]      \n"
            : [r_a] "+l" (a), [r_b] "+l" (b), [r_tmp1] "=&l" (tmp1), [r_tmp2] "=&l" (tmp2), [r_tmp3] "=&l" (tmp3)
            :
            );
            return a;
        }
    #endif

#endif

#define max_iters 127//255

struct mutex frame_logic_mutex;
//static void frame_update_logic();

void fill_scanline_buffer(struct scanvideo_scanline_buffer *buffer);

unsigned char* currentFont = NULL;
unsigned int textwidth = 0; 
unsigned int textheigth = 0; 
unsigned int textStart = 32; 


static uint16_t framebuffer[320 * 240];
//static uint16_t *framebuffer;

uint32_t lastcheck = 0;
uint32_t delay = 8;

static uint16_t colors[16] = {
        PICO_SCANVIDEO_PIXEL_FROM_RGB8(66, 30, 15),
        PICO_SCANVIDEO_PIXEL_FROM_RGB8(25, 7, 26),
        PICO_SCANVIDEO_PIXEL_FROM_RGB8(9, 1, 47),
        PICO_SCANVIDEO_PIXEL_FROM_RGB8(4, 4, 73),
        PICO_SCANVIDEO_PIXEL_FROM_RGB8(0, 7, 100),
        PICO_SCANVIDEO_PIXEL_FROM_RGB8(12, 44, 138),
        PICO_SCANVIDEO_PIXEL_FROM_RGB8(24, 82, 177),
        PICO_SCANVIDEO_PIXEL_FROM_RGB8(57, 125, 209),
        PICO_SCANVIDEO_PIXEL_FROM_RGB8(134, 181, 229),
        PICO_SCANVIDEO_PIXEL_FROM_RGB8(211, 236, 248),
        PICO_SCANVIDEO_PIXEL_FROM_RGB8(241, 233, 191),
        PICO_SCANVIDEO_PIXEL_FROM_RGB8(248, 201, 95),
        PICO_SCANVIDEO_PIXEL_FROM_RGB8(255, 170, 0),
        PICO_SCANVIDEO_PIXEL_FROM_RGB8(204, 128, 0),
        PICO_SCANVIDEO_PIXEL_FROM_RGB8(153, 87, 0),
        PICO_SCANVIDEO_PIXEL_FROM_RGB8(106, 52, 3),
};

unsigned char font4x6[] = {
4,6,32,
//space
0b00000000,
0b00000000,
0b00000000,
0b00000000,
0b00000000,
0b00000000,
//!
0b01000000,
0b01000000,
0b01000000,
0b00000000,
0b01000000,
0b00000000,
//"
0b10100000,
0b10100000,
0b00000000,
0b00000000,
0b00000000,
0b00000000,
//#
0b10100000,
0b11100000,
0b10100000,
0b11100000,
0b10100000,
0b00000000,
//$
0b01000000,
0b01100000,
0b11000000,
0b01100000,
0b11000000,
0b00000000,
//%
0b10100000,
0b00100000,
0b01000000,
0b10000000,
0b10100000,
0b00000000,
//&
0b00100000,
0b01000000,
0b11000000,
0b10100000,
0b11100000,
0b00000000,
//'
0b10000000,
0b10000000,
0b00000000,
0b00000000,
0b00000000,
0b00000000,
//(
0b01000000,
0b10000000,
0b10000000,
0b10000000,
0b01000000,
0b00000000,
//)
0b10000000,
0b01000000,
0b01000000,
0b01000000,
0b10000000,
0b00000000,
//*
0b01000000,
0b10100000,
0b01000000,
0b00000000,
0b00000000,
0b00000000,
//+
0b00000000,
0b01000000,
0b11100000,
0b01000000,
0b00000000,
0b00000000,
//,
0b00000000,
0b00000000,
0b00000000,
0b10000000,
0b10000000,
0b00000000,
//-
0b00000000,
0b00000000,
0b11100000,
0b00000000,
0b00000000,
0b00000000,
//.
0b00000000,
0b00000000,
0b00000000,
0b00000000,
0b01000000,
0b00000000,
// /
0b00100000,
0b00100000,
0b01000000,
0b10000000,
0b10000000,
0b00000000,
//0
0b11100000,
0b10100000,
0b10100000,
0b10100000,
0b11100000,
0b00000000,
//1
0b11000000,
0b01000000,
0b01000000,
0b01000000,
0b11100000,
0b00000000,
//2
0b11100000,
0b00100000,
0b11100000,
0b10000000,
0b11100000,
0b00000000,
//3
0b11100000,
0b00100000,
0b01100000,
0b00100000,
0b11100000,
0b00000000,
//4
0b10100000,
0b10100000,
0b11100000,
0b00100000,
0b00100000,
0b00000000,
//5
0b11100000,
0b10000000,
0b11100000,
0b00100000,
0b11100000,
0b00000000,
//6
0b10000000,
0b10000000,
0b11100000,
0b10100000,
0b11100000,
0b00000000,
//7
0b11100000,
0b00100000,
0b00100000,
0b00100000,
0b00100000,
0b00000000,
//8
0b11100000,
0b10100000,
0b11100000,
0b10100000,
0b11100000,
0b00000000,
//9
0b11100000,
0b10100000,
0b11100000,
0b00100000,
0b00100000,
0b00000000,
//:
0b00000000,
0b01000000,
0b00000000,
0b01000000,
0b00000000,
0b00000000,
//;
0b00000000,
0b01000000,
0b00000000,
0b01000000,
0b10000000,
0b00000000,
//<
0b00100000,
0b01000000,
0b10000000,
0b01000000,
0b00100000,
0b00000000,
//=
0b00000000,
0b11100000,
0b00000000,
0b11100000,
0b00000000,
0b00000000,
//>
0b10000000,
0b01000000,
0b00100000,
0b01000000,
0b10000000,
0b00000000,
//?
0b11000000,
0b00100000,
0b01000000,
0b00000000,
0b01000000,
0b00000000,
//@
0b11100000,
0b10100000,
0b10100000,
0b11100000,
0b11100000,
0b00000000,
//A
0b11100000,
0b10100000,
0b11100000,
0b10100000,
0b10100000,
0b00000000,
//B
0b11100000,
0b10100000,
0b11000000,
0b10100000,
0b11100000,
0b00000000,
//C
0b01100000,
0b10000000,
0b10000000,
0b10000000,
0b01100000,
0b00000000,
//D
0b11000000,
0b10100000,
0b10100000,
0b10100000,
0b11100000,
0b00000000,
//E
0b11100000,
0b10000000,
0b11000000,
0b10000000,
0b11100000,
0b00000000,
//F
0b11100000,
0b10000000,
0b11000000,
0b10000000,
0b10000000,
0b00000000,
//G
0b01100000,
0b10000000,
0b10000000,
0b10100000,
0b11100000,
0b00000000,
//H
0b10100000,
0b10100000,
0b11100000,
0b10100000,
0b10100000,
0b00000000,
//I
0b11100000,
0b01000000,
0b01000000,
0b01000000,
0b11100000,
0b00000000,
//J
0b11100000,
0b01000000,
0b01000000,
0b01000000,
0b11000000,
0b00000000,
//K
0b10100000,
0b10100000,
0b11000000,
0b10100000,
0b10100000,
0b00000000,
//L
0b10000000,
0b10000000,
0b10000000,
0b10000000,
0b11100000,
0b00000000,
//M
0b10100000,
0b11100000,
0b11100000,
0b10100000,
0b10100000,
0b00000000,
//N
0b11000000,
0b10100000,
0b10100000,
0b10100000,
0b10100000,
0b00000000,
//O
0b01100000,
0b10100000,
0b10100000,
0b10100000,
0b11000000,
0b00000000,
//P
0b11100000,
0b10100000,
0b11100000,
0b10000000,
0b10000000,
0b00000000,
//Q
0b01000000,
0b10100000,
0b10100000,
0b11000000,
0b01100110,
0b00000000,
//R
0b11100000,
0b10100000,
0b11000000,
0b10100000,
0b10100000,
0b00000000,
//S
0b01100000,
0b10000000,
0b11100000,
0b00100000,
0b11000000,
0b00000000,
//T
0b11100000,
0b01000000,
0b01000000,
0b01000000,
0b01000000,
0b00000000, 
//U
0b10100000,
0b10100000,
0b10100000,
0b10100000,
0b01100000,
0b00000000, 
//V
0b10100000,
0b10100000,
0b10100000,
0b11100000,
0b01000000,
0b00000000,
//W
0b10100000,
0b10100000,
0b10100000,
0b11100000,
0b11100010,
0b00000000,
//X
0b10100000,
0b10100000,
0b01000000,
0b10100000,
0b10100000,
0b00000000,
//Y
0b10100000,
0b10100000,
0b11100000,
0b00100000,
0b11100000,
0b00000000,
//Z
0b11100000,
0b00100000,
0b01000000,
0b10000000,
0b11100000,
0b00000000,
//[
0b11000000,
0b10000000,
0b10000000,
0b10000000,
0b11000000,
0b00000000,
0,				//for the life of me I have no idea why this is needed
//\
0b10000000,
0b10000000,
0b01000000,
0b00100000,
0b00100000,
0b00000000,
//]
0b11000000,
0b01000000,
0b01000000,
0b01000000,
0b11000000,
0b00000000,
//^
0b01000000,
0b10100000,
0b00000000,
0b00000000,
0b00000000,
0b00000000,
//_
0b00000000,
0b00000000,
0b00000000,
0b00000000,
0b11100000,
0b00000000,
//`
0b10000000,
0b01000000,
0b00000000,
0b00000000,
0b00000000,
0b00000000,
//a
0b00000000,
0b01100000,
0b10100000,
0b10100000,
0b01100000,
0b00000000,
//b
0b10000000,
0b10000000,
0b11000000,
0b10100000,
0b11000000,
0b00000000,
//c
0b00000000,
0b01100000,
0b10000000,
0b10000000,
0b01100000,
0b00000000,
//d
0b00100000,
0b00100000,
0b01100000,
0b10100000,
0b01100000,
0b00000000,
//e
0b01000000,
0b10100000,
0b11100000,
0b10000000,
0b01100000,
0b00000000,
//f
0b01100000,
0b01000000,
0b11100000,
0b01000000,
0b01000000,
0b00000000,
//g
0b01100000,
0b10100000,
0b01100000,
0b00100000,
0b11000000,
0b00000000,
//h
0b10000000,
0b10000000,
0b11000000,
0b10100000,
0b10100000,
0b00000000,
//i
0b01000000,
0b00000000,
0b01000000,
0b01000000,
0b01000000,
0b00000000,
//j
0b01000000,
0b00000000,
0b01000000,
0b01000000,
0b11000000,
0b00000000,
//k
0b10000000,
0b10000000,
0b10100000,
0b11000000,
0b10100000,
0b00000000,
//l
0b11000000,
0b01000000,
0b01000000,
0b01000000,
0b11100000,
0b00000000,
//m
0b00000000,
0b10100000,
0b11100000,
0b10100000,
0b10100000,
0b00000000,
//n
0b00000000,
0b11000000,
0b10100000,
0b10100000,
0b10100000,
0b00000000,
//o
0b00000000,
0b01000000,
0b10100000,
0b10100000,
0b01000000,
0b00000000,
//p
0b00000000,
0b11000000,
0b10100000,
0b11000000,
0b10000000,
0b00000000,
//q
0b00000000,
0b01100000,
0b10100000,
0b01100000,
0b00100000,
0b00000000,
//r
0b00000000,
0b11000000,
0b10100000,
0b10000000,
0b10000000,
0b00000000,
//s
0b00000000,
0b01100000,
0b01000000,
0b00100000,
0b01100000,
0b00000000,
//t
0b01000000,
0b11100000,
0b01000000,
0b01000000,
0b01000000,
0b00000000,
//u
0b00000000,
0b10100000,
0b10100000,
0b10100000,
0b01100000,
0b00000000,
//v
0b00000000,
0b10100000,
0b10100000,
0b10100000,
0b01000000,
0b00000000,
//w
0b00000000,
0b10100000,
0b10100000,
0b11100000,
0b10100000,
0b00000000,
//x
0b00000000,
0b10100000,
0b01000000,
0b01000000,
0b10100000,
0b00000000,
//y
0b00000000,
0b10100000,
0b11100000,
0b00100000,
0b01000000,
0b00000000,
//x
0b00000000,
0b11100000,
0b01000000,
0b10000000,
0b11100000,
0b00000000,
//{
0b00100000,
0b01000000,
0b11000000,
0b01000000,
0b00100010,
0b00000000,
//|
0b01000000,
0b01000000,
0b00000000,
0b01000000,
0b01000000,
0b00000000,
//}
0b10000000,
0b01000000,
0b01100000,
0b01000000,
0b10000000,
0b00000000,
//~
0b00000000,
0b10100000,
0b01000000,
0b00000000,
0b00000000,
0b00000000
};

static void DrawLetter(unsigned int fontIndex, int x, int y)
{
    //unsigned int sWidth = (textwidth / 8) + 1;
    unsigned int sHeigth = 6;//textheigth;

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
                    framebuffer[x+b+(w*8)+((y+h)*320)] = 0xFFFF;
                }
            }
            currentByteIndex++;
            currentByte = font4x6[currentByteIndex];
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

int32_t sign(int32_t n)
{
    if (n > 0)
    {
        return 1;
    }
    if (n < 0)
    {
        return -1;
    }
    return n;
}

void cls(uint16_t c)
{
    for(uint32_t i = 0; i<(320*240); i++)
    {
        framebuffer[i] = c;
    }
}



int draw_rectfill(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t c)
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
    
    if (x1 >= 320 || y1 >= 240 || x2 < 0 || y2 < 0)
    {
        return -1;
    }
    
    for(int32_t i = y1; i < y2; i++)
    {
        for(int32_t j = x1; j < x2; j++)
        {
            framebuffer[j + i * 320] = c;
        }
    }
    
    return 0;
}

// "Worker thread" for each core
void __time_critical_func(render_loop)() {
    static uint32_t last_frame_num = 0;
    int core_num = get_core_num();
    printf("Rendering on core %d\n", core_num);
    SetFont(font4x6);
    
    DrawText("QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ", 0, 0);
    DrawText("THANK YOU FOR 100 SUBSCRIBERS ON JALECKO'S DUMPTRUCK!", 54, 6);
    DrawText("THIS IS 80 COLLUMS BY 40 ROWS OF TEXT QVGA 320X240", 58, 12);
    
    while (true) {
        mutex_enter_blocking(&frame_logic_mutex);
        
        uint32_t rx = rand()%320;
        uint32_t ry = 18+rand()%222;
        uint16_t rc = rand();
        framebuffer[rx + ry*320] = rc;
        
        
        mutex_exit(&frame_logic_mutex);

        //scanline(framebuffer + _y * 320, 320, _x0, _y0 + _dy0_dy * _y, _dx0_dx);
#if !PICO_ON_DEVICE
        struct scanvideo_scanline_buffer *buffer = scanvideo_begin_scanline_generation(true);
        fill_scanline_buffer(buffer);
        scanvideo_end_scanline_generation(buffer);
#endif
    }
}

int64_t timer_callback(alarm_id_t alarm_id, void *user_data) {
    struct scanvideo_scanline_buffer *buffer = scanvideo_begin_scanline_generation(false);
    while (buffer) {
        fill_scanline_buffer(buffer);
        scanvideo_end_scanline_generation(buffer);
        buffer = scanvideo_begin_scanline_generation(false);
    }
    return 100;
}

void fill_scanline_buffer(struct scanvideo_scanline_buffer *buffer) {
    static uint32_t postamble[] = {
            0x0000u | (COMPOSABLE_EOL_ALIGN << 16)
    };

    buffer->data[0] = 4;
    buffer->data[1] = host_safe_hw_ptr(buffer->data + 8);
    buffer->data[2] = 158; // first four pixels are handled separately
    uint16_t *pixels = framebuffer + scanvideo_scanline_number(buffer->scanline_id) * 320;
    buffer->data[3] = host_safe_hw_ptr(pixels + 4);
    buffer->data[4] = count_of(postamble);
    buffer->data[5] = host_safe_hw_ptr(postamble);
    buffer->data[6] = 0;
    buffer->data[7] = 0;
    buffer->data_used = 8;

    // 3 pixel run followed by main run, consuming the first 4 pixels
    buffer->data[8] = (pixels[0] << 16u) | COMPOSABLE_RAW_RUN;
    buffer->data[9] = (pixels[1] << 16u) | 0;
    buffer->data[10] = (COMPOSABLE_RAW_RUN << 16u) | pixels[2];
    buffer->data[11] = ((317 + 1 - 3) << 16u) | pixels[3]; // note we add one for the black pixel at the end
}

struct semaphore video_setup_complete;

void core1_func() {
    sem_acquire_blocking(&video_setup_complete);
    render_loop();
}

int vga_main(void) {
//    framebuffer = calloc(320*240, sizeof(uint16_t));
    mutex_init(&frame_logic_mutex);
    sem_init(&video_setup_complete, 0, 1);

    // Core 1 will wait for us to finish video setup, and then start rendering
    multicore_launch_core1(core1_func);

    hard_assert(vga_mode.width + 4 <= PICO_SCANVIDEO_MAX_SCANLINE_BUFFER_WORDS * 2);
    scanvideo_setup(&vga_mode);
    scanvideo_timing_enable(true);

    //frame_update_logic();////////////THIS THREAD MIGHT BE FREE?
    sem_release(&video_setup_complete);

#if PICO_ON_DEVICE
    add_alarm_in_us(100, timer_callback, NULL, true);
#endif
    render_loop();
    return 0;
}

static inline void raw_scanline_finish(struct scanvideo_scanline_buffer *dest) {
    // Need to pivot the first pixel with the count so that PIO can keep up
    // with its 1 pixel per 2 clocks
    uint32_t first = dest->data[0];
    uint32_t second = dest->data[1];
    dest->data[0] = (first & 0x0000ffffu) | ((second & 0x0000ffffu) << 16);
    dest->data[1] = (second & 0xffff0000u) | ((first & 0xffff0000u) >> 16);
    dest->status = SCANLINE_OK;
}

/*
void __time_critical_func(frame_update_logic)() {
    if (!params_ready) {
        float scale = vga_mode.height / 2;
        static int foo;
        float offx = (MIN(foo, 200)) / 500.0f;
        float offy = -(MIN(foo, 230)) / 250.0f;
        scale *= (10000 + (foo++) * (float) foo) / 10000.0f;
        x0 = float_to_fixed(offx + (-vga_mode.width / 2) / scale - 0.5f);
        y0 = float_to_fixed(offy + (-vga_mode.height / 2) / scale);
        dx0_dx = float_to_fixed(1.0f / scale);
        dy0_dy = float_to_fixed(1.0f / scale);
        max = float_to_fixed(4.f);
        params_ready = true;
    }
}*/

int main(void) 
{
    uint base_freq;
#if PICO_SCANVIDEO_48MHZ
    base_freq = 48000;
#else
    base_freq = 50000;
#endif

#if PICO_ON_DEVICE
    #if TURBO_BOOST
        vreg_set_voltage(VREG_VOLTAGE_1_30);
        sleep_ms(10);
        set_sys_clock_khz(base_freq * 6, true);
    #else
        set_sys_clock_khz(base_freq * 3, true);
    #endif
#endif
    // Re init uart now that clk_peri has changed
    setup_default_uart();
//    gpio_debug_pins_init();

    return vga_main();
}
