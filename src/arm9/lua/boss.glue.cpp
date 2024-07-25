#include "boss.glue.h"

int glue_boss_setName(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 2);
#ifdef DEBUG
	boss_setName((Boss*)lua_touserdata(L, 1), luaL_checkstring(L, 2));
#else
	boss_setName((Boss*)lua_touserdata(L, 1), lua_tostring(L, 2));
#endif
	return 0;
}

