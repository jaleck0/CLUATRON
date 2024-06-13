#include <string.h>

#include "SharedOS.h"
#include "../USBInput/Input.h"
#include "../Framebuffer/FramebufferFunctions.h"
#include "../Graphics/GraphicsFunctions.h"
#include "../Terminal/Terminal.h"

#include "CluaShell.h"
#include "CluatronEditingTool.h"



char command[78] = "";
uint32_t commandPos = 0;

char file[32768] = "x=100 y=100 dx=0 dy=0 t=0 d=16 cls(7) while(true) do if (millis()-t >= d) then t=millis() rectfill(math.floor(x-8),221,16,221,7) rectfill(304,0,319,222,7) rectfill(math.floor(x-10),math.floor(y-10),20,20,7) dy=dy+0.14 l=math.sqrt(dx*dx + dy*dy) a=math.atan(dy,dx) if (keyh(21)==1) then x=100 y=50 end if (keyh(79)==1) then a=a+0.14 end if (keyh(80)==1) then a=a-0.14 end w=0.2-math.cos(a*4)/50 dx=l*math.cos(a) dy=l*math.sin(a) x=(x+dx+8) % 320 - 8 y=y+dy if (y>220) then dx=dx*0.9 dy=-dy/2 y=220 end for b=a-w,a+w,w*2 do e=x-8*math.cos(b) line(math.floor(x)-4,221,math.floor(x)+4,221,6) line(math.floor(x),math.floor(y),math.floor(e),math.floor(y-8*math.sin(b)),0) end end end";
uint32_t filePos = 0;

Program currentProgram = SHELL;
/*
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
};*/

void SwitchProgram()
{
    if (KeyboardGetPressed(USB_F1))
    {
        StartShell();
    }

    if (KeyboardGetPressed(USB_F2))
    {
        StartCET();   
    }
}
