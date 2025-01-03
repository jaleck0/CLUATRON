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
#include "hardware/adc.h"
#include "hardware/uart.h"

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

//temp()
static int l_temp(lua_State* L) 
{
    uint16_t raw = adc_read();
    const float convf = 3.3f / (1<<12);
    float result = raw * convf;
    float temp = 27 - (result - 0.706)/0.001721;

    lua_pushnumber(L, temp);
    return 1;
}

// pcol(col)
static int l_set_printcolor(lua_State* L) 
{
    int col = (int)lua_tonumber(L, 1);
    lua_pop(L, 1);
    col %= 16;
    SetTextColor(col);
    return 0;
}

// bcol(col)
static int l_set_backcolor(lua_State* L) 
{
    int col = (int)lua_tonumber(L, 1);
    lua_pop(L, 1);
    col %= 16;
    TerminalSetBackCol(col);
    return 0;
}

// pal(col-index, col-val)
static int l_set_palindexcolor(lua_State* L) 
{
    int colIndex = (int)lua_tonumber(L, 1);
    int colValue = (int)lua_tonumber(L, 2);
    lua_pop(L, 1);
    ChangeColor(colIndex, colValue);
    set_colours();
    return 0;
}

// int rgb(r,g,b)
static int l_get_rgbcolor(lua_State* L) 
{
    uint16_t r = ((int)lua_tonumber(L, 1) % 256) >> 3;
    uint16_t g = ((int)lua_tonumber(L, 2) % 256) >> 3;
    uint16_t b = ((int)lua_tonumber(L, 3) % 256) >> 3;

    lua_pop(L, 1);
    lua_Integer newCol = r | g << 6 | b << 11;
    lua_pushinteger(L, newCol);
    return 1;
}

//void dot(x,y,color)
static int l_put_pixel(lua_State* L)
{
    int xpos = (int)lua_tonumber(L, 1);
    int ypos = (int)lua_tonumber(L, 2);
    int col = (int)lua_tonumber(L, 3);

    lua_pop(L, 1);
    col %= 16;
    plot_point(xpos , ypos, col);
    
    return 0;
}

//void cls([color])
static int l_clearscreen(lua_State* L)
{
    int col = (int)lua_tonumber(L, 1);
    TerminalSetBackCol(col%16);

    lua_pop(L, 1);
    TerminalClear();
    
    return 0;
}

//void line(x1,y1,x2,y2,color)
static int l_put_line(lua_State* L)
{
    int x1 = (int)lua_tonumber(L, 1);
    int y1 = (int)lua_tonumber(L, 2);
    int x2 = (int)lua_tonumber(L, 3);
    int y2 = (int)lua_tonumber(L, 4);
    int col = (int)lua_tonumber(L, 5);

    lua_pop(L, 1);
    col %= 16;
    DrawLine(x1, y1, x2, y2, col);
    
    return 0;
}


//void rect(x,y,w,h,color)
static int l_put_rect(lua_State* L)
{
    int xpos = (int)lua_tonumber(L, 1);
    int ypos = (int)lua_tonumber(L, 2);
    int w = (int)lua_tonumber(L, 3);
    int h = (int)lua_tonumber(L, 4);
    int col = (int)lua_tonumber(L, 5);

    lua_pop(L, 1);
    col %= 16;
    DrawRect(xpos, ypos, w, h, col);
    
    return 0;
}

//void rectfill(x,y,w,h,color)
static int l_put_rectfill(lua_State* L)
{
    int xpos = (int)lua_tonumber(L, 1);
    int ypos = (int)lua_tonumber(L, 2);
    int w = (int)lua_tonumber(L, 3);
    int h = (int)lua_tonumber(L, 4);
    int col = (int)lua_tonumber(L, 5);

    lua_pop(L, 1);
    col %= 16;
    DrawRectfill(xpos, ypos, w, h, col);
    
    return 0;
}

//void tri(x1,y1,x2,y2,x3,y3,color)
static int l_put_tri(lua_State* L)
{
    int x1 = (int)lua_tonumber(L, 1);
    int y1 = (int)lua_tonumber(L, 2);
    int x2 = (int)lua_tonumber(L, 3);
    int y2 = (int)lua_tonumber(L, 4);
    int x3 = (int)lua_tonumber(L, 5);
    int y3 = (int)lua_tonumber(L, 6);
    int col = (int)lua_tonumber(L, 7);

    lua_pop(L, 1);
    col %= 16;
    DrawTri(x1, y1, x2, y2, x3, y3, col);
    
    return 0;
}

//void tri(x1,y1,x2,y2,x3,y3,color)
static int l_put_trifill(lua_State* L)
{
    int x1 = (int)lua_tonumber(L, 1);
    int y1 = (int)lua_tonumber(L, 2);
    int x2 = (int)lua_tonumber(L, 3);
    int y2 = (int)lua_tonumber(L, 4);
    int x3 = (int)lua_tonumber(L, 5);
    int y3 = (int)lua_tonumber(L, 6);
    int col = (int)lua_tonumber(L, 7);

    lua_pop(L, 1);
    col %= 16;
    DrawTrifill(x1, y1, x2, y2, x3, y3, col);
    
    return 0;
}

