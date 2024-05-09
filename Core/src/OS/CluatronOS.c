#include "CluatronOS.h"
#include "Commands.h"
#include "../Framebuffer/FramebufferFunctions.h"
#include "../Graphics/GraphicsFunctions.h"
#include "../Graphics/font4x6.h"
#include "../Terminal/Terminal.h"
#include "../USBInput/Input.h"

#include <string.h>
#include <stdlib.h>

#include "hardware/watchdog.h"
#include "pico/stdlib.h"
#include "pico/bootrom.h"

#include "../lua/lua.h" 
#include "../lua/lualib.h"
#include "../lua/lauxlib.h"
#include "../lua/lualib.h"

#define PROMPT "> "

char* startCommand = "function start() read_input() if keyh(41) == 1 then error(\"stopped\",2) end end debug.sethook(start,\"l\")";

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
    board_init();
    cls(1);
    SetTextColor(11);
    TerminalSetBackCol(1);
    TerminalPutString("                        ---CLUATRON OS ~ Jalecko 2024---\n");

    InitInputs();

    lua_State* L;
    luaL_Buffer buf;
    int status;
    size_t len;
    char ch;

    stdio_init_all();

    L = luaL_newstate();
    luaL_openlibs(L);
    
    luaL_buffinit(L, &buf);

    RegisterCommands(L);

    TerminalPutString("\n ctrl-c  clear buffer\n ctrl-l  clear screen\n enter   execute buffer\n\n");
    TerminalPutString(PROMPT);

    uint8_t vis = 0;
    uint32_t mes = 0;
    uint32_t t = 0;

    luaL_addlstring(&buf, startCommand, strlen(startCommand));
    luaL_pushresult(&buf);
    const char *start = lua_tolstring(L, -1, &len);
    status = luaL_loadbuffer(L, start, len, "startup");
    status = lua_pcall(L, 0, 0, 0);
    luaL_buffinit(L, &buf);

    for(;;)
    {
        ReadInputs();

        ch = (char)KeyboardGiveLetter();

        if(ch == '\r') 
        {
            ch = '\n';
        }

        //if(ch == 0x7F || ch == 0x08) 
        if (KeyboardGetPressed(USB_BACKSPACE))
        { // DEL or BS
            TerminalBackspace();
            if(luaL_bufflen(&buf) > 0) 
            {
                luaL_buffsub(&buf, 1);
            }
        }
        else if (KeyboardGetPressed(USB_L) && KeyboardGetCtrl())//if(ch == 0x0C) 
        { // Ctrl-L (ANSI clear screen)
            TerminalClear();
            TerminalPutString(PROMPT);
        }
        else if (KeyboardGetPressed(USB_C) && KeyboardGetCtrl())//if(ch == 0x03) 
        { // Ctrl-C (clear buffer without executing)
            luaL_buffinit(L, &buf);
            TerminalPutString("\n");
            TerminalPutString(PROMPT);
        }
        else if (KeyboardGetPressed(USB_ENTER))//if(ch == 0x04) 
        {
            TerminalPutCharacter('\n');
            luaL_pushresult(&buf);
            const char *s = lua_tolstring(L, -1, &len);
            status = luaL_loadbuffer(L, s, len, "cluatron");

            if(status != LUA_OK) 
            {
                const char *msg = lua_tostring(L, -1);
                TerminalPutString("parse error: ");
                TerminalPutString(msg);
                TerminalPutString("\n");
            }
            else
            {
                status = lua_pcall(L, 0, 0, 0);

                if(status != LUA_OK) 
                {
                    const char *msg = lua_tostring(L, -1);
                    TerminalPutString("execute error: ");
                    TerminalPutString(msg);
                    TerminalPutString("\n");
                    //lua_writestringerror("execute error: %s\n", msg);
                }
            }

            lua_pop(L, 1);
            luaL_buffinit(L, &buf);
            TerminalPutString(PROMPT);
        }
        else if((ch >= 0x20 && ch < 0x7F) || ch == '\t' || ch == '\n') 
        { // [ \t\na-zA-z]
            TerminalPutCharacter(ch);
            luaL_addchar(&buf, ch);
        }

        if (KeyboardGetPressed(USB_F1))
        {
            for(uint8_t i = 0; i<16; i++)
            {
                ChangeColor( i, pico8pal[i]);
            }
            set_colours();
        }

        if (KeyboardGetPressed(USB_F2))
        {
            for(uint8_t i = 0; i<16; i++)
            {
                ChangeColor( i, cgapal[i]);
            }
            set_colours();
        }
        
        if (to_ms_since_boot(get_absolute_time()) - mes >= 500)
        {
            mes = to_ms_since_boot(get_absolute_time());

            if(vis)
            {
                DrawWritePoint();
                vis = 0;
            }
            else
            {
                RemoveWritePoint();
                vis = 1;
            }
        }
    }
}
