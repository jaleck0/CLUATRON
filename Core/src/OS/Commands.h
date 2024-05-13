#ifndef LUACOM_H
#define LUACOM_H

#include "../lua/lua.h" 
#include "../lua/lualib.h"
#include "../lua/lauxlib.h"

extern void RegisterConstants(lua_State* L);
extern void RegisterCommands(lua_State* L);

#endif