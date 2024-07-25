#include "sprite.glue.h"

int glue_sprite_setPos(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 3);
#ifdef DEBUG
	sprite_setPos((Sprite*)lua_touserdata(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3));
#else
	sprite_setPos((Sprite*)lua_touserdata(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
#endif
	return 0;
}

int glue_sprite_setSpeed(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 2);
#ifdef DEBUG
	sprite_setSpeed((Sprite*)lua_touserdata(L, 1), luaL_checknumber(L, 2));
#else
	sprite_setSpeed((Sprite*)lua_touserdata(L, 1), lua_tonumber(L, 2));
#endif
	return 0;
}

int glue_sprite_setSpeedInc(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 2);
#ifdef DEBUG
	sprite_setSpeedInc((Sprite*)lua_touserdata(L, 1), luaL_checknumber(L, 2));
#else
	sprite_setSpeedInc((Sprite*)lua_touserdata(L, 1), lua_tonumber(L, 2));
#endif
	return 0;
}

int glue_sprite_setAngle(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 2);
#ifdef DEBUG
	sprite_setAngle((Sprite*)lua_touserdata(L, 1), luaL_checknumber(L, 2));
#else
	sprite_setAngle((Sprite*)lua_touserdata(L, 1), lua_tonumber(L, 2));
#endif
	return 0;
}

int glue_sprite_setAngleInc(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 2);
#ifdef DEBUG
	sprite_setAngleInc((Sprite*)lua_touserdata(L, 1), luaL_checknumber(L, 2));
#else
	sprite_setAngleInc((Sprite*)lua_touserdata(L, 1), lua_tonumber(L, 2));
#endif
	return 0;
}

int glue_sprite_setPower(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 2);
#ifdef DEBUG
	sprite_setPower((Sprite*)lua_touserdata(L, 1), luaL_checknumber(L, 2));
#else
	sprite_setPower((Sprite*)lua_touserdata(L, 1), lua_tonumber(L, 2));
#endif
	return 0;
}

int glue_sprite_setColCircle(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 3);
#ifdef DEBUG
	sprite_setColCircle((Sprite*)lua_touserdata(L, 1), luaL_checkinteger(L, 2), luaL_checknumber(L, 3));
#else
	sprite_setColCircle((Sprite*)lua_touserdata(L, 1), lua_tointeger(L, 2), lua_tonumber(L, 3));
#endif
	return 0;
}

int glue_sprite_setColSegment(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 6);
#ifdef DEBUG
	sprite_setColSegment((Sprite*)lua_touserdata(L, 1), luaL_checkinteger(L, 2), lua_toboolean(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5), luaL_checknumber(L, 6));
#else
	sprite_setColSegment((Sprite*)lua_touserdata(L, 1), lua_tointeger(L, 2), lua_toboolean(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5), lua_tonumber(L, 6));
#endif
	return 0;
}

int glue_sprite_removeColNode(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 2);
#ifdef DEBUG
	sprite_removeColNode((Sprite*)lua_touserdata(L, 1), luaL_checkinteger(L, 2));
#else
	sprite_removeColNode((Sprite*)lua_touserdata(L, 1), lua_tointeger(L, 2));
#endif
	return 0;
}

int glue_sprite_setDrawSize(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 3);
#ifdef DEBUG
	sprite_setDrawSize((Sprite*)lua_touserdata(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3));
#else
	sprite_setDrawSize((Sprite*)lua_touserdata(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
#endif
	return 0;
}

int glue_sprite_setDrawAngle(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 3);
#ifdef DEBUG
	sprite_setDrawAngle((Sprite*)lua_touserdata(L, 1), luaL_checkinteger(L, 2), luaL_checknumber(L, 3));
#else
	sprite_setDrawAngle((Sprite*)lua_touserdata(L, 1), lua_tointeger(L, 2), lua_tonumber(L, 3));
#endif
	return 0;
}

int glue_sprite_setVisible(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 2);
#ifdef DEBUG
	sprite_setVisible((Sprite*)lua_touserdata(L, 1), lua_toboolean(L, 2));
