#include "player.h"
#include "game.h"
#include "focus_sprite.h"
#include "item.h"
#include "controls.h"
#include "bombinfo.h"
#include "lua/thlua.h"
#include "spells/common_spells.h"
#include "custom/custom.h"

#define PLAYER_MIN_X inttof32(8)
#define PLAYER_MAX_X inttof32(LEVEL_WIDTH - 8)
#define PLAYER_MIN_Y inttof32(12)
#define PLAYER_MAX_Y inttof32(LEVEL_HEIGHT - 12)

#define START_POWER 1
#define COOLDOWN_TIME 6
#define BULLET_SPEED floattof32(4.0f)

#define MAX_SHOT_POWER 127
#define MAX_BOMBS 9
#define MAX_LIVES 9
#define DEATH_INVINCIBLE_TIME 120
#define DEATH_POWER_LOSS 32
#define DEATH_POWER_DROP 16
#define DEATH_BOMB_TIME 12

#define POWER_ITEM_VALUE 1
#define LARGE_POWER_ITEM_VALUE 10

#define ANIM_SHIFT 3

Player::Player(Game* game, u8 pid, Texture* texture, const char* charaId)
:	Sprite(game, SPRITE_player, texture, inttof32(LEVEL_WIDTH/2), inttof32(LEVEL_HEIGHT-16),
            32, 32, UV(0, 0, 32, 32)),
      playerId(pid), luaFireScript(NULL)
{
    fireSpellParamsL = 2;
    fireSpellParams = new Param*[fireSpellParamsL];
    fireSpellParams[0] = new Param();
    fireSpellParams[0]->type = PT_float;
    fireSpellParams[1] = new Param();
    fireSpellParams[1]->type = PT_float;

    for (int n = 0; n < 8; n++) {
        char id[32];
        if (n < 4) {
            sprintf(id, "fire%d", n);
        } else {
            sprintf(id, "fire_focus%d", n-4);
        }

        fireSpells[n] = game->runtime.GetSpellTemplate(game->runtime.GetFileId("player_fire"), id);

        if (fireSpells[n]) {
        	fireSpellBooks[n] = fireSpells[n]->ToSpellBook(game, fireSpells[n]->spells,
        			fireSpells[n]->spellsL, fireSpellParams, fireSpellParamsL);
        } else {
        	fireSpellBooks[n] = NULL;
        }
    }

    sprintf(fireFuncName, "fire");
    //sprintf(fireFuncName, "%s_fire", charaId);
    //luaFireScript = new LuaLink(game->runtime.L);

    SetColCircle(0, inttof32(2));

    x = (PLAYER_MIN_X + PLAYER_MAX_X) / 2;
    y = PLAYER_MAX_Y;

    power = START_POWER;

    cooldown = 0;
    bombCooldown = 0;
    focus = false;
    focusSprite = new FocusSprite(game, this);

    bomb = bombFocus = NULL;

    speed = floattof32(2.0f);
    speedFocus = floattof32(0.8f);
    points = 0;
    invincible = 0;
    shotPower = 50;
    extraPoints = 0;
    graze = 0;
    lives = 2;

    #ifdef DEBUG
		shotPower = MAX_SHOT_POWER;
	#endif

	SetShotPowerMul(inttof32(1));
    startBombs = 2;
    bombs = 2;
    grazeRange = inttof32(10);
    dying = -1;

    sndShot = -1;
    justGrazed = false;

    animation = animationDir = animationFrame = 0;
    lastX = x;
}
Player::~Player() {
    if (sndShot >= 0) {
        game->soundManager.StopSound(sndShot);
    }

    if (luaFireScript) {
    	delete luaFireScript;
    }

    for (int n = 0; n < 8; n++) {
    	if (fireSpellBooks[n]) delete fireSpellBooks[n];
    }
    if (fireSpellParams) {
        for (int n = 0; n < fireSpellParamsL; n++) {
            unrefParam(fireSpellParams[n]);
        }
        delete[] fireSpellParams;
    }

    delete focusSprite;
}

