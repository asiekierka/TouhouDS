#include "osd.h"
#include "game.h"
#include "player.h"
#include "boss.h"

OSD::OSD(Game* g, SpriteEntry* osdSpr) {
    game = g;
    sprites = osdSpr;
}
OSD::~OSD() {
}

void OSD::DrawTop() {
    Boss* boss = game->GetBoss();

    for (int n = 0; n < 64; n++) {
        sprites[n].isHidden = false;
        sprites[n].gfxIndex = 0;
    }
    if (boss) {
    	REG_DIVCNT = DIV_32_32; //Set hardware divider to 32-bit mode
    	while(REG_DIVCNT & DIV_BUSY); //Wait for the divider change to finish

        SetNum(1, 2, boss->GetSpellCardsLeft());
        sprites[3].gfxIndex = 0;

        sprites[28].gfxIndex = 0;
        if (boss->GetTimeLeft() == 0) {
            sprites[29].gfxIndex = sprites[30].gfxIndex = sprites[31].gfxIndex = 0;
        } else {
            SetNum(29, 3, boss->GetTimeLeft() / 60);
        }

        int power = 0;
        if (!boss->IsInvincible()) {
            power = f32toint(divf32(8*24*boss->power, boss->startPower));
        }
        for (int n = 4; n < 28; n++) {
            int index = MAX(0, MIN(8, power));
            sprites[n].gfxIndex = (1 << 5) | (7 + index);
            power -= 8;
        }
    }

    if (game->menu <= 0) {
    	REG_DIVCNT = DIV_32_32; //Set hardware divider to 32-bit mode

        for (int n = 32; n < 32 + 6; n++) {
            sprites[n].gfxIndex = (1 << 5) | (16 + (n-32));
        }
        for (int n = 51; n < 51 + 4; n++) {
            sprites[n].gfxIndex = 16 + (n-51);
        }

    	while(REG_DIVCNT & DIV_BUSY); //Wait for the divider change to finish

        SetNum(38, 9, game->GetHiScore()); //hi-score
        SetNum(55, 9, game->score); //score
    }
}
void OSD::DrawBottom(Player* player) {
    for (int n = 0; n < 64; n++) {
        sprites[n].gfxIndex = 0;
    }

    //................................
    //L% B% P%%% G%%%%% E%%%%% P%%%%%%

    REG_DIVCNT = DIV_32_32; //Set hardware divider to 32-bit mode

    int i[] = {32, 35, 38, 43, 50, 57};
    int x[] = { 0,  1,  2,  5,  4,  3};
    int y[] = { 1,  1,  1,  1,  1,  1};
    for (int n = 0; n < 6; n++) {
        sprites[i[n]].gfxIndex = (y[n] << 5) | (x[n]);
    }

	while(REG_DIVCNT & DIV_BUSY); //Wait for the divider change to finish

    if (player) {
		SetNum(33, 1, MIN(9, player->GetLives()));
		SetNum(36, 1, MIN(9, player->GetBombs()));
		SetNum(39, 3, MIN(999, player->GetShotPower()));
		SetNum(44, 5, MIN(99999, player->GetGraze()));
		SetNum(51, 5, MIN(99999, player->GetExtraPoints()));
		SetNum(58, 6, MIN(999999, player->GetPoints()));
    }
}

void OSD::SetNum(u16 start, u8 L, u32 num) {
    for (u16 n = start + L-1; n >= start; n--) {
    	REG_DIV_NUMER_L = num;
    	REG_DIV_DENOM_L = 10;

        sprites[n].isHidden = false;
        sprites[n].gfxIndex &= ~31; //zero x-index bytes

    	while(REG_DIVCNT & DIV_BUSY); //Wait for divide to finish
        sprites[n].gfxIndex |= (1 + (REG_DIVREM_RESULT_L)); //zero x-index bytes
        num = (REG_DIV_RESULT_L);
    }
}
