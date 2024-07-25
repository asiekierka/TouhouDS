#ifndef _GLUA_ENUMS_H_
#define _GLUA_ENUMS_H_

#include "thlua.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

int glua_register_enums(lua_State* L);

#ifdef __cplusplus
}
#endif

#endif
