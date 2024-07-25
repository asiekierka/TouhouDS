#include "pausemenu.h"
#include "../game.h"
#include "../level.h"
#include "../controls.h"
#include "../tcommon/dssprite.h"
#include "../tcommon/text.h"

#define MODE_NONE     0
#define MODE_MAIN     1
#define MODE_CONFIRM  2
#define MODE_RESULTS  3
#define MODE_GAMEOVER 4

PauseMenu::PauseMenu(Game* game, SpriteEntry* spriteEntries) {
    this->game = game;
    this->spriteEntries = spriteEntries;

    mode = MODE_NONE;
    selection[0] = selection[1] = NULL;
    options = NULL;
    optionsL = 0;
    selectedOption = 0;
}
PauseMenu::~PauseMenu() {
    SetMode(MODE_NONE);
}

void PauseMenu::SetMode(u16 m) {
    //Cleanup
    if (m == MODE_RESULTS) {
        memset(game->main_bg, 0, 256*192*sizeof(u16));
    }

    //Clear selection sprite
    for (int n = 0; n < 2; n++) {
        if (selection[n]) {
            delete selection[n];
            selection[n] = NULL;
        }
    }

    //Clear option sprites
    if (options) {
        for (int n = 0; n < optionsL; n++) {
            if (options[n]) delete options[n];
        }
        delete[] options;
        options = NULL;
    }
    optionsL = 0;

    //Set mode
    mode = m;
    selectedOption = 0;
    frame = 0;

    //Init new sprites
    if (m == MODE_MAIN || m == MODE_CONFIRM || m == MODE_GAMEOVER) {
        for (int n = 0; n < 2; n++) {
            selection[n] = new DSSprite(spriteEntries+n, Rect(32*n, 16, 32, 16));
        }

        optionsL = (m == MODE_MAIN ? 6 : 4);
        options = new DSSprite*[optionsL];

        u8 x = (m == MODE_MAIN ? 64 : 128);
        u8 y = (m == MODE_MAIN ? 16 :   0);
        if (m == MODE_GAMEOVER) {
            x = 128; y = 16;
        }
        for (int n = 0; n < optionsL; n++) {
            options[n] = new DSSprite(spriteEntries+2+n, Rect((x + 32*n)%256, y, 32, 16));
        }
    } else if (m == MODE_RESULTS) {
        Level* level = game->level;
        char str[128];

        Text* text = game->GetText();
        text->ClearBuffer();
        text->SetFont("mono.ttf");

        text->PushState();
        text->SetFontSize(11);
        text->SetMargins(32, 32, 64, 48);
        sprintf(str, "lives lost: %d\n"
                     "bombs used: %d\n"
                     "\n"
                     "old score: %09d\n"
                     "new score: %09d\n",
            level->livesLost, level->bombsUsed, level->startScore, game->score);
        text->PrintString(str);
        text->PopState();
        text->SetFont("font.ttf");

        game->DestroyLevel();

        u16* buffer = new u16[256*192];
        if (buffer) {
        	loadImage("img/results", buffer, NULL, 256, 192, GL_RGB256);

        	text->BlitToScreen(buffer, 256, 192);
			DC_FlushRange(buffer, 256*192*sizeof(u16));
			dmaCopy(buffer, game->main_bg, 256*192*sizeof(u16));
			delete[] buffer;
        }
    }
}

u16 PauseMenu::Select(u16 mode, u8 option, bool confirmed) {
    if (mode == MODE_MAIN) {
        switch (option) {
            case 0: return MODE_NONE;
            case 1:
                if (!confirmed) {
                    confirmOptionMode = mode;
                    confirmOption = option;
                    return MODE_CONFIRM;
                }
                game->Restart();
                return MODE_NONE;
            case 2:
                if (!confirmed) {
                    confirmOptionMode = mode;
                    confirmOption = option;
                    return MODE_CONFIRM;
                }
                game->Quit();
                return MODE_NONE;
        }
    } else if (mode == MODE_CONFIRM) {
        switch (option) {
            case 0: return Select(confirmOptionMode, confirmOption, true);
            case 1: return confirmOptionMode;
        }
    } else if (mode == MODE_GAMEOVER) {
        switch (option) {
            case 0:
                if (!confirmed) {
                    confirmOptionMode = mode;
                    confirmOption = option;
                    return MODE_CONFIRM;
                }
                game->Restart();
                return MODE_NONE;
            case 1:
                if (!confirmed) {
                    confirmOptionMode = mode;
                    confirmOption = option;
                    return MODE_CONFIRM;
                }
                game->Quit();
                return MODE_NONE;
        }
    }

    return mode;
}

u16 PauseMenu::Update(u16 m) {
    if (mode != m) {
        SetMode(m);
    }

    frame++;

    scanKeys();
    u32 down = keysDown();

    if (mode == MODE_MAIN) {
        if (down & (KEY_X|KEY_START)) {
            SetMode(MODE_NONE);
            return mode;
        }
    }

    if (mode == MODE_MAIN || mode == MODE_CONFIRM || mode == MODE_GAMEOVER) {
        if (down & KEY_A) {
            SetMode(Select(mode, selectedOption));
            return mode;
        } else if (down & KEY_B) {
            if (mode == MODE_CONFIRM) {
                SetMode(confirmOptionMode);
                return mode;
            }
        }

        if (down & (KEY_UP|KEY_LEFT)) {
            if (selectedOption > 0) selectedOption--;
            else selectedOption = optionsL/2-1;
        } else if (down & (KEY_DOWN|KEY_RIGHT)) {
            if (selectedOption < optionsL/2-1) selectedOption++;
            else selectedOption = 0;
        }

        for (int n = 0; n < optionsL; n++) {
            options[n]->SetPos(96 + 32*(n%2), (192-24*(optionsL/2))/2 + 24*(n/2));
            if (selectedOption*2 == n) {
                selection[0]->SetPos(options[n]->GetX(), options[n]->GetY());
            } else if (selectedOption*2 == n-1) {
                selection[1]->SetPos(options[n]->GetX(), options[n]->GetY());
            }
        }
    } else if (mode == MODE_RESULTS) {
        if (frame > FPS * 2) {
            if (down & (controls.fireButton|controls.bombButton|KEY_START|KEY_TOUCH)) {
                SetMode(MODE_NONE);
                return mode;
            }
        }
    }

    return mode;
}
