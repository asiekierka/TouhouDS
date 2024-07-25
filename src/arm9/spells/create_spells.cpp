#include "common_spells.h"
#include "../boss.h"
#include "../charinfo.h"
#include "../conversation.h"
#include "../game.h"
#include "../level.h"
#include "../player.h"
#include "../sprite.h"

//==============================================================================

AnimationSpell::AnimationSpell(SpellBookTemplate* target, Param** ps, int psL) {
    this->target = target;

    paramsL = MIN(MAX_SPELL_PARAMS, psL);
    for (int n = 0; n < paramsL; n++) {
        params[n] = refParam(ps[n]);
    }
}
AnimationSpell::~AnimationSpell() {
	for (int n = 0; n < paramsL; n++) {
		unrefParam(params[n]);
	}
}

void AnimationSpell::Update() {
	owner->SetAnimationSpellBook(target->ToSpellBook(game,
			target->spells, target->spellsL, params, paramsL));
	finished = true;
}

void AnimationSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("anim", &AnimationSpell::ToSpell);
	decl->SetTypesig(0, SPT_callFlag, SPT_animation, SPT_null);
	decls[declsL++] = decl;
}
Spell* AnimationSpell::ToSpell(Game* game, u16 fileId, Param** p, u8 pL) {
    SpellBookTemplate* sbt = p[0]->AsSpell(game, fileId);
    if (sbt && sbt->type == ANIMATION) return new AnimationSpell(sbt, p + 1, pL - 1);
    else { compilerLog(EL_error, __FILENAME__, "Invalid spell: %s", p[0]->AsString()); return NULL; }
}

//==============================================================================

CharaAnimationSpell::CharaAnimationSpell(Param* animType, SpellBookTemplate* target, Param** ps, int psL) {
	this->animTypeP = refParam(animType);
    this->target = target;
    paramsL = MIN(MAX_SPELL_PARAMS, psL);
    for (int n = 0; n < paramsL; n++) {
        params[n] = refParam(ps[n]);
    }

    Reset();
}
CharaAnimationSpell::~CharaAnimationSpell() {
	unrefParam(animTypeP);
	for (int n = 0; n < paramsL; n++) {
		unrefParam(params[n]);
	}
}

void CharaAnimationSpell::Reset() {
	Spell::Reset();

	animType = animTypeP->AsCharaAnimationType();
}

void CharaAnimationSpell::Update() {
    //This spell only works on boss objects right now
    Boss* boss = (Boss*)owner;
    boss->SetCharaAnim(animType,
    		target->ToSpellBook(game, target->spells, target->spellsL, params, paramsL));
	finished = true;
}

void CharaAnimationSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("chara_anim", &CharaAnimationSpell::ToSpell);
	decl->SetValidContexts(SPT_boss);
	decl->SetTypesig(0, SPT_charaAnimationType, SPT_callFlag, SPT_animation, SPT_null);
	decls[declsL++] = decl;
}
Spell* CharaAnimationSpell::ToSpell(Game* game, u16 fileId, Param** p, u8 pL) {
	SpellBookTemplate* sbt = p[1]->AsSpell(game, fileId);
    if (sbt && sbt->type == ANIMATION) return new CharaAnimationSpell(p[0], sbt, p + 2, pL - 2);
    else { compilerLog(EL_error, __FILENAME__, "Invalid spell: %s", p[0]->AsString()); return NULL; }
}

//==============================================================================

FireSpell::FireSpell(ObjectTemplate* proj, Param** ps, int psL) {
    projectile = proj;
    paramsL = MIN(MAX_SPELL_PARAMS, psL);
    for (int n = 0; n < paramsL; n++) {
        params[n] = refParam(ps[n]);
    }
}
FireSpell::~FireSpell() {
	for (int n = 0; n < paramsL; n++) {
		unrefParam(params[n]);
	}
}

ITCM_CODE
void FireSpell::Update() {
	SpriteType type = SPRITE_enemyShot;
    if (owner->type == SPRITE_player || owner->type == SPRITE_playerShot) {
    	type = SPRITE_playerShot;
    } else if (owner->type == SPRITE_remote) {
    	type = SPRITE_remote;
    }

    Sprite* result = projectile->ToObject(game, type, params, paramsL);
    if (result) {
    	if (type == SPRITE_enemyShot) {
    		result->SetAngle(owner->GetAngle());
    	}

        if (owner) {
            result->x = owner->x;
            result->y = owner->y;
        } else {
            result->x = inttof32(SCREEN_WIDTH >> 1);
            result->y = 0;
        }
        game->AddSprite(result);
    }

    finished = true;
}

void FireSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("fire", &FireSpell::ToSpell);
	decl->SetTypesig(0, SPT_callFlag, SPT_object, SPT_null);
	decls[declsL++] = decl;
}
Spell* FireSpell::ToSpell(Game* game, u16 fileId, Param** p, u8 pL) {
	SpellBookTemplate* sbt = p[0]->AsSpell(game, fileId);
    if (sbt && sbt->type == OBJECT) return new FireSpell((ObjectTemplate*)sbt, p + 1, pL - 1);
    else { compilerLog(EL_error, __FILENAME__, "Invalid spell: %s", p[0]->AsString()); return NULL; }
}

