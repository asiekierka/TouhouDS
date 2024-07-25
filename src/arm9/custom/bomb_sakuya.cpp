#include "bomb_sakuya.h"
#include "../game.h"
#include "../player.h"

#define BOMB_TIME 60

Bomb_Sakuya::Bomb_Sakuya(Game* game, Player* owner, Texture* texture, int w, int h,
		UV uv)
:	Bomb(game, owner, texture, BOMB_TIME, owner->x, owner->y, w, h, uv)
{
    game->soundManager.PlaySound("giant_shot.wav");

    drawData.visible = false;
    SetHitRadius(inttof32(3));

    int maxV = 2;
    for (int v = 0; v < maxV; v++) {
    	int deltaA = v * (DEGREES_IN_CIRCLE / 32) / maxV;
    	int u = (v % 2 == 0 ? 32 : 0);
		for (int n = 0; n < 32; n++) {
			Sprite* sprite = new Sprite(game, SPRITE_playerShot, texture,
					x, y, 32, 32, UV(u, 0, 32, 32));
			sprite->SetHitRadius(inttof32(12));
			sprite->power = inttof32(20);
			sprite->SetSpeed(inttof32(3));
			sprite->SetAngle((n * DEGREES_IN_CIRCLE / 32) + deltaA);
			sprite->Advance(inttof32(50 - 25 * v));
			game->AddSprite(sprite);
		}
    }
}

Bomb_Sakuya::~Bomb_Sakuya() {
}

void Bomb_Sakuya::Update() {
	if (owner) {
		x = owner->x;
		y = owner->y;
	}

	Bomb::Update();
}
