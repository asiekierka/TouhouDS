#include "lua/thlua.h"
#include "level.h"
#include "game.h"
#include "sprite.h"
#include "spells/common_spells.h"
#include "tcommon/text.h"
#include "tcommon/parser/ini_parser.h"

using namespace std;

Level::Level(Game* g, u32 score, const char* f) {
    folder = strdup(f);
    game = g;
    frame = 1;

    AddLuaThread(new LuaFunctionLink(g->runtime.L, "main"));

    name = NULL;
    desc = NULL;
    unlocks = NULL;
    unlocksL = 0;

    startScore = score;
    livesLost = 0;
    bombsUsed = 0;

    bgScrollX = 0;
    bgScrollY = 0;
    targetBgScrollDx = bgScrollDx = 0;
    targetBgScrollDy = bgScrollDy = inttof32(1);

    char texPlayerFx[128] = "img/game/tex_player_fx:A3I5";
    char texItem[128] = "img/game/tex_item:A3I5";
    char texBullet[128] = "img/game/tex_bullet:A3I5";
    char texBullet2[128] = "img/game/tex_bullet2:A3I5";
    char texEnemy[128] = "img/game/tex_enemy:A3I5";
    char texExplosion[128] = "img/game/tex_explosion:A5I3";
    char texBoss[128] = "img/game/tex_boss:A3I5";
    char texBackground[128] = "img/game/tex_background:RGB256";

    //Parse ini
    IniFile iniFile;
    char iniPath[MAXPATHLEN];
    sprintf(iniPath, "%s.ini", folder);
    if (iniFile.Load(iniPath)) {
        IniRecord* r;

        r = iniFile.GetRecord("name");
        if (r) name = strdup(r->AsString());
        r = iniFile.GetRecord("desc");
        if (r) desc = strdup(r->AsString());

        r = iniFile.GetRecord("unlocks");
        if (r) SetUnlocksString(r->AsString());

        r = iniFile.GetRecord("tex_player_fx");
        if (r) sprintf(texPlayerFx, "%s/%s", folder, r->AsString());
        r = iniFile.GetRecord("tex_bullet");
        if (r) sprintf(texBullet, "%s/%s", folder, r->AsString());
        r = iniFile.GetRecord("tex_bullet2");
        if (r) sprintf(texBullet2, "%s/%s", folder, r->AsString());
        r = iniFile.GetRecord("tex_item");
        if (r) sprintf(texItem, "%s/%s", folder, r->AsString());
        r = iniFile.GetRecord("tex_enemy");
        if (r) sprintf(texEnemy, "%s/%s", folder, r->AsString());
        r = iniFile.GetRecord("tex_explosion");
        if (r) sprintf(texExplosion, "%s/%s", folder, r->AsString());
        r = iniFile.GetRecord("tex_boss");
        if (r) sprintf(texBoss, "%s/%s", folder, r->AsString());
        r = iniFile.GetRecord("tex_background");
        if (r) sprintf(texBackground, "%s/%s", folder, r->AsString());
    }

    game->textures[TEX_playerFx] = game->LoadTexture(".", texPlayerFx);
    game->textures[TEX_item] = game->LoadTexture(".", texItem);
    game->textures[TEX_bullet] = game->LoadTexture(".", texBullet);
    game->textures[TEX_bullet2] = game->LoadTexture(".", texBullet2);
    game->textures[TEX_enemy] = game->LoadTexture(".", texEnemy);
    game->textures[TEX_explosion] = game->LoadTexture(".", texExplosion);
    game->textures[TEX_boss] = game->LoadTexture(".", texBoss);
    game->textures[TEX_background] = game->LoadTexture(".", texBackground,
			(GL_TEXTURE_PARAM_ENUM)(TEXGEN_TEXCOORD|GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T));
}

Level::~Level() {
	list<SpellBook*>::iterator sbIt = spellBooks.begin();
	while (sbIt != spellBooks.end()) {
		delete *sbIt;
		++sbIt;
	}
	spellBooks.clear();

	list<LuaLink*>::iterator luaIt = luaThreads.begin();
	while (luaIt != luaThreads.end()) {
		delete *luaIt;
		++luaIt;
	}
	luaThreads.clear();

    //Delete other stuff
    if (folder) free(folder);
    if (name) free(name);
    if (desc) free(desc);
    if (unlocks) {
        for (int n = 0; n < unlocksL; n++) {
            if (unlocks[n]) free(unlocks[n]);
        }
        delete[] unlocks;
    }
}

const char* Level::GetFolder() {
	return folder;
}

