#include "bomb_youmu.h"
#include "../game.h"
#include "../player.h"

#define BOMB_TIME 90

Bomb_Youmu::Bomb_Youmu(Game* game, Player* owner, Texture* texture, int w, int h,
		UV uv)
:	Bomb(game, owner, texture, BOMB_TIME, owner->x, owner->y-h, w, h, uv)
{
    game->soundManager.PlaySound("giant_shot.wav");

    SetSpeed(0);
    SetAngle(0);
    hitrad = inttof32(w/2);
    power = BOMB_POWER;
    slashY = inttof32(-16);
}

Bomb_Youmu::~Bomb_Youmu() {
}

void Bomb_Youmu::Update() {
	Bomb::Update();

	int s = 6;

	w += s;
	h += s;

    drawData.vw += VERTEX_SCALE(s);
    drawData.vh += VERTEX_SCALE(s);
    drawData.OnSizeChanged();

	SetHitRadius(hitrad + inttof32(s));
}
void Bomb_Youmu::Draw() {
	Bomb::Draw();

	int alpha = 30;
	if (time <= 60) {
		alpha = 0;
	} else if (time <= 75) {
		alpha = (time-60) * 2;
	} else if (BOMB_TIME - time <= 5) {
		alpha = (BOMB_TIME - time) * 6;
		slashY -= floattof32(6.0f);
	}

	if (alpha > 0) {
		glPolyFmt(TH_BASE_POLY_FMT|POLY_ID(2)|POLY_ALPHA(1+alpha));
		s32 vw = VERTEX_SCALE(64);
		s32 vh = VERTEX_SCALE(32);
		drawQuad(drawData.texture, x-inttof32(32), y+slashY, vw, vh, Rect(0, 0, 64, 32));
		glPolyFmt(TH_DEFAULT_POLY_FMT);
	}
}
