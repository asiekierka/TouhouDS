#include "sprite.h"

#include <vector>
#include "game.h"
#include "explosion.h"
#include "spells/common_spells.h"
#include "lua/thlua.h"

using namespace std;

//-----------------------------------------------------------------------------

const s8 default_z[] = {-60, -20, -30, -50, -40, -10,   0,   0,
                          0,   0,   0,   0,   0,   0,   0,   0};

Sprite::Sprite(Game* g, SpriteType t, Texture* tex, s32 sx, s32 sy, u16 w, u16 h, UV uv)
:	speed(0), speedInc(0),
	angle(DEGREES_IN_CIRCLE >> 1), angleInc(0),
	speedX(0), speedY(inttof32(1)), speedVecDirty(true),
	initSpellBook(NULL), lifeSpellBook(NULL), deathSpellBook(NULL), animationSpellBook(NULL),
	drawAngleMode(DAM_manual),

	game(g), x(sx), y(sy), listIndex(-1), type(t), z(default_z[t]), power(1),
	invincible(0), refcount(1), damageFlash(0), grazed(false),
	dieOutsideBounds(type != SPRITE_default && type != SPRITE_player && type != SPRITE_item),
	wasOnScreen(false),
	drawData(tex, uv, VERTEX_SCALE(w), VERTEX_SCALE(h), angle),
	luaLink(NULL), luaAnimatorLink(NULL), colNodes(0)
{
	CircleColNode* node = new CircleColNode(this, 0);
	node->SetRadius(inttof32(MIN(w, h)>>1));
	colNodes.push_back(node);

	if (type == SPRITE_enemyShot || type == SPRITE_playerShot) {
		drawAngleMode = DAM_auto;
		SetDrawAngle(angle);
	} else {
		SetDrawAngle(0);
	}
}
Sprite::~Sprite() {
	if (initSpellBook) delete initSpellBook;
    if (lifeSpellBook) delete lifeSpellBook;
    if (deathSpellBook) delete deathSpellBook;
    if (animationSpellBook) delete animationSpellBook;

    if (luaLink) delete luaLink;
    if (luaAnimatorLink) delete luaAnimatorLink;

    vector<ColNode*>::iterator itr = colNodes.begin();
    while (itr != colNodes.end()) {
    	delete *itr;
    	++itr;
    }
}

void Sprite::Destroy() {
    while (deathSpellBook && !deathSpellBook->finished) {
        deathSpellBook->Update(game, this);
    }

    if (type == SPRITE_enemy && drawData.visible) {
        game->soundManager.PlaySound("explode_small.wav");
        Explosion* explosion = new Explosion(game, this);
        game->AddSprite(explosion);
    } else if (type == SPRITE_playerShot) {
        game->shotExploded = true;
    }

    if (luaLink) {
    	luaLink->Call("onDestroyed", "");
    }

    power = 0;
    damageFlash = 0;
    game->RemoveSprite(this);
}
void Sprite::Kill() {
    damageFlash = 0;
    game->RemoveSprite(this);
}

ITCM_CODE
void Sprite::UpdateSpellBook() {
	while (initSpellBook && !initSpellBook->finished) {
		initSpellBook->Update(game, this);
	}

    if (lifeSpellBook && !lifeSpellBook->finished) {
    	lifeSpellBook->Update(game, this);
    }
}

ITCM_CODE
void Sprite::Update() {
    if (invincible)  invincible--;
    if (damageFlash) damageFlash--;

	if (luaLink && !luaLink->finished) {
		luaLink->Update();
	}
	if (luaAnimatorLink && !luaAnimatorLink->finished) {
		luaAnimatorLink->Update();
	}

	if (animationSpellBook && !animationSpellBook->finished) {
		animationSpellBook->Update(game, this);
	}
	UpdateSpellBook();

    speed += speedInc;

    if (angleInc) {
    	angle = (angle + angleInc) & (DEGREES_IN_CIRCLE - 1);
    	if (drawAngleMode == DAM_auto) {
    		SetDrawAngle(angle);
    	}
    }

    if (speedVecDirty || speedInc || angleInc) {
        speedVecDirty = false;
        if (speed) {
			speedX =  mulf32(speed, fastSin(angle));
			speedY = -mulf32(speed, fastCos(angle));
		} else {
			speedX = speedY = 0;
		}
    }

	x += speedX;
	y += speedY;
}

ITCM_CODE
void Sprite::Draw() {
	#define dd drawData

    if (!dd.visible || !dd.texture) return;

    glPushMatrix();

	setActiveTexture(dd.texture);

	MATRIX_MULT4x3 = dd.cosA;
	MATRIX_MULT4x3 = dd.sinA;
	MATRIX_MULT4x3 = 0;

	MATRIX_MULT4x3 = -dd.sinA;
	MATRIX_MULT4x3 = dd.cosA;
	MATRIX_MULT4x3 = 0;

	MATRIX_MULT4x3 = 0;
	MATRIX_MULT4x3 = 0;
	MATRIX_MULT4x3 = inttof32(1);

	MATRIX_MULT4x3 = x;
	MATRIX_MULT4x3 = y;
	MATRIX_MULT4x3 = inttof32(z);

    int u0 = (dd.uv.x << 4);
    int u1 = u0 + (dd.uv.w << 4);
    int v0 = (dd.uv.y << 20);
    int v1 = v0 + (dd.uv.h << 20);

    int c = (dd.vw >> 1) & 0xFFFF;
    int a = (-c) & 0xFFFF;

    int d = (dd.vh >> 1);
    int b = ((-d) << 16);
    d <<= 16;

	glBegin(GL_QUAD);
	if (damageFlash && damageFlash != DAMAGE_FLASH_DEFAULT-1) {
		glNormal(NORMAL_PACK(0, 0, inttov10(1)));
	} else {
		glColor(RGB15(31, 31, 31));
	}
	GFX_TEX_COORD = u0 | v1;
	GFX_VERTEX_XY = a  | d;
	GFX_TEX_COORD = u1 | v1;
	GFX_VERTEX_XY = c  | d;
	GFX_TEX_COORD = u1 | v0;
	GFX_VERTEX_XY = c  | b;
	GFX_TEX_COORD = u0 | v0;
	GFX_VERTEX_XY = a  | b;

	glPopMatrix(1);
}