//==============================================================================

CreateSpell::CreateSpell(ObjectTemplate* object, Param** ps, int psL, bool createAsBoss) {
    this->object = object;
    this->createAsBoss = createAsBoss;

    paramsL = MIN(MAX_SPELL_PARAMS, psL);
    for (int n = 0; n < paramsL; n++) {
        params[n] = refParam(ps[n]);
    }
}
CreateSpell::~CreateSpell() {
	for (int n = 0; n < paramsL; n++) {
		unrefParam(params[n]);
	}
}

ITCM_CODE
void CreateSpell::Update() {
    SpriteType type = SPRITE_enemy;

    Sprite* result = NULL;
    if (createAsBoss) {
        result = object->ToBoss(game, params, paramsL);
    } else {
        result = object->ToObject(game, type, params, paramsL);
    }
    //Sprite2* result = new Sprite2(game, SPRITE_ENEMY, game->texEnemy,
    //    inttof32(LEVEL_WIDTH/2), inttof32(-4), 16, 16, Rect(0, 0, 16, 16));

    if (result) {
        result->SetSpeed(floattof32(1));
        result->SetAngle(DEGREES_IN_CIRCLE/2);

        if (owner) {
            result->x = owner->x;
            result->y = owner->y;
        } else {
            result->x = inttof32(SCREEN_WIDTH/2);
            result->y = 0;
        }

        if (createAsBoss) {
            game->SetBoss((Boss*)result);
        } else {
            game->AddSprite(result);
        }
    }
    finished = true;
}

Spell* create_create(Game* game, u16 fileId, Param** p, u8 pL) {
	SpellBookTemplate* sbt = p[0]->AsSpell(game, fileId);
	if (sbt && sbt->type == OBJECT) return new CreateSpell((ObjectTemplate*)sbt, p + 1, pL - 1, false);
	else { compilerLog(EL_error, __FILENAME__, "Invalid spell: %s", p[0]->AsString()); return NULL; }
}
Spell* create_create_boss(Game* game, u16 fileId, Param** p, u8 pL) {
	SpellBookTemplate* sbt = p[0]->AsSpell(game, fileId);
	if (sbt && sbt->type == BOSS) return new CreateSpell((ObjectTemplate*)sbt, p + 1, pL - 1, true);
	else { compilerLog(EL_error, __FILENAME__, "Invalid spell: %s", p[0]->AsString()); return NULL; }
}

void CreateSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* createDecl = new SpellDecl("create", &create_create);
	createDecl->SetTypesig(0, SPT_callFlag, SPT_object, SPT_null);
	decls[declsL++] = createDecl;
	SpellDecl* bossDecl = new SpellDecl("create_boss", &create_create_boss);
	bossDecl->SetTypesig(0, SPT_callFlag, SPT_boss, SPT_null);
	decls[declsL++] = bossDecl;
}

//==============================================================================

CallSpell::CallSpell(SpellBookTemplate* target, Param** ps, int psL) {
    this->target = target;
    this->spellbook = NULL;

    paramsL = MIN(MAX_SPELL_PARAMS, psL);
    for (int n = 0; n < paramsL; n++) {
        params[n] = refParam(ps[n]);
    }
}
CallSpell::~CallSpell() {
	for (int n = 0; n < paramsL; n++) {
		unrefParam(params[n]);
	}

	if (spellbook) delete spellbook;
}

void CallSpell::Reset() {
    Spell::Reset();

    if (spellbook) {
        spellbook->Reset();
    }
}

ITCM_CODE
void CallSpell::Update() {
    if (!spellbook) {
        spellbook = target->ToSpellBook(game, target->spells, target->spellsL, params, paramsL);
    } else {
        if (spellbook && !spellbook->finished) {
            spellbook->Update(game, owner);
            finished = spellbook->finished;
        } else {
            finished = true;
        }
    }
}

void CallSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("call", &CallSpell::ToSpell);
	decl->SetTypesig(0, SPT_callFlag, SPT_spell, SPT_null);
	decls[declsL++] = decl;
}
Spell* CallSpell::ToSpell(Game* game, u16 fileId, Param** p, u8 pL) {
	SpellBookTemplate* sbt = p[0]->AsSpell(game, fileId);
    if (sbt && sbt->type == SPELL) return new CallSpell(sbt, p + 1, pL - 1);
    else { compilerLog(EL_error, __FILENAME__, "Invalid spell: %s", p[0]->AsString()); return NULL; }
}

//==============================================================================

