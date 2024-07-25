#ifndef _GLUA_MATH_H_
#define _GLUA_MATH_H_

#include "thlua.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

int glua_register_math(lua_State* L);

int glue_math_abs(lua_State* L);
int glue_math_min(lua_State* L);
int glue_math_max(lua_State* L);
int glue_math_ceil(lua_State* L);
int glue_math_floor(lua_State* L);
int glue_math_round(lua_State* L);
int glue_math_signum(lua_State* L);
int glue_math_sin(lua_State* L);
int glue_math_asin(lua_State* L);
int glue_math_cos(lua_State* L);
int glue_math_acos(lua_State* L);
int glue_math_tan(lua_State* L);
int glue_math_atan2(lua_State* L);
int glue_math_angleBetween(lua_State* L);

#ifdef __cplusplus
}
#endif

s32 math_abs(s32 v);
s32 math_min(s32 a, s32 b);
s32 math_max(s32 a, s32 b);
int math_ceil(s32 v);
int math_floor(s32 v);
int math_round(s32 v);
s32 math_signum(s32 v);
s32 math_sin(s32 a);
s32 math_asin(s32 v);
s32 math_cos(s32 a);
s32 math_acos(s32 v);
s32 math_tan(s32 a);
s32 math_atan2(s32 dy, s32 dx);
s32 math_angleBetween(s32 x0, s32 y0, s32 x1, s32 y1);

#endif
