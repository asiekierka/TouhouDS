#include <ctime>

#include "gameinfo.h"

#include "thcommon.h"
#include "custom/custom.h"
#include "tcommon/parser/ini_parser.h"

GameInfo** gameInfo = NULL;
u8 gameInfoL = 0;

void initRoutes(GameInfo* gi);

GameInfo** initGameInfo() {
    freeGameInfo();

    IniFile iniFile;
    FileList fileList("games", ".ini");
    gameInfoL = fileList.GetFilesL();
    gameInfo = new GameInfo*[gameInfoL];

    int t = 0;
    char path[PATH_MAX];
    char* filename;
    while ((filename = fileList.NextFile()) != NULL) {
        sprintf(path, "games/%s", filename);

        IniRecord* r;
        if (iniFile.Load(path)) {
            //Check version
            r = iniFile.GetRecord("script_version");
            if (r && !isValidScriptVersion(r->AsString())) {
            	log(EL_warning, __FILENAME__, "Illegal script version: %s in %s", r->AsString(), path);
                continue;
            }

            gameInfo[t] = new GameInfo();

            //Strip extension & Set ID to filename w/o ext.
            char* extpos = strrchr(filename, '.');
            *extpos = '\0';
            gameInfo[t]->SetId(filename);
            *extpos = '.';

            r = iniFile.GetRecord("version");
            if (r) gameInfo[t]->SetVersion(r->AsString());
            r = iniFile.GetRecord("name");
            if (r) gameInfo[t]->SetName(r->AsString());
            r = iniFile.GetRecord("desc");
            if (r) gameInfo[t]->SetDesc(r->AsString());
            r = iniFile.GetRecord("banner");
            if (r) gameInfo[t]->SetBanner(r->AsString());

            t++;
        }
    }

    for (int n = 0; n < t; n++) {
        initRoutes(gameInfo[n]);
    }

    gameInfoL = t;
    return gameInfo;
}

void initRoutes(GameInfo* gi) {
    char folder[PATH_MAX];
    sprintf(folder, "games/%s", gi->GetId());

    IniFile* iniFile = new IniFile();
    FileList* fileList = new FileList(folder, ".ini");
    u8 routesL = fileList->GetFilesL();
    gi->SetNumRoutes(routesL);

    int t = 0;
    char path[PATH_MAX];
    char* filename;
    while ((filename = fileList->NextFile()) != NULL) {
        sprintf(path, "%s/%s", folder, filename);

        IniRecord* r;
        if (iniFile->Load(path)) {
            r = iniFile->GetRecord("locked");
            if (!r || !r->AsBool()) {
                //Strip extension & Set ID to filename w/o ext.
                char* extpos = strrchr(filename, '.');
                *extpos = '\0';
                gi->SetRouteId(t, filename);
                *extpos = '.';

                r = iniFile->GetRecord("name");
                if (r) gi->SetRouteName(t, r->AsString());

                t++;
            }
        }
    }

    delete fileList;
    delete iniFile;

    gi->routesL = t;
}

void freeGameInfo() {
    if (gameInfo) {
        for (int n = 0; n < gameInfoL; n++) {
            delete gameInfo[n];
        }
        delete[] gameInfo;
    }
    gameInfo = NULL;
    gameInfoL = 0;
}

GameInfo::GameInfo() {
    id = NULL;
    version = 0;
    name = NULL;
    desc = NULL;
    banner = NULL;

    routesL = 0;
    routeIds = NULL;
    routeNames = NULL;
}
GameInfo::~GameInfo() {
    if (id) delete[] id;
    if (name) delete[] name;
    if (desc) delete[] desc;
    if (banner) delete[] banner;

    if (routeIds) {
        for (int n = 0; n < routesL; n++) {
            if (routeIds[n]) delete[] routeIds[n];
            if (routeNames[n]) delete[] routeNames[n];
        }
        delete[] routeIds;
        delete[] routeNames;
    }
}

