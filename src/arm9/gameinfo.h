#ifndef GAME_INFO_H
#define GAME_INFO_H

#include "thcommon.h"

struct ScoreRecord {
    char routeId[64];
    char charaId[63];
    u8   stage;
    u32  score;
    u32  timestamp;
};

class GameInfo {
    private:
        char* id;
        u32 version;
        char* name;
        char* desc;
        char* banner;

        char** routeIds;
        char** routeNames;

        void SetString(char** dst, const char* src);

    public:
        u8 routesL;

        GameInfo();
        virtual ~GameInfo();

        void  SaveScore(const char* routeId, const char* charaId, u8 stage, u32 score);
        int   GetScoreRecords(ScoreRecord* entries, const char* routeId, const char* charaId);
        u32   GetHiScore(const char* routeId, const char* charaId);

        char* GetId();
        u32   GetVersion();
        void  GetVersionString(char* out);
        char* GetName();
        char* GetDesc();
        char* GetBanner();
        u8    GetNumRoutes();
        char* GetRouteId(u8 index);
        char* GetRouteName(u8 index);

        void SetId(const char* i);
        void SetVersion(const char* v);
        void SetName(const char* n);
        void SetDesc(const char* d);
        void SetBanner(const char* b);
        void SetNumRoutes(u8 routesL);
        void SetRouteId(u8 index, const char* id);
        void SetRouteName(u8 index, const char* name);
};

extern GameInfo** gameInfo;
extern u8 gameInfoL;

GameInfo** initGameInfo();
void freeGameInfo();

#endif

