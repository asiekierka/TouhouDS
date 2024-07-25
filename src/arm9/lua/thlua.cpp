#include "thlua.h"
#include "../game.h"

Game* cur_game = NULL;
LuaLink* cur_luaLink = NULL;
Sprite* cur_sprite = NULL;

void luaGameInit(Game* game) {
	//Call main function
	cur_game = game;

	lua_State* L = game->runtime.L;

    lua_newtable(L);
    lua_setglobal(L, "players");
}

void luaCompileError(lua_State* L, int err) {
    const char* str = lua_tostring(L, -1);
    compilerLog(EL_error, "LUA", "[err=%d] => %s", err, str);
}
void luaError(lua_State* L, int err) {
    const char* str = lua_tostring(L, -1);
    log(EL_error, "LUA", "[err=%d] => %s", err, str);
}

int luaMetaIndex(lua_State* L, const luaL_reg* mreg) {
	//__index(lua_table table, string field)

	//Push current table (access from closure)
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_pushvalue(L, 2);

	//Try to find the field in the current table
	lua_rawget(L, -2);
	if (!lua_isnil(L, -1)) {
		//Field found, return
		return 1;
	}

	//Try to find the field in the sprite_meta table
	const char* str = lua_tostring(L, 2);
	if (str) {
		while (mreg->name) {
			if (strcmp(mreg->name, str) == 0) {
				lua_pushcfunction(L, mreg->func);
				return 1;
			}
			++mreg;
		}
	}

	//Nothing found
	return 0;
}

int l_wait(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);

	int frames = lua_tointeger(L, 1);
	cur_luaLink->wait += frames;
	return 0;
}

int luaRegisterFunctions(lua_State* L) {
    int ret = 0;
	ret += glua_register_enums(L);
	ret += glua_register_math(L);
	ret += glua_register_game(L);
	ret += glua_register_sprite(L);
	ret += glua_register_boss(L);

    lua_pushcfunction(L, &l_wait);
    lua_setglobal(L, "wait");

    lua_gc(L, LUAI_GCPAUSE, 50);

    return ret;
}