void insertScoreAndSave(char* path, ScoreRecord* records, u16 recordsL, ScoreRecord* newEntry) {
    //Collect other entries with the same route/chara
    ScoreRecord* entries[recordsL + 1];

    int entriesL = 0;
    for (int n = 0; n < recordsL; n++) {
        if (strcmp(records[n].routeId, newEntry->routeId) == 0
            && strcmp(records[n].charaId, newEntry->charaId) == 0)
        {
            entries[entriesL++] = &records[n];
        }
    }

    //We now have a list of records with the same route/chara
    //Let's add our new entry
    entries[entriesL++] = newEntry;

    //Bubblesort :) Max 10+1 entries, so whatever...
    for (int a = 0; a < entriesL; a++) {
        for (int b = a+1; b < entriesL; b++) {
            if (entries[a]->score < entries[b]->score) {
                ScoreRecord* temp = entries[a];
                entries[a] = entries[b];
                entries[b] = temp;
            }
        }
    }

    bool changed = false;
    if (entriesL > 10) {
        if (entries[entriesL-1] != newEntry) {
            //Replace lowest score's slot with this score
            memcpy(entries[entriesL-1], newEntry, sizeof(ScoreRecord));
            changed = true;
        }
    } else {
        //Add slot
        memcpy(&records[recordsL], newEntry, sizeof(ScoreRecord));
        recordsL++;
        changed = true;
    }

    //Should rewrite score file
    if (changed) {
        FILE* file = fopen(path, "wb");
        if (file) {
            fprintf(file, "TH_S01");
            fwrite(&recordsL, sizeof(u16), 1, file);
            fwrite(records, sizeof(ScoreRecord), recordsL, file);
            fclose(file);
        }
    }
}

int GameInfo::GetScoreRecords(ScoreRecord* entries, const char* routeId, const char* charaId) {
    char versionStr[32];
    GetVersionString(versionStr);
    char path[PATH_MAX];
    sprintf(path, "score/%s-%s.dat", GetId(), versionStr);

    FileHandle* fh = fhOpen(path);
    if (!fh) {
        return 0;
    }

    char header[7];
    fh->Read(header, 6);
    header[6] = '\0';
    if (strcmp(header, "TH_S01") != 0) {
        fhClose(fh);
        return 0;
    }

    u16 recordsL;
    fh->Read(&recordsL, 2);
    ScoreRecord* records = new ScoreRecord[recordsL];
    fh->Read(records, fh->length);
    fhClose(fh);

    //Collect entries with the correct route/chara
    ScoreRecord** temp = new ScoreRecord*[10];
    int entriesL = 0;
    for (int n = 0; n < recordsL; n++) {
        if (strcmp(records[n].routeId, routeId) == 0
            && strcmp(records[n].charaId, charaId) == 0)
        {
            temp[entriesL++] = &records[n];
        }
    }

    //Bubblesort :) Max 10+1 entries, so whatever...
    for (int a = 0; a < entriesL; a++) {
        for (int b = a+1; b < entriesL; b++) {
            if (temp[a]->score < temp[b]->score) {
                ScoreRecord* swap = temp[a];
                temp[a] = temp[b];
                temp[b] = swap;
            }
        }
    }

    //Copy into result
    for (int n = 0; n < entriesL; n++) {
        memcpy(&entries[n], temp[n], sizeof(ScoreRecord));
    }

    delete[] records;
    delete[] temp;
    return entriesL;
}

