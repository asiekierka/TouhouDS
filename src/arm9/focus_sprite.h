#ifndef FOCUS_SPRITE_H
#define FOCUS_SPRITE_H

#include "thcommon.h"

class FocusSprite {
    private:
        Player* player;
        bool active;
        int frame;

        Sprite* hitboxSprite;
        Sprite* outerSprite;

    public:
        FocusSprite(Game* game, Player* player);
        virtual ~FocusSprite();

        virtual void Update();
        virtual void Draw();

        void SetActive(bool a);
};

#endif
