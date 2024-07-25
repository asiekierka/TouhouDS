#ifndef OSD_H
#define OSD_H

#include "thcommon.h"

class OSD {
    private:
        Game* game;
        SpriteEntry* sprites;

        void SetNum(u16 start, u8 L, u32 num);

    public:
        OSD(Game* game, SpriteEntry* osdSpr);
        virtual ~OSD();

        virtual void DrawTop();
        virtual void DrawBottom(Player* player);

};

#endif
