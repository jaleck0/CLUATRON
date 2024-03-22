#include "CluatronOS.h"
#include "FramebufferFunctions.h"
#include "GraphicsFunctions.h"
#include "font4x6.h"
#include "Terminal.h"

uint16_t pico8pal[16] =
{
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(  0u,   0u,   0u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(29u,   43u,   83u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(126u, 37u,   83u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(0u, 135u,   81u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(  171u,   82u, 54u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8( 95u,  87u,  64u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8( 194u, 195u, 199u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(255u, 241u, 232u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8( 255u,  0u,  77u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(255u,   163u,   0u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(  255u, 236u,   39u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(0u, 228u,   54u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(  41u,   173u, 255u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(131u,   118u, 156u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(255u, 119u, 168u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(255u, 204u, 170u)
};

uint16_t cgapal[16] =
{
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(  0u,   0u,   0u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(0u,   0u,   170u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(0u, 170u,   0u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(0u, 170u,   170u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(  170u,   0u, 0u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8( 170u,  0u,  170u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8( 170u, 85u, 0u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(170u, 170u, 170u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8( 85u,  85u,  85u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(85u,   85u,   255u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(  85u, 255u,   85u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(85u, 255u,   255u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(  255u,   85u, 85u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(255u,   85u, 255u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(255u, 255u, 85u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(255u, 255u, 255u)
};

void core1_entry() 
{
    SetFont(font4x6);
    
    uint8_t p = 1;
    uint32_t mes = 0;

    for(;;)
    {
        
        if (to_ms_since_boot(get_absolute_time()) - mes >= 100)
        {
            mes = to_ms_since_boot(get_absolute_time());

            SetTextColor(p);
            p++;
            p %= 16;
            TerminalPutString("THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG.\n");
            
        }
    }
}