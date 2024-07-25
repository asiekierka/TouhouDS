#include "common_spells.h"
#include "../as_lib9.h"
#include "../game.h"
#include "../gameinfo.h"
#include "../level.h"
#include "../player.h"

//==============================================================================

WaitSpell::WaitSpell(Game* g, Param** p, int pL) {
    amountP = refParam(p[0]);

    Reset();
}
WaitSpell::~WaitSpell() {
	unrefParam(amountP);
}

void WaitSpell::Reset() {
    amount = amountP->AsInt();

    Spell::Reset();
}

void WaitSpell::Update() {
	spellBook->wait += amount;
    finished = true;
}

void WaitSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("wait", &WaitSpell::ToSpell);
	decl->SetValidContexts(SPT_spell|SPT_object|SPT_boss|SPT_animation);
	decl->SetTypesig(0, SPT_int, SPT_null);
	decls[declsL++] = decl;
}
TO_SPELL(WaitSpell)

//==============================================================================

EndStageSpell::EndStageSpell(Game* g, Param** p, int pL) {
    Reset();
}
EndStageSpell::~EndStageSpell() {
}

void EndStageSpell::Reset() {
    Spell::Reset();
    delay = 300;
}
void EndStageSpell::Update() {
    if (owner) {
        Spell* spells[] = { new EndStageSpell(game, NULL, 0) };
        SpellBook* sb = new SpellBook(spells, 1);
        game->level->AddThread(sb);
        finished = true;
    } else {
        if (delay == 0) {
            game->EndStage();
            finished = true;
        } else {
            delay--;
        }
    }
}

void EndStageSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("end_stage", &EndStageSpell::ToSpell);
	decl->SetTypesig(0, SPT_null);
	decls[declsL++] = decl;
}
TO_SPELL(EndStageSpell)

//==============================================================================

BGMSpell::BGMSpell(Game* g, Param** p, int pL) {
    bgmP = refParam(p[0]);

    Reset();
}
BGMSpell::~BGMSpell() {
	unrefParam(bgmP);
}

void BGMSpell::Reset() {
    bgm = bgmP->AsString();
    frame = 0;

    Spell::Reset();
}

void BGMSpell::Update() {
    if (frame == 0) {
        AS_MP3Stop();
    } else if (frame >= 6) {
        GameInfo* gi = game->GetSelectedGame();
        if (gi) {
            char str[256];
            sprintf(str, "games/%s/bgm/%s", gi->GetId(), bgm);

            //log(str);
            FILE* f = fopen(str, "rb");
            if (!f) {
                sprintf(str, "bgm/%s", bgm);
            } else {
                fclose(f);
            }

            game->soundManager.SetBGM(str);
        }
        finished = true;
    }
    frame++;
}

void BGMSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("bgm", &BGMSpell::ToSpell);
	decl->SetTypesig(0, SPT_string, SPT_null);
	decls[declsL++] = decl;
}
TO_SPELL(BGMSpell)

//==============================================================================

SFXSpell::SFXSpell(Game* g, Param** p, int pL) {
    sfxP = refParam(p[0]);

    Reset();
}
SFXSpell::~SFXSpell() {
	unrefParam(sfxP);
}

void SFXSpell::Reset() {
    sfx = sfxP->AsString();

    Spell::Reset();
}

void SFXSpell::Update() {
	game->soundManager.PlayUserSound(game->level->GetFolder(), sfx);
    finished = true;
}

void SFXSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("sfx", &SFXSpell::ToSpell);
	decl->SetTypesig(0, SPT_string, SPT_null);
	decls[declsL++] = decl;
}
TO_SPELL(SFXSpell)

//==============================================================================

BGControlSpell::BGControlSpell(Game* g, Param** p, int pL) {
    xP = refParam(p[0]);
    yP = refParam(p[1]);

    Reset();
}
BGControlSpell::~BGControlSpell() {
	unrefParam(xP);
	unrefParam(yP);
}

void BGControlSpell::Reset() {
    x = xP->AsFixed();
    y = yP->AsFixed();

    Spell::Reset();
}

void BGControlSpell::Update() {
	game->level->SetBgScrollSpeed(x, y);
	finished = true;
}

void BGControlSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("bg_control", &BGControlSpell::ToSpell);
	decl->SetTypesig(0, SPT_float, SPT_float, SPT_null);
	decls[declsL++] = decl;
}
TO_SPELL(BGControlSpell)

//==============================================================================
