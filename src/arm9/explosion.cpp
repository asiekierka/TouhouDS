#include "explosion.h"
#include "game.h"

Explosion::Explosion(Game* game, Sprite* owner, u16 s, u16 ts)
:	Sprite(game, SPRITE_default, game->textures[TEX_explosion], owner->x, owner->y,
		32, 32, UV(0, 0, 32, 32))
{
	scale = s;
    frame = 0;

    alpha = 30;
	timeScale = ts;

	z = owner->z - 5;
}
Explosion::~Explosion() {
}

void Explosion::Update() {
    Sprite::Update();

    int f = frame / timeScale;

    if (f < 4) {
        drawData.uv.x = (f&1 ) << 5;
        drawData.uv.y = (f>>1) << 5;
        drawData.vw = drawData.vh = VERTEX_SCALE(frame * scale / timeScale);
        drawData.OnSizeChanged();

        alpha = 30 - (frame<<2) / timeScale;
    } else {
        Kill();
    }

    frame++;
}

void Explosion::Draw() {
    glPolyFmt(TH_BASE_POLY_FMT|POLY_ID(2)|POLY_ALPHA(1 + alpha));
    Sprite::Draw();
    glPolyFmt(TH_DEFAULT_POLY_FMT);
}
