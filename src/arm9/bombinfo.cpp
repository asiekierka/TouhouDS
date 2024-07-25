#include "bombinfo.h"

#include "game.h"
#include "player.h"
#include "custom/custom.h"
#include "tcommon/parser/ini_parser.h"

BombInfo* bombInfo = NULL;
u8 bombInfoL = 0;

BombInfo* initBombInfo() {
    if (bombInfo) {
        delete[] bombInfo;
    }

    IniFile iniFile;
    FileList fileList("bomb", ".ini");
    bombInfoL = fileList.GetFilesL();
    bombInfo = new BombInfo[bombInfoL];

    int t = 0;
    char path[MAXPATHLEN];
    char* filename;
    while ((filename = fileList.NextFile()) != NULL) {
        sprintf(path, "bomb/%s", filename);

        //Strip extension & Set ID to filename w/o ext.
        char* extpos = strrchr(filename, '.');
        *extpos = '\0';
        bombInfo[t].SetId(filename);
        *extpos = '.';

        IniRecord* r;
        if (iniFile.Load(path)) {
            //Check version
            r = iniFile.GetRecord("script_version");
            if (r && !isValidScriptVersion(r->AsString())) {
            	log(EL_warning, __FILENAME__, "Illegal script version: %s in %s", r->AsString(), path);
                continue;
            }

            r = iniFile.GetRecord("code");
            if (r) bombInfo[t].SetCode(r->AsString());
            r = iniFile.GetRecord("name");
            if (r) bombInfo[t].SetName(r->AsString());
            r = iniFile.GetRecord("texture");
            if (r) bombInfo[t].SetTexture(r->AsString());

            t++;
        }
    }

    bombInfoL = t;
    return bombInfo;
}

BombInfo* bombFromString(const char* bombId) {
    if (!bombId) return NULL;

    for (int n = 0; n < bombInfoL; n++) {
        if (strcmp(bombInfo[n].GetId(), bombId) == 0) {
            return &bombInfo[n];
        }
    }

    return NULL;
}


BombInfo::BombInfo() {
    id = NULL;
    code = NULL;
    name = NULL;
    texture = NULL;
}
BombInfo::~BombInfo() {
    if (id) delete[] id;
    if (code) delete[] code;
    if (name) delete[] name;
    if (texture) delete[] texture;
}

Bomb* BombInfo::CreateBomb(Game* game, Player* player, bool isPrimary) {
    Texture* tex = (isPrimary ? game->texBomb1 : game->texBomb2);

    Bomb* b = NULL;
    if (code && strcmp(code, "master_spark") == 0) {
        b = new Bomb_MasterSpark(game, player, tex);
    } else if (code && strcmp(code, "bomb_reimu") == 0) {
        b = new Bomb_Reimu(game, player, tex);
    } else if (code && strcmp(code, "fangirl_youmu") == 0) {
        b = new Bomb_FangirlYoumu(game, player, tex);
    } else if (code && strcmp(code, "sakuya") == 0) {
        b = new Bomb_Sakuya(game, player, tex);
    } else if (code && strcmp(code, "youmu") == 0) {
        b = new Bomb_Youmu(game, player, tex);
    } else if (code && strcmp(code, "yuka") == 0) {
        b = new Bomb_Yuka(game, player, tex);
    } else if (code && strcmp(code, "sanae") == 0) {
        b = new Bomb_Sanae(game, player, tex);
    } else {
        b = new Bomb_MoF(game, player, tex);
    }
    if (!b) {
        return NULL;
    }

    return b;
}

char* BombInfo::GetId() {
    return id;
}
char* BombInfo::GetCode() {
    return code;
}
char* BombInfo::GetName() {
    return name;
}
char* BombInfo::GetTexture() {
    return texture;
}

void BombInfo::SetString(char** dst, const char* src) {
    if (*dst) delete[] (*dst);

    *dst = new char[strlen(src) + 1];
    strcpy(*dst, src);
}
void BombInfo::SetId(const char* i) {
    SetString(&id, i);
}
void BombInfo::SetCode(const char* c) {
    SetString(&code, c);
}
void BombInfo::SetName(const char* n) {
    SetString(&name, n);
}
void BombInfo::SetTexture(const char* t) {
    SetString(&texture, t);
}