void GameInfo::SaveScore(const char* routeId, const char* charaId, u8 stage, u32 score) {
    if (score == 0) {
        return;
    }

    char versionStr[32];
    GetVersionString(versionStr);
    char path[PATH_MAX];
    sprintf(path, "score/%s-%s.dat", GetId(), versionStr);

    //Create new entry object
    ScoreRecord* newEntry = new ScoreRecord();
    strcpy(newEntry->routeId, routeId);
    strcpy(newEntry->charaId, charaId);
    newEntry->stage = stage;
    newEntry->score = score;
    newEntry->timestamp = time(NULL);

    u16 recordsL;
    ScoreRecord* records;

    FileHandle* fh = fhOpen(path);
    if (fh) {
        char header[7];
        fh->Read(header, 6);
        header[6] = '\0';
        if (strcmp(header, "TH_S01") != 0) {
            recordsL = 0;
            records = new ScoreRecord[1];
        } else {
            fh->Read(&recordsL, 2);
            records = new ScoreRecord[recordsL + 1];
            fh->Read(records, fh->length);
        }
        fhClose(fh);
    } else {
        recordsL = 0;
        records = new ScoreRecord[1];
    }

    //Insert score
    insertScoreAndSave(path, records, recordsL, newEntry);

    //Cleanup
    delete newEntry;
    delete[] records;
}
u32 GameInfo::GetHiScore(const char* routeId, const char* charaId) {
    ScoreRecord* entries = new ScoreRecord[10];
    int entriesL = GetScoreRecords(entries, routeId, charaId);

    u32 score = 0;
    for (int n = 0; n < entriesL; n++) {
        score = MAX(score, entries[n].score);
    }

    delete[] entries;
    return score;
}

char* GameInfo::GetId() {
    return id;
}
u32 GameInfo::GetVersion() {
    return version;
}
void GameInfo::GetVersionString(char* out) {
	versionIntToString(out, version);
}
char* GameInfo::GetName() {
    return name;
}
char* GameInfo::GetDesc() {
    return desc;
}
char* GameInfo::GetBanner() {
    return banner;
}
u8 GameInfo::GetNumRoutes() {
    return routesL;
}
char* GameInfo::GetRouteId(u8 index) {
    return routeIds[index];
}
char* GameInfo::GetRouteName(u8 index) {
    return routeNames[index];
}

void GameInfo::SetString(char** dst, const char* src) {
    if (*dst) delete[] (*dst);

    *dst = new char[strlen(src) + 1];
    strcpy(*dst, src);
}
void GameInfo::SetId(const char* i) {
    SetString(&id, i);
}
void GameInfo::SetVersion(const char* v) {
    //Accept version strings in a(.b(.c)?)? format

    char* firstDot = strchr(v, '.');
    char* secondDot = NULL;
    if (firstDot) {
        *firstDot = '\0';
        secondDot = strchr(firstDot+1, '.');
        if (secondDot) {
            *secondDot = '\0';
        }
    }

    //Example result: 1.12.37 = 11237
    version = atoi(v)*10000;
    if (firstDot) {
        version += atoi(firstDot+1)*100;
        *firstDot = '.';
    }
    if (secondDot) {
        version += atoi(secondDot+1);
        *secondDot = '.';
    }
}
void GameInfo::SetName(const char* n) {
    SetString(&name, n);
}
void GameInfo::SetDesc(const char* d) {
    SetString(&desc, d);
}
void GameInfo::SetBanner(const char* b) {
    SetString(&banner, b);
}
void GameInfo::SetNumRoutes(u8 r) {
    if (routeIds) {
        for (int n = 0; n < routesL; n++) {
            if (routeIds[n]) delete[] routeIds[n];
            if (routeNames[n]) delete[] routeNames[n];
        }
        delete[] routeIds;
        delete[] routeNames;
    }

    routesL = r;

    routeIds = new char*[r];
    routeNames = new char*[r];
    for (int n = 0; n < r; n++) {
        routeIds[n] = NULL;
        routeNames[n] = NULL;
    }
}
void GameInfo::SetRouteId(u8 index, const char* i) {
    SetString(&routeIds[index], i);
}
void GameInfo::SetRouteName(u8 index, const char* n) {
    SetString(&routeNames[index], n);
}
