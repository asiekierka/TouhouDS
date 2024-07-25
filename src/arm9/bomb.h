#ifndef BOMB_H
#define BOMB_H

#include "thcommon.h"
#include "sprite.h"

#define BOMB_POWER inttof32(4)
#define MAX_BOMB_ALPHA 15

class Bomb : public Sprite {
    private:

    protected:
    	bool losesPower;
    	u16 w, h;

        virtual void UpdateCollision();

    public:
        Player* owner; //Player is always alive durig bomb's life; he's invincible from bombing :D
        u16 startTime;
        u16 time;

        Bomb(Game* game, Player* owner, Texture* texture, u16 time,
            s32 x, s32 y, u16 w, u16 h, UV uv=UV(0, 0, 64, 64));
        virtual ~Bomb();

        virtual void Update();
        virtual void Draw();
};

#endif
