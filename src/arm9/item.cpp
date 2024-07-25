#include "item.h"
#include "game.h"
#include "player.h"

#define TARGET_SX 0
#define TARGET_SY ITEM_SPEED
#define SPEED_DELTA floattof32(0.2f)

Item::Item(Game* game, ItemType it, int speedMul)
:	Sprite(game, SPRITE_item, game->textures[TEX_item], 0, 0, 8, 8, UV(0, 0, 8, 8))
{
    itemType = it;
    int i = (int)itemType;
    drawData.uv.x = ((i & 3)  << 3); //(i%4)*8
    drawData.uv.y = ((i >> 2) << 3); //(i/4)*8
    drawData.OnSizeChanged();

    u32 r = rand();
    speedY = inttof32(-2) - (r & 0x3FFF);
    speedX = inttof32(-4) + ((r>>14) & 0x7FFF);

    if (speedMul != 1) {
        speedY *= speedMul;
    }

    dieOutsideBounds = false;
}
Item::~Item() {
}

ITCM_CODE
void Item::Update() {
    Sprite::Update();

    if (speedX > TARGET_SX) {
        speedX -= SPEED_DELTA;
        if (speedX < TARGET_SX) {
            speedX = TARGET_SX;
        }
    } else if (speedX < TARGET_SX) {
        speedX += SPEED_DELTA;
        if (speedX > TARGET_SX) {
            speedX = TARGET_SX;
        }
    }
    if (speedY > TARGET_SY) {
        speedY -= SPEED_DELTA;
        if (speedY < TARGET_SY) {
            speedY = TARGET_SY;
        }
    } else if (speedY < TARGET_SY) {
        speedY += SPEED_DELTA;
        if (speedY > TARGET_SY) {
            speedY = TARGET_SY;
        }
    }

    x += speedX;
    y += speedY;

    int ww = inttof32(INV_VERTEX_SCALE(drawData.vw));
    int hh = inttof32(INV_VERTEX_SCALE(drawData.vh));
    if (x < -ww || x > inttof32(LEVEL_WIDTH)+ww || y > inttof32(LEVEL_HEIGHT)+hh) {
        game->RemoveSprite(this);
    }
}

void Item::Collect() {
    game->soundManager.PlaySound("collect_item.wav");

    Kill();
}
