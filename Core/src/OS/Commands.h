#ifndef LUACOM_H
#define LUACOM_H

#include "../lua/lua.h" 
#include "../lua/lualib.h"
#include "../lua/lauxlib.h"

extern void RegisterCommands(lua_State* L);

#endif