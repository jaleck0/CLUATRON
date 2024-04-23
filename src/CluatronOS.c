#include "CluatronOS.h"
#include "FramebufferFunctions.h"
#include "GraphicsFunctions.h"
#include "font4x6.h"
#include "Terminal.h"
#include "Input.h"

#include <string.h>
#include <stdlib.h>

#include "hardware/watchdog.h"
#include "pico/stdlib.h"
#include "pico/bootrom.h"

#include "lua.h" 
#include "lualib.h"
#include "lauxlib.h"


#include "bsp/board.h"
#include "tusb.h"

#define PROMPT "lua> "

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

static int l_reset(lua_State* L) 
{
    watchdog_reboot(0, 0, 0);
    return 0;
}

// bootsel()
static int l_bootsel(lua_State* L) 
{
    reset_usb_boot(0, 0);
    return 0;
}

// pcol(col)
static int l_set_printcolor(lua_State* L) 
{
    int col = lua_tointeger(L, 1);
    lua_pop(L, 1);
    col %= 16;
    SetTextColor(col);
    return 0;
}

// bcol(col)
static int l_set_backcolor(lua_State* L) 
{
    int col = lua_tointeger(L, 1);
    lua_pop(L, 1);
    col %= 16;
    TerminalSetBackCol(col);
    return 0;
}

// pal(col-index, col-val)
static int l_set_palindexcolor(lua_State* L) 
{
    int colIndex = lua_tointeger(L, 1);
    int colValue = lua_tointeger(L, 2);
    lua_pop(L, 1);
    ChangeColor(colIndex, colValue);
    set_colours();
    return 0;
}


// set_output(pin, bool)
static int l_set_output(lua_State* L) 
{
    int pin = lua_tointeger(L, 1);
    int output = lua_toboolean(L, 2);
    lua_pop(L, 2);
    gpio_init(pin);
    gpio_set_dir(pin, output);
    return 0;
}

// set_pin(pin, bool)
static int l_set_pin(lua_State* L) 
{
    int pin = lua_tointeger(L, 1);
    int state = lua_toboolean(L, 2);
    lua_pop(L, 2);
    gpio_put(pin, state == 1);
    return 0;
}

// bool get_pin(pin)
static int l_get_pin(lua_State* L) 
{
    int pin = lua_tointeger(L, 1);
    int state = gpio_get(pin);
    lua_pop(L, 1);
    lua_pushboolean(L, state);
    return 1;
}

//void dot(x,y,color)
static int l_put_pixel(lua_State* L)
{
    int xpos = lua_tointeger(L, 1);
    int ypos = lua_tointeger(L, 2);
    int col = lua_tointeger(L, 3);

    lua_pop(L, 1);
    col %= 16;
    plot_point(xpos , ypos, col);
    
    return 0;
}

//void cls([color])
static int l_clearscreen(lua_State* L)
{
    int col = lua_tointeger(L, 1);
    TerminalSetBackCol(col%16);

    lua_pop(L, 1);
    TerminalClear();
    
    return 0;
}

//void rectfill(x,y,w,h,color)
static int l_put_rectfill(lua_State* L)
{
    int xpos = lua_tointeger(L, 1);
    int ypos = lua_tointeger(L, 2);
    int w = lua_tointeger(L, 3);
    int h = lua_tointeger(L, 4);
    int col = lua_tointeger(L, 5);

    lua_pop(L, 1);
    col %= 16;
    DrawRectfill(xpos, ypos, w, h, col);
    
    return 0;
}

//int millis()
static int l_get_millis(lua_State* L)
{
    int millis = to_ms_since_boot(get_absolute_time());
    lua_pop(L, 1);
    lua_pushinteger(L, millis);
    
    return 1;
}

void core1_entry() 
{
    SetFont(font4x6);
    board_init();
    cls(1);
    SetTextColor(11);
    TerminalSetBackCol(1);
    TerminalPutString("                        ---CLUATRON OS ~ JALECKO 2024---\n");
    
    tuh_init(BOARD_TUH_RHPORT);

    lua_State *L;
    luaL_Buffer buf;
    int status;
    size_t len;
    char ch;

    stdio_init_all();

    L = luaL_newstate();
    luaL_openlibs(L);
    luaL_buffinit(L, &buf);

    lua_register(L, "reset", l_reset);
    lua_register(L, "bootsel", l_bootsel);
    lua_register(L, "pcol", l_set_printcolor);
    lua_register(L, "bcol", l_set_backcolor);
    lua_register(L, "dot", l_put_pixel);
    lua_register(L, "pal", l_set_palindexcolor);
    lua_register(L, "cls", l_clearscreen);
    lua_register(L, "rectfill", l_put_rectfill);
    lua_register(L, "millis", l_get_millis);
    //lua_register(L, "set_output", l_set_output);
    //lua_register(L, "set_pin", l_set_pin);
    //lua_register(L, "get_pin", l_get_pin);

    TerminalPutString("\n Ctrl-C  Clear buffer\n Ctrl-L  Clear screen\n Enter   Execute buffer\n\n");
    TerminalPutString(PROMPT);

    uint8_t vis = 0;
    uint32_t mes = 0;

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
        { // Ctrl-D
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
            //printf(PROMPT);
            TerminalPutString(PROMPT);
        }
        else if((ch >= 0x20 && ch < 0x7F) || ch == '\t' || ch == '\n') 
        { // [ \t\na-zA-z]
            putchar(ch);
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