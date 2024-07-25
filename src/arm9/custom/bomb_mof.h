#ifndef BOMB_MOF_H
#define BOMB_MOF_H

#include "../thds.h"
#include "../bomb.h"

class Bomb_MoF : public Bomb {
    private:

    public:
        Bomb_MoF(Game* game, Player* owner, Texture* texture, int w=192,
            int h=192, UV uv=UV(0, 0, 64, 64));
        virtual ~Bomb_MoF();

        virtual void Update();

};


#endif
