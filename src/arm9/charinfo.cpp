#include "charinfo.h"

#include "game.h"
#include "player.h"
#include "controls.h"
#include "bombinfo.h"
#include "lua/thlua.h"
#include "parser/compiler.h"
#include "tcommon/parser/ini_parser.h"

CharInfo* charInfo = NULL;
u8 charInfoL = 0;

CharInfo* initCharacterInfo() {
    if (charInfo) {
        delete[] charInfo;
    }

    IniFile iniFile;
    FileList fileList("chara", ".ini");
    charInfoL = fileList.GetFilesL();
    charInfo = new CharInfo[charInfoL];

    int t = 0;
    char path[PATH_MAX];
    char* filename;
    while ((filename = fileList.NextFile()) != NULL) {
        sprintf(path, "chara/%s", filename);

        //Strip extension & Set ID to filename w/o ext.
        char* extpos = strrchr(filename, '.');
        *extpos = '\0';
        charInfo[t].SetId(filename);
        *extpos = '.';

        //Parse
        IniRecord* r;
        if (iniFile.Load(path)) {
            //Check version
            r = iniFile.GetRecord("script_version");
            if (r && !isValidScriptVersion(r->AsString())) {
            	log(EL_warning, __FILENAME__, "Illegal script version: %s in %s", r->AsString(), path);
                continue;
            }

            r = iniFile.GetRecord("version");
            if (r) charInfo[t].SetVersion(r->AsString());

            r = iniFile.GetRecord("code");
            if (r) charInfo[t].SetCode(r->AsString());
            r = iniFile.GetRecord("name");
            if (r) charInfo[t].SetName(r->AsString());

            r = iniFile.GetRecord("portrait");
            if (r) charInfo[t].SetPortrait(r->AsString());
            r = iniFile.GetRecord("desc");
            if (r) charInfo[t].SetDescription(r->AsString());

            r = iniFile.GetRecord("shot_power_mul");
            if (r) charInfo[t].SetShotPowerMul(r->AsFixed());
            r = iniFile.GetRecord("speed");
            if (r) charInfo[t].SetSpeed(r->AsFixed());
            r = iniFile.GetRecord("speed_focus");
            if (r) charInfo[t].SetSpeedFocus(r->AsFixed());
            r = iniFile.GetRecord("bombs");
            if (r) charInfo[t].SetBombs(r->AsInt());
            r = iniFile.GetRecord("graze_range");
            if (r) charInfo[t].SetGrazeRange(r->AsFixed());
            r = iniFile.GetRecord("hitbox");
            if (r) charInfo[t].SetHitRadius(r->AsFixed());

            r = iniFile.GetRecord("texture");
            if (r) charInfo[t].SetTexture(r->AsString());
            r = iniFile.GetRecord("bomb");
            if (r) charInfo[t].SetBomb(r->AsString());
            r = iniFile.GetRecord("bomb_focus");
            if (r) charInfo[t].SetBombFocus(r->AsString());
            t++;
        }
    }

    charInfoL = t;
    return charInfo;
}

CharInfo::CharInfo() {
    id = NULL;
    version = 0;
    code = NULL;
    portrait = NULL;
    name = NULL;
    desc = NULL;

    shotPowerMul = floattof32(1.0f);
    speed = floattof32(2.0f);
    speed_focus = floattof32(0.8f);
    bombs = 2;
    graze_range = 10;
    hitRadius = inttof32(2);

    texture = NULL;
    bomb = NULL;
    bomb_focus = NULL;

    SetId("default");
    SetPortrait("portrait.png");
    SetName("Nameless Character");
    SetDescription("No description");
}
CharInfo::~CharInfo() {
    if (id) delete[] id;
    if (code) delete[] code;

    if (portrait) delete[] portrait;
    if (name) delete[] name;
    if (desc) delete[] desc;

    if (texture) delete[] texture;
    if (bomb) delete[] bomb;
    if (bomb_focus) delete[] bomb_focus;
}

