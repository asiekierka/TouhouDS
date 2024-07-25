#include "common_spells.h"
#include "../game.h"
#include "../item.h"
#include "../sprite.h"
#include "../player.h"

//==============================================================================

ObjectImageSpell::ObjectImageSpell(Game* g, Param** p, int pL) {
    game = g;

    textureP = refParam(p[0]);
    xP = refParam(p[1]);
    yP = refParam(p[2]);
    wP = refParam(p[3]);
    hP = refParam(p[4]);

    Reset();
}
ObjectImageSpell::~ObjectImageSpell() {
	unrefParam(textureP);
	unrefParam(xP);
	unrefParam(yP);
	unrefParam(wP);
	unrefParam(hP);
}
void ObjectImageSpell::Reset() {
    texture = textureP->AsTexture(game);
    x = xP->AsInt();
    y = yP->AsInt();
    w = wP->AsInt();
    h = hP->AsInt();

    Spell::Reset();
}
void ObjectImageSpell::Update() {
    owner->drawData.visible = (texture != NULL);
    owner->drawData.texture = texture;
    owner->drawData.uv = UV(x, y, w, h);
    owner->drawData.vw = VERTEX_SCALE(w);
    owner->drawData.vh = VERTEX_SCALE(h);
    owner->drawData.OnSizeChanged();
    finished = true;
}
void ObjectImageSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("image", &ObjectImageSpell::ToSpell);
	decl->SetValidContexts(SPT_spell|SPT_object|SPT_boss|SPT_animation);
	decl->SetTypesig(0, SPT_texture, SPT_int, SPT_int, SPT_int, SPT_int, SPT_null);
	decls[declsL++] = decl;
}
TO_SPELL(ObjectImageSpell)

//==============================================================================

AdvanceSpell::AdvanceSpell(Game* g, Param** p, int pL) {
    amountP = refParam(p[0]);

    Reset();
}
AdvanceSpell::~AdvanceSpell() {
	unrefParam(amountP);
}
void AdvanceSpell::Reset() {
    amount = amountP->AsFixed();
    Spell::Reset();
}
void AdvanceSpell::Update() {
    owner->Advance(amount);
    finished = true;
}

void AdvanceSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("advance", &AdvanceSpell::ToSpell);
	decl->SetTypesig(0, SPT_float, SPT_null);
	decls[declsL++] = decl;
}
TO_SPELL(AdvanceSpell)

//==============================================================================

AdvanceSideSpell::AdvanceSideSpell(Game* g, Param** p, int pL) {
    amountP = refParam(p[0]);

    Reset();
}
AdvanceSideSpell::~AdvanceSideSpell() {
	unrefParam(amountP);
}
void AdvanceSideSpell::Reset() {
    amount = amountP->AsFixed();

    Spell::Reset();
}
void AdvanceSideSpell::Update() {
    owner->AdvanceSide(amount);
    finished = true;
}

void AdvanceSideSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("advance_side", &AdvanceSideSpell::ToSpell);
	decl->SetTypesig(0, SPT_float, SPT_null);
	decls[declsL++] = decl;
}
TO_SPELL(AdvanceSideSpell)

//==============================================================================

DieSpell::DieSpell(Game* g, Param** p, int pL) {
}
DieSpell::~DieSpell() {
}

void DieSpell::Update() {
    if (!owner) {
        finished = true; //We're fucked, time to bail
        return;
    }

    owner->Destroy();
    finished = true;
}

void DieSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("die", &DieSpell::ToSpell);
	decl->SetTypesig(0, SPT_null);
	decls[declsL++] = decl;
}
TO_SPELL(DieSpell)

//==============================================================================

DropSpell::DropSpell(Game* g, Param** p, int pL) {
    itemP = refParam(p[0]);
    amountP = (pL > 1 ? refParam(p[1]) : NULL);
    chanceP = (pL > 2 ? refParam(p[2]) : NULL);

    Reset();
}
DropSpell::~DropSpell() {
	unrefParam(itemP);
	if (amountP) unrefParam(amountP);
	if (chanceP) unrefParam(chanceP);
}

