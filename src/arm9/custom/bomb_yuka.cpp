#include "bomb_yuka.h"
#include "../game.h"
#include "../player.h"

#define BOMB_TIME 180

Bomb_Yuka::Bomb_Yuka(Game* game, Player* owner, Texture* texture, int w, int h, UV uv)
:	Bomb(game, owner, texture, BOMB_TIME, owner->x, owner->y-h, w, h, uv)
{
    game->soundManager.PlaySound("giant_shot.wav");

    SetSpeed(0);
    SetAngle(0);
    hitrad = inttof32(w/2);
    power = BOMB_POWER;

    frame = 0;
}

Bomb_Yuka::~Bomb_Yuka() {
}

void Bomb_Yuka::Update() {
	Bomb::Update();

	if (owner) {
		x = owner->x;
		y = owner->y;
	}

	SetDrawAngle(GetDrawAngle() + 512);
	frame++;

	if ((frame & 7) == 0) {
		int s = 2;
		if (w >= s && h >= s) {
			w -= s;
			h -= s;

			drawData.vw -= VERTEX_SCALE(s);
			drawData.vh -= VERTEX_SCALE(s);
			drawData.OnSizeChanged();

			SetHitRadius(hitrad - inttof32(s>>1));
		} else {
			Destroy();
		}
	}
}
