#ifndef BOMB_REIMU_H
#define BOMB_REIMU_H

#include "../thds.h"
#include "../bomb.h"

class Bomb_Reimu : public Bomb {
    private:
        u8 id;
        s32 speedX;
        s32 speedY;

    public:
        Bomb_Reimu(Game* game, Player* owner, Texture* texture,
            int w=64, int h=64, UV uv=UV(0, 0, 64, 64), u8 spawn=4, u8 id=0);
        virtual ~Bomb_Reimu();

        virtual void Update();
        virtual void Draw();
};


#endif
