#include "bomb_fangirl_youmu.h"
#include "../game.h"
#include "../player.h"

#define SPD 20
#define BOMB_SPEED inttof32(SPD)

Bomb_FangirlYoumu::Bomb_FangirlYoumu(Game* game, Player* owner, Texture* texture,
		int w, int h, UV uv)
:	Bomb(game, owner, texture, ((LEVEL_HEIGHT+h)*4) / SPD, owner->x, inttof32(-h/2),
    		w, h, uv)
{
    game->soundManager.PlaySound("giant_shot.wav");

    SetSpeed(BOMB_SPEED);
    SetAngle(DEGREES_IN_CIRCLE/2);
    power *= 2;
}

Bomb_FangirlYoumu::~Bomb_FangirlYoumu() {
}

void Bomb_FangirlYoumu::Update() {
    if (y > inttof32(LEVEL_HEIGHT+(h>>1))) {
        SetAngle(GetAngle() + DEGREES_IN_CIRCLE/2);
    } else if (y < inttof32(-(h>>1))) {
        SetAngle(GetAngle() + DEGREES_IN_CIRCLE/2);
    }

    SetDrawAngle(GetAngle() + DEGREES_IN_CIRCLE/2);

    Bomb::Update();
}
