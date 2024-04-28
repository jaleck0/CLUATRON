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

// int rgb(r,g,b)
static int l_get_rgbcolor(lua_State* L) 
{
    uint16_t r = (lua_tointeger(L, 1) % 256) >> 3;
    uint16_t g = (lua_tointeger(L, 2) % 256) >> 3;
    uint16_t b = (lua_tointeger(L, 3) % 256) >> 3;

    lua_pop(L, 1);
    lua_Integer newCol = r | g << 6 | b << 11;
    lua_pushinteger(L, newCol);
    return 1;
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

void RegisterCommands(lua_State* L)
{
    lua_register(L, "reset", l_reset);
    lua_register(L, "bootsel", l_bootsel);
    lua_register(L, "pcol", l_set_printcolor);
    lua_register(L, "bcol", l_set_backcolor);
    lua_register(L, "dot", l_put_pixel);
    lua_register(L, "pal", l_set_palindexcolor);
    lua_register(L, "cls", l_clearscreen);
    lua_register(L, "rectfill", l_put_rectfill);
    lua_register(L, "millis", l_get_millis);
    lua_register(L, "rgb", l_get_rgbcolor);
}