//void circ(x,y,r,color)
static int l_put_circ(lua_State* L)
{
    int x = (int)lua_tonumber(L, 1);
    int y = (int)lua_tonumber(L, 2);
    int r = (int)lua_tonumber(L, 3);
    int col = (int)lua_tonumber(L, 4);

    lua_pop(L, 1);
    col %= 16;
    DrawCirc(x, y, r, col);
    
    return 0;
}

//void circfill(x,y,r,color)
static int l_put_circfill(lua_State* L)
{
    int x = (int)lua_tonumber(L, 1);
    int y = (int)lua_tonumber(L, 2);
    int r = (int)lua_tonumber(L, 3);
    int col = (int)lua_tonumber(L, 4);

    lua_pop(L, 1);
    col %= 16;
    DrawCircfill(x, y, r, col);
    
    return 0;
}

//void oval(x1,y1,w,h,color)
static int l_put_oval(lua_State* L)
{
    int x = (int)lua_tonumber(L, 1);
    int y = (int)lua_tonumber(L, 2);
    int w = (int)lua_tonumber(L, 3);
    int h = (int)lua_tonumber(L, 4);
    int col = (int)lua_tonumber(L, 5);

    lua_pop(L, 1);
    col %= 16;
    DrawEllipse(x, y, w, h, col);
    
    return 0;
}

//void ovalfill(x,y,w,h,color)
static int l_put_ovalfill(lua_State* L)
{
    int x = (int)lua_tonumber(L, 1);
    int y = (int)lua_tonumber(L, 2);
    int w = (int)lua_tonumber(L, 3);
    int h = (int)lua_tonumber(L, 4);
    int col = (int)lua_tonumber(L, 5);

    lua_pop(L, 1);
    col %= 16;
    DrawEllipsefill(x, y, w, h, col);
    
    return 0;
}

//void text(str,x1,y1,color)
static int l_put_text(lua_State* L)
{
    char* str = lua_tostring(L, 1);
    int x = (int)lua_tonumber(L, 2);
    int y = (int)lua_tonumber(L, 3);
    int col = (int)lua_tonumber(L, 4);

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
    int x = (int)lua_tonumber(L, 2);
    int y = (int)lua_tonumber(L, 3);
    int col0 = (int)lua_tonumber(L, 4);
    int col1 = (int)lua_tonumber(L, 5);

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

//int mdx()
static int l_get_mdx(lua_State* L)
{
    int result = MouseGetXSpeed();
    lua_pushinteger(L, result);
    
    return 1;
}

//int mdy()
static int l_get_mdy(lua_State* L)
{
    int result = MouseGetYSpeed();
    lua_pushinteger(L, result);
    
    return 1;
}

//int mscr()
static int l_get_mscr(lua_State* L)
{
    int result = MouseGetSSpeed();
    lua_pushinteger(L, result);
    
    return 1;
}

//int mbp()
static int l_get_mbp(lua_State* L)
{
    int input = lua_tointeger(L, 1);
    int result = MouseGetPressed(input);
    lua_pop(L, 1);
    lua_pushinteger(L, result);
    
    return 1;
}

//int mbh()
static int l_get_mbh(lua_State* L)
{
    int input = lua_tointeger(L, 1);
    int result = MouseGetHold(input);
    lua_pop(L, 1);
    lua_pushinteger(L, result);
    
    return 1;
}

//int mbr()
static int l_get_mbr(lua_State* L)
{
    int input = lua_tointeger(L, 1);
    int result = MouseGetReleased(input);
    lua_pop(L, 1);
    lua_pushinteger(L, result);
    
    return 1;
}

void RegisterConstants(lua_State* L)
{
    lua_pushnumber(L, (lua_Number)l_mathop(3.14));
    lua_setfield(L, 2, "popo");
}

//to be added: setspr spr 

static int l_sendtextuart(lua_State* L)
{
    char* str = lua_tostring(L, 1);
    lua_pop(L, 1);
    uart_init(uart0, 115200);

    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);

    uart_puts(uart0, str);
    
    return 0;
}


void RegisterCommands(lua_State* L)
{
    lua_register(L, "reset", l_reset);
    lua_register(L, "bootsel", l_bootsel);
    lua_register(L, "temp", l_temp);

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
    lua_register(L, "mdx", l_get_mdx);
    lua_register(L, "mdy", l_get_mdy);
    lua_register(L, "mscr", l_get_mscr);
    lua_register(L, "mbp", l_get_mbp);
    lua_register(L, "mbh", l_get_mbh);
    lua_register(L, "mbr", l_get_mbr);

    lua_register(L, "uartsend", l_sendtextuart);
}