#ifndef LEVEL_H
#define LEVEL_H

#include <list>
#include "thcommon.h"

class Level {
    private:
        char* folder;
        char* name;
        char* desc;
        char** unlocks;
        u8 unlocksL;

        Game* game;
        u32 frame;
        std::list<SpellBook*> spellBooks;
        std::list<LuaLink*> luaThreads;

        s32 bgScrollDx, bgScrollDy;
        s32 targetBgScrollDx, targetBgScrollDy;

        void BannerInit();
        void BannerDestroy();
        void UnlockRoute(const char* routeId);
        void SetUnlocksString(const char* s);
        void UpdateSpellBooks();
        void UpdateLuaThreads();

    public:
        u32 startScore;
        u16 livesLost;
        u16 bombsUsed;
        s32 bgScrollX;
        s32 bgScrollY;

        Level(Game* game, u32 score=0, const char* folder=NULL);
        virtual ~Level();

        const char* GetFolder();

        virtual void OnFinished();
        virtual void Update();
        void AddThread(SpellBook* sb);
        void AddLuaThread(LuaFunctionLink* t);
        void OnBombUsed();
        void OnPlayerKilled();

        void SetBgScrollSpeed(s32 dx, s32 dy);
};

#endif
