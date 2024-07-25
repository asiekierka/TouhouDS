#include "bomb_mof.h"
#include "../game.h"
#include "../player.h"

#define BOMB_TIME 180
#define BOMB_SPEED floattof32(1.0f)

Bomb_MoF::Bomb_MoF(Game* game, Player* owner, Texture* texture, int w, int h, UV uv)
:	Bomb(game, owner, texture, BOMB_TIME, owner->x, owner->y, w, h, uv)
{
    game->soundManager.PlaySound("giant_shot.wav");

    SetSpeed(BOMB_SPEED);
    SetAngle(0);
}

Bomb_MoF::~Bomb_MoF() {
}

void Bomb_MoF::Update() {
    SetDrawAngle(GetDrawAngle() + 2048);

    Bomb::Update();
}