#else
	sprite_setVisible((Sprite*)lua_touserdata(L, 1), lua_toboolean(L, 2));
#endif
	return 0;
}

int glue_sprite_setImage(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 6);
#ifdef DEBUG
	sprite_setImage((Sprite*)lua_touserdata(L, 1), luaL_checkinteger(L, 2), luaL_checkinteger(L, 3), luaL_checkinteger(L, 4), luaL_checkinteger(L, 5), luaL_checkinteger(L, 6));
#else
	sprite_setImage((Sprite*)lua_touserdata(L, 1), lua_tointeger(L, 2), lua_tointeger(L, 3), lua_tointeger(L, 4), lua_tointeger(L, 5), lua_tointeger(L, 6));
#endif
	return 0;
}

int glue_sprite_setInvincibleTime(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 2);
#ifdef DEBUG
	sprite_setInvincibleTime((Sprite*)lua_touserdata(L, 1), luaL_checkinteger(L, 2));
#else
	sprite_setInvincibleTime((Sprite*)lua_touserdata(L, 1), lua_tointeger(L, 2));
#endif
	return 0;
}

int glue_sprite_setZ(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 2);
#ifdef DEBUG
	sprite_setZ((Sprite*)lua_touserdata(L, 1), luaL_checkinteger(L, 2));
#else
	sprite_setZ((Sprite*)lua_touserdata(L, 1), lua_tointeger(L, 2));
#endif
	return 0;
}

int glue_sprite_setDieOutsideBounds(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 2);
#ifdef DEBUG
	sprite_setDieOutsideBounds((Sprite*)lua_touserdata(L, 1), lua_toboolean(L, 2));
#else
	sprite_setDieOutsideBounds((Sprite*)lua_touserdata(L, 1), lua_toboolean(L, 2));
#endif
	return 0;
}

