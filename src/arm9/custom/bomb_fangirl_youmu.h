#ifndef BOMB_FANGIRL_YOUMU_H
#define BOMB_FANGIRL_YOUMU_H

#include "../thds.h"
#include "../bomb.h"

class Bomb_FangirlYoumu : public Bomb {
    private:

    public:
        Bomb_FangirlYoumu(Game* game, Player* owner, Texture* texture,
            int w=64, int h=64, UV uv=UV(0, 0, 64, 64));
        virtual ~Bomb_FangirlYoumu();

        virtual void Update();
};


#endif