void Player::Destroy() {
    if (IsInvincible()) {
        return;
    }

    if (sndShot >= 0) {
        game->soundManager.StopSound(sndShot);
        sndShot = -1;
    }
    game->soundManager.PlaySound("player_killed.wav");

    dying = DEATH_BOMB_TIME;

}

void Player::AddItem(ItemType itemType) {
    u32 pts = 0;

    if (itemType == IT_point || itemType == IT_magnetPoint) {
        pts = 1;
    } else if (itemType == IT_pointLarge) {
        pts = 10;
    } else if (itemType == IT_power) {
        ChangeShotPower(POWER_ITEM_VALUE);
    } else if (itemType == IT_powerLarge) {
        ChangeShotPower(LARGE_POWER_ITEM_VALUE);
    } else if (itemType == IT_extra) {
        extraPoints++;
    } else if (itemType == IT_extraLarge) {
        extraPoints += 10;
    } else if (itemType == IT_life) {
        game->soundManager.PlaySound("gain_life.wav");
        lives = MIN(lives+1, MAX_LIVES);
    } else if (itemType == IT_bomb) {
        bombs = MIN(bombs+1, MAX_BOMBS);
    } else if (itemType == IT_fullPower) {
        ChangeShotPower(MAX_SHOT_POWER);
    }

    if (pts > 0) {
        points += pts;
        game->score += (pts + extraPoints + graze);
    }
}

void Player::UpdateDying() {
    if (dying > 0) {
    	dying--;
    }
    if (dying == 0) {
    	dying = -1;

        if (lives > 0) {
            lives--;
            power = START_POWER;
    		bombs = startBombs;
            invincible = DEATH_INVINCIBLE_TIME;
            bombCooldown = -MIN(30, DEATH_INVINCIBLE_TIME); //negative cooldown disables bombs, without slowing your movement

            //Drop Power
            shotPower -= MIN(shotPower, DEATH_POWER_LOSS);
            int powerDrop = DEATH_POWER_DROP;
            while (powerDrop > 0) {
                Item* item;
                if (powerDrop >= LARGE_POWER_ITEM_VALUE) {
                    item = new Item(game, IT_powerLarge, 2);
                    powerDrop -= LARGE_POWER_ITEM_VALUE;
                } else {
                    item = new Item(game, IT_power, 2);
                    powerDrop -= POWER_ITEM_VALUE;
                }
                item->x = x;
                item->y = y - ITEM_ATR;

                game->AddSprite(item);
                for (int n = 0; n < 3; n++) {
                    item->Update();
                }
            }

            game->OnPlayerKilled();
        } else {
            //Drop FullPower
            for (int n = 0; n < 6; n++) {
                Item* item = new Item(game, IT_fullPower, 2);
                item->x = x;
                item->y = y - ITEM_ATR;

                game->AddSprite(item);
                for (int n = 0; n < 3; n++) {
                    item->Update();
                }
            }

            game->OnPlayerKilled();
            Sprite::Destroy();
            game->OnPlayerDead();
        }
    }
}

void Player::UpdateAnimation() {
    int moveDir = (x - lastX > 0 ? 1 : (x == lastX ? 0 : -1));

    //Update Animations
    animationFrame++;
    if (animation == 0) {
        if (moveDir != 0) {
            animationDir += moveDir;

            if (abs(animationDir) >= 30) {
                animation = 1;
                animationDir = moveDir;
                animationFrame = 0;
            }
        } else {
            animationDir = 0;
        }
    } else if (animation == 1 || animation == 2) {
        if ((moveDir < 0 && animationDir <= 0) || (moveDir > 0 && animationDir >= 0)) {
            //All is well with the world

        } else {
            if (animation == 2) {
                animationFrame = 0;
            } else {
                animationFrame = (3 << ANIM_SHIFT) - animationFrame;
            }
            animation = 3;
        }
    }
    if (animation == 0 || animation == 2) {
        if (animationFrame >= (4 << ANIM_SHIFT)) {
            animationFrame = 0;
        }
    } else if (animation == 1) {
        if (animationFrame >= (3 << ANIM_SHIFT)) {
            animation = 2;
            animationFrame = 0;
        }
    } else if (animation == 3) {
        if (animationFrame >= (3 << ANIM_SHIFT)) {
            animation = 0;
            animationDir = 0;
            animationFrame = 0;
        }
    }

    //Calculate UV
    drawData.uv.x = (animationFrame >> ANIM_SHIFT) * 32;
    drawData.uv.y = animation * 32;
    if (animation == 3) {
    	drawData.uv.x = 64 - (animationFrame >> ANIM_SHIFT) * 32;
    	drawData.uv.y = 32;
    }

    int w = INV_VERTEX_SCALE(drawData.vw);
    if (animation > 0 && animationDir > 0) {
    	drawData.uv.x += w;
    	drawData.uv.w = -w;
    } else {
    	drawData.uv.w = w;
    }

    drawData.OnSizeChanged();

    lastX = x;
}

