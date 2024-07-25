#ifndef PAUSEMENU_H
#define PAUSEMENU_H

#include "../thcommon.h"
#include "../tcommon/dssprite.h"

class Game;

class PauseMenu {
    private:
        Game* game;
        SpriteEntry* spriteEntries;
        u16 mode;
        u32 frame;

        DSSprite* selection[2];
        DSSprite** options;
        u8  optionsL;
        u8  selectedOption;
        u16 confirmOptionMode;
        u8  confirmOption;

        u16  Select(u16 mode, u8 option, bool confirmed=false);
        void SetMode(u16 mode);

    public:
        PauseMenu(Game* game, SpriteEntry* spriteEntries);
        virtual ~PauseMenu();

        u16 Update(u16 mode);
};

#endif
