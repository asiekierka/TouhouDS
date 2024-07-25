#include "bomb_sanae.h"
#include "../game.h"
#include "../player.h"
#include "../spritelist.h"

#define BOMB_TIME 240

Bomb_Sanae::Bomb_Sanae(Game* game, Player* owner, Texture* texture, int w, int h,
		UV uv)
:	Bomb(game, owner, texture, BOMB_TIME, LEVEL_WIDTH>>1, LEVEL_HEIGHT>>1, w, h, uv)
{
    game->soundManager.PlaySound("giant_shot.wav");

    SetSpeed(0);
    SetAngle(0);
    power = BOMB_POWER/4;

    frame = 0;
    textscrollSpeed = 2;

	x = inttof32(LEVEL_WIDTH>>1);
	y = inttof32(LEVEL_HEIGHT>>1);

	setTextureParam(texture, TEXGEN_TEXCOORD|GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T);
}

Bomb_Sanae::~Bomb_Sanae() {
}

void Bomb_Sanae::Update() {
	Bomb::Update();

	drawData.uv.y += textscrollSpeed;
	drawData.OnSizeChanged();

	if (time < FPS * 2) {
		textscrollSpeed = 3;
	}
	frame++;
}

void Bomb_Sanae::UpdateCollision() {
    for (int n = SPRITE_enemy; n <= SPRITE_enemyShot; n++) {
    	SpriteList* list = game->sprites[n];
    	Sprite* s2;
    	u32 index = 0;
    	while (!SL_Done(*list, index)) {
    		s2 = SL_Next(*list, index);
    		if (!s2) continue;

    		if (!s2->colNodes.empty() && !s2->IsInvincible()) {
    			onHit(s2, -1, this, -1, power);
    		}
        }
    }
}
