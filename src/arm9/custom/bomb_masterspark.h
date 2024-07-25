#ifndef BOMB_MASTER_SPARK_H
#define BOMB_MASTER_SPARK_H

#include "../thds.h"
#include "../bomb.h"

class VRAMUpdater;

class Bomb_MasterSpark : public Bomb {
    private:
        VRAMUpdater* vramHandler;

    public:
        Bomb_MasterSpark(Game* game, Player* owner, Texture* texture,
            u16 w=LEVEL_WIDTH, u16 h=LEVEL_HEIGHT+32, UV uv=UV(0, 0, 64, 64));
        virtual ~Bomb_MasterSpark();

        virtual void Kill();
        virtual void Update();
        virtual void UpdateCollision();
        virtual void Draw();
};


#endif
