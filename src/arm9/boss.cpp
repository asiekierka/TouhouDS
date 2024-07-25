#include "boss.h"

#include "game.h"
#include "explosion.h"
#include "lua/thlua.h"
#include "spells/common_spells.h"
#include "tcommon/dssprite.h"
#include "tcommon/text.h"

using namespace std;

//-----------------------------------------------------------------------------

LuaBossSpellcard::LuaBossSpellcard(const char* f, s32 p, u16 t, const char* n)
:	func(f ? strdup(f) : NULL), power(p), time(t), name(n ? strdup(n) : NULL)
{

}
LuaBossSpellcard::LuaBossSpellcard(const LuaBossSpellcard& o) {
	func = name = NULL;
	operator =(o);
}
LuaBossSpellcard::~LuaBossSpellcard() {
	if (func) free(func);
	if (name) free(name);
}

LuaBossSpellcard& LuaBossSpellcard::operator =(const LuaBossSpellcard& other) {
	if (func) free(func);
	if (name) free(name);

	func = (other.func ? strdup(other.func) : NULL);
	power = other.power;
	time = other.time;
	name = (other.name ? strdup(other.name) : NULL);

	return *this;
}

//-----------------------------------------------------------------------------

Boss::Boss(Game* game, Texture* texture, s32 x, s32 y, u16 w, u16 h, UV uv)
:	Sprite(game, SPRITE_enemy, texture, x, y, w, h, uv)
{
    for (int n = 0; n < 4; n++) {
        nameSprites[n] = NULL;
        spellcardNameSprites[n] = NULL;
    }
    bossIndicator = new DSSprite(game->mainSprites + 100, Rect(96, 0, 32, 8));
    game->mainSprites[100].priority = OBJPRIORITY_0;

    animationType = CAT_idle;
    mirrorAnim = false;
    dieOutsideBounds = false;

    name = NULL;
    numSpellcards = 0;

    luaSpellcard = NULL;
    spellcard = NULL;
    spellName = NULL;
    time = 0;

    invincible = 1;
}

Boss::~Boss() {
	animationSpellBook = NULL; //To prevent double deletion from CharaAnimSet and Sprite2

    if (name) free(name);
    if (luaSpellcard) delete luaSpellcard;
    if (spellcard) delete spellcard;
    if (spellName) free(spellName);

    for (int n = 0; n < 4; n++) {
        if (nameSprites[n]) delete nameSprites[n];
        if (spellcardNameSprites[n]) delete spellcardNameSprites[n];
    }
    delete bossIndicator;
}

void Boss::Destroy() {
    if (spellcard || luaSpellcard) {
        OnSpellEnd();
    } else {
        //log("Invincible: %d", invincible);

    	game->soundManager.PlaySound("boss_explode.wav");
        if (game->GetBoss() == this) {
            game->SetBoss(NULL);
        }

        Explosion* explosion = new Explosion(game, this, 32, 4);
        game->AddSprite(explosion);

        drawData.visible = false;
        Sprite::Destroy();
    }
}

void Boss::Kill() {
    if (game->GetBoss() == this) {
        game->SetBoss(NULL);
    }

    Sprite::Kill();
}

void Boss::SetName(const char* n) {
    if (name) free(name);
    name = (n ? strdup(n) : NULL);

    //Create name sprite
    for (int n = 0; n < 4; n++) {
        if (nameSprites[n]) delete nameSprites[n];
    }

    DrawTextSprite(game->osdGameI, 0, 16, name);

    for (int n = 0; n < 4; n++) {
        nameSprites[n] = new DSSprite(game->mainSprites + (101+n), Rect(32*n, 16, 32, 16));
    }
}

