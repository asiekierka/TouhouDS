#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "sprite.h"

class Explosion : public Sprite {
    private:
        u16 frame;
        u16 timeScale;
        u16 alpha;
        u16 scale;

    public:
        Explosion(Game* game, Sprite* owner, u16 scale=10, u16 timeScale=2);
        virtual ~Explosion();

        virtual void Update();
        virtual void Draw();
};

#endif
