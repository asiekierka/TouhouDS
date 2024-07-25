#ifndef BOMB_YOUMU_H
#define BOMB_YOUMU_H

#include "../thds.h"
#include "../bomb.h"

class Bomb_Youmu : public Bomb {
    private:
    	s32 slashY;
    	s32 hitrad;

    public:
    	Bomb_Youmu(Game* game, Player* owner, Texture* texture,
    		int w=16, int h=16, UV uv=UV(0, 32, 32, 32));
        virtual ~Bomb_Youmu();

        virtual void Update();
        virtual void Draw();
};


#endif