void DropSpell::Reset() {
    item   = itemP->AsItemType();
    amount = (amountP != NULL ? amountP->AsInt() : 1);
    chance = (chanceP != NULL ? chanceP->AsInt() : 100);

    Spell::Reset();
}

void DropSpell::Update() {
    if (!owner) {
        finished = true; //We're fucked, time to bail
        return;
    }

    bool fp = game->players[0]->HasFullPower();
    for (int n = 0; n < amount; n++) {
        if (rand() % 100 < chance) {
            if (fp) {
                if (item == IT_power) {
                    item = IT_extra;
                } else if (item == IT_powerLarge) {
                    item = IT_extraLarge;
                }
            }

            Item* result = new Item(game, item);
            if (owner) {
                result->x = owner->x;
                result->y = owner->y;
            } else {
                result->x = inttof32(SCREEN_WIDTH>>1);
                result->y = 0;
            }
            game->AddSprite(result);
        }
    }

    finished = true;
}

void DropSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("drop", &DropSpell::ToSpell);
	decl->SetTypesig(0, SPT_itemType, SPT_null);
	decl->SetTypesig(1, SPT_itemType, SPT_int, SPT_null);
	decl->SetTypesig(2, SPT_itemType, SPT_int, SPT_int, SPT_null);
	decls[declsL++] = decl;
}
TO_SPELL(DropSpell)

//==============================================================================
MoveSpell::MoveSpell(Game* g, Param** p, int pL, bool rel) {
    xP = refParam(p[0]);
    yP = refParam(p[1]);
    relative = rel;

    Reset();
}
MoveSpell::~MoveSpell() {
	unrefParam(xP);
	unrefParam(yP);
}

void MoveSpell::Reset() {
    x = xP->AsFixed();
    y = yP->AsFixed();

    Spell::Reset();
}

void MoveSpell::Update() {
    if (!owner) {
        finished = true; //We're fucked, time to bail
        return;
    }

    if (relative) {
        owner->x += x;
        owner->y += y;
    } else {
        owner->x = x;
        owner->y = y;
    }
    finished = true;
}

Spell* create_pos(Game* game, u16 fileId, Param** p, u8 pL) {
	return new MoveSpell(game, p, pL);
}
Spell* create_move(Game* game, u16 fileId, Param** p, u8 pL) {
	return new MoveSpell(game, p, pL, true);
}

void MoveSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* posDecl = new SpellDecl("pos", &create_pos);
	posDecl->SetTypesig(0, SPT_float, SPT_float, SPT_null);
	decls[declsL++] = posDecl;
	SpellDecl* moveDecl = new SpellDecl("move", &create_move);
	moveDecl->SetTypesig(0, SPT_float, SPT_float, SPT_null);
	decls[declsL++] = moveDecl;
}

//==============================================================================
MoveToSpell::MoveToSpell(Game* g, Param** p, int pL) {
    xP = refParam(p[0]);
    yP = refParam(p[1]);
    sP = refParam(p[2]);

    Reset();
}
MoveToSpell::~MoveToSpell() {
	unrefParam(xP);
	unrefParam(yP);
	unrefParam(sP);
}

void MoveToSpell::Reset() {
    x = xP->AsFixed();
    y = yP->AsFixed();
    s = sP->AsFixed();

    angle = -1;

    Spell::Reset();
}

void MoveToSpell::Update() {
    owner->SetSpeed(0);
    owner->SetSpeedInc(0);

    if (angle < 0) {
        angle = game->CalculateAngle(owner->x, owner->y, x, y);
        //log("move_to %d %d %d %d %d", f32toint(owner->x), f32toint(owner->y), f32toint(x), f32toint(y), angle);
    }

    int sx = owner->x;
    int sy = owner->y;
    int dx = mulf32(s, fastSin(angle));
    int dy = -mulf32(s, fastCos(angle));

    if ((sx <= x && sx+dx >= x) || (sx >= x && sx+dx <= x)) {
        dx = 0;
    }
    if ((sy <= y && sy+dy >= y) || (sy >= y && sy+dy <= y)) {
        dy = 0;
    }

    if (dx == 0 && dy == 0) {
        finished = true;
    } else {
        owner->x += dx;
        owner->y += dy;
    }
}

void MoveToSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("move_to", &MoveToSpell::ToSpell);
	decl->SetTypesig(0, SPT_float, SPT_float, SPT_float, SPT_null);
	decls[declsL++] = decl;
}
TO_SPELL(MoveToSpell)

//==============================================================================

SpeedToSpell::SpeedToSpell(Game* g, Param** p, int pL) {
    valueP = refParam(p[0]);
    accelP = refParam(p[1]);

    Reset();
}
SpeedToSpell::~SpeedToSpell() {
	unrefParam(valueP);
	unrefParam(accelP);
}

void SpeedToSpell::Reset() {
    value = valueP->AsFixed();
    accel = accelP->AsFixed();

    Spell::Reset();
}

void SpeedToSpell::Update() {
    owner->SetSpeedInc(0);

    s32 spd = owner->GetSpeed();
    if (abs(spd - value) < accel) {
    	owner->SetSpeed(value);
        finished = true;
    } else {
        if (spd > value) {
        	owner->SetSpeed(spd - accel);
        } else {
        	owner->SetSpeed(spd + accel);
        }
    }
}

void SpeedToSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("speed_to", &SpeedToSpell::ToSpell);
	decl->SetTypesig(0, SPT_float, SPT_float, SPT_null);
	decls[declsL++] = decl;
}
TO_SPELL(SpeedToSpell)

//==============================================================================

TurnToSpell::TurnToSpell(Game* g, Param** p, int pL) {
    angleP = refParam(p[0]);
    deltaP = (pL > 1 ? refParam(p[1]) : NULL);

    Reset();
}
TurnToSpell::~TurnToSpell() {
	unrefParam(angleP);
	if (deltaP) unrefParam(deltaP);
}

void TurnToSpell::Reset() {
    if (angleP->type == PT_float || angleP->type == PT_int || angleP->type == PT_random) {
        target = AT_none;
        value = (angleP->AsInt() << 6) & (DEGREES_IN_CIRCLE-1);
    } else {
        target = angleP->AsAngleTarget();
        value = -1;
    }
    delta = (deltaP ? (deltaP->AsInt() << 6) & (DEGREES_IN_CIRCLE-1) : 1);

    Spell::Reset();
}

void TurnToSpell::Update() {
	owner->SetAngleInc(0);
    if (value < 0) { //Find out what's the target angle
        if (!game->CalculateAngle(&value, owner->x, owner->y, target)) {
            //Fatal Error: can't determine angle. Maybe the target doesn't exist?
            finished = true;
            return;
        }
    }

    int ang = owner->GetAngle();
    int d = value-ang;
    if (abs(d) < delta) {
        owner->SetAngle(value);
        finished = true;
    } else {
        //log("%d %d %d %d", s2a, value, delta, d);

        if (d <= -DEGREES_IN_CIRCLE/2 || (d >= 0 && d <= DEGREES_IN_CIRCLE/2)) {
            owner->SetAngle(ang + delta);
        } else {
            owner->SetAngle(ang - delta);
        }
    }
}

void TurnToSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("turn_to", &TurnToSpell::ToSpell);
	decl->SetTypesig(0, SPT_int, SPT_null);
	decl->SetTypesig(1, SPT_int, SPT_int, SPT_null);
	decl->SetTypesig(2, SPT_angleTarget, SPT_null);
	decl->SetTypesig(3, SPT_angleTarget, SPT_int, SPT_null);
	decls[declsL++] = decl;
}
TO_SPELL(TurnToSpell)

//==============================================================================

HitBoxSpell::HitBoxSpell(Game* g, Param** p, int pL) {
    rP = refParam(p[0]);

    Reset();
}
HitBoxSpell::~HitBoxSpell() {
	unrefParam(rP);
}

void HitBoxSpell::Reset() {
    r = rP->AsFixed();

    Spell::Reset();
}

void HitBoxSpell::Update() {
    owner->SetColCircle(0, r);
    finished = true;
}

void HitBoxSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("hitbox", &HitBoxSpell::ToSpell);
	decl->SetTypesig(0, SPT_int, SPT_null);
	decls[declsL++] = decl;
}
TO_SPELL(HitBoxSpell)

//==============================================================================

PowerSpell::PowerSpell(Game* g, Param** p, int pL) {
    powerP = refParam(p[0]);

    Reset();
}
PowerSpell::~PowerSpell() {
	unrefParam(powerP);
}

void PowerSpell::Reset() {
    power = powerP->AsFixed();

    Spell::Reset();
}

void PowerSpell::Update() {
	owner->power = power;
    finished = true;
}

void PowerSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("power", &PowerSpell::ToSpell);
	decl->SetTypesig(0, SPT_float, SPT_null);
	decls[declsL++] = decl;
}
TO_SPELL(PowerSpell)

//==============================================================================

SpeedSpell::SpeedSpell(Game* g, Param** p, int pL, bool a) {
    valueP = refParam(p[0]);

    isAccel = a;

    Reset();
}
SpeedSpell::~SpeedSpell() {
	unrefParam(valueP);
}

void SpeedSpell::Reset() {
    value = valueP->AsFixed();

    Spell::Reset();
}

void SpeedSpell::Update() {
    if (isAccel) {
        owner->SetSpeedInc(value);
    } else {
        owner->SetSpeed(value);
    }
    finished = true;
}

Spell* create_speed(Game* game, u16 fileId, Param** p, u8 pL) {
	return new SpeedSpell(game, p, pL, false);
}
Spell* create_speed_inc(Game* game, u16 fileId, Param** p, u8 pL) {
	return new SpeedSpell(game, p, pL, true);
}

void SpeedSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* speedDecl = new SpellDecl("speed", &create_speed);
	speedDecl->SetTypesig(0, SPT_float, SPT_null);
	decls[declsL++] = speedDecl;
	SpellDecl* speedIncDecl = new SpellDecl("speed_inc", &create_speed_inc);
	speedIncDecl->SetTypesig(0, SPT_float, SPT_null);
	decls[declsL++] = speedIncDecl;
}

//==============================================================================

AngleSpell::AngleSpell(Param* angle, bool a, bool r) {
    angleP = refParam(angle);
    isAccel = a;
    relative = r;

    Reset();
}
AngleSpell::~AngleSpell() {
	unrefParam(angleP);
}

void AngleSpell::Reset() {
    if (angleP->type == PT_float || angleP->type == PT_int || angleP->type == PT_random) {
        target = AT_none;
        value = (angleP->AsInt() << 6) & (DEGREES_IN_CIRCLE-1);
    } else {
        target = angleP->AsAngleTarget();
        value = -1;
    }

    Spell::Reset();
}

void AngleSpell::Update() {
    if (value < 0) { //Find out what's the target angle
        if (!game->CalculateAngle(&value, owner->x, owner->y, target)) {
            //Fatal Error: can't determine angle. Maybe the target doesn't exist?
            finished = true;
            return;
        }
    }

	if (isAccel) {
        owner->SetAngleInc(relative ? owner->GetAngleInc()+value : value);
    } else {
        owner->SetAngle(relative ? owner->GetAngle()+value : value);
    }

    finished = true;
}

Spell* create_angle(Game* game, u16 fileId, Param** p, u8 pL) {
	return new AngleSpell(p[0], false, false);
}
Spell* create_angle_inc(Game* game, u16 fileId, Param** p, u8 pL) {
	return new AngleSpell(p[0], true, false);
}
Spell* create_turn(Game* game, u16 fileId, Param** p, u8 pL) {
	return new AngleSpell(p[0], false, true);
}

void AngleSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* angleDecl = new SpellDecl("angle", &create_angle);
	angleDecl->SetTypesig(0, SPT_int, SPT_null);
	angleDecl->SetTypesig(1, SPT_angleTarget, SPT_null);
	decls[declsL++] = angleDecl;
	SpellDecl* angleIncDecl = new SpellDecl("angle_inc", &create_angle_inc);
	angleIncDecl->SetTypesig(0, SPT_int, SPT_null);
	decls[declsL++] = angleIncDecl;
	SpellDecl* turnDecl = new SpellDecl("turn", &create_turn);
	turnDecl->SetTypesig(0, SPT_int, SPT_null);
	decls[declsL++] = turnDecl;
}