u8 Player::GetKeys() {
	u32 held = keysHeld();
	u32 down = keysDown();

	u8 keys = 0;

	if (held & KEY_UP)    keys |= VK_up;
	if (held & KEY_DOWN)  keys |= VK_down;
	if (held & KEY_LEFT)  keys |= VK_left;
	if (held & KEY_RIGHT) keys |= VK_right;
	if (held & controls.fireButton)  keys |= VK_fire;
	if (held & controls.focusButton) keys |= VK_focus;

	if (down & controls.bombButton) keys |= VK_bomb;

	return keys;
}

void Player::Update() {
	u8 keys = GetKeys();

    if (keys & VK_bomb) {
    	//int r = (dying >= 0 ? 2 : 1);
    	int r = 1;

        if (bombs > 0 && bombCooldown == 0 && !game->remotePlay) {
        	bool bombOK = false;
            if (luaLink && luaLink->Call("bomb", "")) {
            	bombOK = true;
            }
            if (!bombOK) {
				Bomb* b = CreateBomb();
				if (b) {
					bombOK = true;
					bombCooldown = b->startTime;
				}
            }
            if (bombOK) {
				bombs = MAX(0, (int)bombs - r);

				//Reset power to make deathbombing work
				power = MAX(power, 1);
				dying = -1;

				game->OnBombUsed();
            }
        }
    }

    UpdateDying();

    if (lives < 0 || dying >= 0) {
    	return;
    }

    if (justGrazed) {
        game->soundManager.PlaySound("graze.wav", 5);
        justGrazed = false;
    }

    if (invincible > 0) invincible--;
    if (cooldown > 0) cooldown--;
    if (bombCooldown < 0) bombCooldown++;
    if (bombCooldown > 0) bombCooldown--;
    focus = (keys & VK_focus);

    int speed = (focus || bombCooldown > 0 ? this->speedFocus : this->speed);

    if (keys & VK_left) x -= speed;
    if (keys & VK_right) x += speed;
    if (keys & VK_up) y -= speed;
    if (keys & VK_down) y += speed;

    if (keys & VK_fire) {
        if (cooldown == 0) {
            //Shoot
            if (sndShot < 0) {
                sndShot = game->soundManager.PlaySound("player_fire.wav", 0, true);
            }

            u32 i = 0;
            if (shotPower >= 127) i = 3;
            else if (shotPower >= 80) i = 2;
            else if (shotPower >= 36) i = 1;

            if (focus) i += 4;

            if (fireSpells[i]) {
                fireSpellParams[0]->normal.numval = shotPowerMul1; //Primary shot power
                fireSpellParams[1]->normal.numval = shotPowerMul2; //Secondary shot power

                fireSpellBooks[i]->Reset();
                fireSpellBooks[i]->Update(game, this);
            }

            cooldown = COOLDOWN_TIME;
        }

        if (luaFireScript && !luaFireScript->finished) {
        	luaFireScript->Update();
        }
    } else {
        if (sndShot >= 0) {
            game->soundManager.StopSound(sndShot);
            sndShot = -1;
        }
    }

    x = MAX(PLAYER_MIN_X, MIN(PLAYER_MAX_X, x));
    y = MAX(PLAYER_MIN_Y, MIN(PLAYER_MAX_Y, y));

    focusSprite->SetActive(focus);
    focusSprite->Update();

    Sprite::Update();

    UpdateAnimation();
}

