#ifndef BOMB_SAKUYA_H
#define BOMB_SAKUYA_H

#include "../thds.h"
#include "../bomb.h"

class Bomb_Sakuya : public Bomb {
    private:

    public:
    	Bomb_Sakuya(Game* game, Player* owner, Texture* texture,
            int w=64, int h=64, UV uv=UV(0, 0, 64, 64));
        virtual ~Bomb_Sakuya();

        virtual void Update();
};


#endif
