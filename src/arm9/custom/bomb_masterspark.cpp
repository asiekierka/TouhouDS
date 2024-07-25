#include "bomb_masterspark.h"
#include "../game.h"
#include "../player.h"
#include "../spritelist.h"
#include "../vram_handler.h"

#define BOMB_TIME 300

//-----------------------------------------------------------------------------

class VRAMUpdater : public VRAMHandler {
	private:
		bool killed;
		TextureManager* texmgr;
		Texture* texture;
        u16 palette[32];
        u16 palette2[32];
        int startTime;
        int timeLeft;

	public:
		VRAMUpdater(TextureManager* texmgr, Texture* texture, int startTime);
		virtual ~VRAMUpdater();

		virtual bool Update();
		void Kill();
};

VRAMUpdater::VRAMUpdater(TextureManager* texmgr, Texture* texture, int startTime) {
	this->texmgr = texmgr;
	this->texture = texture;
	this->startTime = startTime;
	this->timeLeft = startTime;
	killed = false;
}

VRAMUpdater::~VRAMUpdater() {
}

bool VRAMUpdater::Update() {
    texmgr->TogglePaletteVRAM(false);
	if (timeLeft == startTime) {
	    for (int n = 0; n < 32; n++) {
	        palette[n] = ((u16*)texture->palChunk.offset)[n];
	    }
	} else if (timeLeft > 0) {
	    int f = timeLeft;
	    if (startTime - timeLeft < 15) {
	        f =  timeLeft * (startTime - timeLeft) / 15;
	    }

	    for (int n = 0; n < 32; n++) {
	        int c = palette[n];
	        int r = ( c     &31) * f / 15;
	        int g = ((c>>5) &31) * f / 15;
	        int b = ((c>>10)&31) * f / 15;

	        int bleed = 0;
	        bleed += MAX(0, r - 26);
	        bleed += MAX(0, g - 26);
	        bleed += MAX(0, b - 26);
	        bleed /= 3;
	        palette2[n] = RGB15(MIN(31, r+bleed), MIN(31, g+bleed), MIN(31, b+bleed));
	    }

	    memcpy((u16*)texture->palChunk.offset, palette2, 32<<1);
	} else if (timeLeft == 0) {
		for (int n = 0; n < 32; n++) {
			((u16*)texture->palChunk.offset)[n] = palette[n];
		}
	}
    texmgr->TogglePaletteVRAM(true);

	timeLeft--;

	return !killed;
}
void VRAMUpdater::Kill() {
	killed = true;
}

//-----------------------------------------------------------------------------

Bomb_MasterSpark::Bomb_MasterSpark(Game* game, Player* owner, Texture* texture,
		u16 w, u16 h, UV uv)
:	Bomb(game, owner, texture, BOMB_TIME, owner->x, owner->y - inttof32(h/2 - 32),
		w, h, uv)
{
	vramHandler = new VRAMUpdater(&game->texmgr, texture, startTime);

    game->soundManager.PlaySound("huge_laser.wav");
    game->AddVRAMHandler(vramHandler);
}

Bomb_MasterSpark::~Bomb_MasterSpark() {
}

void Bomb_MasterSpark::Kill() {
	vramHandler->Kill();

    Sprite::Kill();
}
void Bomb_MasterSpark::Update() {
    x = owner->x;
    y = owner->y - inttof32(h/2 - 32);

    Bomb::Update();
}

void Bomb_MasterSpark::UpdateCollision() {
    s32 x1 = f32toint(x);
    s32 y1 = f32toint(y);

    for (int n = SPRITE_enemy; n <= SPRITE_enemyShot; n++) {
    	SpriteList* list = game->sprites[n];
    	Sprite* s2;
    	u32 index = 0;
    	while (!SL_Done(*list, index)) {
    		s2 = SL_Next(*list, index);
    		if (!s2) continue;

    		s32 x2 = f32toint(s2->x);
            s32 y2 = f32toint(s2->y);

            if (s2->colNodes.empty() || s2->IsInvincible()) {
            	continue;
            }

            if (abs(x2-x1) < (w>>1) || y2 < y1) {
            	onHit(s2, -1, this, -1, power);
            }
        }
    }
}

void Bomb_MasterSpark::Draw() {
    //alpha == 0 shows the object in wireframe mode

	int alpha = 30;
    if (time < FPS) {
        alpha = 30 * time / FPS;
    } else if (startTime - time < FPS/2) {
        alpha = 15 + 30 * (startTime - time) / FPS;
    }
    //alpha = MIN(MAX_BOMB_ALPHA, alpha);
    alpha = MIN(24, alpha);

    glPolyFmt(TH_BASE_POLY_FMT|POLY_ID(63)|POLY_ALPHA(1+alpha));
    Sprite::Draw();
    glPolyFmt(TH_DEFAULT_POLY_FMT);
}
