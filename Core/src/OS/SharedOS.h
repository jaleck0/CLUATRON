#ifndef SHARED_H
#define SHARED_H

#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"

#include "../lua/lua.h"
#include "../lua/lualib.h"
#include "../lua/lauxlib.h"
#include "../lua/lualib.h"

#include "sd_card.h"
#include "ff.h"

#define PROMPT "> "

#define BACKCOL 1
#define TEXTCOL 11

typedef enum
{
    SHELL,
    CET
} Program;

char command[78];
uint32_t commandPos;

char file[32768];
uint32_t filePos;

lua_State *L;
luaL_Buffer buf;
int status;
size_t len;
char ch;

FRESULT fr;
FATFS fs;
FIL fil;
int ret;

Program currentProgram;

void SwitchProgram();

#endif