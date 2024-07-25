#include "game.glue.h"

int glue_game_addThread(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	game_addThread(luaL_checkstring(L, 1));
#else
	game_addThread(lua_tostring(L, 1));
#endif
	return 0;
}

int glue_game_destroyAll(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	game_destroyAll(luaL_checkinteger(L, 1));
#else
	game_destroyAll(lua_tointeger(L, 1));
#endif
	return 0;
}

int glue_game_setBGM(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	game_setBGM(luaL_checkstring(L, 1));
#else
	game_setBGM(lua_tostring(L, 1));
#endif
	return 0;
}

int glue_game_playSFX(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	game_playSFX(luaL_checkstring(L, 1));
#else
	game_playSFX(lua_tostring(L, 1));
#endif
	return 0;
}

int glue_game_setBgScrollSpeed(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 2);
#ifdef DEBUG
	game_setBgScrollSpeed(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
#else
	game_setBgScrollSpeed(lua_tonumber(L, 1), lua_tonumber(L, 2));
#endif
	return 0;
}

int glue_game_endStage(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 0);
#ifdef DEBUG
	game_endStage();
#else
	game_endStage();
#endif
	return 0;
}

