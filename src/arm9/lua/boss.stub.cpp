#include "thlua.h"
#include "../boss.h"
#include "../game.h"

int boss_addSpellcard(lua_State* L);
int boss_meta_index(lua_State* L);

static const struct luaL_reg glua_boss_f[] = {
	{"new", &glue_boss_new},
	{NULL, NULL}
};
static const struct luaL_reg glua_boss_m[] = {
	{"__gc", &glue_boss_gc},
	{"setName", &glue_boss_setName},
	{"addSpellcard", &boss_addSpellcard},
	{NULL, NULL}
};

int glua_register_boss(lua_State* L) {
	luaL_newmetatable(L, "BossMeta");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);
	luaL_openlib(L, NULL, glua_boss_m, 0);
	luaL_openlib(L, "Boss", glua_boss_f, 0);
	return 1;
}


int glue_boss_new(lua_State* L) {
	int numargs = lua_gettop(L);
	int objectRef = LUA_REFNIL;

	if (numargs == 0 || !lua_istable(L, 1)) {
		lua_newtable(L);
	} else {
		lua_pushvalue(L, 1);
	}

	int x = inttof32(128);
	int y = inttof32(128);
	if (numargs >= 4) {
		x = lua_tonumber(L, 2);
		y = lua_tonumber(L, 3);
	}

	//Top of the stack contains a table
	lua_pushvalue(L, -1);
	lua_pushcclosure(L, &boss_meta_index, 1);
	lua_setfield(L, -2, "__index");

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__newindex");

	lua_pushcfunction(L, &glue_boss_gc);
	lua_setfield(L, -2, "__gc");

	void* usrmem = lua_newuserdata(L, sizeof(Boss));
	Boss* obj = new(usrmem) Boss(cur_game,  cur_game->textures[TEX_boss],
			x, y, 32, 32, UV(0, 0, 32, 32));
	cur_game->SetBoss(obj);

	lua_pushvalue(L, -2);
	lua_setmetatable(L, -2);

	lua_pushvalue(L, -1);
	objectRef = luaL_ref(L, LUA_OBJREF_TABLE);
	obj->refcount++;

	obj->luaLink = new LuaObjectLink(L, objectRef);

	LuaMethodLink* animLink = new LuaMethodLink(L, objectRef, "animator");
	animLink->looping = true;
	obj->luaAnimatorLink = animLink;

	return 1;
}

int glue_boss_gc(lua_State* L) {
	return glue_generic_sprite_gc<Boss>(L);
}

int boss_meta_index(lua_State* L) {
	int res = luaMetaIndex(L, glua_boss_m);
	if (res == 0) {
		return luaMetaIndex(L, glua_sprite_m);
	}
	return res;
}

void boss_setName(Boss* obj, const char* name) {
	obj->SetName(name);
}

int boss_addSpellcard(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 5);
	Boss* obj = (Boss*)lua_touserdata(L, 1);
	obj->AddLuaSpellcard(lua_tostring(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4),
			lua_tostring(L, 5));
	return 0;
}
