#include "common_spells.h"
#include "../game.h"
#include "../boss.h"
#include "../parser/compiler.h"

//==============================================================================

BossNameSpell::BossNameSpell(Game* g, Param** p, int pL) {
    nameP = refParam(p[0]);

    Reset();
}
BossNameSpell::~BossNameSpell() {
	unrefParam(nameP);
}

void BossNameSpell::Reset() {
    name = nameP->AsString();

    Spell::Reset();
}

void BossNameSpell::Update() {
    Boss* boss = game->GetBoss();
    if (boss) {
        boss->SetName(name);
    }
    finished = true;
}

void BossNameSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("boss_name", &BossNameSpell::ToSpell);
	decl->SetValidContexts(SPT_boss);
	decl->SetTypesig(0, SPT_string, SPT_null);
	decls[declsL++] = decl;
}
TO_SPELL(BossNameSpell)

//==============================================================================

BossNumSpellcards::BossNumSpellcards(Game* g, Param** p, int pL) {
    valueP = refParam(p[0]);

    Reset();
}
BossNumSpellcards::~BossNumSpellcards() {
	unrefParam(valueP);
}

void BossNumSpellcards::Reset() {
    value = valueP->AsInt();

    Spell::Reset();
}

void BossNumSpellcards::Update() {
    Boss* boss = game->GetBoss();
    if (boss) {
        boss->SetNumSpellcards(value);
    }
    finished = true;
}

void BossNumSpellcards::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("boss_num_spellcards", &BossNumSpellcards::ToSpell);
	decl->SetValidContexts(SPT_boss);
	decl->SetTypesig(0, SPT_int, SPT_null);
	decls[declsL++] = decl;
}
TO_SPELL(BossNumSpellcards)

//==============================================================================

BossCallSpell::BossCallSpell(Param** p, SpellBookTemplate* target, Param** params, int paramsL) {
	this->target = target;

    nameP = refParam(p[0]);
    lifeP = refParam(p[1]);
    timeP = refParam(p[2]);

    this->params = new Param*[paramsL];
    this->paramsL = paramsL;
    for (int n = 0; n < paramsL; n++) {
        this->params[n] = refParam(params[n]);
    }

    Reset();
}

BossCallSpell::~BossCallSpell() {
	unrefParam(nameP);
	unrefParam(lifeP);
	unrefParam(timeP);
    for (int n = 0; n < paramsL; n++) {
    	unrefParam(params[n]);
    }

    if (params) delete[] params;
}

void BossCallSpell::Reset() {
    name = nameP->AsString();
    life = lifeP->AsFixed();
    time = timeP->AsInt();

    Spell::Reset();
}

void BossCallSpell::Update() {
    Boss* boss = game->GetBoss();
    if (boss) {
        boss->SetSpellCard(target->ToSpellBook(game, target->spells, target->spellsL, params, paramsL),
        		life, time, name);
    }
    finished = true;
}

void BossCallSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("boss_spell", &BossCallSpell::ToSpell);
	decl->SetValidContexts(SPT_boss);
	decl->SetTypesig(0, SPT_string, SPT_float, SPT_int, SPT_callFlag, SPT_spell, SPT_null);
	decls[declsL++] = decl;
}
Spell* BossCallSpell::ToSpell(Game* game, u16 fileId, Param** p, u8 pL) {
	SpellBookTemplate* sbt = p[3]->AsSpell(game, fileId);
    if (sbt && sbt->type == SPELL) return new BossCallSpell(p, sbt, p + 4, pL - 4);
    else { compilerLog(EL_error, __FILENAME__, "Invalid spell: %s", p[3]->AsString()); return NULL; }
}

//==============================================================================
