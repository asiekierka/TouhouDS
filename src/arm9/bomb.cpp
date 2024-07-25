#include "bomb.h"
#include "game.h"
#include "player.h"
#include "spritelist.h"

Bomb::Bomb(Game* game, Player* owner, Texture* texture, u16 time, s32 x, s32 y,
		u16 sw, u16 sh, UV uv)
:	Sprite(game, SPRITE_default, texture, x, y, sw, sh, uv),
 	w(sw), h(sh)
{
    //type=SPRITE_DEFAULT which means regular collision detection is disabled for this object

    this->owner = owner;
    this->startTime = time;
    this->time = time;

    losesPower = true;
    dieOutsideBounds = false;
    power = BOMB_POWER;
    drawData.clip = false;
    invincible = 0xFFFF;

    game->AddSprite(this);
}

Bomb::~Bomb() {
}

void Bomb::Update() {
    Sprite::Update();

    if (time & 0x2) {
        if (losesPower && time < FPS * 3/2) {
            power = (power * 6) >> 3;
        }

        UpdateCollision();
    }

    if (time == 0) {
        Kill();
    } else {
        time--;
    }
}

void Bomb::UpdateCollision() {
    for (int n = SPRITE_enemy; n <= SPRITE_enemyShot; n++) {
    	SpriteList* list = game->sprites[n];

    	u32 index = 0;
    	while (!SL_Done(*list, index)) {
    		Sprite* s2 = SL_Next(*list, index);
    		if (!s2) continue;

    		for (u32 n = 0; n < colNodes.size(); n++) {
    			for (u32 x = 0; x < s2->colNodes.size(); x++) {
        			collide(colNodes[n], s2->colNodes[x]);

					if (s2->listIndex < 0) goto next;
					if (listIndex < 0) return;
        		}
    		}

			next: ;
        }
    }
}

void Bomb::Draw() {
	int alpha = 30;
    if (time < FPS * 2) {
        alpha = 30 * time / (FPS*2);
    }
    alpha = MIN(MAX_BOMB_ALPHA, alpha);

    glPolyFmt(TH_BASE_POLY_FMT|POLY_ID(type)|POLY_ALPHA(1+alpha));
    Sprite::Draw();
    glPolyFmt(TH_DEFAULT_POLY_FMT);
}
