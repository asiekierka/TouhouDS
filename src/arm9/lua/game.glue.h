#ifndef _GLUA_GAME_H_
#define _GLUA_GAME_H_

#include "thlua.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

int glua_register_game(lua_State* L);

int glue_game_addThread(lua_State* L);
int glue_game_destroyAll(lua_State* L);
int glue_game_setBGM(lua_State* L);
int glue_game_playSFX(lua_State* L);
int glue_game_setBgScrollSpeed(lua_State* L);
int glue_game_endStage(lua_State* L);

#ifdef __cplusplus
}
#endif

void game_addThread(const char* func);
void game_destroyAll(int spriteType);
void game_setBGM(const char* filename);
void game_playSFX(const char* filename);
void game_setBgScrollSpeed(s32 sx, s32 sy);
void game_endStage();

#endif