int glue_sprite_getX(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = sprite_getX((Sprite*)lua_touserdata(L, 1));
#else
	s32 r = sprite_getX((Sprite*)lua_touserdata(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_sprite_getY(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = sprite_getY((Sprite*)lua_touserdata(L, 1));
#else
	s32 r = sprite_getY((Sprite*)lua_touserdata(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_sprite_getSpeed(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = sprite_getSpeed((Sprite*)lua_touserdata(L, 1));
#else
	s32 r = sprite_getSpeed((Sprite*)lua_touserdata(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_sprite_getSpeedInc(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = sprite_getSpeedInc((Sprite*)lua_touserdata(L, 1));
#else
	s32 r = sprite_getSpeedInc((Sprite*)lua_touserdata(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_sprite_getAngle(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = sprite_getAngle((Sprite*)lua_touserdata(L, 1));
#else
	s32 r = sprite_getAngle((Sprite*)lua_touserdata(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_sprite_getAngleInc(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = sprite_getAngleInc((Sprite*)lua_touserdata(L, 1));
#else
	s32 r = sprite_getAngleInc((Sprite*)lua_touserdata(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_sprite_getPower(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = sprite_getPower((Sprite*)lua_touserdata(L, 1));
#else
	s32 r = sprite_getPower((Sprite*)lua_touserdata(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_sprite_getDrawWidth(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = sprite_getDrawWidth((Sprite*)lua_touserdata(L, 1));
#else
	s32 r = sprite_getDrawWidth((Sprite*)lua_touserdata(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_sprite_getDrawHeight(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = sprite_getDrawHeight((Sprite*)lua_touserdata(L, 1));
#else
	s32 r = sprite_getDrawHeight((Sprite*)lua_touserdata(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_sprite_getDrawAngle(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	s32 r = sprite_getDrawAngle((Sprite*)lua_touserdata(L, 1));
#else
	s32 r = sprite_getDrawAngle((Sprite*)lua_touserdata(L, 1));
#endif
	lua_pushnumber(L, r);
	return 1;
}

int glue_sprite_getDrawAngleMode(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	int r = sprite_getDrawAngleMode((Sprite*)lua_touserdata(L, 1));
#else
	int r = sprite_getDrawAngleMode((Sprite*)lua_touserdata(L, 1));
#endif
	lua_pushinteger(L, r);
	return 1;
}

int glue_sprite_isVisible(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	bool r = sprite_isVisible((Sprite*)lua_touserdata(L, 1));
#else
	bool r = sprite_isVisible((Sprite*)lua_touserdata(L, 1));
#endif
	lua_pushboolean(L, r);
	return 1;
}

int glue_sprite_isDestroyed(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	bool r = sprite_isDestroyed((Sprite*)lua_touserdata(L, 1));
#else
	bool r = sprite_isDestroyed((Sprite*)lua_touserdata(L, 1));
#endif
	lua_pushboolean(L, r);
	return 1;
}

int glue_sprite_getInvincibleTime(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	int r = sprite_getInvincibleTime((Sprite*)lua_touserdata(L, 1));
#else
	int r = sprite_getInvincibleTime((Sprite*)lua_touserdata(L, 1));
#endif
	lua_pushinteger(L, r);
	return 1;
}

int glue_sprite_getZ(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	int r = sprite_getZ((Sprite*)lua_touserdata(L, 1));
#else
	int r = sprite_getZ((Sprite*)lua_touserdata(L, 1));
#endif
	lua_pushinteger(L, r);
	return 1;
}

int glue_sprite_getDieOutsideBounds(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	bool r = sprite_getDieOutsideBounds((Sprite*)lua_touserdata(L, 1));
#else
	bool r = sprite_getDieOutsideBounds((Sprite*)lua_touserdata(L, 1));
#endif
	lua_pushboolean(L, r);
	return 1;
}

int glue_sprite_advance(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 2);
#ifdef DEBUG
	sprite_advance((Sprite*)lua_touserdata(L, 1), luaL_checknumber(L, 2));
#else
	sprite_advance((Sprite*)lua_touserdata(L, 1), lua_tonumber(L, 2));
#endif
	return 0;
}

int glue_sprite_advanceSide(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 2);
#ifdef DEBUG
	sprite_advanceSide((Sprite*)lua_touserdata(L, 1), luaL_checknumber(L, 2));
#else
	sprite_advanceSide((Sprite*)lua_touserdata(L, 1), lua_tonumber(L, 2));
#endif
	return 0;
}

int glue_sprite_die(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 1);
#ifdef DEBUG
	sprite_die((Sprite*)lua_touserdata(L, 1));
#else
	sprite_die((Sprite*)lua_touserdata(L, 1));
#endif
	return 0;
}

int glue_sprite_drop(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 3);
#ifdef DEBUG
	sprite_drop((Sprite*)lua_touserdata(L, 1), luaL_checkinteger(L, 2), luaL_checkinteger(L, 3));
#else
	sprite_drop((Sprite*)lua_touserdata(L, 1), lua_tointeger(L, 2), lua_tointeger(L, 3));
#endif
	return 0;
}

int glue_sprite_slowMove(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 4);
#ifdef DEBUG
	sprite_slowMove((Sprite*)lua_touserdata(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4));
#else
	sprite_slowMove((Sprite*)lua_touserdata(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
#endif
	return 0;
}

int glue_sprite_slowAccel(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 3);
#ifdef DEBUG
	sprite_slowAccel((Sprite*)lua_touserdata(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3));
#else
	sprite_slowAccel((Sprite*)lua_touserdata(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
#endif
	return 0;
}

int glue_sprite_slowTurn(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 3);
#ifdef DEBUG
	sprite_slowTurn((Sprite*)lua_touserdata(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3));
#else
	sprite_slowTurn((Sprite*)lua_touserdata(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
#endif
	return 0;
}

int glue_sprite_slowTurnTo(lua_State* L) {
	GLUA_CHECK_NUM_ARGS(L, 4);
#ifdef DEBUG
	sprite_slowTurnTo((Sprite*)lua_touserdata(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4));
#else
	sprite_slowTurnTo((Sprite*)lua_touserdata(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
#endif
	return 0;
}

