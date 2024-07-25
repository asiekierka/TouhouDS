#ifndef BOMB_INFO_H
#define BOMB_INFO_H

#include "thcommon.h"

class BombInfo {
    private:
        char* id;
        char* code;
        char* name;
        char* texture;

        void SetString(char** dst, const char* src);

    public:

        BombInfo();
        virtual ~BombInfo();

        Bomb* CreateBomb(Game* game, Player* player, bool isPrimary);

        char* GetId();
        char* GetCode();
        char* GetName();
        char* GetTexture();

        void SetId(const char* i);
        void SetCode(const char* c);
        void SetName(const char* n);
        void SetTexture(const char* t);

};

extern BombInfo* bombInfo;
extern u8 bombInfoL;

BombInfo* initBombInfo();
BombInfo* bombFromString(const char* bombId);

#endif