Bomb* Player::CreateBomb() {
    BombInfo* b = bomb;
    if (focus && bombFocus != NULL) {
        b = bombFocus;
    }

    if (b) {
        return b->CreateBomb(game, this, !focus);
    }

    return NULL;
}

void Player::Draw() {
    focusSprite->Draw();

    int oldVW = drawData.vw;
    int oldVH = drawData.vh;

    if (dying >= 0) {
    	s32 s = divf32((s32)dying, DEATH_BOMB_TIME);
    	drawData.vw = mulf32(oldVW, mulf32(s, s));
    }

    if ((invincible & 0x8) == 0) {
        drawData.OnSizeChanged();
        Sprite::Draw();
    }

    drawData.vw = oldVW;
    drawData.vh = oldVH;
}

void Player::ChangeShotPower(s16 delta) {
    int np = shotPower + delta;

    if (shotPower < MAX_SHOT_POWER && np >= MAX_SHOT_POWER) {
        game->ConvertSpritesToPoints(SPRITE_enemyShot);
        game->soundManager.PlaySound("full_power.wav");
    }

    if (np < 0) {
        shotPower = 0;
    } else if (np > MAX_SHOT_POWER) {
        shotPower = MAX_SHOT_POWER;
    } else {
        shotPower = np;
    }
}

void Player::Graze(Sprite* s) {
    graze++;
    s->grazed = true;

    justGrazed = true;
}

bool Player::IsFocus() {
	return focus;
}
u32 Player::GetAttackLevel() {
    if (shotPower >= 100) return 4;
    else if (shotPower >= 75) return 3;
    else if (shotPower >= 50) return 2;
    else if (shotPower >= 25) return 1;
    else return 0;
}
s32 Player::GetAttackPower1() {
	return shotPowerMul1;
}
s32 Player::GetAttackPower2() {
	return shotPowerMul2;
}

bool Player::HasFullPower() {
    return shotPower >= MAX_SHOT_POWER;
}
bool Player::IsInvincible() {
    return invincible > 0 || bombCooldown > 0 || dying >= 0;
}
u32 Player::GetPoints() {
    return points;
}
u16 Player::GetShotPower() {
    return shotPower;
}
u16 Player::GetExtraPoints() {
    return extraPoints;
}
u16 Player::GetGraze() {
    return graze;
}
u8 Player::GetLives() {
    return lives;
}
u8 Player::GetBombs() {
    return bombs;
}

void Player::SetPoints(u32 p) {
    points = p;
}
void Player::SetShotPower(u16 sp) {
    shotPower = sp;
}
void Player::SetExtraPoints(u16 ep) {
    extraPoints = ep;
}
void Player::SetGraze(u16 g) {
    graze = g;
}
void Player::SetLives(u8 l) {
    lives = l;
}
void Player::SetBombs(u8 b) {
    bombs = b;
}

void Player::SetShotPowerMul(s32 s) {
    shotPowerMul = s;

    shotPowerMul1 = (s<<2);
    shotPowerMul2 = (s   );
}
void Player::SetSpeed(s32 s) {
    speed = s;
}
void Player::SetSpeedFocus(s32 sf) {
    speedFocus = sf;
}
void Player::SetGrazeRange(s32 r) {
    grazeRange = r;
}
void Player::SetStartBombs(u8 b) {
    startBombs = b;
	bombs = startBombs;
}

void Player::SetBomb(BombInfo* bomb) {
    this->bomb = bomb;
    if (bomb) {
        game->SetTexBomb(bomb->GetId(), bomb->GetTexture(), true);
    }
}
void Player::SetBombFocus(BombInfo* bombFocus) {
    this->bombFocus = bombFocus;
    if (bombFocus) {
        game->SetTexBomb(bombFocus->GetId(), bombFocus->GetTexture(), false);
    }
}
void Player::SetBombCooldown(u16 frames) {
	bombCooldown = frames;
}
