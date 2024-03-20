#include "FramebufferFunctions.h"

uint16_t    colours[COLOR_COUNT] =
{
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(  0u,   0u,   0u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(29u,   43u,   83u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(126u, 37u,   83u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(0u, 135u,   81u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(  171u,   82u, 54u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8( 95u,  87u,  64u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8( 194u, 195u, 199u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(255u, 255u, 255u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8( 255u,  0u,  77u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(255u,   163u,   0u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(  255u, 236u,   39u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(0u, 228u,   54u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(  41u,   173u, 255u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(131u,   118u, 156u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(255u, 119u, 168u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(255u, 204u, 170u)
};

uint32_t dblpal[COLOR_COUNT * COLOR_COUNT];
uint8_t fbuf[WIDTH * HEIGHT / 2];

void setup_video (void)
{
    scanvideo_setup(&vga_mode_320x240_60);
    scanvideo_timing_enable(true);
}

void set_colours (uint16_t *pclr)
{
    uint32_t *dpal = dblpal;
    for ( int i = 0; i < COLOR_COUNT; ++i )
    {
        for ( int j = 0; j < COLOR_COUNT; ++j )
        {
            *dpal = ( pclr[i] << 16 ) | pclr[j];
            ++dpal;
        }
    }
}

void plot_point (int x, int y, int clr)
{
    bool odd = x & 1;
    int n = ( WIDTH / 2 ) * y + x / 2;
    clr &= 0x0F;
    if ( ( n >= 0 ) && ( n < WIDTH * HEIGHT / 2 ) )
    {
        uint8_t *p = &fbuf[n];
        if ( odd ) *p = ( clr << 4 ) | ( *p & 0x0F );
        else *p = ( *p & 0xF0 ) | clr;
    }
}

void core1_entry() 
{
    
    uint32_t pos = 0;
    uint32_t col = 0x7;
    uint32_t mes = 0;

    for(;;)
    {
        
        plot_point(pos%320, pos/320, col);
        pos++;
        pos %= 76800;
        
        if (to_ms_since_boot(get_absolute_time()) - mes >= 1000)
        {
            mes = to_ms_since_boot(get_absolute_time());
            pos = 0;
            if (col == 0x7)
            {
                col = 0;
            }
            else
            {
                col = 0x7;
            }
        }
        plot_point(0, 0, 8);
		plot_point(319, 0, 8);
		plot_point(0, 239, 8);
		plot_point(319, 239, 8);
    }
}

void __time_critical_func(render_loop)(void)
{
    while (true)
    {
        struct scanvideo_scanline_buffer *buffer = scanvideo_begin_scanline_generation (true);
        int iScan = scanvideo_scanline_number (buffer->scanline_id);
        uint32_t *twoclr = buffer->data;
        uint8_t *twopix = &fbuf[(WIDTH / 2) * iScan];

        for ( int iCol = 0; iCol < WIDTH / 2; ++iCol )
            {
            ++twoclr;
            *twoclr = dblpal[*twopix];
            ++twopix;
            }
        ++twoclr;

        *twoclr = COMPOSABLE_EOL_ALIGN << 16;
        twoclr = buffer->data;
        twoclr[0] = ( twoclr[1] << 16 ) | COMPOSABLE_RAW_RUN;
        twoclr[1] = ( twoclr[1] & 0xFFFF0000 ) | ( WIDTH - 2 );
        buffer->data_used = ( WIDTH + 4 ) / 2;
        scanvideo_end_scanline_generation (buffer);
    }
}