void Boss::DrawTextSprite(u16* screen, u16 x, u16 y, const char* string) {
    if (string) {
        Text* text = game->GetText();
        text->PushState();

        text->ClearBuffer();
        text->SetMargins(0, 0, 0, 0);
        text->SetPen(4, 14);
        text->SetFontSize(10);
        text->PrintString(string);

        for (int n = 0; n < 16; n++) {
            memset(screen + 256*(y+n) + x, 0, 128 * sizeof(u16));
        }
        text->BlitToScreen(screen, 256, 32, 0, 0, x, y, 128, 16);

        text->PopState();
    }

    DC_FlushRange(screen, 256*32*sizeof(u16));
    dmaCopy(screen, SPRITE_GFX, 256*32*sizeof(u16));
}

void Boss::OnSpellEnd(bool timeout) {
    //log("BOSS: Spellcard End: name=%s timeout=%s invincible=%s",
    //    (spellName?spellName:"NULL"), (timeout?"yes":"no"), (IsInvincible()?"yes":"no"));

    //if (!timeout) {
        game->ConvertSpritesToPoints(SPRITE_enemyShot);
        game->ConvertSpritesToPoints(SPRITE_enemy);
    //}

    if (luaSpellcard) {
    	luaSpellcard->Call("onSpellEnd", "");
    	delete luaSpellcard;
    }

    if (spellcard) delete spellcard;
    if (spellName) free(spellName);
    for (int n = 0; n < 4; n++) {
        if (spellcardNameSprites[n]) {
            delete spellcardNameSprites[n];
            spellcardNameSprites[n] = NULL;
        }
    }

    startPower = power = 0;
    luaSpellcard = NULL;
    spellcard = NULL;
    spellName = NULL;
    time = 0;
    invincible = 1;
}

void Boss::UpdateSpellBook() {
    if (luaSpellcard) {
    	if (!luaSpellcard->finished) {
    		luaSpellcard->Update();
    	}
    } else if (spellcard) {
		if (!spellcard->finished) {
			spellcard->Update(game, this);
		} else {
			//Force spellcard to inifinitely loop
			spellcard->Reset();
		}
    } else {
    	if (numSpellcards == 0) {
			if (!lifeSpellBook || lifeSpellBook->finished) { //Life spellbook has ended
				Destroy();
			}
    	}

   		Sprite::UpdateSpellBook();

        if (!spellcard && luaLink) {
			while (!luaSpellcard && luaSpellcards.size() > 0) {
				LuaBossSpellcard& s = luaSpellcards.front();

				luaSpellcard = new LuaMethodLink(game->runtime.L, luaLink->objectRef, s.func);
				SetSpellCard(NULL, s.power, s.time, s.name);

				luaSpellcards.pop_front();
			}
        }
    }
}

void Boss::Update() {
	s32 oldX = x;

    Sprite::Update();

    //Update animation
    if (animationSpellBook && animationSpellBook->finished) {
    	if (animationSpellBook == animations.GetAnimation(CAT_tweenLeft)) {
    		animationType = CAT_left;
    		SetAnimation(CAT_left);
    	} else if (animationSpellBook == animations.GetAnimation(CAT_tweenRight)) {
    		animationType = CAT_right;
    		SetAnimation(CAT_right);
    	} else {
    		animationSpellBook->Reset();
    	}
    }

    if (x >= oldX + floattof32(0.25f)) {
    	SetAnimation(CAT_right);
    } else if (x <= oldX - floattof32(0.25f)) {
    	SetAnimation(CAT_left);
    } else {
    	SetAnimation(CAT_idle);
    }

    //printf("\x1b[0;0H%d %p", animationType, animationSpellBook);

    //Update timer
    if (!spellcard && !luaSpellcard) {
        if (!invincible) invincible++;
    } else {
        if (time > 0) {
            time--;
            if (time == 0) {
                OnSpellEnd(true);
            } else {
                if (time % 60 == 0) {
                    if (time <= 300) {
                        game->soundManager.PlaySound("time_very_low.wav");
                    } else if (time <= 600) {
                        game->soundManager.PlaySound("time_low.wav");
                    }
                }
            }
        }
    }

    for (int n = 0; n < 4; n++) {
        if (nameSprites[n]) {
            nameSprites[n]->SetPos(32*n, 8);
            nameSprites[n]->SetVisible(!game->swapScreens);
        }
        if (spellcardNameSprites[n]) {
            spellcardNameSprites[n]->SetPos(32*n, 160);
            spellcardNameSprites[n]->SetVisible(!game->swapScreens);
        }
    }

    bossIndicator->SetPos(f32toint(x)-16, 0);
    bossIndicator->SetVisible(game->swapScreens);
}

