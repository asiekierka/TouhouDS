#ifndef _GLUA_BOSS_H_
#define _GLUA_BOSS_H_

#include "thlua.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

int glua_register_boss(lua_State* L);

int glue_boss_new(lua_State* L);
int glue_boss_gc(lua_State* L);
int glue_boss_setName(lua_State* L);

#ifdef __cplusplus
}
#endif

void boss_setName(Boss* obj, const char* name);

#endif