Player* CharInfo::CreatePlayer(Game* game, u8 playerId, Player* oldPlayer) {
    game->SetTexPlayer(id, texture);

    //Init corresponding Lua object
    lua_State* L = game->runtime.L;

    char temp[512];

    if (code && strcmp(code, "") != 0) {
    	sprintf(temp, "players[%d] = clone(%s)", playerId, code);
    } else {
    	sprintf(temp, "players[%d] = {}", playerId);
    }
    if (luaL_dostring(L, temp) != 0) {
    	sprintf(temp, "players[%d] = {}", playerId);
        if (luaL_dostring(L, temp) != 0) {
        	compilerLog(EL_error, __FILENAME__, "%s: %s", code, lua_tostring(L, -1));
        	return NULL;
        }
    }

    lua_getglobal(L, "players");
    sprintf(temp, "%d", playerId);

    void* usrmem = lua_newuserdata(L, sizeof(Player));
    Player* p = new(usrmem) Player(game, playerId, game->textures[TEX_player], id);

    //Get table from players[]
    lua_rawgeti(L, -2, playerId);

    luaInitPlayer(L, p);

    lua_pop(L, 1); //Pop metatable
    if (playerId == 0) {
    	lua_pushvalue(L, -1);
    	lua_setglobal(L, "player");
    }
    lua_setfield(L, -2, temp); //Store userdata in players[]
    lua_pop(L, 1); //Pop players[]

    if (!p) {
        return NULL;
    }

    p->SetLives(game->startingLives);

    //Set stats
    p->SetShotPowerMul(shotPowerMul);
    p->SetSpeed(speed);
    p->SetSpeedFocus(speed_focus);
    p->SetStartBombs(bombs);
    p->SetGrazeRange(graze_range);
    p->SetColCircle(0, hitRadius);

    //Set bombs
    BombInfo* bombInfo;

    bombInfo = (bomb ? bombFromString(bomb) : NULL);
    if (bombInfo) {
        p->SetBomb(bombInfo);
    } else {
        p->SetBomb(bombFromString("default"));
    }

    bombInfo = (bomb_focus ? bombFromString(bomb_focus) : NULL);
    if (bombInfo) p->SetBombFocus(bombInfo);

    return p;
}

char* CharInfo::GetId() {
    return id;
}
char* CharInfo::GetCode() {
    return code;
}
char* CharInfo::GetPortrait() {
    return portrait;
}
char* CharInfo::GetName() {
    return name;
}
char* CharInfo::GetDescription() {
    return desc;
}
s32 CharInfo::GetShotPowerMul() {
    return shotPowerMul;
}
s32 CharInfo::GetSpeed() {
    return speed;
}
s32 CharInfo::GetSpeedFocus() {
    return speed_focus;
}
u8 CharInfo::GetBombs() {
    return bombs;
}
s32 CharInfo::GetGrazeRange() {
    return graze_range;
}
s32 CharInfo::GetHitRadius() {
    return hitRadius;
}
char* CharInfo::GetTexture() {
    return texture;
}
char* CharInfo::GetBomb() {
    return bomb;
}
char* CharInfo::GetBombFocus() {
    return bomb_focus;
}
u32 CharInfo::GetVersion() {
	return version;
}

void CharInfo::SetString(char** dst, const char* src) {
    if (*dst) delete[] (*dst);

    *dst = new char[strlen(src) + 1];
    strcpy(*dst, src);
}
void CharInfo::SetVersion(char* v) {
	version = versionStringToInt(v);
}
void CharInfo::SetId(const char* i) {
    SetString(&id, i);
}
void CharInfo::SetCode(const char* c) {
    SetString(&code, c);
}
void CharInfo::SetPortrait(const char* p) {
    SetString(&portrait, p);
}
void CharInfo::SetName(const char* n) {
    SetString(&name, n);
}
void CharInfo::SetDescription(const char* d) {
    SetString(&desc, d);
}
void CharInfo::SetShotPowerMul(s32 s) {
    shotPowerMul = s;
}
void CharInfo::SetSpeed(s32 s) {
    speed = s;
}
void CharInfo::SetSpeedFocus(s32 sf) {
    speed_focus = sf;
}
void CharInfo::SetBombs(u8 b) {
    bombs = b;
}
void CharInfo::SetGrazeRange(s32 r) {
    graze_range = r;
}
void CharInfo::SetHitRadius(s32 r) {
    hitRadius = r;
}
void CharInfo::SetTexture(const char* t) {
    SetString(&texture, t);
}
void CharInfo::SetBomb(const char* b) {
    SetString(&bomb, b);
}
void CharInfo::SetBombFocus(const char* bf) {
    SetString(&bomb_focus, bf);
}

