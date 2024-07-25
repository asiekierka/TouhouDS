#include "focus_sprite.h"
#include "game.h"
#include "player.h"

#define HITBOX_TURN_SPEED 0
#define OUTER_TURN_SPEED 128

const int hitboxMarkerIndex = 1;

FocusSprite::FocusSprite(Game* game, Player* player) {
    this->player = player;

    hitboxSprite = new Sprite(game, SPRITE_default, game->textures[TEX_playerFx],
        0, 0, 8, 8, UV(0, hitboxMarkerIndex * 8, 8, 8));
    hitboxSprite->z = player->z + 1;

    outerSprite = new Sprite(game, SPRITE_default, game->textures[TEX_playerFx],
        0, 0, 0, 0, UV(32, 0, 32, 32));
    outerSprite->z = player->z + 2;

    SetActive(false);
}
FocusSprite::~FocusSprite() {
    delete hitboxSprite;
    delete outerSprite;
}

void FocusSprite::Update() {
    if (!active) {
        return;
    }

    hitboxSprite->Update();
    outerSprite->Update();

    frame++;

    hitboxSprite->x = player->x;
    hitboxSprite->y = player->y;
    hitboxSprite->SetDrawAngle(hitboxSprite->GetDrawAngle() + HITBOX_TURN_SPEED);

    outerSprite->x = player->x;
    outerSprite->y = player->y;
    outerSprite->SetDrawAngle(outerSprite->GetDrawAngle() + OUTER_TURN_SPEED);

    int vs = VERTEX_SCALE(32);
    outerSprite->drawData.vw = MIN(vs, vs * frame / (FPS/2));
    outerSprite->drawData.vh = MIN(vs, vs * frame / (FPS/2));
    outerSprite->drawData.OnSizeChanged();
}

void FocusSprite::Draw() {
    if (!active) {
        return;
    }

    if (frame >= 6) hitboxSprite->Draw();
    outerSprite->Draw();
}

void FocusSprite::SetActive(bool a) {
    active = a;
    if (!a) frame = 0;
}
