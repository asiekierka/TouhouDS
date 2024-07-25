#ifndef CHAR_INFO_H
#define CHAR_INFO_H

#include "thcommon.h"

class CharInfo {
    private:
        char* id;
        u32 version;
        char* code;

        char* portrait;
        char* name;
        char* desc;

        s32 shotPowerMul;
        s32 speed;
        s32 speed_focus;
        u8  bombs;
        s32 graze_range;
        s32 hitRadius;

        char* texture;
        char* bomb;
        char* bomb_focus;

        void SetString(char** dst, const char* src);

    public:

        CharInfo();
        virtual ~CharInfo();

        Player* CreatePlayer(Game* game, u8 playerId, Player* oldPlayer=NULL);

        char* GetId();
        u32   GetVersion();
        void  GetVersionString(char* out);
        char* GetCode();
        char* GetPortrait();
        char* GetName();
        char* GetDescription();
        s32   GetShotPowerMul();
        s32   GetSpeed();
        s32   GetSpeedFocus();
        u8    GetBombs();
        s32   GetGrazeRange();
        s32   GetHitRadius();
        char* GetTexture();
        char* GetBomb();
        char* GetBombFocus();

        void SetId(const char* i);
        void SetVersion(char* v);
        void SetCode(const char* c);
        void SetName(const char* n);
        void SetPortrait(const char* p);
        void SetDescription(const char* d);
        void SetShotPowerMul(s32 s);
        void SetSpeed(s32 s);
        void SetSpeedFocus(s32 sf);
        void SetBombs(u8 b);
        void SetGrazeRange(s32 r);
        void SetHitRadius(s32 r);
        void SetTexture(const char* t);
        void SetBomb(const char* b);
        void SetBombFocus(const char* bf);

};

extern CharInfo* charInfo;
extern u8 charInfoL;

CharInfo* initCharacterInfo();

#endif