ForkSpell::ForkSpell(SpellBookTemplate* target, Param** ps, int psL) {
    this->target = target;
    paramsL = MIN(MAX_SPELL_PARAMS, psL);
    for (int n = 0; n < paramsL; n++) {
        params[n] = refParam(ps[n]);
    }
}
ForkSpell::~ForkSpell() {
	for (int n = 0; n < paramsL; n++) {
		unrefParam(params[n]);
	}
}

void ForkSpell::Update() {
    SpellBook* spellbook = target->ToSpellBook(game, target->spells, target->spellsL, params, paramsL);
    if (spellbook) {
        game->level->AddThread(spellbook);
    }
    finished = true;
}

void ForkSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("fork", &ForkSpell::ToSpell);
	decl->SetTypesig(0, SPT_callFlag, SPT_spell, SPT_null);
	decls[declsL++] = decl;
}
Spell* ForkSpell::ToSpell(Game* game, u16 fileId, Param** p, u8 pL) {
	SpellBookTemplate* sbt = p[0]->AsSpell(game, fileId);
    if (sbt && sbt->type == SPELL) return new ForkSpell(sbt, p + 1, pL - 1);
    else { compilerLog(EL_error, __FILENAME__, "Invalid spell: %s", p[0]->AsString()); return NULL; }
}

//==============================================================================

ConversationSpell::ConversationSpell(SpellBookTemplate* target, Param** ps, int psL) {
    this->target = target;
    paramsL = MIN(MAX_SPELL_PARAMS, psL);
    for (int n = 0; n < paramsL; n++) {
        params[n] = refParam(ps[n]);
    }
}
ConversationSpell::~ConversationSpell() {
	for (int n = 0; n < paramsL; n++) {
		unrefParam(params[n]);
	}

	if (params) delete[] params;
}

void ConversationSpell::Update() {
	game->SetConversation(new Conversation(game,
			target->ToSpellBook(game, target->spells, target->spellsL, params, paramsL)));
	finished = true;
}

void ConversationSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("conv", &ConversationSpell::ToSpell);
	decl->SetTypesig(0, SPT_callFlag, SPT_conversation, SPT_null);
	decls[declsL++] = decl;
}
Spell* ConversationSpell::ToSpell(Game* game, u16 fileId, Param** p, u8 pL) {
	SpellBookTemplate* sbt = NULL;
	char id[256];
	if (game->GetSelectedCharacter() && game->GetSelectedCharacter()->GetId()) {
		//Try to find a specialized conv for the current character
		sprintf(id, "%s_%s", p[0]->AsString(), game->GetSelectedCharacter()->GetId());
    	sbt = game->runtime.GetSpellTemplate(fileId, id, true);
	}
	if (!sbt) {
		//Use the general conv if not specific conv found
    	sbt = game->runtime.GetSpellTemplate(fileId, p[0]->AsString(), true);
	}
    if (sbt && sbt->type == CONVERSATION) {
    	return new ConversationSpell(sbt, p + 1, pL - 1);
    } else {
    	return NULL;
    }
}

//==============================================================================

ForSpell::ForSpell(Game* g, int fid, Param** p, int pL, SpellTemplate* tgt, Param** gl, int glL)
	: game(g), fileId(fid), target(tgt), spell(NULL)
{
	startP = refParam(p[0]);
	endP = refParam(p[1]);
	stepP = refParam(p[2]);

	//Replace '$$' with a reference to our local var.
	Param** params = target->params;
	int paramsL = target->paramsL;

    globalsL = MIN(MAX_SPELL_PARAMS, paramsL);
	for (int n = 0; n < paramsL; n++) {
		globals[n] = refParam(resolveVar(params[n], gl, glL));
    }

    Reset();
}
ForSpell::~ForSpell() {
	unrefParam(startP);
	unrefParam(endP);
	unrefParam(stepP);
	for (int n = 0; n < globalsL; n++) {
		unrefParam(globals[n]);
	}

	if (spell) delete spell;
}

void ForSpell::Reset() {
    Spell::Reset();

    start = startP->AsFixed();
    end = endP->AsFixed();
    step = stepP->AsFixed();
}

ITCM_CODE
void ForSpell::Update() {
	int indices[globalsL];
	int indicesL = 0;
    Param* g[globalsL];
	for (int i = 0; i < globalsL; i++) {
		g[i] = globals[i];

		if (globals[i]->type == PT_variable && globals[i]->normal.numval == -1) {
			indices[indicesL++] = i;
		}
	}

    s32 n = start;
    do {
    	Param* var = new Param();
    	var->type = PT_float;
    	var->normal.numval = n;

    	for (int i = 0; i < indicesL; i++) {
   			g[indices[i]] = var;
    	}
    	spell = target->decl->ToSpell(game, fileId, g, globalsL);

    	//spell->Reset();
    	do {
    		spell->Update();
    	} while (!spell->finished);

    	unrefParam(var);
        delete spell;
        spell = NULL;

    	n += step;

    	if (start == end) break;
    	if (start <= end && n > end) break;
    	if (start >= end && n < end) break;
    } while (true);

	finished = true;
}

//==============================================================================
