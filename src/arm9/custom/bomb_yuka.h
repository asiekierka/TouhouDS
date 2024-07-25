#ifndef BOMB_YUKA_H
#define BOMB_YUKA_H

#include "../thds.h"
#include "../bomb.h"

class Bomb_Yuka : public Bomb {
    private:
    	u32 frame;
    	s32 hitrad;

    public:
    	Bomb_Yuka(Game* game, Player* owner, Texture* texture,
    			int w=96, int h=96, UV uv=UV(0, 0, 64, 64));
        virtual ~Bomb_Yuka();

        virtual void Update();
};


#endif
