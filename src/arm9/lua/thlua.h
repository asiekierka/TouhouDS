#ifndef THLUA_H
#define THLUA_H

#include "../thds.h"
#include "../thlog.h"

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#define LUA_THREADREF_TABLE LUA_REGISTRYINDEX
#define LUA_OBJREF_TABLE	LUA_GLOBALSINDEX

#define GLUA_ERR(x, y...) log(EL_error, "LUA", x, ## y)

#ifndef GLUA_CHECK_NUM_ARGS
#define GLUA_NUM_ARGS_ERROR_FMT "LUA Error: Wrong number of args: expected %d, got %d.\n"
#define GLUA_CHECK_NUM_ARGS(L, num) \
	if (lua_gettop(L) != num) { \
		GLUA_ERR(GLUA_NUM_ARGS_ERROR_FMT, num, lua_gettop(L)); \
		return 0; \
	}
#endif

extern Game* cur_game;
extern LuaLink* cur_luaLink;
extern Sprite* cur_sprite;

int luaRegisterFunctions(lua_State* L);
int luaMetaIndex(lua_State* L, const luaL_reg* reg);

void luaGameInit(Game* game);
void luaCompileError(lua_State* L, int err);
void luaError(lua_State* L, int err);

extern const struct luaL_reg glua_sprite_m[];

template<class T>
int glue_generic_sprite_gc(lua_State* L) { \
	T* obj = (T*)lua_touserdata(L, 1);
	if (obj->refcount == 1) {
		obj->~T();
	} else {
		--obj->refcount;
	}
	return 0;
}

#include "lualink.h"
#include "enums.glue.h"
#include "math.glue.h"
#include "game.glue.h"
#include "sprite.glue.h"
#include "boss.glue.h"
#include "player.glue.h"

#endif
