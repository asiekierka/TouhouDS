#include "thlua.h"
#include "../game.h"
#include "../gameinfo.h"
#include "../level.h"

static const struct luaL_reg glua_game_f[] = {
	{"addThread", &glue_game_addThread},
	{"destroyAll", &glue_game_destroyAll},
	{"setBGM", &glue_game_setBGM},
	{"playSFX", &glue_game_playSFX},
	{"setBgScrollSpeed", &glue_game_setBgScrollSpeed},
	{"endStage", &glue_game_endStage},
	{NULL, NULL}
};
static const struct luaL_reg glua_game_m[] = {
	{NULL, NULL}
};

int glua_register_game(lua_State* L) {
	luaL_newmetatable(L, "GameMeta");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);
	luaL_openlib(L, NULL, glua_game_m, 0);
	luaL_openlib(L, "Game", glua_game_f, 0);
	return 1;
}

void game_addThread(const char* func) {
	cur_game->level->AddLuaThread(new LuaFunctionLink(cur_game->runtime.L, func));
}

void game_destroyAll(int spriteType) {
	cur_game->ConvertSpritesToPoints(spriteType);
}

void game_setBGM(const char* filename) {
    GameInfo* gi = cur_game->GetSelectedGame();
    if (gi) {
        char path[PATH_MAX];
        sprintf(path, "games/%s/bgm/%s", gi->GetId(), filename);
        if (!fexists(path)) {
            sprintf(path, "bgm/%s", filename);
        }
        cur_game->soundManager.SetBGM(path);
    }
}

void game_playSFX(const char* filename) {
	cur_game->soundManager.PlaySound(filename);
}

void game_setBgScrollSpeed(s32 sx, s32 sy) {
	cur_game->level->SetBgScrollSpeed(sx, sy);
}

void game_endStage() {
	cur_game->EndStage();
}
