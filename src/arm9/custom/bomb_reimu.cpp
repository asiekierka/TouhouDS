#include "bomb_reimu.h"
#include "../game.h"
#include "../player.h"

#define BOMB_TIME 250

Bomb_Reimu::Bomb_Reimu(Game* game, Player* owner, Texture* texture, int w, int h,
		UV uv, u8 spawn, u8 id)
:	Bomb(game, owner, texture, BOMB_TIME, owner->x, owner->y, w, h, uv)
{
	if (id == 0) {
		game->soundManager.PlaySound("giant_shot.wav");
	}

    this->id = id;
    z -= id;

    for (int n = 0; n < spawn; n++) {
        new Bomb_Reimu(game, owner, texture, w, h, uv, 0, (id+1) + n);
    }

    u32 random = rand();
    speedX = inttof32(-8) + ((random    )&0xFFFF); //[-8, 8]
    speedY = inttof32(-8) + ((random>>16)&0xFFFF); //[-8, 8]
}

Bomb_Reimu::~Bomb_Reimu() {
}

void Bomb_Reimu::Update() {
    SetDrawAngle(GetDrawAngle() + 1024);

    int h2 = h>>1;
    s32 ny = y + speedY;
    int sy = f32toint(ny);
    if (sy < h2) {
        speedY = -speedY;
        ny = inttof32(h - sy);
    } else if (sy >= LEVEL_HEIGHT-h2) {
        speedY = -speedY;
        ny = inttof32(2*(LEVEL_HEIGHT-h2) - sy);
    }

    int w2 = w>>1;
    s32 nx = x + speedX;
    int sx = f32toint(nx);
    if (sx < w2) {
        speedX = -speedX;
        nx = inttof32(w - sx);
    } else if (sx >= LEVEL_WIDTH-w2) {
        speedX = -speedX;
        nx = inttof32(2*(LEVEL_WIDTH-w2) - sx);
    }

    x = nx;
    y = ny;

    Bomb::Update();
}

void Bomb_Reimu::Draw() {
    if (time < FPS * 2) {
        glPolyFmt(TH_BASE_POLY_FMT|POLY_ID(10+id)|POLY_ALPHA(1 + (30 * time / (FPS*2))));
        Sprite::Draw();
        glPolyFmt(TH_DEFAULT_POLY_FMT);
    } else {
        glPolyFmt(TH_BASE_POLY_FMT|POLY_ID(10+id)|POLY_ALPHA(31));
        Sprite::Draw();
        glPolyFmt(TH_DEFAULT_POLY_FMT);
    }
}
