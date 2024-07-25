#include "enums.glue.h"

static const struct luaL_reg glua_enums_f[] = {
	{NULL, NULL}
};
static const struct luaL_reg glua_enums_m[] = {
	{NULL, NULL}
};
static void glua_setEnumField(lua_State* L, const char* name, int value) {
	lua_pushstring(L, name);
	lua_pushinteger(L, value);
	lua_settable(L, -3);
	lua_pushinteger(L, value);
	lua_setglobal(L, name);
}

int glua_register_enums(lua_State* L) {
	lua_newtable(L);
	glua_setEnumField(L, "AT_none", AT_none);
	glua_setEnumField(L, "AT_player", AT_player);
	glua_setEnumField(L, "AT_boss", AT_boss);
	glua_setEnumField(L, "AT_enemy", AT_enemy);
	glua_setEnumField(L, "AT_center", AT_center);
	glua_setEnumField(L, "AT_top", AT_top);
	glua_setEnumField(L, "AT_bottom", AT_bottom);
	glua_setEnumField(L, "AT_left", AT_left);
	glua_setEnumField(L, "AT_right", AT_right);
	lua_setglobal(L, "AngleTarget");

	lua_newtable(L);
	glua_setEnumField(L, "IT_null", IT_null);
	glua_setEnumField(L, "IT_point", IT_point);
	glua_setEnumField(L, "IT_pointLarge", IT_pointLarge);
	glua_setEnumField(L, "IT_power", IT_power);
	glua_setEnumField(L, "IT_powerLarge", IT_powerLarge);
	glua_setEnumField(L, "IT_extra", IT_extra);
	glua_setEnumField(L, "IT_extraLarge", IT_extraLarge);
	glua_setEnumField(L, "IT_life", IT_life);
	glua_setEnumField(L, "IT_bomb", IT_bomb);
	glua_setEnumField(L, "IT_fullPower", IT_fullPower);
	glua_setEnumField(L, "IT_magnetPoint", IT_magnetPoint);
	lua_setglobal(L, "ItemType");

	lua_newtable(L);
	glua_setEnumField(L, "CAT_idle", CAT_idle);
	glua_setEnumField(L, "CAT_left", CAT_left);
	glua_setEnumField(L, "CAT_right", CAT_right);
	glua_setEnumField(L, "CAT_tweenLeft", CAT_tweenLeft);
	glua_setEnumField(L, "CAT_tweenRight", CAT_tweenRight);
	lua_setglobal(L, "CharaAnimationType");

	lua_newtable(L);
	glua_setEnumField(L, "DAM_auto", DAM_auto);
	glua_setEnumField(L, "DAM_manual", DAM_manual);
	glua_setEnumField(L, "DAM_rotate", DAM_rotate);
	lua_setglobal(L, "DrawAngleMode");

	lua_newtable(L);
	glua_setEnumField(L, "TEX_enemy", TEX_enemy);
	glua_setEnumField(L, "TEX_bullet", TEX_bullet);
	glua_setEnumField(L, "TEX_bullet2", TEX_bullet2);
	glua_setEnumField(L, "TEX_item", TEX_item);
	glua_setEnumField(L, "TEX_player", TEX_player);
	glua_setEnumField(L, "TEX_playerFx", TEX_playerFx);
	glua_setEnumField(L, "TEX_explosion", TEX_explosion);
	glua_setEnumField(L, "TEX_background", TEX_background);
	glua_setEnumField(L, "TEX_boss", TEX_boss);
	glua_setEnumField(L, "NUM_TEXTURE_TYPES", NUM_TEXTURE_TYPES);
	lua_setglobal(L, "TextureType");

	lua_newtable(L);
	glua_setEnumField(L, "SPRITE_default", SPRITE_default);
	glua_setEnumField(L, "SPRITE_remote", SPRITE_remote);
	glua_setEnumField(L, "SPRITE_player", SPRITE_player);
	glua_setEnumField(L, "SPRITE_playerShot", SPRITE_playerShot);
	glua_setEnumField(L, "SPRITE_item", SPRITE_item);
	glua_setEnumField(L, "SPRITE_enemy", SPRITE_enemy);
	glua_setEnumField(L, "SPRITE_enemyShot", SPRITE_enemyShot);
	glua_setEnumField(L, "NUM_SPRITE_TYPES", NUM_SPRITE_TYPES);
	lua_setglobal(L, "SpriteType");

	lua_newtable(L);
	glua_setEnumField(L, "VK_left", VK_left);
	glua_setEnumField(L, "VK_right", VK_right);
	glua_setEnumField(L, "VK_up", VK_up);
	glua_setEnumField(L, "VK_down", VK_down);
	glua_setEnumField(L, "VK_fire", VK_fire);
	glua_setEnumField(L, "VK_focus", VK_focus);
	glua_setEnumField(L, "VK_bomb", VK_bomb);
	lua_setglobal(L, "Keys");

	return 0;
}



