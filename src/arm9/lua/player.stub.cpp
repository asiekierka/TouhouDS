#include "thlua.h"
#include "../player.h"

int player_meta_index(lua_State* L);

static const struct luaL_reg glua_player_f[] = {
	{NULL, NULL}
};
static const struct luaL_reg glua_player_m[] = {
	{"__gc", &glue_player_gc},

	{"isFocus", &glue_player_isFocus},
	{"getAttackLevel", &glue_player_getAttackLevel},
	{"getAttackPower1", &glue_player_getAttackPower1},
	{"getAttackPower2", &glue_player_getAttackPower2},
	{"isButtonPressed", &glue_player_isButtonPressed},
	{"setBombCooldown", &glue_player_setBombCooldown},
	{NULL, NULL}
};

int glua_register_player(lua_State* L) {
	luaL_newmetatable(L, "PlayerMeta");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);
	luaL_openlib(L, NULL, glua_player_m, 0);
	luaL_openlib(L, "Player", glua_player_f, 0);
	return 1;
}

/*
 * Player must be allocated as a LUA userdata object.
 * stack[-2] = userdata
 * stack[-1] = table
 *
 * Function leaves the stack as is.
 */
void luaInitPlayer(lua_State* L, Player* player) {
	lua_pushvalue(L, -1);
	lua_pushcclosure(L, &player_meta_index, 1);
	lua_setfield(L, -2, "__index");

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__newindex");

	lua_pushcfunction(L, &glue_player_gc);
	lua_setfield(L, -2, "__gc");

	lua_pushvalue(L, -2);
	lua_pushvalue(L, -2);
	lua_setmetatable(L, -2);

	int objectRef = luaL_ref(L, LUA_OBJREF_TABLE);

	player->refcount++;
	player->luaLink = new LuaObjectLink(L, objectRef);

	LuaMethodLink* fireLink = new LuaMethodLink(L, objectRef, "fire");
	fireLink->looping = true;
	player->luaFireScript = fireLink;

	LuaMethodLink* animLink = new LuaMethodLink(L, objectRef, "animator");
	animLink->looping = true;
	player->luaAnimatorLink = animLink;
}

int glue_player_gc(lua_State* L) {
	return glue_generic_sprite_gc<Player>(L);
}

int player_meta_index(lua_State* L) {
	int res = luaMetaIndex(L, glua_player_m);
	if (res == 0) {
		return luaMetaIndex(L, glua_sprite_m);
	}
	return res;
}

bool player_isFocus(Player* obj) {
	return obj->IsFocus();
}

int player_getAttackLevel(Player* obj) {
	return obj->GetAttackLevel();
}

s32 player_getAttackPower1(Player* obj) {
	return obj->GetAttackPower1();
}

s32 player_getAttackPower2(Player* obj) {
	return obj->GetAttackPower2();
}

bool player_isButtonPressed(Player* obj, int button) {
	return obj->GetKeys() & button;
}

void player_setBombCooldown(Player* obj, int frames) {
	obj->SetBombCooldown(frames);
}