void Boss::Draw() {
	if (mirrorAnim) {
		drawData.uv.x += drawData.uv.w;
		drawData.uv.w = -drawData.uv.w;
		drawData.OnSizeChanged();
	}

    Sprite::Draw();

	if (mirrorAnim) {
		drawData.uv.x += drawData.uv.w;
		drawData.uv.w = -drawData.uv.w;
		drawData.OnSizeChanged();
	}
}

u16 Boss::GetTimeLeft() {
    return (spellcard || luaSpellcard ? time : 0);
}
u8 Boss::GetSpellCardsLeft() {
    return numSpellcards;
}

void Boss::SetAnimation(CharaAnimationType type) {
	if (animationType == type && animationSpellBook == animations.GetAnimation(type)) {
		return;
	}

	if (animationType == CAT_idle && (type == CAT_left || type == CAT_right)) {
		animationType = (type == CAT_left ? CAT_tweenLeft : CAT_tweenRight);
	} else {
		if (!(animationType == CAT_tweenLeft && type == CAT_left)
				&& !(animationType == CAT_tweenRight && type == CAT_right))
		{
			animationType = type;
		}
	}

	mirrorAnim = false;
	SpellBook* sb = animations.GetAnimation(animationType);
	if (!sb) {
		switch (animationType) {
		case CAT_left: mirrorAnim = true; sb = animations.GetAnimation(CAT_right); break;
		case CAT_right: mirrorAnim = true; sb = animations.GetAnimation(CAT_left); break;
		case CAT_tweenLeft: mirrorAnim = true; sb = animations.GetAnimation(CAT_tweenRight); break;
		case CAT_tweenRight: mirrorAnim = true; sb = animations.GetAnimation(CAT_tweenLeft); break;
		default: sb = animations.GetAnimation(CAT_idle); break;
		}

		if (!sb) {
			if (animationType == CAT_tweenLeft) {
				SetAnimation(animationType = CAT_left);
				return;
			} else if (animationType == CAT_tweenRight) {
				SetAnimation(animationType = CAT_right);
				return;
			}
		}
	}

	if (sb) {
		SetAnimationSpellBook(sb);
	}
}

void Boss::SetSpellCard(SpellBook* sb, s32 p, u16 t, const char* n) {
    if (spellcard) delete spellcard;
    if (spellName) free(spellName);

    spellcard = sb;
    startPower = power = p;
    time = t;
    spellName = (n ? strdup(n) : NULL);

    //Create Sprites
    for (int n = 0; n < 4; n++) {
        if (spellcardNameSprites[n]) delete spellcardNameSprites[n];
    }
    DrawTextSprite(game->osdGameI, 128, 16, spellName);
    for (int n = 0; n < 4; n++) {
        spellcardNameSprites[n] = new DSSprite(game->mainSprites + (105+n), Rect(128+32*n, 16, 32, 16));
    }

    if (numSpellcards > 0) numSpellcards--;
}

void Boss::SetNumSpellcards(u8 sc) {
    numSpellcards = sc;
}

void Boss::SetCharaAnim(CharaAnimationType cat, SpellBook* sb) {
	animations.SetAnimation(cat, sb);
}

void Boss::SetAnimationSpellBook(SpellBook* s) {
	animationSpellBook = s;
}

void Boss::AddLuaSpellcard(const char* methodName, s32 pow, s32 t, const char* n) {
	luaSpellcards.push_back(LuaBossSpellcard(methodName, pow, t, n));
	numSpellcards++;
}
