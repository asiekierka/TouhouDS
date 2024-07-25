#ifndef _GLUA_PLAYER_H_
#define _GLUA_PLAYER_H_

#include "thlua.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

void luaInitPlayer(lua_State* L, Player* player);

int glue_player_gc(lua_State* L);
int glue_player_isFocus(lua_State* L);
int glue_player_getAttackLevel(lua_State* L);
int glue_player_getAttackPower1(lua_State* L);
int glue_player_getAttackPower2(lua_State* L);
int glue_player_isButtonPressed(lua_State* L);
int glue_player_setBombCooldown(lua_State* L);

#ifdef __cplusplus
}
#endif

bool player_isFocus(Player* obj);
int player_getAttackLevel(Player* obj);
s32 player_getAttackPower1(Player* obj);
s32 player_getAttackPower2(Player* obj);
bool player_isButtonPressed(Player* obj, int button);
void player_setBombCooldown(Player* obj, int frames);

#endif
