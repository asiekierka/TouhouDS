#ifndef ITEM_H
#define ITEM_H

#include "thcommon.h"
#include "sprite.h"
#include "parser/parser_c.h"

#define ITEM_GET_LINE inttof32(64)
#define ITEM_SPEED floattof32(1.25f)    //Item vertical speed
#define ITEM_ATR floattof32(5.0f)       //Item attraction speed
#define ITEM_AC_ATR floattof32(10.0f)   //Item autocollection attraction speed
#define ITEM_MAGNET_RAD inttof32(36)	//Item attraction radius
#define ITEM_MAGNET_RAD_SQ inttof32(36*36)

class Item : public Sprite {
    private:
        s32 speedX, speedY;

    public:
        ItemType itemType;

        Item(Game* game, ItemType itemType, int speedMul=1);
        virtual ~Item();

        virtual void Update();
        virtual void Collect();
};

#endif
