#ifndef BOMB_SANAE_H
#define BOMB_SANAE_H

#include "../thds.h"
#include "../bomb.h"

class Bomb_Sanae : public Bomb {
    private:
    	u32 frame;
    	int textscrollSpeed;

    protected:
        virtual void UpdateCollision();

    public:
    	Bomb_Sanae(Game* game, Player* owner, Texture* texture,
    			int w=LEVEL_WIDTH, int h=LEVEL_HEIGHT+2, UV uv=UV(0, 0, 64, 64));
        virtual ~Bomb_Sanae();

        virtual void Update();
};


#endif
