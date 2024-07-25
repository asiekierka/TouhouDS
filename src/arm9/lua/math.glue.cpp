#include "math.glue.h"

int glue_math_abs(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = math_abs(luaL_checknumber(L, 1));
#else
	s32 r = math_abs(lua_tonumber(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_math_min(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 2);
#ifdef DEBUG
	s32 r = math_min(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
#else
	s32 r = math_min(lua_tonumber(L, 1), lua_tonumber(L, 2));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_math_max(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 2);
#ifdef DEBUG
	s32 r = math_max(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
#else
	s32 r = math_max(lua_tonumber(L, 1), lua_tonumber(L, 2));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_math_ceil(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = math_ceil(luaL_checknumber(L, 1));
#else
	s32 r = math_ceil(lua_tonumber(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_math_floor(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = math_floor(luaL_checknumber(L, 1));
#else
	s32 r = math_floor(lua_tonumber(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_math_round(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = math_round(luaL_checknumber(L, 1));
#else
	s32 r = math_round(lua_tonumber(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_math_signum(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = math_signum(luaL_checknumber(L, 1));
#else
	s32 r = math_signum(lua_tonumber(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_math_sin(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = math_sin(luaL_checknumber(L, 1));
#else
	s32 r = math_sin(lua_tonumber(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_math_asin(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = math_asin(luaL_checknumber(L, 1));
#else
	s32 r = math_asin(lua_tonumber(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_math_cos(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = math_cos(luaL_checknumber(L, 1));
#else
	s32 r = math_cos(lua_tonumber(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_math_acos(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = math_acos(luaL_checknumber(L, 1));
#else
	s32 r = math_acos(lua_tonumber(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_math_tan(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = math_tan(luaL_checknumber(L, 1));
#else
	s32 r = math_tan(lua_tonumber(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_math_atan2(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 2);
#ifdef DEBUG
	s32 r = math_atan2(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
#else
	s32 r = math_atan2(lua_tonumber(L, 1), lua_tonumber(L, 2));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_math_angleBetween(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 4);
#ifdef DEBUG
	s32 r = math_angleBetween(luaL_checknumber(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4));
#else
	s32 r = math_angleBetween(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
#endif
	lua_pushnumber(L, r);
	return 1;
}

