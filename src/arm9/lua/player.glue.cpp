#include "player.glue.h"

int glue_player_isFocus(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	bool r = player_isFocus((Player*)lua_touserdata(L, 1));
#else
	bool r = player_isFocus((Player*)lua_touserdata(L, 1));
#endif
	lua_pushboolean(L, r);
	return 1;
}

int glue_player_getAttackLevel(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	int r = player_getAttackLevel((Player*)lua_touserdata(L, 1));
#else
	int r = player_getAttackLevel((Player*)lua_touserdata(L, 1));
#endif
	lua_pushinteger(L, r);
	return 1;
}

int glue_player_getAttackPower1(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = player_getAttackPower1((Player*)lua_touserdata(L, 1));
#else
	s32 r = player_getAttackPower1((Player*)lua_touserdata(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_player_getAttackPower2(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = player_getAttackPower2((Player*)lua_touserdata(L, 1));
#else
	s32 r = player_getAttackPower2((Player*)lua_touserdata(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_player_isButtonPressed(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 2);
#ifdef DEBUG
	bool r = player_isButtonPressed((Player*)lua_touserdata(L, 1), luaL_checkinteger(L, 2));
#else
	bool r = player_isButtonPressed((Player*)lua_touserdata(L, 1), lua_tointeger(L, 2));
#endif
	lua_pushboolean(L, r);
	return 1;
}

int glue_player_setBombCooldown(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 2);
#ifdef DEBUG
	player_setBombCooldown((Player*)lua_touserdata(L, 1), luaL_checkinteger(L, 2));
#else
	player_setBombCooldown((Player*)lua_touserdata(L, 1), lua_tointeger(L, 2));
#endif
	return 0;
}