//==============================================================================

VisibilitySpell::VisibilitySpell(bool visibility) {
    this->visibility = visibility;
}
VisibilitySpell::~VisibilitySpell() {
}

void VisibilitySpell::Update() {
    owner->drawData.visible = visibility;
    finished = true;
}

Spell* create_show(Game* game, u16 fileId, Param** p, u8 pL) {
	return new VisibilitySpell(true);
}
Spell* create_hide(Game* game, u16 fileId, Param** p, u8 pL) {
	return new VisibilitySpell(false);
}

void VisibilitySpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* showDecl = new SpellDecl("show", &create_show);
	showDecl->SetTypesig(0, SPT_null);
	decls[declsL++] = showDecl;
	SpellDecl* hideDecl = new SpellDecl("hide", &create_hide);
	hideDecl->SetTypesig(0, SPT_null);
	decls[declsL++] = hideDecl;
}

//==============================================================================

SizeSpell::SizeSpell(Game* g, Param** p, int pL) {
    wP = refParam(p[0]);
    hP = refParam(p[1]);

    Reset();
}
SizeSpell::~SizeSpell() {
	unrefParam(wP);
	unrefParam(hP);
}

void SizeSpell::Reset() {
    w = wP->AsInt();
    h = hP->AsInt();

    Spell::Reset();
}

void SizeSpell::Update() {
    owner->drawData.vw = VERTEX_SCALE(w);
    owner->drawData.vh = VERTEX_SCALE(h);
    owner->drawData.OnSizeChanged();
    finished = true;
}

void SizeSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("size", &SizeSpell::ToSpell);
	decl->SetValidContexts(SPT_spell|SPT_object|SPT_boss|SPT_animation);
	decl->SetTypesig(0, SPT_int, SPT_int, SPT_null);
	decls[declsL++] = decl;
}
TO_SPELL(SizeSpell)

//==============================================================================

DrawAngleSpell::DrawAngleSpell(Game* g, Param** p, int pL) {
    modeP = refParam(p[0]);
    valueP = (pL > 1 ? refParam(p[1]) : NULL);

    Reset();
}
DrawAngleSpell::~DrawAngleSpell() {
	unrefParam(modeP);
	if (valueP) unrefParam(valueP);
}

void DrawAngleSpell::Reset() {
    mode = modeP->AsDrawAngleMode();
    if (valueP) {
		if (valueP->type == PT_float || valueP->type == PT_int || valueP->type == PT_random) {
			target = AT_none;
			value = (valueP->AsInt() << 6) & (DEGREES_IN_CIRCLE-1);
		} else {
			target = valueP->AsAngleTarget();
			value = -1;
		}
    } else {
    	value = 0;
    }

    Spell::Reset();
}

void DrawAngleSpell::Update() {
    if (value < 0) { //Find out what's the target angle
        if (!game->CalculateAngle(&value, owner->x, owner->y, target)) {
            //Fatal Error: can't determine angle. Maybe the target doesn't exist?
            finished = true;
            return;
        }
    }

    if (mode == DAM_manual) {
        owner->SetDrawAngleMode(DAM_manual);
        owner->SetDrawAngle(value);
    } else if (mode == DAM_rotate) {
        owner->SetDrawAngleMode(DAM_manual);
    	owner->SetDrawAngle(owner->GetDrawAngle() + value);
    } else {
        owner->SetDrawAngleMode(mode);
    }
    finished = true;
}

void DrawAngleSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("draw_angle", &DrawAngleSpell::ToSpell);
	decl->SetValidContexts(SPT_spell|SPT_object|SPT_boss|SPT_animation);
	decl->SetTypesig(0, SPT_drawAngleMode, SPT_null);
	decl->SetTypesig(1, SPT_drawAngleMode, SPT_int, SPT_null);
	decls[declsL++] = decl;
}
TO_SPELL(DrawAngleSpell)

//==============================================================================