void Level::BannerInit() {
    char str[256];

    Text* text = game->GetText();
    text->ClearBuffer();

    text->SetFontSize(18);
    text->SetMargins(16, 16, 16, 16);
    if (name) {
        strcpy(str, name);
    } else {
        sprintf(str, "Stage %d", game->GetStage());
    }
    text->PrintString(str);

    text->SetFontSize(12);
    text->SetMargins(16, 16, 64, 16);
    sprintf(str, "%255s", (desc ? desc : ""));
    text->PrintString(str);

    const int s = 256 * 192;
    u16* buffer = new u16[s];
    for (int n = 0; n < s; n++) {
        buffer[n] = BIT(15);
    }
    text->BlitToScreen(buffer, 256, 192);
    for (int n = 0; n < s; n++) {
        if ((buffer[n] & 31) <= 7) {
            buffer[n] = 0;
        }
    }

    DC_FlushRange(buffer, s*sizeof(u16));
    dmaCopy(buffer, game->main_bg, s*sizeof(u16));
    delete[] buffer;
}

void Level::BannerDestroy() {
    memset(game->main_bg, 0, 256*192*sizeof(u16));
}

void Level::UpdateSpellBooks() {
	list<SpellBook*>::iterator it = spellBooks.begin();
	while (it != spellBooks.end()) {
		SpellBook* val = *it;
        if (val && !val->finished) {
            val->Update(game, NULL);
        }
        if (!val || val->finished) {
        	delete *it;
        	it = spellBooks.erase(it);
        } else {
    		++it;
        }
	}
}

void Level::UpdateLuaThreads() {
	list<LuaLink*>::iterator it = luaThreads.begin();
	while (it != luaThreads.end()) {
		LuaLink* val = *it;
        if (val && !val->finished) {
        	val->Update();
        }
        if (!val || val->finished) {
        	delete *it;
        	it = luaThreads.erase(it);
        } else {
    		++it;
        }
	}

	//if ((frame & 31) == 0) {
	//  lua_State* L = game->runtime.L;
	//	lua_gc(L, LUA_GCSTEP, 0);
	//}

#ifdef DEBUG
	if (keysDown() & (KEY_L|KEY_SELECT)) {
		lua_State* L = game->runtime.L;
		lua_gc(L, LUA_GCCOLLECT, 0);
	}
#endif

}

void Level::Update() {
	if (frame == 1) {
		BannerInit();
	} else if (frame == BANNER_TIME) {
		BannerDestroy();
	} else if (frame > BANNER_TIME) {
		UpdateSpellBooks();
		UpdateLuaThreads();
	}

    if (bgScrollDx != targetBgScrollDx) {
    	bgScrollDx = (15 * bgScrollDx + targetBgScrollDx) >> 4;
    }
    if (bgScrollDy != targetBgScrollDy) {
    	bgScrollDy = (15 * bgScrollDy + targetBgScrollDy) >> 4;
    }

    bgScrollX += bgScrollDx;
    bgScrollY += bgScrollDy;

    frame++;
}

void Level::UnlockRoute(const char* routeId) {
    char target[MAXPATHLEN];

    int index = strlen(folder)-1;
    for (int n = 0; n < 2; n++) {
        //folder is GAME_FOLDER/ROUTE_FOLDER/STAGE_FOLDER
        //we want   GAME_FOLDER
        //so we remove the last 2 parts
        while (index >= 0 && folder[index] == '/') index--;
        while (index >= 0 && folder[index] != '/') index--;
    }

    if (index < 0) { //Invalid path
        return;
    }

    //Set target to "routeFolder/<routeId>.ini"
    sprintf(name, "/%s.ini", routeId);
    strncpy(target, folder, index+1);
    target[index+1] = '\0';
    strcat(target, name);

    IniFile iniFile;
    if (iniFile.Load(target)) {
        iniFile.SetRecord("locked", "false");
        if (!iniFile.Save(target)) {
            log(EL_warning, __FILENAME__, "Unable to unlock: %s -- unable to write %s.ini", routeId, routeId);
        }
    }
}

void Level::OnFinished() {
    for (int n = 0; n < unlocksL; n++) {
        UnlockRoute(unlocks[n]);
    }
}

void Level::OnBombUsed() {
    bombsUsed++;
}
void Level::OnPlayerKilled() {
    livesLost++;
}

void Level::AddThread(SpellBook* sb) {
	spellBooks.push_back(sb);
}
void Level::AddLuaThread(LuaFunctionLink* t) {
	luaThreads.push_back(t);
}

void Level::SetUnlocksString(const char* unlocksString) {
    char* s = new char[strlen(unlocksString)+1];
    strcpy(s, unlocksString);

    //Delete old value
    if (unlocks) {
        for (int n = 0; n < unlocksL; n++) {
            free(unlocks[n]);
        }
        delete[] unlocks;
    }

    //Determing max number of list elements
    int maxTokens = 1;
    char* temp = s;
    while (*temp != '\0') {
        if (*temp == ',') maxTokens++;
        temp++;
    }

    //Place tokens in the array
    unlocks = new char*[maxTokens];
    temp = strtok(s, ", ");
    int t = 0;
    while (temp) {
        unlocks[t] = strdup(temp);
        temp = strtok(NULL, ", ");
        t++;
    }
    unlocksL = t;

    //Cleanup
    delete[] s;
}

void Level::SetBgScrollSpeed(s32 dx, s32 dy) {
	targetBgScrollDx = dx;
	targetBgScrollDy = dy;
}
