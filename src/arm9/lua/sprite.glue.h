#ifndef _GLUA_SPRITE_H_
#define _GLUA_SPRITE_H_

#include "thlua.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

int glua_register_sprite(lua_State* L);

int glue_sprite_new(lua_State* L);
int glue_sprite_gc(lua_State* L);
int glue_sprite_setPos(lua_State* L);
int glue_sprite_setSpeed(lua_State* L);
int glue_sprite_setSpeedInc(lua_State* L);
int glue_sprite_setAngle(lua_State* L);
int glue_sprite_setAngleInc(lua_State* L);
int glue_sprite_setPower(lua_State* L);
int glue_sprite_setColCircle(lua_State* L);
int glue_sprite_setColSegment(lua_State* L);
int glue_sprite_removeColNode(lua_State* L);
int glue_sprite_setDrawSize(lua_State* L);
int glue_sprite_setDrawAngle(lua_State* L);
int glue_sprite_setVisible(lua_State* L);
int glue_sprite_setImage(lua_State* L);
int glue_sprite_setInvincibleTime(lua_State* L);
int glue_sprite_setZ(lua_State* L);
int glue_sprite_setDieOutsideBounds(lua_State* L);
int glue_sprite_getX(lua_State* L);
int glue_sprite_getY(lua_State* L);
int glue_sprite_getSpeed(lua_State* L);
int glue_sprite_getSpeedInc(lua_State* L);
int glue_sprite_getAngle(lua_State* L);
int glue_sprite_getAngleInc(lua_State* L);
int glue_sprite_getPower(lua_State* L);
int glue_sprite_getDrawWidth(lua_State* L);
int glue_sprite_getDrawHeight(lua_State* L);
int glue_sprite_getDrawAngle(lua_State* L);
int glue_sprite_getDrawAngleMode(lua_State* L);
int glue_sprite_isVisible(lua_State* L);
int glue_sprite_isDestroyed(lua_State* L);
int glue_sprite_getInvincibleTime(lua_State* L);
int glue_sprite_getZ(lua_State* L);
int glue_sprite_getDieOutsideBounds(lua_State* L);
int glue_sprite_advance(lua_State* L);
int glue_sprite_advanceSide(lua_State* L);
int glue_sprite_die(lua_State* L);
int glue_sprite_drop(lua_State* L);
int glue_sprite_slowMove(lua_State* L);
int glue_sprite_slowAccel(lua_State* L);
int glue_sprite_slowTurn(lua_State* L);
int glue_sprite_slowTurnTo(lua_State* L);

#ifdef __cplusplus
}
#endif

void sprite_setPos(Sprite* obj, s32 x, s32 y);
void sprite_setSpeed(Sprite* obj, s32 spd);
void sprite_setSpeedInc(Sprite* obj, s32 spdi);
void sprite_setAngle(Sprite* obj, s32 a);
void sprite_setAngleInc(Sprite* obj, s32 ai);
void sprite_setPower(Sprite* obj, s32 pwr);
void sprite_setColCircle(Sprite* obj, int id, s32 rad);
void sprite_setColSegment(Sprite* obj, int id, bool autoRotate, s32 dx, s32 dy, s32 thickness);
void sprite_removeColNode(Sprite* obj, int id);
void sprite_setDrawSize(Sprite* obj, s32 w, s32 h);
void sprite_setDrawAngle(Sprite* obj, int drawAngleMode, s32 angle);
void sprite_setVisible(Sprite* obj, bool v);
void sprite_setImage(Sprite* obj, int texType, int x, int y, int w, int h);
void sprite_setInvincibleTime(Sprite* obj, int time);
void sprite_setZ(Sprite* obj, int z);
void sprite_setDieOutsideBounds(Sprite* obj, bool d);
s32 sprite_getX(Sprite* obj);
s32 sprite_getY(Sprite* obj);
s32 sprite_getSpeed(Sprite* obj);
s32 sprite_getSpeedInc(Sprite* obj);
s32 sprite_getAngle(Sprite* obj);
s32 sprite_getAngleInc(Sprite* obj);
s32 sprite_getPower(Sprite* obj);
s32 sprite_getDrawWidth(Sprite* obj);
s32 sprite_getDrawHeight(Sprite* obj);
s32 sprite_getDrawAngle(Sprite* obj);
int sprite_getDrawAngleMode(Sprite* obj);
bool sprite_isVisible(Sprite* obj);
bool sprite_isDestroyed(Sprite* obj);
int sprite_getInvincibleTime(Sprite* obj);
int sprite_getZ(Sprite* obj);
bool sprite_getDieOutsideBounds(Sprite* obj);
void sprite_advance(Sprite* obj, s32 dist);
void sprite_advanceSide(Sprite* obj, s32 dist);
void sprite_die(Sprite* obj);
void sprite_drop(Sprite* obj, int itemType, int amount);
void sprite_slowMove(Sprite* obj, s32 targetX, s32 targetY, s32 speed);
void sprite_slowAccel(Sprite* obj, s32 targetSpeed, s32 accel);
void sprite_slowTurn(Sprite* obj, s32 targetAngle, s32 turnSpeed);
void sprite_slowTurnTo(Sprite* obj, s32 targetX, s32 targetY, s32 turnSpeed);

#endif