bool Sprite::IsInvincible() {
    return invincible > 0;
}

void Sprite::SetAngle(s32 a) {
	a = a & (DEGREES_IN_CIRCLE - 1);
	if (angle != a) {
		speedVecDirty = true;
		angle = a;

		if (drawAngleMode == DAM_auto) {
			SetDrawAngle(a);
		}
	}
}
void Sprite::SetAngleInc(s32 da) {
    angleInc = da;
}

void Sprite::SetSpeed(s32 s) {
    speedVecDirty |= speed - s;
    speed = s;
}
void Sprite::SetSpeedInc(s32 ds) {
    speedInc = ds;
}

void Sprite::Advance(s32 dist) {
    x += mulf32(dist, fastSin(angle));
    y -= mulf32(dist, fastCos(angle));
}

void Sprite::AdvanceSide(s32 dist) {
    x += mulf32(dist, fastCos(angle));
    y -= mulf32(dist, fastSin(angle));
}

void Sprite::SetInitSpellBook(SpellBook* s) {
    if (initSpellBook) delete initSpellBook;
    initSpellBook = s;
}

void Sprite::SetLifeSpellBook(SpellBook* s) {
    if (lifeSpellBook) delete lifeSpellBook;
    lifeSpellBook = s;
}

void Sprite::SetDeathSpellBook(SpellBook* s) {
    if (deathSpellBook) delete deathSpellBook;
    deathSpellBook = s;
}

void Sprite::SetAnimationSpellBook(SpellBook* s) {
    if (animationSpellBook) delete animationSpellBook;
    animationSpellBook = s;
}

void Sprite::SetHitRadius(s32 rad) {
	if (colNodes.size() == 0) {
		CircleColNode* cn = new CircleColNode(this, 0);
		cn->SetRadius(rad);
		colNodes.push_back(cn);
	} else {
	    vector<ColNode*>::iterator itr = colNodes.begin();
	    while (itr != colNodes.end()) {
			ColNode* cn = *itr;
			if (cn->type == COL_circle) {
				((CircleColNode*)cn)->SetRadius(rad);
			}
			++itr;
		}
	}
}

ColNode* findColNode(vector<ColNode*>& vec, u8 id, ColNodeType type) {
	vector<ColNode*>::iterator itr = vec.begin();
	while (itr != vec.end()) {
		ColNode* node = *itr;
		if (node && node->id == id) {
			if (node->type == type) {
				return node;
			} else {
				delete node;
				vec.erase(itr);
				return NULL;
			}
		}
		++itr;
	}
	return NULL;
}

void Sprite::SetColCircle(u8 id, s32 rad) {
	CircleColNode* cn = (CircleColNode*)findColNode(colNodes, id, COL_circle);
	if (!cn) {
		cn = new CircleColNode(this, id);
		colNodes.push_back(cn);
	}
	cn->SetRadius(rad);
}

void Sprite::SetColSegment(u8 id, bool autoRotate, s32 dx, s32 dy, s32 thickness) {
	SegmentColNode* cn = (SegmentColNode*)findColNode(colNodes, id, COL_segment);
	if (!cn) {
		cn = new SegmentColNode(this, id);
		colNodes.push_back(cn);
	}
	cn->Set(autoRotate, dx, dy, thickness);
}

void Sprite::RemoveColNode(u8 id) {
	vector<ColNode*>::iterator itr = colNodes.begin();
	while (itr != colNodes.end()) {
		ColNode* node = *itr;
		if (node && node->id == id) {
			delete node;
			colNodes.erase(itr);
			return;
		}
		++itr;
	}
}

void Sprite::SetDrawAngleMode(DrawAngleMode dam) {
	drawAngleMode = dam;
}

void Sprite::SetDrawAngle(s32 da) {
	if (drawData.drawAngle == da) {
		return;
	}

	drawData.SetDrawAngle(da);

	vector<ColNode*>::const_iterator itr = colNodes.begin();
	vector<ColNode*>::const_iterator end = colNodes.end();
	while (itr != end) {
		ColNode* cn = *itr;
		if (cn && cn->type == COL_segment) {
			SegmentColNode* seg = (SegmentColNode*)cn;
			if (seg->autoRotate) {
				seg->SetRotation(da);
			}
		}
		++itr;
	}

}

//-----------------------------------------------------------------------------

DrawData::DrawData(Texture* tex, UV duv, v16 w, v16 h, s32 da)
: texture(tex), uv(duv), vw(w), vh(h), visible(tex != NULL), clip(true)
{
	OnSizeChanged();

	drawAngle = da;
	sinA = fastSin(da) << 6;
	cosA = fastCos(da) << 6;
}

DrawData::~DrawData() {
}

void DrawData::OnSizeChanged() {

}

void DrawData::SetDrawAngle(s32 a) {
	if (drawAngle != a) {
		drawAngle = a;
		sinA = fastSin(a) << 6;
		cosA = fastCos(a) << 6;
	}
}
