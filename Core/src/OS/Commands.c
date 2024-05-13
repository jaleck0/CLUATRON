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

//void line(x1,y1,x2,y2,color)
static int l_put_line(lua_State* L)
{
    int x1 = lua_tointeger(L, 1);
    int y1 = lua_tointeger(L, 2);
    int x2 = lua_tointeger(L, 3);
    int y2 = lua_tointeger(L, 4);
    int col = lua_tointeger(L, 5);

    lua_pop(L, 1);
    col %= 16;
    DrawLine(x1, y1, x2, y2, col);
    
    return 0;
}


//void rect(x,y,w,h,color)
static int l_put_rect(lua_State* L)
{
    int xpos = lua_tointeger(L, 1);
    int ypos = lua_tointeger(L, 2);
    int w = lua_tointeger(L, 3);
    int h = lua_tointeger(L, 4);
    int col = lua_tointeger(L, 5);

    lua_pop(L, 1);
    col %= 16;
    DrawRect(xpos, ypos, w, h, col);
    
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

//void tri(x1,y1,x2,y2,x3,y3,color)
static int l_put_tri(lua_State* L)
{
    int x1 = lua_tointeger(L, 1);
    int y1 = lua_tointeger(L, 2);
    int x2 = lua_tointeger(L, 3);
    int y2 = lua_tointeger(L, 4);
    int x3 = lua_tointeger(L, 5);
    int y3 = lua_tointeger(L, 6);
    int col = lua_tointeger(L, 7);

    lua_pop(L, 1);
    col %= 16;
    DrawTri(x1, y1, x2, y2, x3, y3, col);
    
    return 0;
}

//void tri(x1,y1,x2,y2,x3,y3,color)
static int l_put_trifill(lua_State* L)
{
    int x1 = lua_tointeger(L, 1);
    int y1 = lua_tointeger(L, 2);
    int x2 = lua_tointeger(L, 3);
    int y2 = lua_tointeger(L, 4);
    int x3 = lua_tointeger(L, 5);
    int y3 = lua_tointeger(L, 6);
    int col = lua_tointeger(L, 7);

    lua_pop(L, 1);
    col %= 16;
    DrawTrifill(x1, y1, x2, y2, x3, y3, col);
    
    return 0;
}

//void circ(x,y,r,color)
static int l_put_circ(lua_State* L)
{
    int x = lua_tointeger(L, 1);
    int y = lua_tointeger(L, 2);
    int r = lua_tointeger(L, 3);
    int col = lua_tointeger(L, 4);

    lua_pop(L, 1);
    col %= 16;
    DrawCirc(x, y, r, col);
    
    return 0;
}

//void circfill(x,y,r,color)
static int l_put_circfill(lua_State* L)
{
    int x = lua_tointeger(L, 1);
    int y = lua_tointeger(L, 2);
    int r = lua_tointeger(L, 3);
    int col = lua_tointeger(L, 4);

    lua_pop(L, 1);
    col %= 16;
    DrawCircfill(x, y, r, col);
    
    return 0;
}

//void oval(x1,y1,w,h,color)
static int l_put_oval(lua_State* L)
{
    int x = lua_tointeger(L, 1);
    int y = lua_tointeger(L, 2);
    int w = lua_tointeger(L, 3);
    int h = lua_tointeger(L, 4);
    int col = lua_tointeger(L, 5);

    lua_pop(L, 1);
    col %= 16;
    DrawEllipse(x, y, w, h, col);
    
    return 0;
}

//void ovalfill(x,y,w,h,color)
static int l_put_ovalfill(lua_State* L)
{
    int x = lua_tointeger(L, 1);
    int y = lua_tointeger(L, 2);
    int w = lua_tointeger(L, 3);
    int h = lua_tointeger(L, 4);
    int col = lua_tointeger(L, 5);

    lua_pop(L, 1);
    col %= 16;
    DrawEllipsefill(x, y, w, h, col);
    
    return 0;
}

//void text(str,x1,y1,color)
static int l_put_text(lua_State* L)
{
    char* str = lua_tostring(L, 1);
    int x = lua_tointeger(L, 2);
    int y = lua_tointeger(L, 3);
    int col = lua_tointeger(L, 4);

    lua_pop(L, 1);
    col %= 16;
    SetTextColor(col);
    DrawText(str, x, y);
    
    return 0;
}

//void textfill(str,x1,y1,color,color)
static int l_put_textfill(lua_State* L)
{
    char* str = lua_tostring(L, 1);
    int x = lua_tointeger(L, 2);
    int y = lua_tointeger(L, 3);
    int col0 = lua_tointeger(L, 4);
    int col1 = lua_tointeger(L, 5);

    lua_pop(L, 1);
    col0 %= 16;
    col1 %= 16;
    uint32_t len = strlen(str);
    DrawRectfill(x,y,len*4,6,col1);
    SetTextColor(col0);
    DrawText(str, x, y);
    
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


//int init_input()
static int l_set_inputinit(lua_State* L)
{
    
    lua_pop(L, 1);
    InitInputs();
    
    return 0;
}

//int read_input()
static int l_get_input(lua_State* L)
{
    
    lua_pop(L, 1);
    ReadInputs();
    
    return 0;
}

//int keyp()
static int l_get_keyp(lua_State* L)
{
    int input = lua_tointeger(L, 1);
    int result = KeyboardGetPressed(input);
    lua_pop(L, 1);
    lua_pushinteger(L, result);
    
    return 1;
}

//int keyh()
static int l_get_keyh(lua_State* L)
{
    int input = lua_tointeger(L, 1);
    int result = KeyboardGetHold(input);
    lua_pop(L, 1);
    lua_pushinteger(L, result);
    
    return 1;
}

//int keyr()
static int l_get_keyr(lua_State* L)
{
    int input = lua_tointeger(L, 1);
    int result = KeyboardGetReleased(input);
    lua_pop(L, 1);
    lua_pushinteger(L, result);
    
    return 1;
}

void RegisterConstants(lua_State* L)
{
    lua_pushnumber(L, (lua_Number)l_mathop(3.14));
    lua_setfield(L, 2, "popo");
}

//to be added: setspr spr temp 

void RegisterCommands(lua_State* L)
{
    lua_register(L, "reset", l_reset);
    lua_register(L, "bootsel", l_bootsel);
    lua_register(L, "pcol", l_set_printcolor);
    lua_register(L, "bcol", l_set_backcolor);
    lua_register(L, "dot", l_put_pixel);   
    lua_register(L, "pal", l_set_palindexcolor);
    lua_register(L, "cls", l_clearscreen);
    lua_register(L, "line", l_put_line);
    lua_register(L, "rect", l_put_rect);
    lua_register(L, "rectfill", l_put_rectfill);
    lua_register(L, "tri", l_put_tri);
    //lua_register(L, "trifill", l_put_trifill);//to be implemented
    lua_register(L, "circ", l_put_circ);
    lua_register(L, "circfill", l_put_circfill);
    lua_register(L, "oval", l_put_oval);
    lua_register(L, "ovalfill", l_put_ovalfill);
    lua_register(L, "text", l_put_text);
    lua_register(L, "textfill", l_put_textfill);
    lua_register(L, "millis", l_get_millis);
    lua_register(L, "rgb", l_get_rgbcolor);
    lua_register(L, "init_input", l_set_inputinit);
    lua_register(L, "read_input", l_get_input);
    lua_register(L, "keyp", l_get_keyp);
    lua_register(L, "keyh", l_get_keyh);
    lua_register(L, "keyr", l_get_keyr);
}